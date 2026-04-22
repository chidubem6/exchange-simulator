#include "OrderBook.h"
#include <iostream>
#include <iomanip>

void OrderBook::addOrder(const Order& order) {
    auto& symbol = books[order.symbol];

    // Queue the order at its price level on the correct side
    if (order.side == Side::BUY) {
        symbol.bids[order.price].push_back(order);
    }
    else {
        symbol.asks[order.price].push_back(order);
    }

    // Record the order's location so it can be found quickly by ID later
    orderIndex[order.id] = {order.symbol, order.side};
}

bool OrderBook::cancelOrder(int id, std::string& outSymbol) {
    // Use the index to find which symbol and side this order belongs to
    auto orderIndexEntry = orderIndex.find(id);
    if (orderIndexEntry == orderIndex.end()) {
        return false;
    }


    // Extract the symbol and side stored in the index entry
    std::string symbol = orderIndexEntry->second.first;
    Side side = orderIndexEntry->second.second;
    outSymbol = symbol;

    auto& symbolBook = books.at(symbol);
    bool orderWasRemoved = false;

    // Search a price-level map for the order, remove it, and clean up empty price levels.
    // Returns true if the order was found and removed.
    auto removeFromSide = [&](auto& priceMap) {
        for (auto priceLevelIt = priceMap.begin(); priceLevelIt != priceMap.end(); ++priceLevelIt) {
            auto& ordersAtThisPrice = priceLevelIt->second;

            for (auto orderIt = ordersAtThisPrice.begin(); orderIt != ordersAtThisPrice.end(); ++orderIt) {
                if (orderIt->id == id) {
                    ordersAtThisPrice.erase(orderIt);

                    // Clean up the price level if it's now empty
                    if (ordersAtThisPrice.empty()) {
                        priceMap.erase(priceLevelIt);
                    }

                    return true;
                }
            }
        }
        return false;
    };

    if (side == Side::BUY) {
        orderWasRemoved = removeFromSide(symbolBook.bids);
    } else {
        orderWasRemoved = removeFromSide(symbolBook.asks);
    }

    // Remove from the index so it can't be looked up anymore
    if (orderWasRemoved) {
        orderIndex.erase(id);
    }

    return orderWasRemoved;
}

Order* OrderBook::bestBid(const std::string& symbol) {
    auto symbolBookEntry = books.find(symbol);
    if (symbolBookEntry == books.end() || symbolBookEntry->second.bids.empty()) return nullptr;

    // bids is sorted highest-to-lowest, so begin() is the best (highest) price
    // returns a pointer to the oldest order at that price (first in the queue)
    return &symbolBookEntry->second.bids.begin()->second.front();
}

Order* OrderBook::bestAsk(const std::string& symbol) {
    auto symbolBookEntry = books.find(symbol);
    if (symbolBookEntry == books.end() || symbolBookEntry->second.asks.empty()) return nullptr;

    // asks is sorted lowest-to-highest, so begin() is the best (lowest) price
    // returns a pointer to the oldest order at that price (first in the queue)
    return &symbolBookEntry->second.asks.begin()->second.front();
}

void OrderBook::removeTopBid(const std::string& symbol) {
    auto symbolBookEntry = books.find(symbol);
    if (symbolBookEntry == books.end()) return;
    auto& bids = symbolBookEntry->second.bids;
    if (bids.empty()) return;

    auto& ordersAtBestPrice = bids.begin()->second;
    if (!ordersAtBestPrice.empty()) {
        orderIndex.erase(ordersAtBestPrice.front().id); // remove from index before erasing
        ordersAtBestPrice.pop_front();
    }

    // Remove the price level entirely if no orders remain at that price
    if (ordersAtBestPrice.empty()) bids.erase(bids.begin());
}

void OrderBook::removeTopAsk(const std::string& symbol) {
    auto symbolBookEntry = books.find(symbol);
    if (symbolBookEntry == books.end()) return;
    auto& asks = symbolBookEntry->second.asks;
    if (asks.empty()) return;

    auto& ordersAtBestPrice = asks.begin()->second;
    if (!ordersAtBestPrice.empty()) {
        orderIndex.erase(ordersAtBestPrice.front().id); // remove from index before erasing
        ordersAtBestPrice.pop_front();
    }

    // Remove the price level entirely if no orders remain at that price
    if (ordersAtBestPrice.empty()) asks.erase(asks.begin());
}

void OrderBook::display(const std::string& symbol) const {
    auto symbolBookEntry = books.find(symbol);
    std::cout << "Order Book [" << symbol << "]:\n";

    // Print asks first (they appear above bids on a real order book display)
    std::cout << "  ASKS: ";
    if (symbolBookEntry == books.end() || symbolBookEntry->second.asks.empty()) {
        std::cout << "(empty)";
    } else {
        bool isFirstEntry = true;
        for (const auto& priceLevel : symbolBookEntry->second.asks) {
            int totalQuantity = 0;
            for (const auto& order : priceLevel.second) totalQuantity += order.quantity;
            if (!isFirstEntry) std::cout << ", ";
            std::cout << std::fixed << std::setprecision(2) << priceLevel.first << " (" << totalQuantity << ")";
            isFirstEntry = false;
        }
    }
    std::cout << "\n";

    std::cout << "  BIDS: ";
    if (symbolBookEntry == books.end() || symbolBookEntry->second.bids.empty()) {
        std::cout << "(empty)";
    } else {
        bool isFirstEntry = true;
        for (const auto& priceLevel : symbolBookEntry->second.bids) {
            int totalQuantity = 0;
            for (const auto& order : priceLevel.second) totalQuantity += order.quantity;
            if (!isFirstEntry) std::cout << ", ";
            std::cout << std::fixed << std::setprecision(2) << priceLevel.first << " (" << totalQuantity << ")";
            isFirstEntry = false;
        }
    }
    std::cout << "\n";
}

void OrderBook::displayAll() const {
    if (books.empty()) { std::cout << "Order book is empty.\n"; return; }
    for (const auto& symbolBookEntry : books) display(symbolBookEntry.first);
}

std::vector<std::string> OrderBook::getSymbols() const {
    std::vector<std::string> symbols;
    for (const auto& symbolBookEntry : books) symbols.push_back(symbolBookEntry.first);
    return symbols;
}
