#pragma once

// This class is for reading in large files in chunks rather than IO line by line. It's much more efficient
#include <string>

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN // Prevent old winsock.h from messing stuff up
	#define NOMINMAX // Prevent macros for min and max
	#include <windows.h>
#else
	#include <sys/mman.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <unistd.h>
#endif

// RAII Helper for Memory Mapped Chunks
class MemoryMappedChunk {
public:
	MemoryMappedChunk(void* data, size_t size)
		: data(data), size(size) {}

	~MemoryMappedChunk() {
#ifdef _WIN32
		if (data) UnmapViewOfFile(data);
#else
		if (data) munmap(data, size);
#endif
	}

	const char* getData() const { return static_cast<const char*>(data); }

	MemoryMappedChunk(const MemoryMappedChunk&) = delete;
	MemoryMappedChunk& operator=(const MemoryMappedChunk&) = delete;
	MemoryMappedChunk(MemoryMappedChunk&&) = default;
	MemoryMappedChunk& operator=(MemoryMappedChunk&&) = default;

private:
	void* data;
	size_t size;
};

class IOMapper {
public:
	IOMapper(const std::string& filePath, size_t chunkSize = 128 * 1024 * 1024);

	// Fetch the next chunk
	bool fetchNextChunk(const char*& chunkData, size_t& chunkSize);
	
	size_t getChunkSize() const { return chunkSize; }

private:
	std::string filePath;
	size_t chunkSize;
	size_t fileSize;
	size_t currentOffset;

#ifdef _WIN32
	HANDLE fileHandle = INVALID_HANDLE_VALUE;
	HANDLE fileMapping = nullptr;
#else
	int fd = -1;
#endif

	void* mapFile(size_t offset, size_t size);
	size_t getFileSize();
};
