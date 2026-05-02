#pragma once

#include <map>
#include <variant>
#include <glm/glm.hpp>
#include <any>
#include <vector>
#include <string>
#include <cstring>
#include <stdexcept>
#include <algorithm>   // for std::find_if, std::any_of, std::find
#include <iterator>

namespace BitPouncePack
{
    // Forward declarations
    class SerializationObject;
    class SerializationObjectArray;

    // ------------------------------------------------------------------------
    // Binary I/O helpers (trivially copyable types)
    // ------------------------------------------------------------------------
    template<typename T>
    void write_binary(std::ostream& os, const T& value)
    {
        os.write(reinterpret_cast<const char*>(&value), sizeof(T));
    }

    template<typename T>
    T read_binary(std::istream& is)
    {
        T value;
        is.read(reinterpret_cast<char*>(&value), sizeof(T));
        if (!is) throw std::runtime_error("Failed to read binary data");
        return value;
    }

    // String serialization
    inline void write_string(std::ostream& os, const std::string& str)
    {
        uint64_t len = str.size();
        write_binary(os, len);
        os.write(str.data(), len);
    }

    inline std::string read_string(std::istream& is)
    {
        uint64_t len = read_binary<uint64_t>(is);
        std::string str(len, '\0');
        is.read(str.data(), len);
        if (!is) throw std::runtime_error("Failed to read string");
        return str;
    }

    // glm vector serialization
    inline void write_vec2(std::ostream& os, const glm::vec2& v)
    {
        write_binary(os, v.x); write_binary(os, v.y);
    }
    inline glm::vec2 read_vec2(std::istream& is)
    {
        glm::vec2 v;
        v.x = read_binary<float>(is); v.y = read_binary<float>(is);
        return v;
    }
    inline void write_vec3(std::ostream& os, const glm::vec3& v)
    {
        write_binary(os, v.x); write_binary(os, v.y); write_binary(os, v.z);
    }
    inline glm::vec3 read_vec3(std::istream& is)
    {
        glm::vec3 v;
        v.x = read_binary<float>(is); v.y = read_binary<float>(is); v.z = read_binary<float>(is);
        return v;
    }
    inline void write_vec4(std::ostream& os, const glm::vec4& v)
    {
        write_binary(os, v.x); write_binary(os, v.y); write_binary(os, v.z); write_binary(os, v.w);
    }
    inline glm::vec4 read_vec4(std::istream& is)
    {
        glm::vec4 v;
        v.x = read_binary<float>(is); v.y = read_binary<float>(is); v.z = read_binary<float>(is); v.w = read_binary<float>(is);
        return v;
    }
    inline void write_vec2i(std::ostream& os, const glm::ivec2& v)
    {
        write_binary(os, v.x); write_binary(os, v.y);
    }
    inline glm::ivec2 read_vec2i(std::istream& is)
    {
        glm::ivec2 v;
        v.x = read_binary<int32_t>(is); v.y = read_binary<int32_t>(is);
        return v;
    }
    inline void write_vec3i(std::ostream& os, const glm::ivec3& v)
    {
        write_binary(os, v.x); write_binary(os, v.y); write_binary(os, v.z);
    }
    inline glm::ivec3 read_vec3i(std::istream& is)
    {
        glm::ivec3 v;
        v.x = read_binary<int32_t>(is); v.y = read_binary<int32_t>(is); v.z = read_binary<int32_t>(is);
        return v;
    }
    inline void write_vec4i(std::ostream& os, const glm::ivec4& v)
    {
        write_binary(os, v.x); write_binary(os, v.y); write_binary(os, v.z); write_binary(os, v.w);
    }
    inline glm::ivec4 read_vec4i(std::istream& is)
    {
        glm::ivec4 v;
        v.x = read_binary<int32_t>(is); v.y = read_binary<int32_t>(is); v.z = read_binary<int32_t>(is); v.w = read_binary<int32_t>(is);
        return v;
    }

    // ------------------------------------------------------------------------
    // SerializationType enum
    // ------------------------------------------------------------------------
    enum class SerializationType : uint8_t
    {
        None = 0,
        Bool = 1,
        Int8 = 2,
        Int16 = 3,
        Int32 = 4,
        Int64 = 5,
        Int = Int32,
        Uint8 = 6,
        Uint16 = 7,
        Uint32 = 8,
        Uint64 = 9,
        Uint = Uint32,
        Float = 10,
        Double = 11,
        String = 12,
        Vector2 = 13,
        Vector3 = 14,
        Vector4 = 15,
        Vector2Int = 16,
        Vector3Int = 17,
        Vector4Int = 18,
        Colour = Vector4,
        SerializationObject = 19,
        SerializationObjectArray = 20
    };

