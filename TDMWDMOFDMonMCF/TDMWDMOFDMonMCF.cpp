//This file is the main function of TDM, WDM or OFDM on MCF.
//------------------------------------------------
//File Name: TDMWDMOFDMonMCF.cpp
//Author: Qihan Zhang
//Email: lengkudaodi@outlook.com
//Last Modified: Jun. 30th 2021
//------------------------------------------------

#include "ILP.h"
#include <fstream>

namespace
{
	static void Usage(const char* ProgramName);
	int TopologyandTrafficinput(int TypeFlag, string TopoFileName, int NumberofTraffic, string TrafficFileName,
		VONETopo::Topo& Substrate_Network, vector<VONETopo::Topo>& Virtual_Network_List);
}

//***************Program entry***************//
//MainError Code which is the retrun integer value of the main function can indicate the mistake details
//when debug the program:
//0: There are no errors during the executing;
//-1: The number of program arguments is wrong, may be not enough or too much; 
//-2: One of the name of the arguments is wrong;
//-3: The topology or traffic input file fails to read, check the return code of function TopologyandTrafficinput;
//-4: The wrong code usage of the type of the service, check that 0 is tdm, 1 is wdm and 2 is ofdm;
//-5: The result file fails to open.
//
//ErrorFlag Code which is the return integer value of the TopologyandTrafficinput function can indicate the 
//mistake details when debug the program:
//0: There are no errors during the file input;
//-1: The topology file fails to open;
//-2: The traffic file fails to open;
//1: Wrong trafiic file for the service type.
int main(int argc, char **argv)
{
	int KindofService, ServiceNumber, CoreNumber, MainError;
	string TopoFileName, TrafficFileName;
	switch (argc)
	{
	    case 1:
			Usage(argv[0]);
			MainError = -1;
		    break;
		case 2:
			if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "-help") == 0))
			{
				Usage(argv[0]);
				MainError = 0;
				exit(1);
			}
			else
			{
				cerr << "Wrong argument name!\n";
				MainError = -2;
			}		
			break;
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			Usage(argv[0]);
			MainError = -1;
			break;
		case 11:
			MainError = 0; //There are no errors.
			for (int i = 2; i <= argc; i=i+2)
			{
				if ((strcmp(argv[i - 1], "-k") == 0) || (strcmp(argv[i - 1], "-type") == 0))
					KindofService = stoi(argv[i + 1 - 1]);
				else if ((strcmp(argv[i - 1], "-t") == 0) || (strcmp(argv[i - 1], "-topo") == 0))
					TopoFileName = argv[i + 1 - 1];
				else if ((strcmp(argv[i - 1], "-r") == 0) || (strcmp(argv[i - 1], "-rnum") == 0))
					ServiceNumber = stoi(argv[i + 1 - 1]);
				else if ((strcmp(argv[i - 1], "-s") == 0) || (strcmp(argv[i - 1], "-traffic") == 0))
					TrafficFileName = argv[i + 1 - 1];
				else if ((strcmp(argv[i - 1], "-c") == 0) || (strcmp(argv[i - 1], "-cnum") == 0))
					CoreNumber = stoi(argv[i + 1 - 1]);
				else
				{
					cerr << "Wrong argument name!\n";
					MainError = -2;
					break;
				}
			}
			break;
		default:
			Usage(argv[0]);
			MainError = -1;
	}

	if (MainError != 0)
	{
		exit(1);
	}

	//Input the Substrate Network and Virtual Network list:
	VONETopo::Topo Substrate_Network;
	vector<VONETopo::Topo> Virtual_Network_List;
	int inputerror;
	inputerror = TopologyandTrafficinput(KindofService, TopoFileName, ServiceNumber, TrafficFileName, Substrate_Network, Virtual_Network_List);
	if (inputerror != 0)
	{
		MainError = -3;
		exit(1);
	}
	cout << Substrate_Network << endl;
	for (size_t i = 1; i <= Virtual_Network_List.size(); i++)
	{
		cout << "The Virtual Request Topology " << i << " are following:\n";
		cout << Virtual_Network_List[i - 1] << endl;
	}
	
	VONEILP::VONECplex SolvexDMonMCFofILP;
	ofstream output;
	string ResultFileName;
	switch (KindofService)
	{
	    case 0:
			ResultFileName = "TDMonMCFILP";
			break;
		case 1:
			ResultFileName = "WDMonMCFILP";
			break;
		case 2:
			ResultFileName = "OFDMonMCFILP";
			break;
		default:
			cerr << "The type of traffic input wrong!\n";
			MainError = -4;
			exit(1);
	}
	ResultFileName = ResultFileName + to_string(ServiceNumber) + "Result.txt";
	output.open(ResultFileName.c_str()); //Use result file to record some information
	if (output.fail())
	{
		cerr << "File " << ResultFileName << " opening failed.\n";
		MainError = -5;
		exit(1);
	}

	//Start to solve the ILP of TDM, WDM or OFDM on MCF:
	clock_t start = clock();
	SolvexDMonMCFofILP.solve(Substrate_Network, Virtual_Network_List, KindofService, CoreNumber);
	clock_t end = clock();
	output << "\nThe running time of ILP is: " << static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000 << "ms\n\n";
	cout << "\nThe running time of ILP is: " << static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000 << "ms\n\n";
	output << SolvexDMonMCFofILP << endl;
	cout << SolvexDMonMCFofILP << endl;
	output.close();

	return MainError;
}

