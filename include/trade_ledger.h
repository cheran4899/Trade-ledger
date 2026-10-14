#ifndef TRADE_LEDGER_H
#define TRADE_LEDGER_H

#include <vector>
#include <cstddef>
#include <string>
#include "trade.h"
//#include "portfolio.h"


class TradeLedger{

    private:
        std::vector<Trade> records;

    public:
        void record_trade(const Trade& trade);
        void show_trades() const;
        std::size_t get_trade_count() const;
        bool save_to_csv(const std::string& filename) const;
        bool load_from_csv(const std::string& filename);
        const std::vector<Trade>& get_records() const;
        bool has_trade(const Trade& trade) const;
        bool has_trade(const std::string& trade_id) const;
        bool correct_trade(const std::string& trade_id, const Trade& corrected_trade);
        bool cancel_trade(const std::string& trade_id);
};





#endif