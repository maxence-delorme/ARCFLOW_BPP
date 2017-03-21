#include "instance.h"

int readCSPInstance(int& c, int& m, vector<int>& w, vector<int>& b, const string& filein){

	// Local variables 
	istringstream iss;
	string parser;
	string garbage;

	// File opening
	ifstream file(filein.c_str(), ios::in);

	// File lecture
	if (file){
		// Number of item types
		getline(file, parser);
		iss.str(parser);
		iss >> m; w.resize(m); b.resize(m);
		iss.clear();

		// Capacity
		getline(file, parser);
		iss.str(parser);
		iss >> c;
		iss.clear();

		for (int i = 0; i < m; i++){
			getline(file, parser);
			iss.str(parser);
			iss >> w[i];
			iss >> b[i];
			iss.clear();
		}
		file.close();
		quickSortCSP(w, b, 0, m - 1);
	}
	else{
		return -1;
	}
	return 0;
}

int writeCSPInstance(const int& c, const int& m, const vector<int>& w, const vector<int>& b){
	cout << "Instance read with success: " << endl;
	cout << "m = " << m << endl;
	cout << "c = " << c << endl;
	for (int i = 0; i < m; i++){
		cout << w[i] << " " << b[i] << endl;
	}
	return 0;
}

void quickSortCSP(vector<int>& w, vector<int>& b, const int& first, const int& last){
	int index_pivot = (first + last) / 2;
	int j = last - 1;
	if (first < last){
		exchangeCSP(w[last], w[index_pivot], b[last], b[index_pivot]);
		index_pivot = last;
		for (int i = last - 1; i >= first; i--){
			if (w[i] < w[index_pivot]){
				exchangeCSP(w[i], w[j], b[i], b[j]);
				j = j - 1;
			}
		}
		exchangeCSP(w[index_pivot], w[j + 1], b[index_pivot], b[j + 1]);
		index_pivot = j + 1;
		quickSortCSP(w, b, first, index_pivot - 1);
		quickSortCSP(w, b, index_pivot + 1, last);
	}
}

void exchangeCSP(int& w1, int& w2, int& b1, int& b2) {
	int var;
	var = w1;
	w1 = w2;
	w2 = var;
	var = b1;
	b1 = b2;
	b2 = var;
}

int FFD(const int& c, const int& m, const vector<int>& w, const vector<int>& b, vector<vector<int> >& sol) {
	sol.resize(0);
	vector<int> bins;
	vector<int> empty;
	for (int i = 0; i < m; i++){
		for (int j = 0; j < b[i]; j++){
			bool packed = false;
			for (int k = 0; k < bins.size(); k++){
				if (c - bins[k] >= w[i]){
					packed = true;
					sol[k].push_back(i);
					bins[k] += w[i];
					break;
				}
			}
			if (!packed){
				bins.push_back(w[i]);
				sol.push_back(empty);
				sol.back().push_back(i);
			}
		}
	}
	return bins.size();
}

int writeAndCheckSolution(const int& c, const int& m, const vector<int>& w, const vector<int>& b, const int& of, const int& lb, const vector<vector<int> >& sol, const string& fileInstance, const string& fileMethod, const vector<int>& info){
	
	int errorValue = 0;
	
	if (sol.size() != of){
		cout << "Error 1 - Solution has a size different from the objective value" << endl;
		errorValue = 1;
	}

	vector<int> demand(m,0);
	vector<int> binWeight(of, 0);

	for (int i = 0; i < sol.size(); i++){
		for (int j = 0; j < sol[i].size(); j++){
			binWeight[i] += w[sol[i][j]];
			demand[sol[i][j]]++;
		}
		if (binWeight[i] > c){
			cout << "Error 2 - Solution has an overcapacitated bin" << endl;
			errorValue = 2;
		}
	}

	for (int i = 0; i < m; i++){
		if (demand[i] < b[i]){
			cout << "Error 2 - Solution has at least a missing item" << endl;
			errorValue = 3;
		}
	}

	// Writting in a file the method results
	ofstream fileMeth(fileMethod.c_str(), ios::out | ios::app);
	fileMeth << fileInstance << "\t" << lb << "\t" << of << "\t";
	for (int i = 0; i<info.size(); i++){
		fileMeth << info[i] << "\t";
	}
	fileMeth << errorValue << "\n";
	fileMeth.close();

	// Writting in a file the instance results
	if (errorValue == 0){
		ofstream fileInst(fileInstance.c_str(), ios::out | ios::trunc);
		fileInst << "Solution: " << endl;
		for (int i = 0; i < sol.size(); i++){
			fileInst << "Bin " << i << ": " << binWeight[i] << " - ";
			for (int j = 0; j < sol[i].size(); j++){
				fileInst << sol[i][j] << " ";
			}
			fileInst << "\n";
		}
		fileInst.close();
	}
	return 0;
}