#ifndef MATCHING_ENGINE_H
#define MATCHING_ENGINE_H

#include "OrderBook.h"

// The MatchingEngine pairs up buy and sell orders.
// It needs an OrderBook to store and look up those orders.
class MatchingEngine {
private:
    OrderBook& orderBook;

    // Looks for matching buy/sell orders for a given stock symbol and executes trades
    void matchOrders(const std::string& symbol);

public:
    // Constructor: give the engine an order book to work with
    MatchingEngine(OrderBook& orderBook) : orderBook{orderBook} {}

    // Call this to submit a new order (buy or sell)
    void processOrder(const Order& newOrder);
};

#endif
