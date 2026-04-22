#include "MatchingEngine.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

void MatchingEngine::processOrder(const Order& o) {
    // Add the order to the book, then check if it matches anything
    orderBook.addOrder(o);
    matchOrders(o.symbol);
}

void MatchingEngine::matchOrders(const std::string& symbol) {
    // Keep looping until there are no more matching buy/sell pairs
    while (true) {
        Order* bid = orderBook.bestBid(symbol); // highest buy order
        Order* ask = orderBook.bestAsk(symbol); // lowest sell order

        // Stop if either side is empty, or the buyer isn't offering enough
        if (!bid || !ask || bid->price < ask->price) break;

        // The trade size is limited by whichever order has the smaller quantity
        int qty = std::min(bid->quantity, ask->quantity);

        // Trade at the passive (resting) order's price — the one placed first has the lower ID
        double tradePrice = (bid->id < ask->id) ? bid->price : ask->price;

        std::cout << "TRADE: " << qty << " " << symbol
                  << " @ " << std::fixed << std::setprecision(2) << tradePrice << "\n";

        // Reduce the quantity on both orders by how much was traded
        bid->quantity -= qty;
        ask->quantity -= qty;

        // Remove an order from the book if it has been fully filled
        bool bidFilled = (bid->quantity == 0);
        bool askFilled = (ask->quantity == 0);

        if (bidFilled) orderBook.removeTopBid(symbol);
        if (askFilled) orderBook.removeTopAsk(symbol);
    }
}
