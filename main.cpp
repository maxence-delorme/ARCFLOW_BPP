#include "main.h"


/*	*************************************************************************************
	*************************************  MAIN *****************************************
	************************************************************************************* */

double EPSILON = 0.00001;
int deb = GetTimeMs64();

int main(int argc, char **argv){

	// Local variables
	int c, m;					// capacity, number of items
	vector<int> w, b, info;		// weights, demands, informations
	int of = 999999; int lb = 0;// ojective function, lower bound
	vector<vector<int> > sol;	// solution

	// Init
	info.resize(7);				// info contains 7 values: total time, time for arcs, nbArcs, nbSouvenirs, nbVariables, nbConstraints, nbOccurencies

	readCSPInstance(c, m, w, b, argv[1]);
	writeCSPInstance(c, m, w, b);
	of = FFD(c, m, w, b, sol);
	arcflow(c, m, w, b, of, lb, sol, info);
	writeAndCheckSolution(c, m, w, b, of, lb, sol, argv[2], argv[3], info);
}

/*	*************************************************************************************
	**********************************	FUNCTIONS  **************************************
	************************************************************************************* */

int arcflow(const int& c, const int& m, const vector<int>& w, const vector<int>& b, int& of, int& lb, vector<vector<int> >& sol, vector<int>& info) {

	// Local variables

	vector<vector<int> > arcs;
	vector<bool> souvenir;

	// Arc definition
	createArcs(c, m, w, b, arcs, souvenir);

	// Fill info
	info[1] = GetTimeMs64() - deb;
	info[2] = arcs.size();
	info[3] = 0;
	for (int i = 0; i<c; i++){
		if (souvenir[i] == true){
			info[3]++;
		}
	}

	if (arcs.size() > 2000000){
		info[0] = GetTimeMs64() - deb;
		return -1;
	}

	// Model
	GRBEnv env = GRBEnv();
	try{
		GRBModel model = GRBModel(env);

		// Declaration of the variables for the model
		vector<GRBVar> isArcUsed(arcs.size());
		vector<GRBLinExpr>	cIn(c + 1);
		vector<GRBLinExpr>  cOut(c + 1);
		vector<GRBLinExpr>  objT(m);

		// Initialization
		for (int i = 0; i < arcs.size(); i++){
			if (arcs[i][2] >= 0){
				isArcUsed[i] = model.addVar(0, GRB_INFINITY, 0, GRB_INTEGER);
			}
			else {
				isArcUsed[i] = model.addVar(0, GRB_INFINITY, 0, GRB_INTEGER);
			}
		}
		for (int i = 0; i < c + 1; i++){
			cIn[i] = 0;
			cOut[i] = 0;
		}
		for (int i = 0; i < m; i++){
			objT[i] = 0;
		}

		model.update();

		// Constraints
		for (int i = 0; i < arcs.size(); i++){
			cIn[arcs[i][1]] += isArcUsed[i];
			cOut[arcs[i][0]] += isArcUsed[i];
			if (arcs[i][2] >= 0) {
				objT[arcs[i][2]] += isArcUsed[i];
			}
		}

		// Objective function
		model.setObjective(cOut[0], GRB_MINIMIZE);

		// Flow conservation
		for (int i = 1; i < c; i++){
			if (souvenir[i] == true){
				model.addConstr(cIn[i] == cOut[i]);
			}
		}
		model.addConstr(cOut[0] == cIn[c]);

		// Take the objects
		for (int i = 0; i < m; i++){
			if (b[i] == 1){
				model.addConstr(objT[i] >= b[i]);
			}
			else {
				model.addConstr(objT[i] >= b[i]);
			}
		}

		// Setting of cplex

		model.getEnv().set(GRB_DoubleParam_TimeLimit, 3600 - (GetTimeMs64() - deb) / 1000);

		model.getEnv().set(GRB_DoubleParam_MIPGap, 0);
		model.getEnv().set(GRB_DoubleParam_MIPGapAbs, 1 - EPSILON);

		model.getEnv().set(GRB_IntParam_Threads, 1);
		model.getEnv().set(GRB_IntParam_Method, 2);
	/*	model.getEnv().set(GRB_IntParam_Presolve, 1);

		model.getEnv().set(GRB_DoubleParam_Heuristics, 1);
		model.getEnv().set(GRB_IntParam_MIPFocus, 1);*/

		model.optimize();

		// Fill info
		info[4] = model.get(GRB_IntAttr_NumVars);
		info[5] = model.get(GRB_IntAttr_NumConstrs);
		info[6] = model.get(GRB_IntAttr_NumNZs);

		// Solving the model (or get some information)
		if (model.get(GRB_IntAttr_SolCount) < 1){
			cout << "Failed to optimize LP. " << endl;
			// get info
			info[0] = GetTimeMs64() - deb;
			lb = ceil(model.get(GRB_DoubleAttr_ObjBound) - EPSILON);
			return -1;
		}

		// Get solution
		vector<vector<vector<int> > > arcsUsed(c + 1);
		for (int i = 0; i < arcs.size(); i++){
			for (int j = 0; j < ceil(isArcUsed[i].get(GRB_DoubleAttr_X) - EPSILON); j++){
				arcsUsed[arcs[i][0]].push_back(arcs[i]);
			}
		}

		// Fill info
		info[0] = GetTimeMs64() - deb;
		if (of >= ceil(model.get(GRB_DoubleAttr_ObjVal) - EPSILON)){
			of = ceil(model.get(GRB_DoubleAttr_ObjVal) - EPSILON);
			transformSolution(c, arcsUsed, sol);
		}
		lb = ceil(model.get(GRB_DoubleAttr_ObjBound) - EPSILON);
	}
	
	// Exceptions
	catch (GRBException e) {
		cout << "Error code = " << e.getErrorCode() << endl;
		info[0] = GetTimeMs64() - deb;
		cout << e.getMessage() << endl;
	}
	catch (...) {
		cout << "Exception during optimization" << endl;
		info[0] = GetTimeMs64() - deb;
	}

	// End
	return 0;
}

