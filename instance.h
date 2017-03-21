#ifndef INSTANCE_H 
	#define INSTANCE_H
	
	#include <iostream> 
	#include <fstream>
	#include <sstream>
	#include <vector>
	#include <string>

	using namespace std;

	int readCSPInstance(int& c, int& m, vector<int>& w, vector<int>& b, const string& filein);
	int writeCSPInstance(const int& c, const int& m, const vector<int>& w, const vector<int>& b);
	void quickSortCSP(vector<int>& w, vector<int>& b, const int& first, const int& last);
	void exchangeCSP(int& w1, int& w2, int& b1, int& b2);
	int writeAndCheckSolution(const int& c, const int& m, const vector<int>& w, const vector<int>& b, const int& of, const int& lb, const vector<vector<int> >& sol, const string& fileInstance, const string& fileMethod, const vector<int>& info);
	int FFD(const int& c, const int& m, const vector<int>& w, const vector<int>& b, vector<vector<int> >& sol);
	
	#endif 
