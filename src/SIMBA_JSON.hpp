#pragma once

#include <ostream>
#include <sstream>
#include <variant>
#include <iomanip>

#include <iostream>

#include "SIMBA_Schema.hpp"

// MsgFlagsSet stream operator
std::ostream& operator<<(std::ostream& os, const MsgFlagsSet& entryType)
{
    switch (entryType)
   
{
    case MsgFlagsSet::LastFragment:
        os << "LastFragment";
        break;
    case MsgFlagsSet::StartOfSnapshot:
        os << "StartOfSnapshot";
        break;
    case MsgFlagsSet::EndOfSnapshot:
        os << "EndOfSnapshot";
        break;
    case MsgFlagsSet::IncrementalPacket:
        os << "IncrementalPacket";
        break;
    case MsgFlagsSet::PossDupFlag:
        os << "PossDupFlag";
        break;
    default:
        os << "Unknown MsgFlagsSet";
        break;
    }
    return os;
}

// For any optional type
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const std::optional<T> opt)
{
    if (opt.has_value())
        os << opt.value();
    else
        os << 0;

    return os;
}

// Overload operator<< for std::vector<T>
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
    if (vec.size() < 1)
    {
        os << "[]";
        return os;
    }

    os << "[";
    for (size_t i = 0; i < vec.size(); ++i)
    {
        os << vec[i];
        if (i != vec.size() - 1)
        {
            os << ", "; // Add a comma and space between elements
        }
    }
    os << "]";
    return os;
}

