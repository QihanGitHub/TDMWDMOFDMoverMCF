//This file defines class VONECplex
//------------------------------------------------
//File Name: ILP.cpp
//Author: Qihan Zhang
//Email: lengkudaodi@outlook.com
//Last Modified: Jun. 30th 2021
//------------------------------------------------

#include "ILP.h"

#include <numeric>
#include <string>
#include <iomanip>
#include <algorithm>

namespace
{
	void getILPVertexInputParameter
	(const VONETopo::Topo& Substrate_Network,
		const vector<VONETopo::Topo>& Virtual_Network_List,
		int& Request_Number,
		int& Substrate_Network_Vertex_Number,
		vector<int>& Virtual_Networks_Vertex_Number,
		vector<int>& Accumulate_Virtual_Networks_Vertex_Number,
		vector<vector<int> >& Computing_Capacity_in_Virtual_Vertexes);
	//Precondition: The topology Substrate_Network and topology vector Virtual_Network_List have been assigned, and other
	//    parameters have been defined.
	//Postcondition: The information from the topologys will be assigned to Request_Number, Substrate_Network_Vertex_Number, 
	//    Virtual_Networks_Vertex_Number, Accumulate_Virtual_Networks_Vertex_Number and Computing_Capacity_in_Virtual_Vertexes.

	void constraints1
	(IloBoolVarArray x,
		IloRangeArray c,
		const int Request_Number,
		const int Substrate_Network_Vertex_Number,
		const vector<int> Virtual_Networks_Vertex_Number,
		const vector<int> Accumulate_Virtual_Networks_Vertex_Number,
		const vector<vector<int> > Computing_Capacity_in_Virtual_Vertexes);
	//Precondition: The function getILPVertexInputParameter has been excuted, so Request_Number, Substrate_Network_Vertex_Number,
	//    Virtual_Networks_Vertex_Number Accumulate_Virtual_Networks_Vertex_Number, Computing_Capacity_in_Virtual_Vertexes were assigned.
	//Postcondition: The constraint of that assigning request source and destination has been added into constraint c. 
	//    And new variables of x will be added.

	void getILPEdgeInputParameter
	(const VONETopo::Topo& Substrate_Network,
		const vector<VONETopo::Topo>& Virtual_Network_List,
		VONETopo::LinkList& Substrate_Network_Edge,
		int& Substrate_Network_Edge_Number,
		int& Min_Substrate_Network_Bandwidth,
		vector<VONETopo::LinkList>& Virtual_Networks_Edge,
		vector<int>& Virtual_Networks_Edge_Number,
		vector<int>& Accumulate_Virtual_Networks_Edge_Number,
		vector<int>& Virtual_Networks_Bandwidth,
		vector<int>& Accumulate_Virtual_Networks_Bandwidth,
		vector<int>& Accumulate_Virtual_Networks_Edge_Number_and_Bandwidth);
	//Precondition: The topology Substrate_Network and topology vector Virtual_Network_List have been assigned, and other
	//    parameters have been defined.
	//Postcondition: The information from the topologys will be assigned to Substrate_Network_Edge, Substrate_Network_Edge_Number, 
	//    Virtual_Networks_Edge, Virtual_Networks_Edge_Number, Accumulate_Virtual_Networks_Edge_Number, Virtual_Networks_Bandwidth 
	//    and Accumulate_Virtual_Networks_Bandwidth.

	void constraints2
	(IloBoolVarArray x,
		IloRangeArray c,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const VONETopo::LinkList& Substrate_Network_Edge);
	//Precondition: The function getILPVertexInputParameter and getILPEdgeInputParameter have been excuted, so Request_Number, 
	//    Substrate_Network_Edge_Number, Virtual_Networks_Edge_Number, Accumulate_Virtual_Networks_Edge_Number and Substrate_Network_Edge
	//    were assigned. 
	//Postcondition: The constraint of that the link from s to d and d to s should be disjoint has been added to the constraint c.
	//    And new variables of x will be added.

	void constraints3
	(IloRangeArray c,
		const IloBoolVarArray x,
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const int Substrate_Network_Vertex_Number,
		const vector<int> Accumulate_Virtual_Networks_Vertex_Number,
		const VONETopo::LinkList& Substrate_Network_Edge,
		const vector<VONETopo::LinkList>& Virtual_Networks_Edge);
	//Precondition: The function getILPVertexInputParameter and getILPEdgeInputParameter have been excuted, so Request_Number, 
	//    Substrate_Network_Edge_Number, Virtual_Networks_Edge_Number, Accumulate_Virtual_Networks_Edge_Number, Substrate_Network_Vertex_Number, 
	//    Accumulate_Virtual_Networks_Vertex_Number, Substrate_Network_Edge and Virtual_Networks_Edge were assigned. 
	//    X and y must be added into the constraint c.
	//Postcondition: The constraint of that the flow conservation of each request when it is embedded into the substrate has been 
	//    added to the constraint c.

	void constraints4
	(IloBoolVarArray x,
		IloRangeArray con,
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const VONETopo::LinkList& Substrate_Network_Edge,
		const int C);
	//Precondition: The function getILPVertexInputParameter and getILPEdgeInputParameter have been excuted, so Request_Number, 
	//    Substrate_Network_Edge_Number, Virtual_Networks_Edge_Number, Accumulate_Virtual_Networks_Edge_Number, 
	//    Substrate_Network_Edge and C were assigned. And y must be added into the constraint con.
	//Postcondition: The constraint of that only one core can be selected when the link is determined has been added to the 
	//    constraint con. And new variables of x will be added. 

	void constraints5
	(IloIntVarArray x,
		IloModel mod,
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const VONETopo::LinkList& Substrate_Network_Edge,
		const vector<int> Virtual_Networks_Bandwidth);
	//Precondition: The function getILPVertexInputParameter and getILPEdgeInputParameter have been excuted, so Request_Number, 
    //    Substrate_Network_Edge_Number, Virtual_Networks_Edge_Number, Accumulate_Virtual_Networks_Edge_Number, 
    //    Substrate_Network_Edge and Virtual_Networks_Bandwidth were assigned. And y must be added into the problem model mod.
    //Postcondition: The constraint of wavelength start index or frequency slot start has been added to the 
    //    problem model mod. And new variables of x will be added. 

	void constraints6
	(IloIntVarArray x,
		IloModel mod,
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const VONETopo::LinkList& Substrate_Network_Edge,
		const vector<int> Virtual_Networks_Bandwidth,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number_and_Bandwidth);
	//Precondition: The function getILPVertexInputParameter and getILPEdgeInputParameter have been excuted, so Request_Number, 
    //    Substrate_Network_Edge_Number, Virtual_Networks_Edge_Number, Accumulate_Virtual_Networks_Edge_Number, 
    //    Substrate_Network_Edge, Virtual_Networks_Bandwidth and Accumulate_Virtual_Networks_Bandwidth were assigned. 
	//    And y must be added into the problem model mod.
    //Postcondition: The constraint of time slot start index has been added to the problem model mod. And new variables of x will be added. 

	void constraints7
	(IloIntVarArray x,
		IloRangeArray con,
		const IloIntVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const VONETopo::LinkList& Substrate_Network_Edge,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number_and_Bandwidth);
	//Precondition: The function getILPVertexInputParameter and getILPEdgeInputParameter have been excuted, so Request_Number, 
	//    Substrate_Network_Edge_Number, Virtual_Networks_Edge_Number, Accumulate_Virtual_Networks_Edge_Number, 
	//    Substrate_Network_Edge and Virtual_Networks_Bandwidth were assigned. 
	//    And y must be added into the constraint con.
	//Postcondition: The constraint of wavelength end index or frequency slot end index named spectrum continuity has been added to the constraint con. 
	//    And new variables of x will be added.

