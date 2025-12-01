#pragma once
#include "PriceBar.h"
#include "Utility.h"
#include <vector>
#include <string>
#include <sqlite3.h>

class Engine {
    public:
    void loadData(const std::string& filePath);
    void loadData(sqlite3* &dbConnection);
    void dbConnectionSetup(const std::string& dbFile);
    void run();
    void runReal();
    ~Engine();
    Engine();

    sqlite3* dbConnection = nullptr;

    private:
    std::vector<PriceBar> data;
    const std::string balanceFileLocation = (std::filesystem::current_path() / ".." / "data" / "account_balance.txt").string();
    moneyHandle moneyhandle;
    double cash = 10000.0;
    int position = 0;

    double buy(double price);
    double sell(double price);
};
