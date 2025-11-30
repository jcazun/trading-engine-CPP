#include <iostream>
#include <thread>
#include <chrono>
#include "Engine.h"
#include "Strategy.h"
#include "Utility.h"
using namespace std;

void backgroundTask()
{
    for (int i = 0; i < 5; i++)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2)); // simulate some work
        std::cout << "Background task running..." << i+1 << std::endl;
    }
}

void backgroundTask2()
{
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "Heartbeat check from worker thread!" << std::endl;
    }
}

int main() {
    const std::string balanceFileLocation = (std::filesystem::current_path() / ".." / "data" / "account_balance.txt").string();
    // must call join or detach everytime a thread is called.
    // detach lets the thread run independently (meaning we do not halt the execution for it to complete)
    // and join waits for the thread to finish if not done by the time we call join
//thread worker(backgroundTask); // create a thread that runs backgroundTask
//thread worker2(backgroundTask2); // create a thread that runs backgroundTask2
//worker.detach(); // detach the thread so it runs independently- use when you dont need to wait for this guy to finish
const std::string pythonExePath = "../TradingHandle/"; // path to python executable -> go up a directory then into TradingHandle folder
const std::string dbFilePath = "../market_data.db"; // path to database file
// Main thread continues to run the engine
std::cout << "Starting trading engine..." << std::endl;
Engine          engine;             // trading engine handler - handles strategy and execution
fileHandle      filehandle;         // only necessary for simulations. this is txt based rather than db based
pythonHandle    pythonhandle;       // python data ingestion handler
moneyHandle     moneyhandle;        // money/account balance handler - might swap to engine class later to handle money in db

engine.loadData("data/prices.csv");
pythonhandle.dataIngestionStartup(pythonExePath); // path to python executable
engine.dbConnectionSetup(dbFilePath); // assign the database connection from fileHandle to Engine
//engine.loadData('market_data.db'); // load data from database connection
moneyhandle.updateBalance(balanceFileLocation, 150000.00); // create account with initial balance of 10,000.00
moneyhandle.getCurrentBalance(balanceFileLocation);
engine.run();
//worker2.join(); // wait for the user input thread to finish - use when you want this guy to finish before continuing
//worker.join(); // wait for the background thread to finish - use when you want this guy to finish before continuing

return 0;
}
