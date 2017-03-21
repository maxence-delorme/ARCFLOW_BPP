#ifndef MAIN_H 
	#define MAIN_H

	#include <iostream> 
	#include <fstream>
	#include <sstream>
	#include <vector>
	#include <string>
	#include <set>
	#include "time.h"
	#include "instance.h"
	#include "gurobi_c++.h"
	#include <math.h> 

	using namespace std;

	int createArcs(const int& c, const int& m, const vector<int>& w, const vector<int>& b, vector<vector<int> >& arcs, vector<bool>& souvenir);
	int transformSolution(const int& c, vector<vector<vector<int> > >& arcsUsed, vector<vector<int> >& sol);
	int arcflow(const int& c, const int& m, const vector<int>& w, const vector<int>& b, int& of, int& lb, vector<vector<int> >& sol, vector<int>& info);
	
#endif 
