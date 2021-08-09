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

makePlots::makePlots() {
    unitConverter = 1.;
    conversingFactorCh0 = 1.;
    quantity = "Voltage";

    outfile = nullptr;
    filename = "";
    fileStat = "RECREATE";
    isFileSet = false;
    isInit = false;
}



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

	prepareOutputFile(filename);
	isInit = true;
    }
}



void makePlots::execute() {    
    makeHistoCh0();
    //makeHistoCh1();
    //doCoincidence(0, 1, 0.0055);
    //energyRange = "peak06";
    //assignCutEnergyRange(energyRange, lower, upper);
    //collectWithFilter(energyRange, lower, upper);
    //collectWithFilter("peak16", 1.5, 1.64);
    //collectWithFilter("peak04", 0.385, 0.41);
    //collectWithFilter("peak24", 2.35, 2.45);
    //collectWithFilter("0to25", 0., 2.5);

    //collectWithDynamicFilter("peak16");

    outfile->Close();
}



void makePlots::makeHistoCh0() {
    initialize();

    for(unsigned int iInt = 0; iInt < startDateTime.size(); iInt++) {
	bool willBeDealed = false;
	Calendar *dtStart = new Calendar(startDateTime[iInt]);
	Calendar *dtEnd = new Calendar(endDateTime[iInt]);

	TH1D* hCh0 = new TH1D(dtStart->getTime().c_str(), "Maximum as Amplitude of Channel 0", bin[0], min[0], max[0]);

	string folderName = "HistoCh0";
	if(outfile->GetDirectory(folderName.c_str()) == nullptr)
	    outfile->mkdir(folderName.c_str());

	char histoPath[150];
	sprintf(histoPath, "%s/%s", folderName.c_str(), dtStart->getDate().c_str());
	if(outfile->GetDirectory(histoPath) == nullptr)
	    outfile->mkdir(histoPath, histoPath);

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

		delete dr;
	    }
	}

	if(quantity == "Voltage")
	    hCh0->SetXTitle("Voltage (V)");
	else if(quantity == "Energy")
	    hCh0->SetXTitle("Energy (MeV)");

	hCh0->SetYTitle("Entries");

	outfile->cd(histoPath);
	hCh0->Write();

	delete hCh0;
	delete dtEnd;
	delete dtStart;
    }
}



void makePlots::makeHistoCh1() {
    initialize();

    for(unsigned int iInt = 0; iInt < startDateTime.size(); iInt++) {
	bool willBeDealed = false;
	Calendar *dtStart = new Calendar(startDateTime[iInt]);
	Calendar *dtEnd = new Calendar(endDateTime[iInt]);

	TH1D* hCh1 = new TH1D(dtStart->getTime().c_str(), "Maximum as Amplitude of Channel 1", bin[1], min[1], max[1]);

	string folderName = "HistoCh1";
	if(outfile->GetDirectory(folderName.c_str()) == nullptr)
	    outfile->mkdir(folderName.c_str());

	char histoPath[150];
	sprintf(histoPath, "%s/%s", folderName.c_str(), dtStart->getDate().c_str());
	if(outfile->GetDirectory(histoPath) == nullptr)
	    outfile->mkdir(histoPath, histoPath);

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

		delete dr;
	    }
	}

	hCh1->SetXTitle("Voltage (V)");
	hCh1->SetYTitle("Entries");

	outfile->cd(histoPath);
	hCh1->Write();

	delete dtStart;
	delete dtEnd;
	delete hCh1;
    }
}



