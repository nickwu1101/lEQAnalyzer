#include "GraphPrinter.h"

#include <TAxis.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TFitResult.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TLegend.h>
#include <TLine.h>
#include <TMultiGraph.h>
#include <TPaveText.h>
#include <TTree.h>

#include "Calendar.h"
#include "PeakFitter.h"
#include "TempHumi.h"

GraphPrinter::GraphPrinter() {
    handledTU = "P2H";
    timeRange = "0418to0607";
    quantity = "Energy";
    errBandStr = "STD";
}



GraphPrinter::~GraphPrinter() {}



void GraphPrinter::execute() {
}



void GraphPrinter::test() {

    //printFittingGraph();
    printNormFittingGraph();
    //printTempCorrectFittingGraph();
    //printCompareCorrectionFittingGraph();
    //printFittingTempCorrelation();

    //printFittingMeanGraph();

    //printCountingGraph();
    //printNormCountingGraph();
    //printTempCorrectCountingGraph();
    //printCompareCorrectionCountingGraph();
    //printCountingTempCorrelation();

    printTemplateFittingGraph();

    //printEstEventGraph();
    //print50DaysFitting();

    //printNormFitHist();
    //printExpoGraph();
    //printExpoTempCorrelation();

    //printOverlapExpAndSimulation();
    //printOverlapExpAndBkg();
}



