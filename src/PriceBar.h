#pragma once
#include <string>

/*
Specifically: What is #pragma once?
#pragma once is a preprocessor directive that tells the compiler:

"Only include this file once per compilation unit."
Why is this useful?

When you include header files multiple times (directly or indirectly through other includes), you can get multiple definition errors or redundant processing.
Traditionally, this is handled with include guards like:
*/

struct PriceBar {
    std::string date;       // use string for date representation in simulation (csv loading) (e.g., "2023-10-05")
    int timestamp;          // use timestamp for polling from db (its an epoch integer)
    double open, high, low, close, volume;
};