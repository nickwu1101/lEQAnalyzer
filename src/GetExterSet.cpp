#include "GetExterSet.h"

#include <fstream>

GetExterSet::GetExterSet(string inputTermFile) {
    loadTermList(inputTermFile);
}



GetExterSet::~GetExterSet() {}



void GetExterSet::loadTermList(string termFileStr) {
    ifstream termListFile;
    termListFile.open(termFileStr);

    string line;
    while(termListFile.is_open()) {
	if(termListFile.eof()) break;

	getline(termListFile, line);
	if(line.size() != 0 && line.find(":") != string::npos) {
	    OneTerm aTerm;
	    aTerm.termName = line.substr(0, line.find_first_of(":"));
	    string tailOfLine = line.substr(line.find_first_of(":") + 1);
	    aTerm.valueStr = tailOfLine.substr(tailOfLine.find_first_not_of(" "));
	    termList.push_back(aTerm);
	}
    }
    
    termListFile.close();
}



string GetExterSet::giveStrVar(string termRead) {
    for(unsigned int i = 0; i < termList.size(); i++) {
	if(termRead == termList[i].termName) {
	    return termList[i].valueStr;
	}
    }

    cout << "Cannot find the parameter of " << termRead << "!!!" << endl;
    exit(0);
}



int GetExterSet::giveIntVar(string termRead) {
    for(unsigned int i = 0; i < termList.size(); i++) {
	if(termRead == termList[i].termName) {
	    return atoi(termList[i].valueStr.c_str());
	}
    }

    cout << "Cannot find the parameter of " << termRead << "!!!" << endl;
    exit(0);
}



double GetExterSet::giveDoubleVar(string termRead) {
    for(unsigned int i = 0; i < termList.size(); i++) {
	if(termRead == termList[i].termName) {
	    return atof(termList[i].valueStr.c_str());
	}
    }

    cout << "Cannot find the parameter of " << termRead << "!!!" << endl;
    exit(0);
}



bool GetExterSet::giveBoolVar(string termRead) {
    for(unsigned int i = 0; i < termList.size(); i++) {
	if(termRead == termList[i].termName) {
	    if(termList[i].valueStr == "true" || termList[i].valueStr == "\'TRUE\'")
		return true;
	    else if(termList[i].valueStr == "false" || termList[i].valueStr == "\'FALSE\'")
		return false;
	    else
		cout << "The input text cannot be identified. Please check the parameter is fitted." << endl;
	    exit(0);
	}
    }

    cout << "Cannot find the parameter of " << termRead << "!!!" << endl;
    exit(0);
}
