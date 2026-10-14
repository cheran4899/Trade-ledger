#include <iostream>
#include <iomanip>
#include <cmath>
#include "portfolio.h"

Portfolio::Portfolio(const std::string& name)
    //creating a portfolio with a name
    : portfolio_name(name){}



bool Portfolio::process_trade(const Trade& trade){
    //if sides are not buy or sell

    if(trade.side != "BUY" && trade.side != "SELL"){
        std::cout << "Invalid side for " << trade.symbol << " side: " <<trade.side << "\n";
        return false;
    }

    if(trade.quantity <=0 || trade.price <= 0){
        std::cout << "Invalid price/quantity for " << trade.symbol << "\n";
        return false;
    }

        // finds the key
    auto it = positions.find(trade.symbol);
    if(it != positions.end()){
        //if the key exists, then the position's reference is read and the it->secon refer to the value it -> first refer to key
        Position& p = it -> second;

        //perform actions accordingly as the position already exists

        if (trade.side == "BUY"){
            p.buy(trade.quantity, trade.price);
            return true;
        }
        else{
            return p.sell(trade.quantity, trade.price);
        }
    }
    //since position doesn't exist, we need to create one and we cant sell before buying it
    if(trade.side == "SELL"){
        std::cout << "Cannot sell without the position existing. " << trade.symbol << "\n";
        return false;
    }
    Position p(trade.symbol);
    p.buy(trade.quantity, trade.price);
    positions.emplace(trade.symbol, p);

    return true;


}
Position* Portfolio::find_position(const std::string& symbol ){
    auto it = positions.find(symbol);
    if(it != positions.end()){
        return &it -> second;
    }
    return nullptr;
}

const Position* Portfolio::find_position(const std::string& symbol) const{
    auto it = positions.find(symbol);
    if (it != positions.end()){
        return &it->second;
    }
    return nullptr;
}

void Portfolio::show_position(const std::string& symbol ) const{
    auto it = positions.find(symbol);
    if(it != positions.end()){
        const Position& p = it -> second;
        std::cout << "Symbol: " << p.get_symbol() << "\n";
        std::cout << "Average Price: " << p.get_average_price() << "\n";
        std::cout << "Quantity: " << p.get_quantity() << "\n";
        std::cout << "PnL: " << p.get_realised_pnl() << "\n";
    }
    else{
        std::cout << "Position couldn't be found with symbol:  " << symbol << "\n";
    }
}

void Portfolio::show_all_positions() const
{
    std::cout << std::left;
    std::cout << "\n";
    std::cout << std::string(65, '-') << '\n';
    std::cout << std::setw(12) << "Symbol"
              << std::setw(18) << "Average Price"
              << std::setw(15) << "Quantity"
              << std::setw(18) << "Realised PnL"
              << '\n';
    std::cout << std::string(65, '-') << '\n';
    std::cout << std::fixed << std::setprecision(2);
    for (const auto& entry : positions)
    {
        const Position& p = entry.second;
        std::cout << std::setw(12) << p.get_symbol()
                  << std::setw(18) << p.get_average_price()
                  << std::setw(15) << p.get_quantity()
                  << std::setw(18) << p.get_realised_pnl()
                  << '\n';
    }
    std::cout << std::string(65, '-') << '\n';
}

double Portfolio::get_total_realised_pnl() const{
    double realised_pnl = 0;
    for (const auto& entry: positions){
        const Position& p = entry.second;
        realised_pnl += p.get_realised_pnl();
    }
    return realised_pnl;
}

double Portfolio::get_total_unrealised_pnl(const std::map<std::string, double>& market_prices ) const{

    if(!validate_market_prices(market_prices)){
        return 0;
    }

    bool flag = true;

    for(const auto& position:positions){
        auto it = market_prices.find(position.first);
        if(it == market_prices.end()){
            std::cout << "P&L calculation failed as it missing: "<< position.first << "\n";
        }     
    }
    if(!flag){
        return 0;
    }

    double unrealised_pnl = 0;
    for(const auto& key: market_prices){
        const Position* p = find_position(key.first);
        if(p!=nullptr){
        unrealised_pnl += p->get_unrealised_pnl(key.second);}
    }
    return unrealised_pnl;
}


