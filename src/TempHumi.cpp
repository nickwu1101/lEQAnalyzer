#include "TempHumi.h"

#include "Calendar.h"

#include <TAxis.h>
#include <TFile.h>
#include <TGraph.h>

#include <dirent.h>
#include <fstream>

TempHumi::TempHumi() {
    startDateTime = new Calendar("20210101000000");
    endDateTime = new Calendar("20210102000000");
    timeLengthOfGraph = "day";
    timeUnit = "1hour";
}



TempHumi::~TempHumi() {}



void TempHumi::execute() {
    assignTimeInterval();
    prepareDataList();
    doTempHumiMap();
    makeTempHumiPlotting();
}



void TempHumi::test() {
    ifstream DHTfile;
    DHTfile.open("csv/20210411080000.csv");

    vector<double> measuredTemp;
    vector<double> measuredHumi;

    string line;
    while(DHTfile.is_open()) {
	if(DHTfile.eof()) break;
	getline(DHTfile, line);

	if(line.length() != 0) {
	    string cutLine = line.substr(line.find_first_of(",") + 1);
	    string tempStr = cutLine.substr(0, cutLine.find_first_of(","));
	    string humiStr = cutLine.substr(cutLine.find_first_of(",") + 1);

	    measuredTemp.push_back(atof(tempStr.c_str()));
	    measuredHumi.push_back(atof(humiStr.c_str()));
	}
    }

    double avgTemp = 0;
    double avgHumi = 0;

    for(unsigned int i = 0; i < measuredTemp.size(); i++)
	avgTemp += measuredTemp[i];

    for(unsigned int i = 0; i < measuredHumi.size(); i++)
	avgHumi += measuredHumi[i];

    avgTemp /= (double)measuredTemp.size();
    avgHumi /= (double)measuredHumi.size();

    cout << avgTemp << endl << avgHumi << endl;

    cout << calAvgTemp("20210411080000") << endl << calAvgHumi("20210411080000") << endl;

    prepareDataList();
    for(unsigned int i = 0; i < dataList.size(); i++) {
	cout << dataList[i] << endl;
    }

    cout << "-----------------------------------------------------" << endl;

    setStartDateTime("20210409000000");
    setEndDateTime("20210410000000");
    doTempHumiMap();
    makeTempHumiPlotting();
}



void TempHumi::makeTempHumiPlotting() {
    TFile *f = new TFile("analyzedFile/TempHumi.root", "RECREATE");

    f->mkdir("temperature", "temperature");
    f->mkdir("humidity", "humidity");

    map<string, TGraph*> gT;
    map<string, TGraph*> gH;

    double xtime = 0;
    double ytemp = 0;
    double yhumi = 0;

    for(map<string, double>::iterator it = temp.begin();
	it != temp.end(); ++it) {
	ytemp = it->second;
	yhumi = humi[it->first];

	Calendar* dt = new Calendar(it->first);

	string adding2Graph;

	string xAxisTitle;
	if(timeLengthOfGraph == "day") {
	    xAxisTitle = "Time (h)";
	    adding2Graph = dt->getDate();

	    if(timeUnit == "15min") {
		xtime = (double)dt->getHour() + ((double)dt->getMinute()/60.);
	    } else if(timeUnit == "1hour") {
		if(dt->getMinute() != 0 || dt->getSecond() != 0)
		    continue;
		else
		    xtime = (double)dt->getHour();
	    }
	} else if (timeLengthOfGraph == "month") {
	    adding2Graph = dt->getDate().substr(0, 6);
	} else if (timeLengthOfGraph == "year") {
	    adding2Graph = dt->getDate().substr(0, 4);
	}

	TGraph* thisGT = nullptr;
	TGraph* thisGH = nullptr;
	if(gT.find(adding2Graph) == gT.cend()) {
	    gT[adding2Graph] = new TGraph();
	    thisGT = gT[adding2Graph];

	    char line[100];
	    thisGT->SetName(adding2Graph.c_str());
	    sprintf(line, "Temperature in %s", adding2Graph.c_str());
	    thisGT->SetTitle(line);
	    thisGT->GetXaxis()->SetTitle(xAxisTitle.c_str());
	    thisGT->GetYaxis()->SetTitle("Temperature ({}^{#circ}C)");
	    thisGT->GetYaxis()->SetRangeUser(0., 40.);
	    thisGT->SetLineColor(kRed);
	} else {
	    thisGT = gT[adding2Graph];
	}

	if(gH.find(adding2Graph) == gH.cend()) {
	    gH[adding2Graph] = new TGraph();
	    thisGH = gH[adding2Graph];

	    char line[100];
	    thisGH->SetName(adding2Graph.c_str());
	    sprintf(line, "Humidity in %s", adding2Graph.c_str());
	    thisGH->SetTitle(line);
	    thisGH->GetXaxis()->SetTitle(xAxisTitle.c_str());
	    thisGH->GetYaxis()->SetTitle("Humidity (\%)");
	    thisGH->GetYaxis()->SetRangeUser(0., 100.);
	    thisGH->SetLineColor(kBlue);
	} else {
	    thisGH = gH[adding2Graph];
	}

	thisGT->SetPoint(thisGT->GetN(), xtime, ytemp);
	thisGH->SetPoint(thisGH->GetN(), xtime, yhumi);
    }

    f->cd("temperature");
    for(map<string, TGraph*>::iterator it = gT.begin();
	it != gT.end(); ++it) {
	it->second->Write();
    }

    f->cd("humidity");
    for(map<string, TGraph*>::iterator it = gH.begin();
	it != gH.end(); ++it) {
	it->second->Write();
    }

    f->Close();
}



