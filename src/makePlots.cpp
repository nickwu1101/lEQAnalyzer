#ifndef makePlots_cpp
#define makePlots_cpp 1

#include <dirent.h>

#include <TBranch.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TLegend.h>
#include <TTree.h>
#include <TGraph.h>

#include "Calendar.h"
#include "DataReader.h"
#include "GetExterSet.h"
#include "makePlots.h"

makePlots::makePlots() {}



makePlots::~makePlots() {
    cout << "Calling Destructor" << endl;
}



void makePlots::initialize() {
    if(!isInit) {
	readAnalysisParameter();
	prepareDataList();
	if(anaType == 0) {
	    assignTimeIntervals();
	}
	else if(anaType == 1) {
	    assignSingleLengthOfIntervals();
	}

	isInit = true;
    }
}



void makePlots::execute() {
    makeHistoCh0();
    makeHistoCh1();
    doCoincidence(0, 1, 0.0055);
}



void makePlots::makeHistoCh0() {
    initialize();
    prepareOutputFile("HistoCh0");

    for(unsigned int iInt = 0; iInt < startDateTime.size(); iInt++) {
	bool willBeDealed = false;
	Calendar *dtStart = new Calendar(startDateTime[iInt]);
	Calendar *dtEnd = new Calendar(endDateTime[iInt]);

	TH1D* hCh0 = new TH1D(dtStart->getTime().c_str(), "Maximum as Amplitude of Channel 0", bin[0], min[0], max[0]);

	if(outfile->GetDirectory(dtStart->getDate().c_str()) == nullptr)
	    outfile->mkdir(dtStart->getDate().c_str(), dtStart->getDate().c_str());

	cout << dtStart->getDateTime() << " - " << dtEnd->getDateTime() << endl;

	for(unsigned int iData = 0; iData < dataList.size(); iData++) {
	    if(iData + 1 >= dataList.size())
		willBeDealed = hasDataInInterval(dataList[iData], dtStart, dtEnd, "null");
	    else
		willBeDealed = hasDataInInterval(dataList[iData], dtStart, dtEnd, dataList[iData + 1]);

	    if(willBeDealed) {
		cout << dataList[iData] << endl;
		DataReader* dr = new DataReader(dataList[iData]);
		dr->setQuantity(quantity);
		dr->setStartDateTime(dtStart->getDateTime());
		dr->setEndDateTime(dtEnd->getDateTime());
		dr->setThreshold(true, 0.3);
		dr->runFillingLoop(hCh0, 0);
	    }
	}

	if(quantity == "Voltage")
	    hCh0->SetXTitle("Voltage (V)");
	else if(quantity == "Energy")
	    hCh0->SetXTitle("Energy (MeV)");

	hCh0->SetYTitle("Entries");

	outfile->cd(dtStart->getDate().c_str());
	hCh0->Write();
    }

    outfile->Close();
}



void makePlots::makeHistoCh1() {
    initialize();
    prepareOutputFile("HistoCh1");

    for(unsigned int iInt = 0; iInt < startDateTime.size(); iInt++) {
	bool willBeDealed = false;
	Calendar *dtStart = new Calendar(startDateTime[iInt]);
	Calendar *dtEnd = new Calendar(endDateTime[iInt]);

	TH1D* hCh1 = new TH1D(dtStart->getTime().c_str(), "Maximum as Amplitude of Channel 1", bin[1], min[1], max[1]);

	if(outfile->GetDirectory(dtStart->getDate().c_str()) == nullptr)
	    outfile->mkdir(dtStart->getDate().c_str(), dtStart->getDate().c_str());

	cout << dtStart->getDateTime() << " - " << dtEnd->getDateTime() << endl;

	for(unsigned int iData = 0; iData < dataList.size(); iData++) {
	    if(iData + 1 >= dataList.size())
		willBeDealed = hasDataInInterval(dataList[iData], dtStart, dtEnd, "null");
	    else
		willBeDealed = hasDataInInterval(dataList[iData], dtStart, dtEnd, dataList[iData + 1]);

	    if(willBeDealed) {
		cout << dataList[iData] << endl;
		DataReader* dr = new DataReader(dataList[iData]);
		dr->setQuantity("Voltage");
		dr->setStartDateTime(dtStart->getDateTime());
		dr->setEndDateTime(dtEnd->getDateTime());
		dr->runFillingLoop(hCh1, 1);
	    }
	}

	hCh1->SetXTitle("Voltage (V)");
	hCh1->SetYTitle("Entries");

	outfile->cd(dtStart->getDate().c_str());
	hCh1->Write();
    }

    outfile->Close();
}



