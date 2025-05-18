#include "SpriteMap.h"
#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

bool FileExists(const char* path) {
	const DWORD fileFlags = GetFileAttributesA(path);

	return (fileFlags != INVALID_FILE_SIZE) && !(fileFlags & FILE_ATTRIBUTE_DIRECTORY);
}

std::vector<byte> ReadFont(stbtt_fontinfo& font, const char* filepath) {

    if (FALSE == FileExists(filepath))
        throw std::runtime_error("Unable to find font file");
    
    SmartHandle hFile(CreateFileA(
        filepath,
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    ));

    LARGE_INTEGER size;
    GetFileSizeEx(hFile, &size);
    
    std::vector<byte> buffer(size.LowPart);

    DWORD bytesRead;
    if (FALSE == ReadFile(hFile, buffer.data(), size.LowPart, &bytesRead, nullptr))
        throw std::runtime_error("Failed to read file");
    
    if(!stbtt_InitFont(&font, buffer.data(), NULL))
        throw std::runtime_error("Failed to initialize font");
    
    return buffer;
}

std::vector<SprSpriteInfo> ReadSpr(const char* sprfile) {
    if (!FileExists(sprfile))
        throw std::runtime_error("Provided sprite sheet does not exist");

    SmartHandle hFile(CreateFileA(
        sprfile,
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    ));

    DWORD fileSize;
    GetFileSize(hFile, &fileSize);
    if (INVALID_FILE_SIZE == fileSize)
        throw std::runtime_error("Failed to read filesize");
    
    char path[512];
    char width[10];
    char height[10];
    char alias[512];

    char tempBuff[512];
    ZeroMemory(tempBuff, sizeof(tempBuff));
    int offset = 0;
    // stages:
    // 1. name;
    // 2. width;
    // 3. height;
    // 4. alias;
    int stage = 1;
    DWORD readBytes;

    std::vector<SprSpriteInfo> sprites;

    char ch;
    while (ReadFile(hFile, &ch, 1, &readBytes, nullptr) && readBytes == 1) {
        if (ch != '\n') {
            if (ch == '\r')
                continue;
            else if (offset < sizeof(tempBuff) - 1)
                tempBuff[offset++] = ch;
            continue;
        }
        // We zero memory everything so the byte will always end on a NULL byte.
        tempBuff[offset] = '\0';
        offset = 0;

        switch (stage) {
        case 1: strncpy(path, tempBuff, sizeof(path));      break;
        case 2: strncpy(width, tempBuff, sizeof(width));    break;
        case 3: strncpy(height, tempBuff, sizeof(height));  break;
        case 4: strncpy(alias, tempBuff, sizeof(alias));    break;
        }

        if (stage == 4) {
            char* endPtr;
            int nWidth = strtol(width, &endPtr, 10);
            int nHeight = strtol(height, &endPtr, 10);
            sprites.emplace_back(alias, path, nWidth, nHeight);

            ZeroMemory(path, sizeof(path));
            ZeroMemory(width, sizeof(width));
            ZeroMemory(height, sizeof(height));

            stage = 1;
            continue;
        }
        stage += 1;
    }
    
    return sprites;
}



// TODO: also add truefont types here, considering they are just images
std::unique_ptr<SpriteMap> _SpriteManager::LoadSprites(const char* sprfile, const char* spriteDir, const char* fontfile)
{
    std::vector<SprSpriteInfo> spritesLoadInfo = ReadSpr(sprfile);

    int atlas_width = 1024;
    int atlas_height = 1024;
    int nodes_len = 512;
TRY_AGAIN:

    // Load TTF files
    stbtt_fontinfo font;
    std::vector<byte> ttf_buffer = ReadFont(font, fontfile);

    constexpr int glyphSize = 128;
    float scale = stbtt_ScaleForPixelHeight(&font, glyphSize);
    int width, height, xoffset, yoffset;


    for (uint32_t codepoint = 0; codepoint <= 0x10FFFF; codepoint++) {
        uint32_t glyph_index = stbtt_FindGlyphIndex(&font, codepoint);

        if (NULL == glyph_index)
            continue;

        byte* bitmap = stbtt_GetCodepointBitmap(&font, 0, scale, codepoint, &width, &height, &xoffset, &yoffset);

        if (nullptr == bitmap)
            continue;

        char path[25];
        char glyph[25];

        ZeroMemory(glyph, sizeof(glyph));
        ZeroMemory(path, sizeof(path));

        sprintf(path, "glyph_%d.png", glyph_index);
        sprintf(glyph, "glyph_%d", glyph_index);

        spritesLoadInfo.emplace_back(glyph, path, width, height);

        stbi_write_png(path, width, height, 1, bitmap, width);
        stbtt_FreeBitmap(bitmap, nullptr);
    }

    size_t nodesSize = nodes_len * sizeof(stbrp_node);

    std::vector<stbrp_node> nodes(nodes_len);

    stbrp_context context;
    stbrp_init_target(&context, atlas_width, atlas_height, nodes.data(), nodes_len);

    std::vector<stbrp_rect> rects(spritesLoadInfo.size());

    for (size_t i = 0; i < spritesLoadInfo.size(); i++) {
        const SprSpriteInfo sprite = spritesLoadInfo[i];
        stbrp_rect& rect = rects[i];

        rect.id = i;
        rect.w = sprite.width;
        rect.h = sprite.height;
    }

    stbrp_pack_rects(&context, rects.data(), rects.size());

    size_t buffSize = atlas_width * atlas_height * 4;
    for (const stbrp_rect& rect : rects) {
        if (!rect.was_packed) {
            atlas_height *= 2;
            atlas_width *= 2;
            nodes_len *= 2;

            buffSize = 0;
            goto TRY_AGAIN;
        }
    }
    
    std::unique_ptr<byte[]> textureBuff = std::make_unique<byte[]>(buffSize);

    ReadAllSprites("Resource/Sprites", spritesLoadInfo, rects.data(), textureBuff.get(), buffSize, atlas_width, atlas_height);
    Texture2d textureAtlas = Texture2D::CreateTexture(textureBuff.get(), GL_RGBA8, atlas_width, atlas_height, GL_RGBA, GL_UNSIGNED_BYTE);

    std::vector<SpriteLocation> sprites;
    sprites.reserve(spritesLoadInfo.size());

    for (size_t i = 0; i < spritesLoadInfo.size(); i++) {
        const SprSpriteInfo& spriteInfo = spritesLoadInfo[i];
        const stbrp_rect& rect = rects[i];

        sprites.emplace_back(spriteInfo.name, rect.x, rect.y, rect.w, rect.h);
    }
    
    return std::make_unique<SpriteMap>(textureAtlas, std::move(sprites), atlas_width, atlas_height);
}

