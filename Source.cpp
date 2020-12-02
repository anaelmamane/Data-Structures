#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <unordered_map>

using namespace std;
struct Case {
	string dateCDC; //Holds the data the CDC report data
	int month; //Holds the data the CDC report data
	string sex; //Holds the sex of the person
	string age; //Holds the age range of the person
	string race; //Holds the race person
	string hospitalization; //Hospitalization
	string icu; //ICU admittance
	string death; //Did the person die
	string medicalCondition; //Did the person have a medical condition
	Case* right = nullptr;
	Case* left = nullptr;
	int threshold;
};
class AVL_Tree {
public:
	Case* insert_AVL(Case* root, Case* newcase);
	Case* root = nullptr;
	int calculateThreshold(Case* case_);
	int height(Case* case_);
	Case* rotateRight(Case* case_);
	Case* rotateLeft(Case* case_);
};
Case* AVL_Tree::rotateLeft(Case* case_) {
	Case* newparent = case_->right;
	Case* temp = newparent->left;
	newparent->left = case_;
	case_->right = temp;
	return newparent;
}

Case* AVL_Tree::rotateRight(Case* newcase) {
	Case* newparent = newcase->left;
	Case* temp = newparent->right;
	newparent->right = newcase;
	newcase->left = temp;
	return newparent;
}
int AVL_Tree::height(Case* root) {
	if (root == nullptr)
		return 0;
	else
		return 1 + std::max(height(root->left), height(root->right));
}
int AVL_Tree::calculateThreshold(Case* case_) {
	int leftheight = height(case_->left);
	int rightheight = height(case_->right);
	case_->threshold = leftheight - rightheight;
	return leftheight - rightheight;
}

Case* AVL_Tree::insert_AVL(Case* root, Case* newcase) {
	if (root == nullptr) {
		return newcase;
	}
	else if (newcase->dateCDC < root->dateCDC) {
		root->left = insert_AVL(root->left, newcase);
		calculateThreshold(root);
	}
	else {
		root->right = insert_AVL(root->right, newcase);
		calculateThreshold(root);

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
//CDC_Report  N/A   Sex    Age     Race     Hosp     ICU    Death    MedCond
int main() {
	AVL_Tree* test_tree = new AVL_Tree;

	//Map that holds cases
	unordered_map<string, vector<Case*>> caseListmap;
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
		string datecdc;
		int counter = 0;
		//Loads data from file into map using commma as delimeter
		while (getline(file, datecdc, ',')) {
			Case* temp = new Case;
			temp->dateCDC = datecdc;
			getline(file, name, ',');
			getline(file, temp->sex, ',');
			getline(file, temp->age, ',');
			getline(file, temp->race, ',');
			getline(file, temp->hospitalization, ',');
			getline(file, temp->icu, ',');
			getline(file, temp->death, ',');
			getline(file, temp->medicalCondition, '\n');
			//Places data into a map
			char tempchar = temp->dateCDC.at(0);
			temp->month = int(tempchar);
			caseListmap[temp->age].push_back(temp);
			test_tree->root = test_tree->insert_AVL(test_tree->root, temp);
		}
	}
}
