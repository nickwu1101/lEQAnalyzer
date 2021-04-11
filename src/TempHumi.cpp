#include "TempHumi.h"

#include "Calendar.h"

#include <TFile.h>
#include <TGraph.h>

#include <dirent.h>
#include <fstream>

TempHumi::TempHumi() {
    startDateTime = new Calendar("20210101000000");
    endDateTime = new Calendar("20210102000000");
}



TempHumi::~TempHumi() {}



void TempHumi::execute() {
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
    makeTempHumiPlotting();
}



void TempHumi::makeTempHumiPlotting() {
    TFile *f = new TFile("analyzedFile/TempHumi.root");
    
}



void TempHumi::doTempHumiMap() {
    for(unsigned int iData = 0; iData < dataList.size(); iData++) {
	Calendar* dataDateTime = new Calendar(dataList[iData]);

	if(*dataDateTime >= *startDateTime && *dataDateTime <= *endDateTime) {
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
