#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include <map>
#include <string>
#include "position.h"
#include "trade.h"

class Portfolio{
    public:
        Portfolio(const std::string& name);
        bool process_trade(const Trade& trade);
        Position* find_position(const std::string& symbol );
        const Position* find_position(const std::string& symbol) const;
        void show_position(const std::string& symbol ) const;
        void show_all_positions() const;
        double get_total_realised_pnl() const;
        double get_total_unrealised_pnl(const std::map<std::string, double>& market_prices ) const;
        double get_total_market_value(const std::map<std::string, double>& market_prices ) const;
        void show_portfolio_report(const std::map<std::string, double>& market_prices) const;
        private:
        bool validate_market_prices(const std::map<std::string, double>& market_prices) const;

    private:
        std::map<std::string, Position> positions; 
        std::string portfolio_name;
};

#endif