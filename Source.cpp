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

using namespace std;

//Object that holds a case
struct Case {
	//uint8_t month;
	//uint8_t day;

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

	vector<Case*> cases;

	short threshold = 0;
};

class AVL_Tree {
public:
	int size = 0; //Holds the number of cases in the tree


	Node* insert_AVL(Node* root, Node* newNode, Case* newCase);
	Node* root = nullptr;
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

//CDC_Report  N/A   Sex    Age     Race     Hosp     ICU    Death    MedCond
int main() {
	AVL_Tree* test_tree = new AVL_Tree;

	//Map that holds cases bases on age
	unordered_map<char, vector<Case*>> caseAgeList;
	//Map that holds cases bases on race
	unordered_map<string, vector<Case*>> caseRaceList;
	//Map that holds cases bases on sex
	unordered_map<bool, vector<Case*>> caseListmapSex;
	//Map that holds cases bases on age for case 4
	unordered_map<char, vector<Case*>> temporaryAgeMap;
	//Map that holds cases bases on age for case 4
	unordered_map<bool, vector<Case*>> temporarySexMap;

	//creates an ifstream object
	ifstream file;
	// Load the states
	string temp;
	//Gets data for State data
	file.open("cases.csv");
	if (file.is_open()) {
		//Ignores title lines
		getline(file, temp);

		//string date;
		//int counter = 0;

		//Loads data from file into map using commma as delimeter
		while (getline(file, temp, ',')) {
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
			//map thats sorted by race
			caseRaceList[newCase->race].push_back(newCase);
			//map sorted by sex
			caseListmapSex[newCase->sex].push_back(newCase);
			//tree sorted by date
			test_tree->root = test_tree->insert_AVL(test_tree->root, newNode, newCase);
		}
	}

	//cout << "Size of tree: " << test_tree->size << endl;

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

		cout << "Main Menu" << endl;
		//input 2 might not be used as some inputs only require one input
		short input1, input2;

		//Menu option
		int option;

		//menu opptions
		cout << "1. Month and Day" << endl;
		cout << "2. Age" << endl;
		cout << "3. Race" << endl;
		cout << "4. Sex" << endl;
		cout << "5. Race, Age, Sex" << endl;
		cout << "9. Quit" << endl;

		//receiving the input
		cin >> option;

		short ageinput;
		short raceinput;
		short sexinput;
		
		//Temporary boolean key
		bool keyB;

		//Temporary char key
		char keyC;

		string key;
		string key1;
		string key2;
		string key3;
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
			matchDate = test_tree->Search(input1, input2, test_tree->root);
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
			
			//Prints data
			cout << "Number of Hospitalizations on this Date: " << hosp << endl;
			cout << "Percentage of hospitalizations among chosen age range: "
				<< ((float)hosp / matchDate.size()) * 10.0 << fixed << setprecision(3) << " %\n";
			cout << "Number of ICU cases on this Date: " << icuNum << endl;
			cout << "Percentage of ICU cases among chosen age range: "
				<< ((float)icuNum / matchDate.size()) * 10.0 << fixed << setprecision(3) << " %\n";
			cout << "Number of Deaths on this Date: " << deaths << endl;
			cout << "Percentage of deaths among chosen age range: "
				<< ((float)deaths / matchDate.size()) * 10.0 << fixed << setprecision(3) << " %\n";
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
				<< ((float)hosp / caseAgeList[keyC].size()) * 10.0 << fixed << setprecision(3) << " %\n";
			cout << "Number of ICU cases in your age range: " << icuNum << endl;
			cout << "Percentage of ICU cases among chosen age range: "
				<< ((float)icuNum / caseAgeList[keyC].size()) * 10.0 << fixed << setprecision(3) << " %\n";
			cout << "Number of fatalities in your age range: " << deaths << endl;
			cout << "Percentage of deaths among chosen age range: " 
				<< ((float)deaths / caseAgeList[keyC].size()) * 10.0 << fixed << setprecision(3) << " %\n";
			

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
				((float)hosp / caseRaceList[key].size()) * 10.0 << fixed << setprecision(3) << " %\n";
			cout << "Number of ICU cases in your race: " << icuNum << endl;
			cout << "Percentage of ICU cases among chosen race: " <<
				((float)icuNum / caseRaceList[key].size()) * 10.0 << fixed << setprecision(3) << " %\n";
			cout << "Number of fatalities in your race: " << deaths << endl;
			cout << "Percentage of deaths among chosen race: " <<
				((float)deaths / caseRaceList[key].size()) * 10.0 << fixed << setprecision(3) << " %\n";
			
			break;
		case 4:
			//Asks user for age
			cout << "Input your Sex: " << endl;
			cout << "1. Male" << endl;
			cout << "2. Female" << endl;
			cin >> sexinput;
			if (sexinput == 1)
				keyB = 1;
			else if (sexinput == 2)
				keyB = 0;

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
			cout << "Number of Hospitalizations in your race: " << hosp << endl;
			cout << "Percentage of hospitalizations among chosen race: " <<
				((float)hosp / caseListmapSex[keyB].size()) * 10.0 << fixed << setprecision(3) << " %\n";
			cout << "Number of ICU cases in your race: " << icuNum << endl;
			cout << "Percentage of ICU cases among chosen race: " <<
				((float)icuNum / caseListmapSex[keyB].size()) * 10.0 << fixed << setprecision(3) << " %\n";
			cout << "Number of fatalities in your sex: " << deaths << endl;
			cout << "Percentage of deaths among chosen sex: " <<
				((float)deaths / caseListmapSex[keyB].size()) * 10.0 << fixed << setprecision(3) << " %\n";

			break;
		case 5:
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

			//Adds all cases with age
			for (unsigned int i = 0; i < caseRaceList[key1].size(); i++) {
				temporaryAgeMap[caseRaceList[key1].at(i)->age].push_back(caseRaceList[key1].at(i));
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

			//Adds all cases bases on sex
			for (unsigned int i = 0; i < temporaryAgeMap[keyC].size(); i++) {
				temporarySexMap[temporaryAgeMap[keyC].at(i)->sex].push_back(temporaryAgeMap[keyC].at(i));
			}

			//Counts the total hospitalization, ICU cases and deaths within specied range
			for (unsigned int i = 0; i < temporarySexMap[keyB].size(); i++) {
				if (temporarySexMap[keyB].at(i)->hospitalization == 'Y')
					hosp++;
				if (temporarySexMap[keyB].at(i)->icu == 'Y')
					icuNum++;
				if (temporarySexMap[keyB].at(i)->death == 'Y')
					deaths++;
			}

			//Print data
			cout << "Number of Hospitalizations in your race: " << hosp << endl;
			cout << "Percentage of hospitalizations among chosen race: " <<
				((float)hosp / temporarySexMap[keyB].size()) * 10.0 << fixed << setprecision(3) << " %\n";
			cout << "Number of ICU cases in your race: " << icuNum << endl;
			cout << "Percentage of ICU cases among chosen race: " <<
				((float)icuNum / temporarySexMap[keyB].size()) * 10.0 << fixed << setprecision(3) << " %\n";
			cout << "Number of fatalities in your race: " << deaths << endl;
			cout << "Percentage of deaths among chosen race: " <<
				((float)deaths / temporarySexMap[keyB].size()) * 10.0 << fixed << setprecision(3) << " %\n";
			break;
		case 9:
			loop = false;
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