    // ------------------------------------------------------------------------
    // SerializationObject class (declaration)
    // ------------------------------------------------------------------------
    class SerializationObject
    {
    public:
        struct SerializationData
        {
            std::string name;
            SerializationType type;
            std::any value;
        };

        SerializationObject() : m_ID(ID++) {}
        SerializationObject(const SerializationObject& other) = default;
        SerializationObject& operator=(const SerializationObject&) = default;

        bool operator==(const SerializationObject& other) const { return m_ID == other.m_ID; }

        SerializationData Get(const std::string& name) const
        {
            for (const auto& data : m_data)
                if (data.name == name) return data;
            return { "", SerializationType::None, {} };
        }

        const std::vector<SerializationData>& GetAll() const { return m_data; }

        template<typename T>
        void Set(const std::string& name, SerializationType type, T value)
        {
            for (auto& data : m_data)
            {
                if (data.name == name)
                {
                    data.value = value;
                    data.type = type;
                    return;
                }
            }
            m_data.push_back({ name, type, value });
        }

        bool Remove(const std::string& name)
        {
            auto it = std::find_if(m_data.begin(), m_data.end(),
                [&](const SerializationData& d) { return d.name == name; });
            if (it != m_data.end())
            {
                m_data.erase(it);
                return true;
            }
            return false;
        }

        bool Contains(const std::string& name) const
        {
            return std::any_of(m_data.begin(), m_data.end(),
                [&](const SerializationData& d) { return d.name == name; });
        }

        template<typename T>
        T Get(const std::string& name, SerializationType type) const
        {
            for (const auto& data : m_data)
                if (data.name == name && data.type == type)
                    return std::any_cast<T>(data.value);
            return T{};
        }

        // Binary serialization (declaration)
        void Write(std::ostream& os) const;
        void Read(std::istream& is);

    private:
        std::vector<SerializationData> m_data;
        uint64_t m_ID;
        static inline uint64_t ID = 0;

        // Helper methods (to be defined after SerializationObjectArray is complete)
        void write_any(std::ostream& os, SerializationType type, const std::any& value) const;
        std::any read_any(std::istream& is, SerializationType type) const;
    };

    // ------------------------------------------------------------------------
    // SerializationObjectArray class
    // ------------------------------------------------------------------------
    class SerializationObjectArray
    {
    public:
        SerializationObjectArray() = default;
        SerializationObjectArray(const std::vector<SerializationObject>& data) : m_data(data) {}

        std::vector<SerializationObject> GetAll() const { return m_data; }
        std::vector<SerializationObject>& GetAll() { return m_data; }
        void Add(const SerializationObject& obj) { m_data.push_back(obj); }
        void Remove(const SerializationObject& obj)
        {
            auto it = std::find(m_data.begin(), m_data.end(), obj);
            if (it != m_data.end()) m_data.erase(it);
        }
        bool Contains(const SerializationObject& obj) const
        {
            return std::find(m_data.begin(), m_data.end(), obj) != m_data.end();
        }
        size_t Size() const { return m_data.size(); }

        void Write(std::ostream& os) const
        {
            uint64_t count = m_data.size();
            write_binary(os, count);
            for (const auto& obj : m_data)
                obj.Write(os);
        }

        void Read(std::istream& is)
        {
            uint64_t count = read_binary<uint64_t>(is);
            m_data.clear();
            m_data.reserve(count);
            for (uint64_t i = 0; i < count; ++i)
            {
                SerializationObject obj;
                obj.Read(is);
                m_data.push_back(std::move(obj));
            }
        }

    private:
        std::vector<SerializationObject> m_data;
    };

    // ------------------------------------------------------------------------
    // SerializationObject implementation (after SerializationObjectArray is complete)
    // ------------------------------------------------------------------------
    inline void SerializationObject::Write(std::ostream& os) const
    {
        write_binary(os, m_ID);
        uint64_t count = m_data.size();
        write_binary(os, count);
        for (const auto& data : m_data)
        {
            write_string(os, data.name);
            write_binary(os, data.type);
            write_any(os, data.type, data.value);
        }
    }