void GraphPrinter::printFittingGraph() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);

    TFile* f = nullptr;
    if(handledTU == "P2H") {
	if(timeRange == "0418to0607") {
	    if(quantity == "Energy") {
		f = new TFile("ready/fitResult.root", "READ");
	    } else if(quantity == "Shifting") {
		f = new TFile("ready/fitResultS.root", "READ");
	    }
	} else if(timeRange == "0418to0716") {
	    f = new TFile("ready/fitResultL.root", "READ");
	}
    } else if(handledTU == "P4H") {
	f = new TFile("ready/fitResult4.root", "READ");
    } else if(handledTU == "P6H") {
	f = new TFile("ready/fitResult6.root", "READ");
    } else if(handledTU == "P12H") {
	f = new TFile("ready/fitResult12.root", "READ");
    } else if(handledTU == "PD") {
	f = new TFile("ready/fitResultD.root", "READ");
    }

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
	if(quantity == "Energy") {
	    foutlier = new TFile("ready/NormalizedFitResult.root", "READ");
	} else if(quantity == "Shifting") {
	    foutlier = new TFile("ready/NormalizedFitResultS.root", "READ");
	}
    } else if(handledTU == "P4H") {
	foutlier = new TFile("ready/NormalizedFitResult4.root", "READ");
    } else if(handledTU == "P6H") {
	foutlier = new TFile("ready/NormalizedFitResult6.root", "READ");
    } else if(handledTU == "P12H") {
	foutlier = new TFile("ready/NormalizedFitResult12.root", "READ");
    } else if(handledTU == "PD") {
	foutlier = new TFile("ready/NormalizedFitResultD.root", "READ");
    }

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
    double Esum04 = 0.;
    double Esum06 = 0.;
    double Esum16 = 0.;
    double Esum0406 = 0.;
    int ntimebin04 = 0;
    int ntimebin06 = 0;
    int ntimebin16 = 0;
    int ntimebin0406 = 0;

    bool condition06 = true;
    bool condition0406 = true;

    Long64_t nentries = fitTree->GetEntries();
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	fitTree->GetEntry(entry);
	outlierTree->GetEntry(entry);
	if(entryNo == entryNoOutlier) {
	    TDatime* tdt = new TDatime(year, month, day, hour, min, (int)sec);

	    if(handledTU == "P2H") {
		condition06 = cg06 < 150.;
		condition0406 = cg0406 < 150.;
	    } else if(handledTU == "P4H") {
		condition06 = true;
		condition0406 = true;
	    } else {
		condition06 = true;
		condition0406 = true;
	    }

	    if(!isoutlier16 && !isoutlier04) {
		ge04->SetPoint(ge04->GetN(), tdt->Convert(), cg04);
		ge04->SetPointError(ge04->GetN() - 1, 0., cge04);
		mean04 += cg04;
		std04 += cg04*cg04;
		Esum04 += cge04*cge04;
		ntimebin04++;
	    }

	    if(!isoutlier16 && !isoutlier06 && condition06) {
		ge06->SetPoint(ge06->GetN(), tdt->Convert(), cg06);
		ge06->SetPointError(ge06->GetN() - 1, 0., cge06);
		mean06 += cg06;
		std06 += cg06*cg06;
		Esum06 += cge06*cge06;
		ntimebin06++;
	    }

	    if(!isoutlier16) {
		ge16->SetPoint(ge16->GetN(), tdt->Convert(), cg16);
		ge16->SetPointError(ge16->GetN() - 1, 0., cge16);
		mean16 += cg16;
		std16 += cg16*cg16;
		Esum16 += cge16*cge16;
		ntimebin16++;
	    }

	    if(!isoutlier16 && !isoutlier0406 && condition0406) {
		ge0406->SetPoint(ge0406->GetN(), tdt->Convert(), cg0406);
		ge0406->SetPointError(ge0406->GetN() - 1, 0., cge0406);
		mean0406 += cg0406;
		std0406 += cg0406*cg0406;
		Esum0406 += cge0406*cge0406;
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

    Esum04 = TMath::Sqrt(Esum04);
    Esum06 = TMath::Sqrt(Esum06);
    Esum16 = TMath::Sqrt(Esum16);
    Esum0406 = TMath::Sqrt(Esum0406);

    double err04 = 0.;
    double err06 = 0.;
    double err16 = 0.;
    double err0406 = 0.;

    if(errBandStr == "STD") {
	err04 = std04/TMath::Sqrt(ntimebin04);
	err06 = std06/TMath::Sqrt(ntimebin06);
	err16 = std16/TMath::Sqrt(ntimebin16);
	err0406 = std0406/TMath::Sqrt(ntimebin0406);
    } else if(errBandStr == "Esum") {
	err04 = Esum04/ntimebin04;
	err06 = Esum06/ntimebin06;
	err16 = Esum16/ntimebin16;
	err0406 = Esum0406/ntimebin0406;
    }

    printWithErrorBand(ge04, mean04, std04, err04, "cGauss", "peak04", handledTU);
    printWithErrorBand(ge06, mean06, std06, err06, "cGauss", "peak06", handledTU);
    printWithErrorBand(ge16, mean16, std16, err16, "cGauss", "peak16", handledTU);
    printWithErrorBand(ge0406, mean0406, std0406, err0406, "cGauss", "peak0406", handledTU);

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



void GraphPrinter::printEstEventGraph() {
    bool isTempCorrection = false;
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);

    TFile* f = nullptr;
    if(isTempCorrection == false) {
	if(handledTU == "P2H") {
	    if(quantity == "Energy")
		f = new TFile("ready/fitResult.root", "READ");
	    else if(quantity == "Shifting")
		f = new TFile("ready/fitResultS.root", "READ");
	} else if(handledTU == "P4H") {
	    f = new TFile("ready/fitResult4.root", "READ");
	} else if(handledTU == "P6H") {
	    f = new TFile("ready/fitResult6.root", "READ");
	} else if(handledTU == "P12H") {
	    f = new TFile("ready/fitResult12.root", "READ");
	} else if(handledTU == "PD") {
	    f = new TFile("ready/fitResultD.root", "READ");
	}
    } else if(isTempCorrection == true) {
	if(handledTU == "P2H") {
	    if(quantity == "Energy")
		f = new TFile("ready/TempCorrectFitResult.root", "READ");
	    else if(quantity == "Shifting")
		f = new TFile("ready/TempCorrectFitResultS.root", "READ");
	} else if(handledTU == "P4H") {
	    f = new TFile("ready/TempCorrectFitResult4.root", "READ");
	} else if(handledTU == "P6H") {
	    f = new TFile("ready/TempCorrectFitResult.root", "READ");
	}
    }

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
    } else if(handledTU == "P4H") {
	foutlier = new TFile("ready/NormalizedFitResult4.root", "READ");
    } else if(handledTU == "P6H") {
	foutlier = new TFile("ready/NormalizedFitResult6.root", "READ");
    } else if(handledTU == "P12H") {
	foutlier = new TFile("ready/NormalizedFitResult12.root", "READ");
    } else if(handledTU == "PD") {
	foutlier = new TFile("ready/NormalizedFitResultD.root", "READ");
    }

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
    double Esum04 = 0.;
    double Esum06 = 0.;
    double Esum16 = 0.;
    double Esum0406 = 0.;
    int ntimebin04 = 0;
    int ntimebin06 = 0;
    int ntimebin16 = 0;
    int ntimebin0406 = 0;

    bool condition06 = true;
    bool condition0406 = true;

    Long64_t nentries = fitTree->GetEntries();
    double estFactor = 5./867.;
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	fitTree->GetEntry(entry);
	outlierTree->GetEntry(entry);
	if(entryNo == entryNoOutlier) {
	    TDatime* tdt = new TDatime(year, month, day, hour, min, (int)sec);

	    if(handledTU == "P2H") {
		condition06 = cg06 < 150.;
		condition0406 = cg0406 < 150.;
	    } else if(handledTU == "P4H" ||
		      handledTU == "P6H") {
		condition06 = true;
		condition0406 = true;
	    }

	    if(!isoutlier16 && !isoutlier04) {
		ge04->SetPoint(ge04->GetN(), tdt->Convert(), cg04/estFactor);
		ge04->SetPointError(ge04->GetN() - 1, 0., cge04/estFactor);
		mean04 += cg04/estFactor;
		std04 += (cg04/estFactor)*(cg04/estFactor);
		Esum04 += (cge04/estFactor)*(cge04/estFactor);
		ntimebin04++;
	    }

	    if(!isoutlier16 && !isoutlier06 && condition06) {
		ge06->SetPoint(ge06->GetN(), tdt->Convert(), cg06/estFactor);
		ge06->SetPointError(ge06->GetN() - 1, 0., cge06/estFactor);
		mean06 += cg06/estFactor;
		std06 += (cg06/estFactor)*(cg06/estFactor);
		Esum06 += (cge06/estFactor)*(cge06/estFactor);
		ntimebin06++;
	    }

	    if(!isoutlier16) {
		ge16->SetPoint(ge16->GetN(), tdt->Convert(), cg16/estFactor);
		ge16->SetPointError(ge16->GetN() - 1, 0., cge16/estFactor);
		mean16 += cg16/estFactor;
		std16 += (cg16/estFactor)*(cg16/estFactor);
		Esum16 += (cge16/estFactor)*(cge16/estFactor);
		ntimebin16++;
	    }

	    if(!isoutlier16 && !isoutlier0406 && condition0406) {
		ge0406->SetPoint(ge0406->GetN(), tdt->Convert(), cg0406/estFactor);
		ge0406->SetPointError(ge0406->GetN() - 1, 0., cge0406/estFactor);
		mean0406 += cg0406/estFactor;
		std0406 += (cg0406/estFactor)*(cg0406/estFactor);
		Esum0406 += (cge0406/estFactor)*(cge0406/estFactor);
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

    Esum04 = TMath::Sqrt(Esum04);
    Esum06 = TMath::Sqrt(Esum06);
    Esum16 = TMath::Sqrt(Esum16);
    Esum0406 = TMath::Sqrt(Esum0406);

    double err04 = 0.;
    double err06 = 0.;
    double err16 = 0.;
    double err0406 = 0.;

    if(errBandStr == "STD") {
	err04 = std04/TMath::Sqrt(ntimebin04);
	err06 = std06/TMath::Sqrt(ntimebin06);
	err16 = std16/TMath::Sqrt(ntimebin16);
	err0406 = std0406/TMath::Sqrt(ntimebin0406);
    } else if(errBandStr == "Esum") {
	err04 = Esum04/ntimebin04;
	err06 = Esum06/ntimebin06;
	err16 = Esum16/ntimebin16;
	err0406 = Esum0406/ntimebin0406;
    }

    printWithErrorBand(ge04, mean04, std04, err04, "estEvents", "peak04", handledTU);
    printWithErrorBand(ge06, mean06, std06, err06, "estEvents", "peak06", handledTU);
    printWithErrorBand(ge16, mean16, std16, err16, "estEvents", "peak16", handledTU);
    printWithErrorBand(ge0406, mean0406, std0406, err0406, "estEvents", "peak0406", handledTU);

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



void GraphPrinter::printCountingGraph() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);

    TFile* f = nullptr;
    if(handledTU == "P2H") {
	if(quantity == "Energy")
	    f = new TFile("ready/fluctResult.root", "READ");
	else if(quantity == "Shifting")
	    f = new TFile("ready/fluctResultS.root", "READ");
    } else if(handledTU == "P4H") {
	f = new TFile("ready/fluctResult4.root", "READ");
    } else if(handledTU == "P6H") {
	f = new TFile("ready/fluctResult6.root", "READ");
    } else if(handledTU == "P12H") {
	f = new TFile("ready/fluctResult12.root", "READ");
    } else if(handledTU == "PD") {
	f = new TFile("ready/fluctResultD.root", "READ");
    }

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

    TFile* foutlier = nullptr;
    if(handledTU == "P2H") {
	if(quantity == "Energy")
	    foutlier = new TFile("ready/NormalizedFluctResult.root", "READ");
	else if(quantity == "Shifting")
	    foutlier = new TFile("ready/NormalizedFluctResultS.root", "READ");
    } else if(handledTU == "P4H") {
	foutlier = new TFile("ready/NormalizedFluctResult4.root", "READ");
    } else if(handledTU == "P6H") {
	foutlier = new TFile("ready/NormalizedFluctResult6.root", "READ");
    } else if(handledTU == "P12H") {
	foutlier = new TFile("ready/NormalizedFluctResult12.root", "READ");
    } else if(handledTU == "PD") {
	foutlier = new TFile("ready/NormalizedFluctResultD.root", "READ");
    }

    TTree* outlierTree;
    int entryNoOutlier;
    bool isoutlier04;
    bool isoutlier06;
    bool isoutlier16;
    bool isoutlier0to25;
    bool isoutlier0406;

    foutlier->GetObject("dataCounts", outlierTree);
    outlierTree->SetBranchAddress("entryNo", &entryNoOutlier);
    outlierTree->SetBranchAddress("isoutlier04", &isoutlier04);
    outlierTree->SetBranchAddress("isoutlier06", &isoutlier06);
    outlierTree->SetBranchAddress("isoutlier16", &isoutlier16);
    outlierTree->SetBranchAddress("isoutlier0to25", &isoutlier0to25);
    outlierTree->SetBranchAddress("isoutlier0406", &isoutlier0406);

    TGraphErrors* ge04 = new TGraphErrors();
    TGraphErrors* ge06 = new TGraphErrors();
    TGraphErrors* ge16 = new TGraphErrors();
    TGraphErrors* ge0to25 = new TGraphErrors();
    TGraphErrors* ge0406 = new TGraphErrors();

    double mean04 = 0.;
    double mean06 = 0.;
    double mean16 = 0.;
    double mean0to25 = 0.;
    double mean0406 = 0.;
    double std04 = 0.;
    double std06 = 0.;
    double std0to25 = 0.;
    double std16 = 0.;
    double std0406 = 0.;
    int ntimebin04 = 0;
    int ntimebin06 = 0;
    int ntimebin16 = 0;
    int ntimebin0to25 = 0;
    int ntimebin0406 = 0;

    bool condition0406 = true;

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

	    if(!isoutlier16 && !isoutlier0to25) {
		ge0to25->SetPoint(ge0to25->GetN(), tdt->Convert(), counts0to25);
		ge0to25->SetPointError(ge0to25->GetN() - 1, 0., countse0to25);
		mean0to25 += counts0to25;
		std0to25 += counts0to25*counts0to25;
		ntimebin0to25++;
	    }

	    if(quantity == "Energy")
		condition0406 = true;
	    else if(quantity == "Shifting")
		condition0406 = counts0406 > 70000.;

	    if(!isoutlier16 &&
	       !isoutlier0406 &&
	       condition0406) {
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
    mean0to25 /= ntimebin0to25;
    mean0406 /= ntimebin0406;

    std04 -= ntimebin04*mean04*mean04;
    std06 -= ntimebin06*mean06*mean06;
    std16 -= ntimebin16*mean16*mean16;
    std0to25 -= ntimebin0to25*mean0to25*mean0to25;
    std0406 -= ntimebin0406*mean0406*mean0406;
    std04 = TMath::Sqrt(std04/(ntimebin04 - 1));
    std06 = TMath::Sqrt(std06/(ntimebin06 - 1));
    std16 = TMath::Sqrt(std16/(ntimebin16 - 1));
    std0to25 = TMath::Sqrt(std0to25/(ntimebin0to25 - 1));
    std0406 = TMath::Sqrt(std0406/(ntimebin0406 - 1));

    double err04 = std04/TMath::Sqrt(ntimebin04);
    double err06 = std06/TMath::Sqrt(ntimebin06);
    double err16 = std16/TMath::Sqrt(ntimebin16);
    double err0to25 = std0to25/TMath::Sqrt(ntimebin0to25);
    double err0406 = std0406/TMath::Sqrt(ntimebin0406);

    printWithErrorBand(ge04, mean04, std04, err04, "counts", "peak04", handledTU);
    printWithErrorBand(ge06, mean06, std06, err06, "counts", "peak06", handledTU);
    printWithErrorBand(ge16, mean16, std16, err16, "counts", "peak16", handledTU);
    printWithErrorBand(ge0to25, mean0to25, std0to25, err0to25, "counts", "0to25", handledTU);
    printWithErrorBand(ge0406, mean0406, std0406, err0406, "counts", "peak0406", handledTU);

    delete ge0406;
    delete ge0to25;
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
    } else if(handledTU == "P4H") {
	f = new TFile("ready/fitResult4.root", "READ");
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
    if(handledTU == "P2H") {
	if(quantity == "Energy")
	    f = new TFile("ready/fluctResult.root", "READ");
	else if(quantity == "Shifting")
	    f = new TFile("ready/fluctResultS.root", "READ");
    } else if(handledTU == "P4H") {
	f = new TFile("ready/fluctResult4.root", "READ");
    } else if(handledTU == "P6H") {
	f = new TFile("ready/fluctResult6.root", "READ");
    }

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
    } else if(handledTU == "P4H") {
	f = new TFile("ready/TempCorrectFitResult4.root", "READ");
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
    } else if(handledTU == "P4H") {
	foutlier = new TFile("ready/NormalizedFitResult4.root", "READ");
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

	    if(!isoutlier16 && !isoutlier04) {
		ge04->SetPoint(ge04->GetN(), tdt->Convert(), cg04);
		ge04->SetPointError(ge04->GetN() - 1, 0., cge04);
		mean04 += cg04;
		std04 += cg04*cg04;
		ntimebin04++;
	    }

	    if(!isoutlier16 && !isoutlier06) {
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

	    if(!isoutlier16 && !isoutlier0406) {
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

    printWithErrorBand(ge04, mean04, std04, err04, "correctedCGauss", "peak04", handledTU);
    printWithErrorBand(ge06, mean06, std06, err06, "correctedCGauss", "peak06", handledTU);
    printWithErrorBand(ge16, mean16, std16, err16, "correctedCGauss", "peak16", handledTU);
    printWithErrorBand(ge0406, mean0406, std0406, err0406, "correctedCGauss", "peak0406", handledTU);

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
    if(handledTU == "P2H") {
	if(quantity == "Energy")
	    f = new TFile("ready/TempCorrectFluctResult.root", "READ");
	else if(quantity == "Shifting")
	    f = new TFile("ready/TempCorrectFluctResultS.root", "READ");
    } else if(handledTU == "P4H") {
	f = new TFile("ready/TempCorrectFluctResult4.root", "READ");
    } else if(handledTU == "P6H") {
	f = new TFile("ready/TempCorrectFluctResult6.root", "READ");
    }

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

    TFile* foutlier = nullptr;
    if(handledTU == "P2H") {
	if(quantity == "Energy")
	    foutlier = new TFile("ready/NormalizedFluctResult.root", "READ");
	else if(quantity == "Shifting")
	    foutlier = new TFile("ready/NormalizedFluctResultS.root", "READ");
    } else if(handledTU == "P4H") {
	foutlier = new TFile("ready/NormalizedFluctResult4.root", "READ");
    } else if(handledTU == "P6H") {
	foutlier = new TFile("ready/NormalizedFluctResult6.root", "READ");
    }

    TTree* outlierTree;
    int entryNoOutlier;
    bool isoutlier04;
    bool isoutlier06;
    bool isoutlier16;
    bool isoutlier0to25;
    bool isoutlier0406;

    foutlier->GetObject("dataCounts", outlierTree);
    outlierTree->SetBranchAddress("entryNo", &entryNoOutlier);
    outlierTree->SetBranchAddress("isoutlier04", &isoutlier04);
    outlierTree->SetBranchAddress("isoutlier06", &isoutlier06);
    outlierTree->SetBranchAddress("isoutlier16", &isoutlier16);
    outlierTree->SetBranchAddress("isoutlier0to25", &isoutlier0to25);
    outlierTree->SetBranchAddress("isoutlier0406", &isoutlier0406);

    TGraphErrors* ge04 = new TGraphErrors();
    TGraphErrors* ge06 = new TGraphErrors();
    TGraphErrors* ge16 = new TGraphErrors();
    TGraphErrors* ge0to25 = new TGraphErrors();
    TGraphErrors* ge0406 = new TGraphErrors();

    double mean04 = 0.;
    double mean06 = 0.;
    double mean16 = 0.;
    double mean0to25 = 0.;
    double mean0406 = 0.;
    double std04 = 0.;
    double std06 = 0.;
    double std16 = 0.;
    double std0to25 = 0.;
    double std0406 = 0.;
    int ntimebin04 = 0;
    int ntimebin06 = 0;
    int ntimebin16 = 0;
    int ntimebin0to25 = 0;
    int ntimebin0406 = 0;

    Long64_t nentries = countTree->GetEntries();
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	countTree->GetEntry(entry);
	outlierTree->GetEntry(entry);
	if(entryNo == entryNoOutlier) {
	    TDatime* tdt = new TDatime(year, month, day, hour, min, (int)sec);

	    if(!isoutlier16 && !isoutlier04) {
		ge04->SetPoint(ge04->GetN(), tdt->Convert(), counts04);
		ge04->SetPointError(ge04->GetN() - 1, 0., countse04);
		mean04 += counts04;
		std04 += counts04*counts04;
		ntimebin04++;
	    }

	    if(!isoutlier16 && !isoutlier06) {
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

	    if(!isoutlier16 && !isoutlier0to25) {
		ge0to25->SetPoint(ge0to25->GetN(), tdt->Convert(), counts0to25);
		ge0to25->SetPointError(ge0to25->GetN() - 1, 0., countse0to25);
		mean0to25 += counts0to25;
		std0to25 += counts0to25*counts0to25;
		ntimebin0to25++;
	    }

	    if(!isoutlier16 && !isoutlier0406) {
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
    mean0to25 /= ntimebin0to25;
    mean0406 /= ntimebin0406;

    std04 -= ntimebin04*mean04*mean04;
    std06 -= ntimebin06*mean06*mean06;
    std16 -= ntimebin16*mean16*mean16;
    std0to25 -= ntimebin0to25*mean0to25*mean0to25;
    std0406 -= ntimebin0406*mean0406*mean0406;
    std04 = TMath::Sqrt(std04/(ntimebin04 - 1));
    std06 = TMath::Sqrt(std06/(ntimebin06 - 1));
    std16 = TMath::Sqrt(std16/(ntimebin16 - 1));
    std0to25 = TMath::Sqrt(std0to25/(ntimebin0to25 - 1));
    std0406 = TMath::Sqrt(std0406/(ntimebin0406 - 1));

    double err04 = std04/TMath::Sqrt(ntimebin04);
    double err06 = std06/TMath::Sqrt(ntimebin06);
    double err16 = std16/TMath::Sqrt(ntimebin16);
    double err0to25 = std0to25/TMath::Sqrt(ntimebin0to25);
    double err0406 = std0406/TMath::Sqrt(ntimebin0406);

    printWithErrorBand(ge04, mean04, std04, err04, "correctedCounts", "peak04", handledTU);
    printWithErrorBand(ge06, mean06, std06, err06, "correctedCounts", "peak06", handledTU);
    printWithErrorBand(ge16, mean16, std16, err16, "correctedCounts", "peak16", handledTU);
    printWithErrorBand(ge0to25, mean0to25, std0to25, err0to25, "correctedCounts", "0to25", handledTU);
    printWithErrorBand(ge0406, mean0406, std0406, err0406, "correctedCounts", "peak0406", handledTU);

    delete ge0406;
    delete ge0to25;
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
    } else if(handledTU == "P4H") {
	f = new TFile("ready/NormalizedFitResult4.root", "READ");
    } else if(handledTU == "P6H") {
	f = new TFile("ready/NormalizedFitResult6.root", "READ");
    } else if(handledTU == "P12H") {
	f = new TFile("ready/NormalizedFitResult12.root", "READ");
    } else if(handledTU == "PD") {
	f = new TFile("ready/NormalizedFitResultD.root", "READ");
    }

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
    double Esum04 = 0.;
    double Esum06 = 0.;
    double Esum16 = 0.;
    double Esum0406 = 0.;
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
	    Esum04 += cgne04*cgne04;
	    ntimebin04++;
	}

	if(!isoutlier16) {
	    ge06->SetPoint(ge06->GetN(), tdt->Convert(), cgn06);
	    ge06->SetPointError(ge06->GetN() - 1, 0, cgne06);

	    mean06 += cgn06;
	    std06 += cgn06*cgn06;
	    Esum06 += cgne06*cgne06;
	    ntimebin06++;
	}

	if(!isoutlier16) {
	    ge16->SetPoint(ge16->GetN(), tdt->Convert(), cgn16);
	    ge16->SetPointError(ge16->GetN() - 1, 0, cgne16);

	    mean16 += cgn16;
	    std16 += cgn16*cgn16;
	    Esum16 += cgne16*cgne16;
	    ntimebin16++;
	}

	if(!isoutlier16) {
	    ge0406->SetPoint(ge0406->GetN(), tdt->Convert(), cgn0406);
	    ge0406->SetPointError(ge0406->GetN() - 1, 0, cgne0406);

	    mean0406 += cgn0406;
	    std0406 += cgn0406*cgn0406;
	    Esum0406 += cgne0406*cgne0406;
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

    double err04 = 0.;
    double err06 = 0.;
    double err16 = 0.;
    double err0406 = 0.;

    if(errBandStr == "STD") {
	err04 = std04/TMath::Sqrt(ntimebin04);
	err06 = std06/TMath::Sqrt(ntimebin06);
	err16 = std16/TMath::Sqrt(ntimebin16);
	err0406 = std0406/TMath::Sqrt(ntimebin0406);
    } else if(errBandStr == "Esum") {
	err04 = Esum04/ntimebin04;
	err06 = Esum06/ntimebin06;
	err16 = Esum16/ntimebin16;
	err0406 = Esum0406/ntimebin0406;
    }

    printWithErrorBand(ge04, mean04, std04, err04, "normCGauss", "peak04", handledTU);
    printWithErrorBand(ge06, mean06, std06, err06, "normCGauss", "peak06", handledTU);
    printWithErrorBand(ge16, mean16, std16, err16, "normCGauss", "peak16", handledTU);
    printWithErrorBand(ge0406, mean0406, std0406, err0406, "normCGauss", "peak0406", handledTU);

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
    if(handledTU == "P2H") {
	if(quantity == "Energy")
	    f = new TFile("ready/NormalizedFluctResult.root", "READ");
	else if(quantity == "Shifting")
	    f = new TFile("ready/NormalizedFluctResultS.root", "READ");
    } else if(handledTU == "P4H") {
	f = new TFile("ready/NormalizedFluctResult4.root", "READ");
    } else if(handledTU == "P6H") {
	f = new TFile("ready/NormalizedFluctResult6.root", "READ");
    } else if(handledTU == "P12H") {
	f = new TFile("ready/NormalizedFluctResult12.root", "READ");
    } else if(handledTU == "PD") {
	f = new TFile("ready/NormalizedFluctResultD.root", "READ");
    }

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
    double countsn0to25;
    double countsn0406;

    double countsne04;
    double countsne06;
    double countsne16;
    double countsne0to25;
    double countsne0406;

    bool isoutlier04;
    bool isoutlier06;
    bool isoutlier16;
    bool isoutlier0to25;
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
    countTree->SetBranchAddress("countsn0to25", &countsn0to25);
    countTree->SetBranchAddress("countsn0406", &countsn0406);

    countTree->SetBranchAddress("countsne04", &countsne04);
    countTree->SetBranchAddress("countsne06", &countsne06);
    countTree->SetBranchAddress("countsne16", &countsne16);
    countTree->SetBranchAddress("countsne0to25", &countsne0to25);
    countTree->SetBranchAddress("countsne0406", &countsne0406);

    countTree->SetBranchAddress("isoutlier04", &isoutlier04);
    countTree->SetBranchAddress("isoutlier06", &isoutlier06);
    countTree->SetBranchAddress("isoutlier16", &isoutlier16);
    countTree->SetBranchAddress("isoutlier0to25", &isoutlier0to25);
    countTree->SetBranchAddress("isoutlier0406", &isoutlier0406);

    TGraphErrors* ge04 = new TGraphErrors();
    TGraphErrors* ge06 = new TGraphErrors();
    TGraphErrors* ge16 = new TGraphErrors();
    TGraphErrors* ge0to25 = new TGraphErrors();
    TGraphErrors* ge0406 = new TGraphErrors();

    double mean04 = 0.;
    double mean06 = 0.;
    double mean16 = 0.;
    double mean0to25 = 0.;
    double mean0406 = 0.;
    double std04 = 0.;
    double std06 = 0.;
    double std16 = 0.;
    double std0to25 = 0.;
    double std0406 = 0.;
    int ntimebin04 = 0;
    int ntimebin06 = 0;
    int ntimebin16 = 0;
    int ntimebin0to25 = 0;
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

	if(!isoutlier16 && !isoutlier0to25) {
	    ge0to25->SetPoint(ge0to25->GetN(), tdt->Convert(), countsn0to25);
	    ge0to25->SetPointError(ge0to25->GetN() - 1, 0., countsne0to25);

	    mean0to25 += countsn0to25;
	    std0to25 += countsn0to25*countsn0to25;
	    ntimebin0to25++;
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
    mean0to25 /= ntimebin0to25;
    mean0406 /= ntimebin0406;

    std04 -= ntimebin04*mean04*mean04;
    std06 -= ntimebin06*mean06*mean06;
    std16 -= ntimebin16*mean16*mean16;
    std0to25 -= ntimebin0to25*mean0to25*mean0to25;
    std0406 -= ntimebin0406*mean0406*mean0406;
    std04 = TMath::Sqrt(std04/(ntimebin04 - 1));
    std06 = TMath::Sqrt(std06/(ntimebin06 - 1));
    std16 = TMath::Sqrt(std16/(ntimebin16 - 1));
    std0to25 = TMath::Sqrt(std0to25/(ntimebin0to25 - 1));
    std0406 = TMath::Sqrt(std0406/(ntimebin0406 - 1));

    double err04 = std04/TMath::Sqrt(ntimebin04);
    double err06 = std06/TMath::Sqrt(ntimebin06);
    double err16 = std16/TMath::Sqrt(ntimebin16);
    double err0to25 = std0to25/TMath::Sqrt(ntimebin0to25);
    double err0406 = std0406/TMath::Sqrt(ntimebin0406);

    printWithErrorBand(ge04, mean04, std04, err04, "normCounts", "peak04", handledTU);
    printWithErrorBand(ge06, mean06, std06, err06, "normCounts", "peak06", handledTU);
    printWithErrorBand(ge16, mean16, std16, err16, "normCounts", "peak16", handledTU);
    printWithErrorBand(ge0to25, mean0to25, std0to25, err0to25, "normCounts", "0to25", handledTU);
    printWithErrorBand(ge0406, mean0406, std0406, err0406, "normCounts", "peak0406", handledTU);

    delete ge0406;
    delete ge0to25;
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
    } else if(handledTU == "P4H") {
	fraw = new TFile("ready/fitResult4.root", "READ");
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
    } else if(handledTU == "P4H") {
	fcor = new TFile("ready/TempCorrectFitResult4.root", "READ");
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
    } else if(handledTU == "P4H") {
	foutlier = new TFile("ready/NormalizedFitResult4.root", "READ");
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
    if(handledTU == "P2H") {
	if(quantity == "Energy")
	    fraw = new TFile("ready/fluctResult.root", "READ");
	else if(quantity == "Shifting")
	    fraw = new TFile("ready/fluctResultS.root", "READ");
    } else if(handledTU == "P4H") {
	fraw = new TFile("ready/fluctResult4.root", "READ");
    } else if(handledTU == "P6H") {
	fraw = new TFile("ready/fluctResult6.root", "READ");
    }

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
    double raw0to25;
    double raw0406;

    double rawe04;
    double rawe06;
    double rawe16;
    double rawe0to25;
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
    rawTree->SetBranchAddress("counts0to25", &raw0to25);
    rawTree->SetBranchAddress("counts0406", &raw0406);

    rawTree->SetBranchAddress("countse04", &rawe04);
    rawTree->SetBranchAddress("countse06", &rawe06);
    rawTree->SetBranchAddress("countse16", &rawe16);
    rawTree->SetBranchAddress("countse0to25", &rawe0to25);
    rawTree->SetBranchAddress("countse0406", &rawe0406);

    TFile* fcor = nullptr;
    if(handledTU == "P2H") {
	if(quantity == "Energy")
	    fcor = new TFile("ready/TempCorrectFluctResult.root", "READ");
	else if(quantity == "Shifting")
	    fcor = new TFile("ready/TempCorrectFluctResultS.root", "READ");
    } else if(handledTU == "P4H") {
	fcor = new TFile("ready/TempCorrectFluctResult4.root", "READ");
    } else if(handledTU == "P6H") {
	fcor = new TFile("ready/TempCorrectFluctResult6.root", "READ");
    }

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
    double cor0to25;
    double cor0406;

    double core04;
    double core06;
    double core16;
    double core0to25;
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
    corTree->SetBranchAddress("counts0to25", &cor0to25);
    corTree->SetBranchAddress("counts0406", &cor0406);

    corTree->SetBranchAddress("countse04", &core04);
    corTree->SetBranchAddress("countse06", &core06);
    corTree->SetBranchAddress("countse16", &core16);
    corTree->SetBranchAddress("countse0to25", &core0to25);
    corTree->SetBranchAddress("countse0406", &core0406);

    TFile* foutlier = nullptr;
    if(handledTU == "P2H") {
	if(quantity == "Energy")
	    foutlier = new TFile("ready/NormalizedFluctResult.root", "READ");
	else if(quantity == "Shifting")
	    foutlier = new TFile("ready/NormalizedFluctResultS.root", "READ");
    } else if(handledTU == "P4H") {
	foutlier = new TFile("ready/NormalizedFluctResult4.root", "READ");
    } else if(handledTU == "P6H") {
	foutlier = new TFile("ready/NormalizedFluctResult6.root", "READ");
    }

    TTree* outlierTree;
    int entryNoOutlier;
    bool isoutlier16;

    foutlier->GetObject("dataCounts", outlierTree);
    outlierTree->SetBranchAddress("entryNo", &entryNoOutlier);
    outlierTree->SetBranchAddress("isoutlier16", &isoutlier16);

    TGraphErrors* geRaw04 = new TGraphErrors();
    TGraphErrors* geRaw06 = new TGraphErrors();
    TGraphErrors* geRaw16 = new TGraphErrors();
    TGraphErrors* geRaw0to25 = new TGraphErrors();
    TGraphErrors* geRaw0406 = new TGraphErrors();

    TGraphErrors* geCor04 = new TGraphErrors();
    TGraphErrors* geCor06 = new TGraphErrors();
    TGraphErrors* geCor16 = new TGraphErrors();
    TGraphErrors* geCor0to25 = new TGraphErrors();
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
	    geRaw0to25->SetPoint(geRaw0to25->GetN(), tdt->Convert(), raw0to25);
	    geRaw0406->SetPoint(geRaw0406->GetN(), tdt->Convert(), raw0406);

	    geRaw04->SetPointError(geRaw04->GetN() - 1, 0., rawe04);
	    geRaw06->SetPointError(geRaw06->GetN() - 1, 0., rawe06);
	    geRaw16->SetPointError(geRaw16->GetN() - 1, 0., rawe16);
	    geRaw0to25->SetPointError(geRaw0to25->GetN() - 1, 0., rawe0to25);
	    geRaw0406->SetPointError(geRaw0406->GetN() - 1, 0., rawe0406);

	    delete tdt;
	}
    }

    Long64_t cnentries = corTree->GetEntries();
    for(Long64_t entry = 0; entry < cnentries; ++entry) {
	corTree->GetEntry(entry);    double rawe0to25;
	outlierTree->GetEntry(entry);
	if(centryNo == entryNoOutlier) {
	    if(isoutlier16)
		continue;

	    TDatime* tdt = new TDatime(cyear, cmonth, cday, chour, cmin, (int)csec);
	    geCor04->SetPoint(geCor04->GetN(), tdt->Convert(), cor04);
	    geCor06->SetPoint(geCor06->GetN(), tdt->Convert(), cor06);
	    geCor16->SetPoint(geCor16->GetN(), tdt->Convert(), cor16);
	    geCor0to25->SetPoint(geCor0to25->GetN(), tdt->Convert(), cor0to25);
	    geCor0406->SetPoint(geCor0406->GetN(), tdt->Convert(), cor0406);

	    geCor04->SetPointError(geCor04->GetN() - 1, 0., core04);
	    geCor06->SetPointError(geCor06->GetN() - 1, 0., core06);
	    geCor16->SetPointError(geCor16->GetN() - 1, 0., core16);
	    geCor0to25->SetPointError(geCor0to25->GetN() - 1, 0., core0to25);
	    geCor0406->SetPointError(geCor0406->GetN() - 1, 0., core0406);

	    delete tdt;
	}
    }

    printCompareMultiG(geRaw04, geCor04, "counts", "peak04", handledTU);
    printCompareMultiG(geRaw06, geCor06, "counts", "peak06", handledTU);
    printCompareMultiG(geRaw16, geCor16, "counts", "peak16", handledTU);
    printCompareMultiG(geRaw0to25, geCor0to25, "counts", "0to25", handledTU);
    printCompareMultiG(geRaw0406, geCor0406, "counts", "peak0406", handledTU);

    delete geCor0406;
    delete geCor0to25;
    delete geCor16;
    delete geCor06;
    delete geCor04;

    delete geRaw0406;
    delete geRaw0to25;
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



void GraphPrinter::printTemplateFittingGraph() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);

    TFile* f = nullptr;
    if(handledTU == "P2H") {
	f = new TFile("ready/stability_modified.root", "READ");
    }

    TTree* t;
    UInt_t time;
    Float_t nRadon;

    f->GetObject("t", t);
    t->SetBranchAddress("time", &time);
    t->SetBranchAddress("nRadon", &nRadon);
    
    TGraphErrors* geNRadon = new TGraphErrors();

    double mean = 0.;
    double std = 0.;
    double ntimebin = 0.;

    Long64_t nentries = t->GetEntries();
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	t->GetEntry(entry);

	geNRadon->SetPoint(geNRadon->GetN(), time, nRadon);
	geNRadon->SetPointError(geNRadon->GetN() - 1, 0., 0.);
	mean += nRadon;
	std += nRadon*nRadon;
	ntimebin++;
    }

    mean /= ntimebin;
    std -= ntimebin*mean*mean;
    std = TMath::Sqrt(std/(ntimebin - 1));

    double error = std/TMath::Sqrt(ntimebin);

    printWithErrorBand(geNRadon, mean, std, error, "template", "full", handledTU);

    TGraphErrors* geNNRadon = new TGraphErrors();
    double nmean = 0.;
    double nstd = 0.;
    int nntimebin = 0;

    for(Long64_t entry = 0; entry < nentries; ++entry) {
	t->GetEntry(entry);
	geNNRadon->SetPoint(geNNRadon->GetN(), time, nRadon/mean);
	geNNRadon->SetPointError(geNNRadon->GetN() - 1, 0., 0.);

	nmean += nRadon/mean;
	nstd += (nRadon*nRadon)/(mean*mean);
	nntimebin++;
    }

    nmean /= nntimebin;
    nstd -= nntimebin*nmean*nmean;
    nstd = TMath::Sqrt(nstd/(nntimebin - 1));

    double nerror = nstd/TMath::Sqrt(nntimebin);

    printWithErrorBand(geNNRadon, nmean, nstd, nerror, "normTemplate", "full", handledTU);

    delete geNNRadon;
    delete geNRadon;

    if(f != nullptr) {
	f->Close();
	delete f;
    }

    delete cGraph;
}



void GraphPrinter::printFittingMeanGraph() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);

    TFile* f = nullptr;
    if(handledTU == "P2H") {
	if(quantity == "Energy")
	    f = new TFile("ready/fitResult.root", "READ");
	else if(quantity == "Shifting")
	    f = new TFile("ready/fitResultS.root", "READ");
    } else if(handledTU == "P4H") {
	f = new TFile("ready/fitResult4.root", "READ");
    } else if(handledTU == "P6H") {
	f = new TFile("ready/fitResult6.root", "READ");
    } else if(handledTU == "P12H") {
	f = new TFile("ready/fitResult12.root", "READ");
    } else if(handledTU == "PD") {
	f = new TFile("ready/fitResultD.root", "READ");
    }

    TTree* fitTree;
    int entryNo;
    int year;
    int month;
    int day;
    int hour;
    int min;
    double sec;

    double meang04;
    double meang06;
    double meang16;

    f->GetObject("dataFitting", fitTree);
    fitTree->SetBranchAddress("entryNo", &entryNo);
    fitTree->SetBranchAddress("year", &year);
    fitTree->SetBranchAddress("month", &month);
    fitTree->SetBranchAddress("day", &day);
    fitTree->SetBranchAddress("hour", &hour);
    fitTree->SetBranchAddress("minute", &min);
    fitTree->SetBranchAddress("second", &sec);

    fitTree->SetBranchAddress("meang04", &meang04);
    fitTree->SetBranchAddress("meang06", &meang06);
    fitTree->SetBranchAddress("meang16", &meang16);

    TGraphErrors* ge04 = new TGraphErrors();
    TGraphErrors* ge06 = new TGraphErrors();
    TGraphErrors* ge16 = new TGraphErrors();

    TGraphErrors* geFactor = new TGraphErrors();
    TH1D* hFactor = new TH1D("hFactor", "Factor", 35, 0.99, 1.025); 

    double m04 = 0.;
    double m06 = 0.;
    double m16 = 0.;
    double std04 = 0.;
    double std06 = 0.;
    double std16 = 0.;
    int ntimebin04 = 0;
    int ntimebin06 = 0;
    int ntimebin16 = 0;

    double mFac = 0.;
    double stdFac = 0.;
    int ntimebinFac = 0;

    Long64_t nentries = fitTree->GetEntries();
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	fitTree->GetEntry(entry);

	TDatime* tdt = new TDatime(year, month, day, hour, min, (int)sec);

	ge04->SetPoint(ge04->GetN(), tdt->Convert(), meang04);
	ge04->SetPointError(ge04->GetN() - 1, 0., 0.);
	m04 += meang04;
	std04 += meang04*meang04;
	ntimebin04++;

	ge06->SetPoint(ge06->GetN(), tdt->Convert(), meang06);
	ge06->SetPointError(ge06->GetN() - 1, 0., 0.);
	m06 += meang06;
	std06 += meang06*meang06;
	ntimebin06++;

	ge16->SetPoint(ge16->GetN(), tdt->Convert(), meang16);
	ge16->SetPointError(ge16->GetN() - 1, 0., 0.);
	m16 += meang16;
	std16 += meang16*meang16;
	ntimebin16++;

	double inputFac = 1.46/meang16;
	hFactor->Fill(inputFac);
	geFactor->SetPoint(geFactor->GetN(), tdt->Convert(), inputFac);
	geFactor->SetPointError(geFactor->GetN() - 1, 0., 0.);
	mFac += inputFac;
	stdFac += inputFac*inputFac;
	ntimebinFac++;

	delete tdt;
    }

    m04 /= ntimebin04;
    m06 /= ntimebin06;
    m16 /= ntimebin16;
    mFac /= ntimebinFac;

    std04 -= ntimebin04*m04*m04;
    std06 -= ntimebin06*m06*m06;
    std16 -= ntimebin16*m16*m16;
    stdFac -= ntimebinFac*mFac*mFac;
    std04 = TMath::Sqrt(std04/(ntimebin04 - 1));
    std06 = TMath::Sqrt(std06/(ntimebin06 - 1));
    std16 = TMath::Sqrt(std16/(ntimebin16 - 1));
    stdFac = TMath::Sqrt(stdFac/(ntimebinFac - 1));

    double err04 = std04/TMath::Sqrt(ntimebin04);
    double err06 = std06/TMath::Sqrt(ntimebin06);
    double err16 = std16/TMath::Sqrt(ntimebin16);
    double errFac = stdFac/TMath::Sqrt(ntimebinFac);

    printWithErrorBand(ge04, m04, std04, err04, "mean", "peak04", handledTU);
    printWithErrorBand(ge06, m06, std06, err06, "mean", "peak06", handledTU);
    printWithErrorBand(ge16, m16, std16, err16, "mean", "peak16", handledTU);

    printWithErrorBand(geFactor, mFac, stdFac, errFac, "factor", "peak16", handledTU);

    hFactor->SetXTitle("factor");
    hFactor->SetYTitle("Entries");
    string outputhistofilename = "";
    if(quantity == "Energy")
	outputhistofilename = "plotting/fitting2/factor_histogram.png";
    else if(quantity == "Shifting")
	outputhistofilename = "plotting/fittingShifting/factor_histogram.png";

    cGraph->cd();
    hFactor->Draw("HISTO");
    cGraph->Print(outputhistofilename.c_str());

    delete hFactor;
    delete geFactor;

    delete ge04;
    delete ge06;
    delete ge16;

    if(f != nullptr) {
	f->Close();
	delete f;
    }

    delete cGraph;
}



