#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <limits>

#pragma pack(push, 1)

// MarketID
static constexpr char MarketID[4] = { 'M', 'O', 'E', 'X' }; // Constant value "MOEX"

struct Utf8String {
    uint16_t length;         // Length of the string
    uint8_t* varData;        // Pointer to UTF-8 data

    // Provide data() and size() methods
    const uint8_t* data() const { return varData; }
    size_t size() const { return length; }
    bool empty() const { return length == 0 || varData == nullptr; }
};

struct VarString {
    uint16_t length;         // Length of the string
    uint8_t* varData;        // Pointer to ASCII data

    // Provide data() and size() methods
    const uint8_t* data() const { return varData; }
    size_t size() const { return length; }
    bool empty() const { return length == 0 || varData == nullptr; }
};

enum class MsgFlagsSet : uint16_t {
    LastFragment = 1ULL << 0,       // Message fragmentation flag
    StartOfSnapshot = 1ULL << 1,    // First message in the snapshot for the instrument
    EndOfSnapshot = 1ULL << 2,      // Last message in the snapshot for the instrument
    IncrementalPacket = 1ULL << 3,  // Incremental packet flag
    PossDupFlag = 1ULL << 4         // Order book retransmission in the incremental stream
};

// Market Data Packet Header
struct MarketDataPacketHeader
{
    uint32_t MsgSeqNum;   // Message sequence number
    uint16_t MsgSize;     // Message size includes size of Market Data Packet Header
    uint16_t MsgFlags;    // Message fragmentation and other flags
    uint64_t SendingTime; // Sending time in number of nanoseconds since Unix epoch, UTC timezone

    bool incremental() const noexcept { return MsgFlags & 0x8; }
};
static_assert(sizeof(MarketDataPacketHeader) == 16, "MarketDataPacketHeader size is incorrect");

// Incremental Packet Header
struct IncrementalPacketHeader
{
    uint64_t TransactTime;                // Start time of event processing (nanoseconds since Unix epoch, UTC)
    uint32_t ExchangeTradingSessionID;    // ID of the trading session (optional field
};
static_assert(sizeof(IncrementalPacketHeader) == 12, "IncrementalPacketHeader size is incorrect");


// Message Header
struct MessageHeader
{
    uint16_t blockLength; // Length of the message block
    uint16_t templateId;  // ID of the template defining the message structure
    uint16_t schemaId;    // ID of the schema being used
    uint16_t version;     // Version of the schema
};
static_assert(sizeof(MessageHeader) == 8, "MessageHeader size is incorrect");

// Group Size
struct GroupSize
{
    uint16_t blockLength; // Length of each group entry
    uint8_t numInGroup;   // Number of entries in the group
};
static_assert(sizeof(GroupSize) == 3, "GroupSize size is incorrect");

// Group Size 2
struct GroupSize2
{
    uint16_t blockLength; // Length of each group entry
    uint16_t numInGroup;  // Number of entries in the group
};
static_assert(sizeof(GroupSize2) == 4, "GroupSize2 size is incorrect");

// Decimal5
struct Decimal5 {
    int64_t mantissa;       // Mantissa of the price value
    static constexpr int8_t exponent = -5; // Fixed exponent, representing a scale of 10^-5

    Decimal5() = default;
    explicit Decimal5(int64_t mantissaValue) : mantissa(mantissaValue) {}
};

// Decimal5NULL
struct Decimal5NULL {
    int64_t mantissa;       // Mantissa of the price value (optional presence)
    static constexpr int8_t exponent = -5; // Fixed exponent, representing a scale of 10^-5

    static constexpr int64_t MAX_VALUE = 9223372036854775806;
    static constexpr int64_t NULL_VALUE = 9223372036854775807;

    Decimal5NULL() = default;
    explicit Decimal5NULL(int64_t mantissaValue) : mantissa(mantissaValue) {}
};

// Decimal5NULL
struct DoubleNULL {
    double value;       // Mantissa of the price value (optional presence)

