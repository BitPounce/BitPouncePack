// PackAsset.h
#pragma once
#include <variant>
#include <string>
#include "PackFile.h"
#include "PackTexture.h"
#include "PackScene.h"
#include "PackAudio.h"
#include "PackFont.h"

namespace BitPouncePack 
{
    using PackAsset = std::variant<PackFile, PackTexture, PackScene, PackAudio, PackFont>;
}