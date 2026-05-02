#pragma once
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cstring>
#include "Buffer.h"

namespace BitPouncePack
{
    struct Hash
    {
        uint64_t d1;
        uint64_t d2;
        uint64_t d3;
        uint64_t d4;

        bool operator==(const Hash& other) const
        {
            return d1 == other.d1 && d2 == other.d2 && d3 == other.d3 && d4 == other.d4;
        }
    };

    Hash sha256(const Buffer& input);
}