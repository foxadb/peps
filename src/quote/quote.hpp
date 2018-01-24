#ifndef QUOTE_HPP
#define QUOTE_HPP

#include <string>
#include <ctime>
#include <vector>
#include "pnl/pnl_vector.h"

#include "spot.hpp"

class Quote {
public:
    Quote(std::string symbol);
    ~Quote();

    size_t nbSpots();

    Spot getSpot(size_t i);
    Spot getSpot(std::string date);
    PnlVect* getCloseSpots();

    void printSpots();
    void clearSpots();

    std::string getHistoricalCsv(std::time_t period1,
                                 std::time_t period2,
                                 const char *interval);

    void getHistoricalSpots(std::time_t period1,
                            std::time_t period2,
                            const char *interval);

    void getHistoricalSpots(const char *period1,
                            const char *period2,
                            const char *interval);

private:
    std::string symbol;
    std::vector<Spot> spots;
};

#endif /* QUOTE_HPP */