#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <sqlite3.h>

/// Interface for managing user money/account balance.
class moneyHandle
{
    // start with balance in text file, then eventually move onto a database
    public:
    moneyHandle(const std::string initFileLocation) :   balance(0.0), 
                                                        balanceFileLocation (initFileLocation){}
    ~moneyHandle() {}
    double getCurrentBalance();
    void updateBalance(double delta);
    void createAccount(double initialBalance);

    private:
    double balance;
    const std::string balanceFileLocation;

};

/// Interface for managing the file that holds our market tracking information.
class fileHandle
{
    // eventually need to change name to dbHandle since we are moving to a database instead of fileHandle
    public:
    fileHandle(const std::string& fileName = "market_data.db");
    bool fileExists(const std::string& fileLocation);
    void fileCreate();
    void fileRead(const std::string& fileLocation);
    void dataBaseRead(const std::string& dbLocation);
    void fileWrite(std::string& fileLocation, std::string data);
    ~fileHandle();

    private:
    std::string fileName = "market_data.db";
    std::string filePath;
    std::string fileLocation = "market_data.db";
    sqlite3* s3dB = nullptr; // member variable to hold sqlite3 database connection, usable by all class methods
};

/// Interface for managing python ingestion process.
class pythonHandle
{
    public:
    void dataIngestionStartup(const std::string& pythonExePath);
    bool dataIngestionStop(pid_t& pyPID);

    pythonHandle()
    {
        std::cout<<"Default CONSTRUCTOR called!"<<std::endl <<std::flush;
    }
    ~pythonHandle(){
        std::cout<<"Default DESTRUCTOR called!"<<std::endl;
    }

    private:
    bool programHeartbeatCheck(pid_t& pyID);
};