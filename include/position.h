#ifndef POSITION_H
#define POSITION_H

class Position{
    public:
        Position(std::string s);

        void buy(long trade_quantity, double buy_price);
        bool sell(long trade_quantity, double sell_price);
        std::string get_symbol() const;
        long get_quantity() const;
        double get_average_price() const;
        double get_realised_pnl() const;  
        double get_unrealised_pnl(double market_price) const;
        double get_market_value(double market_price) const;
        
    private:
        std::string symbol;
        long quantity;
        double average_price;
        double realised_pnl;
};

#endif