void makePlots::doCoincidence(int goalCh, int threCh, double threshold) {
    initialize();

    for(unsigned int iInt = 0; iInt < startDateTime.size(); iInt++) {
	bool willBeDealed = false;
	Calendar *dtStart = new Calendar(startDateTime[iInt]);
	Calendar *dtEnd = new Calendar(endDateTime[iInt]);

	TH1D* hGoal = new TH1D(dtStart->getTime().c_str(), "Coincidence", bin[goalCh], min[goalCh], max[goalCh]);

	string folderName = "Coincidence";
	if(outfile->GetDirectory(folderName.c_str()) == nullptr)
	    outfile->mkdir(folderName.c_str());

	char histoPath[150];
	sprintf(histoPath, "%s/%s", folderName.c_str(), dtStart->getDate().c_str());
	if(outfile->GetDirectory(histoPath) == nullptr)
	    outfile->mkdir(histoPath, histoPath);

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

		delete dr;
	    }
	}

	if(quantity == "Voltage")
	    hGoal->SetXTitle("Voltage (V)");
	else if(quantity == "Energy")
	    hGoal->SetXTitle("Energy (MeV)");

	hGoal->SetYTitle("Entries");

	outfile->cd(histoPath);
	hGoal->Write();

	delete dtStart;
	delete dtEnd;
	delete hGoal;
    }
}



void makePlots::collectWithFilter(string filtingRegion,
				  double low, double up) {
    initialize();

    for(unsigned int iInt = 0; iInt < startDateTime.size(); iInt++) {
	bool willBeDealed = false;
	Calendar* dtStart = new Calendar(startDateTime[iInt]);
	Calendar* dtEnd = new Calendar(endDateTime[iInt]);

	TH1D* hCh0 = new TH1D(dtStart->getTime().c_str(), "Maximum as Amplitude of Channel 0", bin[0], min[0], max[0]);

	string folderName = "withFilt_" + filtingRegion;
	if(outfile->GetDirectory(folderName.c_str()) == nullptr)
	    outfile->mkdir(folderName.c_str());

	char histoPath[150];
	sprintf(histoPath, "%s/%s", folderName.c_str(), dtStart->getDate().c_str());
	if(outfile->GetDirectory(histoPath) == nullptr)
	    outfile->mkdir(histoPath, histoPath);

	cout << dtStart->getDateTime() << " - " << dtEnd->getDateTime() << endl;

	for(unsigned int iData = 0; iData < dataList.size(); iData++) {
	    if(iData + 1 >= dataList.size())
		willBeDealed = hasDataInInterval(dataList[iData], dtStart, dtEnd, "null");
	    else
		willBeDealed = hasDataInInterval(dataList[iData], dtStart, dtEnd, dataList[iData + 1]);

	    if(willBeDealed) {
		cout << dataList[iData] << endl
		     << filtingRegion << endl;
		DataReader* dr = new DataReader(dataList[iData]);
		dr->setQuantity(quantity);
		dr->setStartDateTime(dtStart->getDateTime());
		dr->setEndDateTime(dtEnd->getDateTime());
		dr->setThreshold(true, 0.3);
		dr->runFilterFilling(hCh0, 0, low, up);

		delete dr;
	    }
	}

	if(quantity == "Voltage")
	    hCh0->SetXTitle("Voltage (V)");
	else if(quantity == "Energy")
	    hCh0->SetXTitle("Energy (MeV)");

	hCh0->SetYTitle("Entries");

	outfile->cd(histoPath);
	hCh0->Write();

	delete hCh0;
	delete dtEnd;
	delete dtStart;	
    }
}



