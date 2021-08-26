#include "GraphPrinter.h"

#include <TAxis.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TFitResult.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TPaveText.h>
#include <TTree.h>

#include "Calendar.h"
#include "TempHumi.h"

GraphPrinter::GraphPrinter() {
    handledTU = "P2H";
    quantity = "Shifting";
}



GraphPrinter::~GraphPrinter() {}



void GraphPrinter::execute() {
}



void GraphPrinter::test() {
    printFittingGraph();
    printNormFittingGraph();
    printTempCorrectFittingGraph();
    printCompareCorrectionFittingGraph();
    printFittingTempCorrelation();

    handledTU = "P2H";
    printCountingGraph();
    printNormCountingGraph();
    printTempCorrectCountingGraph();
    printCompareCorrectionCountingGraph();
    printCountingTempCorrelation();

    printOverlapExpAndSimulation();
    //printOverlapExpAndBkg();
}



void GraphPrinter::printFittingGraph() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);

    TFile* f = nullptr;
    if(handledTU == "P2H") {
	if(quantity == "Energy")
	    f = new TFile("ready/fitResult.root", "READ");
	else if(quantity == "Shifting")
	    f = new TFile("ready/fitResultS.root", "READ");
    } else if(handledTU == "P6H")
	f = new TFile("ready/fitResult6.root", "READ");

    TTree* fitTree;
    int entryNo;
    int year;
    int month;
    int day;
    int hour;
    int min;
    double sec;

    double cg04;
    double cg06;
    double cg16;
    double cg0406;

    double cge04;
    double cge06;
    double cge16;
    double cge0406;

    f->GetObject("dataFitting", fitTree);
    fitTree->SetBranchAddress("entryNo", &entryNo);
    fitTree->SetBranchAddress("year", &year);
    fitTree->SetBranchAddress("month", &month);
    fitTree->SetBranchAddress("day", &day);
    fitTree->SetBranchAddress("hour", &hour);
    fitTree->SetBranchAddress("minute", &min);
    fitTree->SetBranchAddress("second", &sec);

    fitTree->SetBranchAddress("cg04", &cg04);
    fitTree->SetBranchAddress("cg06", &cg06);
    fitTree->SetBranchAddress("cg16", &cg16);
    fitTree->SetBranchAddress("cg0406", &cg0406);

    fitTree->SetBranchAddress("cge04", &cge04);
    fitTree->SetBranchAddress("cge06", &cge06);
    fitTree->SetBranchAddress("cge16", &cge16);
    fitTree->SetBranchAddress("cge0406", &cge0406);

    TFile* foutlier = nullptr;
    if(handledTU == "P2H") {
	if(quantity == "Energy")
	    foutlier = new TFile("ready/NormalizedFitResult.root", "READ");
	else if(quantity == "Shifting")
	    foutlier = new TFile("ready/NormalizedFitResultS.root", "READ");
    } else if(handledTU == "P6H")
	foutlier = new TFile("ready/NormalizedFitResult6.root", "READ");

    TTree* outlierTree;
    int entryNoOutlier;
    bool isoutlier04;
    bool isoutlier06;
    bool isoutlier16;
    bool isoutlier0406;

    foutlier->GetObject("dataFitting", outlierTree);
    outlierTree->SetBranchAddress("entryNo", &entryNoOutlier);
    outlierTree->SetBranchAddress("isoutlier04", &isoutlier04);
    outlierTree->SetBranchAddress("isoutlier06", &isoutlier06);
    outlierTree->SetBranchAddress("isoutlier16", &isoutlier16);
    outlierTree->SetBranchAddress("isoutlier0406", &isoutlier0406);

    TGraphErrors* ge04 = new TGraphErrors();
    TGraphErrors* ge06 = new TGraphErrors();
    TGraphErrors* ge16 = new TGraphErrors();
    TGraphErrors* ge0406 = new TGraphErrors();

    double mean04 = 0.;
    double mean06 = 0.;
    double mean16 = 0.;
    double mean0406 = 0.;
    double std04 = 0.;
    double std06 = 0.;
    double std16 = 0.;
    double std0406 = 0.;
    int ntimebin04 = 0;
    int ntimebin06 = 0;
    int ntimebin16 = 0;
    int ntimebin0406 = 0;

    Long64_t nentries = fitTree->GetEntries();
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	fitTree->GetEntry(entry);
	outlierTree->GetEntry(entry);
	if(entryNo == entryNoOutlier) {
	    TDatime* tdt = new TDatime(year, month, day, hour, min, (int)sec);

	    if(!isoutlier16) {
		ge04->SetPoint(ge04->GetN(), tdt->Convert(), cg04);
		ge04->SetPointError(ge04->GetN() - 1, 0., cge04);
		mean04 += cg04;
		std04 += cg04*cg04;
		ntimebin04++;
	    }

	    if(!isoutlier16) {
		ge06->SetPoint(ge06->GetN(), tdt->Convert(), cg06);
		ge06->SetPointError(ge06->GetN() - 1, 0., cge06);
		mean06 += cg06;
		std06 += cg06*cg06;
		ntimebin06++;
	    }

	    if(!isoutlier16) {
		ge16->SetPoint(ge16->GetN(), tdt->Convert(), cg16);
		ge16->SetPointError(ge16->GetN() - 1, 0., cge16);
		mean16 += cg16;
		std16 += cg16*cg16;
		ntimebin16++;
	    }

	    if(!isoutlier16) {
		ge0406->SetPoint(ge0406->GetN(), tdt->Convert(), cg0406);
		ge0406->SetPointError(ge0406->GetN() - 1, 0., cge0406);
		mean0406 += cg0406;
		std0406 += cg0406*cg0406;
		ntimebin0406++;
	    }

	delete tdt;
	} else {
	    cout << "Both entry numbers are not match!!!" << endl;
	    exit(0);
	}
    }

    mean04 /= ntimebin04;
    mean06 /= ntimebin06;
    mean16 /= ntimebin16;
    mean0406 /= ntimebin0406;

    std04 -= ntimebin04*mean04*mean04;
    std06 -= ntimebin06*mean06*mean06;
    std16 -= ntimebin16*mean16*mean16;
    std0406 -= ntimebin0406*mean0406*mean0406;
    std04 = TMath::Sqrt(std04/(ntimebin04 - 1));
    std06 = TMath::Sqrt(std06/(ntimebin06 - 1));
    std16 = TMath::Sqrt(std16/(ntimebin16 - 1));
    std0406 = TMath::Sqrt(std0406/(ntimebin0406 - 1));

    double err04 = std04/TMath::Sqrt(ntimebin04);
    double err06 = std06/TMath::Sqrt(ntimebin06);
    double err16 = std16/TMath::Sqrt(ntimebin16);
    double err0406 = std0406/TMath::Sqrt(ntimebin0406);

    printWithErrorBand(ge04, mean04, err04, "cGauss", "peak04", handledTU);
    printWithErrorBand(ge06, mean06, err06, "cGauss", "peak06", handledTU);
    printWithErrorBand(ge16, mean16, err16, "cGauss", "peak16", handledTU);
    printWithErrorBand(ge0406, mean0406, err0406, "cGauss", "peak0406", handledTU);

    delete ge0406;
    delete ge16;
    delete ge06;
    delete ge04;

    foutlier->Close();
    delete foutlier;

    if(f != nullptr) {
	f->Close();
	delete f;
    }

    delete cGraph;
}



void GraphPrinter::printCountingGraph() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);

    TFile* f = nullptr;
    if(quantity == "Energy")
	f = new TFile("ready/fluctResult.root", "READ");
    else if(quantity == "Shifting")
	f = new TFile("ready/fluctResultS.root", "READ");

    TTree* countTree;
    int entryNo;
    int year;
    int month;
    int day;
    int hour;
    int min;
    double sec;

    double counts04;
    double counts06;
    double counts16;
    double counts0406;

    double countse04;
    double countse06;
    double countse16;
    double countse0406;

    f->GetObject("dataCounts", countTree);
    countTree->SetBranchAddress("entryNo", &entryNo);
    countTree->SetBranchAddress("year", &year);
    countTree->SetBranchAddress("month", &month);
    countTree->SetBranchAddress("day", &day);
    countTree->SetBranchAddress("hour", &hour);
    countTree->SetBranchAddress("minute", &min);
    countTree->SetBranchAddress("second", &sec);

    countTree->SetBranchAddress("counts04", &counts04);
    countTree->SetBranchAddress("counts06", &counts06);
    countTree->SetBranchAddress("counts16", &counts16);
    countTree->SetBranchAddress("counts0406", &counts0406);

    countTree->SetBranchAddress("countse04", &countse04);
    countTree->SetBranchAddress("countse06", &countse06);
    countTree->SetBranchAddress("countse16", &countse16);
    countTree->SetBranchAddress("countse0406", &countse0406);

    TFile* foutlier = nullptr;
    if(quantity == "Energy")
	foutlier = new TFile("ready/NormalizedFluctResult.root", "READ");
    else if(quantity == "Shifting")
	foutlier = new TFile("ready/NormalizedFluctResultS.root", "READ");

    TTree* outlierTree;
    int entryNoOutlier;
    bool isoutlier04;
    bool isoutlier06;
    bool isoutlier16;
    bool isoutlier0406;

    foutlier->GetObject("dataCounts", outlierTree);
    outlierTree->SetBranchAddress("entryNo", &entryNoOutlier);
    outlierTree->SetBranchAddress("isoutlier04", &isoutlier04);
    outlierTree->SetBranchAddress("isoutlier06", &isoutlier06);
    outlierTree->SetBranchAddress("isoutlier16", &isoutlier16);
    outlierTree->SetBranchAddress("isoutlier0406", &isoutlier0406);

    TGraphErrors* ge04 = new TGraphErrors();
    TGraphErrors* ge06 = new TGraphErrors();
    TGraphErrors* ge16 = new TGraphErrors();
    TGraphErrors* ge0406 = new TGraphErrors();

    double mean04 = 0.;
    double mean06 = 0.;
    double mean16 = 0.;
    double mean0406 = 0.;
    double std04 = 0.;
    double std06 = 0.;
    double std16 = 0.;
    double std0406 = 0.;
    int ntimebin04 = 0;
    int ntimebin06 = 0;
    int ntimebin16 = 0;
    int ntimebin0406 = 0;

    Long64_t nentries = countTree->GetEntries();
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	countTree->GetEntry(entry);
	outlierTree->GetEntry(entry);
	if(entryNo == entryNoOutlier) {
	    TDatime* tdt = new TDatime(year, month, day, hour, min, (int)sec);

	    if(!isoutlier16 &&
	       !isoutlier04 &&
	       counts04 > 0.) {
		ge04->SetPoint(ge04->GetN(), tdt->Convert(), counts04);
		ge04->SetPointError(ge04->GetN() - 1, 0., countse04);
		mean04 += counts04;
		std04 += counts04*counts04;
		ntimebin04++;
	    }

	    if(!isoutlier16 &&
	       !isoutlier06 &&
	       counts06 > 0.) {
		ge06->SetPoint(ge06->GetN(), tdt->Convert(), counts06);
		ge06->SetPointError(ge06->GetN() - 1, 0., countse06);
		mean06 += counts06;
		std06 += counts06*counts06;
		ntimebin06++;
	    }

	    if(!isoutlier16 && counts16 > 0.) {
		ge16->SetPoint(ge16->GetN(), tdt->Convert(), counts16);
		ge16->SetPointError(ge16->GetN() - 1, 0., countse16);
		mean16 += counts16;
		std16 += counts16*counts16;
		ntimebin16++;
	    }

	    if(!isoutlier16 &&
	       !isoutlier0406 &&
	       counts0406 > 70000.) {
		ge0406->SetPoint(ge0406->GetN(), tdt->Convert(), counts0406);
		ge0406->SetPointError(ge0406->GetN() - 1, 0., countse0406);
		mean0406 += counts0406;
		std0406 += counts0406*counts0406;
		ntimebin0406++;		
	    }

	    delete tdt;
	} else {
	    cout << "Both entry numbers are not match!!!" << endl;
	    exit(0);
	}
    }

    mean04 /= ntimebin04;
    mean06 /= ntimebin06;
    mean16 /= ntimebin16;
    mean0406 /= ntimebin0406;

    std04 -= ntimebin04*mean04*mean04;
    std06 -= ntimebin06*mean06*mean06;
    std16 -= ntimebin16*mean16*mean16;
    std0406 -= ntimebin0406*mean0406*mean0406;
    std04 = TMath::Sqrt(std04/(ntimebin04 - 1));
    std06 = TMath::Sqrt(std06/(ntimebin06 - 1));
    std16 = TMath::Sqrt(std16/(ntimebin16 - 1));
    std0406 = TMath::Sqrt(std0406/(ntimebin0406 - 1));

    double err04 = std04/TMath::Sqrt(ntimebin04);
    double err06 = std06/TMath::Sqrt(ntimebin06);
    double err16 = std16/TMath::Sqrt(ntimebin16);
    double err0406 = std0406/TMath::Sqrt(ntimebin0406);

    printWithErrorBand(ge04, mean04, err04, "counts", "peak04", handledTU);
    printWithErrorBand(ge06, mean06, err06, "counts", "peak06", handledTU);
    printWithErrorBand(ge16, mean16, err16, "counts", "peak16", handledTU);
    printWithErrorBand(ge0406, mean0406, err0406, "counts", "peak0406", handledTU);

    delete ge0406;
    delete ge16;
    delete ge06;
    delete ge04;

    foutlier->Close();
    delete foutlier;

    if(f != nullptr) {
	f->Close();
	delete f;
    }

    delete cGraph;
}