    static constexpr double MAX_VALUE = std::numeric_limits<double>::max();
    static constexpr double NULL_VALUE = std::numeric_limits<double>::quiet_NaN();

    DoubleNULL() : value(NULL_VALUE) {};
    explicit DoubleNULL(double value) : value(value) {}
};

// Decimal2NULL
struct Decimal2NULL {
    int64_t mantissa;       // Mantissa of the price value (optional presence)
    static constexpr int8_t exponent = -2; // Fixed exponent, representing a scale of 10^-2

    static constexpr int64_t MAX_VALUE = 9223372036854775806;
    static constexpr int64_t NULL_VALUE = 9223372036854775807;

    Decimal2NULL() = default;
    explicit Decimal2NULL(int64_t mantissaValue) : mantissa(mantissaValue) {}
};

// SecurityIDSource
static constexpr char SECURITY_ID_SOURCE = '8'; // Constant value '8'

enum class MDUpdateAction : uint8_t
{
    New = 0,      // New action
    Change = 1,   // Change action
    Delete = 2    // Delete action
};

// Enum: MDEntryType
enum class MDEntryType : char
{
    Bid = '0',         // Bid entry
    Offer = '1',       // Offer entry
    EmptyBook = 'J'    // Empty Book entry
};

// Enum: SecurityAltIDSource
enum class SecurityAltIDSource : char
{
    ISIN = '4',                // ISIN identifier
    ExchangeSymbol = '8'       // Exchange symbol
};

// Enum: SecurityTradingStatus
enum class SecurityTradingStatus : uint8_t
{
    TradingHalt = 2,                   // Trading halt
    ReadyToTrade = 17,                 // Ready to trade
    NotAvailableForTrading = 18,       // Not available for trading
    NotTradedOnThisMarket = 19,        // Not traded on this market
    UnknownOrInvalid = 20,             // Unknown or Invalid status
    PreOpen = 21,                      // Pre-open
    DiscreteAuctionOpen = 119,         // Discrete auction started
    DiscreteAuctionClose = 121,        // Discrete auction ended
    InstrumentHalt = 122,              // Instrument halt
};

// Enum: TradingSessionID
enum class TradingSessionID : uint8_t
{
    Null = 0,
    Day = 1,       // Day session
    Morning = 3,   // Morning session
    Evening = 5    // Evening session
};

// Enum: MarketSegmentID
enum class MarketSegmentID : char
{
    Derivatives = 'D' // Derivatives segment
};

// Enum: TradSesStatus
enum class TradSesStatus : uint8_t
{
    Halted = 1,     // Session paused
    Open = 2,       // Session started
    Closed = 3,     // Session ended
    PreOpen = 4     // Session initiated
};

// Enum: TradSesEvent
enum class TradSesEvent : uint8_t
{
    TradingResumes = 0,           // Trading resumed after intraday clearing session
    ChangeOfTradingSession = 1,   // Start and end of trading session
    ChangeOfTradingStatus = 3     // Trading session status change
};

// Enum: NegativePrices
enum class NegativePrices : uint8_t
{
    NotEligible = 0,  // Futures prices, price limits, and options strikes are limited to positive only
    Eligible = 1      // Futures prices and options strikes are not limited
};

