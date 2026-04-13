

project "BitPouncePack"
    kind "StaticLib"
    language "C++"
	cppdialect "C++23"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

    files { "src/**cpp", "src/**cpp" }

    includedirs { "src" }

    filter "system:windows"
        staticruntime "On"
		systemversion "latest"
    filter "system:linux"
		cppdialect "C++23"
		systemversion "latest"

    filter "configurations:Debug"
		defines "BPPACK_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "BPPACK_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "BPPACK_DIST"
		optimize "On"