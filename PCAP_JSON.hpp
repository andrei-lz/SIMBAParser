#include <sstream>
#include <ostream>
#include <iomanip>

#include "PCAP_Schema.hpp"

// PCAP Methods

// Helper function for converting a MAC address to a string
inline std::string macToString(const uint8_t mac[6]) {
    std::ostringstream macStream;
    macStream << std::hex << std::setfill('0');
    for (int i = 0; i < 6; ++i) {
        macStream << std::setw(2) << static_cast<int>(mac[i]);
        if (i < 5) macStream << ":";
    }
    return macStream.str();
}

// PCAPPacketHeader stream operator
inline std::ostream& operator<<(std::ostream& os, const PCAPPacketHeader& header) {
    os << "{"
        << "\"ts_sec\":" << header.ts_sec
        << ",\"ts_usec\":" << header.ts_usec
        << ",\"incl_len\":" << header.incl_len
        << ",\"orig_len\":" << header.orig_len
        << "}";
    return os;
}

// EthernetHeader stream operator
inline std::ostream& operator<<(std::ostream& os, const EthernetHeader& header) {
    os << "{"
        << "\"destination\":\"" << macToString(header.destination) << "\""
        << ",\"source\":\"" << macToString(header.source) << "\""
        << ",\"etherType\":0x" << std::hex << std::setw(4) << std::setfill('0') << header.etherType
        << "}";
    return os;
}

// IPv4Header stream operator
inline std::ostream& operator<<(std::ostream& os, const IPv4Header& header) {
    os << "{"
        << "\"versionAndHeaderLength\":0x" << std::hex << static_cast<int>(header.versionAndHeaderLength)
        << ",\"typeOfService\":0x" << std::hex << static_cast<int>(header.typeOfService)
        << ",\"totalLength\":" << std::dec << header.totalLength
        << ",\"identification\":" << header.identification
        << ",\"flagsAndFragmentOffset\":0x" << std::hex << header.flagsAndFragmentOffset
        << ",\"ttl\":" << std::dec << static_cast<int>(header.ttl)
        << ",\"protocol\":" << static_cast<int>(header.protocol)
        << ",\"headerChecksum\":0x" << std::hex << header.headerChecksum
        << ",\"sourceAddress\":\"" << ((header.sourceAddress >> 24) & 0xFF) << "."
        << ((header.sourceAddress >> 16) & 0xFF) << "."
        << ((header.sourceAddress >> 8) & 0xFF) << "."
        << (header.sourceAddress & 0xFF) << "\""
        << ",\"destinationAddress\":\"" << ((header.destinationAddress >> 24) & 0xFF) << "."
        << ((header.destinationAddress >> 16) & 0xFF) << "."
        << ((header.destinationAddress >> 8) & 0xFF) << "."
        << (header.destinationAddress & 0xFF) << "\""
        << "}";
    return os;
}

// UDPHeader stream operator
inline std::ostream& operator<<(std::ostream& os, const UDPHeader& header) {
    os << "{"
        << "\"sourcePort\":" << header.sourcePort
        << ",\"destinationPort\":" << header.destinationPort
        << ",\"length\":" << header.length
        << ",\"checksum\":0x" << std::hex << header.checksum
        << "}";
    return os;
}

// PCAPPacket stream operator
inline std::ostream& operator<<(std::ostream& os, const PCAPPacket& packet) {
    os << "{"
        << "\"header\":" << packet.header
        << ",\"ethernetHeader\":" << packet.ethernetHeader
        << ",\"ipHeader\":" << packet.ipHeader
        << ",\"udpHeader\":" << packet.udpHeader
        << ",\"data\":\"";
    for (const auto& byte : packet.data) {
        os << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    os << "\""
        << "}";
    return os;
}