// MDFlagsSet
enum class MDFlagsSet : uint64_t
{
    Day = 1ULL << 0,                  // Orders and Trades: Day order
    IOC = 1ULL << 1,                  // Orders and Trades: IOC order
    NonQuote = 1ULL << 2,             // Orders and Trades: Non quote entry
    EndOfTransaction = 1ULL << 12,    // Orders and Trades: The end of matching transaction
    DueToCrossCancel = 1ULL << 13,    // Orders: The record results from cancelling the order due to cross
    SecondLeg = 1ULL << 14,           // Trades: Second leg of multileg trade
    FOK = 1ULL << 19,                 // Orders: FOK order
    Replace = 1ULL << 20,             // Orders: The record results from replacing the order
    Cancel = 1ULL << 21,              // Orders: The record results from cancelling the order
    MassCancel = 1ULL << 22,          // Orders: The record results from mass cancelling
    Negotiated = 1ULL << 26,          // Trades: Negotiated trade
    MultiLeg = 1ULL << 27,            // Trades: Multileg trade
    CrossTrade = 1ULL << 29,          // Orders: Flag of cancelling the left balance of the order because of a cross-trade
    NegotiatedMatchByRef = 1ULL << 31,// Orders and Trades: Negotiated order or trade matched by reference
    COD = 1ULL << 32,                 // Orders: The record results from cancelling an order via 'Cancel on Disconnect' service
    ActiveSide = 1ULL << 41,          // Trades: Flag of aggressive side
    PassiveSide = 1ULL << 42,         // Trades: Flag of passive side
    Synthetic = 1ULL << 45,           // Orders and Trades: Flag of the synthetic order
    RFS = 1ULL << 46,                 // Orders and Trades: RFS is the source of entry
    SyntheticPassive = 1ULL << 57,    // Orders: Flag of the passive synthetic order
    BOC = 1ULL << 60,                 // Orders: Book or Cancel order
    DuringDiscreteAuction = 1ULL << 62 // Orders and Trades: The record formed in the process of discrete auction
};

// MDFlags2Set
struct MDFlags2Set
{
    const uint64_t value = 0; // Bitmask representing MD Flags 2
};

// Enum class for FlagsSet
enum class FlagsSet : uint64_t
{
    EveningOrMorningSession = 1ULL << 0,  // Trading in the evening or morning session
    AnonymousTrading = 1ULL << 4,  // Anonymous trading
    PrivateTrading = 1ULL << 5,  // Private trading
    DaySession = 1ULL << 6,  // Trading in the day session
    MultiLeg = 1ULL << 8,  // MultiLeg instrument
    Collateral = 1ULL << 18, // Collateral instrument
    IntradayExercise = 1ULL << 19  // Exercise in the intraday clearing session
};

// Message: Heartbeat
struct Heartbeat
{
    // No fields in the Heartbeat message
};

// Message: SequenceReset
struct SequenceReset
{
    uint32_t NewSeqNo; // New sequence number
};
static_assert(sizeof(SequenceReset) == 4, "SequenceReset size is incorrect");

// Message: BestPrices
struct BestPricesEntry
{
    Decimal5NULL MktBidPx;    // Best bid price
    Decimal5NULL MktOfferPx;  // Best offer price
    int64_t MktBidSize;     // Total quantity in best bid
    int64_t MktOfferSize;   // Total quantity in best offer
    int32_t SecurityID;         // Instrument numeric code
};

struct BestPrices
{
    GroupSize NoMDEntries;                     // Group size
    std::vector<BestPricesEntry> MDEntries;    // Array of entries
};

// Message: EmptyBook
struct EmptyBook
{
    uint32_t LastMsgSeqNumProcessed; // Sequence number of the last valid Incremental feed packet
};

// Message: OrderUpdate
struct OrderUpdate
{
    int64_t MDEntryID;             // Order ID
    Decimal5 MDEntryPx;          // Order price
    int64_t MDEntrySize;           // Market Data entry size
    MDFlagsSet MDFlags;          // Bitmask of flags
    MDFlags2Set MDFlags2;        // Additional bitmask of flags
    int32_t SecurityID;            // Instrument numeric code
    uint32_t RptSeq;               // Market Data entry sequence number
    MDUpdateAction MDUpdateAction; // Market Data update action
    MDEntryType MDEntryType;     // Market Data entry type
};

// Message: OrderExecution
struct OrderExecution
{
    int64_t MDEntryID;             // Order ID
    Decimal5NULL MDEntryPx;      // Order price
    int64_t MDEntrySize;       // Market Data entry size
    Decimal5 LastPx;             // Matched trade price
    int64_t LastQty;               // Trade volume
    int64_t TradeID;               // Trade ID
    MDFlagsSet MDFlags;          // Bitmask of flags
    MDFlags2Set MDFlags2;        // Additional bitmask of flags
    int32_t SecurityID;            // Instrument numeric code
    uint32_t RptSeq;               // Market Data entry sequence number
    MDUpdateAction MDUpdateAction; // Market Data update action
    MDEntryType MDEntryType;     // Market Data entry type
};

