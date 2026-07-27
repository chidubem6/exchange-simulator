#include "MatchingEngine.h"
#include "Price.h"
#include <iostream>
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
        // Prices are integer ticks, so this comparison is exact -- no epsilon needed.
        if (!bid || !ask || bid->priceTicks < ask->priceTicks) break;

        // The trade size is limited by whichever order has the smaller quantity
        int qty = std::min(bid->quantity, ask->quantity);

        // Trade at the passive (resting) order's price -- the one placed first has the lower ID
        long long tradePriceTicks = (bid->id < ask->id) ? bid->priceTicks : ask->priceTicks;

        std::cout << "TRADE: " << qty << " " << symbol
                  << " @ " << formatTicks(tradePriceTicks) << "\n";

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
