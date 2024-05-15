#include "LevelLoader.h"
#include "LevelBlock.h"

#include <fstream>
#include <box2d/box2d.h>

using namespace Gameplay;

#pragma pack(push, 1)
struct BMPHeader {
    char signature[2];
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t pixelDataOffset;
    uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compressionMethod;
    uint32_t imageSize;
    int32_t horizontalPixelsPerMeter;
    int32_t verticalPixelsPerMeter;
    uint32_t colorsInPalette;
    uint32_t importantColors;
};
#pragma pack(pop)

LevelLoader::LevelLoader(const std::shared_ptr<b2World>& world)
	: m_world(world)
{
}

LevelLoader::~LevelLoader()
{

}

bool LevelLoader::LoadLevel(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        printf_s("Error opening file: %s\n", filename.c_str());
        return false;
    }

    BMPHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (header.signature[0] != 'B' || header.signature[1] != 'M') {
        printf_s("Not a valid BMP file\n");
        return false;
    }

    if (header.bitsPerPixel != 24) {
        printf_s("Only 24-bit BMP files are supported\n");
        return false;
    }
    file.seekg(header.pixelDataOffset);

    const int padding = (4 - (header.width * 3) % 4) % 4;

    std::vector<std::vector<uint8_t>> rows;
    for (int i = 0; i < header.height; i++) {
        std::vector<uint8_t> pixelData(header.width*3);

        file.read(reinterpret_cast<char*>(pixelData.data()), pixelData.size());
        file.seekg(padding, std::ios_base::cur);

        rows.emplace_back(pixelData);
    } 

    return BuildLevel(rows);
}

std::vector<std::shared_ptr<Asset>> LevelLoader::GetLevelAssets() const 
{
    std::vector<std::shared_ptr<Asset>> assets;
    for (const auto& block : m_blocks)
    {
        const auto& blockAsset = block->GetAsset();
        if (blockAsset != nullptr)
        {
            assets.emplace_back(blockAsset);
        }      
    }
    
    return assets;
}

bool LevelLoader::BuildLevel(std::vector<std::vector<uint8_t>> rows)
{
    for (int row = 0; row < rows.size(); row++) {
        auto cols = rows[row].size() / 3;
        for (int col = 0; col < cols; col++) {
            auto index = col * 3;
            uint8_t red = rows[row][index];
            uint8_t green = rows[row][index +1];
            uint8_t blue = rows[row][index +1];

            if (red + green + blue == 0) {
                CreateStaticBlock(col, row);
            }
        }
    }

    return true;
}

bool LevelLoader::CreateStaticBlock(int x, int y)
{
    m_blocks.emplace_back(std::make_shared<LevelBlock>(m_world.get(), x, y));

    return true;
}