double Portfolio::get_total_market_value(const std::map<std::string, double>& market_prices ) const{
    
    if(!validate_market_prices(market_prices)){
        return 0;
    }
    bool flag = true;
    for(const auto& position:positions){
        auto it = market_prices.find(position.first);
        if(it == market_prices.end()){
            std::cout << "Market value calculations failed as it is missing: " << position.first << "\n";
        }
    }
    if(!flag){
        return 0;
    }
    double total_market_value = 0;
    for(const auto& key:market_prices){
        const Position* p = find_position(key.first);
        if(p!=nullptr){
            total_market_value += p->get_market_value(key.second);
        }
    }
    return total_market_value;
}


void Portfolio::show_portfolio_report(
    const std::map<std::string, double>& market_prices
) const
{   
    if(!validate_market_prices(market_prices)){
        return;
    }
    // Validate market prices before printing the report
    for (const auto& entry : positions)
    {
        if (entry.second.get_quantity() == 0)
            continue;

        if (market_prices.find(entry.first) == market_prices.end())
        {
            std::cerr << "Portfolio report failed: missing market price for "
                      << entry.first << '\n';
            return;
        }
    }

    constexpr int symbol_width = 10;
    constexpr int quantity_width = 10;
    constexpr int price_width = 14;
    constexpr int value_width = 15;
    constexpr int pnl_width = 15;

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "\n"
              << std::string(93, '=') << '\n'
              << std::setw(55) << "PORTFOLIO REPORT" << '\n'
              << std::string(93, '=') << '\n';

    std::cout << std::left
              << std::setw(symbol_width) << "Symbol"
              << std::right
              << std::setw(quantity_width) << "Quantity"
              << std::setw(price_width) << "Avg Price"
              << std::setw(price_width) << "Market Price"
              << std::setw(value_width) << "Market Value"
              << std::setw(pnl_width) << "Realised PnL"
              << std::setw(pnl_width) << "Unrealised PnL"
              << '\n';

    std::cout << std::string(93, '-') << '\n';

    double total_market_value = 0.0;
    double total_realised_pnl = 0.0;
    double total_unrealised_pnl = 0.0;

    for (const auto& entry : positions)
    {
        const Position& p = entry.second;

        double market_price = 0.0;
        auto price_it = market_prices.find(entry.first);

        if (price_it != market_prices.end())
        {
            market_price = price_it->second;
        }

        double market_value = p.get_market_value(market_price);
        double realised_pnl = p.get_realised_pnl();
        double unrealised_pnl = p.get_unrealised_pnl(market_price);

        total_market_value += market_value;
        total_realised_pnl += realised_pnl;
        total_unrealised_pnl += unrealised_pnl;

        std::cout << std::left
                  << std::setw(symbol_width) << p.get_symbol()
                  << std::right
                  << std::setw(quantity_width) << p.get_quantity()
                  << std::setw(price_width) << p.get_average_price()
                  << std::setw(price_width) << market_price
                  << std::setw(value_width) << market_value
                  << std::setw(pnl_width) << realised_pnl
                  << std::setw(pnl_width) << unrealised_pnl
                  << '\n';
    }

    std::cout << std::string(93, '-') << '\n';

    std::cout << std::left
              << std::setw(symbol_width + quantity_width) << "TOTAL"
              << std::right
              << std::setw(price_width * 2) << ""
              << std::setw(value_width) << total_market_value
              << std::setw(pnl_width) << total_realised_pnl
              << std::setw(pnl_width) << total_unrealised_pnl
              << '\n';

    std::cout << std::string(93, '=') << '\n';
}

bool Portfolio::validate_market_prices(const std::map<std::string, double>& market_prices) const{

    for(const auto& record:market_prices){

        double price = record.second;

        if (!std::isfinite(price) || price <= 0.0)
        {
            std::cerr << "Invalid market price for "
                    << record.first << '\n';
            return false;
        }
    }
    return true;
}
