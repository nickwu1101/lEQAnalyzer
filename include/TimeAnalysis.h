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

private:
    vector<double> vd;
    string handledTU;
    double correctByTemp(string, string, string, double, double);
    double calErrAfterCorrect(string, string, string, double, double, double);
    double takeTimeLength(string);
};

#endif
