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

	//string dateCDC; //Holds the data the CDC report data D/MM/YYYY
	string sex; //Holds the sex of the person
	string age; //Holds the age range of the person
	string race; //Holds the race person
	string hospitalization; //Hospitalization
	string icu; //ICU admittance
	string death; //Did the person die
	string medicalCondition; //Did the person have a medical condition
	//Case* right = nullptr; //Case date that was later
	//Case* left = nullptr; //Case date that was earlier
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

	int threshold = 0;
};

class AVL_Tree {
public:
	int size = 0; //Holds the number of cases in the tree


	Node* insert_AVL(Node* root, Node* newNode, Case* newCase);
	Node* root = nullptr;
	int calculateThreshold(Node* newNode);
	int height(Node* newNode);
	Node* rotateRight(Node* newNode);
	Node* rotateLeft(Node* newNode);
	//Search for a case
	vector<Case*> Search(int month, int date, Node* root);

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
int AVL_Tree::height(Node* root) {

	if (root == nullptr)
		return 0;
	else
		return 1 + std::max(height(root->left), height(root->right));
}

//Calculates threshold of a node
int AVL_Tree::calculateThreshold(Node* newNode) {
	int leftheight = height(newNode->left);
	int rightheight = height(newNode->right);
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

vector<Case*> AVL_Tree::Search(int month, int date, Node* root) {
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

	//Map that holds cases
	unordered_map<string, vector<Case*>> caseListmap;
	unordered_map<string, vector<Case*>> caseListmapRace;
	//Temporary Case object
	//creates an ifstream object
	ifstream file;
	// Load the states
	string name;
	//Gets data for State data
	file.open("cases.csv");
	if (file.is_open()) {
		//Ignores title lines
		getline(file, name);
		string date;
		int counter = 0;

		//Loads data from file into map using commma as delimeter
		while (getline(file, date, ',')) {
			//Creates node for tree using the Node object
			Node* newNode = new Node;
			//Uses
			Case* newCase = new Case;
			//temp->dateCDC = date;

			//   Date format   ##/##/#### saved in datecdc
			//Grabs month and saves it as a 8 bit unsigned int
			newNode->month = (short)stoi(date.substr(0, date.find('/')));

			//Grabs day and saves it as a 8 bit unsigned int
			newNode->day = (short)stoi(date.substr(date.find('/') + 1, date.size() - 7));


			getline(file, name, ',');
			getline(file, newCase->sex, ',');
			getline(file, newCase->age, ',');
			getline(file, newCase->race, ',');
			getline(file, newCase->hospitalization, ',');
			getline(file, newCase->icu, ',');
			getline(file, newCase->death, ',');
			getline(file, newCase->medicalCondition, '\n');

			//Places data into a map
			//map sorted by age
			caseListmap[newCase->age].push_back(newCase);
			//map that sorted by race
			caseListmapRace[newCase->race].push_back(newCase);
			//tree sorted by date
			test_tree->root = test_tree->insert_AVL(test_tree->root, newNode, newCase);
		}
	}

	cout << "Size of tree: " << test_tree->size << endl;

	//main menu
	//bool that runs the while loop
	bool loop = true;

	while (loop) {
		//vector that holds the cases that match the date. Had to declare here for some reason???
		vector<Case*> matchDate;
		//int for deaths and hospitalizations
		int deaths = 0, hosp = 0;

		cout << "Main Menu" << endl;
		//input 2 might not be used as some inputs only require one input
		short input1, input2;

		//Menu option
		int option;

		//menu opptions
		cout << "1. Month and Day" << endl;
		cout << "2. Age" << endl;
		cout << "3. Race" << endl;
		cout << "9. Quit" << endl;

		//receiving the input
		cin >> option;

		int ageinput;
		int raceinput;
		string key;
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
			//count num of deaths and num of hospitalizations - ADD TO LATER IF WANT
			for (int i = 0; i < matchDate.size(); i++) {
				if (matchDate[i]->death == "Yes") {
					deaths++;
				}
				if (matchDate[i]->hospitalization == "Yes") {
					hosp++;
				}
			}
			//cout << "root: " << test_tree->root->month << " " << test_tree->root->day << endl;
			//print stats
			cout << "Number of Deaths on this Date: " << deaths << endl;
			cout << "Number of Hospitalizations on this Date: " << hosp << endl;

			break;
		case 2:
			cout << "Input your age" << endl;
			cin >> ageinput;
			if (ageinput <= 9)
				key = "0 - 9 Years";
			else if (ageinput <= 19)
				key = "10 - 19 Years";
			else if (ageinput <= 29)
				key = "20 - 29 Years";
			else if (ageinput <= 39)
				key = "30 - 39 Years";
			else if (ageinput <= 49)
				key = "40 - 49 Years";
			else if (ageinput <= 59)
				key = "50 - 59 Years";
			else if (ageinput <= 69)
				key = "60 - 69 Years";
			else if (ageinput <= 79)
				key = "70 - 79 Years";
			else if (ageinput >= 80)
				key = "80+ Years";


			for (int i = 0;i < caseListmap[key].size();i++) {
				if (caseListmap[key].at(i)->hospitalization == "Yes")
					hosp++;
				if (caseListmap[key].at(i)->death == "Yes")
					deaths++;
			}
			cout << "Number of fatalities in your age range: " << deaths << endl;
			cout << "Number of Hospitalizations in your age range: " << hosp << endl;

			break;
		case 3:
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
			
			for (int i = 0;i < caseListmapRace[key].size();i++) {
				if (caseListmapRace[key].at(i)->hospitalization == "Yes")
					hosp++;
				if (caseListmapRace[key].at(i)->death == "Yes")
					deaths++;
			}
			cout << "Number of fatalities in your race: " << deaths << endl;
			cout << "Number of Hospitalizations in your race: " << hosp << endl;
			
			/*case 4:
				break;
			case 5:
				break;
			case 6:
				break;
			case 7:
				break;
			case 8:
				break;*/
		case 9:
			loop = false;
			break;
		}
	}
	return 0;
}