void GraphPrinter::printFittingTempCorrelation() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);
    TempHumi* th = new TempHumi();

    TFile* f = nullptr;
    if(handledTU == "P2H") {
	if(quantity == "Energy")
	    f = new TFile("ready/fitResult.root", "READ");
	else if(quantity == "Shifting")
	    f = new TFile("ready/fitResultS.root", "READ");
    } else if(handledTU == "P6H")
	f = new TFile("ready/fitResult6.root", "READ");

    TTree* fitTree;
    int entryNo;
    int year;
    int month;
    int day;
    int hour;
    int min;
    double sec;

    double cg04;
    double cg06;
    double cg16;
    double cg0406;

    double cge04;
    double cge06;
    double cge16;
    double cge0406;

    f->GetObject("dataFitting", fitTree);
    fitTree->SetBranchAddress("entryNo", &entryNo);
    fitTree->SetBranchAddress("year", &year);
    fitTree->SetBranchAddress("month", &month);
    fitTree->SetBranchAddress("day", &day);
    fitTree->SetBranchAddress("hour", &hour);
    fitTree->SetBranchAddress("minute", &min);
    fitTree->SetBranchAddress("second", &sec);

    fitTree->SetBranchAddress("cg04", &cg04);
    fitTree->SetBranchAddress("cg06", &cg06);
    fitTree->SetBranchAddress("cg16", &cg16);
    fitTree->SetBranchAddress("cg0406", &cg0406);

    fitTree->SetBranchAddress("cge04", &cge04);
    fitTree->SetBranchAddress("cge06", &cge06);
    fitTree->SetBranchAddress("cge16", &cge16);
    fitTree->SetBranchAddress("cge0406", &cge0406);

    TGraphErrors* ge04 = new TGraphErrors();
    TGraphErrors* ge06 = new TGraphErrors();
    TGraphErrors* ge16 = new TGraphErrors();
    TGraphErrors* ge0406 = new TGraphErrors();

    Long64_t nentries = fitTree->GetEntries();
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	fitTree->GetEntry(entry);

	char dtStr[15];
	sprintf(dtStr, "%04d%02d%02d%02d%02d%02.f", year, month, day, hour, min, sec);

	double temp = th->getAvgTemp(dtStr);
	double tempErr = th->getErrTemp(dtStr);

	ge04->SetPoint(ge04->GetN(), temp, cg04);
	ge06->SetPoint(ge06->GetN(), temp, cg06);
	ge16->SetPoint(ge16->GetN(), temp, cg16);
	ge0406->SetPoint(ge0406->GetN(), temp, cg0406);

	ge04->SetPointError(ge04->GetN() - 1, tempErr, cge04);
	ge06->SetPointError(ge06->GetN() - 1, tempErr, cge06);
	ge16->SetPointError(ge16->GetN() - 1, tempErr, cge16);
	ge0406->SetPointError(ge0406->GetN() - 1, tempErr, cge0406);
    }

    printCorrelationWithTemp(ge04, "cGauss", "peak04", handledTU);
    printCorrelationWithTemp(ge06, "cGauss", "peak06", handledTU);
    printCorrelationWithTemp(ge16, "cGauss", "peak16", handledTU);
    printCorrelationWithTemp(ge0406, "cGauss", "peak0406", handledTU);

    delete ge0406;
    delete ge16;
    delete ge06;
    delete ge04;

    if(f != nullptr) {
	f->Close();
	delete f;
    }

    delete th;
    delete cGraph;
}



void GraphPrinter::printCountingTempCorrelation() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);
    TempHumi* th = new TempHumi();

    TFile* f = nullptr;
    if(quantity == "Energy")
	f = new TFile("ready/fluctResult.root", "READ");
    else if(quantity == "Shifting")
	f = new TFile("ready/fluctResultS.root", "READ");

    TTree* countTree;
    int entryNo;
    int year;
    int month;
    int day;
    int hour;
    int min;
    double sec;

    double counts04;
    double counts06;
    double counts16;
    double counts0to25;
    double counts0406;

    double countse04;
    double countse06;
    double countse16;
    double countse0to25;
    double countse0406;

    f->GetObject("dataCounts", countTree);
    countTree->SetBranchAddress("entryNo", &entryNo);
    countTree->SetBranchAddress("year", &year);
    countTree->SetBranchAddress("month", &month);
    countTree->SetBranchAddress("day", &day);
    countTree->SetBranchAddress("hour", &hour);
    countTree->SetBranchAddress("minute", &min);
    countTree->SetBranchAddress("second", &sec);

    countTree->SetBranchAddress("counts04", &counts04);
    countTree->SetBranchAddress("counts06", &counts06);
    countTree->SetBranchAddress("counts16", &counts16);
    countTree->SetBranchAddress("counts0to25", &counts0to25);
    countTree->SetBranchAddress("counts0406", &counts0406);

    countTree->SetBranchAddress("countse04", &countse04);
    countTree->SetBranchAddress("countse06", &countse06);
    countTree->SetBranchAddress("countse16", &countse16);
    countTree->SetBranchAddress("countse0to25", &countse0to25);
    countTree->SetBranchAddress("countse0406", &countse0406);

    TGraphErrors* ge04 = new TGraphErrors();
    TGraphErrors* ge06 = new TGraphErrors();
    TGraphErrors* ge16 = new TGraphErrors();
    TGraphErrors* ge0to25 = new TGraphErrors();
    TGraphErrors* ge0406 = new TGraphErrors();

    Long64_t nentries = countTree->GetEntries();
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	countTree->GetEntry(entry);

	char dtStr[15];
	sprintf(dtStr, "%04d%02d%02d%02d%02d%02.f", year, month, day, hour, min, sec);

	double temp = th->getAvgTemp(dtStr);
	double tempErr = th->getErrTemp(dtStr);

	ge04->SetPoint(ge04->GetN(), temp, counts04);
	ge06->SetPoint(ge06->GetN(), temp, counts06);
	ge16->SetPoint(ge16->GetN(), temp, counts16);
	ge0to25->SetPoint(ge0to25->GetN(), temp, counts0to25);
	ge0406->SetPoint(ge0406->GetN(), temp, counts0406);

	ge04->SetPointError(ge04->GetN() - 1, tempErr, countse04);
	ge06->SetPointError(ge06->GetN() - 1, tempErr, countse06);
	ge16->SetPointError(ge16->GetN() - 1, tempErr, countse16);
	ge0to25->SetPointError(ge0to25->GetN() - 1, tempErr, countse0to25);
	ge0406->SetPointError(ge0406->GetN() - 1, tempErr, countse0406);
    }

    printCorrelationWithTemp(ge04, "counts", "peak04", handledTU);
    printCorrelationWithTemp(ge06, "counts", "peak06", handledTU);
    printCorrelationWithTemp(ge16, "counts", "peak16", handledTU);
    printCorrelationWithTemp(ge0to25, "counts", "0to25", handledTU);
    printCorrelationWithTemp(ge0406, "counts", "peak0406", handledTU);

    delete ge0406;
    delete ge16;
    delete ge06;
    delete ge04;

    if(f != nullptr) {
	f->Close();
	delete f;
    }

    delete th;
    delete cGraph;
}



