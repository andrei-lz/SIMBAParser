#include "SIMBA_Decoder.hpp"

#include <cstring>
#include <memory>

#include <type_traits>

SIMBADecoder::SIMBADecoder(std::span<const char> packetData)
    : packetData(packetData)
{
}

// Main decode function that processes the entire packet data
SIMBAPacket SIMBADecoder::decode()
{
    size_t offset = 0;

    SIMBAPacket returnPacket;

    // Parse Market Data Packet Header
    MarketDataPacketHeader marketDataPacketHeader = parseType<MarketDataPacketHeader>(offset);
    returnPacket.marketDataHeader = marketDataPacketHeader;

    // If Incremental Packet, parse separately
    if (marketDataPacketHeader.incremental())
    {
        returnPacket.incrementalHeader = parseType<IncrementalPacketHeader>(offset);
    }

    // Until the end of packet data
    while (offset < packetData.size())
    {
        MessageHeader header = parseType<MessageHeader>(offset);

        switch (header.templateId)
        {
            case 15: // OrderUpdate
                returnPacket.messages.emplace_back(parseType<OrderUpdate>(offset));
                break;
            case 16: // OrderExecution
                returnPacket.messages.emplace_back(parseType<OrderExecution>(offset));
                break;
            case 17: // OrderBookSnapshot
                returnPacket.messages.emplace_back(parseOrderBookSnapshot(offset));
                break;
            case 18: // SecurityDefinition
                returnPacket.messages.emplace_back(parseSecurityDefinition(offset));
                break;
            case 10: // SecurityDefinitionUpdateReport
                returnPacket.messages.emplace_back(parseType<SecurityDefinitionUpdateReport>(offset));
                break;
            case 9: // SecurityStatus
                returnPacket.messages.emplace_back(parseType<SecurityStatus>(offset));
                break;
            case 2:// SequenceReset
                returnPacket.messages.emplace_back(parseType<SequenceReset>(offset));
                break; //TradingSessionStatus
            case 11:
                returnPacket.messages.emplace_back(parseType<TradingSessionStatus>(offset));
                break;
            default:
                // Skip unknown messages by advancing the offset by the block
                // length
                offset += header.blockLength;
                break;
        }
        returnPacket.messageHeader = header;
    }
    return returnPacket; //Blank
}

template<typename T>
T SIMBADecoder::parseType(size_t& offset) const noexcept
{
    static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");

    T order{};
    if (offset + sizeof(T) <= packetData.size()) [[likely]]
        std::memcpy(&order, packetData.data() + offset, sizeof(T));

    offset += sizeof(T);
    return order;
}

template<typename T>
std::vector<T> SIMBADecoder::parseVectorType(size_t& offset, const GroupSize& numElements) const noexcept
{
    std::vector<T> entries;
    entries.reserve(numElements.numInGroup);

    for (size_t i = 0; i < numElements.numInGroup; ++i)
        entries.emplace_back(parseType<T>(offset));

    // If it's empty we should still put in the json as empty instead of not having it at all. Down to preference
    return entries;
}

OrderBookSnapshot SIMBADecoder::parseOrderBookSnapshot(size_t& offset) const noexcept
{
    OrderBookSnapshot snapshot{};

    if (offset + OrderBookSnapshot::BASE_SIZE <= packetData.size())
        std::memcpy(&snapshot, packetData.data() + offset, OrderBookSnapshot::BASE_SIZE);

    offset += OrderBookSnapshot::BASE_SIZE;

    snapshot.NoMDEntries = parseType<GroupSize>(offset);

    snapshot.MDEntries = std::make_unique<std::vector<OrderBookSnapshotEntry>>(std::move(parseVectorType<OrderBookSnapshotEntry>(offset, snapshot.NoMDEntries)));

    return snapshot;
}

