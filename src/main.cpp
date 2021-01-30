#include <cctype>
#include <cstdlib>

#include "makePlots.h"
#include "StatisticTools.h"

void main_test();
void main_makePlots();
void main_help();

int anaType = 0;

int main(int argc, char** argv) {
    string arg_string;
    vector<string> arg_list;
    for(int i = 0; i < argc; ++i) {
	arg_string = argv[i];
	arg_list.push_back(arg_string);
    }

    int iarg = 1;
    while(iarg < argc) {
	string arg = arg_list[iarg];

	if(arg ==  "-h" || arg == "--help") {
	    anaType = -1;
	    break;
	}
	else if(arg == "-t" || arg == "--test") {
	    anaType = -2;
	    break;
	}
	else if(arg == "-s" || arg == "--sigleLength") {
	    anaType = 1;
	    break;
	}
	else {
	    std::cout << "Unknown option ... print usage" << std::endl;
	    anaType = -1;
	    break;
	}
    }

    switch(anaType) {
    case -2:
	main_test();
	break;
    case -1:
	main_help();
	break;
    case 0:
    case 1:
	main_makePlots();
	break;
    }

    return 0;
}

void main_test() {
    //makePlots *testObj = new makePlots();
    StatisticTools *testObj = new StatisticTools();
    testObj->test();
}

void main_makePlots() {
    makePlots *mp = new makePlots();
    mp->setAnaType(anaType);
    mp->execute();
}

void main_help() {
    std::cout  << "This will be help text." << std::endl;
}
