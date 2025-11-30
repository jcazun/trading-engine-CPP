#include "Strategy.h"

/// @brief This strategy is used for comparing short-term and long-term moving averages to make buy/sell decisions.
/// @param bar  The latest price bar data.
void Strategy::onBar(const PriceBar& bar) {
    history.push_back(bar.close);
    if (history.size() > 20) {
        history.pop_front(); // keep this at a fixed length of 20 days
    }

    if (history.size() >= 5) { // have at least 5 price data points
        // Calculate the short-term and long-term moving averages
        double shortSum = 0, longSum = 0;

        for (size_t i = history.size() - 5; i < history.size(); ++i) { // run the last 5 days for the short
            shortSum += history[i];
        }

        for (double val : history) // similar to python syntax for iterating over each element in the array, and assigning it to val
        {
            longSum += val;
        }

        shortMA = shortSum / 5.0;
        longMA = longSum / history.size();
    }
}

// MA stands for Moving Average - average stock price over time
// shortMA is the average of the last 5 closing prices (e.g., 5 days)
// longMA is the moving average over a longer period (e.g., 20 days or historical)

// shortMA reacts faster to price changes
// longMA reacts slower to price changes

/// @brief Determines if the strategy should buy or sell based on moving averages. 
/// @return true if the short-term trend is going up faster than the longer-term trend AND
/// you are not holding that stock.   
bool Strategy::shouldBuy() const {
    return shortMA > longMA && !holding;
}

/// @brief Determines if the strategy should sell the stock based on the moving averages.
/// @return true if the short-term trend is going down faster than the longer-term trend AND
/// you are currently holding the stock.
bool Strategy::shouldSell() const {
    return shortMA < longMA && holding;
}