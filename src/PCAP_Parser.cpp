#include <iostream>
#include <variant>
#include <span>
#include <chrono>
#include <cstring>

#include "PCAP_Parser.hpp"
#include "SIMBA_Decoder.hpp"
#include "SIMBA_JSON.hpp"

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "Ws2_32.lib")
#else
    #include <netinet/in.h>
#endif

#define EXTRA_BUFFER_SPACE 1.2

PCAPParser::PCAPParser(const std::string& inputFilePath, const std::string& outputFilePath)
    : inputMapper(inputFilePath) {
    
    chunkDataBuffer = new char[inputMapper.getChunkSize() * EXTRA_BUFFER_SPACE];
    inputMapper.fetchNextChunk(chunkOffset, chunkUnprocessedSize); // Start reading input

    outputFile.open(outputFilePath, std::ios::out);
    if (!outputFile.is_open()) {
        throw std::runtime_error("Unable to open output file.");
    }

    jsonBuffer << "["; // Start JSON array
}

void PCAPParser::parseGlobalHeader()
{
    if (chunkUnprocessedSize < sizeof(PCAPGlobalHeader))
    {
        std::cerr << "Not enough data in chunk to read global header" << "\n";
        return; // Or handle the error appropriately
    }

    // Read the global header from the chunk
    globalHeader = *reinterpret_cast<const PCAPGlobalHeader*>(chunkOffset);
    // You can now access globalHeader directly, e.g.:
    // std::cout << "Magic number: " << std::hex << globalHeader->magicNumber << std::dec << "\n";

    // Advance the chunkData pointer for subsequent reads
    chunkOffset += sizeof(PCAPGlobalHeader);
    chunkUnprocessedSize -= sizeof(PCAPGlobalHeader);
}

void PCAPParser::parse()
{
    parseGlobalHeader();

    // Read and parse packets until the end of the file
    auto begin = std::chrono::high_resolution_clock::now();
    auto start = begin;

    // for (size_t i = 0; i < 100000; i++)
    size_t i = 1;
    while (chunkUnprocessedSize > 0)
    {
        parsePCAPPacket();
        if (i % 50000 == 0)
        {
            std::cout << i << " packets processed | "
                << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start) << " | "
                << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - begin) << " elapsed" << "\n";
            start = std::chrono::high_resolution_clock::now();
            
            outputFile << jsonBuffer.str();
            jsonBuffer.str("");
            jsonBuffer.clear();
        }
        i++;
    }
}

void PCAPParser::parsePCAPPacket()
{
    PCAPPacketHeader packetHeader = parseGenericHeader<PCAPPacketHeader>();

    if (chunkUnprocessedSize < packetHeader.incl_len)
        readMoreInput();

    std::vector<char> packetData = readPacketData(packetHeader);

    // Process the packet data (example)

    switch (globalHeader.network) {
    case 1: // Ethernet
        processPCAPPayload(packetData);
        break;
    case 105: // IEEE 802.11 Wireless LAN
        break;
    case 101: // Raw IP
        break;
    default:  // Unsupported data link type
        break;
    }
}

// Cross-platform utility function to convert IPv4 address to string
std::string ipv4ToString(uint32_t ipAddress) {
    // Convert from network byte order to host byte order
    ipAddress = ntohl(ipAddress);

    std::ostringstream oss;
    oss << ((ipAddress >> 24) & 0xFF) << "."
        << ((ipAddress >> 16) & 0xFF) << "."
        << ((ipAddress >> 8) & 0xFF) << "."
        << (ipAddress & 0xFF);

    return oss.str();
}

