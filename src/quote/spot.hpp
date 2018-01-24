#ifndef SPOT_HPP
#define SPOT_HPP

#include <string>

class Spot {
public:
    Spot(std::string date, double open, double high, double low, double close);
    ~Spot();

    std::string getDate();
    double getOpen();
    double getHigh();
    double getLow();
    double getClose();

    std::string toString();

    void printSpot();

private:
    std::string date;
    double open;
    double high;
    double low;
    double close;
};

#endif /* SPOT_HPP */