void GraphPrinter::print50DaysFitting() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);

    TFile* f = nullptr;
    f = new TFile("ready/0418to0607oneHist.root", "READ");

    TH1D* h = (TH1D*)f->Get("HistoCh0/20210418/000000");
    PeakFitter* pf = new PeakFitter(h, "peak04");

    pf->setHistoName("20210418000000_full50days_peak04");
    pf->setQuantity("Energy");
    pf->setNeedZoom(true);

    pf->fitPeak();

    pf->setPeakType("peak06");
    pf->setHistoName("20210418000000_full50days_peak06");

    pf->fitPeak();

    pf->setPeakType("peak16");
    pf->setHistoName("20210418000000_full50days_peak16");

    pf->fitPeak();

    delete pf;

    if(f != nullptr)
	delete f;

    delete cGraph;
}



void GraphPrinter::printNormFitHist() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);

    TFile* f = nullptr;
    if(handledTU == "P2H") {
	f = new TFile("ready/NormalizedFitResult.root", "READ");
    } else if(handledTU == "P4H") {
	f = new TFile("ready/NormalizedFitResult4.root", "READ");
    } else if(handledTU == "P6H") {
	f = new TFile("ready/NormalizedFitResult6.root", "READ");
    } else if(handledTU == "P12H") {
	f = new TFile("ready/NormalizedFitResult12.root", "READ");
    } else if(handledTU == "PD") {
	f = new TFile("ready/NormalizedFitResultD.root", "READ");
    }

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

    TH1D* hcgn0406 = new TH1D("hcgn0406", "hcgn0406", 300, 0.85, 1.15);
    TH1D* hcgne0406 = new TH1D("hcgne0406", "hcgne0406", 500, 0., 0.05);

    Long64_t nentries = fitTree->GetEntries();
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	fitTree->GetEntry(entry);

	hcgn0406->Fill(cgn0406);
	hcgne0406->Fill(cgne0406);
    }

    char filename[100];
    hcgn0406->Draw("HISTO");
    sprintf(filename, "plotting/fitting2/hist/hcgn0406_%s.png", handledTU.c_str());
    cGraph->Print(filename);
    hcgne0406->Draw("HISTO");
    sprintf(filename, "plotting/fitting2/hist/hcgne0406_%s.png", handledTU.c_str());
    cGraph->Print(filename);

    delete hcgne0406;
    delete hcgn0406;

    if(f != nullptr) {
	f->Close();
	delete f;
    }

    delete cGraph;
}



