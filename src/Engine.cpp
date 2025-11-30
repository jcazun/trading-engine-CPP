#include "Engine.h"
#include "Strategy.h"
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
    std::vector<PriceBar> data;
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

    PriceBar bar;
    std::vector<PriceBar> data;
    const char * sql = "SELECT date, open, high, low, close, volume FROM MARKET_DATA;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(dbConnection, sql, -1, &stmt, nullptr);
    if (rc != 0) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(dbConnection) << std::endl;
        return;
    }

    // sqlite3_step executes one prepared statement, aka one row of data.
    // we are checking if the return code is SQLITE_ROW, meaning there is data to be read
    while((rc == sqlite3_step(stmt)) == SQLITE_ROW)
    {
            bar.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            bar.open = sqlite3_column_double(stmt, 1);
            bar.high = sqlite3_column_double(stmt, 2);
            bar.low = sqlite3_column_double(stmt, 3);
            bar.close = sqlite3_column_double(stmt, 4);
            bar.volume = sqlite3_column_double(stmt, 5);
            data.push_back(bar);
    }
    std::cout<< "Data loaded from database successfully!\nNow executing trading engine..." <<std::endl;
    sqlite3_finalize(stmt);
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

void Engine::buy(double price) {
    if (position == 0) {
        position =1;
        cash -= price;
        std::cout << "BUY @ " << price << "\n";
    }
}

void Engine::sell(double price) {
    if (position == 1) {
        position = 0;
        cash += price;
        std::cout << "SELL @ " << price << "\n";
    }
}
