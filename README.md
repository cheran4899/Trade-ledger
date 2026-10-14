# Trade Ledger

A C++ trade ledger and portfolio engine that records trades, persists trade history to CSV, and reconstructs portfolio positions and realised profit and loss (P&L) by replaying trade records.

The project explores core C++ concepts through a financial application, including structs, classes, STL containers, file handling, validation, and state management.

## Features

- **Trade recording:** Store trades with unique trade IDs, symbols, sides, quantities, and prices.
- **Duplicate ID detection:** Prevent the same trade ID from being recorded twice.
- **Portfolio management:** Maintain positions and calculate average prices and realised P&L.
- **Trade correction:** Correct an existing trade and replay the ledger to reconstruct the portfolio.
- **Trade cancellation:** Retain cancelled trades in the ledger for historical traceability while excluding them from portfolio replay.
- **CSV persistence:** Save trade history to CSV and load it back into the application.
- **Portfolio replay:** Rebuild portfolio state from the recorded trade history.
- **External coordination:** Stage ledger changes and replay them into a separate portfolio before committing the proposed in-memory state.

## Design

### Trade Ledger

The ledger owns the trade history. Each trade has a unique identifier, allowing the application to identify individual records for correction and cancellation.

### Portfolio and positions

The portfolio processes valid trades and maintains positions by symbol. Each position tracks its quantity, average price, and realised P&L.

### Replay and coordination

Instead of directly modifying the active portfolio during a correction or cancellation, the coordinator works with a proposed ledger and a fresh portfolio.

The workflow is:

1. Create a copy of the current ledger.
2. Apply the proposed change to the copy.
3. Replay the proposed ledger into a fresh portfolio, skipping cancelled trades.
4. Reject the change if replay fails.
5. Replace the active in-memory ledger and portfolio only after replay succeeds.

This approach helps keep the in-memory ledger and portfolio consistent when a proposed change cannot be replayed successfully.

**Persistence limitation:** the in-memory coordination is not a fully atomic transaction with the CSV file. A process interruption or file-writing failure can still leave persisted data out of sync with in-memory state.

## Requirements

- C++17-compatible compiler
- CMake 3.16 or newer
- A supported build tool, such as Make or Ninja

## Build

From the project root:

```bash

cmake -S . -B build

cmake --build build

./build/trade_ledger

```

This configures the project and builds the `trade_ledger` executable.

## Run

On Linux:

```bash
./build/trade_ledger
```

The application provides a menu for recording trades, viewing trade history and portfolio positions, correcting or cancelling trades, saving and loading CSV data, and rebuilding the portfolio.

## CSV data

The ledger persists trade records to CSV. The schema should match the implementation and include the cancellation status, for example:

```csv
TradeID,Symbol,Quantity,Price,Side,Cancelled
T001,AAPL,100,150.00,BUY,0
T002,AAPL,40,160.00,SELL,0
T003,MSFT,50,420.00,BUY,1
```

The final column indicates whether a trade is cancelled. The example values are illustrative; use the exact Boolean representation supported by your CSV implementation.

## Project goals

This project is part of a broader effort to develop practical C++ skills through financial software, with an emphasis on data structures, trade processing, P&L calculations, and reliable state reconstruction.
