#ifndef FittingWork_h
#define FittingWork_h 1

#include <iostream>
#include <vector>

using namespace std;

class FittingWork {
public:
    FittingWork();
    ~FittingWork();

    void execute();
    void test();

private:
    vector<string> fileList;
    string fittingType = "Minuit";

    void fit1();
};

#endif
