#ifndef TimeAnalysis_h
#define TimeAnalysis_h 1

#include <iostream>
#include <vector>

using namespace std;

class TimeAnalysis {
public:
    TimeAnalysis();
    ~TimeAnalysis();

    void execute();
    void test();

    void analyzeByFitting();
    void analyzeByCounting();
    void normalizeFittingResult();
    void normalizeCountingResult();    
    void correctFittingAnaByTemp();
    void correctCountingAnaByTemp();
    void correctCountingAnaByTempU();

    void analyzeBestBinWidth();

    void fitOnTotalTime();

private:
    vector<double> vd;
    string handledTU;
    string timeRange;
    string quantity;

    double const q1x = 0.25;
    double const q2x = 0.5;
    double const q3x = 0.75;

    double correctByTemp(string, string, string, double, double);
    double calErrAfterCorrect(string, string, string, double, double, double);
    double takeTimeLength(string);
};

#endif
