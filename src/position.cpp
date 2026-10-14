#include <iostream>
#include "position.h"

Position::Position(std::string s){
    symbol = s;
    average_price = 0;
    realised_pnl = 0;
    quantity = 0;
};

long Position::get_quantity() const{
    return quantity;
}

double Position::get_realised_pnl() const{
    return realised_pnl;
}

double Position::get_average_price() const{
    return average_price;
}

std::string Position::get_symbol() const{
    return symbol;
}
void Position::buy(long trade_quantity, double buy_price){
    //updating quanitity and average price
    double total_price = quantity * average_price;
    double new_price = total_price + (trade_quantity * buy_price);
    quantity += trade_quantity;
    average_price = new_price / quantity;
}

bool Position::sell(long trade_quantity, double sell_price){
    // here selling doesn't change the price. Only changes the pnl 
    if(quantity < trade_quantity){
        std::cout << "Cannot sell "
          << trade_quantity
          << " "
          << symbol
          << ": current position is only "
          << quantity
          << '\n';
        return false;
    }
    double pnl = (trade_quantity * sell_price )- (average_price * trade_quantity);
    realised_pnl += pnl;
    quantity -= trade_quantity;
    return true;
}

double Position::get_unrealised_pnl(double market_price) const{
    //unrealised pnl is profit if all the stocks area sold right now
    return (market_price - average_price) * quantity; 
}

double Position::get_market_value(double market_price) const{
    return market_price * quantity;
}