void GraphPrinter::printTempCorrectFittingGraph() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);

    TFile* f = nullptr;
    if(handledTU == "P2H") {
	if(quantity == "Energy")
	    f = new TFile("ready/TempCorrectFitResult.root", "READ");
	else if(quantity == "Shifting")
	    f = new TFile("ready/TempCorrectFitResultS.root", "READ");
    } else if(handledTU == "P6H")
	f = new TFile("ready/TempCorrectFitResult6.root", "READ");

    TTree* fitTree;
    int entryNo;
    int year;
    int month;
    int day;
    int hour;
    int min;
    double sec;

    double cg04;
    double cg06;
    double cg16;
    double cg0406;

    double cge04;
    double cge06;
    double cge16;
    double cge0406;

    f->GetObject("dataFitting", fitTree);
    fitTree->SetBranchAddress("entryNo", &entryNo);
    fitTree->SetBranchAddress("year", &year);
    fitTree->SetBranchAddress("month", &month);
    fitTree->SetBranchAddress("day", &day);
    fitTree->SetBranchAddress("hour", &hour);
    fitTree->SetBranchAddress("minute", &min);
    fitTree->SetBranchAddress("second", &sec);

    fitTree->SetBranchAddress("cg04", &cg04);
    fitTree->SetBranchAddress("cg06", &cg06);
    fitTree->SetBranchAddress("cg16", &cg16);
    fitTree->SetBranchAddress("cg0406", &cg0406);

    fitTree->SetBranchAddress("cge04", &cge04);
    fitTree->SetBranchAddress("cge06", &cge06);
    fitTree->SetBranchAddress("cge16", &cge16);
    fitTree->SetBranchAddress("cge0406", &cge0406);

    TFile* foutlier = nullptr;
    if(handledTU == "P2H") {
	if(quantity == "Energy")
	    foutlier = new TFile("ready/NormalizedFitResult.root", "READ");
	else if(quantity == "Shifting")
	    foutlier = new TFile("ready/NormalizedFitResultS.root", "READ");
    } else if(handledTU == "P6H")
	foutlier = new TFile("ready/NormalizedFitResult6.root", "READ");

    TTree* outlierTree;
    int entryNoOutlier;
    bool isoutlier04;
    bool isoutlier06;
    bool isoutlier16;
    bool isoutlier0406;

    foutlier->GetObject("dataFitting", outlierTree);
    outlierTree->SetBranchAddress("entryNo", &entryNoOutlier);
    outlierTree->SetBranchAddress("isoutlier04", &isoutlier04);
    outlierTree->SetBranchAddress("isoutlier06", &isoutlier06);
    outlierTree->SetBranchAddress("isoutlier16", &isoutlier16);
    outlierTree->SetBranchAddress("isoutlier0406", &isoutlier0406);

    TGraphErrors* ge04 = new TGraphErrors();
    TGraphErrors* ge06 = new TGraphErrors();
    TGraphErrors* ge16 = new TGraphErrors();
    TGraphErrors* ge0406 = new TGraphErrors();

    double mean04 = 0.;
    double mean06 = 0.;
    double mean16 = 0.;
    double mean0406 = 0.;
    double std04 = 0.;
    double std06 = 0.;
    double std16 = 0.;
    double std0406 = 0.;
    int ntimebin04 = 0;
    int ntimebin06 = 0;
    int ntimebin16 = 0;
    int ntimebin0406 = 0;

    Long64_t nentries = fitTree->GetEntries();
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	fitTree->GetEntry(entry);
	outlierTree->GetEntry(entry);
	if(entryNo == entryNoOutlier) {
	    TDatime* tdt = new TDatime(year, month, day, hour, min, (int)sec);

	    if(!isoutlier16) {
		ge04->SetPoint(ge04->GetN(), tdt->Convert(), cg04);
		ge04->SetPointError(ge04->GetN() - 1, 0., cge04);
		mean04 += cg04;
		std04 += cg04*cg04;
		ntimebin04++;
	    }

	    if(!isoutlier16) {
		ge06->SetPoint(ge06->GetN(), tdt->Convert(), cg06);
		ge06->SetPointError(ge06->GetN() - 1, 0., cge06);
		mean06 += cg06;
		std06 += cg06*cg06;
		ntimebin06++;
	    }

	    if(!isoutlier16) {
		ge16->SetPoint(ge16->GetN(), tdt->Convert(), cg16);
		ge16->SetPointError(ge16->GetN() - 1, 0., cge16);
		mean16 += cg16;
		std16 += cg16*cg16;
		ntimebin16++;
	    }

	    if(!isoutlier16) {
		ge0406->SetPoint(ge0406->GetN(), tdt->Convert(), cg0406);
		ge0406->SetPointError(ge0406->GetN() - 1, 0., cge0406);
		mean0406 += cg0406;
		std0406 += cg0406*cg0406;
		ntimebin0406++;
	    }

	    delete tdt;
	} else {
	    cout << "Both entry numbers are not match!!!" << endl;
	    exit(0);
	}
    }

    mean04 /= ntimebin04;
    mean06 /= ntimebin06;
    mean16 /= ntimebin16;
    mean0406 /= ntimebin0406;

    std04 -= ntimebin04*mean04*mean04;
    std06 -= ntimebin06*mean06*mean06;
    std16 -= ntimebin16*mean16*mean16;
    std0406 -= ntimebin0406*mean0406*mean0406;
    std04 = TMath::Sqrt(std04/(ntimebin04 - 1));
    std06 = TMath::Sqrt(std06/(ntimebin06 - 1));
    std16 = TMath::Sqrt(std16/(ntimebin16 - 1));
    std0406 = TMath::Sqrt(std0406/(ntimebin0406 - 1));

    double err04 = std04/TMath::Sqrt(ntimebin04);
    double err06 = std06/TMath::Sqrt(ntimebin06);
    double err16 = std16/TMath::Sqrt(ntimebin16);
    double err0406 = std0406/TMath::Sqrt(ntimebin0406);

    printWithErrorBand(ge04, mean04, err04, "correctedCGauss", "peak04", handledTU);
    printWithErrorBand(ge06, mean06, err06, "correctedCGauss", "peak06", handledTU);
    printWithErrorBand(ge16, mean16, err16, "correctedCGauss", "peak16", handledTU);
    printWithErrorBand(ge0406, mean0406, err0406, "correctedCGauss", "peak0406", handledTU);

    delete ge0406;
    delete ge16;
    delete ge06;
    delete ge04;

    if(foutlier != nullptr) {
	foutlier->Close();
	delete foutlier;
    }

    if(f != nullptr) {
	f->Close();
	delete f;
    }

    delete cGraph;
}



void GraphPrinter::printTempCorrectCountingGraph() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);

    TFile* f = nullptr;
    if(quantity == "Energy")
	f = new TFile("ready/TempCorrectFluctResult.root", "READ");
    else if(quantity == "Shifting")
	f = new TFile("ready/TempCorrectFluctResultS.root", "READ");

    TTree* countTree;
    int entryNo;
    int year;
    int month;
    int day;
    int hour;
    int min;
    double sec;

    double counts04;
    double counts06;
    double counts16;
    double counts0406;

    double countse04;
    double countse06;
    double countse16;
    double countse0406;

    f->GetObject("dataCounts", countTree);
    countTree->SetBranchAddress("entryNo", &entryNo);
    countTree->SetBranchAddress("year", &year);
    countTree->SetBranchAddress("month", &month);
    countTree->SetBranchAddress("day", &day);
    countTree->SetBranchAddress("hour", &hour);
    countTree->SetBranchAddress("minute", &min);
    countTree->SetBranchAddress("second", &sec);

    countTree->SetBranchAddress("counts04", &counts04);
    countTree->SetBranchAddress("counts06", &counts06);
    countTree->SetBranchAddress("counts16", &counts16);
    countTree->SetBranchAddress("counts0406", &counts0406);

    countTree->SetBranchAddress("countse04", &countse04);
    countTree->SetBranchAddress("countse06", &countse06);
    countTree->SetBranchAddress("countse16", &countse16);
    countTree->SetBranchAddress("countse0406", &countse0406);

    TFile* foutlier = nullptr;
    if(quantity == "Energy")
	foutlier = new TFile("ready/NormalizedFluctResult.root", "READ");
    else if(quantity == "Shifting")
	foutlier = new TFile("ready/NormalizedFluctResultS.root", "READ");

    TTree* outlierTree;
    int entryNoOutlier;
    bool isoutlier04;
    bool isoutlier06;
    bool isoutlier16;
    bool isoutlier0406;

    foutlier->GetObject("dataCounts", outlierTree);
    outlierTree->SetBranchAddress("entryNo", &entryNoOutlier);
    outlierTree->SetBranchAddress("isoutlier04", &isoutlier04);
    outlierTree->SetBranchAddress("isoutlier06", &isoutlier06);
    outlierTree->SetBranchAddress("isoutlier16", &isoutlier16);
    outlierTree->SetBranchAddress("isoutlier0406", &isoutlier0406);

    TGraphErrors* ge04 = new TGraphErrors();
    TGraphErrors* ge06 = new TGraphErrors();
    TGraphErrors* ge16 = new TGraphErrors();
    TGraphErrors* ge0406 = new TGraphErrors();

    double mean04 = 0.;
    double mean06 = 0.;
    double mean16 = 0.;
    double mean0406 = 0.;
    double std04 = 0.;
    double std06 = 0.;
    double std16 = 0.;
    double std0406 = 0.;
    int ntimebin04 = 0;
    int ntimebin06 = 0;
    int ntimebin16 = 0;
    int ntimebin0406 = 0;

    Long64_t nentries = countTree->GetEntries();
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	countTree->GetEntry(entry);
	outlierTree->GetEntry(entry);
	if(entryNo == entryNoOutlier) {
	    TDatime* tdt = new TDatime(year, month, day, hour, min, (int)sec);

	    if(!isoutlier16) {
		ge04->SetPoint(ge04->GetN(), tdt->Convert(), counts04);
		ge04->SetPointError(ge04->GetN() - 1, 0., countse04);
		mean04 += counts04;
		std04 += counts04*counts04;
		ntimebin04++;
	    }

	    if(!isoutlier16) {
		ge06->SetPoint(ge06->GetN(), tdt->Convert(), counts06);
		ge06->SetPointError(ge06->GetN() - 1, 0., countse06);
		mean06 += counts06;
		std06 += counts06*counts06;
		ntimebin06++;
	    }

	    if(!isoutlier16) {
		ge16->SetPoint(ge16->GetN(), tdt->Convert(), counts16);
		ge16->SetPointError(ge16->GetN() - 1, 0., countse16);
		mean16 += counts16;
		std16 += counts16*counts16;
		ntimebin16++;
	    }

	    if(!isoutlier16) {
		ge0406->SetPoint(ge0406->GetN(), tdt->Convert(), counts0406);
		ge0406->SetPointError(ge0406->GetN() - 1, 0., countse0406);
		mean0406 += counts0406;
		std0406 += counts0406*counts0406;
		ntimebin0406++;
	    }

	    delete tdt;
	} else {
	    cout << "Both entry numbers are not match!!!" << endl;
	    exit(0);
	}
    }

    mean04 /= ntimebin04;
    mean06 /= ntimebin06;
    mean16 /= ntimebin16;
    mean0406 /= ntimebin0406;

    std04 -= ntimebin04*mean04*mean04;
    std06 -= ntimebin06*mean06*mean06;
    std16 -= ntimebin16*mean16*mean16;
    std0406 -= ntimebin0406*mean0406*mean0406;
    std04 = TMath::Sqrt(std04/(ntimebin04 - 1));
    std06 = TMath::Sqrt(std06/(ntimebin06 - 1));
    std16 = TMath::Sqrt(std16/(ntimebin16 - 1));
    std0406 = TMath::Sqrt(std0406/(ntimebin0406 - 1));

    double err04 = std04/TMath::Sqrt(ntimebin04);
    double err06 = std06/TMath::Sqrt(ntimebin06);
    double err16 = std16/TMath::Sqrt(ntimebin16);
    double err0406 = std0406/TMath::Sqrt(ntimebin0406);

    printWithErrorBand(ge04, mean04, err04, "correctedCounts", "peak04", handledTU);
    printWithErrorBand(ge06, mean06, err06, "correctedCounts", "peak06", handledTU);
    printWithErrorBand(ge16, mean16, err16, "correctedCounts", "peak16", handledTU);
    printWithErrorBand(ge0406, mean0406, err0406, "correctedCounts", "peak0406", handledTU);

    delete ge0406;
    delete ge16;
    delete ge06;
    delete ge04;

    if(foutlier != nullptr) {
	foutlier->Close();
	delete foutlier;
    }

    if(f != nullptr) {
	f->Close();
	delete f;
    }

    delete cGraph;
}