void makePlots::collectWithDynamicFilter(string filtingRegion) {
    initialize();

    TFile* fmean = new TFile("ready/fitResult.root");
    TTree* meanTree;
    int entryNo;
    int year;
    int month;
    int day;
    int hour;
    int minute;
    double sec;

    double meang04;
    double meang06;
    double meang16;

    double stdg04;
    double stdg06;
    double stdg16;

    fmean->GetObject("dataFitting", meanTree);
    meanTree->SetBranchAddress("entryNo", &entryNo);
    meanTree->SetBranchAddress("year", &year);
    meanTree->SetBranchAddress("month", &month);
    meanTree->SetBranchAddress("day", &day);
    meanTree->SetBranchAddress("hour", &hour);
    meanTree->SetBranchAddress("minute", &minute);
    meanTree->SetBranchAddress("second", &sec);

    meanTree->SetBranchAddress("meang04", &meang04);
    meanTree->SetBranchAddress("meang06", &meang06);
    meanTree->SetBranchAddress("meang16", &meang16);

    meanTree->SetBranchAddress("stdg04", &stdg04);
    meanTree->SetBranchAddress("stdg06", &stdg06);
    meanTree->SetBranchAddress("stdg16", &stdg16);

    map<string, double> mapmeang04;
    map<string, double> mapmeang06;
    map<string, double> mapmeang16;

    map<string, double> mapstdg04;
    map<string, double> mapstdg06;
    map<string, double> mapstdg16;

    Long64_t nentries = meanTree->GetEntries();
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	meanTree->GetEntry(entry);
	char tag[15];
	sprintf(tag, "%04d%02d%02d%02d%02d%02.f", year, month, day, hour, minute, sec);

	mapmeang04[tag] = meang04;
	mapmeang06[tag] = meang06;
	mapmeang16[tag] = meang16;

	mapstdg04[tag] = stdg04;
	mapstdg06[tag] = stdg06;
	mapstdg16[tag] = stdg16;
    }

    for(unsigned int iInt = 0; iInt < startDateTime.size(); iInt++) {
	bool willBeDealed = false;
	Calendar* dtStart = new Calendar(startDateTime[iInt]);
	Calendar* dtEnd = new Calendar(endDateTime[iInt]);

	TH1D* hCh0 = new TH1D(dtStart->getTime().c_str(), "Maximum as Amplitude of Channel 0", bin[0], min[0], max[0]);

	string folderName = "withFilt_" + filtingRegion;
	if(outfile->GetDirectory(folderName.c_str()) == nullptr)
	    outfile->mkdir(folderName.c_str());

	char histoPath[150];
	sprintf(histoPath, "%s/%s", folderName.c_str(), dtStart->getDate().c_str());
	if(outfile->GetDirectory(histoPath) == nullptr)
	    outfile->mkdir(histoPath, histoPath);

	cout << dtStart->getDateTime() << " - " << dtEnd->getDateTime() << endl;

	for(unsigned int iData = 0; iData < dataList.size(); iData++) {
	    if(iData + 1 >= dataList.size())
		willBeDealed = hasDataInInterval(dataList[iData], dtStart, dtEnd, "null");
	    else
		willBeDealed = hasDataInInterval(dataList[iData], dtStart, dtEnd, dataList[iData + 1]);

	    if(willBeDealed) {
		cout << dataList[iData] << endl
		     << filtingRegion << endl;

		string tag = dtStart->getDateTime();
		double low = 0.;
		double up = 2.5;
		double stdwidth = 2.;

		if(filtingRegion == "peak04") {
		    low = mapmeang04[tag] - stdwidth*mapstdg04[tag];
		    up = mapmeang04[tag] + stdwidth*mapstdg04[tag];
		} else if(filtingRegion == "peak06") {
		    low = mapmeang06[tag] - stdwidth*mapstdg06[tag];
		    up = mapmeang06[tag] + stdwidth*mapstdg06[tag];
		} else if(filtingRegion == "peak16") {
		    low = mapmeang16[tag] - stdwidth*mapstdg16[tag];
		    up = mapmeang16[tag] + stdwidth*mapstdg16[tag];
		}

		DataReader* dr = new DataReader(dataList[iData]);
		dr->setQuantity(quantity);
		dr->setStartDateTime(dtStart->getDateTime());
		dr->setEndDateTime(dtEnd->getDateTime());
		dr->setThreshold(true, 0.3);
		dr->runFilterFilling(hCh0, 0, low, up);

		delete dr;
	    }
	}

	if(quantity == "Voltage")
	    hCh0->SetXTitle("Voltage (V)");
	else if(quantity == "Energy")
	    hCh0->SetXTitle("Energy (MeV)");

	hCh0->SetYTitle("Entries");
	outfile->cd(histoPath);
	hCh0->Write();

	delete hCh0;
	delete dtEnd;
	delete dtStart;
    }
}