	void constraints8
	(IloModel mod,
		const IloBoolVarArray x,
		const IloIntVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const vector<int> Virtual_Networks_Bandwidth,
		const int C);
	//Precondition: The function getILPVertexInputParameter and getILPEdgeInputParameter have been excuted, so Request_Number, 
	//    Substrate_Network_Edge_Number, Virtual_Networks_Edge_Number, Accumulate_Virtual_Networks_Edge_Number, 
	//    Virtual_Networks_Bandwidth and C were assigned. X and y must be added into the problem model mod.
	//Postcondition: The constraint of spectrum no overlap for WDM or OFDM has been added to the problem model mod.
	
	void constraints9
	(IloModel mod,
		const IloBoolVarArray x,
		const IloIntVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Virtual_Networks_Bandwidth,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number_and_Bandwidth,
		const int C);
	//Precondition: The function getILPVertexInputParameter and getILPEdgeInputParameter have been excuted, so Request_Number, 
    //    Substrate_Network_Edge_Number, Virtual_Networks_Edge_Number, Virtual_Networks_Bandwidth, Accumulate_Virtual_Networks_Bandwidth
	//    and C were assigned. X and y must be added into the problem mopdel mod.
    //Postcondition: The constraint of spectrum no overlap for TDM has been added to the problem model mod.

	void constraints10
	(IloModel mod,
		const IloIntVarArray x,
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number);
	//Precondition: The function getILPVertexInputParameter and getILPEdgeInputParameter have been excuted, so Request_Number, 
    //    Substrate_Network_Edge_Number, Virtual_Networks_Edge_Number and Accumulate_Virtual_Networks_Edge_Number
    //    were assigned. X and y must be added into the problem mopdel mod.
    //Postcondition: The constraint of spectrum consistency for WDM or OFDM has been added to the problem model mod.

	void constraints11
	(IloModel mod,
		const IloIntVarArray x,
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const vector<int> Virtual_Networks_Bandwidth,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number_and_Bandwidth);
	//Precondition: The function getILPVertexInputParameter and getILPEdgeInputParameter have been excuted, so Request_Number, 
	//    Substrate_Network_Edge_Number, Virtual_Networks_Edge_Number, Accumulate_Virtual_Networks_Edge_Number, Virtual_Networks_Bandwidth 
	//    and Accumulate_Virtual_Networks_Bandwidth were assigned. X and y must be added into the problem mopdel mod.
	//Postcondition: The constraint of spectrum consistency for TDM has been added to the problem model mod.

	void constraints12
	(IloModel mod,
		const IloIntVarArray x,
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const vector<int> Virtual_Networks_Bandwidth,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number_and_Bandwidth);
	//Precondition: The function getILPVertexInputParameter and getILPEdgeInputParameter have been excuted, so Request_Number, 
    //    Substrate_Network_Edge_Number, Virtual_Networks_Edge_Number, Accumulate_Virtual_Networks_Edge_Number, Virtual_Networks_Bandwidth 
    //    and Accumulate_Virtual_Networks_Bandwidth were assigned. X and y must be added into the problem mopdel mod.
    //Postcondition: The constraint of time slot order maintain for TDM has been added to the problem model mod.

	void constraints13
	(IloIntVarArray x,
		IloRangeArray con,
		const int Request_Number,
		const int Min_Substrate_Network_Bandwidth,
		const vector<int> Virtual_Networks_Bandwidth,
		const vector<int> Accumulate_Virtual_Networks_Bandwidth);
	//Precondition: The function getILPVertexInputParameter and getILPEdgeInputParameter have been excuted, so Request_Number, 
    //    Substrate_Network_Edge_Number, Virtual_Networks_Edge_Number and Accumulate_Virtual_Networks_Edge_Number were assigned.
    //Postcondition: The constraint of time slot start index for TDM has been added to the constraint con. 
	//    And new variables of x will be added. This constraint is another method to make sure the spectrum consistency.

	void constraints14
	(IloModel mod,
		const IloIntVarArray x,
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Vertex_Number,
		const vector<int> Virtual_Networks_Vertex_Number,
		const vector<int> Accumulate_Virtual_Networks_Vertex_Number,
		const vector<vector<int> > Computing_Capacity_in_Virtual_Vertexes,
		const vector<int> Virtual_Networks_Bandwidth,
		const vector<int> Accumulate_Virtual_Networks_Bandwidth);

	void constraints15
	(IloModel mod,
		const IloBoolVarArray x,
		const IloIntVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Bandwidth,
		const vector<int> Accumulate_Virtual_Networks_Bandwidth,
		const int C);
}

namespace VONEILP
{
	VONECplex::VONECplex()
	{
		_model = IloModel(_env);
		_variables = IloIntVarArray(_env);
		_constraints = IloRangeArray(_env);
		_cplex = IloCplex(_env);
	}