namespace
{
	static void Usage(const char* ProgramName)
	{
		cerr << "\nUsage: " << ProgramName << " -help/-h to elaborate the reference of the program.\n";
		cerr << "\nRight usage of " << ProgramName << " is like the following: \n";
		cerr << ProgramName << " -type/-k 2 -topo/-t 'OFDMN6S8.txt' -rnum/-r 10 -traffic/-s 'ofdm5000.txt' -cnum/-c 3\n";
	    cerr << "\nWhere -type/-k <Type of the Traffic> is an integer to indicate the type of this traffic."
			<<" 0 is tdm, 1 is wdm and 2 is ofdm.\n";
		cerr << "\nWhere -topo/-t <Name of the Topology File> is a string to describe the file of the input topology.\n";
		cerr << "\nWhere -rnum/-r <Number of Traffic> is a integer to indicate the number of the traffic.\n";
		cerr << "\nWhere -traffic/-s <Name of the Traffic File> is a string to describe the file of the input ttaffic.\n";
		cerr << "\nWhere -cnum/-c <Number of Cores> is a integer to indicate the number of cores the traffic will be used.\n";
		cerr << "\nThe arguments above must be enough and not too much, or the program will be exit with error code -1." << endl;
	}

	int TopologyandTrafficinput(int TypeFlag, string TopoFileName, int NumberofTraffic, string TrafficFileName,  
		VONETopo::Topo& Substrate_Network, vector<VONETopo::Topo>& Virtual_Network_List)
	{
		//Topo input:
		int ErrorFlag = 0; //There are no errors.
		ifstream topoinput;
		topoinput.open(TopoFileName.c_str());
		if (topoinput.fail())
		{
			cerr << "Topo input file opened failed.\n";
			ErrorFlag = -1;
		}

		topoinput >> Substrate_Network;
		topoinput.close();

		//Traffic input:
		int Source, Destination, Type, Slot;
		ifstream trafficinput;
		trafficinput.open(TrafficFileName.c_str());
		if (trafficinput.fail())
		{
			cerr << "Traffic input file opened failed.\n";
			ErrorFlag = -2;
		}

		int Num = 0;
		while (trafficinput >> Source >> Destination >> Type >> Slot)
		{
			if (Num == NumberofTraffic)
				break;
			else
			{
				if (TypeFlag != Type)
				{
					cerr << "Traffic file does not fit its type!\n";
					ErrorFlag = 1;
					break;
				}
				else
				{
					VONETopo::Topo VN_tmp;
					VN_tmp.AddVertex(1, Source);
					VN_tmp.AddVertex(2, Destination);
					VN_tmp.AddEdge(1, 2, Slot);
					Virtual_Network_List.push_back(VN_tmp);
				}
				Num = Num + 1;
			}
		}
		trafficinput.close();

		return ErrorFlag;
	}
}