// Message: OrderBookSnapshot
struct OrderBookSnapshotEntry
{
    int64_t MDEntryID;         // Order ID
    uint64_t TransactTime;         // Start of event processing time
    Decimal5NULL MDEntryPx;      // Order price
    int64_t MDEntrySize;       // Market Data entry size
    int64_t TradeID;           // Trade ID
    MDFlagsSet MDFlags;          // Bitmask of flags
    MDFlags2Set MDFlags2;        // Additional bitmask of flags
    MDEntryType MDEntryType;     // Market Data entry type
};

struct OrderBookSnapshot
{
    int32_t SecurityID;              // Instrument numeric code
    uint32_t LastMsgSeqNumProcessed; // Sequence number of the last Incremental feed packet processed
    uint32_t RptSeq;                 // Market Data entry sequence number
    uint32_t ExchangeTradingSessionID; // Trading session ID
    GroupSize NoMDEntries;         // Group size for entries
    std::unique_ptr<std::vector<OrderBookSnapshotEntry>> MDEntries; // Entries array

    static constexpr size_t BASE_SIZE = sizeof(SecurityID) + sizeof(LastMsgSeqNumProcessed) +
                                        sizeof(RptSeq) + sizeof(ExchangeTradingSessionID);
};

// Message: SecurityDefinition
struct SecurityDefinition
{
    uint32_t TotNumReports;         // Total messages number in the current list
    char Symbol[25];              // Symbol code of the instrument
    int32_t SecurityID;             // Instrument numeric code
    static constexpr char SecurityIDSource = SECURITY_ID_SOURCE; // Class or source of SecurityID // needs to be constexpr so it's not counted in sequence of memory cos packet doesn't contain this field, it's implied
    char SecurityAltID[25];       // Instrument symbol code
    SecurityAltIDSource SecurityAltIDSource; // Class of SecurityAltID
    char SecurityType[4];         // Multileg type
    char CFICode[6];              // Financial instrument class
    Decimal5NULL StrikePrice;     // Strike price
    int32_t ContractMultiplier; // Units of underlying asset in instrument
    SecurityTradingStatus SecurityTradingStatus; // Trading status of the instrument
    char Currency[3];             // Currency
    static constexpr char MarketID[4] = { 'M', 'O', 'E', 'X' }; // Market identifier
    MarketSegmentID MarketSegmentID; // Market segment identifier
    TradingSessionID TradingSessionID; // Trading session type
    int32_t ExchangeTradingSessionID; // Trading session ID
    Decimal5NULL Volatility;      // Option volatility
    Decimal5NULL HighLimitPx;     // Upper price limit
    Decimal5NULL LowLimitPx;      // Lower price limit
    Decimal5NULL MinPriceIncrement; // Minimum price step
    Decimal5NULL MinPriceIncrementAmount; // Price step cost in RUB
    Decimal2NULL InitialMarginOnBuy; // Initial margin for buy
    Decimal2NULL InitialMarginOnSell; // Initial margin for sell
    Decimal2NULL InitialMarginSyntetic; // Underlying collateral for uncovered position
    Decimal5NULL TheorPrice;      // Option theoretical price
    Decimal5NULL TheorPriceLimit; // Option theoretical price with adjusted limits
    Decimal5NULL UnderlyingQty;   // Security nominal value
    char UnderlyingCurrency[3];   // Code of currency of the security nominal value
    uint32_t MaturityDate;      // Instrument settlement date
    uint32_t MaturityTime;      // Instrument settlement time
    FlagsSet Flags;               // Flags of instrument
    Decimal5NULL MinPriceIncrementAmountCurr; // Value of the minimum increment in foreign currency
    Decimal5NULL SettlPriceOpen; // Settlement price at the start of the session
    char ValuationMethod[4];      // Type of valuation method applied
    DoubleNULL RiskFreeRate;      // Risk-free interest rate
    DoubleNULL FixedSpotDiscount; // Discounted values of declared cash flows
    DoubleNULL ProjectedSpotDiscount; // Discounted values of projected cash flows
    char SettlCurrency[3];        // Settlement currency
    NegativePrices NegativePrices; // Negative prices eligibility
    int32_t DerivativeContractMultiplier; // Volume of underlying asset in the contract
    DoubleNULL InterestRateRiskUp; // Interest risk rate on upward scenario
    DoubleNULL InterestRateRiskDown; // Interest risk rate on downward scenario
    DoubleNULL RiskFreeRate2;     // Second risk-free interest rate
    DoubleNULL InterestRate2RiskUp; // Interest rate risk for upward scenario (second rate)
    DoubleNULL InterestRate2RiskDown; // Interest rate risk for downward scenario (second rate)
    Decimal5NULL SettlPrice;      // Settlement price at the end of the clearing session
    
