#include <cctype>
#include <cstdlib>

#include "makePlots.h"
#include "StatisticTools.h"
#include "Handling.h"
#include "TempHumi.h"
#include "TimeAnalysis.h"
#include "GraphPrinter.h"

void main_test();
void main_makePlots();
void main_overlap();
void main_tempHumiPlots();
void main_longAnalysis();
void main_checkFitting();
void main_fluctuation();
void main_merge();
void main_countTemp();
void main_timeAna();
void main_IQR();
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
	else if(arg == "-m" || arg == "--merge") {
	    anaType = 7;
	    break;
	}
	else if(arg == "-o" || arg == "--overlap") {
	    anaType = 2;
	    break;
	}
	else if(arg == "-th" || arg == "--tempHumi") {
	    anaType = 3;
	    break;
	}
	else if(arg == "-l" || arg == "--long") {
	    anaType = 4;
	    break;
	}
	else if(arg == "-ch" || arg == "--check") {
	    anaType = 5;
	    break;
	}
	else if(arg == "-fluct" || arg == "--fluctuation") {
	    anaType = 6;
	    break;
	}
	else if(arg == "-ct") {
	    anaType = 8;
	    break;
	}
	else if(arg == "-af") {
	    anaType = 9;
	    break;
	}
	else if(arg == "-IQR") {
	    anaType = 10;
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
    case 2:
	main_overlap();
	break;
    case 3:
	main_tempHumiPlots();
	break;
    case 4:
	main_longAnalysis();
	break;
    case 5:
	main_checkFitting();
	break;
    case 6:
	main_fluctuation();
	break;
    case 7:
	main_merge();
	break;
    case 8:
	main_countTemp();
	break;
    case 9:
	main_timeAna();
	break;
    case 10:
	main_IQR();
	break;
    }

    return 0;
}

void main_test() {
    //makePlots *testObj = new makePlots();
    //StatisticTools *testObj = new StatisticTools();
    //Handling *testObj = new Handling();
    //TempHumi *testObj = new TempHumi();
    TimeAnalysis *testObj = new TimeAnalysis();
    GraphPrinter *testObj2 = new GraphPrinter();
    //testObj->test();

    testObj2->setHandledTimeUnit("P2H");
    testObj2->test();
/*
    testObj2->setHandledTimeUnit("P4H");
    testObj2->test();
    testObj2->setHandledTimeUnit("P6H");
    testObj2->test();
    testObj2->setHandledTimeUnit("P12H");
    testObj2->test();
    testObj2->setHandledTimeUnit("PD");
    testObj2->test();
*/
}

void main_makePlots() {
    makePlots *mp = new makePlots();
    mp->setAnaType(anaType);
    mp->execute();
}

void main_overlap() {
    Handling *hd = new Handling();
    hd->doProcedure1();
}

void main_tempHumiPlots() {
    TempHumi *th = new TempHumi();
    th->execute();
}

void main_longAnalysis() {
    Handling *hd = new Handling();
    hd->doProcedure6();
}

void main_checkFitting() {
    Handling *hd = new Handling();
    hd->checkHist();
    hd->overlapForComparison();
}

void main_fluctuation() {
    Handling *hd = new Handling();
    hd->doProcedure3();
    hd->doProcedure5();
}

void main_merge() {
    makePlots* mp = new makePlots();
    mp->mergeHist();
}

void main_countTemp() {
    Handling* hd = new Handling();
    hd->doProcedure4();
}

void main_timeAna() {
}

void main_IQR() {
    TimeAnalysis* ta = new TimeAnalysis();
    ta->analyzeBestBinWidth();
}

void main_help() {
    std::cout  << "This will be help text." << std::endl;
}
