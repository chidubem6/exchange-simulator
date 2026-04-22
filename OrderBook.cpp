#include "OrderBook.h"
#include <iostream>
#include <iomanip>

void OrderBook::addOrder(const Order& order) {
    auto& symbolBook = books[order.symbol];

    // Append to the correct side's price level and store a direct iterator into
    // the list so cancelOrder() can erase in O(1) without scanning.
    if (order.side == Side::BUY) {
        auto& level = symbolBook.bids[order.price];
        level.push_back(order);
        orderIndex[order.id] = {order.symbol, Side::BUY, order.price, std::prev(level.end())};
    } else {
        auto& level = symbolBook.asks[order.price];
        level.push_back(order);
        orderIndex[order.id] = {order.symbol, Side::SELL, order.price, std::prev(level.end())};
    }
}

bool OrderBook::cancelOrder(long long id, std::string& outSymbol) {
    auto indexIt = orderIndex.find(id);
    if (indexIt == orderIndex.end()) return false;

    OrderLocation& loc = indexIt->second;
    outSymbol = loc.symbol;

    auto& symBook = books.at(loc.symbol);

    // Jump directly to the order using the stored iterator — no scan needed.
    if (loc.side == Side::BUY) {
        auto& level = symBook.bids.at(loc.price);
        level.erase(loc.it);
        if (level.empty()) symBook.bids.erase(loc.price);
    } else {
        auto& level = symBook.asks.at(loc.price);
        level.erase(loc.it);
        if (level.empty()) symBook.asks.erase(loc.price);
    }

    orderIndex.erase(indexIt);
    return true;
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