void PCAPParser::processPCAPPayload(const std::vector<char>& packetData)
{
    if (packetData.size() < sizeof(EthernetHeader)) {
        throw std::runtime_error("Packet too short for Ethernet header");
    }

    // Parse Ethernet header
    const EthernetHeader* ethHeader = reinterpret_cast<const EthernetHeader*>(packetData.data());
    if (ntohs(ethHeader->etherType) != 0x0800) {
        return; // Not IPv4
    }

    // Move to the IPv4 header
    const size_t ethernetHeaderLength = sizeof(EthernetHeader);
    if (packetData.size() < ethernetHeaderLength + sizeof(IPv4Header)) {
        throw std::runtime_error("Packet too short for IPv4 header");
    }

    const IPv4Header* ipHeader = reinterpret_cast<const IPv4Header*>(packetData.data() + ethernetHeaderLength);
    const auto ipHeaderLength = sizeof(IPv4Header);

    // Convert source and destination addresses to strings
    std::string srcStr = ipv4ToString(ipHeader->sourceAddress);
    std::string destStr = ipv4ToString(ipHeader->destinationAddress);

    if (ipHeader->protocol == 1)  // ICMP
    {
        std::cout << "ICMP (Internet Control Message Protocol) not implemented\n";
    }
    else if (ipHeader->protocol == 2)  // IGMP
    {
        std::cout << "IGMP (Internet Group Management Protocol) not implemented\n";
    }
    else if (ipHeader->protocol == 6) [[likely]]  // TCP
    {
        auto tcpHeader = reinterpret_cast<const TCPHeader*>(packetData.data() + ipHeaderLength);
        size_t tcpHeaderLength = (tcpHeader->dataOffset >> 4) * 4;

        std::span<const char> payload(
            packetData.data() + ipHeaderLength + tcpHeaderLength,
            packetData.size() - (ipHeaderLength + tcpHeaderLength)
        );

        // Pass the payload on to the SIMBA protocol
        SIMBADecoder decoder(payload);
        auto debug = decoder.decode();
        jsonBuffer << debug;
    }
    else if (ipHeader->protocol == 8)  // EGP
    {
        std::cout << "EGP (Exterior Gateway Protocol) not implemented\n";
    }
    else if (ipHeader->protocol == 17) [[likely]]  // UDP
    {
        auto udpHeader = reinterpret_cast<const UDPHeader*>(packetData.data() + ethernetHeaderLength + ipHeaderLength);
        size_t udpHeaderLength = 8;

        auto destPort = ntohs(udpHeader->destinationPort);
        auto srcPort = ntohs(udpHeader->sourcePort);
        auto length = ntohs(udpHeader->length);
        auto checksum = ntohs(udpHeader->checksum);

        std::span<const char> payload(
            packetData.data() + ethernetHeaderLength + ipHeaderLength + udpHeaderLength,
            ntohs(udpHeader->length) - udpHeaderLength
        );

        // Pass the payload on to the SIMBA protocol
        SIMBADecoder decoder(payload);
        auto debug = decoder.decode();
        jsonBuffer << debug;
    }
    else if (ipHeader->protocol == 41)  // IPv6
    {
        std::cout << "IPv6 (IPv6 encapsulated in IPv4) not implemented\n";
    }
    else if (ipHeader->protocol == 50)  // ESP
    {
        std::cout << "ESP (Encapsulating Security Payload) not implemented\n";
    }
    else if (ipHeader->protocol == 51)  // AH
    {
        std::cout << "AH (Authentication Header) not implemented\n";
    }
    else if (ipHeader->protocol == 58)  // ICMPv6
    {
        std::cout << "ICMPv6 (ICMP for IPv6) not implemented\n";
    }
    else if (ipHeader->protocol == 88)  // EIGRP
    {
        std::cout << "EIGRP (Enhanced Interior Gateway Routing Protocol) not implemented\n";
    }
    else if (ipHeader->protocol == 89)  // OSPF
    {
        std::cout << "OSPF (Open Shortest Path First) not implemented\n";
    }
    else if (ipHeader->protocol == 132)  // SCTP
    {
        std::cout << "SCTP (Stream Control Transmission Protocol) not implemented\n";
    }
    else  // Default case
    {
        std::cout << "Unknown Protocol\n";
    }
    jsonBuffer << ",\n";
}


IPv4Header PCAPParser::parseIPv4Header()
{
    // Use a fixed-size buffer on the stack for maximum speed
    constexpr size_t BASE_HEADER_SIZE = 20; // Standard IPv4 header size (20 bytes)
    constexpr size_t MAX_HEADER_SIZE = 60; // Maximum possible IPv4 header size

    // Ensure there is enough data for the base header
    if (chunkUnprocessedSize < BASE_HEADER_SIZE) {
        throw std::runtime_error("Not enough data in chunk to read IPv4 base header.");
    }

    // Parse the base IPv4 header directly from chunkData
    const char* rawBuffer = chunkOffset; // Pointer to the start of the IPv4 header
    uint8_t ihl = static_cast<uint8_t>(rawBuffer[0]) & 0x0F; // Extract IHL
    size_t ipHeaderSize = ihl * 4;

    // Ensure the header size is valid
    if (ipHeaderSize > MAX_HEADER_SIZE || ipHeaderSize < BASE_HEADER_SIZE) {
        throw std::runtime_error("Invalid IPv4 header size.");
    }

    // Ensure there is enough data for the full header (base + options)
    if (chunkUnprocessedSize < ipHeaderSize) {
        throw std::runtime_error("Not enough data in chunk to read complete IPv4 header.");
    }

    // Advance chunkData and reduce chunkUnprocessedSize
    chunkOffset += ipHeaderSize;
    chunkUnprocessedSize -= ipHeaderSize;

    // Cast the buffer to IPv4Header and return a copy
    return *reinterpret_cast<const IPv4Header*>(rawBuffer);
}