    struct MDFeedTypes
    {
        char MDFeedType[25];
        uint32_t MarketDepth;
        uint32_t MDBookType;
    };

    struct Underlyings
    {
        char UnderlyingSymbol[25];
        char UnderlyingBoard[4];
        int32_t UnderlyingSecurityID;
        int32_t UnderlyingFutureID;
    };

    struct Legs
    {
        char LegSymbol[25];
        int32_t LegSecurityID;
        int32_t LegRatioQty;
    };

    struct InstrAttrib
    {
        int32_t InstrAttribType;
        char InstrAttribValue[31];
    };

    struct Events
    {
        int32_t EventType;
        uint32_t EventDate;
        uint64_t EventTime;
    };

    GroupSize NoMDFeedTypes;         // Group size for entries
    std::unique_ptr<std::vector<MDFeedTypes>> MDFeedTypesEntries;
    
    GroupSize NoUnderlyings;         // Group size for entries
    std::unique_ptr<std::vector<Underlyings>> UnderlyingsEntries;

    GroupSize NoLegs;         // Group size for entries
    std::unique_ptr<std::vector<Legs>> LegsEntries;
    
    GroupSize NoInstrAttrib;         // Group size for entries
    std::unique_ptr<std::vector<InstrAttrib>> InstrAttribEntries;

    GroupSize NoEvents;         // Group size for entries
    std::unique_ptr<std::vector<Events>> EventsEntries;

    Utf8String SecurityDesc;
    VarString QuotationList;

    // Static constexpr member for size
    static constexpr size_t BASE_SIZE =
        sizeof(TotNumReports) +
        sizeof(Symbol) +
        sizeof(SecurityID) +
        sizeof(SecurityIDSource) +
        sizeof(SecurityAltID) +
        sizeof(SecurityAltIDSource) +
        sizeof(SecurityType) +
        sizeof(CFICode) +
        sizeof(StrikePrice) +
        sizeof(ContractMultiplier) +
        sizeof(SecurityTradingStatus) +
        sizeof(Currency) +
        sizeof(MarketID) +
        sizeof(MarketSegmentID) +
        sizeof(TradingSessionID) +
        sizeof(ExchangeTradingSessionID) +
        sizeof(Volatility) +
        sizeof(HighLimitPx) +
        sizeof(LowLimitPx) +
        sizeof(MinPriceIncrement) +
        sizeof(MinPriceIncrementAmount) +
        sizeof(InitialMarginOnBuy) +
        sizeof(InitialMarginOnSell) +
        sizeof(InitialMarginSyntetic) +
        sizeof(TheorPrice) +
        sizeof(TheorPriceLimit) +
        sizeof(UnderlyingQty) +
        sizeof(UnderlyingCurrency) +
        sizeof(MaturityDate) +
        sizeof(MaturityTime) +
        sizeof(Flags) +
        sizeof(MinPriceIncrementAmountCurr) +
        sizeof(SettlPriceOpen) +
        sizeof(ValuationMethod) +
        sizeof(RiskFreeRate) +
        sizeof(FixedSpotDiscount) +
        sizeof(ProjectedSpotDiscount) +
        sizeof(SettlCurrency) +
        sizeof(NegativePrices) +
        sizeof(DerivativeContractMultiplier) +
        sizeof(InterestRateRiskUp) +
        sizeof(InterestRateRiskDown) +
        sizeof(RiskFreeRate2) +
        sizeof(InterestRate2RiskUp) +
        sizeof(InterestRate2RiskDown) +
        sizeof(SettlPrice);
};