void makePlots::doCoincidence(int goalCh, int threCh, double threshold) {
    initialize();
    prepareOutputFile("Coincidence");

    for(unsigned int iInt = 0; iInt < startDateTime.size(); iInt++) {
	bool willBeDealed = false;
	Calendar *dtStart = new Calendar(startDateTime[iInt]);
	Calendar *dtEnd = new Calendar(endDateTime[iInt]);

	TH1D* hGoal = new TH1D(dtStart->getTime().c_str(), "Coincidence", bin[goalCh], min[goalCh], max[goalCh]);

	if(outfile->GetDirectory(dtStart->getDate().c_str()) == nullptr)
	    outfile->mkdir(dtStart->getDate().c_str(), dtStart->getDate().c_str());

	cout << dtStart->getDateTime() << " - " << dtEnd->getDateTime() << endl;

	for(unsigned int iData = 0; iData< dataList.size(); iData++) {
	    if(iData + 1 >= dataList.size())
		willBeDealed = hasDataInInterval(dataList[iData], dtStart, dtEnd, "null");
	    else
		willBeDealed = hasDataInInterval(dataList[iData], dtStart, dtEnd, dataList[iData + 1]);

	    if(willBeDealed) {
		cout << dataList[iData] << endl;
		DataReader* dr = new DataReader(dataList[iData]);
		dr->setQuantity(quantity);
		dr->setStartDateTime(dtStart->getDateTime());
		dr->setEndDateTime(dtEnd->getDateTime());
		dr->runCoincidenceFilling(hGoal, goalCh, threshold);
	    }
	}

	if(quantity == "Voltage")
	    hGoal->SetXTitle("Voltage (V)");
	else if(quantity == "Energy")
	    hGoal->SetXTitle("Energy (MeV)");

	hGoal->SetYTitle("Entries");

	outfile->cd(dtStart->getDate().c_str());
	hGoal->Write();
    }

    outfile->Close();
}



void makePlots::test() {
    
    ifstream testFile;
    testFile.open("recordDoc/test.txt");
    string input;
    string line;
    while(testFile.is_open()) {
	if(testFile.eof()) break;

	getline(testFile, line);
	if(!line.empty())
	    input = line;
    }

    testFile.close();
    
    Calendar* c1 = new Calendar(input);
    Calendar* c2 = new Calendar("20210201000000");

    Duration dr = *c1 - *c2;
    cout << dr.sec/3600. << endl;

    //cout << (*c1 >= *c2) << endl;

    //assignTimeIntervals();
    

    //execute(0);
}



void makePlots::readAnalysisParameter() {
    GetExterSet ges { "recordDoc/Control.yaml" };

    string energyUnit = ges.giveStrVar("energy");
    if(energyUnit == "MeV") unitConverter = 1.;

    quantity = ges.giveStrVar("quantity");

    string binline = ges.giveStrVar("bin");
    while(!binline.empty()) {
	Int_t gottenBin;
	if(binline.find(",") == string::npos) {
	    gottenBin = atoi(binline.substr(binline.find_first_not_of(" ")).c_str());
	    bin.push_back(gottenBin);
	    binline.clear();
	} else {
	    gottenBin = atoi(binline.substr(binline.find_first_not_of(" "), binline.find_first_of(",") - binline.find_first_not_of(" ")).c_str());
	    bin.push_back(gottenBin);
	    binline = binline.substr(binline.find_first_of(",") + 1);
	}
    }

    string minline = ges.giveStrVar("min");
    while(!minline.empty()) {
	Double_t gottenMin;
	if(minline.find(",") == string::npos) {
	    gottenMin = atof(minline.substr(minline.find_first_not_of(" ")).c_str());
	    min.push_back(gottenMin);
	    minline.clear();
	} else {
	    gottenMin = atof(minline.substr(minline.find_first_not_of(" "), minline.find_first_of(",") - minline.find_first_not_of(" ")).c_str());
	    min.push_back(gottenMin);
	    minline = minline.substr(minline.find_first_of(",") + 1);
	}
    }

    string maxline = ges.giveStrVar("max");
    while(!maxline.empty()) {
	Double_t gottenMax;
	if(maxline.find(",") == string::npos) {
	    gottenMax = atof(maxline.substr(maxline.find_first_not_of(" ")).c_str());
	    max.push_back(gottenMax);
	    maxline.clear();
	} else {
	    gottenMax = atof(maxline.substr(maxline.find_first_not_of(" "), maxline.find_first_of(",") - maxline.find_first_not_of(" ")).c_str());
	    max.push_back(gottenMax);
	    maxline = maxline.substr(maxline.find_first_of(",") + 1);
	}
    }
}