void makePlots::mergeHist() {
    vector<string> mergingFilename;

    mergingFilename.push_back("analyzedFile/0418to0607oneHist.root");
    mergingFilename.push_back("analyzedFile/0418to0607aHistPD.root");
    mergingFilename.push_back("analyzedFile/0418to0607aHistP12h.root");
    mergingFilename.push_back("analyzedFile/0418to0607aHistP06h.root");
    mergingFilename.push_back("analyzedFile/0418to0607aHistP02h.root");
    mergingFilename.push_back("analyzedFile/0418to0607aHistP01h.root");

    for(unsigned int i = 0; i < mergingFilename.size(); i++) {
	TFile* mergingFile = new TFile(mergingFilename[i].c_str(), "UPDATE");

	string dir04n06name = "withFilt_peak04n06";
	if(mergingFile->GetDirectory(dir04n06name.c_str()) == nullptr)
	    mergingFile->mkdir(dir04n06name.c_str());

	string dir04name = "withFilt_peak04";
	string dir06name = "withFilt_peak06";

	TDirectory* dir04n06 = mergingFile->GetDirectory(dir04n06name.c_str());
	TDirectory* dir04 = mergingFile->GetDirectory(dir04name.c_str());
	TDirectory* dir06 = mergingFile->GetDirectory(dir06name.c_str());
	TList* listDate = dir04->GetListOfKeys();
	listDate->Sort();

	TIter iterDate(listDate);
	TObject* objDate = nullptr;
	while((objDate = iterDate())) {
	    if(dir04n06->GetDirectory(objDate->GetName()) == nullptr)
		dir04n06->mkdir(objDate->GetName());

	    TDirectory* dir04n06Date = dir04n06->GetDirectory(objDate->GetName());
	    TDirectory* dir04Date = dir04->GetDirectory(objDate->GetName());
	    TDirectory* dir06Date = dir06->GetDirectory(objDate->GetName());
	    TList* listTime = dir04Date->GetListOfKeys();

	    TIter iterTime(listTime);
	    TObject* objTime = nullptr;
	    while((objTime = iterTime())) {
		TH1D* thisH04 = (TH1D*)dir04Date->Get(objTime->GetName());
		TH1D* thisH06 = (TH1D*)dir06Date->Get(objTime->GetName());
		TH1D* thisH04n06 = (TH1D*)thisH04->Clone();
		thisH04n06->Add(thisH06);

		dir04n06Date->cd();
		thisH04n06->Write();
	    }
	}

	mergingFile->Close();
	delete mergingFile;
    }
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

    delete c1;
    delete c2;
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
    //if(isFileSet) return;

    char outputFilename[300];
    sprintf(outputFilename, "analyzedFile/%s.root", outfileName.c_str());
    if(outfile == nullptr)
	outfile = new TFile(outputFilename, fileStat.c_str());
    else {
	//outfile->Close();
	//outfile = outfile->Open(outputFilename, "UPDATE");
	return;
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
	    if(line.find(":") != string::npos)
		continue;

	    startline = line.substr(0, line.find_first_of("-"));
	    endline = line.substr(line.find_first_of("-"));

	    startDateTime.push_back(startline.substr(0, startline.find_last_not_of(" -") + 1));
	    endDateTime.push_back(endline.substr(endline.find_first_not_of(" -")));
	}
    }

    intervalFile.close();

    GetExterSet ges { "recordDoc/timeIntervalList.txt" };
    filename = ges.giveStrVar("Filename");
}


void makePlots::assignSingleLengthOfIntervals() {
    GetExterSet ges { "recordDoc/SingleLengthOfInterval.txt" };

    filename = ges.giveStrVar("Filename");
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



void makePlots::assignCutEnergyRange(string peak, double& lower, double& upper) {
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

	delete dtNext;
    }

    delete dtData;
    delete dtStart;
    delete dtEnd;
}



bool makePlots::hasDataInInterval(string inputDTStr, Calendar* startDT, Calendar* endDT, string nextDTStr) {
    Calendar *dtData = new Calendar(inputDTStr);

    if(nextDTStr == "null") {
	return hasDataInInterval(dtData, startDT, endDT, nullptr);
    } else {
	Calendar *dtNext = new Calendar(nextDTStr);
	return hasDataInInterval(dtData, startDT, endDT, dtNext);

	delete dtNext;
    }

    delete dtData;
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
