#ifndef ORDERBOOK_H
#define ORDERBOOK_H
#include "Order.h"
#include <map>
#include <list>
#include <unordered_map>
#include <vector>

// Manages buy and sell orders for one or more trading symbols (e.g. "AAPL", "TSLA").
class OrderBook {
    private:
        // Holds all bids and asks for a single symbol.
        // std::list is used instead of std::deque because list iterators remain valid
        // after insertions/deletions elsewhere in the list, letting us cache them in orderIndex.
        struct SymbolBook {
            std::map<double, std::list<Order>, std::greater<double>> bids; // highest price first
            std::map<double, std::list<Order>> asks;                       // lowest price first
        };

        // Remembers exactly where an order lives so cancelOrder() can erase it in O(1).
        struct OrderLocation {
            std::string symbol;
            Side side;
            double price;                  // which price level
            std::list<Order>::iterator it; // exact position within that level's list
        };

        std::map<std::string, SymbolBook> books;
        std::unordered_map<long long, OrderLocation> orderIndex; // orderId -> precise location

    public:
        void addOrder(const Order& order);
        bool cancelOrder(long long orderId, std::string& outSymbol);
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
