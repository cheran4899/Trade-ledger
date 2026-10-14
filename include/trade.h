#ifndef TRADE_H
#define TRADE_H

#include <string>
struct Trade{
    std::string trade_id;
    std::string symbol;
    long quantity;
    double price;
    std::string side;
    bool is_cancelled = false;

};

#endif