struct SecurityStatus {
    int32_t SecurityID;                     // Instrument numeric code
    char SecurityIDSource = SECURITY_ID_SOURCE; // Identifies class or source of SecurityID value
    char Symbol[25];                        // Symbol code of the instrument
    SecurityTradingStatus SecurityTradingStatus; // Identifies the trading status of the instrument
    Decimal5NULL HighLimitPx;               // Upper price limit
    Decimal5NULL LowLimitPx;                // Lower price limit
    Decimal2NULL InitialMarginOnBuy;        // Initial margin
    Decimal2NULL InitialMarginOnSell;       // Initial margin
    Decimal2NULL InitialMarginSyntetic;     // Underlying collateral for one uncovered position (RUB)
};


// Message: SecurityMassStatus
struct SecurityMassStatusEntry
{
    int32_t SecurityID;            // Instrument numeric code
    char SecurityIDSource = SECURITY_ID_SOURCE; // Class or source of SecurityID
    SecurityTradingStatus SecurityTradingStatus; // Trading status of the instrument
};

struct SecurityMassStatus
{
    GroupSize2 NoRelatedSym;                      // Group size
    std::vector<SecurityMassStatusEntry> Entries; // Entries array
};

struct SecurityDefinitionUpdateReport
{
    int32_t SecurityID;
    static constexpr char SecurityIDSource = '8';
    Decimal5NULL Volatility;
    Decimal5NULL TheorPrice;
    Decimal5NULL TheorPriceLimit;
};

struct TradingSessionStatus {
    uint64_t TradSesOpenTime;                                 // Trading session open date and time
    uint64_t TradSesCloseTime;                                // Trading session close date and time
    uint64_t TradSesIntermClearingStartTime;                  // Intraday clearing session start date and time (nullable)
    uint64_t TradSesIntermClearingEndTime;                    // Intraday clearing session end date and time (nullable)
    uint8_t TradingSessionID;                                 // Trading session type
    uint64_t ExchangeTradingSessionID;                        // Trading session ID (nullable)
    uint8_t TradSesStatus;                                    // State of the trading session
    static constexpr char MarketID[4] = {'M', 'O', 'E', 'X'}; // Identifies the market
    char MarketSegmentID;                                     // Identifies the market segment
    uint8_t TradSesEvent;                                     // Identifies an event related to TradSesStatus
};


// Message: Logon
struct Logon
{
    // No fields in Logon
};

// Message: Logout
struct Logout
{
    char Text[256]; // Free format text string, reason for logout
};

// Message: MarketDataRequest
struct MarketDataRequest
{
    uint32_t ApplBegSeqNum; // Sequence number of the first requested message
    uint32_t ApplEndSeqNum; // Sequence number of the last requested message
};

using SIMBAMessage = std::variant<OrderUpdate, OrderExecution, OrderBookSnapshot, SecurityDefinition, SecurityStatus, SecurityDefinitionUpdateReport, SequenceReset, TradingSessionStatus>;

struct SIMBAPacket
{
    MarketDataPacketHeader marketDataHeader{};
    std::optional<IncrementalPacketHeader> incrementalHeader{};
    MessageHeader messageHeader{};
    std::vector<SIMBAMessage> messages;
};

#pragma pack(pop)