void TempHumi::doTempHumiMap() {
    for(unsigned int iData = 0; iData < dataList.size(); iData++) {
	Calendar* dataDateTime = new Calendar(dataList[iData]);

	if(*dataDateTime >= *startDateTime && *dataDateTime < *endDateTime) {
	    string thisDT = dataDateTime->getDateTime();
	    temp[thisDT] = calAvgTemp(thisDT);
	    humi[thisDT] = calAvgHumi(thisDT);
	}
    }
}



void TempHumi::setEndDateTime(string dtStr) {
    endDateTime->setDateTime(dtStr);
}



void TempHumi::setStartDateTime(string dtStr) {
    startDateTime->setDateTime(dtStr);
}



double TempHumi::calAvgTemp(string dateTime) {
    ifstream DHTfile;
    DHTfile.open("csv/" + dateTime + ".csv");

    vector<double> measuredTemp;

    string line;
    while(DHTfile.is_open()) {
	if(DHTfile.eof()) break;
	getline(DHTfile, line);

	if(line.length() != 0) {
	    string cutLine = line.substr(line.find_first_of(",") + 1);
	    string tempStr = cutLine.substr(0, cutLine.find_first_of(","));

	    measuredTemp.push_back(atof(tempStr.c_str()));
	}
    }

    double avgTemp = 0;
    for(unsigned int i = 0; i < measuredTemp.size(); i++)
	avgTemp += measuredTemp[i];

    avgTemp /= (double)measuredTemp.size();
    return avgTemp;
}



double TempHumi::calAvgHumi(string dateTime) {
    ifstream DHTfile;
    DHTfile.open("csv/" + dateTime + ".csv");

    vector<double> measuredHumi;

    string line;
    while(DHTfile.is_open()) {
	if(DHTfile.eof()) break;
	getline(DHTfile, line);

	if(line.length() != 0) {
	    string cutLine = line.substr(line.find_first_of(",") + 1);
	    string humiStr = cutLine.substr(cutLine.find_first_of(",") + 1);

	    measuredHumi.push_back(atof(humiStr.c_str()));
	}
    }

    double avgHumi = 0;
    for(unsigned int i = 0; i < measuredHumi.size(); i++)
	avgHumi += measuredHumi[i];

    avgHumi /= (double)measuredHumi.size();
    return avgHumi;
}



void TempHumi::assignTimeInterval() {
    ifstream timeIntervalFile;
    timeIntervalFile.open("recordDoc/TempHumiTimeInterval.txt");

    string startline;
    string endline;
    string line;
    while(timeIntervalFile.is_open()) {
	if(timeIntervalFile.eof()) break;

	getline(timeIntervalFile, line);
	if(line.length() != 0) {
	    startline = line.substr(0, line.find_first_of("-"));
	    endline = line.substr(line.find_first_of("-"));

	    setStartDateTime(startline.substr(0, startline.find_last_not_of(" -")));
	    setEndDateTime(endline.substr(endline.find_first_not_of(" -")));
	}
    }

    timeIntervalFile.close();
}



void TempHumi::prepareDataList() {
    DIR *dir;
    struct dirent *diread;
    string filename;

    if((dir = opendir("csv/")) != nullptr) {
	while((diread = readdir(dir)) != nullptr) {
	    filename = diread->d_name;
	    if(filename != "." && filename != "..")
		dataList.push_back(filename.substr(0, filename.find(".csv")));
	}
	closedir(dir);
    } else {
	perror("opendir");
    }

    sort(dataList.begin(), dataList.end());
}
