#include "Engine.h"
#include "Strategy.h"
#include "Utility.h"
#include <fstream>
#include <sstream>
#include <iostream>

Engine::~Engine() 
    {
        std::cout << "Engine destructor called, closing database connection if open." << std::endl;
        if(dbConnection != nullptr) {
            std::cout << "Closing database connection at "<< dbConnection << std::endl;
            sqlite3_close(dbConnection);
            dbConnection = nullptr;
            std::cout << "Database connection closed." << std::endl;
        }
    }   

Engine::Engine() :
    moneyhandle(balanceFileLocation) // initialize moneyhandle with balance file location
    {
        // show user what they are starting off with before executing strategy
        double initialBalance = moneyhandle.getCurrentBalance();
        maxBalance = initialBalance;
        minBalance = initialBalance;
        std::cout << "Engine initialized with account balance: " << initialBalance << std::endl;
    } 

void Engine::dbConnectionSetup(const std::string& dbFile) {
    // Implementation for setting up the database connection
    int rc = sqlite3_open(dbFile.c_str(), &dbConnection);
    if (rc == 0) {
        std::cout << "Database connection established successfully." << std::endl;
    } else {
        std::cerr << "Failed to connect to database: " << sqlite3_errmsg(dbConnection) << std::endl;
        dbConnection = nullptr;
    }
}

void Engine::loadData(const std::string& filepath) { 
    std::ifstream file(filepath);
    std::string line;
    getline(file, line); // Skip header line
    while (getline(file, line)) { // loop through each line in file each time its called
        std::stringstream ss(line);  // ss is a new string object created from line (copy) - ss is used for parsing a string
        PriceBar bar;
        std::string val; 

        getline(ss, bar.date, ','); // seperate each item by comma, reads up to first comma
        ss >> bar.open; ss.ignore(); // >> extration operator, continues extracting into data that can be converted into a double (ex. 102.5) until it hits something it can'tconvert like a comma. we ignore that char then keep it pushing
        ss >> bar.high; ss.ignore(); // >> extract next set of double convertable data until we find comma (to ignore)
        ss >> bar.low;  ss.ignore(); // >> extract next set of double convertable data until we find comma (to ignore)
        ss >> bar.close;ss.ignore(); // >> extract next set of double convertable data until we find comma (to ignore)
        ss >> bar.volume;               // >> extract next set of double convertable data, should be last bit of data that does NOT have a comma at the end to ignore
        data.push_back(bar); // add parsed bar to data ( a list )
    }
}

/// @brief  pass in the existing database connection to load data from the database. must be used AFTER dbConnectionSetup is called.
/// @param dbConnection 
void Engine::loadData(sqlite3* &dbConnection) {
    // Implementation for loading data from SQLite database
    // This function would use the dbConnection to query the database
    // and populate the data vector with PriceBar objects.
    // For brevity, the implementation details are omitted here.
    if (dbConnection == nullptr) {
        std::cerr << "Database connection is null.\nWas dbConnectionSetup ran before calling this method?" << std::endl;
        return;
    }

    const char * sql =
    "SELECT timestamp, open, high, low, close, volume "
    "FROM prices "
    "WHERE symbol = 'AAPL';";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(dbConnection, sql, -1, &stmt, nullptr);
    if (rc != 0) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(dbConnection) << std::endl;
        return;
    }

    // sqlite3_step executes one prepared statement, aka one row of data.
    // we are checking if the return code is SQLITE_ROW, meaning there is data to be read
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW)     // other keyword is SQLITE_DONE, meaning no more data to be read
    {       // fill up data members from PriceBar into a PriceBar vector called data, then push back all data into data vector
            PriceBar bar;
            bar.timestamp = sqlite3_column_int(stmt, 0);
            bar.open =      sqlite3_column_double(stmt, 1);
            bar.high =      sqlite3_column_double(stmt, 2);
            bar.low =       sqlite3_column_double(stmt, 3);
            bar.close =     sqlite3_column_double(stmt, 4);
            bar.volume =    sqlite3_column_double(stmt, 5);
            data.push_back(bar);

            // DEBUG PRINT!
            /*
            std::cout << "DEBUG PRINT "
                      << bar.timestamp << ", "
                      << bar.open << ", "
                      << bar.high << ", "
                      << bar.low << ", "
                      << bar.close << ", "
                      << bar.volume << std::endl;
                      */
    }
    std::cout<< "Data loaded from database successfully!\nNow executing trading engine..." <<std::endl;
    sqlite3_finalize(stmt);
}

void Engine::runReal() {
    // Implementation for running the engine in real-time mode
    // This function would handle real-time data processing and trading logic.
    // For brevity, the implementation details are omitted here.

    Strategy strat;
    for(const auto& bar: data)
    {
        //std::cout << "debug print statement for runReal! Data is available!" << std::endl;
        strat.onBar(bar);
        if(strat.shouldBuy())
        {
            std::cout << "Real-time BUY signal at price: " << bar.close << std::endl;
            buy(bar.close);
            double currentBalance = moneyhandle.getCurrentBalance();
            bool stopLimitHit = getBalanceRatio(currentBalance); // check if we hit either our stop loss or take profit limits
        }
        else if(strat.shouldSell())
        {
            std::cout << "Real-time SELL signal at price: " << bar.close << std::endl;
            sell(bar.close);
            double currentBalance = moneyhandle.getCurrentBalance();
            bool stopLimitHit = getBalanceRatio(currentBalance); // check if we hit either our stop loss or take profit limits
        }
    }
}

void Engine::run() {
    Strategy strat;
    for (const auto& bar : data) { // read only of each bar object in data list. auto deduces the type
        strat.onBar(bar);
        if (strat.shouldBuy()) {
            buy(bar.close);
        } 
        else if (strat.shouldSell()) {
            sell(bar.close);
        }
    }
}

double Engine::buy(double price) {
    // for now im ignoring position check to allow multiple buys
    //if (position == 0) {
    //    position =1;
        cash -= price;
        if(cash > maxBalance) {
            maxBalance = cash;
        }
        std::cout << "BUY @ " << price << "\n";
        moneyhandle.updateBalance(-price);
        return cash;
    //}
    //else 
    //{
    //    return cash; // no action taken, return current cash
    //}
}

double Engine::sell(double price) {
    // for now im ignoring position check to allow multiple sells
    //if (position == 1) {
    //    position = 0;
        cash += price;
        std::cout << "SELL @ " << price << "\n";
        moneyhandle.updateBalance(price);
        return cash;
    //}
    //else 
    //{
    //    return cash; // no action taken, return current cash
    //}


}

bool Engine::getBalanceRatio(double currentBalance) {
    double balanceChange = currentBalance - minBalance;
    double balanceChangePercent = (balanceChange / minBalance) * 100.0;

    if (balanceChangePercent >= profitThresholdPercent) {
        std::cout << "Take Profit Triggered! Balance increased by " << balanceChangePercent << "%\n";
        setTerminateEngine(true);//terminateEngine = true;
        return true;
    } 
    else if (balanceChangePercent <= lossThresholdPercent) {
        std::cout << "Stop Loss Triggered! Balance decreased by " << balanceChangePercent << "%\n";
        setTerminateEngine(true);//terminateEngine = true;
        return true;
    }
    return false; // no thresholds hit
}
