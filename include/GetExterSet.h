#ifndef GetExterSet_h
#define GetExterSet_h 1

#include <iostream>
#include <vector>

using namespace std;

struct OneTerm {
    string termName;
    string valueStr;
};

class GetExterSet {
public:
    GetExterSet(string inputTermFile);
    ~GetExterSet();

    string giveStrVar(string);
    int giveIntVar(string);
    double giveDoubleVar(string);
    bool giveBoolVar(string);

private:
    vector<OneTerm> termList;

    void loadTermList(string);
};

#endif