	void VONECplex::solve(const VONETopo::Topo& Substrate_Network,
		const vector<VONETopo::Topo>& Virtual_Network_List, int TypeFlag, int CoreNum)
	{
		int RNum;
		int SVNum;
		vector<int> VVNum;
		vector<int> AVVNum;
		vector<vector<int> > C_r_v;
		getILPVertexInputParameter(Substrate_Network, Virtual_Network_List, RNum, SVNum, VVNum, AVVNum, C_r_v);

		VONETopo::LinkList SGraph;
		int SENum;
		int MinSEBw;
		vector<VONETopo::LinkList> VsGraph;
		vector<int> VENum;
		vector<int> AVENum;
		vector<int> VEBw;
		vector<int> AVEBw;
		vector<int> AVENumBw;
		getILPEdgeInputParameter(Substrate_Network, Virtual_Network_List, SGraph, SENum, MinSEBw, VsGraph, VENum, 
			AVENum, VEBw, AVEBw, AVENumBw);

		try
		{
			IloBoolVarArray Node_r_v_s(_env);
			constraints1(Node_r_v_s, _constraints, RNum, SVNum, VVNum, AVVNum, C_r_v);
			_variables.add(Node_r_v_s);

			IloBoolVarArray Link_r_e_s_d(_env);
			constraints2(Link_r_e_s_d, _constraints, RNum, SENum, VENum, AVENum, SGraph);
			_variables.add(Link_r_e_s_d);
			constraints3(_constraints, Link_r_e_s_d, Node_r_v_s, RNum, SENum, VENum, AVENum, SVNum, AVVNum, SGraph, VsGraph);
			IloExpr Obj_1(_env);
			for (IloInt i = 1; i <= Link_r_e_s_d.getSize(); i++)
			{
				Obj_1 += Link_r_e_s_d[i - 1];
			}

			string ModelLPFileName;
			IloExpr Obj(_env);
			if ((TypeFlag == 1) || (TypeFlag == 2))
			{
				IloBoolVarArray Core_r_c_s_d(_env);
				constraints4(Core_r_c_s_d, _constraints, Link_r_e_s_d, RNum, SENum, VENum, AVENum, SGraph, CoreNum);
				_variables.add(Core_r_c_s_d);

				IloIntVarArray f_r_e_s_d(_env);
				constraints5(f_r_e_s_d, _model, Link_r_e_s_d, RNum, SENum, VENum, AVENum, SGraph, VEBw); //wavelength or frequency start index
				_variables.add(f_r_e_s_d);

				IloIntVarArray Ms_r_e_s_d(_env);
				constraints7(Ms_r_e_s_d, _constraints, f_r_e_s_d, RNum, SENum, VENum, AVENum, SGraph, VEBw); //spectrum continuity
				_variables.add(Ms_r_e_s_d);

				_model.add(_variables);
				_model.add(_constraints);
				constraints8(_model, Core_r_c_s_d, f_r_e_s_d, RNum, SENum, VENum, AVENum, VEBw, CoreNum); //spectrum no overlap
				//constraints10(_model, f_r_e_s_d, Link_r_e_s_d, RNum, SENum, VENum, AVENum); //spectrum consistency

				Obj = IloMax(Ms_r_e_s_d) + Obj_1;
				ModelLPFileName = TypeFlag == 1 ? "WDMonMCF" : "OFDMonMCF";
			}
			else
			{
				if (TypeFlag == 0)
				{
					IloBoolVarArray Core_r_c_s_d(_env);
					constraints4(Core_r_c_s_d, _constraints, Link_r_e_s_d, RNum, SENum, VENum, AVENum, SGraph, CoreNum);
					_variables.add(Core_r_c_s_d);

					IloIntVarArray f_r_slot_e_s_d(_env);
					constraints6(f_r_slot_e_s_d, _model, Link_r_e_s_d, RNum, SENum, VENum, AVENum, SGraph, VEBw, AVENumBw); //time slot start index
					_variables.add(f_r_slot_e_s_d);

					_model.add(_variables);
					_model.add(_constraints);
					constraints9(_model, Core_r_c_s_d, f_r_slot_e_s_d, RNum, SENum, VENum, VEBw, AVENumBw, CoreNum); //spectrum no overlap					
					constraints11(_model, f_r_slot_e_s_d, Link_r_e_s_d, RNum, SENum, VENum, AVENum, VEBw, AVENumBw); //time slot consistency
					constraints12(_model, f_r_slot_e_s_d, Link_r_e_s_d, RNum, SENum, VENum, AVENum, VEBw, AVENumBw); //time slot order maintain

					/*//Another way to TDM time slot assignment:
					IloIntVarArray f_r_slot(_env);
					constraints13(f_r_slot, _constraints, RNum, MinSEBw, VEBw, AVEBw);
					_variables.add(f_r_slot);

					_model.add(_variables);
					_model.add(_constraints);
					constraints14(_model, f_r_slot, Node_r_v_s, RNum, SVNum, VVNum, AVVNum, C_r_v, VEBw, AVEBw);
					constraints15(_model, Core_r_c_s_d, f_r_slot, RNum, SENum, VEBw, AVEBw, CoreNum);*/

					Obj = IloMax(f_r_slot_e_s_d) + Obj_1;
					ModelLPFileName = "TDMonMCF";
				}
				else
				{
					cerr << "TypeFlag is wrong.\n The input model does not match the TypeFlag!\n";
				}
			}

			_model.add(IloMinimize(_env, Obj));
			_cplex.extract(_model);
			ModelLPFileName = ModelLPFileName + to_string(RNum) + ".lp";
			_cplex.exportModel(ModelLPFileName.c_str());

			//Some parameters to control the whole ILP solveing process:
			//_cplex.setParam(IloCplex::Param::Emphasis::MIP, CPX_MIPEMPHASIS_BESTBOUND);
			//_cplex.setParam(IloCplex::Param::Emphasis::MIP, CPX_MIPEMPHASIS_FEASIBILITY);
			//_cplex.setParam(IloCplex::Param::MIP::Strategy::HeuristicFreq, 1);
			//_cplex.setParam(IloCplex::Param::MIP::Strategy::NodeSelect, CPX_NODESEL_BESTEST);
			//_cplex.setParam(IloCplex::Param::MIP::Strategy::NodeSelect, CPX_NODESEL_BESTEST_ALT);
			//_cplex.setParam(IloCplex::Param::MIP::Strategy::NodeSelect, CPX_NODESEL_DFS);
			//_cplex.setParam(IloCplex::Param::MIP::Strategy::VariableSelect, CPX_VARSEL_STRONG);
			//_cplex.setParam(IloCplex::Param::MIP::Tolerances::RelObjDifference, 0.05);
			//_cplex.setParam(IloCplex::Param::MIP::Tolerances::ObjDifference, 6);
			//_cplex.setParam(IloCplex::Param::MIP::Tolerances::MIPGap, 0.1);
			//_cplex.setParam(IloCplex::Param::MIP::Strategy::Search, 1);
			//_cplex.setParam(IloCplex::Param::TimeLimit, 60);

			if (!_cplex.solve())
			{
				_env.error() << "Failed to optimize the RSCA ILP.\n";
				throw(-1);
			}
		}
		catch (IloException& e)
		{
			cerr << "Concert exception caught: " << e << endl;
		}
		catch (...)
		{
			cerr << "Unknown exception caught!\n";
		}
	}

	ostream& operator <<(ostream& outs, VONECplex& VONEILP)
	{
		IloNumArray _values(VONEILP._env);
		VONEILP._cplex.getValues(_values, VONEILP._variables);
		VONEILP._env.setOut(outs);

		VONEILP._env.out() << "Solution Status = " << VONEILP._cplex.getStatus() << endl;
		VONEILP._env.out() << "Solution Objective Value Maximum M = " << VONEILP._cplex.getObjValue() << endl;
		VONEILP._env.out() << "Solution Variables Value are following:\n";
		VONEILP._env.out() << left << setw(30) << "Variable Name" << "Value" << endl;

		int LinkNum = 0;
		for (int i = 0; i < _values.getSize(); i++)
		{
			if (_values[i] != 0)
			{
				string VarName = VONEILP._variables[i].getName();
				if (VarName[0] != 'M')
				{
					VONEILP._env.out() << left << setw(30) << VarName
						<< _values[i] << endl;
				}
				 
				if (VarName[0] == 'x')
				{
					LinkNum += 1;
				}
			}
		}
		VONEILP._env.out() << "Ms = " << VONEILP._cplex.getObjValue() - LinkNum << endl;
		return outs;
	}

	VONECplex::~VONECplex()
	{
		_env.end();
	}
}
namespace
{
	void getILPVertexInputParameter
	(const VONETopo::Topo& Substrate_Network,
		const vector<VONETopo::Topo>& Virtual_Network_List,
		int& Request_Number,
		int& Substrate_Network_Vertex_Number,
		vector<int>& Virtual_Networks_Vertex_Number,
		vector<int>& Accumulate_Virtual_Networks_Vertex_Number,
		vector<vector<int> >& Computing_Capacity_in_Virtual_Vertexes)
	{
		VONETopo::NodeList Substrate_Network_NodeList;
		Substrate_Network.get_vertex(Substrate_Network_NodeList);

		Substrate_Network_Vertex_Number = static_cast<int>(Substrate_Network_NodeList.size());

		Request_Number = static_cast<int>(Virtual_Network_List.size());

		Computing_Capacity_in_Virtual_Vertexes.resize(Request_Number);

		for (vector<VONETopo::Topo>::const_iterator TopoIT = Virtual_Network_List.begin();
			TopoIT != Virtual_Network_List.end(); TopoIT++)
		{
			VONETopo::NodeList Virtual_Network_NodeList;
			TopoIT->get_vertex(Virtual_Network_NodeList);
			Virtual_Networks_Vertex_Number.push_back(static_cast<int>(Virtual_Network_NodeList.size()));

			int request_index = static_cast<int>(TopoIT - Virtual_Network_List.begin() + 1);

			if (request_index == 1)
			{
				Accumulate_Virtual_Networks_Vertex_Number.push_back(0);
			}
			else
			{

				int Sum = accumulate(Virtual_Networks_Vertex_Number.begin(),
					Virtual_Networks_Vertex_Number.begin() + request_index - 1, 0);
				Accumulate_Virtual_Networks_Vertex_Number.push_back(Sum);
			}

			for (VONETopo::NodeListIT VNTopoIT = Virtual_Network_NodeList.begin();
				VNTopoIT != Virtual_Network_NodeList.end(); VNTopoIT++)
			{
				Computing_Capacity_in_Virtual_Vertexes[request_index - 1].push_back(VNTopoIT->_vertex_computing_capacity);
			}
		}
	}

