#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <unordered_map>
#include <cstdint>
#include <vector>
#include <ctime>  
#include <chrono>

#define FIXED_DOUBLE4(x) std::fixed <<std::setprecision(4)<<(x)
#define FIXED_DOUBLE6(x) std::fixed <<std::setprecision(6)<<(x)

using namespace std;

//Object that holds a case
struct Case {
	bool sex; //Holds the sex of the person Male = 1 Female = 0
	char age; //Holds the age range of the person
	string race; //Holds the race person
	char hospitalization; //Hospitalization
	char icu; //ICU admittance
	char death; //Did the person die
	bool medicalCondition; //Did the person have a medical condition Yes = 1 No = 0
};

//Object that represesnts node and holds Case objects
struct Node {
	//Pointers that point left of right
	Node* left = nullptr;
	Node* right = nullptr;

	//Data for Node
	short day = 0;
	short month = 0;

	//Vector that holds Cases
	vector<Case*> cases;

	//Threshold of a Node
	short threshold = 0;
};

class AVL_Tree {
public:
	int size = 0; //Holds the number of cases in the tree
	Node* root = nullptr; //Holds the root of the tree

	//Method used to insert Node to AVL tree
	Node* insert_AVL(Node* root, Node* newNode, Case* newCase);
	
	//Methods needed for correct rotation
	short calculateThreshold(Node* newNode);
	short height(Node* newNode);
	Node* rotateRight(Node* newNode);
	Node* rotateLeft(Node* newNode);
	//Search for a case
	vector<Case*> Search(short month, short date, Node* root);
};

//Rebalances tree
Node* AVL_Tree::rotateLeft(Node* newNode) {
	Node* newparent = newNode->right;
	Node* temp = newparent->left;
	newparent->left = newNode;
	newNode->right = temp;
	return newparent;
}

//Rebalances tree
Node* AVL_Tree::rotateRight(Node* newNode) {
	Node* newparent = newNode->left;
	Node* temp = newparent->right;
	newparent->right = newNode;
	newNode->left = temp;
	return newparent;
}

//Finds the height of a node
short AVL_Tree::height(Node* root) {

	if (root == nullptr)
		return 0;
	else
		return 1 + std::max(height(root->left), height(root->right));
}

//Calculates threshold of a node
short AVL_Tree::calculateThreshold(Node* newNode) {
	short leftheight = height(newNode->left);
	short rightheight = height(newNode->right);
	newNode->threshold = leftheight - rightheight;
	return leftheight - rightheight;
}

//Inserts Node into tree
Node* AVL_Tree::insert_AVL(Node* root, Node* newNode, Case* newCase) {
	//If not case present in the root, add the case
	if (root == nullptr) {
		size++;
		newNode->cases.push_back(newCase);
		return newNode;
	}
	//If Case date is less than current root, go left
	else if (newNode->month < root->month) { //month
		root->left = insert_AVL(root->left, newNode, newCase);
		//calculateThreshold(root);
	}
	//If Case data is greater than current root, go right
	else if (newNode->month > root->month) { //month >
		root->right = insert_AVL(root->right, newNode, newCase);
		//calculateThreshold(root);
	}
	else { //If months equal
		if (newNode->day < root->day) {
			root->left = insert_AVL(root->left, newNode, newCase);
			//calculateThreshold(root);
		}
		else if (newNode->day > root->day) {
			root->right = insert_AVL(root->right, newNode, newCase);
			//calculateThreshold(root);
		}
		//Both day and month equal
		else {
			root->cases.push_back(newCase);
			//size++;
			return root;
		}
	}

	//balance at each insertion
	calculateThreshold(root);
	if (root->threshold == 2) {
		if (root->left->threshold == 1)
			return rotateRight(root);
		else if (root->left->threshold == -1) {
			root->left = rotateLeft(root->left);
			return rotateRight(root);
		}
	}
	else if (root->threshold == -2) {
		if (root->right->threshold == 1) {
			root->right = rotateRight(root->right);
			return rotateLeft(root);
		}
		else if (root->right->threshold == -1) {
			return rotateLeft(root);
		}
	}

	return root;
}