void GraphPrinter::printNormFittingGraph() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);

    TFile* f = nullptr;
    if(handledTU == "P2H") {
	if(quantity == "Energy")
	    f = new TFile("ready/NormalizedFitResult.root", "READ");
	else if(quantity == "Shifting")
	    f = new TFile("ready/NormalizedFitResultS.root", "READ");
    } else if(handledTU == "P6H")
	f = new TFile("ready/NormalizedFitResult6.root", "READ");

    TTree* fitTree;
    int entryNo;
    int year;
    int month;
    int day;
    int hour;
    int min;
    double sec;

    double cgn04;
    double cgn06;
    double cgn16;
    double cgn0406;

    double cgne04;
    double cgne06;
    double cgne16;
    double cgne0406;

    bool isoutlier04;
    bool isoutlier06;
    bool isoutlier16;
    bool isoutlier0406;

    f->GetObject("dataFitting", fitTree);
    fitTree->SetBranchAddress("entryNo", &entryNo);
    fitTree->SetBranchAddress("year", &year);
    fitTree->SetBranchAddress("month", &month);
    fitTree->SetBranchAddress("day", &day);
    fitTree->SetBranchAddress("hour", &hour);
    fitTree->SetBranchAddress("minute", &min);
    fitTree->SetBranchAddress("second", &sec);

    fitTree->SetBranchAddress("cgn04", &cgn04);
    fitTree->SetBranchAddress("cgn06", &cgn06);
    fitTree->SetBranchAddress("cgn16", &cgn16);
    fitTree->SetBranchAddress("cgn0406", &cgn0406);

    fitTree->SetBranchAddress("cgne04", &cgne04);
    fitTree->SetBranchAddress("cgne06", &cgne06);
    fitTree->SetBranchAddress("cgne16", &cgne16);
    fitTree->SetBranchAddress("cgne0406", &cgne0406);

    fitTree->SetBranchAddress("isoutlier04", &isoutlier04);
    fitTree->SetBranchAddress("isoutlier06", &isoutlier06);
    fitTree->SetBranchAddress("isoutlier16", &isoutlier16);
    fitTree->SetBranchAddress("isoutlier0406", &isoutlier0406);

    TGraphErrors* ge04 = new TGraphErrors();
    TGraphErrors* ge06 = new TGraphErrors();
    TGraphErrors* ge16 = new TGraphErrors();
    TGraphErrors* ge0406 = new TGraphErrors();

    double mean04 = 0.;
    double mean06 = 0.;
    double mean16 = 0.;
    double mean0406 = 0.;
    double std04 = 0.;
    double std06 = 0.;
    double std16 = 0.;
    double std0406 = 0.;
    int ntimebin04 = 0;
    int ntimebin06 = 0;
    int ntimebin16 = 0;
    int ntimebin0406 = 0;

    Long64_t nentries = fitTree->GetEntries();
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	fitTree->GetEntry(entry);
	TDatime* tdt = new TDatime(year, month, day, hour, min, (int)sec);
	if(!isoutlier16) {
	    ge04->SetPoint(ge04->GetN(), tdt->Convert(), cgn04);
	    ge04->SetPointError(ge04->GetN() - 1, 0, cgne04);

	    mean04 += cgn04;
	    std04 += cgn04*cgn04;
	    ntimebin04++;
	}

	if(!isoutlier16) {
	    ge06->SetPoint(ge06->GetN(), tdt->Convert(), cgn06);
	    ge06->SetPointError(ge06->GetN() - 1, 0, cgne06);

	    mean06 += cgn06;
	    std06 += cgn06*cgn06;
	    ntimebin06++;
	}

	if(!isoutlier16) {
	    ge16->SetPoint(ge16->GetN(), tdt->Convert(), cgn16);
	    ge16->SetPointError(ge16->GetN() - 1, 0, cgne16);

	    mean16 += cgn16;
	    std16 += cgn16*cgn16;
	    ntimebin16++;
	}

	if(!isoutlier16) {
	    ge0406->SetPoint(ge0406->GetN(), tdt->Convert(), cgn0406);
	    ge0406->SetPointError(ge0406->GetN() - 1, 0, cgne0406);

	    mean0406 += cgn0406;
	    std0406 += cgn0406*cgn0406;
	    ntimebin0406++;
	}

	delete tdt;
    }

    mean04 /= ntimebin04;
    mean06 /= ntimebin06;
    mean16 /= ntimebin16;
    mean0406 /= ntimebin0406;

    std04 -= ntimebin04*mean04*mean04;
    std06 -= ntimebin06*mean06*mean06;
    std16 -= ntimebin16*mean16*mean16;
    std0406 -= ntimebin0406*mean0406*mean0406;
    std04 = TMath::Sqrt(std04/(ntimebin04 - 1));
    std06 = TMath::Sqrt(std06/(ntimebin06 - 1));
    std16 = TMath::Sqrt(std16/(ntimebin16 - 1));
    std0406 = TMath::Sqrt(std0406/(ntimebin0406 - 1));

    double err04 = std04/TMath::Sqrt(ntimebin04);
    double err06 = std06/TMath::Sqrt(ntimebin06);
    double err16 = std16/TMath::Sqrt(ntimebin16);
    double err0406 = std0406/TMath::Sqrt(ntimebin0406);

    printWithErrorBand(ge04, mean04, err04, "normCGauss", "peak04", handledTU);
    printWithErrorBand(ge06, mean06, err06, "normCGauss", "peak06", handledTU);
    printWithErrorBand(ge16, mean16, err16, "normCGauss", "peak16", handledTU);
    printWithErrorBand(ge0406, mean0406, err0406, "normCGauss", "peak0406", handledTU);

    delete ge0406;
    delete ge16;
    delete ge06;
    delete ge04;

    if(f != nullptr) {
	f->Close();
	delete f;
    }

    delete cGraph;
}



