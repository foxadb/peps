#include "spot.hpp"
#include "DataBaseManager.hpp"
#include "Data.hpp"
#include "Actigo.hpp"
#include "Calibration.hpp"
#include "BlackScholesModel.hpp"
#include "MonteCarlo.hpp"
#include "Option.hpp"
#include "pricer_utils.hpp"
#include "time_utils.hpp"

#include <vector>
#include <iostream>
#include <string>
#include <ctime>

int main(int argc, char** argv) {
    time_t current_date = std::atoi(argv[1]);
    int mcSamplesNb = 50000; // Monte Carlo samples number

    std::vector<time_t> semesterDates {
        1428451200, 1444608000, 1460332800, 1476057600,
        1491782400, 1507593600, 1523318400, 1539129600,
        1554854400, 1570665600, 1586822400, 1602460800,
        1618185600, 1633910400, 1649635200, 1665360000,
        1681171200
    };

    DataBaseManager *dbManager = DataBaseManager::getDbManager();

    time_t last_year = current_date - 365 * 24 * 3600;
    std::vector<Spot> euroStoxSpotspots = dbManager->getSpots(last_year, current_date, "^STOXX50E");

    PnlVect* euroStoxSpots = pnl_vect_create_from_scalar(euroStoxSpotspots.size(), 0);
    vectorToPnlVect(euroStoxSpotspots, euroStoxSpots);
    std::vector<Spot> spUsdSpotspots = dbManager->getSpots(last_year, current_date, "^GSPC");

    PnlVect* spUsdSpots = pnl_vect_create_from_scalar(spUsdSpotspots.size(), 0);
    vectorToPnlVect(spUsdSpotspots, spUsdSpots);
    std::vector<Spot> spAudSpotspots = dbManager->getSpots(last_year, current_date, "^AXJO");

    PnlVect* spAudSpots = pnl_vect_create_from_scalar(spAudSpotspots.size(), 0);
    vectorToPnlVect(spAudSpotspots, spAudSpots);
    std::vector<Spot> eurUsdSpotspots = dbManager->getSpots(last_year, current_date, "EURUSD=X");

    PnlVect* eurUsdSpots = pnl_vect_create_from_scalar(eurUsdSpotspots.size(), 0);
    vectorToPnlVect(eurUsdSpotspots, eurUsdSpots);
    std::vector<Spot> eurAudSpotspots = dbManager->getSpots(last_year, current_date, "EURAUD=X");

    PnlVect* eurAudSpots = pnl_vect_create_from_scalar(eurAudSpotspots.size(), 0);
    vectorToPnlVect(eurAudSpotspots, eurAudSpots);
    //int dataSize = euroStoxSpots->size;
    double minSize = MIN(euroStoxSpots->size, spUsdSpots->size);
    minSize = MIN(minSize, spAudSpots->size);
    minSize = MIN(minSize, eurUsdSpots->size);
    minSize = MIN(minSize, eurAudSpots->size);

    // Resize spots
    pnl_vect_resize(euroStoxSpots, minSize);
    pnl_vect_resize(spUsdSpots, minSize);
    pnl_vect_resize(spAudSpots, minSize);
    pnl_vect_resize(eurUsdSpots, minSize);
    pnl_vect_resize(eurAudSpots, minSize);
    int dataSize = euroStoxSpots->size;
    int actigoSize = 5;
    double calibrationMaturity = 1;

    // free risk rates

    double rEur = 0.0075;
    double rUsd = 0.028;
    double rAud = 0.026;

    // Maturity to modify
    PnlMat* calibrationDataMatrix = pnl_mat_create_from_scalar(dataSize, actigoSize, 0);
    pnl_mat_set_col(calibrationDataMatrix, euroStoxSpots, 0);
    pnl_mat_set_col(calibrationDataMatrix, spUsdSpots, 1);
    pnl_mat_set_col(calibrationDataMatrix, spAudSpots, 2);
    pnl_mat_set_col(calibrationDataMatrix, eurUsdSpots, 3);
    pnl_mat_set_col(calibrationDataMatrix, eurAudSpots, 4);

    Data *data = new Data(calibrationDataMatrix, rEur, rUsd, rAud);
    double step = calibrationMaturity / dataSize;
    Calibration *calibration = new Calibration(data, step);
    double euroStoxInitialSpot = dbManager->getSpot(1428451200, "^STOXX50E").getClose();
    double spUsdInitialSpot = dbManager->getSpot(1428451200, "^GSPC").getClose();
    double spAudInitialSpot = dbManager->getSpot(1428451200, "^AXJO").getClose();
    double maturity = 8;
    Actigo *actigo = new Actigo(maturity, 16, actigoSize, euroStoxInitialSpot, spUsdInitialSpot, spAudInitialSpot,
                      rEur, rUsd, rAud);

    // Create the BlackScholesModel

    // Recuprate Initial Spots
    double actuParam = exp(-rEur * maturity);
    PnlVect* initialSpotsEuro = pnl_vect_create_from_scalar(actigoSize, 0);
    double eurUsdInitialSpot = dbManager->getSpot(1428451200, "EURUSD=X").getClose() ;
    spUsdInitialSpot *= eurUsdInitialSpot;
    double eurAudInitialSpot = dbManager->getSpot(1428451200, "EURAUD=X").getClose() ;
    spAudInitialSpot *= eurAudInitialSpot;

    // TODO change zeros coupons so as to use the correct free risk rates
    eurUsdInitialSpot *= actuParam;
    eurAudInitialSpot *= actuParam;
    LET(initialSpotsEuro, 0) = euroStoxInitialSpot;
    LET(initialSpotsEuro, 1) = spUsdInitialSpot;
    LET(initialSpotsEuro, 2) = spAudInitialSpot;
    LET(initialSpotsEuro, 3) = eurUsdInitialSpot;
    LET(initialSpotsEuro, 4) = eurAudInitialSpot;
    BlackScholesModel *bsm = new BlackScholesModel(
                actigoSize, rEur, calibration->getCorrelationsMatrix(),
                calibration->getVolatilities(), initialSpotsEuro);
    PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
    MonteCarlo *mc = new MonteCarlo(bsm, actigo, rng, 0.01, mcSamplesNb);
    PnlVect* current_delta = pnl_vect_create_from_scalar(actigoSize, 0);
    PnlVect* previous_delta = pnl_vect_create_from_scalar(actigoSize, 0);
    std::vector<time_t> rightDates;
    double actigoPrice = 0;
    double hedgingPrice = 0;
    double free_risk_part = 0;
    double risked_part = 0;

    PnlVect* current_spots = pnl_vect_create_from_scalar(actigoSize, 0);
    PnlVect* previous_spots = pnl_vect_create_from_scalar(actigoSize, 0);
    // Rebalancing for loop
    std::time_t start = std::clock();

    rightDates = getRightDates(current_date, semesterDates);
    PnlMat* past = pnl_mat_create_from_scalar(rightDates.size(), actigoSize, 0);
    getPastData(dbManager, past, rightDates, rUsd, rAud);
    time_t dateDifference = current_date - 1428451200;

    double convertedDate = (double) dateDifference / (365 * 24 * 3600);
    if (convertedDate > 8) {
          convertedDate = 8;
    }

    LET(current_spots, 0) = getLastAvailableSpot(dbManager, "^STOXX50E", current_date);
    LET(current_spots, 1) = getLastAvailableSpot(dbManager, "^GSPC", current_date);
    LET(current_spots, 2) = getLastAvailableSpot(dbManager, "^AXJO", current_date);
    LET(current_spots, 3) = getLastAvailableSpot(dbManager, "EURUSD=X", current_date);
    LET(current_spots, 4) = getLastAvailableSpot(dbManager, "EURAUD=X", current_date);
    mc->rebalanceAtSpecificDate(past, convertedDate, current_delta, actigoPrice);
    dbManager->postDelta(GET(current_delta, 0), current_date, "^STOXX50E");
    dbManager->postDelta(GET(current_delta, 1), current_date, "^GSPC");
    dbManager->postDelta(GET(current_delta, 2), current_date, "^AXJO");
    dbManager->postDelta(GET(current_delta, 3), current_date, "EURUSD=X");
    dbManager->postDelta(GET(current_delta, 4), current_date, "EURAUD=X");
    risked_part = pnl_vect_scalar_prod(current_spots, current_delta);

    if (current_date == 1428451200) {
        free_risk_part = actigoPrice - risked_part;
    } else {
      time_t previous_date = dbManager->getLastHedgingDate();
      //previous deltas
      LET(previous_delta, 0) = dbManager->getDelta(previous_date, "^STOXX50E");
      LET(previous_delta, 1) = dbManager->getDelta(previous_date, "^GSPC");
      LET(previous_delta, 2) = dbManager->getDelta(previous_date, "^AXJO");
      LET(previous_delta, 3) = dbManager->getDelta(previous_date, "EURUSD=X");
      LET(previous_delta, 4) = dbManager->getDelta(previous_date, "EURAUD=X");
      //previous spots
      LET(previous_spots, 0) = getLastAvailableSpot(dbManager, "^STOXX50E", previous_date);
      LET(previous_spots, 1) = getLastAvailableSpot(dbManager, "^GSPC", previous_date);
      LET(previous_spots, 2) = getLastAvailableSpot(dbManager, "^AXJO", previous_date);
      LET(previous_spots, 3) = getLastAvailableSpot(dbManager, "EURUSD=X", previous_date);
      LET(previous_spots, 4) = getLastAvailableSpot(dbManager, "EURAUD=X", previous_date);
      //previous hedging portfolio value
      double previous_hedging_value = dbManager->getHedging(previous_date);
      double previous_risked_part = pnl_vect_scalar_prod(previous_spots, previous_delta);
      double previous_free_risk_part = previous_hedging_value - previous_risked_part;

      double step = (double) ((current_date - previous_date) / (365 * 24 * 3600));
      free_risk_part = previous_free_risk_part * exp(rEur * step);
                     - risked_part + pnl_vect_scalar_prod(previous_delta, current_spots);
    }

        hedgingPrice = free_risk_part + risked_part;
        dbManager->postPrice(current_date, actigoPrice, hedgingPrice);
        pnl_vect_clone(previous_delta, current_delta);

        double duration = (double)(std::clock() - start) / CLOCKS_PER_SEC;
        std::cout << "Actigo: " << actigoPrice
                  << ", Hedging: " << hedgingPrice
                  << ", Error: " << hedgingPrice - actigoPrice
                  << ", Timer: " << duration << " s"
                  << std::endl;

        // Free past matrix
        pnl_mat_free(&past);

    // Free memory
    pnl_rng_free(&rng);

    pnl_vect_free(&euroStoxSpots);
    pnl_vect_free(&spUsdSpots);
    pnl_vect_free(&spAudSpots);
    pnl_vect_free(&eurUsdSpots);
    pnl_vect_free(&eurAudSpots);
    pnl_vect_free(&initialSpotsEuro);
    pnl_vect_free(&current_spots);
    pnl_vect_free(&previous_spots);
    pnl_vect_free(&current_delta);
    pnl_vect_free(&previous_delta);

    delete data;
    delete actigo;
    delete calibration;
    delete bsm;
    delete mc;
}
