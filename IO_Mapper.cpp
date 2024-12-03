#include "IO_Mapper.hpp"

#include <algorithm>
#include <stdexcept>

IOMapper::IOMapper(const std::string& filePath, size_t chunkSize)
    : filePath(filePath), chunkSize(chunkSize), currentOffset(0) {
    fileSize = getFileSize();
}

bool IOMapper::fetchNextChunk(const char*& chunkData, size_t& chunkSizeOut) {
    if (currentOffset >= fileSize) {
        return false; // No more chunks to fetch
    }

    size_t remainingSize = fileSize - currentOffset;
    chunkSizeOut = std::min(chunkSize, remainingSize);

    chunkData = static_cast<const char*>(mapFile(currentOffset, chunkSizeOut));
    currentOffset += chunkSizeOut;

    return true;
}

void* IOMapper::mapFile(size_t offset, size_t size) {
#ifdef _WIN32
    if (fileHandle == INVALID_HANDLE_VALUE) {
        fileHandle = CreateFile(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (fileHandle == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Failed to open file.");
        }
        fileMapping = CreateFileMapping(fileHandle, NULL, PAGE_READONLY, 0, 0, NULL);
    }

    void* data = MapViewOfFile(fileMapping, FILE_MAP_READ, static_cast<DWORD>(offset >> 32), static_cast<DWORD>(offset & 0xFFFFFFFF), size);
    if (!data) {
        throw std::runtime_error("Failed to map file.");
    }
    return data;
#else
    if (fd == -1) {
        fd = open(filePath.c_str(), O_RDONLY);
        if (fd < 0) {
            throw std::runtime_error("Failed to open file.");
        }
    }

    void* data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, offset);
    if (data == MAP_FAILED) {
        throw std::runtime_error("Failed to map file.");
    }
    return data;
#endif
}

size_t IOMapper::getFileSize() {
#ifdef _WIN32
    LARGE_INTEGER size;
    HANDLE handle = CreateFile(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to open file.");
    }

    if (!GetFileSizeEx(handle, &size)) {
        CloseHandle(handle);
        throw std::runtime_error("Failed to get file size.");
    }
    CloseHandle(handle);
    return static_cast<size_t>(size.QuadPart);
#else
    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) < 0) {
        throw std::runtime_error("Failed to get file stats.");
    }
    return static_cast<size_t>(fileStat.st_size);
#endif
}