void GraphPrinter::printNormCountingGraph() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);

    TFile* f = nullptr;
    if(quantity == "Energy")
	f = new TFile("ready/NormalizedFluctResult.root", "READ");
    else if(quantity == "Shifting")
	f = new TFile("ready/NormalizedFluctResultS.root", "READ");

    TTree* countTree;
    int entryNo;
    int year;
    int month;
    int day;
    int hour;
    int min;
    double sec;

    double countsn04;
    double countsn06;
    double countsn16;
    double countsn0406;

    double countsne04;
    double countsne06;
    double countsne16;
    double countsne0406;

    bool isoutlier04;
    bool isoutlier06;
    bool isoutlier16;
    bool isoutlier0406;

    f->GetObject("dataCounts", countTree);
    countTree->SetBranchAddress("entryNo", &entryNo);
    countTree->SetBranchAddress("year", &year);
    countTree->SetBranchAddress("month", &month);
    countTree->SetBranchAddress("day", &day);
    countTree->SetBranchAddress("hour", &hour);
    countTree->SetBranchAddress("minute", &min);
    countTree->SetBranchAddress("second", &sec);

    countTree->SetBranchAddress("countsn04", &countsn04);
    countTree->SetBranchAddress("countsn06", &countsn06);
    countTree->SetBranchAddress("countsn16", &countsn16);
    countTree->SetBranchAddress("countsn0406", &countsn0406);

    countTree->SetBranchAddress("countsne04", &countsne04);
    countTree->SetBranchAddress("countsne06", &countsne06);
    countTree->SetBranchAddress("countsne16", &countsne16);
    countTree->SetBranchAddress("countsne0406", &countsne0406);

    countTree->SetBranchAddress("isoutlier04", &isoutlier04);
    countTree->SetBranchAddress("isoutlier06", &isoutlier06);
    countTree->SetBranchAddress("isoutlier16", &isoutlier16);
    countTree->SetBranchAddress("isoutlier0406", &isoutlier0406);

    TGraphErrors* ge04 = new TGraphErrors();
    TGraphErrors* ge06 = new TGraphErrors();
    TGraphErrors* ge16 = new TGraphErrors();
    TGraphErrors* ge0406 = new TGraphErrors();

    double mean04 = 0.;
    double mean06 = 0.;
    double mean16 = 0.;
    double mean0406 = 0.;
    double std04 = 0.;
    double std06 = 0.;
    double std16 = 0.;
    double std0406 = 0.;
    int ntimebin04 = 0;
    int ntimebin06 = 0;
    int ntimebin16 = 0;
    int ntimebin0406 = 0;

    Long64_t nentries = countTree->GetEntries();
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	countTree->GetEntry(entry);
	TDatime* tdt = new TDatime(year, month, day, hour, min, (int)sec);
	if(!isoutlier16 && !isoutlier04) {
	    ge04->SetPoint(ge04->GetN(), tdt->Convert(), countsn04);
	    ge04->SetPointError(ge04->GetN() - 1, 0, countsne04);

	    mean04 += countsn04;
	    std04 += countsn04*countsn04;
	    ntimebin04++;
	}

	if(!isoutlier16 && !isoutlier06) {
	    ge06->SetPoint(ge06->GetN(), tdt->Convert(), countsn06);
	    ge06->SetPointError(ge06->GetN() - 1, 0, countsne06);

	    mean06 += countsn06;
	    std06 += countsn06*countsn06;
	    ntimebin06++;
	}

	if(!isoutlier16) {
	    ge16->SetPoint(ge16->GetN(), tdt->Convert(), countsn16);
	    ge16->SetPointError(ge16->GetN() - 1, 0, countsne16);

	    mean16 += countsn16;
	    std16 += countsn16*countsn16;
	    ntimebin16++;
	}

	if(!isoutlier16 && !isoutlier0406) {
	    ge0406->SetPoint(ge0406->GetN(), tdt->Convert(), countsn0406);
	    ge0406->SetPointError(ge0406->GetN() - 1, 0, countsne0406);

	    mean0406 += countsn0406;
	    std0406 += countsn0406*countsn0406;
	    ntimebin0406++;
	}

	delete tdt;
    }

    mean04 /= ntimebin04;
    mean06 /= ntimebin06;
    mean16 /= ntimebin16;
    mean0406 /= ntimebin0406;

    std04 -= ntimebin04*mean04*mean04;
    std06 -= ntimebin06*mean06*mean06;
    std16 -= ntimebin16*mean16*mean16;
    std0406 -= ntimebin0406*mean0406*mean0406;
    std04 = TMath::Sqrt(std04/(ntimebin04 - 1));
    std06 = TMath::Sqrt(std06/(ntimebin06 - 1));
    std16 = TMath::Sqrt(std16/(ntimebin16 - 1));
    std0406 = TMath::Sqrt(std0406/(ntimebin0406 - 1));

    double err04 = std04/TMath::Sqrt(ntimebin04);
    double err06 = std06/TMath::Sqrt(ntimebin06);
    double err16 = std16/TMath::Sqrt(ntimebin16);
    double err0406 = std0406/TMath::Sqrt(ntimebin0406);

    printWithErrorBand(ge04, mean04, err04, "normCounts", "peak04", handledTU);
    printWithErrorBand(ge06, mean06, err06, "normCounts", "peak06", handledTU);
    printWithErrorBand(ge16, mean16, err16, "normCounts", "peak16", handledTU);
    printWithErrorBand(ge0406, mean0406, err0406, "normCounts", "peak0406", handledTU);

    delete ge0406;
    delete ge16;
    delete ge06;
    delete ge04;

    if(f != nullptr) {
	f->Close();
	delete f;
    }

    delete cGraph;
}



void GraphPrinter::printCompareCorrectionFittingGraph() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);
    TempHumi* th = new TempHumi();

    TFile* fraw = nullptr;
    if(handledTU == "P2H") {
	if(quantity == "Energy")
	    fraw = new TFile("ready/fitResult.root", "READ");
	else if(quantity == "Shifting")
	    fraw = new TFile("ready/fitResultS.root", "READ");
    } else if(handledTU == "P6H")
	fraw = new TFile("ready/fitResult6.root", "READ");

    TTree* rawTree;
    int rentryNo;
    int ryear;
    int rmonth;
    int rday;
    int rhour;
    int rmin;
    double rsec;

    double raw04;
    double raw06;
    double raw16;
    double raw0406;

    double rawe04;
    double rawe06;
    double rawe16;
    double rawe0406;

    fraw->GetObject("dataFitting", rawTree);
    rawTree->SetBranchAddress("entryNo", &rentryNo);
    rawTree->SetBranchAddress("year", &ryear);
    rawTree->SetBranchAddress("month", &rmonth);
    rawTree->SetBranchAddress("day", &rday);
    rawTree->SetBranchAddress("hour", &rhour);
    rawTree->SetBranchAddress("minute", &rmin);
    rawTree->SetBranchAddress("second", &rsec);

    rawTree->SetBranchAddress("cg04", &raw04);
    rawTree->SetBranchAddress("cg06", &raw06);
    rawTree->SetBranchAddress("cg16", &raw16);
    rawTree->SetBranchAddress("cg0406", &raw0406);

    rawTree->SetBranchAddress("cge04", &rawe04);
    rawTree->SetBranchAddress("cge06", &rawe06);
    rawTree->SetBranchAddress("cge16", &rawe16);
    rawTree->SetBranchAddress("cge0406", &rawe0406);

    TFile* fcor = nullptr;
    if(handledTU == "P2H") {
	if(quantity == "Energy")
	    fcor = new TFile("ready/TempCorrectFitResult.root", "READ");
	else if(quantity == "Shifting")
	    fcor = new TFile("ready/TempCorrectFitResultS.root", "READ");
    } else if(handledTU == "P6H")
	fcor = new TFile("ready/TempCorrectFitResult6.root", "READ");

    TTree* corTree;
    int centryNo;
    int cyear;
    int cmonth;
    int cday;
    int chour;
    int cmin;
    double csec;

    double cor04;
    double cor06;
    double cor16;
    double cor0406;

    double core04;
    double core06;
    double core16;
    double core0406;

    fcor->GetObject("dataFitting", corTree);
    corTree->SetBranchAddress("entryNo", &centryNo);
    corTree->SetBranchAddress("year", &cyear);
    corTree->SetBranchAddress("month", &cmonth);
    corTree->SetBranchAddress("day", &cday);
    corTree->SetBranchAddress("hour", &chour);
    corTree->SetBranchAddress("minute", &cmin);
    corTree->SetBranchAddress("second", &csec);

    corTree->SetBranchAddress("cg04", &cor04);
    corTree->SetBranchAddress("cg06", &cor06);
    corTree->SetBranchAddress("cg16", &cor16);
    corTree->SetBranchAddress("cg0406", &cor0406);

    corTree->SetBranchAddress("cge04", &core04);
    corTree->SetBranchAddress("cge06", &core06);
    corTree->SetBranchAddress("cge16", &core16);
    corTree->SetBranchAddress("cge0406", &core0406);

    TFile* foutlier = nullptr;
    if(handledTU == "P2H") {
	if(quantity == "Energy")
	    foutlier = new TFile("ready/NormalizedFitResult.root", "READ");
	else if(quantity == "Shifting")
	    foutlier = new TFile("ready/NormalizedFitResultS.root", "READ");
    } else if(handledTU == "P6H")
	foutlier = new TFile("ready/NormalizedFitResult6.root", "READ");

    TTree* outlierTree;
    int entryNoOutlier;
    bool isoutlier04;
    bool isoutlier06;
    bool isoutlier16;
    bool isoutlier0406;

    foutlier->GetObject("dataFitting", outlierTree);
    outlierTree->SetBranchAddress("entryNo", &entryNoOutlier);
    outlierTree->SetBranchAddress("isoutlier04", &isoutlier04);
    outlierTree->SetBranchAddress("isoutlier06", &isoutlier06);
    outlierTree->SetBranchAddress("isoutlier16", &isoutlier16);
    outlierTree->SetBranchAddress("isoutlier0406", &isoutlier0406);

    TGraphErrors* geRaw04 = new TGraphErrors();
    TGraphErrors* geRaw06 = new TGraphErrors();
    TGraphErrors* geRaw16 = new TGraphErrors();
    TGraphErrors* geRaw0406 = new TGraphErrors();

    TGraphErrors* geCor04 = new TGraphErrors();
    TGraphErrors* geCor06 = new TGraphErrors();
    TGraphErrors* geCor16 = new TGraphErrors();
    TGraphErrors* geCor0406 = new TGraphErrors();

    Long64_t rnentries = rawTree->GetEntries();
    for(Long64_t entry = 0; entry < rnentries; ++entry) {
	rawTree->GetEntry(entry);
	outlierTree->GetEntry(entry);
	if(rentryNo == entryNoOutlier) {
	    if(isoutlier16)
		continue;

	    TDatime* tdt = new TDatime(ryear, rmonth, rday, rhour, rmin, (int)rsec);
	    geRaw04->SetPoint(geRaw04->GetN(), tdt->Convert(), raw04);
	    geRaw06->SetPoint(geRaw06->GetN(), tdt->Convert(), raw06);
	    geRaw16->SetPoint(geRaw16->GetN(), tdt->Convert(), raw16);
	    geRaw0406->SetPoint(geRaw0406->GetN(), tdt->Convert(), raw0406);

	    geRaw04->SetPointError(geRaw04->GetN() - 1, 0., rawe04);
	    geRaw06->SetPointError(geRaw06->GetN() - 1, 0., rawe06);
	    geRaw16->SetPointError(geRaw16->GetN() - 1, 0., rawe16);
	    geRaw0406->SetPointError(geRaw0406->GetN() - 1, 0., rawe0406);

	    delete tdt;
	}
    }

    Long64_t cnentries = corTree->GetEntries();
    for(Long64_t entry = 0; entry < cnentries; ++entry) {
	corTree->GetEntry(entry);
	outlierTree->GetEntry(entry);
	if(centryNo == entryNoOutlier) {
	    if(isoutlier16)
		continue;

	    TDatime* tdt = new TDatime(cyear, cmonth, cday, chour, cmin, (int)csec);
	    geCor04->SetPoint(geCor04->GetN(), tdt->Convert(), cor04);
	    geCor06->SetPoint(geCor06->GetN(), tdt->Convert(), cor06);
	    geCor16->SetPoint(geCor16->GetN(), tdt->Convert(), cor16);
	    geCor0406->SetPoint(geCor0406->GetN(), tdt->Convert(), cor0406);

	    geCor04->SetPointError(geCor04->GetN() - 1, 0., core04);
	    geCor06->SetPointError(geCor06->GetN() - 1, 0., core06);
	    geCor16->SetPointError(geCor16->GetN() - 1, 0., core16);
	    geCor0406->SetPointError(geCor0406->GetN() - 1, 0., core0406);

	    delete tdt;
	}
    }

    printCompareMultiG(geRaw04, geCor04, "cGauss", "peak04", handledTU);
    printCompareMultiG(geRaw06, geCor06, "cGauss", "peak06", handledTU);
    printCompareMultiG(geRaw16, geCor16, "cGauss", "peak16", handledTU);
    printCompareMultiG(geRaw0406, geCor0406, "cGauss", "peak0406", handledTU);

    delete geCor0406;
    delete geCor16;
    delete geCor06;
    delete geCor04;

    delete geRaw0406;
    delete geRaw16;
    delete geRaw06;
    delete geRaw04;

    if(foutlier != nullptr) {
	foutlier->Close();
	delete foutlier;
    }

    if(fcor != nullptr) {
	fcor->Close();
	delete fcor;
    }

    if(fraw != nullptr) {
	fraw->Close();
	delete fraw;
    }

    delete th;
    delete cGraph;
}



