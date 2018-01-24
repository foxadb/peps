#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <ctime>
#include <curl/curl.h>

double getForexRate(std::string date, std::string base, std::string symbol);

double* getForexRates(std::string date,
                      std::string base,
                      std::string *symbols,
                      int nbSymbols);

void getYahooCrumbCookie(std::string url,
                         std::string *crumb,
                         std::string *cookie);

std::string extractYahooCrumb(std::string code);

std::string extractYahooCookie(const char* cookieFile);

std::string downloadYahooCsv(std::string symbol,
                             std::time_t period1,
                             std::time_t period2,
                             std::string interval,
                             std::string *crumb,
                             std::string *cookie);

#endif /* UTILS_HPP */