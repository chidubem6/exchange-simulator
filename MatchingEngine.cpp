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

        // Stop if either side is empty, or the buyer isn't offering enough.
        // Use epsilon tolerance to treat prices equal within floating-point rounding.
        if (!bid || !ask || bid->price < ask->price - 1e-9) break;

        // The trade size is limited by whichever order has the smaller quantity
        int qty = std::min(bid->quantity, ask->quantity);

        // Trade at the passive (resting) order's price — the one placed first has the lower ID
        double tradePrice = (bid->id < ask->id) ? bid->price : ask->price;

        std::cout << "TRADE: " << qty << " " << symbol
                  << " @ " << std::fixed << std::setprecision(2) << tradePrice << "\n";

        // Determine fills before any mutation so we don't touch a pointer after removal.
        bool bidFilled = (bid->quantity == qty);
        bool askFilled = (ask->quantity == qty);

        // Only update quantity on orders that will remain in the book.
        if (!bidFilled) bid->quantity -= qty;
        if (!askFilled) ask->quantity -= qty;

        // Remove fully-filled orders. After each call the corresponding pointer is dangling —
        // bidFilled/askFilled were captured above so we never dereference them again.
        if (bidFilled) orderBook.removeTopBid(symbol);
        if (askFilled) orderBook.removeTopAsk(symbol);
    }
}