//Funtion that returns a Vector of cases, based on an input date
vector<Case*> AVL_Tree::Search(short month, short date, Node* root) {
	vector<Case*> found;
	//if not found
	if (root == nullptr) {
		vector<Case*> empty;
		return empty;
	}
	//if month is less than
	else if (root->month < month) {
		found = Search(month, date, root->right);
	}
	//month greater than
	else if (root->month > month) {
		found = Search(month, date, root->left);
	}
	//equal month
	else {
		//day less than
		if (root->day < date) {
			found = Search(month, date, root->right);
		}
		//day greater than
		else if (root->day > date) {
			found = Search(month, date, root->left);
		}
		//if everything is equal
		else {
			//cout << "Found root: " << root->month << "   " << root->day << endl;
			found = root->cases;
		}
	}
	return found;
}

//Hash funtion that recieves month and day and returns unique key
short hashf(short month, short day) {
	return 100 * month + day;
}

//CDC_Report  N/A   Sex    Age     Race     Hosp     ICU    Death    MedCond
int main() {
	//AVL Tree that holds nodes based on dates
	AVL_Tree* datesTree = new AVL_Tree;

	//Map that holds cases based on age
	unordered_map<char, vector<Case*>> caseAgeList;
	//Map that holds cases based on race
	unordered_map<string, vector<Case*>> caseRaceList;
	//Map that holds cases based on sex
	unordered_map<bool, vector<Case*>> caseListmapSex;
	//Map that holds cases based on medical condition
	unordered_map<bool, vector<Case*>> caseListmapMedCond;
	//Map that holds cases based on age for case 6
	unordered_map<char, vector<Case*>> temporaryAgeMap;
	//Map that holds cases based on sex for case 6
	unordered_map<bool, vector<Case*>> temporarySexMap;
	//Map that holds cases based on medical condition for case 6
	unordered_map<bool, vector<Case*>> temporaryMedCondMap;
	//Map that holds cases based on a date that was hashed
	unordered_map<short, vector<Case*>> datesMap;

	//Temporary key for 
	short tempKey;
	//creates an ifstream object
	ifstream file;
	// Load the states
	string temp;
	//Gets data for State data
	file.open("cases.csv");
	if (file.is_open()) {
		//Ignores title lines
		getline(file, temp);

		//Loads data from file into map using commma as delimeter
		while (getline(file, temp, ',')) {
			pair<int, int> pairforkey;

			//Creates a node for the tree using the Node object
			Node* newNode = new Node;
			//Create a new Case object for a case
			Case* newCase = new Case;

			//   Date format   ##/##/#### saved in datecdc
			//Grabs month and saves it as a 8 bit unsigned int
			newNode->month = (short)stoi(temp.substr(0, temp.find('/')));

			//Grabs day and saves it as a 8 bit unsigned int
			newNode->day = (short)stoi(temp.substr(temp.find('/') + 1, temp.size() - 7));

			//Laboratory status, not needed
			getline(file, temp, ',');

			//Saves sex as a bool: Male = 1 Female = 0
			getline(file, temp, ',');
			if (temp.at(0) == 'M')
				newCase->sex = 1;
			else
				newCase->sex = 0;

			//Saves age data by first character
			getline(file, temp, ',');
			newCase->age = temp.at(0);

			getline(file, newCase->race, ',');

			//Saves hospitalization data by first letter
			getline(file, temp, ',');
			newCase->hospitalization = temp.at(0);

			//Saves icu data by first letter
			getline(file, temp, ',');
			newCase->icu = temp.at(0);

			//Saves death data by first letter
			getline(file, temp, ',');
			newCase->death = temp.at(0);

			//Saves medical condition data by boolean
			getline(file, temp, '\n');
			if (temp.at(0) == 'Y')
				newCase->medicalCondition = 1;
			else
				newCase->medicalCondition = 0;


			//Places data into a map
			//map sorted by age
			caseAgeList[newCase->age].push_back(newCase);
			//map for comparison
			datesMap[hashf(newNode->month, newNode->day)].push_back(newCase);
			//map thats sorted by race
			caseRaceList[newCase->race].push_back(newCase);
			//map sorted by sex
			caseListmapSex[newCase->sex].push_back(newCase);
			//map sorted by medical condition
			caseListmapMedCond[newCase->medicalCondition].push_back(newCase);
			//tree sorted by date
			datesTree->root = datesTree->insert_AVL(datesTree->root, newNode, newCase);
		}
	}

	//main menu
	//bool that runs the while loop
	bool loop = true;

	while (loop) {
		cout << "============================== Welcome to COVID WATCH ==============================\n";
		cout << "====================================================================================\n";
		cout << "====================================================================================\n";
		//vector that holds the cases that match the date. Had to declare here for some reason???
		vector<Case*> matchDate;

		//int for deaths and hospitalizations
		short deaths = 0, hosp = 0, icuNum = 0;


		cout << "Please select from the menu below to pick the demographic from which to assess your risk factor.\n" <<
			"Or choose option 7 to compare two data structures\n" << endl;
		//input 2 might not be used as some inputs only require one input
		short input1, input2;

		//Menu option
		short option;
		cout << "Main Menu" << endl;
		//menu opptions
		cout << "1. Month and Day" << endl;
		cout << "2. Age" << endl;
		cout << "3. Race" << endl;
		cout << "4. Sex" << endl;
		cout << "5. Medical Condition" << endl;
		cout << "6. Race, Age, Sex and Medical Condition" << endl;
		cout << "7. Comparison" << endl;
		cout << "8. Quit" << endl;

		//receiving the input
		cin >> option;

		short ageinput;
		short raceinput;
		short sexinput;
		short medcondinput;

		//Temporary boolean key
		bool keyB;

		//Temporary char key
		char keyC;

		string key;
		string key1;
		string key2;
		string key3;
		//start time:has to be initialized outside of switch, will re-declare it in case 6
		auto start = std::chrono::system_clock::now();
		pair <int, int> mapkey;
		auto end = std::chrono::system_clock::now();

		time_t end_time = chrono::system_clock::to_time_t(end);


		//make a switch statement that checks option
		switch (option) {
		case 1:
			cout << "Insert a Month then a Date" << endl;
			cout << "Month: ";
			cin >> input1;
			cout << "Date: ";
			cin >> input2;

			cout << input1 << " " << input2 << endl;

			//vector of nodes that matches the searched for date
			matchDate = datesTree->Search(input1, input2, datesTree->root);
			//cout << "vector return size: " << matchDate.size();


			//Counts the total hospitalization, ICU cases and deaths within specied range
			for (unsigned int i = 0; i < matchDate.size(); i++) {
				if (matchDate[i]->death == 'Y')
					deaths++;
				if (matchDate[i]->icu == 'Y')
					icuNum++;
				if (matchDate[i]->hospitalization == 'Y')
					hosp++;
			}
			//bad input check
			if (input1 > 12 || input1 < 1 || input2 < 1 || input2 > 31) {
				cout << "Invalid Date!" << endl;
			}
			else {
				//Prints data
				cout << "Number of Hospitalizations on this Date: " << hosp << endl;
				cout << "Percentage of hospitalizations among chosen date: "
					<< FIXED_DOUBLE4( ((float)hosp / matchDate.size()) * 10.0)  << " %\n";
				cout << "Number of ICU cases on this Date: " << icuNum << endl;
				cout << "Percentage of ICU cases among chosen date: "
					<< FIXED_DOUBLE4( ((float)icuNum / matchDate.size()) * 10.0) << " %\n";
				cout << "Number of Deaths on this Date: " << deaths << endl;
				cout << "Percentage of deaths among chosen date: "
					<< FIXED_DOUBLE4( ((float)deaths / matchDate.size()) * 10.0) << " %\n";
			}
			break;
		case 2:
			//Asks user for their age
			cout << "Input your Age: " << endl;
			cin >> ageinput;
			if (ageinput <= 9)
				keyC = '0';
			else if (ageinput <= 19)
				keyC = '1';
			else if (ageinput <= 29)
				keyC = '2';
			else if (ageinput <= 39)
				keyC = '3';
			else if (ageinput <= 49)
				keyC = '4';
			else if (ageinput <= 59)
				keyC = '5';
			else if (ageinput <= 69)
				keyC = '6';
			else if (ageinput <= 79)
				keyC = '7';
			else if (ageinput >= 80)
				keyC = '8';

			//Counts the total hospitalization, ICU cases and deaths within specied range
			for (unsigned int i = 0; i < caseAgeList[keyC].size(); i++) {

				if (caseAgeList[keyC].at(i)->hospitalization == 'Y')
					hosp++;
				if (caseAgeList[keyC].at(i)->icu == 'Y')
					icuNum++;
				if (caseAgeList[keyC].at(i)->death == 'Y')
					deaths++;
			}

			//Prints data
			cout << "Number of Hospitalizations in your age range: " << hosp << endl;
			cout << "Percentage of hospitalizations among chosen age range: "
				<< FIXED_DOUBLE4( ((float)hosp / caseAgeList[keyC].size()) * 10.0) <<  " %\n";
			cout << "Number of ICU cases in your age range: " << icuNum << endl;
			cout << "Percentage of ICU cases among chosen age range: "
				<< FIXED_DOUBLE4( ((float)icuNum / caseAgeList[keyC].size()) * 10.0) <<  " %\n";
			cout << "Number of fatalities in your age range: " << deaths << endl;
			cout << "Percentage of deaths among chosen age range: "
				<< FIXED_DOUBLE4( ((float)deaths / caseAgeList[keyC].size()) * 10.0) <<  " %\n";


			break;
		case 3:
			//Asks user for their race
			cout << "Input your Race: " << endl;
			cout << "1. Hispanic/Latino" << endl;
			cout << "2. Black; Non-Hispanic" << endl;
			cout << "3. American Indian/Alaska Native; Non-Hispanic" << endl;
			cout << "4. Asian; Non-Hispanic" << endl;
			cout << "5. Native Hawaiian/Other Pacific Islander; Non-Hispanic" << endl;
			cout << "6. White; Non-Hispanic" << endl;
			cout << "7. Multiple/Other" << endl;
			cin >> raceinput;

			//Assign a key for the race map
			if (raceinput == 1)
				key = "Hispanic/Latino";
			else if (raceinput == 2)
				key = "Black; Non-Hispanic";
			else if (raceinput == 3)
				key = "American Indian/Alaska Native; Non-Hispanic";
			else if (raceinput == 4)
				key = "Asian; Non-Hispanic";
			else if (raceinput == 5)
				key = "Native Hawaiian/Other Pacific Islander; Non-Hispanic";
			else if (raceinput == 6)
				key = "White; Non-Hispanic";
			else if (raceinput == 7)
				key = "Multiple/Other; Non-Hispanic";
			else {
				cout << "Invalid Selection!" << endl;
				break;
			}

			//Counts the total hospitalization, ICU cases and deaths within specied range
			for (unsigned int i = 0; i < caseRaceList[key].size(); i++) {
				if (caseRaceList[key].at(i)->hospitalization == 'Y')
					hosp++;
				if (caseRaceList[key].at(i)->icu == 'Y')
					icuNum++;
				if (caseRaceList[key].at(i)->death == 'Y')
					deaths++;
			}

			//Prints data
			cout << "Number of Hospitalizations in your race: " << hosp << endl;
			cout << "Percentage of hospitalizations among chosen race: " <<
				FIXED_DOUBLE4( ((float)hosp / caseRaceList[key].size()) * 10.0) <<  " %\n";
			cout << "Number of ICU cases in your race: " << icuNum << endl;
			cout << "Percentage of ICU cases among chosen race: " <<
				FIXED_DOUBLE4( ((float)icuNum / caseRaceList[key].size()) * 10.0 ) << " %\n";
			cout << "Number of fatalities in your race: " << deaths << endl;
			cout << "Percentage of deaths among chosen race: " <<
				FIXED_DOUBLE4( ((float)deaths / caseRaceList[key].size()) * 10.0)  << " %\n";

			break;
		case 4:
			//Asks user for sex
			cout << "Input your Sex: " << endl;
			cout << "1. Male" << endl;
			cout << "2. Female" << endl;
			cin >> sexinput;
			if (sexinput == 1)
				keyB = 1;
			else if (sexinput == 2)
				keyB = 0;
			else {
				cout << "Invalid Selection!" << endl;
				break;
			}

			//Counts the total hospitalization, ICU cases and deaths within specied range
			for (unsigned int i = 0; i < caseListmapSex[keyB].size(); i++) {
				if (caseListmapSex[keyB].at(i)->hospitalization == 'Y')
					hosp++;
				if (caseListmapSex[keyB].at(i)->icu == 'Y')
					icuNum++;
				if (caseListmapSex[keyB].at(i)->death == 'Y')
					deaths++;
			}

			//Prints data
			cout << "Number of Hospitalizations in your sex: " << hosp << endl;
			cout << "Percentage of hospitalizations among chosen sex: " <<
				FIXED_DOUBLE4( ((float)hosp / caseListmapSex[keyB].size()) * 10.0) <<  " %\n";
			cout << "Number of ICU cases in your sex: " << icuNum << endl;
			cout << "Percentage of ICU cases among chosen sex: " <<
				FIXED_DOUBLE4( ((float)icuNum / caseListmapSex[keyB].size()) * 10.0) << " %\n";
			cout << "Number of fatalities in your sex: " << deaths << endl;
			cout << "Percentage of deaths among chosen sex: " <<
				FIXED_DOUBLE4( ((float)deaths / caseListmapSex[keyB].size()) * 10.0) << " %\n";

			break;
		case 5:
			//Asks user for medical condition
			cout << "Do you have a medical condition: " << endl;
			cout << "1. Yes" << endl;
			cout << "2. No" << endl;
			cin >> medcondinput;
			if (medcondinput == 1)
				keyB = 1;
			else if (medcondinput == 2)
				keyB = 0;
			else {
				cout << "Invalid Selection!" << endl;
				break;
			}

			//Counts the total hospitalization, ICU cases and deaths within specied range
			for (unsigned int i = 0; i < caseListmapMedCond[keyB].size(); i++) {
				if (caseListmapMedCond[keyB].at(i)->hospitalization == 'Y')
					hosp++;
				if (caseListmapMedCond[keyB].at(i)->icu == 'Y')
					icuNum++;
				if (caseListmapMedCond[keyB].at(i)->death == 'Y')
					deaths++;
			}

			//Prints data
			cout << "Number of Hospitalizations in your medical condition: " << hosp << endl;
			cout << "Percentage of hospitalizations among chosen medical condition: " <<
				FIXED_DOUBLE4( ((float)hosp / caseListmapMedCond[keyB].size()) * 10.0) <<  " %\n";
			cout << "Number of ICU cases in your medical condition: " << icuNum << endl;
			cout << "Percentage of ICU cases among chosen medical condition: " <<
				FIXED_DOUBLE4( ((float)icuNum / caseListmapMedCond[keyB].size()) * 10.0)  << " %\n";
			cout << "Number of fatalities in your medical condition: " << deaths << endl;
			cout << "Percentage of deaths among chosen medical condition: " <<
				FIXED_DOUBLE4( ((float)deaths / caseListmapMedCond[keyB].size()) * 10.0)  << " %\n";


			break;
		case 6:
			cout << "Input your Race: " << endl;
			cout << "1. Hispanic/Latino" << endl;
			cout << "2. Black; Non-Hispanic" << endl;
			cout << "3. American Indian/Alaska Native; Non-Hispanic" << endl;
			cout << "4. Asian; Non-Hispanic" << endl;
			cout << "5. Native Hawaiian/Other Pacific Islander; Non-Hispanic" << endl;
			cout << "6. White; Non-Hispanic" << endl;
			cout << "7. Multiple/Other" << endl;
			cin >> raceinput;

			//assign a key for the race map
			if (raceinput == 1)
				key1 = "Hispanic/Latino";
			else if (raceinput == 2)
				key1 = "Black; Non-Hispanic";
			else if (raceinput == 3)
				key1 = "American Indian/Alaska Native; Non-Hispanic";
			else if (raceinput == 4)
				key1 = "Asian; Non-Hispanic";
			else if (raceinput == 5)
				key1 = "Native Hawaiian/Other Pacific Islander; Non-Hispanic";
			else if (raceinput == 6)
				key1 = "White; Non-Hispanic";
			else if (raceinput == 7)
				key1 = "Multiple/Other; Non-Hispanic";
			else {
				cout << "Invalid Selection!" << endl;
				break;
			}

			//Adds all cases with specified race to an age map
			for (unsigned int i = 0; i < caseRaceList[key1].size(); i++) {
				temporaryAgeMap[caseRaceList[key1].at(i)->age].push_back(caseRaceList[key1].at(i));
			}

			//Asks user age
			cout << "Input your age" << endl;
			cin >> ageinput;
			if (ageinput <= 9)
				keyC = '0';
			else if (ageinput <= 19)
				keyC = '1';
			else if (ageinput <= 29)
				keyC = '2';
			else if (ageinput <= 39)
				keyC = '3';
			else if (ageinput <= 49)
				keyC = '4';
			else if (ageinput <= 59)
				keyC = '5';
			else if (ageinput <= 69)
				keyC = '6';
			else if (ageinput <= 79)
				keyC = '7';
			else if (ageinput >= 80)
				keyC = '8';

			//Adds all cases with specified age to a sex map
			for (unsigned int i = 0; i < temporaryAgeMap[keyC].size(); i++) {
				temporarySexMap[temporaryAgeMap[keyC].at(i)->sex].push_back(temporaryAgeMap[keyC].at(i));
			}

			//Asks user for their sex
			cout << "Input your Sex: " << endl;
			cout << "1. Male" << endl;
			cout << "2. Female" << endl;
			cin >> sexinput;
			if (sexinput == 1)
				keyB = 1;
			else if (sexinput == 2)
				keyB = 0;
			else {
				cout << "Invalid Selection!" << endl;
				break;
			}

			//Adds all cases with specified sex to a medical condition map
			for (unsigned int i = 0; i < temporarySexMap[keyB].size(); i++) {
				temporaryMedCondMap[temporarySexMap[keyB].at(i)->medicalCondition].push_back(temporarySexMap[keyB].at(i));
			}

			//Asks user for medical condition
			cout << "Do you have a medical condition: " << endl;
			cout << "1. Yes" << endl;
			cout << "2. No" << endl;
			cin >> medcondinput;
			if (medcondinput == 1)
				keyB = 1;
			else if (medcondinput == 2)
				keyB = 0;
			else {
				cout << "Invalid Selection!" << endl;
				break;
			}


			//Counts the total hospitalization, ICU cases and deaths within specied range
			for (unsigned int i = 0; i < temporaryMedCondMap[keyB].size(); i++) {
				if (temporaryMedCondMap[keyB].at(i)->hospitalization == 'Y')
					hosp++;
				if (temporaryMedCondMap[keyB].at(i)->icu == 'Y')
					icuNum++;
				if (temporaryMedCondMap[keyB].at(i)->death == 'Y')
					deaths++;
			}

			//Print data
			cout << "Number of Hospitalizations in your demographic: " << hosp << endl;
			cout << "Percentage of hospitalizations among chosen demographic: " <<
				FIXED_DOUBLE4( ((float)hosp / temporaryMedCondMap[keyB].size()) * 10.0) << " %\n";
			cout << "Number of ICU cases in your demographic: " << icuNum << endl;
			cout << "Percentage of ICU cases among chosen demographic: " <<
				FIXED_DOUBLE4( ((float)icuNum / temporaryMedCondMap[keyB].size()) * 10.0) << " %\n";
			cout << "Number of fatalities in your demographic: " << deaths << endl;
			cout << "Percentage of deaths among chosen demographic: " <<
				FIXED_DOUBLE4( ((float)deaths / temporaryMedCondMap[keyB].size()) * 10.0) << " %\n";
			break;
		case 7:
			cout << "Insert a Month then a Day" << endl;
			cout << "Month: ";
			cin >> input1;
			cout << "Day: ";
			cin >> input2;

			//bad input check
			if (input1 > 12 || input1 < 1 || input2 < 1 || input2 > 31) {
				cout << "Invalid Date!" << endl;
			}
			else {
				start = chrono::system_clock::now();

				//vector of nodes that matches the searched for date
				matchDate = datesTree->Search(input1, input2, datesTree->root);

				//Counts the total hospitalization, ICU cases and deaths within specied range
				for (unsigned int i = 0; i < matchDate.size(); i++) {
					if (matchDate[i]->death == 'Y')
						deaths++;
					if (matchDate[i]->icu == 'Y')
						icuNum++;
					if (matchDate[i]->hospitalization == 'Y')
						hosp++;
				}


				end = chrono::system_clock::now();
				chrono::duration<double> elapsed_seconds = end - start;
				end_time = chrono::system_clock::to_time_t(end);

				//Prints data
				cout << "Using an AVL:" << endl;
				cout << "Number of Hospitalizations on this Date: " << hosp << endl;
				cout << "Percentage of Hospitalizations among chosen date: "
					<< FIXED_DOUBLE4( ((float)hosp / matchDate.size()) * 10.0) << " %\n";
				cout << "Number of ICU cases on this Date: " << icuNum << endl;
				cout << "Percentage of ICU cases among chosen date: "
					<< FIXED_DOUBLE4( ((float)icuNum / matchDate.size()) * 10.0) << " %\n";
				cout << "Number of Deaths on this Date: " << deaths << endl;
				cout << "Percentage of deaths among chosen date: "
					<< FIXED_DOUBLE4( ((float)deaths / matchDate.size()) * 10.0) << " %\n";
				std::cout << "elapsed time for AVL: " << FIXED_DOUBLE6(elapsed_seconds.count()) << "s\n";
				hosp = 0;
				icuNum = 0;
				deaths = 0;


				cout << "\n\n";

				start = chrono::system_clock::now();
				tempKey = hashf(input1, input2);

				//Holds vector of Dates from Node
				matchDate = datesMap[tempKey];

				//Counts the total hospitalization, ICU cases and deaths within specied range
				for (unsigned int i = 0; i < matchDate.size(); i++) {
					if (matchDate.at(i)->hospitalization == 'Y')
						hosp++;
					if (matchDate.at(i)->death == 'Y')
						deaths++;
					if (matchDate.at(i)->icu == 'Y')
						icuNum++;

				}
				end = chrono::system_clock::now();
				elapsed_seconds = end - start;
				end_time = chrono::system_clock::to_time_t(end);

				//Prints data
				cout << "Using an Unordered Map:" << endl;
				cout << "Number of Hospitalizations on this Date: " << hosp << endl;
				cout << "Percentage of Hospitalizations among chosen date: "
					<< FIXED_DOUBLE4( ((float)hosp / matchDate.size()) * 10.0) << " %\n";
				cout << "Number of ICU cases on this Date: " << icuNum << endl;
				cout << "Percentage of ICU cases among chosen date: "
					<< FIXED_DOUBLE4( ((float)icuNum / matchDate.size()) * 10.0) << " %\n";
				cout << "Number of Deaths on this Date: " << deaths << endl;
				cout << "Percentage of deaths among chosen date: "
					<< FIXED_DOUBLE4( ((float)deaths / matchDate.size()) * 10.0) << " %\n";
				std::cout << "elapsed time for Hashmap: " << FIXED_DOUBLE6(elapsed_seconds.count()) << "s\n";
			}
			break;

		case 8:
			loop = false;
			break;
		default:
			cout << "Invalid Selection!" << endl;
			break;
		}
		cout << endl;
		cout << endl;
		cout << endl;
		cout << endl;
		cout << endl;
		cout << endl;
	}
	return 0;
}
