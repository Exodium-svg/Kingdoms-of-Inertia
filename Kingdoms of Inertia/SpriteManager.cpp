#include "SpriteManager.h"


bool FileExists(const char* path) {
	DWORD fileFlags = GetFileAttributesA(path);

	return (fileFlags != INVALID_FILE_SIZE) && !(fileFlags & FILE_ATTRIBUTE_DIRECTORY);
}

std::vector<SpriteInfo>  ReadSpr(const char* sprfile) {
    HANDLE hFile = CreateFileA(
        sprfile,
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,          
        OPEN_EXISTING,   
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    DWORD fileSize;
    if (NO_ERROR != GetFileSize(hFile, &fileSize)) {
        CloseHandle(hFile);
        throw std::runtime_error("Failed to read filesize");
    }

    char name[512];
    char width[10];
    char height[10];

    char tempBuff[512];
    ZeroMemory(tempBuff, sizeof(tempBuff));
    int offset = 0;
    // stages:
    // 1. name;
    // 2. width;
    // 4. height;
    int stage = 1;
    DWORD readBytes;

    std::vector<SpriteInfo> sprites;

    char ch;
    while (ReadFile(hFile, &ch, 1, &readBytes, nullptr) && readBytes == 1) {
        if (ch != '\n') {
            if (offset < sizeof(tempBuff) - 1)
                tempBuff[offset++] = ch;
            continue;
        }
        // We zero memory everything so the byte will always end on a NULL byte.
        //tempBuff[offset] = '\0';
        offset = offset ^ offset;

        switch (stage) {
        case 1: strncpy(name, tempBuff, sizeof(name)); break;
        case 2: strncpy(width, tempBuff, sizeof(width)); break;
        case 4: strncpy(height, tempBuff, sizeof(height)); break;
        }

        if (stage == 4) {
            char* endPtr;
            int nWidth = strtol(width, &endPtr, 10);
            int nHeight = strtol(height, &endPtr, 10);
            sprites.emplace_back(name, nWidth, nHeight);

            ZeroMemory(name, sizeof(name));
            ZeroMemory(width, sizeof(width));
            ZeroMemory(height, sizeof(height));

            stage = stage | 0x01;
            continue;
        }
        stage <<= 1;
    }
    
    return sprites;
}

void ReadAllSprites(const char* directory, std::vector<SpriteInfo>& sprites, byte* textureAtlas, size_t size) {
    size_t offset = 0;
    char path[MAX_PATH];

    for (const SpriteInfo& sprite : sprites) {
        ZeroMemory(path, sizeof(path));
        snprintf(path, sizeof(path), "%s/%s", directory, sprite.name.c_str());

        HANDLE hFile = CreateFileA(
        path,
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
        );
        DWORD size;

        if (GetFileSize(hFile, &size) != NO_ERROR) {
            CloseHandle(hFile);
            throw std::runtime_error("Failed to read file");
        }
        char* buff = (char*)malloc(size);

        DWORD bytesRead;
        if (ReadFile(hFile, buff, size, &bytesRead, nullptr) != NO_ERROR) {
            CloseHandle(hFile);
            free(buff);
            throw std::runtime_error("Failed to read file");
        }

        if (bytesRead != size) {
            CloseHandle(hFile);
            free(buff);
            throw std::runtime_error("Failed to read file");
        }

        int originalWidth, originalHeight, originalChannels;
        byte* data = stbi_load_from_memory((stbi_uc*)buff, size, &originalWidth, &originalHeight, &originalChannels, STBI_rgb_alpha);
        size_t resizedSize = sprite.width * sprite.height * 4;

        byte* newBuffer = (byte*)malloc(resizedSize);

        stbir_resize_uint8(data, originalWidth, originalHeight, 0, newBuffer, sprite.width, sprite.height, NULL, originalChannels);
        free(data);

        byte* resizedData = newBuffer;
        memcpy_s(textureAtlas + offset, resizedSize - offset, resizedData, resizedSize);

        offset += resizedSize;
        free(resizedData);
        free(buff);
    }
}

SpriteManager* _SpriteManager::LoadSprites(const char* sprfile, const char* spriteDir)
{
	if (!FileExists(sprfile))
		throw std::runtime_error("Provided sprite sheet does not exist");

    std::vector<SpriteInfo> spritesLoadInfo = ReadSpr(sprfile);

    TRY_AGAIN:
    int atlas_width = 1024;
    int atlas_height = 1024;
    int nodes_len = 512;
    stbrp_node* nodes = (stbrp_node*)malloc(nodes_len * sizeof(stbrp_node));
    stbrp_context context;

    stbrp_init_target(&context, atlas_width, atlas_height, nodes, sizeof(nodes));

    
    std::vector<stbrp_rect> rects(spritesLoadInfo.size());

    for (size_t i = 0; i < spritesLoadInfo.size(); i++) {
        const SpriteInfo sprite = spritesLoadInfo[i];
        stbrp_rect& rect = rects[i];

        rect.id = i;
        rect.w = sprite.width;
        rect.h = sprite.height;
    }

    stbrp_pack_rects(&context, rects.data(), rects.size());
    free(nodes);

    size_t buffSize = 0;
    for (const stbrp_rect& rect : rects) {
        if (!rect.was_packed) {
            atlas_height *= 2;
            atlas_width *= 2;
            nodes_len *= 2;

            buffSize = 0;
            goto TRY_AGAIN;
        }

        buffSize += (rect.h * rect.w);
    }
    buffSize *= 4; // we have 4 bytes per pixel.

    byte* textureBuff = (byte*)malloc(buffSize);
    
    ReadAllSprites("Resource/Sprites", spritesLoadInfo, textureBuff, buffSize);

    Texture2d textureAtlas = Texture2D::CreateTexture(textureBuff, GL_RGBA8, atlas_width, atlas_height, GL_RGBA, GL_UNSIGNED_BYTE);
    free(textureBuff);

    std::vector<Sprite> sprites;
    sprites.reserve(spritesLoadInfo.size());

    for (size_t i = 0; i < spritesLoadInfo.size(); i++) {
        const SpriteInfo& spriteInfo = spritesLoadInfo[i];
        const stbrp_rect& rect = rects[i];

        sprites.emplace_back(spriteInfo.name, rect.x, rect.y, rect.w, rect.h);
    }

    return new SpriteManager(textureAtlas, std::move(sprites), atlas_width, atlas_height);
}

SpriteManager::SpriteManager(Texture2d texture, std::vector<Sprite>&& spritesInfo, int width, int height) : 
    texture(texture), spritesInfo(spritesInfo), width(width), height(height)
{
}

SpriteManager::~SpriteManager()
{
    Texture2D::DeleteTexture(&texture);
}

const Sprite* SpriteManager::GetSprite(char* name)
{
    for (size_t i = 0; i < spritesInfo.size(); i++) {
        const Sprite& sprite = spritesInfo[i];

        if (sprite.name.compare(name))
            return &sprite;
    }

    // Maybe have some kind of missing texture texture?
    return nullptr;
}