void GraphPrinter::printCompareCorrectionCountingGraph() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);
    TempHumi* th = new TempHumi();

    TFile* fraw = nullptr;
    if(quantity == "Energy")
	fraw = new TFile("ready/fluctResult.root", "READ");
    else if(quantity == "Shifting")
	fraw = new TFile("ready/fluctResultS.root", "READ");

    TTree* rawTree;
    int rentryNo;
    int ryear;
    int rmonth;
    int rday;
    int rhour;
    int rmin;
    double rsec;

    double raw04;
    double raw06;
    double raw16;
    double raw0406;

    double rawe04;
    double rawe06;
    double rawe16;
    double rawe0406;

    fraw->GetObject("dataCounts", rawTree);
    rawTree->SetBranchAddress("entryNo", &rentryNo);
    rawTree->SetBranchAddress("year", &ryear);
    rawTree->SetBranchAddress("month", &rmonth);
    rawTree->SetBranchAddress("day", &rday);
    rawTree->SetBranchAddress("hour", &rhour);
    rawTree->SetBranchAddress("minute", &rmin);
    rawTree->SetBranchAddress("second", &rsec);

    rawTree->SetBranchAddress("counts04", &raw04);
    rawTree->SetBranchAddress("counts06", &raw06);
    rawTree->SetBranchAddress("counts16", &raw16);
    rawTree->SetBranchAddress("counts0406", &raw0406);

    rawTree->SetBranchAddress("countse04", &rawe04);
    rawTree->SetBranchAddress("countse06", &rawe06);
    rawTree->SetBranchAddress("countse16", &rawe16);
    rawTree->SetBranchAddress("countse0406", &rawe0406);

    TFile* fcor = nullptr;
    if(quantity == "Energy")
	fcor = new TFile("ready/TempCorrectFluctResult.root", "READ");
    else if(quantity == "Shifting")
	fcor = new TFile("ready/TempCorrectFluctResultS.root", "READ");

    TTree* corTree;
    int centryNo;
    int cyear;
    int cmonth;
    int cday;
    int chour;
    int cmin;
    double csec;

    double cor04;
    double cor06;
    double cor16;
    double cor0406;

    double core04;
    double core06;
    double core16;
    double core0406;

    fcor->GetObject("dataCounts", corTree);
    corTree->SetBranchAddress("entryNo", &centryNo);
    corTree->SetBranchAddress("year", &cyear);
    corTree->SetBranchAddress("month", &cmonth);
    corTree->SetBranchAddress("day", &cday);
    corTree->SetBranchAddress("hour", &chour);
    corTree->SetBranchAddress("minute", &cmin);
    corTree->SetBranchAddress("second", &csec);

    corTree->SetBranchAddress("counts04", &cor04);
    corTree->SetBranchAddress("counts06", &cor06);
    corTree->SetBranchAddress("counts16", &cor16);
    corTree->SetBranchAddress("counts0406", &cor0406);

    corTree->SetBranchAddress("countse04", &core04);
    corTree->SetBranchAddress("countse06", &core06);
    corTree->SetBranchAddress("countse16", &core16);
    corTree->SetBranchAddress("countse0406", &core0406);

    TFile* foutlier = nullptr;
    if(quantity == "Energy")
	foutlier = new TFile("ready/NormalizedFluctResult.root", "READ");
    else if(quantity == "Shifting")
	foutlier = new TFile("ready/NormalizedFluctResultS.root", "READ");

    TTree* outlierTree;
    int entryNoOutlier;
    bool isoutlier16;

    foutlier->GetObject("dataCounts", outlierTree);
    outlierTree->SetBranchAddress("entryNo", &entryNoOutlier);
    outlierTree->SetBranchAddress("isoutlier16", &isoutlier16);

    TGraphErrors* geRaw04 = new TGraphErrors();
    TGraphErrors* geRaw06 = new TGraphErrors();
    TGraphErrors* geRaw16 = new TGraphErrors();
    TGraphErrors* geRaw0406 = new TGraphErrors();

    TGraphErrors* geCor04 = new TGraphErrors();
    TGraphErrors* geCor06 = new TGraphErrors();
    TGraphErrors* geCor16 = new TGraphErrors();
    TGraphErrors* geCor0406 = new TGraphErrors();

    Long64_t rnentries = rawTree->GetEntries();
    for(Long64_t entry = 0; entry < rnentries; ++entry) {
	rawTree->GetEntry(entry);
	outlierTree->GetEntry(entry);
	if(rentryNo == entryNoOutlier) {
	    if(isoutlier16)
		continue;

	    TDatime* tdt = new TDatime(ryear, rmonth, rday, rhour, rmin, (int)rsec);
	    geRaw04->SetPoint(geRaw04->GetN(), tdt->Convert(), raw04);
	    geRaw06->SetPoint(geRaw06->GetN(), tdt->Convert(), raw06);
	    geRaw16->SetPoint(geRaw16->GetN(), tdt->Convert(), raw16);
	    geRaw0406->SetPoint(geRaw0406->GetN(), tdt->Convert(), raw0406);

	    geRaw04->SetPointError(geRaw04->GetN() - 1, 0., rawe04);
	    geRaw06->SetPointError(geRaw06->GetN() - 1, 0., rawe06);
	    geRaw16->SetPointError(geRaw16->GetN() - 1, 0., rawe16);
	    geRaw0406->SetPointError(geRaw0406->GetN() - 1, 0., rawe0406);

	    delete tdt;
	}
    }

    Long64_t cnentries = corTree->GetEntries();
    for(Long64_t entry = 0; entry < cnentries; ++entry) {
	corTree->GetEntry(entry);
	outlierTree->GetEntry(entry);
	if(centryNo == entryNoOutlier) {
	    if(isoutlier16)
		continue;

	    TDatime* tdt = new TDatime(cyear, cmonth, cday, chour, cmin, (int)csec);
	    geCor04->SetPoint(geCor04->GetN(), tdt->Convert(), cor04);
	    geCor06->SetPoint(geCor06->GetN(), tdt->Convert(), cor06);
	    geCor16->SetPoint(geCor16->GetN(), tdt->Convert(), cor16);
	    geCor0406->SetPoint(geCor0406->GetN(), tdt->Convert(), cor0406);

	    geCor04->SetPointError(geCor04->GetN() - 1, 0., core04);
	    geCor06->SetPointError(geCor06->GetN() - 1, 0., core06);
	    geCor16->SetPointError(geCor16->GetN() - 1, 0., core16);
	    geCor0406->SetPointError(geCor0406->GetN() - 1, 0., core0406);

	    delete tdt;
	}
    }

    printCompareMultiG(geRaw04, geCor04, "counts", "peak04", handledTU);
    printCompareMultiG(geRaw06, geCor06, "counts", "peak06", handledTU);
    printCompareMultiG(geRaw16, geCor16, "counts", "peak16", handledTU);
    printCompareMultiG(geRaw0406, geCor0406, "counts", "peak0406", handledTU);

    delete geCor0406;
    delete geCor16;
    delete geCor06;
    delete geCor04;

    delete geRaw0406;
    delete geRaw16;
    delete geRaw06;
    delete geRaw04;

    if(foutlier != nullptr) {
	foutlier->Close();
	delete foutlier;
    }

    if(fcor != nullptr) {
	fcor->Close();
	delete fcor;
    }

    if(fraw != nullptr) {
	fraw->Close();
	delete fraw;
    }

    delete th;
    delete cGraph;
}



void GraphPrinter::printOverlapExpAndSimulation() {
    TCanvas* c = new TCanvas("c", "c", 1400, 800);
    
    TFile* fexp = new TFile("ready/0418to0607oneHist.root", "READ");
    TFile* fsim = new TFile("ready/Rn222_smear_output.root", "READ");

    TH1D* hexp = (TH1D*)fexp->Get("HistoCh0/20210418/000000");
    TH1D* hsim = (TH1D*)fsim->Get("ETotalCore");

    hexp->Scale(1./hexp->GetEntries());
    hsim->Scale(1./hsim->GetEntries());

    hexp->SetLineColor(kBlue);
    hsim->SetLineColor(kRed);

    hsim->Draw("HISTO");
    hexp->Draw("HISTO SAME");

    hsim->GetXaxis()->SetRangeUser(0., 1.5);

    c->Update();
    c->Print("plotting/test.png");

    fsim->Close();
    delete fsim;

    fexp->Close();
    delete fexp;

    delete c;
}



void GraphPrinter::printOverlapExpAndBkg() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);
    TFile* fexp = new TFile("ready/0418to0607oneHist.root", "READ");
    TFile* fbkg = new TFile("ready/0329to0330oneHist.root", "READ");

    TH1D* hexp = (TH1D*)fexp->Get("HistoCh0/20210418/000000");
    TH1D* hbkg = (TH1D*)fbkg->Get("HistoCh0/20210329/200000");

    hexp->Scale(1./919636);
    hbkg->Scale(1./8696.08);

    hexp->SetLineColor(kBlue);
    hbkg->SetLineColor(kRed);

    hbkg->Draw("HISTO");
    hexp->Draw("HISTO SAME");

    hbkg->SetStats(kFALSE);
    TLegend* lg = new TLegend(0.7, 0.65, 0.9, 0.9);
    lg->SetHeader("Legend");
    lg->AddEntry(hexp, "At Hualien");
    lg->AddEntry(hbkg, "In Lab 923");
    lg->Draw();

    cGraph->Update();
    cGraph->Print("plotting/ExpAndBkg.png");

    fbkg->Close();
    delete fbkg;

    fexp->Close();
    delete fexp;

    delete cGraph;
}