SpriteMap::SpriteMap(Texture2d texture, std::vector<SpriteLocation>&& spritesInfo, int width, int height) : 
    texture(texture), spritesInfo(spritesInfo), width(width), height(height)
{
}

SpriteMap::~SpriteMap()
{
    Texture2D::DeleteTexture(&texture);
}

void SpriteMap::Bind(int textureSlot)
{
    int slot;

    switch (textureSlot) {
    case 0:
        slot = GL_TEXTURE0;
        break;
    case 1:
        slot = GL_TEXTURE1;
        break;
    case 2:
        slot = GL_TEXTURE2;
        break;
    default:
        throw std::runtime_error("unknown slot");
    }

    glActiveTexture(slot);
    glBindTextureUnit(textureSlot, texture.handle);
}

const SpriteLocation* SpriteMap::GetSprite(const char* name)
{
    for (size_t i = 0; i < spritesInfo.size(); i++) {
        const SpriteLocation& sprite = spritesInfo[i];

        
        if (strcmp(sprite.name.c_str(), name) == 0)
            return &sprite;
    }

    // Maybe have some kind of missing texture texture?
    return nullptr;
}

inline void Fill(byte* textureAtlas, const SprSpriteInfo& sprite, const stbrp_rect* rect, size_t atlasWidth, byte r, byte g, byte b, byte a) {
    for (size_t y = 0; y < sprite.height; y++) {
        for (size_t x = 0; x < sprite.width; x++) {
            size_t atlasIndex = ((rect->y + y) * atlasWidth + (rect->x + x)) * 4;

            textureAtlas[atlasIndex] = r;       // R
            textureAtlas[atlasIndex + 1] = g;   // G
            textureAtlas[atlasIndex + 2] = b;   // B
            textureAtlas[atlasIndex + 3] = a;   // A
        }
    }
}

void ReadAllSprites(const char* directory, std::vector<SprSpriteInfo>& sprites, void* rects, byte* textureAtlas, size_t size, size_t atlasWidth, size_t atlasHeight)
{
    size_t offset = 0;
    char path[MAX_PATH];

    for (size_t i = 0; i < sprites.size(); i++) {
        const SprSpriteInfo& sprite = sprites[i];
        const stbrp_rect rect = ((stbrp_rect*)rects)[i];

        sprintf(path, "%s/%s", directory, sprite.path .c_str());
        SmartHandle hFile(CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));

        DWORD high;
        DWORD low = GetFileSize(hFile, &high);

        uint64_t size = ((uint64_t)high << 32) | low;

        if (hFile == INVALID_HANDLE_VALUE || size == INVALID_FILE_SIZE) {
            Fill(textureAtlas, sprite, &rect, atlasWidth, 255, 255, 255, 255);
            continue;
        }

        char* fileBuff = new char[size];

        DWORD bytesRead;

        const BOOL result = ReadFile(hFile, fileBuff, size, &bytesRead, nullptr);

        if (FALSE == result) {
            Fill(textureAtlas, sprite, &rect, atlasWidth, 255, 255, 255, 255);
            continue;
        }

        if (bytesRead != size) {
            Fill(textureAtlas, sprite, &rect, atlasWidth, 255, 255, 255, 255);
            continue;
        }

        stbi_set_flip_vertically_on_load(false);
        int originalWidth, originalHeight, originalChannels;
        stbi_uc* pixelData = stbi_load_from_memory((stbi_uc*)fileBuff, size, &originalWidth, &originalHeight, &originalChannels, STBI_rgb_alpha);

        stbi_uc* resizedPixelData = new stbi_uc[sprite.width * sprite.height * 4];
        stbir_resize_uint8(pixelData, originalWidth, originalHeight, NULL, resizedPixelData, sprite.width, sprite.height, NULL, STBI_rgb_alpha);

        for (size_t y = 0; y < sprite.height; y++) {
            for (size_t x = 0; x < sprite.width; x++) {
                size_t atlasIndex = ((rect.y + y) * atlasWidth + (rect.x + x)) * 4;
                size_t spriteIndex = (y * sprite.width + x) * 4;

                textureAtlas[atlasIndex] = resizedPixelData[spriteIndex];         // R
                textureAtlas[atlasIndex + 1] = resizedPixelData[spriteIndex + 1]; // G
                textureAtlas[atlasIndex + 2] = resizedPixelData[spriteIndex + 2]; // B
                textureAtlas[atlasIndex + 3] = resizedPixelData[spriteIndex + 3]; // A
            }
        }

        stbi_image_free(pixelData);
        
        delete[] resizedPixelData;
        delete[] fileBuff;
    }
}
