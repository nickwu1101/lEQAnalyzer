#include "StatisticTools.h"

#include <TCanvas.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1.h>

#include "Calendar.h"

StatisticTools::StatisticTools() {}



StatisticTools::~StatisticTools() {}



void StatisticTools::test() {
    doEntriesGraphByTime();
}



void StatisticTools::execute() {}



void StatisticTools::prepareHistoMap() {
    if(!isHistoMapDone) {
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

    isHistoMapDone = true;
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
    c->Print("plotting/TimeEntries.png");

    closeFile();
}



void StatisticTools::doPeakFitting() {
    prepareHistoMap();

    TCanvas* c = new TCanvas("c", "c", 1400, 800);

    Calendar* cStart = nullptr;
    for(map<string, TH1D*>::iterator it = histoMap.begin(); it != histoMap.end(); ++it) {
	if(it == histoMap.begin())
	    cStart = new Calendar(it->first);

	Calendar* cPoint = new Calendar(it->first);
	Duration dr = *cPoint - *cStart;
    }
}



void StatisticTools::openFile() {
    if(!f.IsOpen())
	f = new TFile("analyzedFile/NewRootFile.root", "READ");
}



void StatisticTools::closeFile() {
    f->Close();
}
