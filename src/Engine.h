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
    bool getTerminateEngine() const { return terminateEngine.load(); }
    bool setTerminateEngine(bool value) { terminateEngine.store(value); return terminateEngine.load(); }
    ~Engine();
    Engine();

    sqlite3* dbConnection = nullptr;

    private:
    std::vector<PriceBar> data;
    const std::string balanceFileLocation = (std::filesystem::current_path() / ".." / "data" / "account_balance.txt").string();
    moneyHandle moneyhandle;
    double cash = 10000.0;
    int position = 0;
    double maxBalance = 0.0; // max balance achieved during trading session
    double minBalance = 1000000.0; // min balance achieved during trading session
    const u_int profitThresholdPercent = 2; // 5% profit threshold
    const int lossThresholdPercent = -2;    // 5% loss threshold

    std::atomic<bool> terminateEngine = false;
    bool getBalanceRatio(double currentBalance);    // check if either we hit our stop loss or take profit limits
    double buy(double price);
    double sell(double price);
};
