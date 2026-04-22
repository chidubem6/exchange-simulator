#ifndef ORDERBOOK_H
#define ORDERBOOK_H
#include "Order.h"
#include <map>
#include <deque>
#include <unordered_map>

class OrderBook {
    private:
        struct SymbolBook {
            std::map<double, std::deque<Order>, std::greater<double>> bids; // highest price first
            std::map<double, std::deque<Order>> asks; // lowest price first
        };

        std::map<std::string, SymbolBook> books; // symbol -> order book
        std::unordered_map<int, std::pair<std::string, Side>> orderIndex; // orderId -> (symbol, side)

    public:

        // Order management
        void addOrder(const Order& order);
        bool cancelOrder(int orderId, std::string& outSymbol);
        void display(const std::string& symbol) const;
        void displayAll() const;

        // Best bid/ask retrieval and removal
        Order* bestBid(const std::string& symbol);
        Order* bestAsk(const std::string& symbol);
        void removeTopBid(const std::string& symbol);
        void removeTopAsk(const std::string& symbol);

        std::vector<std::string> getSymbols() const;
};


#endif