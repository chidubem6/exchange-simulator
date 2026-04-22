// --- Standard library headers ---
// iostream  : std::cout (print to screen), std::cin (read keyboard input)
// iomanip   : std::fixed and std::setprecision (control how numbers print, e.g. "1.50" not "1.5")
// string    : std::string (text)
// sstream   : std::istringstream (treat a string like a stream so we can extract words from it)
// algorithm : std::transform (used to convert a string to uppercase)
// cctype    : ::toupper (converts a single character to uppercase)
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include "MatchingEngine.h"

// Every order the user places gets a unique ID (1, 2, 3, ...).
// 'static' here means this variable is private to this file — no other file can touch it.
// Lower ID = placed earlier, which the matching engine uses to break ties when two orders
// have the same price (this is called "price-time priority").
static int nextId = 1;

// Converts a string to all uppercase letters.
// e.g. "aapl" -> "AAPL",  "Buy" -> "BUY"
// We pass 's' by value (a copy) so the original string is not changed.
static std::string toUpper(std::string s) {
    // std::transform walks every character in the string and applies ::toupper to it
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

// Prints the list of available commands to the screen.
static void printHelp() {
    std::cout << "Commands:\n"
              << "  BUY <qty> <symbol> @ <price>   -- place a buy order\n"
              << "  SELL <qty> <symbol> @ <price>  -- place a sell order\n"
              << "  CANCEL <order_id>              -- cancel an open order\n"
              << "  BOOK [symbol]                  -- display order book\n"
              << "  QUIT                           -- exit\n";
}

int main() {
    // The OrderBook stores all open buy and sell orders.
    // The MatchingEngine receives new orders and checks if any trades can happen.
    OrderBook book;
    MatchingEngine engine(book);

    std::cout << "Exchange Simulator\n";
    printHelp();
    std::cout << "\n";

    // --- Main loop ---
    // We sit here reading one command at a time until the user types QUIT (or hits Ctrl+D / end-of-file).
    std::string line;
    while (true) {
        std::cout << "> ";                              // print a prompt
        if (!std::getline(std::cin, line)) break;       // read a full line; if input ends, exit the loop

        // --- Trim whitespace ---
        // Find the position of the first non-whitespace character
        auto firstChar = line.find_first_not_of(" \t\r\n");
        if (firstChar == std::string::npos) continue;   // the line was blank — skip it
        // Find the position of the last non-whitespace character
        auto lastChar = line.find_last_not_of(" \t\r\n");
        // Keep only the text between those two positions
        line = line.substr(firstChar, lastChar - firstChar + 1);
        if (line.empty()) continue;

        // --- Parse the first word as the command ---
        std::istringstream ss(line);
        std::string cmd;
        ss >> cmd;              // extract the first word (e.g. "buy", "SELL", "Book")
        cmd = toUpper(cmd);     // normalise to uppercase so matching is case-insensitive

        // --- QUIT / EXIT ---
        if (cmd == "QUIT" || cmd == "EXIT") {
            std::cout << "Goodbye.\n";
            break;  // exit the while loop, ending the program
        }

        // --- HELP ---
        if (cmd == "HELP") {
            printHelp();
            continue;   // go back to the top of the loop and wait for the next command
        }

        // --- BOOK [symbol] ---
        // Shows the current state of the order book.
        // If the user gives a symbol (e.g. "BOOK AAPL"), show only that symbol.
        // If they just type "BOOK", show every symbol.
        if (cmd == "BOOK") {
            std::string sym;
            if (ss >> sym)          // try to read an optional symbol from the rest of the line
                book.display(toUpper(sym));
            else
                book.displayAll();
            continue;
        }

        // --- CANCEL <order_id> ---
        // Removes an open order from the book using its numeric ID.
        if (cmd == "CANCEL") {
            int id;
            if (!(ss >> id)) {      // try to read an integer after CANCEL
                std::cout << "Error: CANCEL requires an order ID. Example: CANCEL 3\n";
                continue;
            }
            std::string sym;
            // cancelOrder returns true if the order was found and removed
            if (book.cancelOrder(id, sym))
                std::cout << "Cancelled order #" << id << " [" << sym << "]\n";
            else
                std::cout << "Error: order #" << id << " not found.\n";
            continue;
        }

        // --- BUY / SELL <qty> <symbol> @ <price> ---
        // Creates a new limit order and sends it to the matching engine.
        if (cmd == "BUY" || cmd == "SELL") {
            int qty;
            std::string symbol, at;     // 'at' will hold the literal "@" token
            double price;

            // Read the remaining tokens from the line: quantity, symbol, "@", price.
            // The condition fails if any token is missing or "@" is not in the right place.
            if (!(ss >> qty >> symbol >> at >> price) || at != "@") {
                std::cout << "Error: expected format: " << cmd << " <qty> <symbol> @ <price>\n";
                continue;
            }

            // Basic validation — reject nonsensical inputs early
            if (qty <= 0)    { std::cout << "Error: quantity must be positive.\n";  continue; }
            if (price <= 0.0){ std::cout << "Error: price must be positive.\n";     continue; }

            // Build the Order object and fill in all its fields
            Order o;
            o.id       = nextId++;                              // assign next available ID, then increment
            o.side     = (cmd == "BUY") ? Side::BUY : Side::SELL;
            o.symbol   = toUpper(symbol);   // normalize so "aapl" and "AAPL" go to the same book
            o.price    = price;
            o.quantity = qty;

            // Confirm the order to the user, then hand it to the engine
            // std::fixed + std::setprecision(2) makes the price always print with 2 decimal places
            std::cout << "Order #" << o.id << " accepted: " << cmd << " " << qty
                      << " " << o.symbol << " @ " << std::fixed << std::setprecision(2) << price << "\n";
            engine.processOrder(o);     // this may immediately print a TRADE if orders match
            continue;
        }

        // If we get here, the command was not recognised
        std::cout << "Unknown command: " << cmd << ". Type HELP for usage.\n";
    }

    return 0;  // 0 means the program finished successfully
}
