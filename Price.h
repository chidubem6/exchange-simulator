#ifndef PRICE_H
#define PRICE_H

#include <cctype>
#include <string>

// Prices are stored as an integer number of ticks so that every comparison and
// arithmetic operation inside the book and matching engine is exact. Floating
// point is never used for a price after this boundary, which removes the need
// for epsilon tolerances when deciding whether two prices are equal.
//
// One tick = 0.01 price units, so 185.50 is stored as 18550 ticks.
inline constexpr long long TICKS_PER_UNIT = 100;

// Parses a decimal price string (e.g. "185.50", "42", "7.5") into ticks without
// using floating point, so there is no rounding error. Returns false if the
// string is not a valid, non-negative price with at most two decimal places.
inline bool parsePriceToTicks(const std::string& text, long long& outTicks) {
    if (text.empty()) return false;

    const std::size_t dot = text.find('.');
    const std::string intPart = (dot == std::string::npos) ? text : text.substr(0, dot);
    std::string fracPart = (dot == std::string::npos) ? "" : text.substr(dot + 1);

    // "" or ".50" have no integer part; a fraction longer than two digits is
    // finer than one tick; a very long integer part risks long long overflow.
    if (intPart.empty()) return false;
    if (fracPart.size() > 2) return false;
    if (intPart.size() > 15) return false;

    // A second '.' would still be inside fracPart (find() returns the first).
    if (fracPart.find('.') != std::string::npos) return false;

    for (char c : intPart) {
        if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    }
    for (char c : fracPart) {
        if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    }

    // Pad the fraction to exactly two digits: "5" -> "50", "" -> "00".
    while (fracPart.size() < 2) fracPart.push_back('0');

    long long units = 0;
    for (char c : intPart) units = units * 10 + (c - '0');

    const long long fracTicks = (fracPart[0] - '0') * 10 + (fracPart[1] - '0');
    outTicks = units * TICKS_PER_UNIT + fracTicks;
    return true;
}

// Formats ticks back into a fixed two-decimal string, e.g. 18550 -> "185.50".
inline std::string formatTicks(long long ticks) {
    const long long units = ticks / TICKS_PER_UNIT;
    long long frac = ticks % TICKS_PER_UNIT;
    if (frac < 0) frac = -frac;

    std::string fracStr = std::to_string(frac);
    if (fracStr.size() < 2) fracStr = "0" + fracStr;  // 5 -> "05"

    return std::to_string(units) + "." + fracStr;
}

#endif
