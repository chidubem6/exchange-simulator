# Exchange Simulator

A simple C++ command-line exchange simulator with a limit order book and matching engine.

The project lets you place `BUY` and `SELL` orders, cancel open orders by ID, inspect the book for one or more symbols, and watch trades execute when bid and ask prices cross.

## Features

- Limit order book for multiple symbols such as `AAPL` or `TSLA`
- Price-time priority matching
- Partial fills and full fills
- Order cancellation by order ID
- Aggregated book display by price level
- Interactive CLI for submitting and viewing orders

## How It Works

Orders are stored separately as bids and asks for each symbol:

- Bids are sorted highest price first
- Asks are sorted lowest price first
- Orders at the same price are matched in arrival order

When a new order is submitted:

1. It is added to the order book
2. The matching engine checks the best bid and best ask for that symbol
3. If `best bid >= best ask`, a trade is executed
4. The traded quantity is the smaller of the two order sizes
5. Remaining quantity stays on the book unless the order is fully filled

Trade price is determined by the resting order using the project's current time-priority rule.

## Project Structure

```text
main.cpp              Interactive command-line interface
Order.h               Order model and side enum
OrderBook.h/.cpp      Order storage, lookup, cancel, and display logic
MatchingEngine.h/.cpp Matching and trade execution
```

## Commands

```text
BUY <qty> <symbol> @ <price>
SELL <qty> <symbol> @ <price>
CANCEL <order_id>
BOOK [symbol]
HELP
QUIT
```

Examples:

```text
BUY 100 AAPL @ 185.50
SELL 40 AAPL @ 185.25
BOOK AAPL
CANCEL 1
QUIT
```

## Example Session

```text
Exchange Simulator
> BUY 100 AAPL @ 185.50
Order #1 accepted: BUY 100 AAPL @ 185.50
> SELL 40 AAPL @ 185.25
Order #2 accepted: SELL 40 AAPL @ 185.25
TRADE: 40 AAPL @ 185.50
> BOOK AAPL
Order Book [AAPL]:
  ASKS: (empty)
  BIDS: 185.50 (60)
```

## Building

### Option 1: Visual Studio / `cl.exe`

This repo already includes a VS Code build task that compiles all `.cpp` files into `bin\` and names the executable after the active file in VS Code.

Manual command:

```powershell
cl.exe /Zi /EHsc /nologo /Febin\exchange-simulator.exe /Fdbin\ /Fobin\ *.cpp
```

### Option 2: `g++`

```bash
g++ -std=c++17 -O2 -Wall -Wextra -o exchange-simulator main.cpp OrderBook.cpp MatchingEngine.cpp
```

## Running

With the manual MSVC build command above:

```powershell
.\bin\exchange-simulator.exe
```

If you build through the existing VS Code task while `main.cpp` is the active file:

```powershell
.\bin\main.exe
```

With the `g++` build:

```bash
./exchange-simulator
```

## Notes

- Symbols are normalized to uppercase
- Order IDs are assigned sequentially starting from `1`
- `BOOK` with no symbol prints every symbol currently in the book
- Quantities and prices must be positive

## Possible Next Steps

- Add market orders
- Add unit tests
- Persist trades and order history
- Support amend/replace order operations
- Track last traded price and volume
