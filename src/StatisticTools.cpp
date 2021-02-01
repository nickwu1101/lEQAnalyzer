#include "StatisticTools.h"

#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TFitResult.h>
#include <TGraph.h>
#include <TH1.h>

#include "Calendar.h"
#include "GetExterSet.h"

StatisticTools::StatisticTools() {
    anaFilename = "analyzedFile/HistoCh0.root";
}



StatisticTools::~StatisticTools() {}



void StatisticTools::test() {
    doEntriesGraphByTime();
    doPeakFitting();
}



void StatisticTools::execute() {}



void StatisticTools::prepareHistoMap() {
    openFile();
    TList* list1 = f->GetListOfKeys();

    TIter next1(list1);
    TObject* obj1 = nullptr;
    while((obj1 = next1())) {
	cout << obj1->GetName() << endl;
	TDirectory* dir = f->GetDirectory(obj1->GetName());
	TList* list2 = dir->GetListOfKeys();

	TIter next2(list2);
	TObject* obj2 = nullptr;
	while((obj2 = next2())) {
	    cout << "\t" << obj2->GetName() << endl;
	    string totalPath = (string)obj1->GetName() + "/" + (string)obj2->GetName();
	    histoMap[(string)obj1->GetName() + (string)obj2->GetName()] = (TH1D*)f->Get(totalPath.c_str());
	}
    }
}



void StatisticTools::doEntriesGraphByTime() {
    prepareHistoMap();    

    TCanvas* c = new TCanvas("c", "c", 1400, 800);
    TGraph* gEntry = new TGraph();

    Calendar* cStart = nullptr;
    double pastInterval = 0.;
    for(map<string, TH1D*>::iterator it = histoMap.begin(); it != histoMap.end(); ++it) {
	if(it == histoMap.begin())
	    cStart = new Calendar(it->first);

	Calendar* cPoint = new Calendar(it->first);
	Duration dr = *cPoint - *cStart;

	cout << it->first << "|" << dr.sec/60. << "|" << it->second->GetEntries() << endl;
	pastInterval = dr.sec/60.;

	gEntry->SetPoint(gEntry->GetN(), dr.sec/60., it->second->GetEntries());
    }
    pastInterval /= histoMap.size();

    char title[50];
    sprintf(title, "Gotten Entries in Each %.0f Minutes Interval", pastInterval);
    gEntry->SetTitle(title);
    gEntry->GetXaxis()->SetTitle("Start Time of Interval (min)");
    gEntry->GetYaxis()->SetTitle("Entries");
    gEntry->SetMarkerStyle(20);
    gEntry->SetMarkerColor(kRed);
    gEntry->SetMarkerSize(2);
    gEntry->Draw("AP");

    c->Update();
    c->Print("plotting/fitting/TimeEntriesCh0.png");

    c->Close();
    closeFile();
}



void StatisticTools::doPeakFitting() {
    GetExterSet ges { "recordDoc/Rn222Setting.txt" };
    double lowerLimit = ges.giveDoubleVar("lowerLimit");
    double lowerMean = ges.giveDoubleVar("lowerMean");
    double Mean = ges.giveDoubleVar("Mean");
    double higherMean = ges.giveDoubleVar("higherMean");
    double higherLimit = ges.giveDoubleVar("higherLimit");
    string quantity = ges.giveStrVar("quantity");

    prepareHistoMap();

    TCanvas* c = new TCanvas("c", "c", 1400, 800);
    TGraph* g[5];
    for(unsigned int i = 0; i < sizeof(g)/sizeof(g[0]); i++)
	g[i] = new TGraph();

    Calendar* cStart = nullptr;
    for(map<string, TH1D*>::iterator it = histoMap.begin(); it != histoMap.end(); ++it) {
	if(it == histoMap.begin())
	    cStart = new Calendar(it->first);

	Calendar* cPoint = new Calendar(it->first);
	Duration dr = *cPoint - *cStart;

	TF1* signalFit = new TF1("signalFit", "expo(0) + gaus(2)", lowerLimit, higherLimit);
	signalFit->SetParameter(0, 0.);
	signalFit->SetParameter(1, -3.);
	signalFit->SetParameter(2, 100.);
	signalFit->SetParameter(3, Mean);
	signalFit->SetParameter(4, 0.01);
	signalFit->SetParLimits(2, 0., 10000.);
	signalFit->SetParLimits(3, lowerMean, higherMean);
	signalFit->SetParLimits(4, 0., 0.1);

	TFitResultPtr fitptr = it->second->Fit(signalFit, "S", "", lowerLimit, higherLimit);

	it->second->SetTitle(it->first.c_str());
	if(quantity == "Voltage")
	    it->second->SetXTitle("Voltage (V)");
	else if(quantity == "Energy")
	    it->second->SetXTitle("Energy (MeV)");

	it->second->SetYTitle("Entries");
	it->second->SetStats(kFALSE);

	c->Update();
	string fittingName = "plotting/fitting/" + it->first + ".png";
	c->Print(fittingName.c_str());

	for(unsigned int i = 0; i < sizeof(g)/sizeof(g[0]); i++)
	    g[i]->SetPoint(g[i]->GetN(), dr.sec/60., fitptr->Parameter(i));
    }

    for(unsigned int i = 0; i < sizeof(g)/sizeof(g[0]); i++) {
	char title[50];
	sprintf(title, "Parameter %d in Each Interval", i);
	g[i]->SetTitle(title);
	g[i]->GetXaxis()->SetTitle("Start Time of Interval (min)");

	sprintf(title, "Parameter %d", i);
	if(i == 3) {
	    if(quantity == "Voltage")
		strcat(title, " (V)");
	    else if(quantity == "Energy")
		strcat(title, " (MeV)");

	    g[i]->GetYaxis()->SetTitle(title);
	} else
	    g[i]->GetYaxis()->SetTitle(title);

	g[i]->SetMarkerStyle(20);
	g[i]->SetMarkerColor(kRed);
	g[i]->SetMarkerSize(2);
	g[i]->Draw("AP");

	c->Update();
	char graphFilename[100];
	sprintf(graphFilename, "plotting/fitting/TimePar%d.png", i);
	c->Print(graphFilename);
    }

    c->Close();
    closeFile();
}



void StatisticTools::openFile() {
    if(f == nullptr)
	f = new TFile(anaFilename.c_str(), "READ");
    else if(!f->IsOpen()) {
	delete f;
	f = new TFile(anaFilename.c_str(), "READ");
    }
}



void StatisticTools::closeFile() {
    f->Close();
}
