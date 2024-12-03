#pragma once

#include <vector>
#include <span>
#include <variant>

#include "SIMBA_Schema.hpp"

class SIMBADecoder {
/*

2. Create a binary protocol decoder based on the Simba format specification (Moscow exchange moex). The protocol specification can be found at
http://ftp.moex.ru/pub/SIMBA/Spectra/prod/doc/spectra_simba_en.pdf
Focus on decoding messages like OrderUpdate, OrderExecution, and OrderBookSnapshot.

*/
public:

	SIMBADecoder(std::span<const char> packetData);
    ~SIMBADecoder();

	SIMBAPacket decode();

private: //Parser functions
		
	template<typename T>
	inline T parseType(size_t& offset) const noexcept;

	template<typename T>
	inline std::vector<T> parseVectorType(size_t& offset, const GroupSize& numElements) const noexcept;

	OrderBookSnapshot parseOrderBookSnapshot(size_t& offset) const noexcept;
	SecurityDefinition parseSecurityDefinition(size_t& offset) const noexcept;

private:

	std::span<const char> packetData;
};