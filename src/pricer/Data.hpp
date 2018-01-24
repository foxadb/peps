#ifndef DATA_H
#define	DATA_H

class Data:{
public:
      PnlVect* euroStoxSpots;
      PnlVect* spUsdSpots;
      PnlVect* spAudSpots;
      PnlVect* forexEurUsd;
      PnlVect* forexEurAud;
      double rEur;
      double rUsd;
      double rAud;
      PnlMat* dataMatrix;

      data(const char* debutDate, const char* currentDate);
      void completeData(int remainingDates, Option *option, PnlVect *currentSpots, PnlVect *volatilities, PnlMat *correlations);
};
