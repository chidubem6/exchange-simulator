#ifndef ORDER_H
#define ORDER_H

#include <string>

enum class Side { BUY, SELL };

struct Order {
    int id;
    Side side;
    std::string symbol;
    double price;
    int quantity;
};

#endif