    inline void SerializationObject::Read(std::istream& is)
    {
        m_ID = read_binary<uint64_t>(is);
        uint64_t count = read_binary<uint64_t>(is);
        m_data.clear();
        for (uint64_t i = 0; i < count; ++i)
        {
            std::string name = read_string(is);
            SerializationType type = read_binary<SerializationType>(is);
            std::any value = read_any(is, type);
            m_data.push_back({ name, type, value });
        }
    }

    inline void SerializationObject::write_any(std::ostream& os, SerializationType type, const std::any& value) const
    {
        switch (type)
        {
            case SerializationType::Bool: write_binary(os, std::any_cast<bool>(value)); break;
            case SerializationType::Int8: write_binary(os, std::any_cast<int8_t>(value)); break;
            case SerializationType::Int16: write_binary(os, std::any_cast<int16_t>(value)); break;
            case SerializationType::Int32: write_binary(os, std::any_cast<int32_t>(value)); break;
            case SerializationType::Int64: write_binary(os, std::any_cast<int64_t>(value)); break;
            case SerializationType::Uint8: write_binary(os, std::any_cast<uint8_t>(value)); break;
            case SerializationType::Uint16: write_binary(os, std::any_cast<uint16_t>(value)); break;
            case SerializationType::Uint32: write_binary(os, std::any_cast<uint32_t>(value)); break;
            case SerializationType::Uint64: write_binary(os, std::any_cast<uint64_t>(value)); break;
            case SerializationType::Float: write_binary(os, std::any_cast<float>(value)); break;
            case SerializationType::Double: write_binary(os, std::any_cast<double>(value)); break;
            case SerializationType::String: write_string(os, std::any_cast<std::string>(value)); break;
            case SerializationType::Vector2: write_vec2(os, std::any_cast<glm::vec2>(value)); break;
            case SerializationType::Vector3: write_vec3(os, std::any_cast<glm::vec3>(value)); break;
            case SerializationType::Vector4: write_vec4(os, std::any_cast<glm::vec4>(value)); break;
            case SerializationType::Vector2Int: write_vec2i(os, std::any_cast<glm::ivec2>(value)); break;
            case SerializationType::Vector3Int: write_vec3i(os, std::any_cast<glm::ivec3>(value)); break;
            case SerializationType::Vector4Int: write_vec4i(os, std::any_cast<glm::ivec4>(value)); break;
            case SerializationType::SerializationObject:
                std::any_cast<SerializationObject>(value).Write(os);
                break;
            case SerializationType::SerializationObjectArray:
                std::any_cast<SerializationObjectArray>(value).Write(os);
                break;
            default: throw std::runtime_error("Unknown SerializationType for writing");
        }
    }

    inline std::any SerializationObject::read_any(std::istream& is, SerializationType type) const
    {
        switch (type)
        {
            case SerializationType::Bool: return read_binary<bool>(is);
            case SerializationType::Int8: return read_binary<int8_t>(is);
            case SerializationType::Int16: return read_binary<int16_t>(is);
            case SerializationType::Int32: return read_binary<int32_t>(is);
            case SerializationType::Int64: return read_binary<int64_t>(is);
            case SerializationType::Uint8: return read_binary<uint8_t>(is);
            case SerializationType::Uint16: return read_binary<uint16_t>(is);
            case SerializationType::Uint32: return read_binary<uint32_t>(is);
            case SerializationType::Uint64: return read_binary<uint64_t>(is);
            case SerializationType::Float: return read_binary<float>(is);
            case SerializationType::Double: return read_binary<double>(is);
            case SerializationType::String: return read_string(is);
            case SerializationType::Vector2: return read_vec2(is);
            case SerializationType::Vector3: return read_vec3(is);
            case SerializationType::Vector4: return read_vec4(is);
            case SerializationType::Vector2Int: return read_vec2i(is);
            case SerializationType::Vector3Int: return read_vec3i(is);
            case SerializationType::Vector4Int: return read_vec4i(is);
            case SerializationType::SerializationObject:
            {
                SerializationObject obj;
                obj.Read(is);
                return obj;
            }
            case SerializationType::SerializationObjectArray:
            {
                SerializationObjectArray arr;
                arr.Read(is);
                return arr;
            }
            default: throw std::runtime_error("Unknown SerializationType for reading");
        }
    }
}