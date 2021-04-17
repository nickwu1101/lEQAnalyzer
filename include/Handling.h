#ifndef Handling_h
#define Handling_h 1

#include <iostream>
#include <vector>

using namespace std;

class Handling {
public:
    Handling();
    ~Handling();

    void execute();
    void test();

    void doProcedure1(); // Overlap
    void doProcedure2();
    void doProcedure3();

private:
    vector<string> filenameList;

    void readFilenameListTxt(string);
};

#endif
