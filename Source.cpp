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
	uint8_t day = 0;
	uint8_t month = 0;

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
		calculateThreshold(root);
	}
	//If Case data is greater than current root, go right
	else if (newNode->month > root->month) { //month >
		root->right = insert_AVL(root->right, newNode, newCase);
		calculateThreshold(root);
	}
	else { //If months equal
		if (newNode->day < root->day) {
			root->left = insert_AVL(root->left, newNode, newCase);
			calculateThreshold(root);
		}
		else if (newNode->day > root->day) {
			root->right = insert_AVL(root->right, newNode, newCase);
			calculateThreshold(root);
		}
		//Both day and month equal
		else {
			root->cases.push_back(newCase);
			size++;
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
		found = Search(month, date, root->left);
	}
	//month greater than
	else if (root->month > month) {
		found = Search(month, date, root->right);
	}
	//equal month
	else {
		//day less than
		if (root->day < date) {
			found =	Search(month, date, root->left);
		}
		//day greater than
		else if (root->day > date) {
			found = Search(month, date, root->right);
		}
		//if everything is equal
		else {
			found = root->cases;
		}
	}
	return found;
}

//CDC_Report  N/A   Sex    Age     Race     Hosp     ICU    Death    MedCond
int main() {
	AVL_Tree* test_tree = new AVL_Tree;

	//Map that holds cases
	//unordered_map<string, vector<Case*>> caseListmap;
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
			Node* rootNode = new Node;
			//Uses
			Case* temp = new Case;
			//temp->dateCDC = date;

			//   Date format   ##/##/#### saved in datecdc
			//Grabs month and saves it as a 8 bit unsigned int
			rootNode->month = (uint8_t)stoi(date.substr(0, date.find('/')));

			//Grabs day and saves it as a 8 bit unsigned int
			rootNode->day = (uint8_t)stoi(date.substr(date.find('/') + 1, date.size() - 7));


			getline(file, name, ',');
			getline(file, temp->sex, ',');
			getline(file, temp->age, ',');
			getline(file, temp->race, ',');
			getline(file, temp->hospitalization, ',');
			getline(file, temp->icu, ',');
			getline(file, temp->death, ',');
			getline(file, temp->medicalCondition, '\n');



			//Places data into a map
			//char tempchar = temp->dateCDC.at(0);
			//temp->month = int(tempchar);
			//caseListmap[temp->age].push_back(temp);
			test_tree->root = test_tree->insert_AVL(test_tree->root, rootNode, temp);
		}
	}

	//main menu
	//bool that runs the while loop
	bool loop = true;
	//vector that holds the cases that match the date. Had to declare here for some reason???
	vector<Case*> matchDate;
	//int for deaths and hospitalizations
	int deaths = 0, hosp = 0;
	while (loop) {
		cout << "Main Menu" << endl;
		//input 2 might not be used as some inputs only require one input
		int input1, input2;

		//Menu option
		int option;

		//1. month and date
		cout << "1. Month and Date" << endl;

		//2, 3, 4, 5, etc

		//ending the loop & program
		cout << "9. Quit" << endl;

		//receiving the input
		cin >> option;

		//make a switch statement that checks option
		switch (option) {
		case 1:
			cout << "Insert a Month then a Date" << endl;
			cout << "Month: ";
			cin >> input1;
			cout << "Date: ";
			cin >> input2;

			//vector of nodes that matches the searched for date
			matchDate = test_tree->Search(input1, input2, test_tree->root);
			//count num of deaths and num of hospitalizations - ADD TO LATER IF WANT
			for (int i = 0; i < matchDate.size(); i++) {
				if (matchDate[i]->death == "Yes") {
					deaths++;
				}
				if (matchDate[i]->hospitalization == "Yes") {
					hosp++;
				}
			}
			//cout << (int)test_tree->root->month << " " << (int)test_tree->root->day;
			//print stats
			cout << "Number of Deaths on this Date: " << deaths << endl;
			cout << "Number of Hospitalizations on this Date: " << hosp << endl;

			//ready for next loop
			deaths = 0;
			hosp = 0;

			break;
		/*case 2:
			break;
		case 3:
			break;
		case 4:
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
