#pragma once
#include "PriceBar.h"
#include <vector>
#include <string>
#include <sqlite3.h>

class Engine {
    public:
    void loadData(const std::string& filePath);
    void run();

    private:
    std::vector<PriceBar> data;
    double cash = 10000.0;
    int position = 0;

    void buy(double price);
    void sell(double price);
};