void GraphPrinter::printExpoGraph() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);

    TFile* f = nullptr;
    if(handledTU == "P2H") {
	f = new TFile("ready/fitResult.root", "READ");
    }

    TTree* fitTree;
    int entryNo;
    int year;
    int month;
    int day;
    int hour;
    int min;
    double sec;

    double cpow04;
    double cpow06;
    double cpow16;

    double expo04;
    double expo06;
    double expo16;

    f->GetObject("dataFitting", fitTree);
    fitTree->SetBranchAddress("entryNo", &entryNo);
    fitTree->SetBranchAddress("year", &year);
    fitTree->SetBranchAddress("month", &month);
    fitTree->SetBranchAddress("day", &day);
    fitTree->SetBranchAddress("hour", &hour);
    fitTree->SetBranchAddress("minute", &min);
    fitTree->SetBranchAddress("second", &sec);

    fitTree->SetBranchAddress("cpow04", &cpow04);
    fitTree->SetBranchAddress("cpow06", &cpow06);
    fitTree->SetBranchAddress("cpow16", &cpow16);

    fitTree->SetBranchAddress("expo04", &expo04);
    fitTree->SetBranchAddress("expo06", &expo06);
    fitTree->SetBranchAddress("expo16", &expo16);

    TGraphErrors* geCPow04 = new TGraphErrors();
    TGraphErrors* geCPow06 = new TGraphErrors();
    TGraphErrors* geCPow16 = new TGraphErrors();

    TGraphErrors* geExpo04 = new TGraphErrors();
    TGraphErrors* geExpo06 = new TGraphErrors();
    TGraphErrors* geExpo16 = new TGraphErrors();

    double meanCP04 = 0.;
    double meanCP06 = 0.;
    double meanCP16 = 0.;
    double meanEX04 = 0.;
    double meanEX06 = 0.;
    double meanEX16 = 0.;

    double stdCP04 = 0.;
    double stdCP06 = 0.;
    double stdCP16 = 0.;
    double stdEX04 = 0.;
    double stdEX06 = 0.;
    double stdEX16 = 0.;

    int ntimebin04 = 0;
    int ntimebin06 = 0;
    int ntimebin16 = 0;

    Long64_t nentries = fitTree->GetEntries();
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	fitTree->GetEntry(entry);
	TDatime* tdt = new TDatime(year, month, day, hour, min, (int)sec);

	geCPow04->SetPoint(geCPow04->GetN(), tdt->Convert(), cpow04);
	geCPow06->SetPoint(geCPow06->GetN(), tdt->Convert(), cpow06);
	geCPow16->SetPoint(geCPow16->GetN(), tdt->Convert(), cpow16);
	geExpo04->SetPoint(geExpo04->GetN(), tdt->Convert(), expo04);
	geExpo06->SetPoint(geExpo06->GetN(), tdt->Convert(), expo06);
	geExpo16->SetPoint(geExpo16->GetN(), tdt->Convert(), expo16);

	geCPow04->SetPointError(geCPow04->GetN() - 1, 0., 0.);
	geCPow06->SetPointError(geCPow06->GetN() - 1, 0., 0.);
	geCPow16->SetPointError(geCPow16->GetN() - 1, 0., 0.);
	geExpo04->SetPointError(geExpo04->GetN() - 1, 0., 0.);
	geExpo06->SetPointError(geExpo06->GetN() - 1, 0., 0.);
	geExpo16->SetPointError(geExpo16->GetN() - 1, 0., 0.);

	meanCP04 += cpow04;
	meanCP06 += cpow06;
	meanCP16 += cpow16;
	meanEX04 += expo04;
	meanEX06 += expo06;
	meanEX16 += expo16;

	stdCP04 += cpow04*cpow04;
	stdCP06 += cpow06*cpow06;
	stdCP16 += cpow16*cpow16;
	stdEX04 += expo04*expo04;
	stdEX06 += expo06*expo06;
	stdEX16 += expo16*expo16;

	ntimebin04++;
	ntimebin06++;
	ntimebin16++;

	delete tdt;
    }

    meanCP04 /= ntimebin04;
    meanCP06 /= ntimebin06;
    meanCP16 /= ntimebin16;
    meanEX04 /= ntimebin04;
    meanEX06 /= ntimebin06;
    meanEX16 /= ntimebin16;

    stdCP04 -= ntimebin04*meanCP04*meanCP04;
    stdCP06 -= ntimebin06*meanCP06*meanCP06;
    stdCP16 -= ntimebin16*meanCP16*meanCP16;
    stdEX04 -= ntimebin04*meanEX04*meanEX04;
    stdEX06 -= ntimebin06*meanEX06*meanEX06;
    stdEX16 -= ntimebin16*meanEX16*meanEX16;
    stdCP04 = TMath::Sqrt(stdCP04/(ntimebin04 - 1));
    stdCP06 = TMath::Sqrt(stdCP06/(ntimebin06 - 1));
    stdCP16 = TMath::Sqrt(stdCP16/(ntimebin16 - 1));
    stdEX04 = TMath::Sqrt(stdEX04/(ntimebin04 - 1));
    stdEX06 = TMath::Sqrt(stdEX06/(ntimebin06 - 1));
    stdEX16 = TMath::Sqrt(stdEX16/(ntimebin16 - 1));

    double errCP04 = stdCP04/TMath::Sqrt(ntimebin04);
    double errCP06 = stdCP06/TMath::Sqrt(ntimebin06);
    double errCP16 = stdCP16/TMath::Sqrt(ntimebin16);
    double errEX04 = stdEX04/TMath::Sqrt(ntimebin04);
    double errEX06 = stdEX06/TMath::Sqrt(ntimebin06);
    double errEX16 = stdEX16/TMath::Sqrt(ntimebin16);

    printWithErrorBand(geCPow04, meanCP04, stdCP04, errCP04, "cPow", "peak04", handledTU);
    printWithErrorBand(geCPow06, meanCP06, stdCP06, errCP06, "cPow", "peak06", handledTU);
    printWithErrorBand(geCPow16, meanCP16, stdCP16, errCP16, "cPow", "peak16", handledTU);
    printWithErrorBand(geExpo04, meanEX04, stdEX04, errEX04, "expo", "peak04", handledTU);
    printWithErrorBand(geExpo06, meanEX06, stdEX06, errEX06, "expo", "peak06", handledTU);
    printWithErrorBand(geExpo16, meanEX16, stdEX16, errEX16, "expo", "peak16", handledTU);

    delete geExpo16;
    delete geExpo06;
    delete geExpo04;

    delete geCPow16;
    delete geCPow06;
    delete geCPow04;

    if(f != nullptr) {
	f->Close();
	delete f;
    }

    delete cGraph;
}



