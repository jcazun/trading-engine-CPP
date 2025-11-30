import yfinance as yf
import pandas as pd
import datetime
import sqlite3 as sql3
import os, sys
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
            
    def ingestMarketData(self) ->  pd.DataFrame: # eventually, return market data
        print("Ingesting market data...\n")
        dataCapture = yf.Ticker("AAPL").history(period="1d", interval="60m") # using this for testing purposes
        print(dataCapture)
        return dataCapture
                
    def formatUtility() -> str :
        returnString = ""
        for i in range(106):
            returnString += "*"
        return returnString + "\n"
    
    def writeDataToDatabase(self):
        dbIO = sql3.connect('market_data.db')
        cursor = dbIO.cursor()
        # create structure if it doesnt exist - really only need to do once
        cursor.execute("""CREATE TABLE IF NOT EXISTS prices (
                        symbol TEXT,
                        timestamp INTEGER, 
                        open REAL,
                        high REAL,
                        low REAL,
                        close REAL,
                        volume REAL
                    )""")
                
        marketData = self.ingestMarketData(self)
        for ts, row in marketData.iterrows():
            time_ =     int(ts.timestamp()) # store as a second epoch timestamp for quick query
            open_ =     row['Open']
            high_ =     row['High']
            low_ =      row['Low']
            close_ =    row['Close']
            volume_ =   row['Volume']
            # print for debug purposes, remove later.
            # print(f"{time_}, {open_}, {high_}, {low_}, {close_}, {volume_}") - code works! so im getting rid of the print statement
            cursor.execute(f"INSERT INTO prices VALUES (?,?,?,?,?,?,?)",
                ("AAPL", time_, open_, high_, low_, close_, volume_))
        # commit saves changes and close ends connection
        dbIO.commit()
        print("Pushed market data to database successfully.\n")
        dbIO.close()

testing = fileHandle

# commands to open the database and check data vvv
# sqlite3 market_data.db
# .tables
# .schema prices
# SELECT * FROM prices;

testing.writeDataToDatabase(self=testing)