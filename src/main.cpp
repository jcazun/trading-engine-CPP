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

void backgroundTask2(Engine* engine)
{
    while(!engine->getTerminateEngine())
    {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    std::cout << "Engine termination signal recieved. Either stop loss or take profit limit hit. Stopping engine..." << std::endl;
    std::exit(1); // exit entire program
}

// convert this to a thread
void dataIngestion(const std::string& pythonExePath)
{
    // use filesystem to get current path
    namespace fs = std::filesystem;
    fs::path projectPath = fs::current_path().parent_path();
    fs::path fullPath = projectPath / "src"/ "TradingHandle" / "MarketInterface.py";
    std::cout << "Starting Python data ingestion process at: " + fullPath.string() << std::endl;
    std::string cmd = "python3 " + fullPath.string();
    // didnt even know you could 
    int result = std::system(cmd.c_str());
    if (result == -1) {
        std::cerr << "Error starting Python process." << std::endl;
    } else {
        std::cout << "Ingesting data..." << std::endl;
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
//moneyHandle     moneyhandle;        // money/account balance handler - trying to get this managed in engine.cpp now

thread worker(backgroundTask2, &engine); // create a thread that runs backgroundTask2 and pass in engine pointer
thread dataIngester(dataIngestion, pythonExePath); // create a thread that runs dataIngestion and pass in pythonExePath
//worker.detach(); // detach the thread so it runs independently- use when you dont need to wait for this guy to finish
pythonhandle.dataIngestionStartup(pythonExePath); // path to python executable
engine.dbConnectionSetup(dbFilePath); // assign the database connection from fileHandle to Engine
engine.loadData(engine.dbConnection); // load data from database connection - filled in member variable via dbConnection Setup
engine.runReal(); // run the real-time trading engine

worker.join(); // wait for the background thread to finish - use when you want this guy to finish before continuing

return 0;
}
