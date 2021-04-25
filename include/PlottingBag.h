#ifndef PlottingBag_h
#define PlottingBag_h 1

#include <iostream>
#include <map>

using namespace std;

class TGraph;
class TF1;

class PlottingBag {
public:
    PlottingBag();
    ~PlottingBag();

    TGraph* getGraph();

    void setGraph(string, TGraph*);
    void setTF1(string, TGraph*);

private:
    map<string, TGraph*> graphMap;
    map<string, TF1*> TF1Map;
};

#endif
