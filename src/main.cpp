
#include <iostream>
#include <string>
#include <limits>
#include <utility>
#include <cctype>
#include <bits/stdc++.h>

#include "trade.h"
#include "trade_ledger.h"
#include "portfolio.h"

// ==================================================
// External coordinator: replay ledger into portfolio
// ==================================================

bool replay_ledger(
    const TradeLedger& ledger,
    Portfolio& rebuilt_portfolio
) {
    for (const Trade& trade : ledger.get_records()) {
        if (trade.is_cancelled) {
            continue;
        }

        if (!rebuilt_portfolio.process_trade(trade)) {
            std::cout << "Replay failed for trade ID: "
                      << trade.trade_id << '\n';
            return false;
        }
    }

    return true;
}

// ==================================================
// External coordinator: add trade
// ==================================================

bool add_trade(
    TradeLedger& active_ledger,
    Portfolio& active_portfolio,
    const Trade& new_trade,
    const std::string& portfolio_name
) {
    if (active_ledger.has_trade(new_trade.trade_id)) {
        std::cout << "Trade ID already exists.\n";
        return false;
    }

    TradeLedger proposed_ledger = active_ledger;
    proposed_ledger.record_trade(new_trade);

    Portfolio proposed_portfolio(portfolio_name);

    if (!replay_ledger(proposed_ledger, proposed_portfolio)) {
        std::cout << "Trade rejected. Active state unchanged.\n";
        return false;
    }

    active_ledger = std::move(proposed_ledger);
    active_portfolio = std::move(proposed_portfolio);

    std::cout << "Trade recorded successfully.\n";
    return true;
}

// ==================================================
// External coordinator: correct trade
// ==================================================

bool correct_and_replay(
    TradeLedger& active_ledger,
    Portfolio& active_portfolio,
    const std::string& trade_id,
    const Trade& corrected_trade,
    const std::string& portfolio_name
) {
    TradeLedger proposed_ledger = active_ledger;

    // Apply correction to the copy, never the active ledger.
    if (!proposed_ledger.correct_trade(trade_id, corrected_trade)) {
        std::cout << "Could not prepare trade correction.\n";
        return false;
    }

    Portfolio proposed_portfolio(portfolio_name);

    if (!replay_ledger(proposed_ledger, proposed_portfolio)) {
        std::cout << "Correction rejected. Active state unchanged.\n";
        return false;
    }

    active_ledger = std::move(proposed_ledger);
    active_portfolio = std::move(proposed_portfolio);

    std::cout << "Trade corrected successfully.\n";
    return true;
}

// ==================================================
// External coordinator: cancel trade
// ==================================================

bool cancel_and_replay(
    TradeLedger& active_ledger,
    Portfolio& active_portfolio,
    const std::string& trade_id,
    const std::string& portfolio_name
) {
    TradeLedger proposed_ledger = active_ledger;

    // This method must mark the trade cancelled in the ledger.
    if (!proposed_ledger.cancel_trade(trade_id)) {
        std::cout << "Could not cancel trade.\n";
        return false;
    }

    Portfolio proposed_portfolio(portfolio_name);

    if (!replay_ledger(proposed_ledger, proposed_portfolio)) {
        std::cout << "Cancellation rejected. Active state unchanged.\n";
        return false;
    }

    active_ledger = std::move(proposed_ledger);
    active_portfolio = std::move(proposed_portfolio);

    std::cout << "Trade cancelled successfully.\n";
    return true;
}

// ==================================================
// Input helper
// ==================================================

bool read_trade_fields(Trade& trade) {
    std::cout << "Symbol: ";
    std::getline(std::cin, trade.symbol);

    std::cout << "Side (BUY/SELL): ";
    std::getline(std::cin, trade.side);

    for (char& c : trade.side) {
        c = static_cast<char>(
            std::toupper(static_cast<unsigned char>(c))
        );
    }

    std::cout << "Quantity: ";
    if (!(std::cin >> trade.quantity)) {
        std::cout << "Invalid quantity.\n";
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n'
        );
        return false;
    }

    std::cout << "Price: ";
    if (!(std::cin >> trade.price)) {
        std::cout << "Invalid price.\n";
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n'
        );
        return false;
    }

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(), '\n'
    );

    if (trade.symbol.empty() ||
        trade.quantity <= 0 ||
        trade.price <= 0 ||
        !std::isfinite(trade.price) ||
        (trade.side != "BUY" && trade.side != "SELL")) {
        std::cout << "Invalid trade details.\n";
        return false;
    }

    return true;
}