int createArcs(const int& c, const int& m, const vector<int>& w, const vector<int>& b, vector<vector<int> >& arcs, vector<bool>& souvenir){
	
	// Local variables
	souvenir.resize(c + 1, false);
	souvenir[0] = true;
	vector<int> vec(3);
	set<vector<int> > setArcs;

	// Arcs for items		
	for (int i = 0; i < m; i++){
		for (int j = c - w[i]; j >= 0; j--){
			if (souvenir[j] == true){
				for (int k = 1; k <= b[i]; k++){
					if (j + k *w[i] > c){
						break;
					}
					souvenir[j + k*w[i]] = true;
					vec[0] = j + (k - 1)*w[i]; 
					vec[1] = j + k		*w[i]; 
					vec[2] = i;
					setArcs.insert(vec);
				}
			}
		}
	}

	// Loss Arcs
	vector<int> activeN;
	for (int i = 1; i <= c; i++){
		if (souvenir[i] == true){
			activeN.push_back(i);
		}
	}

	for (int i = 0; i < activeN.size() - 1; i++){
		vec[0] = activeN[i]; vec[1] = activeN[i + 1]; vec[2] = -1;
		setArcs.insert(vec);
	}

	for (set<vector<int> >::iterator it = setArcs.begin(); it != setArcs.end(); ++it) {
		arcs.push_back(*it);
	}

	return 0;
}

int transformSolution(const int& c, vector<vector<vector<int> > >& arcsUsed, vector<vector<int> >& sol){
	sol.resize(0);
	while (arcsUsed[0].size() > 0){
		vector<int> curBin;
		int thisTail = 0;
		for (;;){
		
			int nextTail = arcsUsed[thisTail].back()[1];

			// If the next arc is an item
			if (arcsUsed[thisTail].back()[2] >= 0){
				curBin.push_back(arcsUsed[thisTail].back()[2]);
			}

			arcsUsed[thisTail].pop_back();
			thisTail = nextTail;

			if (thisTail == c){
				break;
			}
		}
		sol.push_back(curBin);
	}
	return 0;
}