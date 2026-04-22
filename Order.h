#ifndef ORDER_H
#define ORDER_H

#include <string>

enum class Side { BUY, SELL };

// Represents a single resting or incoming order in the exchange.
struct Order {
    int id;          // unique, monotonically increasing; lower ID = placed earlier (used for price-time priority)
    Side side;       // BUY or SELL
    std::string symbol;  // ticker, e.g. "AAPL"
    double price;    // limit price the trader is willing to buy/sell at
    int quantity;    // remaining unfilled quantity (decremented as partial fills occur)
};

#endif