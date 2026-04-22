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
        void addOrder(const Order& order);
        void cancelOrder(int orderId, std::string& outSymbol);
};


#endif