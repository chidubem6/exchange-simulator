#ifndef ORDERBOOK_H
#define ORDERBOOK_H
#include "Order.h"
#include <map>
#include <deque>
#include <unordered_map>

// Manages buy and sell orders for one or more trading symbols (e.g. "AAPL", "TSLA").
class OrderBook {
    private:
        // Holds all bids and asks for a single symbol
        struct SymbolBook {
            std::map<double, std::deque<Order>, std::greater<double>> bids; // highest price first
            std::map<double, std::deque<Order>> asks;                       // lowest price first
        };

        std::map<std::string, SymbolBook> books;                       // symbol -> its order book
        std::unordered_map<int, std::pair<std::string, Side>> orderIndex; // orderId -> (symbol, side) for fast lookup

    public:
        void addOrder(const Order& order);
        bool cancelOrder(int orderId, std::string& outSymbol);
        void display(const std::string& symbol) const;
        void displayAll() const;

        // Returns the highest-priced buy order for the symbol, or nullptr if none exists.
        Order* bestBid(const std::string& symbol);

        // Returns the lowest-priced sell order for the symbol, or nullptr if none exists.
        Order* bestAsk(const std::string& symbol);

        void removeTopBid(const std::string& symbol);
        void removeTopAsk(const std::string& symbol);

        // Returns a list of all symbols currently in the order book.
        std::vector<std::string> getSymbols() const;
};


#endif