SecurityDefinition SIMBADecoder::parseSecurityDefinition(size_t& offset) const noexcept
{
    SecurityDefinition def{};

    if (offset + sizeof(def.TotNumReports) <= packetData.size()) {
        def.TotNumReports = parseType<uint32_t>(offset);
    }

    if (offset + sizeof(def.Symbol) <= packetData.size()) {
        std::memcpy(def.Symbol, packetData.data() + offset, sizeof(def.Symbol));
        offset += sizeof(def.Symbol);
    }

    if (offset + sizeof(def.SecurityID) <= packetData.size()) {
        def.SecurityID = parseType<int32_t>(offset);
    }

    // SecurityIDSource is static, no parsing needed

    if (offset + sizeof(def.SecurityAltID) <= packetData.size()) {
        std::memcpy(def.SecurityAltID, packetData.data() + offset, sizeof(def.SecurityAltID));
        offset += sizeof(def.SecurityAltID);
    }

    if (offset + sizeof(def.securityAltIDSource) <= packetData.size()) {
        def.securityAltIDSource = parseType<SecurityAltIDSource>(offset);
    }

    if (offset + sizeof(def.SecurityType) <= packetData.size()) {
        std::memcpy(def.SecurityType, packetData.data() + offset, sizeof(def.SecurityType));
        offset += sizeof(def.SecurityType);
    }

    if (offset + sizeof(def.CFICode) <= packetData.size()) {
        std::memcpy(def.CFICode, packetData.data() + offset, sizeof(def.CFICode));
        offset += sizeof(def.CFICode);
    }

    if (offset + sizeof(def.StrikePrice) <= packetData.size()) {
        def.StrikePrice = parseType<Decimal5NULL>(offset);
    }

    if (offset + sizeof(def.ContractMultiplier) <= packetData.size()) {
        def.ContractMultiplier = parseType<int32_t>(offset);
    }

    if (offset + sizeof(def.securityTradingStatus) <= packetData.size()) {
        def.securityTradingStatus = parseType<SecurityTradingStatus>(offset);
    }

    if (offset + sizeof(def.Currency) <= packetData.size()) {
        std::memcpy(def.Currency, packetData.data() + offset, sizeof(def.Currency));
        offset += sizeof(def.Currency);
    }

    // MarketID is static, no parsing needed

    if (offset + sizeof(def.marketSegmentID) <= packetData.size()) {
        def.marketSegmentID = parseType<MarketSegmentID>(offset);
    }

    if (offset + sizeof(def.tradingSessionID) <= packetData.size()) {
        def.tradingSessionID = parseType<TradingSessionID>(offset);
    }

    if (offset + sizeof(def.ExchangeTradingSessionID) <= packetData.size()) {
        def.ExchangeTradingSessionID = parseType<int32_t>(offset);
    }

    if (offset + sizeof(def.Volatility) <= packetData.size()) {
        def.Volatility = parseType<Decimal5NULL>(offset);
    }
    if (offset + sizeof(def.HighLimitPx) <= packetData.size()) {
        def.HighLimitPx = parseType<Decimal5NULL>(offset);
    }

    if (offset + sizeof(def.LowLimitPx) <= packetData.size()) {
        def.LowLimitPx = parseType<Decimal5NULL>(offset);
    }

    if (offset + sizeof(def.MinPriceIncrement) <= packetData.size()) {
        def.MinPriceIncrement = parseType<Decimal5NULL>(offset);
    }

    if (offset + sizeof(def.MinPriceIncrementAmount) <= packetData.size()) {
        def.MinPriceIncrementAmount = parseType<Decimal5NULL>(offset);
    }

    if (offset + sizeof(def.InitialMarginOnBuy) <= packetData.size()) {
        def.InitialMarginOnBuy = parseType<Decimal2NULL>(offset);
    }

    if (offset + sizeof(def.InitialMarginOnSell) <= packetData.size()) {
        def.InitialMarginOnSell = parseType<Decimal2NULL>(offset);
    }

    if (offset + sizeof(def.InitialMarginSyntetic) <= packetData.size()) {
        def.InitialMarginSyntetic = parseType<Decimal2NULL>(offset);
    }

    if (offset + sizeof(def.TheorPrice) <= packetData.size()) {
        def.TheorPrice = parseType<Decimal5NULL>(offset);
    }

    if (offset + sizeof(def.TheorPriceLimit) <= packetData.size()) {
        def.TheorPriceLimit = parseType<Decimal5NULL>(offset);
    }

    if (offset + sizeof(def.UnderlyingQty) <= packetData.size()) {
        def.UnderlyingQty = parseType<Decimal5NULL>(offset);
    }

    if (offset + sizeof(def.UnderlyingCurrency) <= packetData.size()) {
        std::memcpy(def.UnderlyingCurrency, packetData.data() + offset, sizeof(def.UnderlyingCurrency));
        offset += sizeof(def.UnderlyingCurrency);
    }

    if (offset + sizeof(def.MaturityDate) <= packetData.size()) {
        def.MaturityDate = parseType<uint32_t>(offset);
    }

    if (offset + sizeof(def.MaturityTime) <= packetData.size()) {
        def.MaturityTime = parseType<uint32_t>(offset);
    }

    if (offset + sizeof(def.Flags) <= packetData.size()) {
        def.Flags = parseType<FlagsSet>(offset);
    }

    if (offset + sizeof(def.MinPriceIncrementAmountCurr) <= packetData.size()) {
        def.MinPriceIncrementAmountCurr = parseType<Decimal5NULL>(offset);
    }

    if (offset + sizeof(def.SettlPriceOpen) <= packetData.size()) {
        def.SettlPriceOpen = parseType<Decimal5NULL>(offset);
    }

    if (offset + sizeof(def.ValuationMethod) <= packetData.size()) {
        std::memcpy(def.ValuationMethod, packetData.data() + offset, sizeof(def.ValuationMethod));
        offset += sizeof(def.ValuationMethod);
    }

    if (offset + sizeof(def.RiskFreeRate) <= packetData.size()) {
        def.RiskFreeRate = parseType<DoubleNULL>(offset);
    }

    if (offset + sizeof(def.FixedSpotDiscount) <= packetData.size()) {
        def.FixedSpotDiscount = parseType<DoubleNULL>(offset);
    }

    if (offset + sizeof(def.ProjectedSpotDiscount) <= packetData.size()) {
        def.ProjectedSpotDiscount = parseType<DoubleNULL>(offset);
    }

    if (offset + sizeof(def.SettlCurrency) <= packetData.size()) {
        std::memcpy(def.SettlCurrency, packetData.data() + offset, sizeof(def.SettlCurrency));
        offset += sizeof(def.SettlCurrency);
    }

    if (offset + sizeof(def.negativePrices) <= packetData.size()) {
        def.negativePrices = parseType<NegativePrices>(offset);
    }

    if (offset + sizeof(def.DerivativeContractMultiplier) <= packetData.size()) {
        def.DerivativeContractMultiplier = parseType<int32_t>(offset);
    }

    if (offset + sizeof(def.InterestRateRiskUp) <= packetData.size()) {
        def.InterestRateRiskUp = parseType<DoubleNULL>(offset);
    }

    if (offset + sizeof(def.InterestRateRiskDown) <= packetData.size()) {
        def.InterestRateRiskDown = parseType<DoubleNULL>(offset);
    }

    if (offset + sizeof(def.RiskFreeRate2) <= packetData.size()) {
        def.RiskFreeRate2 = parseType<DoubleNULL>(offset);
    }

    if (offset + sizeof(def.InterestRate2RiskUp) <= packetData.size()) {
        def.InterestRate2RiskUp = parseType<DoubleNULL>(offset);
    }

    if (offset + sizeof(def.InterestRate2RiskDown) <= packetData.size()) {
        def.InterestRate2RiskDown = parseType<DoubleNULL>(offset);
    }

    if (offset + sizeof(def.SettlPrice) <= packetData.size()) {
        def.SettlPrice = parseType<Decimal5NULL>(offset);
    }

    def.NoMDFeedTypes = parseType<GroupSize>(offset);
    def.MDFeedTypesEntries = std::make_unique<std::vector<SecurityDefinition::MDFeedTypes>>(std::move(parseVectorType<SecurityDefinition::MDFeedTypes>(offset, def.NoMDFeedTypes)));

    def.NoUnderlyings = parseType<GroupSize>(offset);
    def.UnderlyingsEntries = std::make_unique<std::vector<SecurityDefinition::Underlyings>>(std::move(parseVectorType<SecurityDefinition::Underlyings>(offset, def.NoUnderlyings)));

    def.NoLegs = parseType<GroupSize>(offset);
    def.LegsEntries = std::make_unique<std::vector<SecurityDefinition::Legs>>(std::move(parseVectorType<SecurityDefinition::Legs>(offset, def.NoMDFeedTypes)));

    def.NoInstrAttrib = parseType<GroupSize>(offset);
    def.InstrAttribEntries = std::make_unique<std::vector<SecurityDefinition::InstrAttrib>>(std::move(parseVectorType<SecurityDefinition::InstrAttrib>(offset, def.NoMDFeedTypes)));

    def.NoEvents = parseType<GroupSize>(offset);
    def.EventsEntries = std::make_unique<std::vector<SecurityDefinition::Events>>(std::move(parseVectorType<SecurityDefinition::Events>(offset, def.NoMDFeedTypes)));

    auto SecurityDescLength = parseType<uint16_t>(offset);
    def.SecurityDesc = parseType<Utf8String>(offset);
    auto QuotationListLength = parseType<uint16_t>(offset);
    def.QuotationList = parseType<VarString>(offset);

    return def;
}

SIMBADecoder::~SIMBADecoder()
{

}