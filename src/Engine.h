#pragma once
#include "PriceBar.h"
#include <vector>
#include <string>
#include <sqlite3.h>

class Engine {
    public:
    void loadData(const std::string& filePath);
    void loadData(sqlite3* &dbConnection);
    void dbConnectionSetup(const std::string& dbFile);
    void run();
    ~Engine();
    sqlite3* dbConnection = nullptr;

    private:
    std::vector<PriceBar> data;
    double cash = 10000.0;
    int position = 0;

    void buy(double price);
    void sell(double price);
};
