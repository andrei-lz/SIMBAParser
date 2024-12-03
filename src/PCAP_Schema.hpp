#pragma once

#include <vector>

// Struct for the pcap global header
struct PCAPGlobalHeader
{
    uint32_t magic_number;
    uint16_t version_major;
    uint16_t version_minor;
    int32_t thiszone;
    uint32_t sigfigs;
    uint32_t snaplen;
    uint32_t network;
};
static_assert(sizeof(PCAPGlobalHeader) == 24, "PCAPGlobalHeader size mismatch!");

// Struct for the pcap packet header
struct PCAPPacketHeader
{
    uint32_t ts_sec;   // Timestamp seconds
    uint32_t ts_usec;  // Timestamp microseconds
    uint32_t incl_len; // Number of octets of packet saved in file
    uint32_t orig_len; // Actual length of the packet
};
static_assert(sizeof(PCAPPacketHeader) == 16, "PCAPPacketHeader size mismatch!");

// Struct for the Ethernet header
struct EthernetHeader
{
    uint8_t destination[6]; // Destination MAC address
    uint8_t source[6];      // Source MAC address
    uint16_t etherType;     // Protocol type
};
static_assert(sizeof(EthernetHeader) == 14, "EthernetHeader size mismatch!");

// Struct for the IPv4 header
struct IPv4Header
{
    uint8_t versionAndHeaderLength;  // Version and header length
    uint8_t typeOfService;           // Type of service
    uint16_t totalLength;            // Total length of the datagram
    uint16_t identification;         // Identification
    uint16_t flagsAndFragmentOffset; // Flags and fragment offset
    uint8_t ttl;                     // Time to live
    uint8_t protocol;                // Protocol
    uint16_t headerChecksum;         // Header checksum
    uint32_t sourceAddress;          // Source IP address
    uint32_t destinationAddress;     // Destination IP address
};
static_assert(sizeof(IPv4Header) == 20, "IPv4Header base size mismatch!");

// Struct for the UDP header
struct UDPHeader
{
    uint16_t sourcePort;      // Source port
    uint16_t destinationPort; // Destination port
    uint16_t length;          // Length of UDP header and data
    uint16_t checksum;        // Checksum
};
static_assert(sizeof(UDPHeader) == 8, "UDPHeader size mismatch!");

struct TCPHeader {
    uint16_t srcPort;         // Source Port
    uint16_t destPort;        // Destination Port
    uint32_t seqNumber;       // Sequence Number
    uint32_t ackNumber;       // Acknowledgment Number (if ACK set)
    uint8_t  dataOffset;      // Data offset (4 bits) + Reserved (3 bits) + NS flag (1 bit)
    uint8_t  flags;           // CWR, ECE, URG, ACK, PSH, RST, SYN, FIN (1 bit each)
    uint16_t window;          // Window size
    uint16_t checksum;        // Checksum
    uint16_t urgentPointer;   // Urgent pointer (if URG set)
    // Optional TCP header options follow...
};

// Struct to hold a packet, including its headers and data
struct PCAPPacket
{
    PCAPPacketHeader header;       // Packet header
    EthernetHeader ethernetHeader; // Ethernet header
    IPv4Header ipHeader;           // IPv4 header
    TCPHeader tcpHeader;           // TCP header
    UDPHeader udpHeader;           // UDP header
    std::vector<uint8_t> data;     // Payload (SIMBA data)
};