void GraphPrinter::printCorrelationWithTemp(TGraph* inputG, string term, string ER, string timeUnit) {
    TCanvas* cG = new TCanvas("cG", "cG", 1400, 800);

    inputG->Draw("AP");
    setGraphAtt(inputG, term + "-Temp", ER, timeUnit);
    setRangeUser(inputG->GetYaxis(), term, ER, timeUnit);

    TFitResultPtr fitptr = inputG->Fit("pol1", "ROB=0.95 WS");

    cG->Update();
    double xleft = 0.6;
    double yupbd = 0.4;
    double ydnbd = 0.1;
    TPaveText* pt = new TPaveText(xleft*gPad->GetUxmax()
				  + (1 - xleft)*gPad->GetUxmin(),
				  ydnbd*gPad->GetUymax()
				  + (1 - ydnbd)*gPad->GetUymin(),
				  gPad->GetUxmax(),
				  yupbd*gPad->GetUymax()
				  + (1 - yupbd)*gPad->GetUymin());

    char tline[200];
    pt->AddText("y = p0 + p1*x");
    sprintf(tline, "#chi^{2}: %.4e", fitptr->Chi2());
    pt->AddText(tline);
    sprintf(tline, "Ndf: %d", fitptr->Ndf());
    pt->AddText(tline);
    sprintf(tline, "#chi^{2}/Ndf: %.2e", fitptr->Chi2()/(double)fitptr->Ndf());
    pt->AddText(tline);
    sprintf(tline, "p0 = %.6e +/- %.6e", fitptr->Parameter(0), fitptr->Error(0));
    pt->AddText(tline);
    sprintf(tline, "p1 = %.6f +/- %.6f", fitptr->Parameter(1), fitptr->Error(1));
    pt->AddText(tline);
    pt->Draw();

    cG->Update();
    string outputfolder = "";
    if(quantity == "Energy" ||
       quantity == "Voltage")
	outputfolder = "plotting/fitting2";
    else if(quantity == "Shifting")
	outputfolder = "plotting/fittingShifting";

    string outputfilename = "";
    if(term == "cGauss")
	outputfilename = "cGaussTemp";
    else if(term == "counts")
	outputfilename = "countsTemp";
    outputfilename = outputfilename + "_" + ER + "_" + timeUnit + ".png";

    string outputPath = outputfolder + "/" + outputfilename;
    cG->Print(outputPath.c_str());

    delete pt;
    delete cG;
}



void GraphPrinter::printCompareMultiG(TGraph* graw, TGraph* gcor, string term, string ER, string timeUnit) {
    TCanvas* cMG = new TCanvas("cMG", "cMG", 1400, 800);

    string rawTerm = "";
    string corTerm = "";
    string multiTerm = "";
    if(term == "cGauss") {
	rawTerm = term;
	corTerm = "correctedCGauss";
	multiTerm = "cGaussCompare";
    } else if(term == "counts") {
	rawTerm = term;
	corTerm = "correctedCounts";
	multiTerm = "countsCompare";
    }

    setGraphAtt(graw, rawTerm, ER, timeUnit);
    setGraphAtt(gcor, corTerm, ER, timeUnit);
    gcor->SetLineColor(kRed);
    gcor->SetMarkerColor(kRed);

    cMG->cd();
    TMultiGraph* mg = new TMultiGraph();
    mg->Add(graw, "AP");
    mg->Add(gcor, "AP");
    setMultiGAtt(mg, multiTerm, ER, timeUnit);
    setRangeUser(mg->GetYaxis(), corTerm, ER, timeUnit);
    mg->Draw("A");

    TLegend* lg = new TLegend(0.6, 0.15, 0.9, 0.3);
    lg->SetHeader("Legend");
    lg->AddEntry(graw, "raw data");
    lg->AddEntry(gcor, "corrected data");
    lg->Draw();    

    cMG->Update();
    string outputfolder = "";
    if(quantity == "Energy" ||
       quantity == "Voltage")
	outputfolder = "plotting/fitting2";
    else if(quantity == "Shifting")
	outputfolder = "plotting/fittingShifting";

    string outputfilename = "";

    if(term == "cGauss") {
	outputfilename = "compareCGauss";
    } else if(term == "counts") {
	outputfilename = "compareCounts";
    }
    outputfilename = outputfilename + "_" + ER + "_" + timeUnit + ".png";

    string outputPath = outputfolder + "/" + outputfilename;
    cMG->Print(outputPath.c_str());

    delete lg;

    mg->GetListOfGraphs()->Clear();
    delete mg;

    delete cMG;
}



void GraphPrinter::printWithErrorBand(TGraph* dataG, double mean, double bandWidth, string term, string ER, string timeUnit) {
    TCanvas* cMG = new TCanvas("cMG", "cMG", 1400, 800);

    TGraphErrors* geMean = new TGraphErrors();
    TGraphErrors* ge1Sigma = new TGraphErrors();
    TGraphErrors* ge2Sigma = new TGraphErrors();
    for(int i = 0; i < 2; i++) {
	TDatime* tdt = nullptr;
	if(i == 0)
	    tdt = new TDatime(2021, 4, 18, 0, 0, 0);
	else if(i == 1)
	    tdt = new TDatime(2021, 6, 7, 0, 0, 0);

	geMean->SetPoint(i, tdt->Convert(), mean);
	ge1Sigma->SetPoint(i, tdt->Convert(), mean);
	ge2Sigma->SetPoint(i, tdt->Convert(), mean);

	geMean->SetPointError(i, 0., 0.);
	ge1Sigma->SetPointError(i, 0., bandWidth);
	ge2Sigma->SetPointError(i, 0., 2*bandWidth);

	delete tdt;
    }

    setGraphAtt(dataG, term, ER, timeUnit);
    setErrorBandAtt(geMean, ge1Sigma, ge2Sigma, "formal");

    cMG->cd();
    TMultiGraph* mg = new TMultiGraph();
    mg->Add(ge2Sigma, "A3");
    mg->Add(ge1Sigma, "A3");
    mg->Add(geMean, "LC");
    mg->Add(dataG, "AP");
    setMultiGAtt(mg, term, ER, timeUnit);
    setRangeUser(mg->GetYaxis(), term, ER, timeUnit);
    mg->Draw("A");

    TLegend* lg = new TLegend(0.6, 0.15, 0.9, 0.3);
    char lline[200];
    lg->SetHeader(takePeakTypeStr(ER, "energyFirst").c_str(), "C");
    lg->AddEntry(dataG, "data");

    if(term.find("norm") != string::npos)
	sprintf(lline, "average");
    else
	sprintf(lline, "average: %.4f", mean);

    lg->AddEntry(geMean, lline);
    sprintf(lline, "1 SE: %.4f", bandWidth);
    lg->AddEntry(ge1Sigma, lline);
    sprintf(lline, "2 SE: %.4f", 2*bandWidth);
    lg->AddEntry(ge2Sigma, lline);
    lg->SetTextSize(0.025);
    lg->Draw();

    cMG->Update();
    string outputfolder = "";
    if(quantity == "Energy" ||
       quantity == "Voltage")
	outputfolder = "plotting/fitting2";
    else if(quantity == "Shifting")
	outputfolder = "plotting/fittingShifting";

    string outputfilename = "";

    if(term == "cGauss") {
	outputfilename = "cGauss";
    } else if(term == "counts") {
	outputfilename = "counts";
    } else if(term == "correctedCGauss") {
	outputfilename = "tempCorrectCGauss";
    } else if(term == "correctedCounts") {
	outputfilename = "tempCorrectCounts";
    } else if(term == "normCGauss") {
	outputfilename = "normCGauss";
    } else if(term == "normCounts") {
	outputfilename = "normCounts";
    }
    outputfilename = outputfilename + "_" + ER + "_" + timeUnit + ".png";

    string outputPath = outputfolder + "/" + outputfilename;
    cMG->Print(outputPath.c_str());

    delete lg;

    mg->GetListOfGraphs()->Clear();
    delete mg;

    delete ge2Sigma;
    delete ge1Sigma;
    delete geMean;

    delete cMG;
}



void GraphPrinter::setErrorBandAtt(TGraphErrors* lineMean, TGraphErrors* band1Sig, TGraphErrors* band2Sig, string style = "formal") {
    Color_t colorLine = kBlack;
    Color_t color1Sig = kGreen;
    Color_t color2Sig = kYellow;

    if(style == "formal") {
	colorLine = kBlack;
	color1Sig = kGreen;
	color2Sig = kYellow;
    }

    lineMean->SetLineColor(colorLine);
    band1Sig->SetLineWidth(0);
    band1Sig->SetMarkerSize(0);
    band1Sig->SetFillColor(color1Sig);
    band2Sig->SetLineWidth(0);
    band2Sig->SetMarkerSize(0);
    band2Sig->SetFillColor(color2Sig);
}



void GraphPrinter::setGraphAtt(TGraph* inputG, string term, string ER, string timeUnit) {
    string graphTitle = "";
    string xAxisTitle = "";
    string yAxisTitle = "";
    if(term == "cGauss") {
	graphTitle = "Coefficient of Gaussian Term";
	xAxisTitle = "Date";
	yAxisTitle = "Coefficient";
    } else if(term == "counts") {
	graphTitle = "Total Counts in an Energy Range";
	xAxisTitle = "Date";
	yAxisTitle = "Counts";
    } else if(term == "normCGauss") {
	graphTitle = "Normalized Coefficient of Gaussian Term";
	xAxisTitle = "Date";

	if(timeUnit == "P2H")
	    yAxisTitle = "C_{2hr}/#bar{C}_{2hr}";
    } else if(term == "normCounts") {
	graphTitle = "Normalized Counting Events";
	xAxisTitle = "Date";

	if(timeUnit == "P2H")
	    yAxisTitle = "N_{2hr}/#bar{N}_{2hr}";
    } else if(term == "correctedCGauss") {
	graphTitle = "Gaussian Coefficient with Correction by Temperature";
	xAxisTitle = "Date";
	yAxisTitle = "Coefficient";
    } else if(term == "correctedCounts") {
	graphTitle = "Total Counts with Correction by Temperature in an Energy Range";
	xAxisTitle = "Date";
	yAxisTitle = "Counts";
    } else if(term == "cGauss-Temp") {
	graphTitle = "Gaussian Coefficient-Temperature Correlation";
	xAxisTitle = "Temperature ({}^{o}C)";
	yAxisTitle = "Coefficient";
    } else if(term == "counts-Temp") {
	graphTitle = "Counts-Temperature Correlation";
	xAxisTitle = "Temperature ({}^{o}C)";
	yAxisTitle = "Counts";
    }

    if(ER != "")
	graphTitle = graphTitle + " (" + takePeakTypeStr(ER, "elementFirst") + ")";

    if(xAxisTitle == "Date") {
	TDatime beginDT(2021, 4, 18, 0, 0, 0);
	TDatime finalDT(2021, 6, 7, 0, 0, 0);

	inputG->GetXaxis()->SetRangeUser(beginDT.Convert(), finalDT.Convert());
	inputG->GetXaxis()->SetTimeDisplay(kTRUE);
	inputG->GetXaxis()->SetNdivisions(10, 5, 0, kFALSE);
	inputG->GetXaxis()->SetTimeFormat("%m\/%d");
    }

    inputG->SetTitle(graphTitle.c_str());
    inputG->GetXaxis()->SetTitle(xAxisTitle.c_str());
    inputG->GetYaxis()->SetTitle(yAxisTitle.c_str());
    inputG->SetLineWidth(1);
    inputG->SetLineColor(kBlack);
    inputG->SetMarkerSize(1);
    inputG->SetMarkerStyle(kCircle);
    inputG->SetMarkerColor(kBlack);
}



