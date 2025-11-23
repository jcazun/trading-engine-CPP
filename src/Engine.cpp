#include "Engine.h"
#include "Strategy.h"
#include <fstream>
#include <sstream>
#include <iostream>

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
