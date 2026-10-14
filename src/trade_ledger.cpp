#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <algorithm>
#include "trade_ledger.h"



std::size_t TradeLedger::get_trade_count() const{
    return records.size();
}

void TradeLedger::record_trade(const Trade& trade){

//if validation failed, return false    
    records.push_back(trade);
}

bool TradeLedger::has_trade(const Trade& trade) const{
    for(const Trade& r_trade:records){
        if(trade.trade_id == r_trade.trade_id){
            return true;
        }
    }
    return false;
}

bool TradeLedger::has_trade(const std::string& trade_id) const{
    for(const Trade& r_trade:records){
        if(trade_id == r_trade.trade_id){
            return true;
        }
    }
    return false;
}


void TradeLedger::show_trades() const{

    std::size_t count = 1;

    std::cout << std::string(80, '-') << '\n';
              
    std::cout << std::setw(6) << "#"
              << std::setw(12) << "Trade id"
              << std::setw(12) << "Side"
              << std::setw(18) << "Symbol"
              << std::setw(15) << "Quantity"
              << std::setw(18) << "Price"
              << '\n';
    for(const Trade& trade:records){
            std::cout << std::setw(6) << count++
              << std::setw(12) << trade.trade_id
              << std::setw(12) << trade.side
              << std::setw(18) << trade.symbol
              << std::setw(15) << trade.quantity
              << std::setw(18) << trade.price
              << '\n';
    }
}

const std::vector<Trade>& TradeLedger::get_records() const{
    return records;
}

bool TradeLedger::correct_trade(const std::string& trade_id, const Trade& corrected_trade){
    if(trade_id != corrected_trade.trade_id){
        std::cout << "The corrected trade id doesn't match with provided trade id \n";
        return false;
    }
    if(!has_trade(trade_id)){
        std::cout << "The requested trade correction id is not present in the ledger \n";
        return false;
    }

    if(corrected_trade.price <=0){
        std::cout << "Trade cannot be corrected as the price is not positive\n";
        return false;
    }
    if(corrected_trade.quantity <= 0){
        std::cout << "Trade cannot be corrected as the quanitity is not positive!\n";
        return false;
    }
    if(!(corrected_trade.side == "BUY" || corrected_trade.side == "SELL")){
        std::cout << "Trade cannot be corrected as the side is not buy or sell!";
        return false;
    }
    if(corrected_trade.symbol.empty()){
        std::cout << "Symbol cannot be set to empty for correction!";
        return false;
    }

    for(Trade& trade:records){
        if(trade.trade_id == corrected_trade.trade_id){
            trade.side = corrected_trade.side;
            trade.quantity = corrected_trade.quantity;
            trade.symbol = corrected_trade.symbol;
            trade.price = corrected_trade.price;
            return true;
        }
    }
    return false; 
}

bool TradeLedger::cancel_trade(const std::string& trade_id){

    if(!has_trade(trade_id)){
        std::cout << "Trade cannot be found to cancel\n";
        return false;
    }

    for(Trade& trade:records){
        if(trade.trade_id == trade_id){

            if(trade.is_cancelled){
                std::cout << "Trade is already cancelled\n";
                return false;
            }
            else{
            trade.is_cancelled = true;
            return true;

            } 
            }

    }
    return false;
    }






bool TradeLedger::save_to_csv(const std::string& filename) const{

    std::ofstream file(filename);
    if(!file.is_open()){
        std::cout << filename << " cannot be opened/created!";
        return false;
    }

    file << "Tradeid,Symbol,Quantity,Price,Side,Is_cancelled\n";
    for(const Trade& trade:records){
        file << trade.trade_id << ","
             << trade.symbol << "," 
             << trade.quantity << ","
             << trade.price << "," 
             << trade.side << ","
             << trade.is_cancelled << "\n";
    }
    file.close();

    if (file.fail()){
        std::cout << "Saving failed!/n";
        return false;
    }
    return true;

}

bool TradeLedger::load_from_csv(const std::string& filename){

    std::ifstream file(filename);
    
    std::vector<Trade> loaded_trades;

    //if cannot open file 

    if(!file.is_open()){
        std::cout << filename << " cannot be opened/created!";
        return false;
    }

    std::string line;

    //if file is empty
    if(!std::getline(file, line)){
        std::cout << "File is empty!\n";
        return false;
    }

    //if header doesn't match
    if(line != "Tradeid,Symbol,Quantity,Price,Side,Is_cancelled"){
        std::cout << "Invalid header!\n";
        return false;
    }

    std::vector<std::string> trade_ids;

    while(std::getline(file,line)){
        std::string trade_id;
        std::string sy;
        std::string q;
        std::string p;
        std::string si;
        std::string can;
        std::stringstream ss(line);
        int quantity ;
        double price;
        int is_cancelled;
        bool is_cancel;
        
        //if cannot process line;
        if(!(std::getline(ss,trade_id,',') && std::getline(ss, sy, ',') && std::getline(ss, q, ',') && std::getline(ss, p, ',') && std::getline(ss, si, ',') && std::getline(ss, can, ','))){
            std::cout << "Connot process line: " << line << "\n";
            return false;
        }
        std::string extra;
        if(std::getline(ss, extra, ',')){
            std::cout << "Extra columns provided! Ledger cannot be processed\n";
            return false;
        }
        //if trade id is empty or already present
        
        if(trade_id.empty()){
            std::cout << "Trade id is empty! Cannot load \n";
            return false;
        }

        if(std::find(trade_ids.begin(), trade_ids.end(), trade_id) != trade_ids.end()){
            std::cout << "Duplicate trade exists! cannot process the trade! \n";
            return false;
        }

        //if symbol is empty
        if(sy.empty()){
            std::cout << "Symbol is empty! Cannot load \n";
            return false;
        }
        //if side is not buy or sell

        if(si != "BUY" && si != "SELL"){
            std::cout << "Direction " << si << " is not in BUY/SELL\n";
            return false; 
        }

        try {
            std::size_t quantity_pos = 0;
            std::size_t price_pos = 0;
            std::size_t is_cancelled_pos =0;
            quantity = std::stoi(q, &quantity_pos);
            //if quantity is invalid or not being able to process
            if(quantity_pos!=q.size()){
                std::cout << "Quantity " << q << " is invalid!\n";
                return false; 
            }

            if(quantity <= 0 || !std::isfinite(quantity)){
                std::cout << "Quantity:  " << q << " is invalid!\n";
                return false;   
            }
            //if price is invalid or not being able to process

            price = std::stod(p, &price_pos);
            if(price_pos!=p.size()){
                std::cout << "Price " << p << " is invalid!\n";
                return false; 
            }
            if(price <= 0 || !std::isfinite(price)){
                std::cout << "Price:  " << p << " is invalid!\n";
                return false; 
            }
            is_cancelled = std::stoi(can, &is_cancelled_pos);
            
            if(is_cancelled_pos!=can.size()){
                std::cout << "Is cancelled bool " << can << " is invalid!\n";
                return false;
            }
            is_cancel = (is_cancelled == 1);

        }
        catch (const std::exception&) {
            std::cout << "Cannot convert" << q << "or" << p << "to numerical values\n";
            return false;
        }
        loaded_trades.push_back({trade_id, sy, quantity, price, si, is_cancel});
    }
    records = loaded_trades;
    file.close();
    return true;
}
