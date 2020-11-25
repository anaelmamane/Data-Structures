#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <string>
#include <unordered_map>

using namespace std;
	string icu; //ICU admittance
	//AVL class that has nodes, the case
	class AVL_Tree {
	public:
		struct Case {
			string dateCDC; //Holds the data the CDC report data
			string sex; //Holds the sex of the person
			string age; //Holds the age range of the person
			string race; //Holds the race person
			string hospitalization; //Hospitalization
			string icu; //ICU admittance
			string death; //Did the person die
			string medicalCondition; //Did the person have a medical condition
		};
		Case* root;
	};

//CDC_Report  N/A   Sex    Age     Race     Hosp     ICU    Death    MedCond
int main() {
	//Map that holds cases
	unordered_map<string, vector<AVL_Tree::Case>> caseList;
	//Temporary Case object
	AVL_Tree::Case temp;
	//creates an ifstream object
	ifstream file;
	// Load the states
	string name;

	//Gets data for State data
	file.open("cases.csv");
	if (file.is_open()) {
		//Ignores title lines
		getline(file, name);

		//Loads data from file into map using commma as delimeter
		while (getline(file, temp.dateCDC, ',')) {
			getline(file, name, ',');
			getline(file, temp.sex, ',');
			getline(file, temp.age, ',');
			getline(file, temp.race, ',');
			getline(file, temp.hospitalization, ',');
			getline(file, temp.icu, ',');
			getline(file, temp.death, ',');
			getline(file, temp.medicalCondition, '\n');

			//Places data into a map
			caseList[temp.age].push_back(temp);			
		}
	}
}