template<typename T>
T PCAPParser::parseGenericHeader()
{
    constexpr size_t PACKET_HEADER_SIZE = sizeof(T);

    // Ensure there is enough data in the chunk
    if (chunkUnprocessedSize < PACKET_HEADER_SIZE) [[unlikely]]
        {
            throw std::runtime_error("Not enough data in chunk to read PCAP packet header.");
        }

            // Read the packet header directly from chunkData
        const char* rawBuffer = chunkOffset; // Pointer to the start of the packet header
        T packetHeader = *reinterpret_cast<const T*>(rawBuffer);

        // Advance the chunkData pointer and reduce chunkUnprocessedSize
        chunkOffset += PACKET_HEADER_SIZE;
        chunkUnprocessedSize -= PACKET_HEADER_SIZE;

        return packetHeader;
}

std::vector<char> PCAPParser::readPacketData(const PCAPPacketHeader& packetHeader) {
    uint32_t capturedLength = packetHeader.incl_len;

    // Ensure there is enough data in the chunk
    if (chunkUnprocessedSize < capturedLength) {
        throw std::runtime_error("Not enough data in chunk to read PCAP packet data.");
    }

    // Read the packet data directly from chunkData
    const char* rawBuffer = chunkOffset; // Pointer to the start of the packet data
    std::vector<char> packetData(rawBuffer, rawBuffer + capturedLength);

    // Advance the chunkData pointer and reduce chunkUnprocessedSize
    chunkOffset += capturedLength;
    chunkUnprocessedSize -= capturedLength;

    return packetData;
}

void PCAPParser::readMoreInput()
{
    size_t remainingSize = chunkUnprocessedSize;

    if (remainingSize > 0)
        std::memmove(const_cast<char*>(chunkDataBuffer), chunkOffset, remainingSize);

    // Update the chunkOffset to point to the start of the buffer
    chunkOffset = chunkDataBuffer;

    // Fetch the next chunk
    const char* newChunkData = nullptr;
    size_t newChunkSize = 0;
    if (!inputMapper.fetchNextChunk(newChunkData, newChunkSize)) {
        throw std::runtime_error("No more data available");
    }
    auto debug = inputMapper.getChunkSize();
    // Ensure new data fits within the preallocated buffer
    if (remainingSize + newChunkSize > inputMapper.getChunkSize() * EXTRA_BUFFER_SPACE) { // bufferSize = chunkSize + EXTRA_BUFFER_SPACE
        throw std::runtime_error("Preallocated buffer is too small");
    }

    // Copy the new chunk into the buffer, after the leftover data
    std::memcpy(const_cast<char*>(chunkDataBuffer) + remainingSize, newChunkData, newChunkSize);

    // Update chunkUnprocessedSize to reflect the new data
    chunkUnprocessedSize = remainingSize + newChunkSize;

    // memcpy memory from current chunkOffset pointer to the remaining chunkchunkOffset + chunkUnprocessedSize all the way to the beginning of the structure (og chunkData pointer)
    // now the old chunk is from chunkData to chunkUnprocessedSize.
    // set chunkOffset to 0
    // fetch next chunk of input which will be written from chunkOffset + chunkUnprocessedSize all the way to the new length
    // the new chunkUnprocessedSize will be current chunkUnprocessedSize + the new incoming size
}

PCAPParser::~PCAPParser()
{
    if (outputFile.is_open())
    {
        outputFile << jsonBuffer.str(); // Make sure there's nothing left in the outputBuffer JSON array is closed properly
        outputFile << "]";  // Make sure the JSON array is closed properly
        outputFile.close(); // File closing in destructor for RAII
    }
}