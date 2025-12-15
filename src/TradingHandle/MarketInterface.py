import yfinance as yf
import pandas as pd
import datetime
import sqlite3 as sql3
import os, sys, time
from pathlib import Path



class fileHandle: 
    def writeDataToFile():
        trackingKeys = [("AAPL", "RTX"), ("MSFT", "IBM")]
        # get current location of .py file, then go up 3 directories to reach /data folder
        here = Path(__file__).resolve()
        dataDir = here.parent.parent.parent / "data"
        formatAsterisk = fileHandle.formatUtility()
        dataCapture = yf.Ticker(trackingKeys[0][0]).history(period="1d", interval="1m")
        print("Writing data to: ", dataDir / "MarketData.txt", "\n")
        try:
           f = open(dataDir / "MarketData.txt", "w")
           f.write(formatAsterisk)
           f.write(trackingKeys[0][0] + " Market Data:\n")
           f.write(str(dataCapture))
           f.write("\n" + formatAsterisk)
           f.close()
        except Exception as e:
            print("Error writing to file: ", e)
            
    def __init__(self, db_path: str = 'market_data.db'):
        self.dbIO = sql3.connect(db_path)
    
    def ingestMarketData(self) -> pd.DataFrame: # eventually, return market data
        print("Ingesting market data...\n")
        dataCapture = yf.Ticker("AAPL").history(period="1d", interval="2m") # using this for testing purposes
        print(dataCapture)
        return dataCapture
                
    @staticmethod
    def formatUtility() -> str :
        returnString = ""
        for i in range(106):
            returnString += "*"
        return returnString + "\n"
    
    def lastCapturedTimestamp(self, 
                              symbol: str) -> int: 
        query = "SELECT MAX(timestamp) FROM prices WHERE symbol = ?"
        cursor = self.dbIO.cursor()
        cursor.execute(query, (symbol,))
        result = cursor.fetchone()
        return result[0] if result and result[0] is not None else 0
    
    def disconnectFromDatabase(self):
        self.dbIO.close()
        
    def commitToDatabase(self):
        self.dbIO.commit()
        
    def generateSchema(self):
        cursor = self.dbIO.cursor()
        cursor.execute("""CREATE TABLE IF NOT EXISTS prices (
                            symbol TEXT,
                            timestamp INTEGER, 
                            open REAL,
                            high REAL,
                            low REAL,
                            close REAL,
                            volume REAL
                        )""")
        #self.commitToDatabase()
        
    
    def writeDataToDatabase(self, lastTimestamp: int) -> bool:
        #dbIO = sql3.connect('market_data.db')
        cursor = self.dbIO.cursor()
        # poll latest timeStamp, if no new data - no need to commit
        compareTimestamp = self.lastCapturedTimestamp(symbol="AAPL")
        if compareTimestamp > lastTimestamp:
            # create structure if it doesnt exist - really only need to do once
            self.generateSchema()
            marketData = self.ingestMarketData()
            for ts, row in marketData.iterrows():
                time_ =     int(ts.timestamp()) # store as a second epoch timestamp for quick query
                open_ =     row['Open']
                high_ =     row['High']
                low_ =      row['Low']
                close_ =    row['Close']
                volume_ =   row['Volume']
                cursor.execute("INSERT INTO prices VALUES (?,?,?,?,?,?,?)",
                    ("AAPL", time_, open_, high_, low_, close_, volume_))
            # commit saves changes and close ends connection
            self.commitToDatabase()
            print("Pushed market data to database successfully.\n")
            return True
            
        else:
            print("No new market data to write to database.\n")
            self.disconnectFromDatabase()
            return False

testing = fileHandle()

# commands to open the database and check data vvv
# sqlite3 market_data.db
# .tables
# .schema prices
# SELECT * FROM prices;

# get initial last captured timestamp on boot to start using that against new data
initialLastTimestamp = testing.lastCapturedTimestamp(symbol="AAPL")
while True:
    testing.writeDataToDatabase(initialLastTimestamp)
    # sleep for a bit before polling again
    time.sleep(120)  # sleep for 2 minutes