// ==================================================
// Main
// ==================================================

int main() {
    const std::string portfolio_name = "Main Portfolio";

    TradeLedger ledger;
    Portfolio portfolio(portfolio_name);

    bool running = true;

    while (running) {
        std::cout << "\n========== TRADE LEDGER ==========\n"
                  << "1. Record trade\n"
                  << "2. Show trade history\n"
                  << "3. Show portfolio\n"
                  << "4. Correct trade\n"
                  << "5. Cancel trade\n"
                  << "6. Save ledger to CSV\n"
                  << "7. Load ledger from CSV\n"
                  << "8. Rebuild portfolio from ledger\n"
                  << "9. Exit\n"
                  << "Select option: ";

        int choice;

        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Enter a number.\n";
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            continue;
        }

        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n'
        );

        switch (choice) {
            case 1: {
                Trade trade{};

                std::cout << "Trade ID: ";
                std::getline(std::cin, trade.trade_id);

                if (!read_trade_fields(trade)) {
                    break;
                }

                trade.is_cancelled = false;

                add_trade(
                    ledger, portfolio, trade, portfolio_name
                );
                break;
            }

            case 2: {
                ledger.show_trades();
                break;
            }

            case 3: {
                portfolio.show_all_positions();
                break;
            }

            case 4: {
                std::string trade_id;

                std::cout << "Trade ID to correct: ";
                std::getline(std::cin, trade_id);

                const Trade* original = nullptr;

                for (const Trade& trade : ledger.get_records()) {
                    if (trade.trade_id == trade_id) {
                        original = &trade;
                        break;
                    }
                }

                if (original == nullptr) {
                    std::cout << "Trade ID not found.\n";
                    break;
                }

                if (original->is_cancelled) {
                    std::cout << "Cannot correct a cancelled trade.\n";
                    break;
                }

                Trade corrected_trade = *original;

                std::cout << "Enter corrected trade details:\n";

                if (!read_trade_fields(corrected_trade)) {
                    break;
                }

                // Preserve the original ID and cancellation status.
                corrected_trade.trade_id = trade_id;
                corrected_trade.is_cancelled = false;

                correct_and_replay(
                    ledger,
                    portfolio,
                    trade_id,
                    corrected_trade,
                    portfolio_name
                );
                break;
            }

            case 5: {
                std::string trade_id;

                std::cout << "Trade ID to cancel: ";
                std::getline(std::cin, trade_id);

                cancel_and_replay(
                    ledger, portfolio, trade_id, portfolio_name
                );
                break;
            }

            case 6: {
                if (ledger.save_to_csv("trades.csv")) {
                    std::cout << "Ledger saved successfully.\n";
                } else {
                    std::cout << "Failed to save ledger.\n";
                }
                break;
            }

            case 7: {
                TradeLedger proposed_ledger;

                if (!proposed_ledger.load_from_csv("trades.csv")) {
                    std::cout << "Failed to load ledger. "
                              << "Active state unchanged.\n";
                    break;
                }

                Portfolio proposed_portfolio(portfolio_name);

                if (!replay_ledger(
                        proposed_ledger, proposed_portfolio)) {
                    std::cout << "Replay failed. "
                              << "Active state unchanged.\n";
                    break;
                }

                ledger = std::move(proposed_ledger);
                portfolio = std::move(proposed_portfolio);

                std::cout << "Ledger loaded and portfolio rebuilt.\n";
                break;
            }

            case 8: {
                Portfolio proposed_portfolio(portfolio_name);

                if (!replay_ledger(ledger, proposed_portfolio)) {
                    std::cout << "Rebuild failed. "
                              << "Active portfolio unchanged.\n";
                    break;
                }

                portfolio = std::move(proposed_portfolio);
                std::cout << "Portfolio rebuilt successfully.\n";
                break;
            }

            case 9: {
                running = false;
                std::cout << "Exiting Trade Ledger.\n";
                break;
            }

            default:
                std::cout << "Invalid option. Choose 1-9.\n";
                break;
        }
    }

    return 0;
}