void GraphPrinter::printExpoTempCorrelation() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);
    TempHumi* th = new TempHumi();

    TFile* f = nullptr;
    if(handledTU == "P2H") {
	f = new TFile("ready/fitResult.root", "READ");
    }

    TTree* fitTree;
    int entryNo;
    int year;
    int month;
    int day;
    int hour;
    int min;
    double sec;

    double cpow04;
    double cpow06;
    double cpow16;

    double expo04;
    double expo06;
    double expo16;

    f->GetObject("dataFitting", fitTree);
    fitTree->SetBranchAddress("entryNo", &entryNo);
    fitTree->SetBranchAddress("year", &year);
    fitTree->SetBranchAddress("month", &month);
    fitTree->SetBranchAddress("day", &day);
    fitTree->SetBranchAddress("hour", &hour);
    fitTree->SetBranchAddress("minute", &min);
    fitTree->SetBranchAddress("second", &sec);

    fitTree->SetBranchAddress("cpow04", &cpow04);
    fitTree->SetBranchAddress("cpow06", &cpow06);
    fitTree->SetBranchAddress("cpow16", &cpow16);

    fitTree->SetBranchAddress("expo04", &expo04);
    fitTree->SetBranchAddress("expo06", &expo06);
    fitTree->SetBranchAddress("expo16", &expo16);

    TGraphErrors* geCPow04 = new TGraphErrors();
    TGraphErrors* geCPow06 = new TGraphErrors();
    TGraphErrors* geCPow16 = new TGraphErrors();

    TGraphErrors* geExpo04 = new TGraphErrors();
    TGraphErrors* geExpo06 = new TGraphErrors();
    TGraphErrors* geExpo16 = new TGraphErrors();

    Long64_t nentries = fitTree->GetEntries();
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	fitTree->GetEntry(entry);

	char dtStr[15];
	sprintf(dtStr, "%04d%02d%02d%02d%02d%02.f", year, month, day, hour, min, sec);

	double temp = th->getAvgTemp(dtStr);
	double tempErr = th->getErrTemp(dtStr);

	geCPow04->SetPoint(geCPow04->GetN(), temp, cpow04);
	geCPow06->SetPoint(geCPow06->GetN(), temp, cpow06);
	geCPow16->SetPoint(geCPow16->GetN(), temp, cpow16);

	geExpo04->SetPoint(geExpo04->GetN(), temp, expo04);
	geExpo06->SetPoint(geExpo06->GetN(), temp, expo06);
	geExpo16->SetPoint(geExpo16->GetN(), temp, expo16);

	geCPow04->SetPointError(geCPow04->GetN() - 1, tempErr, 0.);
	geCPow06->SetPointError(geCPow06->GetN() - 1, tempErr, 0.);
	geCPow16->SetPointError(geCPow16->GetN() - 1, tempErr, 0.);

	geExpo04->SetPointError(geExpo04->GetN() - 1, tempErr, 0.);
	geExpo06->SetPointError(geExpo06->GetN() - 1, tempErr, 0.);
	geExpo16->SetPointError(geExpo16->GetN() - 1, tempErr, 0.);
    }


    printCorrelationWithTemp(geCPow04, "cPow", "peak04", handledTU);
    printCorrelationWithTemp(geCPow06, "cPow", "peak06", handledTU);
    printCorrelationWithTemp(geCPow16, "cPow", "peak16", handledTU);

    printCorrelationWithTemp(geExpo04, "expo", "peak04", handledTU);
    printCorrelationWithTemp(geExpo06, "expo", "peak06", handledTU);
    printCorrelationWithTemp(geExpo16, "expo", "peak16", handledTU);

    delete geExpo16;
    delete geExpo06;
    delete geExpo04;

    delete geCPow16;
    delete geCPow06;
    delete geCPow04;

    if(f != nullptr) {
	f->Close();
	delete f;
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

    TPaveText* pt = nullptr;
    if(term != "cPow" && term != "expo") {
	TFitResultPtr fitptr = inputG->Fit("pol1", "ROB=0.95 WS");

	cG->Update();
	double xleft = 0.6;
	double yupbd = 0.4;
	double ydnbd = 0.1;
	pt = new TPaveText(xleft*gPad->GetUxmax()
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
    }

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
    else if(term == "cPow")
	outputfilename = "cPowTemp";
    else if(term == "expo")
	outputfilename = "expoTemp";
    outputfilename = outputfilename + "_" + ER + "_" + timeUnit + ".png";

    string outputPath = outputfolder + "/" + outputfilename;
    cG->Print(outputPath.c_str());

    if(pt != nullptr)
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



void GraphPrinter::printWithErrorBand(TGraph* dataG, double mean, double std, double bandWidth, string term, string ER, string timeUnit) {
    TCanvas* cMG = new TCanvas("cMG", "cMG", 1400, 800);

    TGraphErrors* geMean = new TGraphErrors();
    TGraphErrors* ge1Sigma = new TGraphErrors();
    TGraphErrors* ge2Sigma = new TGraphErrors();

    TDatime* dt0418 = new TDatime(2021, 4, 18, 22, 11, 0);
    TDatime* dt0503 = new TDatime(2021, 5, 3, 14, 7, 0);
    TDatime* dt0508 = new TDatime(2021, 5, 8, 12, 35, 0);
    TDatime* dt0522 = new TDatime(2021, 5, 23, 3, 57, 0);
    TDatime* dt0525 = new TDatime(2021, 5, 26, 0, 47, 0);
    TGraph* g0418 = new TGraph();
    TGraph* g0503 = new TGraph();
    TGraph* g0508 = new TGraph();
    TGraph* g0522 = new TGraph();
    TGraph* g0525 = new TGraph();

    for(int i = 0; i < 2; i++) {
	TDatime* tdt = nullptr;
	if(i == 0)
	    tdt = new TDatime(2021, 4, 18, 0, 0, 0);
	else if(i == 1)
	    tdt = new TDatime(2021, 7, 16, 0, 0, 0);

	geMean->SetPoint(i, tdt->Convert(), mean);
	ge1Sigma->SetPoint(i, tdt->Convert(), mean);
	ge2Sigma->SetPoint(i, tdt->Convert(), mean);

	geMean->SetPointError(i, 0., 0.);
	ge1Sigma->SetPointError(i, 0., bandWidth);
	ge2Sigma->SetPointError(i, 0., 2*bandWidth);

	g0418->SetPoint(i, dt0418->Convert(), ((double)i)*(1e10));
	g0503->SetPoint(i, dt0503->Convert(), ((double)i)*(1e10));
	g0508->SetPoint(i, dt0508->Convert(), ((double)i)*(1e10));
	g0522->SetPoint(i, dt0522->Convert(), ((double)i)*(1e10));
	g0525->SetPoint(i, dt0525->Convert(), ((double)i)*(1e10));

	delete tdt;
    }

    setGraphAtt(dataG, term, ER, timeUnit);
    setErrorBandAtt(geMean, ge1Sigma, ge2Sigma, "formal");

    g0418->SetLineColor(kRed);
    g0503->SetLineColor(kRed);
    g0508->SetLineColor(kRed);
    g0522->SetLineColor(kRed);
    g0525->SetLineColor(kRed);

    cMG->cd();
    TMultiGraph* mg = new TMultiGraph();
    mg->Add(ge2Sigma, "A3");
    mg->Add(ge1Sigma, "A3");
    mg->Add(geMean, "LC");
    mg->Add(dataG, "AP");

    mg->Add(g0418, "LC");
    mg->Add(g0503, "LC");
    mg->Add(g0508, "LC");
    mg->Add(g0522, "LC");
    mg->Add(g0525, "LC");
    setMultiGAtt(mg, term, ER, timeUnit);
    setRangeUser(mg->GetYaxis(), term, ER, timeUnit);
    mg->Draw("A");

    TLegend* lg = new TLegend(0.6, 0.12, 0.9, 0.3);
    char lline[200];
    //lg->SetHeader(takePeakTypeStr(ER, "energyFirst").c_str(), "C");
    lg->AddEntry(dataG, "data");

    if(term.find("norm") != string::npos)
	sprintf(lline, "average");
    else
	sprintf(lline, "average: %.4f", mean);

    lg->AddEntry(geMean, lline);
    sprintf(lline, "STD: %.4f", std);
    lg->AddEntry((TObject*)0, lline, "");
    sprintf(lline, "1 SE: %.4f", bandWidth);
    lg->AddEntry(ge1Sigma, lline);
    sprintf(lline, "2 SE: %.4f", 2*bandWidth);
    lg->AddEntry(ge2Sigma, lline);
    sprintf(lline, "Time of EQ");
    lg->AddEntry(g0418, lline);
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

    if(term == "cPow") {
	outputfilename = "cPow";
    } else if(term == "expo") {
	outputfilename = "expo";
    } else if(term == "cGauss") {
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
    } else if(term == "template") {
	outputfilename = "template";
    } else if(term == "normTemplate") {
	outputfilename = "normTemplate";
    } else if(term == "mean") {
	outputfilename = "mean";
    } else if(term == "factor") {
	outputfilename = "factor";
    } else if(term == "estEvents") {
	outputfilename = "estEvents";
    }
    outputfilename = outputfilename + "_" + ER + "_" + timeUnit + ".png";

    string outputPath = outputfolder + "/" + outputfilename;
    cMG->Print(outputPath.c_str());

    delete lg;

    mg->GetListOfGraphs()->Clear();
    delete mg;

    delete g0525;
    delete g0522;
    delete g0508;
    delete g0503;
    delete g0418;

    delete dt0525;
    delete dt0522;
    delete dt0508;
    delete dt0503;
    delete dt0418;

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
	    yAxisTitle = "N_{2hr}/#bar{N}_{2hr}";
	else if(timeUnit == "P4H")
	    yAxisTitle = "N_{4hr}/#bar{N}_{4hr}";
	else if(timeUnit == "P6H")
	    yAxisTitle = "N_{6hr}/#bar{N}_{6hr}";
    } else if(term == "normCounts") {
	graphTitle = "Normalized Counting Events";
	xAxisTitle = "Date";

	if(timeUnit == "P2H")
	    yAxisTitle = "N_{2hr}/#bar{N}_{2hr}";
	else if(timeUnit == "P4H")
	    yAxisTitle = "N_{4hr}/#bar{N}_{4hr}";
	else if(timeUnit == "P6H")
	    yAxisTitle = "N_{6hr}/#bar{N}_{6hr}";
	else if(timeUnit == "P12H")
	    yAxisTitle = "N_{12hr}/#bar{N}_{12hr}";
	else if(timeUnit == "PD")
	    yAxisTitle = "N_{Day}/#bar{N}_{Day}";
    } else if(term == "correctedCGauss") {
	graphTitle = "Gaussian Coefficient with Correction by Temperature";
	xAxisTitle = "Date";
	yAxisTitle = "Coefficient";
    } else if(term == "correctedCounts") {
	graphTitle = "Total Counts with Correction by Temperature in an Energy Range";
	xAxisTitle = "Date";
	yAxisTitle = "Counts";
    } else if(term == "cPow-Temp") {
	graphTitle = "Coefficient Power-Temperature Correlation";
	xAxisTitle = "Temperature ({}^{o}C)";
	yAxisTitle = "Coefficient Power";
    } else if(term == "expo-Temp") {
	graphTitle = "A for Exponential AX-Temperature Correlation";
	xAxisTitle = "Temperature ({}^{o}C)";
	yAxisTitle = "A for Exponential AX";
    } else if(term == "cGauss-Temp") {
	graphTitle = "Gaussian Coefficient-Temperature Correlation";
	xAxisTitle = "Temperature ({}^{o}C)";
	yAxisTitle = "Coefficient";
    } else if(term == "counts-Temp") {
	graphTitle = "Counts-Temperature Correlation";
	xAxisTitle = "Temperature ({}^{o}C)";
	yAxisTitle = "Counts";
    } else if(term == "mean") {
	graphTitle = "Gaussian Mean";
	xAxisTitle = "Date";
	yAxisTitle = "Mean (MeV)";
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
    if(term == "cPow") {
	graphTitle = "Coefficient Power Term";
	xAxisTitle = "Date";
	yAxisTitle = "Coefficient Power";
    } else if(term == "expo") {
	graphTitle = "A for Exponential AX";
	xAxisTitle = "Date";
	yAxisTitle = "A for AX";
    } else if(term == "cGauss") {
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
	else if(timeUnit == "P4H")
	    yAxisTitle = "C_{4hr}/#bar{C}_{4hr}";
	else if(timeUnit == "P6H")
	    yAxisTitle = "C_{6hr}/#bar{C}_{6hr}";
	else if(timeUnit == "P12H")
	    yAxisTitle = "C_{12hr}/#bar{C}_{12hr}";
	else if(timeUnit == "PD")
	    yAxisTitle = "C_{Day}/#bar{C}_{Day}";
    } else if(term == "normCounts") {
	graphTitle = "Normalized Counting Events";
	xAxisTitle = "Date";

	if(timeUnit == "P2H")
	    yAxisTitle = "N_{2hr}/#bar{N}_{2hr}";
	else if(timeUnit == "P4H")
	    yAxisTitle = "N_{4hr}/#bar{N}_{4hr}";
	else if(timeUnit == "P6H")
	    yAxisTitle = "N_{6hr}/#bar{N}_{6hr}";
	else if(timeUnit == "P12H")
	    yAxisTitle = "N_{12hr}/#bar{N}_{12hr}";
	else if(timeUnit == "PD")
	    yAxisTitle = "N_{Day}/#bar{N}_{Day}";
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
    } else if(term == "template") {
	graphTitle = "Number of Signal Events";
	xAxisTitle = "Date";
	yAxisTitle = "Signal Events";
    } else if(term == "normTemplate") {
	graphTitle = "Normalized Signal Events";
	xAxisTitle = "Date";

	if(timeUnit == "P2H")
	    yAxisTitle = "N_{2hr}/#bar{N}_{2hr}";
    } else if(term == "mean") {
	graphTitle = "Gaussian Mean";
	xAxisTitle = "Date";
	yAxisTitle = "Mean (MeV)";
    } else if(term == "factor") {
	graphTitle = "Factor for Calibration";
	xAxisTitle = "Date";
	yAxisTitle = "Factor";
    } else if(term == "estEvents") {
	graphTitle = "Estimated Events by Fitting";
	xAxisTitle = "Date";
	yAxisTitle = "Estimated Events";
    }

    if(ER != "")
	graphTitle = graphTitle + " (" + takePeakTypeStr(ER, "elementFirst") + ")";

    if(xAxisTitle == "Date") {
	string zoomtype = "Full";
	if(zoomtype == "Full") {
	    TDatime beginDT(2021, 4, 18, 0, 0, 0); // original 4/18
	    TDatime finalDT(2021, 6, 7, 0, 0, 0); // original 6/7

	    inputMG->GetXaxis()->SetRangeUser(beginDT.Convert(), finalDT.Convert());
	    inputMG->GetXaxis()->SetTimeDisplay(kTRUE);
	    inputMG->GetXaxis()->SetNdivisions(10, 5, 0, kFALSE);
	    inputMG->GetXaxis()->SetTimeFormat("%m\/%d");
	} else if(zoomtype == "0418") {
	    TDatime beginDT(2021, 4, 18, 0, 0, 0); // original 4/18
	    TDatime finalDT(2021, 4, 21, 0, 0, 0); // original 6/7

	    inputMG->GetXaxis()->SetRangeUser(beginDT.Convert(), finalDT.Convert());
	    inputMG->GetXaxis()->SetTimeDisplay(kTRUE);
	    inputMG->GetXaxis()->SetNdivisions(3, 6, 3, kFALSE);
	    inputMG->GetXaxis()->SetTimeFormat("%m\/%d, %Hh");
	} else if(zoomtype == "0503") {
	    TDatime beginDT(2021, 4, 30, 0, 0, 0); // original 4/18
	    TDatime finalDT(2021, 5, 6, 0, 0, 0); // original 6/7

	    inputMG->GetXaxis()->SetRangeUser(beginDT.Convert(), finalDT.Convert());
	    inputMG->GetXaxis()->SetTimeDisplay(kTRUE);
	    inputMG->GetXaxis()->SetNdivisions(6, 6, 3, kFALSE);
	    inputMG->GetXaxis()->SetTimeFormat("%m\/%d, %Hh");
	} else if(zoomtype == "0508") {
	    TDatime beginDT(2021, 5, 5, 0, 0, 0); // original 4/18
	    TDatime finalDT(2021, 5, 11, 0, 0, 0); // original 6/7

	    inputMG->GetXaxis()->SetRangeUser(beginDT.Convert(), finalDT.Convert());
	    inputMG->GetXaxis()->SetTimeDisplay(kTRUE);
	    inputMG->GetXaxis()->SetNdivisions(6, 6, 3, kFALSE);
	    inputMG->GetXaxis()->SetTimeFormat("%m\/%d, %Hh");
	} else if(zoomtype == "0522") {
	    TDatime beginDT(2021, 5, 19, 0, 0, 0); // original 4/18
	    TDatime finalDT(2021, 5, 25, 0, 0, 0); // original 6/7

	    inputMG->GetXaxis()->SetRangeUser(beginDT.Convert(), finalDT.Convert());
	    inputMG->GetXaxis()->SetTimeDisplay(kTRUE);
	    inputMG->GetXaxis()->SetNdivisions(6, 6, 3, kFALSE);
	    inputMG->GetXaxis()->SetTimeFormat("%m\/%d, %Hh");
	} else if(zoomtype == "0525") {
	    TDatime beginDT(2021, 5, 22, 0, 0, 0); // original 4/18
	    TDatime finalDT(2021, 5, 28, 0, 0, 0); // original 6/7

	    inputMG->GetXaxis()->SetRangeUser(beginDT.Convert(), finalDT.Convert());
	    inputMG->GetXaxis()->SetTimeDisplay(kTRUE);
	    inputMG->GetXaxis()->SetNdivisions(6, 6, 3, kFALSE);
	    inputMG->GetXaxis()->SetTimeFormat("%m\/%d, %Hh");
	} else if(zoomtype == "05") {
	    TDatime beginDT(2021, 4, 29, 0, 0, 0); // original 4/18
	    TDatime finalDT(2021, 5, 17, 0, 0, 0); // original 6/7

	    inputMG->GetXaxis()->SetRangeUser(beginDT.Convert(), finalDT.Convert());
	    inputMG->GetXaxis()->SetTimeDisplay(kTRUE);
	    inputMG->GetXaxis()->SetNdivisions(18, 2, 0, kFALSE);
	    inputMG->GetXaxis()->SetTimeFormat("%m\/%d");
	}
    }

    inputMG->GetHistogram()->SetTitle(graphTitle.c_str());
    inputMG->GetXaxis()->SetTitle(xAxisTitle.c_str());
    inputMG->GetYaxis()->SetTitle(yAxisTitle.c_str());
    //inputMG->GetXaxis()->SetTitleSize(0.045);
    //inputMG->GetYaxis()->SetTitleSize(0.045);
}



void GraphPrinter::setRangeUser(TAxis* inputAxis, string term, string ER, string timeUnit) {
    double upper = 1.;
    double lower = 0.;

    if(term == "cPow") {
	if(ER == "peak04") {
	    upper = 10.;
	    lower = 9.;
	} else if(ER == "peak06") {
	    upper = 10.;
	    lower = 9.;
	} else if(ER == "peak16") {
	    upper = 9.5;
	    lower = 8.5;
	}
    } else if(term == "expo") {
	if(ER == "peak04") {
	    upper = -2.5;
	    lower = -3.8;
	} else if(ER == "peak06") {
	    upper = -3.2;
	    lower = -4.4;
	} else if(ER == "peak16") {
	    upper = -1.8;
	    lower = -2.8;
	}
    } else if(term == "cGauss") {
	if(ER == "peak04") {
	    if(timeUnit == "P2H") {
		if(quantity == "Energy") {
		    upper = 35.;
		    lower = 0.;
		} else if(quantity == "Shifting") {
		    upper = 80.;
		    lower = -10.;
		}
	    } else if(timeUnit == "P4H") {
		upper = 60.;
		lower = 10.;
	    } else if(timeUnit == "P6H") {
		upper = 85.;
		lower = 55.;
	    }
	} else if(ER == "peak06") {
	    if(timeUnit == "P2H") {
		if(quantity == "Energy") {
		    upper = 90.;
		    lower = 30.;
		} else if(quantity == "Shifting") {
		    upper = 150.;
		    lower = 0.;
		}
	    } else if(timeUnit == "P4H") {
		upper = 170.;
		lower = 50.;
	    } else if(timeUnit == "P6H") {
		upper = 240.;
		lower = 180.;
	    }
	} else if(ER == "peak16") {
	    if(timeUnit == "P2H") {
		if(quantity == "Energy") {
		    upper = 110.;
		    lower = 70.;
		} else if(quantity == "Shifting") {
		    upper = 110.;
		    lower = 60.;
		}
	    } else if(timeUnit == "P4H") {
		upper = 210.;
		lower = 150.;
	    } else if(timeUnit == "P6H") {
		upper = 310.;
		lower = 260.;
	    }
	} else if(ER == "peak0406") {
	    if(timeUnit == "P2H") {
		if(quantity == "Energy") {
		    upper = 120.;
		    lower = 50.;
		} else if(quantity == "Shifting") {
		    upper = 150.;
		    lower = 50.;
		}
	    } else if(timeUnit == "P4H") {
		upper = 250.;
		lower = 150.;
	    } else if(timeUnit == "P6H") {
		upper = 310.;
		lower = 250.;
	    }
	}
    } else if(term == "counts") {
	if(ER == "peak04") {
	    if(timeUnit == "P2H") {
		if(quantity == "Energy") {
		    upper = 46000.;
		    lower = 40000.;
		} else if(quantity == "Shifting") {
		    upper = 25000.;
		    lower = 14000.;
		}
	    } else if(timeUnit == "P4H") {
		upper = 76000.;
		lower = 68000.;
	    } else if(timeUnit == "P6H") {
		upper = 114000.;
		lower = 102000.;
	    }
	} else if(ER == "peak06") {
	    if(timeUnit == "P2H") {
		if(quantity == "Energy") {
		    upper = 46000.;
		    lower = 40000.;
		} else if(quantity == "Shifting") {
		    upper = 54000.;
		    lower = 44000.;
		}
	    } else if(timeUnit == "P4H") {
		upper = 68000.;
		lower = 60000.;
	    } else if(timeUnit == "P6H") {
		upper = 104000.;
		lower = 90000.;
	    }
	} else if(ER == "peak16") {
	    if(timeUnit == "P2H") {
		if(quantity == "Energy") {
		    upper = 24000.;
		    lower = 20000.;
		} else if(quantity == "Shifting") {
		    upper = 24000.;
		    lower = 20000.;
		}
	    } else if(timeUnit == "P4H") {
		upper = 46000.;
		lower = 40000.;
	    } else if(timeUnit == "P6H") {
		upper = 69000.;
		lower = 60000.;
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
	    } else if(timeUnit == "P4H") {
		upper = 940000.;
		lower = 840000.;
	    } else if(timeUnit == "P6H") {
		upper = 1410000.;
		lower = 1200000.;
	    }
	} else if(ER == "peak0406") {
	    if(timeUnit == "P2H") {
		if(quantity == "Energy") {
		    upper = 90000.;
		    lower = 80000.;
		} else if(quantity == "Shifting") {
		    upper = 80000.;
		    lower = 60000.;
		}
	    } else if(timeUnit == "P4H") {
		upper = 143000.;
		lower = 130000.;
	    } else if(timeUnit == "P6H") {
		upper = 214000.;
		lower = 200000.;
	    }
	}
    } else if(term == "template") {
	if(timeUnit == "P2H") {
	    upper = 32000.;
	    lower = 12000.;
	}
    } else if(term == "normCGauss") {
	double dup = 1.25;
	double ddw = 0.75;
	upper = dup;
	lower = ddw;
	if(ER == "peak04") {
	    if(timeUnit == "P2H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P4H") {
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
	    } else if(timeUnit == "P4H") {
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
	    } else if(timeUnit == "P4H") {
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
	    } else if(timeUnit == "P4H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P6H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P12H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "PD") {
		upper = dup;
		lower = ddw;
	    }
	}
    } else if(term == "normCounts") {
	double dup = 1.1;
	double ddw = 0.9;
	upper = dup;
	lower = ddw;
	if(ER == "peak04") {
	    if(timeUnit == "P2H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P4H") {
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
	    } else if(timeUnit == "P4H") {
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
	    } else if(timeUnit == "P4H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P6H") {
		upper = dup;
		lower = ddw;
	    }
	} else if(ER == "0to25") {
	    if(timeUnit == "P2H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P4H") {
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
	    } else if(timeUnit == "P4H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P6H") {
		upper = dup;
		lower = ddw;
	    }
	}
    } else if(term == "normTemplate") {
	if(timeUnit == "P2H") {
	    upper = 1.5;
	    lower = 0.5;
	}
    } else if(term == "correctedCGauss") {
	if(ER == "peak04") {
	    if(timeUnit == "P2H") {
		upper = 35.;
		lower = 0.;
	    } else if(timeUnit == "P4H") {
		upper = 60.;
		lower = 0.;
	    } else if(timeUnit == "P6H") {
		upper = 85.;
		lower = 55.;
	    }
	} else if(ER == "peak06") {
	    if(timeUnit == "P2H") {
		upper = 90.;
		lower = 30.;
	    } else if(timeUnit == "P4H") {
		upper = 170.;
		lower = 50.;
	    } else if(timeUnit == "P6H") {
		upper = 240.;
		lower = 180.;
	    }
	} else if(ER == "peak16") {
	    if(timeUnit == "P2H") {
		upper = 110.;
		lower = 70.;
	    } else if(timeUnit == "P4H") {
		upper = 210.;
		lower = 150.;
	    } else if(timeUnit == "P6H") {
		upper = 310.;
		lower = 260.;
	    }
	} else if(ER == "peak0406") {
	    if(timeUnit == "P2H") {
		upper = 120.;
		lower = 50.;
	    } else if(timeUnit == "P4H") {
		upper = 250.;
		lower = 150.;
	    } else if(timeUnit == "P6H") {
		upper = 310.;
		lower = 250.;
	    }
	}
    } else if(term == "correctedCounts") {
	if(ER == "peak04") {
	    if(timeUnit == "P2H") {
		if(quantity == "Energy") {
		    upper = 46000.;
		    lower = 40000.;
		} else if(quantity == "Shifting") {
		    upper = 25000.;
		    lower = 14000.;
		}
	    } else if(timeUnit == "P4H") {
		upper = 76000.;
		lower = 68000.;
	    } else if(timeUnit == "P6H") {
		upper = 114000.;
		lower = 102000.;
	    }
	} else if(ER == "peak06") {
	    if(timeUnit == "P2H") {
		if(quantity == "Energy") {
		    upper = 46000.;
		    lower = 40000.;
		} else if(quantity == "Shifting") {
		    upper = 54000.;
		    lower = 44000.;
		}
	    } else if(timeUnit == "P4H") {
		upper = 69000.;
		lower = 60000.;
	    } else if(timeUnit == "P6H") {
		upper = 104000.;
		lower = 90000.;
	    }
	} else if(ER == "peak16") {
	    if(timeUnit == "P2H") {
		if(quantity == "Energy") {
		    upper = 24000.;
		    lower = 20000.;
		} else if(quantity == "Shifting") {
		    upper = 24000.;
		    lower = 20000.;
		}
	    } else if(timeUnit == "P4H") {
		upper = 46000.;
		lower = 40000.;
	    } else if(timeUnit == "P6H") {
		upper = 69000.;
		lower = 60000.;
	    }
	} else if(ER == "0to25") {
	    if(timeUnit == "P2H") {
		upper = 480000.;
		lower = 400000.;
	    } else if(timeUnit == "P4H") {
		upper = 940000.;
		lower = 840000.;
	    } else if(timeUnit == "P6H") {
		upper = 1410000.;
		lower = 1200000.;
	    }
	} else if(ER == "peak0406") {
	    if(timeUnit == "P2H") {
		if(quantity == "Energy") {
		    upper = 90000.;
		    lower = 80000.;
		} else if(quantity == "Shifting") {
		    upper = 80000.;
		    lower = 60000.;
		}
	    } else if(timeUnit == "P4H") {
		upper = 143000.;
		lower = 130000.;
	    } else if(timeUnit == "P6H") {
		upper = 214000.;
		lower = 200000.;
	    }
	}
    } else if(term == "mean") {
	if(ER == "peak04") {
	    if(quantity == "Energy") {
		upper = 0.3;
		lower = 0.25;
	    } else if(quantity == "Shifting") {
		upper = 0.4;
		lower = 0.35;
	    }
	} else if(ER == "peak06") {
	    if(quantity == "Energy") {
		upper = 0.62;
		lower = 0.57;
	    } else if(quantity == "Shifting") {
		upper = 0.7;
		lower = 0.65;
	    }
	} else if(ER == "peak16") {
	    upper = 1.47;
	    lower = 1.42;
	}
    } else if(term == "factor") {
	upper = 1.03;
	lower = 0.95;
    } else if(term == "estEvents") {
	if(ER == "peak04") {
	    if(timeUnit == "P2H") {
		upper = 6090.;
		lower = 0.;
	    } else if(timeUnit == "P4H") {
		upper = 10440.;
		lower = 1740.;
	    } else if(timeUnit == "P6H") {
		upper = 14790.;
		lower = 9570.;
	    }
	} else if(ER == "peak06") {
	    if(timeUnit == "P2H") {
		upper = 15660.;
		lower = 5220.;
	    } else if(timeUnit == "P4H") {
		upper = 30276.;
		lower = 8700.;
	    } else if(timeUnit == "P6H") {
		upper = 41760.;
		lower = 31320.;
	    }
	} else if(ER == "peak16") {
	    if(timeUnit == "P2H") {
		upper = 19140.;
		lower = 12180.;
	    } else if(timeUnit == "P4H") {
		upper = 36540.;
		lower = 26100.;
	    } else if(timeUnit == "P6H") {
		upper = 53940.;
		lower = 45240.;
	    }
	} else if(ER == "peak0406") {
	    if(timeUnit == "P2H") {
		upper = 20880.;
		lower = 8700.;
	    } else if(timeUnit == "P4H") {
		upper = 43500.;
		lower = 26100.;
	    } else if(timeUnit == "P6H") {
		upper = 53940.;
		lower = 43500.;
	    }
	}
    }

    inputAxis ->SetRangeUser(lower, upper);
}



string GraphPrinter::takePeakTypeStr(string ER, string style) {
    if(ER == "peak04") {
	if(style == "elementFirst")
	    return "Pb-214, 0.295 MeV";
	else if(style == "energyFirst")
	    return "0.295 MeV (Pb-214)";
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
	    return "Pb-214 & Bi-214, 0.295 & 0.609 MeV";
	else if(style == "energyFirst")
	    return "0.295 & 0.609 MeV (Pb-214 & Bi-214)";
    } else if(ER == "full") {
	return "Full Spectrum";
    } else {
	cout << "Unknown peak type!!! Return original input type name." << endl;
	return ER;
    }
}