void GraphPrinter::setMultiGAtt(TMultiGraph* inputMG, string term, string ER = "", string timeUnit = "") {
    string graphTitle = "";
    string xAxisTitle = "";
    string yAxisTitle = "";
    if(term == "cGauss") {
	graphTitle = "Coefficient of Gaussian Term";
	xAxisTitle = "Date";
	yAxisTitle = "Coefficient";
    } else if(term == "counts") {
	graphTitle = "Total Counts in an Energy Range";
	xAxisTitle = "Date";
	yAxisTitle = "Counts";
    } else if(term == "normCGauss") {
	graphTitle = "Normalized Coefficient of Gaussian Term";
	xAxisTitle = "Date";

	if(timeUnit == "P2H")
	    yAxisTitle = "C_{2hr}/#bar{C}_{2hr}";
    } else if(term == "normCounts") {
	graphTitle = "Normalized Counting Events";
	xAxisTitle = "Date";

	if(timeUnit == "P2H")
	    yAxisTitle = "N_{2hr}/#bar{N}_{2hr}";
    } else if(term == "correctedCGauss") {
	graphTitle = "Gaussian Coefficient with Correction by Temperature";
	xAxisTitle = "Date";
	yAxisTitle = "Coefficient";
    } else if(term == "correctedCounts") {
	graphTitle = "Total Counts with Correction by Temperature in an Energy Range";
	xAxisTitle = "Date";
	yAxisTitle = "Counts";
    } else if(term == "cGaussCompare") {
	graphTitle = "Gaussian Coefficient Comparison between Before and After Temperature Correction";
	xAxisTitle = "Date";
	yAxisTitle = "Coefficient";
    } else if(term == "countsCompare") {
	graphTitle = "Counts Comparison between Before and After Temperature Correction";
	xAxisTitle = "Date";
	yAxisTitle = "Counts";
    }

    if(ER != "")
	graphTitle = graphTitle + " (" + takePeakTypeStr(ER, "elementFirst") + ")";

    if(xAxisTitle == "Date") {
	TDatime beginDT(2021, 4, 18, 0, 0, 0); // original 4/18
	TDatime finalDT(2021, 6, 7, 0, 0, 0); // original 6/7

	inputMG->GetXaxis()->SetRangeUser(beginDT.Convert(), finalDT.Convert());
	inputMG->GetXaxis()->SetTimeDisplay(kTRUE);
	inputMG->GetXaxis()->SetNdivisions(10, 5, 0, kFALSE);
	inputMG->GetXaxis()->SetTimeFormat("%m\/%d");
    }

    inputMG->GetHistogram()->SetTitle(graphTitle.c_str());
    inputMG->GetXaxis()->SetTitle(xAxisTitle.c_str());
    inputMG->GetYaxis()->SetTitle(yAxisTitle.c_str());
}



void GraphPrinter::setRangeUser(TAxis* inputAxis, string term, string ER, string timeUnit) {
    double upper = 1.;
    double lower = 0.;

    if(term == "cGauss") {
	if(ER == "peak04") {
	    if(timeUnit == "P2H") {
		upper = 50.;
		lower = -10.;
	    } else if(timeUnit == "P6H") {
		upper = 85.;
		lower = 55.;
	    }
	} else if(ER == "peak06") {
	    if(timeUnit == "P2H") {
		upper = 100.;
		lower = 0.;
	    } else if(timeUnit == "P6H") {
		upper = 240.;
		lower = 180.;
	    }
	} else if(ER == "peak16") {
	    if(timeUnit == "P2H") {
		upper = 110.;
		lower = 60.;
	    } else if(timeUnit == "P6H") {
		upper = 310.;
		lower = 260.;
	    }
	} else if(ER == "peak0406") {
	    if(timeUnit == "P2H") {
		upper = 120.;
		lower = 30.;
	    } else if(timeUnit == "P6H") {
		upper = 310.;
		lower = 250.;
	    }
	}
    } else if(term == "counts") {
	if(ER == "peak04") {
	    if(timeUnit == "P2H") {
		if(quantity == "Energy") {
		    upper = 25000.;
		    lower = 15000.;
		} else if(quantity == "Shifting") {
		    upper = 24000.;
		    lower = 14000.;
		}
	    }
	} else if(ER == "peak06") {
	    if(timeUnit == "P2H") {
		if(quantity == "Energy") {
		    upper = 40000.;
		    lower = 30000.;
		} else if(quantity == "Shifting") {
		    upper = 54000.;
		    lower = 44000.;
		}
	    }
	} else if(ER == "peak16") {
	    if(timeUnit == "P2H") {
		if(quantity == "Energy") {
		    upper = 20000.;
		    lower = 12000.;
		} else if(quantity == "Shifting") {
		    upper = 24000.;
		    lower = 20000.;
		}
	    }
	} else if(ER == "0to25") {
	    if(timeUnit == "P2H") {
		if(quantity == "Energy") {
		    upper = 480000.;
		    lower = 400000.;
		} else if(quantity == "Shifting") {
		    upper = 480000.;
		    lower = 400000.;
		}
	    }
	} else if(ER == "peak0406") {
	    if(timeUnit == "P2H") {
		if(quantity == "Energy") {
		    upper = 65000.;
		    lower = 55000.;
		} else if(quantity == "Shifting") {
		    upper = 80000.;
		    lower = 60000.;
		}
	    }
	}
    } else if(term == "normCGauss") {
	double dup = 1.25;
	double ddw = 0.75;
	if(ER == "peak04") {
	    if(timeUnit == "P2H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P6H") {
		upper = dup;
		lower = ddw;
	    }
	} else if(ER == "peak06") {
	    if(timeUnit == "P2H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P6H") {
		upper = dup;
		lower = ddw;
	    }
	} else if(ER == "peak16") {
	    if(timeUnit == "P2H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P6H") {
		upper = dup;
		lower = ddw;
	    }
	} else if(ER == "peak0406") {
	    if(timeUnit == "P2H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P6H") {
		upper = dup;
		lower = ddw;
	    }
	}
    } else if(term == "normCounts") {
	double dup = 1.05;
	double ddw = 0.95;
	if(ER == "peak04") {
	    if(timeUnit == "P2H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P6H") {
		upper = dup;
		lower = ddw;
	    }
	} else if(ER == "peak06") {
	    if(timeUnit == "P2H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P6H") {
		upper = dup;
		lower = ddw;
	    }
	} else if(ER == "peak16") {
	    if(timeUnit == "P2H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P6H") {
		upper = dup;
		lower = ddw;
	    }
	} else if(ER == "peak0406") {
	    if(timeUnit == "P2H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P6H") {
		upper = dup;
		lower = ddw;
	    }
	}
    } else if(term == "correctedCGauss") {
	if(ER == "peak04") {
	    if(timeUnit == "P2H") {
		upper = 50.;
		lower = -10.;
	    } else if(timeUnit == "P6H") {
		upper = 85.;
		lower = 55.;
	    }
	} else if(ER == "peak06") {
	    if(timeUnit == "P2H") {
		upper = 100.;
		lower = 0.;
	    } else if(timeUnit == "P6H") {
		upper = 240.;
		lower = 180.;
	    }
	} else if(ER == "peak16") {
	    if(timeUnit == "P2H") {
		upper = 110.;
		lower = 60.;
	    } else if(timeUnit == "P6H") {
		upper = 310.;
		lower = 260.;
	    }
	} else if(ER == "peak0406") {
	    if(timeUnit == "P2H") {
		upper = 120.;
		lower = 30.;
	    } else if(timeUnit == "P6H") {
		upper = 310.;
		lower = 250.;
	    }
	}
    } else if(term == "correctedCounts") {
	if(ER == "peak04") {
	    if(timeUnit == "P2H") {
		if(quantity == "Energy") {
		    upper = 40000.;
		    lower = 15000.;
		} else if(quantity == "Shifting") {
		    upper = 5000.;
		    lower = 0.;
		}
	    }
	} else if(ER == "peak06") {
	    if(timeUnit == "P2H") {
		if(quantity == "Energy") {
		    upper = 100000.;
		    lower = 70000.;
		} else if(quantity == "Shifting") {
		    upper = 52000.;
		    lower = 36000.;
		}
	    }
	} else if(ER == "peak16") {
	    if(timeUnit == "P2H") {
		if(quantity == "Energy") {
		    upper = 27000.;
		    lower = 0.;
		} else if(quantity == "Shifting") {
		    upper = 21000.;
		    lower = 14000.;
		}
	    }
	} else if(ER == "peak0406") {
	    if(timeUnit == "P2H") {
		if(quantity == "Energy") {
		    upper = 135000.;
		    lower = 100000.;
		} else if(quantity == "Shifting") {
		    upper = 85000.;
		    lower = 54000.;
		}
	    }
	}
    }

    inputAxis ->SetRangeUser(lower, upper);
}



string GraphPrinter::takePeakTypeStr(string ER, string style) {
    if(ER == "peak04") {
	if(style == "elementFirst")
	    return "Pb-214, 0.354 MeV";
	else if(style == "energyFirst")
	    return "0.354 MeV (Pb-214)";
    } else if(ER == "peak06") {
	if(style == "elementFirst")
	    return "Bi-214, 0.609 MeV";
	else if(style == "energyFirst")
	    return "0.609 MeV (Bi-214)";
    } else if(ER == "peak16") {
	if(style == "elementFirst")
	    return "K-40, 1.460 MeV";
	else if(style == "energyFirst")
	    return "1.460 MeV (K-40)";
    } else if(ER == "0to25") {
	return "0 to 2.5 MeV";
    } else if(ER == "peak0406") {
	if(style == "elementFirst")
	    return "Pb-214 & Bi-214, 0.354 & 0.609 MeV";
	else if(style == "energyFirst")
	    return "0.354 & 0.609 MeV (Pb-214 & Bi-214)";
    } else {
	cout << "Unknown peak type!!! Return original input type name." << endl;
	return ER;
    }
}