	void constraints1
	(IloBoolVarArray x,
		IloRangeArray c,
		const int Request_Number,
		const int Substrate_Network_Vertex_Number,
		const vector<int> Virtual_Networks_Vertex_Number,
		const vector<int> Accumulate_Virtual_Networks_Vertex_Number,
		const vector<vector<int> > Computing_Capacity_in_Virtual_Vertexes)
	{
		IloEnv env = c.getEnv();
		string Variable = "y_";
		for (int r = 1; r <= Request_Number; r++)
		{
			for (int v = 1; v <= Virtual_Networks_Vertex_Number[r - 1]; v++)
			{
				IloExpr con1(env);
				for (int s = 1; s <= Substrate_Network_Vertex_Number; s++)
				{
					string Variable_Name = Variable + to_string(r) + "_" + to_string(v) + "_" + to_string(s);
					x.add(IloBoolVar(env, Variable_Name.c_str()));
					if (s == Computing_Capacity_in_Virtual_Vertexes[r - 1][v - 1])
					{
						c.add(x[(s - 1)
							+ (v - 1) * Substrate_Network_Vertex_Number
							+ Substrate_Network_Vertex_Number * Accumulate_Virtual_Networks_Vertex_Number[r - 1]] == 1);
					}
					else
					{
						c.add(x[(s - 1)
							+ (v - 1) * Substrate_Network_Vertex_Number
							+ Substrate_Network_Vertex_Number * Accumulate_Virtual_Networks_Vertex_Number[r - 1]] == 0);
					}
				}
			}
		}
	}

	void getILPEdgeInputParameter
	(const VONETopo::Topo& Substrate_Network,
		const vector<VONETopo::Topo>& Virtual_Network_List,
		VONETopo::LinkList& Substrate_Network_Edge,
		int& Substrate_Network_Edge_Number,
		int& Min_Substrate_Network_Bandwidth,
		vector<VONETopo::LinkList>& Virtual_Networks_Edge,
		vector<int>& Virtual_Networks_Edge_Number,
		vector<int>& Accumulate_Virtual_Networks_Edge_Number,
		vector<int>& Virtual_Networks_Bandwidth,
		vector<int>& Accumulate_Virtual_Networks_Bandwidth,
		vector<int>& Accumulate_Virtual_Networks_Edge_Number_and_Bandwidth)
	{
		Substrate_Network.get_edge(Substrate_Network_Edge);
		Substrate_Network_Edge_Number = static_cast<int>(Substrate_Network_Edge.size());
		vector<int> Substrate_Network_Bandwidth;
		for (VONETopo::LinkListIT SNTopoIT = Substrate_Network_Edge.begin();
			SNTopoIT != Substrate_Network_Edge.end(); SNTopoIT++)
		{
			Substrate_Network_Bandwidth.push_back(SNTopoIT->_edge_bandwidth);
		}
		Min_Substrate_Network_Bandwidth = *min_element(Substrate_Network_Bandwidth.begin(),
			Substrate_Network_Bandwidth.end());

		for (vector<VONETopo::Topo>::const_iterator VNTopoIT = Virtual_Network_List.begin();
			VNTopoIT != Virtual_Network_List.end(); VNTopoIT++)
		{
			VONETopo::LinkList Virtual_Network_EdgeList;
			VNTopoIT->get_edge(Virtual_Network_EdgeList);
			Virtual_Networks_Edge.push_back(Virtual_Network_EdgeList);
			Virtual_Networks_Edge_Number.push_back(static_cast<int>(Virtual_Network_EdgeList.size()));
			Virtual_Networks_Bandwidth.push_back(Virtual_Network_EdgeList[0]._edge_bandwidth);

			int request_index = static_cast<int>(VNTopoIT - Virtual_Network_List.begin() + 1);

			if (request_index == 1)
			{
				Accumulate_Virtual_Networks_Edge_Number.push_back(0);
				Accumulate_Virtual_Networks_Bandwidth.push_back(0);
				Accumulate_Virtual_Networks_Edge_Number_and_Bandwidth.push_back(0);
			}
			else
			{
				Accumulate_Virtual_Networks_Edge_Number.push_back(Accumulate_Virtual_Networks_Edge_Number[request_index - 1 - 1] + 
					Virtual_Networks_Edge_Number[request_index - 1 - 1]);
				Accumulate_Virtual_Networks_Bandwidth.push_back(Accumulate_Virtual_Networks_Bandwidth[request_index - 1 - 1] +
					Virtual_Networks_Bandwidth[request_index - 1 - 1]);
				Accumulate_Virtual_Networks_Edge_Number_and_Bandwidth.push_back(Accumulate_Virtual_Networks_Edge_Number_and_Bandwidth[request_index - 1 - 1] +
					Virtual_Networks_Edge_Number[request_index - 1 - 1] * Virtual_Networks_Bandwidth[request_index - 1 - 1]);
			}
		}
	}

