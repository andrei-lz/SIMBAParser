#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "PCAP_Schema.hpp"
#include "IO_Mapper.hpp"

class PCAPParser
{
public:
	PCAPParser(const std::string& inputFilePath, const std::string& outputFilePath);
	~PCAPParser();

	void parse();

	std::vector<char> readPacketData(const PCAPPacketHeader& packetHeader);

	void parsePCAPPacket();
	void processPCAPPayload(const std::vector<char>& packetData);

private:
	
	IOMapper inputMapper;
	const char* chunkDataBuffer = nullptr;
	const char* chunkOffset = nullptr;
	size_t chunkUnprocessedSize = 0;

	std::ofstream outputFile;
	std::ostringstream jsonBuffer; // For performance so we don't have to write every single packet to disk one by one

	PCAPGlobalHeader globalHeader{};

	void readMoreInput();

	void parseGlobalHeader();

	template<typename T>
	T parseGenericHeader();

	IPv4Header parseIPv4Header();
};