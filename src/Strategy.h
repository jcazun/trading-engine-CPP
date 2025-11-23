#pragma once
#include "PriceBar.h"
#include <deque>

class Strategy {
    public:
    void onBar(const PriceBar& bar);
    bool shouldBuy() const;
    bool shouldSell() const;

    private:
    std::deque<double> history;
    double shortMA = 0.0; 
    double longMA  = 0.0;
    bool holding = false;
};