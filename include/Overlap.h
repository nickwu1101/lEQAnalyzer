#ifndef Overlap_h
#define Overlap_h 1

#include <iostream>
#include <vector>

using namespace std;

class Overlap {
public:
    Overlap();
    ~Overlap();

    void execute();
    void test();

    void doProcedure1();

private:
    vector<string> filenameList;

    void readFilenameListTxt(string);
};

#endif