void makePlots::prepareDataList() {
    DIR *dir;
    struct dirent *diread;
    string filename;

    if((dir = opendir("data/")) != nullptr) {
	while((diread = readdir(dir)) != nullptr) {
	    filename = diread->d_name;
	    if(filename != "." && filename != "..")
		dataList.push_back(filename.substr(0, filename.find(".root")));
	}
	closedir(dir);
    } else {
	perror("opendir");
    }

    sort(dataList.begin(), dataList.end());
}



void makePlots::prepareOutputFile(string outfileName) {
    char outputFilename[200];
    sprintf(outputFilename, "analyzedFile/%s.root", outfileName.c_str());
    if(outfile == nullptr)
	outfile = new TFile(outputFilename, "RECREATE");
    else {
	delete outfile;
	outfile = new TFile(outputFilename, "RECREATE");
    }
}



void makePlots::assignTimeIntervals() {
    ifstream intervalFile;
    intervalFile.open("recordDoc/timeIntervalList.txt");

    string startline;
    string endline;
    string line;
    while(intervalFile.is_open()) {
	if(intervalFile.eof()) break;
	
	getline(intervalFile, line);
	if(!line.empty()) {
	    startline = line.substr(0, line.find_first_of("-"));
	    endline = line.substr(line.find_first_of("-"));

	    startDateTime.push_back(startline.substr(0, startline.find_last_not_of(" -")));
	    endDateTime.push_back(endline.substr(endline.find_first_not_of(" -")));
	}
    }

    intervalFile.close();
}


void makePlots::assignSingleLengthOfIntervals() {
    GetExterSet ges { "recordDoc/SingleLengthOfInterval.txt" };

    string startline = ges.giveStrVar("StartTime");
    int interHour = ges.giveIntVar("intervalHour");
    int interMin = ges.giveIntVar("intervalMinute");
    double interSec = ges.giveDoubleVar("intervalSecond");
    int numOfTaking = ges.giveIntVar("NumberOfTaking");

    Calendar* cld = new Calendar(startline);
    for(int i = 0; i < numOfTaking; i++) {
	startDateTime.push_back(cld->getDateTime());
	cld->addDuration(0, 0, interHour, interMin, interSec);
	endDateTime.push_back(cld->getDateTime());
    }
}



bool makePlots::hasDataInInterval(string inputDTStr, string startDTStr, string endDTStr, string nextDTStr) {
    Calendar *dtData = new Calendar(inputDTStr);
    Calendar *dtStart = new Calendar(startDTStr);
    Calendar *dtEnd = new Calendar(endDTStr);

    if(nextDTStr == "null") {
	return hasDataInInterval(dtData, dtStart, dtEnd, nullptr);
    } else {
	Calendar *dtNext = new Calendar(nextDTStr);
	return hasDataInInterval(dtData, dtStart, dtEnd, dtNext);
    }
}



bool makePlots::hasDataInInterval(string inputDTStr, Calendar* startDT, Calendar* endDT, string nextDTStr) {
    Calendar *dtData = new Calendar(inputDTStr);

    if(nextDTStr == "null") {
	return hasDataInInterval(dtData, startDT, endDT, nullptr);
    } else {
	Calendar *dtNext = new Calendar(nextDTStr);
	return hasDataInInterval(dtData, startDT, endDT, dtNext);
    }
}



bool makePlots::hasDataInInterval(Calendar* inputDT, Calendar* startDT, Calendar* endDT, Calendar* nextDT) {
    if(*inputDT >= *startDT && *inputDT < *endDT)
	return true;
    else if(*inputDT < *startDT) {
	if(nextDT != nullptr) {
	    if(*nextDT > *startDT)
		return true;
	    else
		return false;
	} else
	    return false;
    } else
	return false;
}

#endif