// Overload operator<< for std::span<T>
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::span<T>& span) {
    os << "[";
    for (size_t i = 0; i < span.size(); ++i) {
        os << span[i];
        if (i != span.size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

// Decimal5 stream operator
inline std::ostream& operator<<(std::ostream& os, const Decimal5& decimal)
{
    os << "{\"mantissa\":" << decimal.mantissa << ",\"exponent\":" << static_cast<int>(decimal.exponent) << "}";
    return os;
}

// Decimal5NULL stream operator
inline std::ostream& operator<<(std::ostream& os, const Decimal5NULL& decimal)
{
    os << "{\"mantissa\":";
    if (decimal.mantissa == Decimal5NULL::NULL_VALUE)
        os << "null";
    else
        os << decimal.mantissa;
    os << ",\"exponent\":" << static_cast<int>(decimal.exponent) << "}";
    return os;
}

// DoubleNULL stream operator
inline std::ostream& operator<<(std::ostream& os, const DoubleNULL& dbl)
{
    if (dbl.value == DoubleNULL::NULL_VALUE || std::isnan(dbl.value))
        os << "null";
    else
        os << dbl.value;
    return os;
}

// Decimal2NULL stream operator
inline std::ostream& operator<<(std::ostream& os, const Decimal2NULL& decimal)
{
    os << "{\"mantissa\":";
    if (decimal.mantissa == Decimal5NULL::NULL_VALUE)
        os << "null";
    else
        os << decimal.mantissa;
    os << ",\"exponent\":" << static_cast<int>(decimal.exponent) << "}";
    return os;
}

// Overload operator<< for MDUpdateAction
std::ostream& operator<<(std::ostream& os, const MDUpdateAction& action)
{
    switch (action)
   
{
    case MDUpdateAction::New: os << "\"New\""; break;
    case MDUpdateAction::Change: os << "\"Change\""; break;
    case MDUpdateAction::Delete: os << "\"Delete\""; break;
    default: os << "Unknown MDUpdateAction"; break;
    }
    return os;
}

// Overload operator<< for MDEntryType
std::ostream& operator<<(std::ostream& os, const MDEntryType& entryType)
{
    switch (entryType)
   
{
    case MDEntryType::Bid:
        os << "Bid";
        break;
    case MDEntryType::Offer:
        os << "Offer";
        break;
    case MDEntryType::EmptyBook:
        os << "EmptyBook";
        break;
    default:
        os << "Unknown MDEntryType";
        break;
    }
    return os;
}

// Helper function to map a flag to its string name
std::string FlagsSetToString(const FlagsSet& flag)
{
    switch (flag)
    {
    case FlagsSet::EveningOrMorningSession: return "eveningOrMorningSession";
    case FlagsSet::AnonymousTrading: return "anonymousTrading";
    case FlagsSet::PrivateTrading: return "privateTrading";
    case FlagsSet::DaySession: return "daySession";
    case FlagsSet::MultiLeg: return "multiLeg";
    case FlagsSet::Collateral: return "collateral";
    case FlagsSet::IntradayExercise: return "intradayExercise";
    default: return "";
    }
}

// Overload operator<< for FlagsSet
std::ostream& operator<<(std::ostream& os, const FlagsSet& value)
{
    os << "[";

    bool first = true; // To manage commas between flags
    for (uint64_t bit = 0; bit < 64; ++bit) {
        uint64_t flagBit = 1ULL << bit;

        if (static_cast<uint64_t>(value) & flagBit) {
            // Get the string representation of the flag
            auto status = FlagsSetToString(static_cast<FlagsSet>(flagBit));

            // If the status is not empty, add it to the stream
            if (!status.empty()) {
                if (!first) {
                    os << ", ";
                }
                os << "\"" << status << "\"";
                first = false; // Update to indicate a non-empty item was added
            }
        }
    }

    os << "]";

    return os;
}

std::ostream& operator<<(std::ostream& os, const NegativePrices& value)
{
    switch (value)
{
    case NegativePrices::NotEligible:
        os << "NotEligible";
        break;
    case NegativePrices::Eligible:
        os << "Eligible";
        break;
    default:
        os << "Unknown value";
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const TradingSessionID& sessionID)
{
    switch (sessionID)
{
    case TradingSessionID::Day:
        os << "Day";
        break;
    case TradingSessionID::Morning:
        os << "Morning";
        break;
    case TradingSessionID::Evening:
        os << "Evening";
        break;
    default:
        os << "Null";
        break;
    }
    return os;
}

// Helper function to map a flag to its string name
std::string MDFlagSetToString(const MDFlagsSet& flag)
{
    switch (flag)
   
{
    case MDFlagsSet::Day: return "day";
    case MDFlagsSet::IOC: return "ioc";
    case MDFlagsSet::NonQuote: return "nonQuote";
    case MDFlagsSet::EndOfTransaction: return "endOfTransaction";
    case MDFlagsSet::DueToCrossCancel: return "dueToCrossCancel";
    case MDFlagsSet::SecondLeg: return "secondLeg";
    case MDFlagsSet::FOK: return "fok";
    case MDFlagsSet::Replace: return "replace";
    case MDFlagsSet::Cancel: return "cancel";
    case MDFlagsSet::MassCancel: return "massCancel";
    case MDFlagsSet::Negotiated: return "negotiated";
    case MDFlagsSet::MultiLeg: return "multiLeg";
    case MDFlagsSet::CrossTrade: return "crossTrade";
    case MDFlagsSet::NegotiatedMatchByRef: return "negotiatedMatchByRef";
    case MDFlagsSet::COD: return "cod";
    case MDFlagsSet::ActiveSide: return "activeSide";
    case MDFlagsSet::PassiveSide: return "passiveSide";
    case MDFlagsSet::Synthetic: return "synthetic";
    case MDFlagsSet::RFS: return "rfs";
    case MDFlagsSet::SyntheticPassive: return "syntheticPassive";
    case MDFlagsSet::BOC: return "boc";
    case MDFlagsSet::DuringDiscreteAuction: return "duringDiscreteAuction";
    default: return "unknown";
    }
}

// Overload operator<< for MDFlagsSet
std::ostream& operator<<(std::ostream& os, const MDFlagsSet& value)
{    
    os << "[";

    bool first = true; // To manage commas between flags
    for (uint64_t bit = 0; bit < 64; ++bit)
{

        uint64_t flagBit = 1ULL << bit;
        
        if (static_cast<uint64_t>(value) & flagBit)
{
            if (!first) os << ", ";
            first = false;

            os << "\"" << MDFlagSetToString(static_cast<MDFlagsSet>(flagBit)) << "\"";
        }
    }

    os << "]";
    return os;
}

// Overload operator<< for MDFlags2Set
std::ostream& operator<<(std::ostream& os, const MDFlags2Set& value)
{
    os << "[]"; // Schema does not specify any contents
    return os;
}

std::string SecurityTradingStatusToString(const SecurityTradingStatus& status)
{
    switch (status)
{
    case SecurityTradingStatus::TradingHalt: return "TradingHalt";
    case SecurityTradingStatus::ReadyToTrade: return "ReadyToTrade";
    case SecurityTradingStatus::NotAvailableForTrading: return "NotAvailableForTrading";
    case SecurityTradingStatus::NotTradedOnThisMarket: return "NotTradedOnThisMarket";
    case SecurityTradingStatus::UnknownOrInvalid: return "UnknownOrInvalid";
    case SecurityTradingStatus::PreOpen: return "PreOpen";
    case SecurityTradingStatus::DiscreteAuctionOpen: return "DiscreteAuctionOpen";
    case SecurityTradingStatus::DiscreteAuctionClose: return "DiscreteAuctionClose";
    case SecurityTradingStatus::InstrumentHalt: return "InstrumentHalt";
    default: return "Unknown";
    }
}

std::ostream& operator<<(std::ostream& os, const SecurityTradingStatus& status)
{
    os << SecurityTradingStatusToString(status);
    return os;
}


// MessageHeader stream operator
inline std::ostream& operator<<(std::ostream& os, const MessageHeader& header)
{
    os << "{\"blockLength\":" << header.blockLength
        << ",\"templateId\":" << header.templateId
        << ",\"schemaId\":" << header.schemaId
        << ",\"version\":" << header.version << "}";
    return os;
}

// MarketDataPacketHeader stream operator
inline std::ostream& operator<<(std::ostream& os, const MarketDataPacketHeader& packetHeader)
{
    os << "{\"MsgSeqNum\":" << packetHeader.MsgSeqNum
        << ",\"MsgSize\":" << packetHeader.MsgSize
        << ",\"MsgFlags\":" << packetHeader.MsgFlags
        << ",\"SendingTime\":" << packetHeader.SendingTime << "}";
    return os;
}

// SecurityAltIDSource stream operator
std::ostream& operator<<(std::ostream& os, const SecurityAltIDSource& entryType)
{
    switch (entryType)
   
{
    case SecurityAltIDSource::ISIN:
        os << "ISIN";
        break;
    case SecurityAltIDSource::ExchangeSymbol:
        os << "ExchangeSymbol";
        break;
    default:
        os << "Unknown SecurityAltIDSource";
        break;
    }
    return os;
}

// MarketSegmentID stream operator
std::ostream& operator<<(std::ostream& os, const MarketSegmentID& entryType)
{
    switch (entryType)
   
{
    case MarketSegmentID::Derivatives:
        os << "Derivatives";
        break;
    default:
        os << "Unknown MarketSegmentID";
        break;
    }
    return os;
}

// IncrementalPacketHeader stream operator
inline std::ostream& operator<<(std::ostream& os, const IncrementalPacketHeader& packetHeader)
{
    os << "{\"TransactTime\":" << packetHeader.TransactTime
        << ",\"ExchangeTradingSessionID\":" << packetHeader.ExchangeTradingSessionID << "}";
    return os;
}

// OrderUpdate stream operator
inline std::ostream& operator<<(std::ostream& os, const OrderUpdate& update)
{
    os << "{\"Name\":\"OrderUpdate\","
        << "\"MDEntryID\":" << update.MDEntryID
        << ",\"MDEntryPx\":" << update.MDEntryPx
        << ",\"MDEntrySize\":" << update.MDEntrySize
        << ",\"MDFlags\":" << update.MDFlags
        << ",\"MDFlags2\":" << update.MDFlags2
        << ",\"SecurityID\":" << update.SecurityID
        << ",\"RptSeq\":" << update.RptSeq
        << ",\"MDUpdateAction\":" << update.MDUpdateAction
        << ",\"MDEntryType\":\"" << update.MDEntryType << "\"}";
    return os;
}

// OrderExecution stream operator
inline std::ostream& operator<<(std::ostream& os, const OrderExecution& execution)
{
    os << "{\"Name\":\"OrderExecution\","
        << "\"MDEntryID\":" << execution.MDEntryID
        << ",\"MDEntryPx\":" << execution.MDEntryPx
        << ",\"MDEntrySize\":" << execution.MDEntrySize
        << ",\"LastPx\":" << execution.LastPx
        << ",\"LastQty\":" << execution.LastQty
        << ",\"TradeID\":" << execution.TradeID
        << ",\"MDFlags\":" << execution.MDFlags
        << ",\"MDFlags2\":" << execution.MDFlags2
        << ",\"SecurityID\":" << execution.SecurityID
        << ",\"RptSeq\":" << execution.RptSeq
        << ",\"MDUpdateAction\":" << execution.MDUpdateAction
        << ",\"MDEntryType\":\"" << execution.MDEntryType << "\"}";
    return os;
}

// OrderBookSnapshot stream operator
inline std::ostream& operator<<(std::ostream& os, const OrderBookSnapshot& snapshot)
{
    os << "{\"Name\":\"OrderBookSnapshot\","
        << "\"SecurityID\":" << snapshot.SecurityID
        << ",\"LastMsgSeqNumProcessed\":" << snapshot.LastMsgSeqNumProcessed
        << ",\"RptSeq\":" << snapshot.RptSeq
        << ",\"ExchangeTradingSessionID\":" << snapshot.ExchangeTradingSessionID
        << ",\"NoMDEntries\":" << *(snapshot.MDEntries); // Using the vector << overload
    os << "}";
    return os;
}

// OrderBookSnapshotEntry stream operator
inline std::ostream& operator<<(std::ostream& os, const OrderBookSnapshotEntry& snapshotEntry)
{
    os << "{\"MDEntryID\":" << snapshotEntry.MDEntryID
        << ",\"TransactTime\":" << snapshotEntry.TransactTime
        << ",\"MDEntryPx\":" << snapshotEntry.MDEntryPx
        << ",\"MDEntrySize\":" << snapshotEntry.MDEntrySize
        << ",\"TradeID\":" << snapshotEntry.TradeID
        << ",\"MDFlags\":" << snapshotEntry.MDFlags
        << ",\"MDFlags2\":" << snapshotEntry.MDFlags2
        << ",\"MDEntryType\":\"" << snapshotEntry.MDEntryType << "\"}";
    return os;
}
// Overload for Utf8String
std::ostream& operator<<(std::ostream& os, const Utf8String& str) {
    if (!str.empty())
    {
        std::ostringstream escaped;
        escaped << "\"";
        for (size_t i = 0; i < str.length; ++i) {
            unsigned char c = str.varData[i];
            switch (c) {
            case '"':  escaped << "\\\""; break;
            case '\\': escaped << "\\\\"; break;
            case '\b': escaped << "\\b"; break;
            case '\f': escaped << "\\f"; break;
            case '\n': escaped << "\\n"; break;
            case '\r': escaped << "\\r"; break;
            case '\t': escaped << "\\t"; break;
            default:
                if (c < 0x20 || c > 0x7E) {
                    // Encode non-ASCII characters as Unicode escape sequences
                    escaped << "\\u" << std::hex << std::uppercase
                        << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                }
                else {
                    escaped << c;
                }
            }
        }
        escaped << "\"";
        os << escaped.str();
    }
    else
    {
        os << "null";
    }
    return os;
}

// Overload for VarString
std::ostream& operator<<(std::ostream& os, const VarString& str) {
    if (!str.empty() && str.data() != nullptr) {
        std::string trimmedDesc(reinterpret_cast<const char*>(str.data()),
            strnlen(reinterpret_cast<const char*>(str.data()), str.size()));
        os << "\"" << trimmedDesc << "\"";
    }
    else {
        os << "null";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const SecurityDefinition::MDFeedTypes& entry) {
    os << "{\"MDFeedType\":\"" << entry.MDFeedType
        << "\",\"MarketDepth\":" << entry.MarketDepth
        << ",\"MDBookType\":" << entry.MDBookType << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const SecurityDefinition::Underlyings& entry) {
    os << "{\"UnderlyingSymbol\":\"" << entry.UnderlyingSymbol
        << "\",\"UnderlyingBoard\":\"" << entry.UnderlyingBoard
        << "\",\"UnderlyingSecurityID\":" << entry.UnderlyingSecurityID
        << ",\"UnderlyingFutureID\":" << entry.UnderlyingFutureID << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const SecurityDefinition::Legs& entry) {
    os << "{\"LegSymbol\":\"" << entry.LegSymbol
        << "\",\"LegSecurityID\":" << entry.LegSecurityID
        << ",\"LegRatioQty\":" << entry.LegRatioQty << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const SecurityDefinition::InstrAttrib& entry) {
    os << "{\"InstrAttribType\":" << entry.InstrAttribType
        << ",\"InstrAttribValue\":\"" << entry.InstrAttribValue << "\"}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const SecurityDefinition::Events& entry) {
    os << "{\"EventType\":" << entry.EventType
        << ",\"EventDate\":" << entry.EventDate
        << ",\"EventTime\":" << entry.EventTime << "}";
    return os;
}

// SecurityDefinition stream operator
std::ostream& operator<<(std::ostream& os, const SecurityDefinition& def)
{
    os << "{\"Name\": \"SecurityDefinition\", ";
    os << "\"TotNumReports\": " << def.TotNumReports << ", ";
    os << "\"Symbol\": \"" << std::string(def.Symbol, strnlen(def.Symbol, sizeof(def.Symbol))) << "\", ";
    os << "\"SecurityID\": " << def.SecurityID << ", ";
    os << "\"SecurityIDSource\": " << static_cast<char>(def.SecurityIDSource) << ", ";
    os << "\"SecurityAltID\": \"" << std::string(def.SecurityAltID) << "\", ";
    os << "\"SecurityAltIDSource\": \"" << def.SecurityAltIDSource << "\", ";
    os << "\"SecurityType\": \"" << std::string(def.SecurityType) << "\", ";
    os << "\"CFICode\": \"" << std::string(def.CFICode, 6) << "\", ";
    os << "\"StrikePrice\": " << def.StrikePrice << ", ";
    os << "\"ContractMultiplier\": " << def.ContractMultiplier << ", ";
    os << "\"SecurityTradingStatus\": \"" << def.SecurityTradingStatus << "\", ";
    os << "\"Currency\": \"" << std::string(def.Currency, strnlen(def.Currency, sizeof(def.Currency))) << "\", ";
    os << "\"MarketID\": \"" << std::string(SecurityDefinition::MarketID, 4) << "\", ";
    os << "\"MarketSegmentID\": \"" << def.MarketSegmentID << "\", ";
    os << "\"TradingSessionID\": \"" << def.TradingSessionID << "\", ";
    os << "\"ExchangeTradingSessionID\": " << def.ExchangeTradingSessionID << ", ";
    os << "\"Volatility\": " << def.Volatility << ", ";
    os << "\"HighLimitPx\": " << def.HighLimitPx << ", ";
    os << "\"LowLimitPx\": " << def.LowLimitPx << ", ";
    os << "\"MinPriceIncrement\": " << def.MinPriceIncrement << ", ";
    os << "\"MinPriceIncrementAmount\": " << def.MinPriceIncrementAmount << ", ";
    os << "\"InitialMarginOnBuy\": " << def.InitialMarginOnBuy << ", ";
    os << "\"InitialMarginOnSell\": " << def.InitialMarginOnSell << ", ";
    os << "\"InitialMarginSyntetic\": " << def.InitialMarginSyntetic << ", ";
    os << "\"TheorPrice\": " << def.TheorPrice << ", ";
    os << "\"TheorPriceLimit\": " << def.TheorPriceLimit << ", ";
    os << "\"UnderlyingQty\": " << def.UnderlyingQty << ", ";
    os << "\"UnderlyingCurrency\": \"" << std::string(def.UnderlyingCurrency) << "\", ";
    os << "\"MaturityDate\": " << def.MaturityDate << ", ";
    os << "\"MaturityTime\": " << def.MaturityTime << ", ";
    os << "\"Flags\": " << def.Flags << ", ";
    os << "\"MinPriceIncrementAmountCurr\": " << def.MinPriceIncrementAmountCurr << ", ";
    os << "\"SettlPriceOpen\": " << def.SettlPriceOpen << ", ";
    os << "\"ValuationMethod\": \"" << std::string(def.ValuationMethod) << "\", ";
    os << "\"RiskFreeRate\": " << def.RiskFreeRate << ", ";
    os << "\"FixedSpotDiscount\": " << def.FixedSpotDiscount << ", ";
    os << "\"ProjectedSpotDiscount\": " << def.ProjectedSpotDiscount << ", ";
    os << "\"SettlCurrency\": \"" << std::string(def.SettlCurrency, strnlen(def.SettlCurrency, sizeof(def.SettlCurrency))) << "\", ";
    os << "\"NegativePrices\": \"" << def.NegativePrices << "\", ";
    os << "\"DerivativeContractMultiplier\": " << def.DerivativeContractMultiplier << ", ";
    os << "\"InterestRateRiskUp\": " << def.InterestRateRiskUp << ", ";
    os << "\"InterestRateRiskDown\": " << def.InterestRateRiskDown << ", ";
    os << "\"RiskFreeRate2\": " << def.RiskFreeRate2 << ", ";
    os << "\"InterestRate2RiskUp\": " << def.InterestRate2RiskUp << ", ";
    os << "\"InterestRate2RiskDown\": " << def.InterestRate2RiskDown << ", ";
    os << "\"SettlPrice\": " << def.SettlPrice << ", ";
    os << "\"NoMDFeedTypes\":" << std::span<SecurityDefinition::MDFeedTypes>((*def.MDFeedTypesEntries).data(), def.NoMDFeedTypes.numInGroup) << ", ";
    os << "\"NoUnderlyings\":" << std::span<SecurityDefinition::Underlyings>((*def.UnderlyingsEntries).data(), def.NoUnderlyings.numInGroup) << ", ";
    os << "\"NoLegs\":" << std::span<SecurityDefinition::Legs>((*def.LegsEntries).data(), def.NoLegs.numInGroup) << ", ";
    os << "\"NoInstrAttrib\":" << std::span<SecurityDefinition::InstrAttrib>((*def.InstrAttribEntries).data(), def.NoInstrAttrib.numInGroup) << ", ";
    os << "\"NoEvents\":" << std::span<SecurityDefinition::Events>((*def.EventsEntries).data(), def.NoEvents.numInGroup) << ", ";
    os << "\"SecurityDesc\":" << (def.SecurityDesc.empty()
        ? "null"
        : std::string(reinterpret_cast<const char*>(def.SecurityDesc.data()), def.SecurityDesc.size())); // Temporary solution, need wide stream to properly support UTF8
    os << ", ";
    os << "\"QuotationList\":" << def.QuotationList << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const SequenceReset& entry) {
    os << "{\"SequenceReset\":" << entry.NewSeqNo << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const SecurityStatus& status)
{
    os << "{ \"Name\": \"SecurityStatus\", ";
    os << "SecurityID: " << status.SecurityID << ", ";
    os << "SecurityIDSource: " << status.SecurityIDSource << ", ";
    os << "Symbol: " << std::string(status.Symbol, 25) << ", ";
    os << "SecurityTradingStatus: " << status.SecurityTradingStatus << ", ";
    os << "HighLimitPx: " << status.HighLimitPx << ", ";
    os << "LowLimitPx: " << status.LowLimitPx << ", ";
    os << "InitialMarginOnBuy: " << status.InitialMarginOnBuy << ", ";
    os << "InitialMarginOnSell: " << status.InitialMarginOnSell << ", ";
    os << "InitialMarginSyntetic: " << status.InitialMarginSyntetic;
    os << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const SecurityDefinitionUpdateReport& report)
{
    os << "{ ";
    os << "\"SecurityID\": " << report.SecurityID << ", ";
    os << "\"SecurityIDSource\": \"" << report.SecurityIDSource << "\", ";
    os << "\"Volatility\": " << report.Volatility << ", ";
    os << "\"TheorPrice\": " << report.TheorPrice << ", ";
    os << "\"TheorPriceLimit\": " << report.TheorPriceLimit;
    os << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const TradingSessionStatus& status) {
    os << "{ \"Name\": \"TradingSessionStatus\", ";
    os << "\"TradSesOpenTime\": " << status.TradSesOpenTime << ", ";
    os << "\"TradSesCloseTime\": " << status.TradSesCloseTime << ", ";
    if (status.TradSesIntermClearingStartTime != 0) {
        os << "\"TradSesIntermClearingStartTime\": " << status.TradSesIntermClearingStartTime << ", ";
    }
    else {
        os << "\"TradSesIntermClearingStartTime\": null, ";
    }
    if (status.TradSesIntermClearingEndTime != 0) {
        os << "\"TradSesIntermClearingEndTime\": " << status.TradSesIntermClearingEndTime << ", ";
    }
    else {
        os << "\"TradSesIntermClearingEndTime\": null, ";
    }
    os << "\"TradingSessionID\": " << static_cast<int>(status.TradingSessionID) << ", ";
    if (status.ExchangeTradingSessionID != 0) {
        os << "\"ExchangeTradingSessionID\": " << status.ExchangeTradingSessionID << ", ";
    }
    else {
        os << "\"ExchangeTradingSessionID\": null, ";
    }
    os << "\"TradSesStatus\": " << static_cast<int>(status.TradSesStatus) << ", ";
    os << "\"MarketID\": \"" << std::string(status.MarketID, 4) << "\", ";
    os << "\"MarketSegmentID\": \"" << status.MarketSegmentID << "\", ";
    os << "\"TradSesEvent\": " << static_cast<int>(status.TradSesEvent);
    os << "}";

    return os;
}

std::ostream& operator<<(std::ostream& os, const SIMBAPacket& packet)
{
    os << packet.marketDataHeader << ", ";
    if (packet.incrementalHeader)
{
        os << *packet.incrementalHeader << ", ";
    }
    os << packet.messageHeader << ", ";
    os << packet.messages;
    return os;
}

// Define the operator<< for the std::variant
template<typename... Types>
std::ostream& operator<<(std::ostream& os, const std::variant<Types...>& var)
{
    // Check that all types in the variant are streamable
    static_assert((std::is_same_v<std::ostream&, decltype(os << std::declval<Types>())> && ...),
                   "All types in std::variant must be streamable with operator<<");
    
    std::visit([&os](const auto& value)
{ os << value; }, var);
    return os;
}