	void constraints2
	(IloBoolVarArray x,
		IloRangeArray c,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const VONETopo::LinkList& Substrate_Network_Edge)
	{
		IloEnv env = c.getEnv();
		string Variable = "x_";
		for (int r = 1; r <= Request_Number; r++)
		{
			for (int sd = 1; sd <= Substrate_Network_Edge_Number; sd++)
			{
				IloExpr con4(env);
				for (int e = 1; e <= Virtual_Networks_Edge_Number[r - 1]; e++)
				{
					string Variable_Name = Variable + to_string(r) + "_" + to_string(e) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_source._vertex_id) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_destination._vertex_id);
					x.add(IloBoolVar(env, Variable_Name.c_str()));
					Variable_Name = Variable + to_string(r) + "_" + to_string(e) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_destination._vertex_id) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_source._vertex_id);
					x.add(IloBoolVar(env, Variable_Name.c_str()));
					con4 += x[2 * (e - 1)
						+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number]
						+ x[(2 * e - 1)
						+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number];
				}
				c.add(con4 <= 1);
			}
		}
	}

	void constraints3
	(IloRangeArray c,
		const IloBoolVarArray x,
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const int Substrate_Network_Vertex_Number,
		const vector<int> Accumulate_Virtual_Networks_Vertex_Number,
		const VONETopo::LinkList& Substrate_Network_Edge,
		const vector<VONETopo::LinkList>& Virtual_Networks_Edge)
	{
		IloEnv env = c.getEnv();
		for (int r = 1; r <= Request_Number; r++)
		{
			for (int e = 1; e <= Virtual_Networks_Edge_Number[r - 1]; e++)
			{
				for (int s = 1; s <= Substrate_Network_Vertex_Number; s++)
				{
					IloExpr con5(env);
					for (int sd = 1; sd <= Substrate_Network_Edge_Number; sd++)
					{
						if (Substrate_Network_Edge[sd - 1]._edge_source._vertex_id == s)
						{
							con5 += x[2 * (e - 1)
								+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number]
								- x[(2 * e - 1)
								+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number];
						}

						if (Substrate_Network_Edge[sd - 1]._edge_destination._vertex_id == s)
						{
							con5 += x[(2 * e - 1)
								+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number]
								- x[2 * (e - 1)
								+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number];
						}
					}
					c.add(con5 - y[(s - 1)
						+ (Virtual_Networks_Edge[r - 1][e - 1]._edge_source._vertex_id - 1) * Substrate_Network_Vertex_Number
						+ Substrate_Network_Vertex_Number * Accumulate_Virtual_Networks_Vertex_Number[r - 1]] + y[(s - 1)
						+ (Virtual_Networks_Edge[r - 1][e - 1]._edge_destination._vertex_id - 1) * Substrate_Network_Vertex_Number
						+ Substrate_Network_Vertex_Number * Accumulate_Virtual_Networks_Vertex_Number[r - 1]] == 0);
				}
			}
		}
	}

	void constraints4
	(IloBoolVarArray x,
		IloRangeArray con,
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const VONETopo::LinkList& Substrate_Network_Edge,
		const int C)
	{
		IloEnv env = con.getEnv();
		string Variable = "X_";
		for (int r = 1; r <= Request_Number; r++)
		{
			for (int sd = 1; sd <= Substrate_Network_Edge_Number; sd++)
			{
				IloExpr con6_1(env), con6_2(env);
				for (int c = 1; c <= C; c++)
				{
					string Variable_Name = Variable + to_string(r) + "_" + to_string(c) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_source._vertex_id) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_destination._vertex_id);
					x.add(IloBoolVar(env, Variable_Name.c_str()));
					Variable_Name = Variable + to_string(r) + "_" + to_string(c) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_destination._vertex_id) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_source._vertex_id);
					x.add(IloBoolVar(env, Variable_Name.c_str()));
					con6_1 += x[2 * (c - 1)
						+ (sd - 1) * C * 2
						+ (r - 1) * C * 2 * Substrate_Network_Edge_Number];
					con6_2 += x[(2 * c - 1)
						+ (sd - 1) * C * 2
						+ (r - 1) * C * 2 * Substrate_Network_Edge_Number];
				}

				IloExpr con6_1_right(env), con6_2_right(env);
				for (int e = 1; e <= Virtual_Networks_Edge_Number[r - 1]; e++)
				{
					con6_1_right += y[2 * (e - 1)
						+ (sd - 1) *  Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number];
					con6_2_right += y[(2 * e - 1)
						+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number];
				}

				con.add(con6_1 - con6_1_right == 0);
				con.add(con6_2 - con6_2_right == 0);
			}
		}
	}

	void constraints5
	(IloIntVarArray x,
		IloModel mod,
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const VONETopo::LinkList& Substrate_Network_Edge,
		const vector<int> Virtual_Networks_Bandwidth)
	{
		IloEnv env = mod.getEnv();
		string Variable = "f_";
		for (int r = 1; r <= Request_Number; r++)
		{
			for (int sd = 1; sd <= Substrate_Network_Edge_Number; sd++)
			{
				for (int e = 1; e <= Virtual_Networks_Edge_Number[r - 1]; e++)
				{
					string Variable_Name = Variable + to_string(r) + "_" + to_string(e) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_source._vertex_id) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_destination._vertex_id);
					x.add(IloIntVar(env, 0, Substrate_Network_Edge[sd-1]._edge_bandwidth + 
						1 - Virtual_Networks_Bandwidth[r - 1], Variable_Name.c_str()));
					Variable_Name = Variable + to_string(r) + "_" + to_string(e) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_destination._vertex_id) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_source._vertex_id);
					x.add(IloIntVar(env, 0, Substrate_Network_Edge[sd - 1]._edge_bandwidth +
						1 - Virtual_Networks_Bandwidth[r - 1], Variable_Name.c_str()));

					mod.add(IloIfThen(env, y[2 * (e - 1)
						+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 0, 
						x[2 * (e - 1)
						+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 0));
					mod.add(IloIfThen(env, y[(2 * e - 1)
						+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 0, 
						x[(2 * e - 1)
						+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 0));
					
					mod.add(IloIfThen(env, y[2 * (e - 1)
						+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 1,
						x[2 * (e - 1)
						+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] >= 1));
					mod.add(IloIfThen(env, y[(2 * e - 1)
						+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 1,
						x[(2 * e - 1)
						+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] >= 1));
				}
			}
		}
	}

	void constraints6
	(IloIntVarArray x,
		IloModel mod,
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const VONETopo::LinkList& Substrate_Network_Edge,
		const vector<int> Virtual_Networks_Bandwidth,
		const vector<int> Accumulate_Virtual_Networks_Bandwidth)
	{
		IloEnv env = mod.getEnv();
		string Variable = "t_";
		for (int r = 1; r <= Request_Number; r++)
		{
			for (int slot = 1; slot <= Virtual_Networks_Bandwidth[r - 1]; slot++)
			{
				for (int sd = 1; sd <= Substrate_Network_Edge_Number; sd++)
				{
					for (int e = 1; e <= Virtual_Networks_Edge_Number[r - 1]; e++)
					{
						string Variable_Name = Variable + to_string(r) + "_" + to_string(slot) + "_"
							+ to_string(e) + "_" + to_string(Substrate_Network_Edge[sd - 1]._edge_source._vertex_id) + "_"
							+ to_string(Substrate_Network_Edge[sd - 1]._edge_destination._vertex_id);
						x.add(IloIntVar(env, 0, Substrate_Network_Edge[sd - 1]._edge_bandwidth,
							Variable_Name.c_str()));
						Variable_Name = Variable + to_string(r) + "_" + to_string(slot) + "_"
							+ to_string(e) + "_" + to_string(Substrate_Network_Edge[sd - 1]._edge_destination._vertex_id) + "_"
							+ to_string(Substrate_Network_Edge[sd - 1]._edge_source._vertex_id);
						x.add(IloIntVar(env, 0, Substrate_Network_Edge[sd - 1]._edge_bandwidth,
							Variable_Name.c_str()));

						mod.add(IloIfThen(env, y[2 * (e - 1)
							+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
							+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 0,
							x[2 * (e - 1)
							+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
							+ (slot - 1) * Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number
							+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r - 1]] == 0));
						mod.add(IloIfThen(env, y[(2 * e - 1)
							+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
							+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 0,
							x[(2 * e - 1)
							+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
							+ (slot - 1) * Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number
							+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r - 1]] == 0));

						mod.add(IloIfThen(env, y[2 * (e - 1)
							+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
							+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 1,
							x[2 * (e - 1)
							+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
							+ (slot - 1) * Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number
							+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r - 1]] >= 1));
						mod.add(IloIfThen(env, y[(2 * e - 1)
							+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
							+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 1,
							x[(2 * e - 1)
							+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
							+ (slot - 1) * Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number
							+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r - 1]] >= 1));
					}
				}
			}
		}
	}

	void constraints7
	(IloIntVarArray x,
		IloRangeArray con,
		const IloIntVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const VONETopo::LinkList& Substrate_Network_Edge,
		const vector<int> Virtual_Networks_Bandwidth)
	{
		IloEnv env = con.getEnv();
		string Variable = "Ms_";
		for (int r = 1; r <= Request_Number; r++)
		{
			for (int sd = 1; sd <= Substrate_Network_Edge_Number; sd++)
			{
				for (int e = 1; e <= Virtual_Networks_Edge_Number[r - 1]; e++)
				{
					string Variable_Name = Variable + to_string(r) + "_" + to_string(e) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_source._vertex_id) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_destination._vertex_id);
					x.add(IloIntVar(env, Variable_Name.c_str()));
					Variable_Name = Variable + to_string(r) + "_" + to_string(e) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_destination._vertex_id) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_source._vertex_id);
					x.add(IloIntVar(env, Variable_Name.c_str()));

					con.add(x[2 * (e - 1)
						+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] -
						y[2 * (e - 1)
						+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] ==
						Virtual_Networks_Bandwidth[r - 1] - 1);
					con.add(x[(2 * e - 1)
						+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] -
						y[(2 * e - 1)
						+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] ==
						Virtual_Networks_Bandwidth[r - 1] - 1);
				}
			}
		}
	}

	void constraints8
	(IloModel mod,
		const IloBoolVarArray x,
		const IloIntVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const vector<int> Virtual_Networks_Bandwidth,
		const int C)
	{
		IloEnv env = mod.getEnv();
		for (int r1 = 1; r1 <= Request_Number - 1; r1++)
		{
			for (int r2 = r1 + 1; r2 <= Request_Number; r2++)
			{
				int Omega_r1 = Virtual_Networks_Bandwidth[r1 - 1];
				int Omega_r2 = Virtual_Networks_Bandwidth[r2 - 1];
				for (int sd = 1; sd <= Substrate_Network_Edge_Number; sd++)
				{
					for (int c = 1; c <= C; c++)
					{
						IloExpr same_core_con1(env), same_core_con2(env),
							spectrum_nooverlap_con1(env), spectrum_nooverlap_con2(env);

						same_core_con1 = x[2 * (c - 1)
							+ (sd - 1) * C * 2
							+ (r1 - 1) * C * 2 * Substrate_Network_Edge_Number] +
							x[2 * (c - 1)
							+ (sd - 1) * C * 2
							+ (r2 - 1) * C * 2 * Substrate_Network_Edge_Number];

						same_core_con2 = x[(2 * c - 1)
							+ (sd - 1) * C * 2
							+ (r1 - 1) * C * 2 * Substrate_Network_Edge_Number] +
							x[(2 * c - 1)
							+ (sd - 1) * C * 2
							+ (r2 - 1) * C * 2 * Substrate_Network_Edge_Number];

						for (int e1 = 1; e1 <= Virtual_Networks_Edge_Number[r1 - 1]; e1++)
						{
							for (int e2 = 1; e2 <= Virtual_Networks_Edge_Number[r2 - 1]; e2++)
							{
								spectrum_nooverlap_con1 = 
									(y[2 * (e1 - 1)
									+ (sd - 1) * Virtual_Networks_Edge_Number[r1 - 1] * 2
									+ Accumulate_Virtual_Networks_Edge_Number[r1 - 1] * 2 * Substrate_Network_Edge_Number] -
									y[2 * (e2 - 1)
									+ (sd - 1) * Virtual_Networks_Edge_Number[r2 - 1] * 2
									+ Accumulate_Virtual_Networks_Edge_Number[r2 - 1] * 2 * Substrate_Network_Edge_Number] >= Omega_r2) +
									(y[2 * (e2 - 1)
									+ (sd - 1) * Virtual_Networks_Edge_Number[r2 - 1] * 2
									+ Accumulate_Virtual_Networks_Edge_Number[r2 - 1] * 2 * Substrate_Network_Edge_Number] -
									y[2 * (e1 - 1)
									+ (sd - 1) * Virtual_Networks_Edge_Number[r1 - 1] * 2
									+ Accumulate_Virtual_Networks_Edge_Number[r1 - 1] * 2 * Substrate_Network_Edge_Number] >= Omega_r1);

								spectrum_nooverlap_con2 =
									(y[(2 * e1 - 1)
									+ (sd - 1) * Virtual_Networks_Edge_Number[r1 - 1] * 2
									+ Accumulate_Virtual_Networks_Edge_Number[r1 - 1] * 2 * Substrate_Network_Edge_Number] -
									y[(2 * e2 - 1)
									+ (sd - 1) * Virtual_Networks_Edge_Number[r2 - 1] * 2
									+ Accumulate_Virtual_Networks_Edge_Number[r2 - 1] * 2 * Substrate_Network_Edge_Number] >= Omega_r2) +
									(y[(2 * e2 - 1)
									+ (sd - 1) * Virtual_Networks_Edge_Number[r2 - 1] * 2
									+ Accumulate_Virtual_Networks_Edge_Number[r2 - 1] * 2 * Substrate_Network_Edge_Number] -
									y[(2 * e1 - 1)
									+ (sd - 1) * Virtual_Networks_Edge_Number[r1 - 1] * 2
									+ Accumulate_Virtual_Networks_Edge_Number[r1 - 1] * 2 * Substrate_Network_Edge_Number] >= Omega_r1);

								//Without taking the cross-talk between two adjacent cores into account:
								mod.add(IloIfThen(env,
									((same_core_con1 == 2)&&
									(y[2 * (e1 - 1)
										+ (sd - 1) * Virtual_Networks_Edge_Number[r1 - 1] * 2
										+ Accumulate_Virtual_Networks_Edge_Number[r1 - 1] * 2 * Substrate_Network_Edge_Number] != 0)&&
									(y[2 * (e2 - 1)
										+ (sd - 1) * Virtual_Networks_Edge_Number[r2 - 1] * 2
										+ Accumulate_Virtual_Networks_Edge_Number[r2 - 1] * 2 * Substrate_Network_Edge_Number] != 0)), 
									spectrum_nooverlap_con1 == 1));

								mod.add(IloIfThen(env,
									((same_core_con2 == 2)&&
									(y[(2 * e1 - 1)
										+ (sd - 1) * Virtual_Networks_Edge_Number[r1 - 1] * 2
										+ Accumulate_Virtual_Networks_Edge_Number[r1 - 1] * 2 * Substrate_Network_Edge_Number] != 0)&&
									(y[(2 * e2 - 1)
										+ (sd - 1) * Virtual_Networks_Edge_Number[r2 - 1] * 2
										+ Accumulate_Virtual_Networks_Edge_Number[r2 - 1] * 2 * Substrate_Network_Edge_Number] != 0)), 
									spectrum_nooverlap_con2 == 1));
							}
						}
					}
				}
			}
		}
	}

	void constraints9
	(IloModel mod,
		const IloBoolVarArray x,
		const IloIntVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Virtual_Networks_Bandwidth,
		const vector<int> Accumulate_Virtual_Networks_Bandwidth,
		const int C)
	{
		IloEnv env = mod.getEnv();
		for (int r1 = 1; r1 <= Request_Number - 1; r1++)
		{
			for (int r2 = r1 + 1; r2 <= Request_Number; r2++)
			{
				int Omega_r1 = 1;
				int Omega_r2 = 1;
				for (int slot1 = 1; slot1 <= Virtual_Networks_Bandwidth[r1 - 1]; slot1++)
				{
					for (int slot2 = 1; slot2 <= Virtual_Networks_Bandwidth[r2 - 1]; slot2++)
					{
						for (int sd = 1; sd <= Substrate_Network_Edge_Number; sd++)
						{
							for (int e1 = 1; e1 <= Virtual_Networks_Edge_Number[r1 - 1]; e1++)
							{
								for (int e2 = 1; e2 <= Virtual_Networks_Edge_Number[r2 - 1]; e2++)
								{
									for (int c = 1; c <= C; c++)
									{
										IloExpr same_core_con1(env), same_core_con2(env),
											spectrum_nooverlap_con1(env), spectrum_nooverlap_con2(env);

										same_core_con1 = x[2 * (c - 1)
											+ (sd - 1) * C * 2
											+ (r1 - 1) * C * 2 * Substrate_Network_Edge_Number] +
											x[2 * (c - 1)
											+ (sd - 1) * C * 2
											+ (r2 - 1) * C * 2 * Substrate_Network_Edge_Number];

										same_core_con2 = x[(2 * c - 1)
											+ (sd - 1) * C * 2
											+ (r1 - 1) * C * 2 * Substrate_Network_Edge_Number] +
											x[(2 * c - 1)
											+ (sd - 1) * C * 2
											+ (r2 - 1) * C * 2 * Substrate_Network_Edge_Number];

										spectrum_nooverlap_con1 = (y[2 * (e1 - 1)
											+ (sd - 1) * Virtual_Networks_Edge_Number[r1 - 1] * 2
											+ (slot1 - 1) * Virtual_Networks_Edge_Number[r1 - 1] * 2 * Substrate_Network_Edge_Number
											+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r1 - 1]] -
											y[2 * (e2 - 1)
											+ (sd - 1) * Virtual_Networks_Edge_Number[r2 - 1] * 2
											+ (slot2 - 1) * Virtual_Networks_Edge_Number[r2 - 1] * 2 * Substrate_Network_Edge_Number
											+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r2 - 1]] >= Omega_r2) +
											(y[2 * (e2 - 1)
												+ (sd - 1) * Virtual_Networks_Edge_Number[r2 - 1] * 2
												+ (slot2 - 1) * Virtual_Networks_Edge_Number[r2 - 1] * 2 * Substrate_Network_Edge_Number
												+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r2 - 1]] -
												y[2 * (e1 - 1)
												+ (sd - 1) * Virtual_Networks_Edge_Number[r1 - 1] * 2
												+ (slot1 - 1) * Virtual_Networks_Edge_Number[r1 - 1] * 2 * Substrate_Network_Edge_Number
												+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r1 - 1]] >= Omega_r1);
										spectrum_nooverlap_con2 = (y[(2 * e1 - 1)
											+ (sd - 1) * Virtual_Networks_Edge_Number[r1 - 1] * 2
											+ (slot1 - 1) * Virtual_Networks_Edge_Number[r1 - 1] * 2 * Substrate_Network_Edge_Number
											+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r1 - 1]] -
											y[(2 * e2 - 1)
											+ (sd - 1) * Virtual_Networks_Edge_Number[r2 - 1] * 2
											+ (slot2 - 1) * Virtual_Networks_Edge_Number[r2 - 1] * 2 * Substrate_Network_Edge_Number
											+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r2 - 1]] >= Omega_r2) +
											(y[(2 * e2 - 1)
												+ (sd - 1) * Virtual_Networks_Edge_Number[r2 - 1] * 2
												+ (slot2 - 1) * Virtual_Networks_Edge_Number[r2 - 1] * 2 * Substrate_Network_Edge_Number
												+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r2 - 1]] -
												y[(2 * e1 - 1)
												+ (sd - 1) * Virtual_Networks_Edge_Number[r1 - 1] * 2
												+ (slot1 - 1) * Virtual_Networks_Edge_Number[r1 - 1] * 2 * Substrate_Network_Edge_Number
												+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r1 - 1]] >= Omega_r1);

										mod.add(IloIfThen(env,
											((same_core_con1 == 2) &&
											(y[2 * (e1 - 1)
												+ (sd - 1) * Virtual_Networks_Edge_Number[r1 - 1] * 2
												+ (slot1 - 1) * Virtual_Networks_Edge_Number[r1 - 1] * 2 * Substrate_Network_Edge_Number
												+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r1 - 1]] != 0) &&
												(y[2 * (e2 - 1)
													+ (sd - 1) * Virtual_Networks_Edge_Number[r2 - 1] * 2
													+ (slot2 - 1) * Virtual_Networks_Edge_Number[r2 - 1] * 2 * Substrate_Network_Edge_Number
													+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r2 - 1]] != 0)),
											spectrum_nooverlap_con1 == 1));

										mod.add(IloIfThen(env,
											((same_core_con2 == 2) &&
											(y[(2 * e1 - 1)
												+ (sd - 1) * Virtual_Networks_Edge_Number[r1 - 1] * 2
												+ (slot1 - 1) * Virtual_Networks_Edge_Number[r1 - 1] * 2 * Substrate_Network_Edge_Number
												+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r1 - 1]] != 0) &&
												(y[(2 * e2 - 1)
													+ (sd - 1) * Virtual_Networks_Edge_Number[r2 - 1] * 2
													+ (slot2 - 1) * Virtual_Networks_Edge_Number[r2 - 1] * 2 * Substrate_Network_Edge_Number
													+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r2 - 1]] != 0)),
											spectrum_nooverlap_con2 == 1));
									}
								}
							}
						}
					}
				}
			}
		}
	}

	void constraints10
	(IloModel mod,
		const IloIntVarArray x,
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number)
	{
		IloEnv env = mod.getEnv();
		for (int r = 1; r <= Request_Number; r++)
		{
			for (int sd1 = 1; sd1 <= Substrate_Network_Edge_Number; sd1++)
			{
				for (int sd2 = 1; sd2 <= Substrate_Network_Edge_Number; sd2++)
				{
					for (int e = 1; e <= Virtual_Networks_Edge_Number[r - 1]; e++)
					{
						if (sd1 != sd2)
						{
							mod.add(IloIfThen(env, y[2 * (e - 1)
								+ (sd1 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] +
								y[2 * (e - 1)
								+ (sd2 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 2,
								x[2 * (e - 1)
								+ (sd1 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] -
								x[2 * (e - 1)
								+ (sd2 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 0));

							mod.add(IloIfThen(env, y[2 * (e - 1)
								+ (sd1 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] +
								y[(2 * e - 1)
								+ (sd2 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 2,
								x[2 * (e - 1)
								+ (sd1 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] -
								x[(2 * e - 1)
								+ (sd2 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 0));

							mod.add(IloIfThen(env, y[(2 * e - 1)
								+ (sd1 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] +
								y[2 * (e - 1)
								+ (sd2 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 2,
								x[(2 * e - 1)
								+ (sd1 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] -
								x[2 * (e - 1)
								+ (sd2 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 0));

							mod.add(IloIfThen(env, y[(2 * e - 1)
								+ (sd1 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] +
								y[(2 * e - 1)
								+ (sd2 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 2,
								x[(2 * e - 1)
								+ (sd1 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] -
								x[(2 * e - 1)
								+ (sd2 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 0));
						}
					}
				}
			}
		}
	}

	void constraints11
	(IloModel mod,
		const IloIntVarArray x,
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const vector<int> Virtual_Networks_Bandwidth,
		const vector<int> Accumulate_Virtual_Networks_Bandwidth)
	{
		IloEnv env = mod.getEnv();
		for (int r = 1; r <= Request_Number; r++)
		{
			for (int slot = 1; slot <= Virtual_Networks_Bandwidth[r - 1]; slot++)
			{
				for (int sd1 = 1; sd1 <= Substrate_Network_Edge_Number; sd1++)
				{
					for (int sd2 = 1; sd2 <= Substrate_Network_Edge_Number; sd2++)
					{
						for (int e = 1; e <= Virtual_Networks_Edge_Number[r - 1]; e++)
						{
							if (sd1 != sd2)
							{
								mod.add(IloIfThen(env, y[2 * (e - 1)
									+ (sd1 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
									+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] +
									y[2 * (e - 1)
									+ (sd2 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
									+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 2,
									x[2 * (e - 1)
									+ (sd1 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
									+ (slot - 1) * Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number
									+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r - 1]] -
									x[2 * (e - 1)
									+ (sd2 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
									+ (slot - 1) * Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number
									+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r - 1]] == 0));

								mod.add(IloIfThen(env, y[2 * (e - 1)
									+ (sd1 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
									+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] +
									y[(2 * e - 1)
									+ (sd2 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
									+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 2,
									x[2 * (e - 1)
									+ (sd1 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
									+ (slot - 1) * Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number
									+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r - 1]] -
									x[(2 * e - 1)
									+ (sd2 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
									+ (slot - 1) * Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number
									+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r - 1]] == 0));

								mod.add(IloIfThen(env, y[(2 * e - 1)
									+ (sd1 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
									+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] +
									y[2 * (e - 1)
									+ (sd2 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
									+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 2,
									x[(2 * e - 1)
									+ (sd1 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
									+ (slot - 1) * Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number
									+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r - 1]] -
									x[2 * (e - 1)
									+ (sd2 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
									+ (slot - 1) * Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number
									+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r - 1]] == 0));

								mod.add(IloIfThen(env, y[(2 * e - 1)
									+ (sd1 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
									+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] +
									y[(2 * e - 1)
									+ (sd2 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
									+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 2,
									x[(2 * e - 1)
									+ (sd1 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
									+ (slot - 1) * Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number
									+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r - 1]] -
									x[(2 * e - 1)
									+ (sd2 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
									+ (slot - 1) * Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number
									+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r - 1]] == 0));
							}
						}
					}
				}
			}
		}
	}

	void constraints12
	(IloModel mod,
		const IloIntVarArray x,
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const vector<int> Virtual_Networks_Bandwidth,
		const vector<int> Accumulate_Virtual_Networks_Bandwidth)
	{
		IloEnv env = mod.getEnv();
		for (int r = 1; r <= Request_Number; r++)
		{
			for (int slot = 1; slot <= Virtual_Networks_Bandwidth[r - 1] - 1; slot++)
			{
				for (int sd = 1; sd <= Substrate_Network_Edge_Number; sd++)
				{
					for (int e = 1; e <= Virtual_Networks_Edge_Number[r - 1]; e++)
					{
						mod.add(IloIfThen(env, y[2 * (e - 1)
							+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
							+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 1, 
							x[2 * (e - 1)
							+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
							+ (slot + 1 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number
							+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r - 1]] - 
							x[2 * (e - 1)
							+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
							+ (slot - 1) * Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number
							+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r - 1]] >= 1));
						mod.add(IloIfThen(env, y[(2 * e - 1)
							+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
							+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number] == 1,
							x[(2 * e - 1)
							+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
							+ (slot + 1 - 1) * Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number
							+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r - 1]] - 
							x[(2 * e - 1)
							+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
							+ (slot - 1) * Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number
							+ 2 * Substrate_Network_Edge_Number * Accumulate_Virtual_Networks_Bandwidth[r - 1]] >= 1));
					}
				}
			}
		}
	}

	void constraints13
	(IloIntVarArray x,
		IloRangeArray con,
		const int Request_Number,
		const int Min_Substrate_Network_Bandwidth,
		const vector<int> Virtual_Networks_Bandwidth,
		const vector<int> Accumulate_Virtual_Networks_Bandwidth)
	{
		IloEnv env = con.getEnv();
		string Variable = "f_";
		for (int r = 1; r <= Request_Number; r++)
		{
			string Variable_Name = Variable + to_string(r) + "_1";
			x.add(IloIntVar(env, 0, Min_Substrate_Network_Bandwidth, Variable_Name.c_str()));
			for (int slot = 2; slot <= Virtual_Networks_Bandwidth[r - 1]; slot++)
			{
				string Variable_Name = Variable + to_string(r) + "_" + to_string(slot);
				x.add(IloIntVar(env, 0, Min_Substrate_Network_Bandwidth, Variable_Name.c_str()));

				con.add(x[(slot - 1) + Accumulate_Virtual_Networks_Bandwidth[r - 1]] -
					x[(slot - 2) + Accumulate_Virtual_Networks_Bandwidth[r - 1]] >= 1);
			}
		}
	}

	void constraints14
	(IloModel mod, 
		const IloIntVarArray x,	
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Vertex_Number,
		const vector<int> Virtual_Networks_Vertex_Number,
		const vector<int> Accumulate_Virtual_Networks_Vertex_Number,
		const vector<vector<int> > Computing_Capacity_in_Virtual_Vertexes,
		const vector<int> Virtual_Networks_Bandwidth,
		const vector<int> Accumulate_Virtual_Networks_Bandwidth)
	{
		IloEnv env = mod.getEnv();
		for (int r = 1; r <= Request_Number; r++)
		{
			for (int slot = 1; slot <= Virtual_Networks_Bandwidth[r - 1]; slot++)
			{
				for (int v = 1; v <= Virtual_Networks_Vertex_Number[r - 1]; v++)
				{
					mod.add(IloIfThen(env, y[(Computing_Capacity_in_Virtual_Vertexes[r - 1][v - 1] - 1)
						+ (v - 1) * Substrate_Network_Vertex_Number
						+ Substrate_Network_Vertex_Number * Accumulate_Virtual_Networks_Vertex_Number[r - 1]] + 
						y[(Computing_Capacity_in_Virtual_Vertexes[r - 1][v - 1] - 1)
						+ (v - 1) * Substrate_Network_Vertex_Number
						+ Substrate_Network_Vertex_Number * Accumulate_Virtual_Networks_Vertex_Number[r - 1]] == 0,
						x[(slot - 1) + Accumulate_Virtual_Networks_Bandwidth[r - 1]] == 0));
					mod.add(IloIfThen(env, y[(Computing_Capacity_in_Virtual_Vertexes[r - 1][v - 1] - 1)
						+ (v - 1) * Substrate_Network_Vertex_Number
						+ Substrate_Network_Vertex_Number * Accumulate_Virtual_Networks_Vertex_Number[r - 1]] + 
						y[(Computing_Capacity_in_Virtual_Vertexes[r - 1][v - 1] - 1)
						+ (v - 1) * Substrate_Network_Vertex_Number
						+ Substrate_Network_Vertex_Number * Accumulate_Virtual_Networks_Vertex_Number[r - 1]] == 2,
							x[(slot - 1) + Accumulate_Virtual_Networks_Bandwidth[r - 1]] >= 1));
				}
			}
		}			
	}

	void constraints15
	(IloModel mod,
		const IloBoolVarArray x,
		const IloIntVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Bandwidth,
		const vector<int> Accumulate_Virtual_Networks_Bandwidth,
		const int C)
	{
		IloEnv env = mod.getEnv();
		for (int r1 = 1; r1 <= Request_Number - 1; r1++)
		{
			for (int r2 = r1 + 1; r2 <= Request_Number; r2++)
			{
				int Omega_r1 = 1;
				int Omega_r2 = 1;
				for (int slot1 = 1; slot1 <= Virtual_Networks_Bandwidth[r1 - 1]; slot1++)
				{
					for (int slot2 = 1; slot2 <= Virtual_Networks_Bandwidth[r2 - 1]; slot2++)
					{
						for (int sd = 1; sd <= Substrate_Network_Edge_Number; sd++)
						{
							for (int c = 1; c <= C; c++)
							{
								IloExpr same_core_con1(env), same_core_con2(env),
									spectrum_nooverlap_con(env);

								same_core_con1 = x[2 * (c - 1)
									+ (sd - 1) * C * 2
									+ (r1 - 1) * C * 2 * Substrate_Network_Edge_Number] +
									x[2 * (c - 1)
									+ (sd - 1) * C * 2
									+ (r2 - 1) * C * 2 * Substrate_Network_Edge_Number];

								same_core_con2 = x[(2 * c - 1)
									+ (sd - 1) * C * 2
									+ (r1 - 1) * C * 2 * Substrate_Network_Edge_Number] +
									x[(2 * c - 1)
									+ (sd - 1) * C * 2
									+ (r2 - 1) * C * 2 * Substrate_Network_Edge_Number];

								spectrum_nooverlap_con = (y[(slot1 - 1) + Accumulate_Virtual_Networks_Bandwidth[r1 - 1]] -
											y[(slot2 - 1) + Accumulate_Virtual_Networks_Bandwidth[r2 - 1]] >= Omega_r2) +
											(y[(slot2 - 1) + Accumulate_Virtual_Networks_Bandwidth[r2 - 1]] -
											y[(slot1 - 1) + Accumulate_Virtual_Networks_Bandwidth[r1 - 1]] >= Omega_r1);

								mod.add(IloIfThen(env,
									((same_core_con1 == 2) &&
									(y[(slot1 - 1) + Accumulate_Virtual_Networks_Bandwidth[r1 - 1]] != 0) &&
									(y[(slot2 - 1) + Accumulate_Virtual_Networks_Bandwidth[r2 - 1]] != 0)),
									spectrum_nooverlap_con == 1));

								mod.add(IloIfThen(env,
									((same_core_con2 == 2) &&
									(y[(slot1 - 1) + Accumulate_Virtual_Networks_Bandwidth[r1 - 1]] != 0) &&
									(y[(slot2 - 1) + Accumulate_Virtual_Networks_Bandwidth[r2 - 1]] != 0)),
									spectrum_nooverlap_con == 1));
							}
						}
					}
				}
			}
		}
	}
}