#include "RWNet.h"
#include <ctime>
#include <sstream>
using namespace std;


void RWNet::WriteNetToFile(int NumbGeneration, NeuralNet * Net)
{
	Neuron* s;
	EVOLUTION_FILE_WRITE << "Generation ¹" << NumbGeneration << endl<<"Fit="<<Net->Fitness<<endl;
	EVOLUTION_FILE_WRITE << "NODE:" << endl;
	for (int i = 0; i < Net->GENOTYPE->Node.size(); i++)
	{
	
		int ind;
		if (Net->GENOTYPE->Node[i]->TYPE[0]) ind = 0;
		if (Net->GENOTYPE->Node[i]->TYPE[1]) ind = 1;
		if (Net->GENOTYPE->Node[i]->TYPE[2]) ind = 2;
		EVOLUTION_FILE_WRITE <<Net->GENOTYPE->Node[i]->ID << ","<<ind<<","<< Net->GENOTYPE->Node[i]->ActivationFunction  << ","<<endl;
	}
	EVOLUTION_FILE_WRITE << "ENDNODE" << endl<<"GEN:"<<endl;
	for (int i=0;i<Net->GENOTYPE->Gen.size();i++)
	{
		EVOLUTION_FILE_WRITE << Net->GENOTYPE->Gen[i]->INOVATION_ID << "," << Net->GENOTYPE->Gen[i]->LEFT_NEURON_ID << "," << Net->GENOTYPE->Gen[i]->RIGHT_NEURON_ID << "," << Net->GENOTYPE->Gen[i]->TYPE << "," << Net->GENOTYPE->Gen[i]->ENABLE<<",";
		if (Net->GENOTYPE->Gen[i]->ENABLE)
		{
			if (!Net->GENOTYPE->Gen[i]->TYPE)
			{
				EVOLUTION_FILE_WRITE << Net->Weight[Net->GENOTYPE->Gen[i]->RIGHT_NEURON_ID][Net->GENOTYPE->Gen[i]->LEFT_NEURON_ID];
			}
			else
			{
				EVOLUTION_FILE_WRITE << Net->Weight[Net->GENOTYPE->Gen[i]->RIGHT_NEURON_ID][-1*Net->GENOTYPE->Gen[i]->LEFT_NEURON_ID];
			}
		}
		EVOLUTION_FILE_WRITE << endl;
	}
	
	EVOLUTION_FILE_WRITE << "ENDGen" << endl;
}

void RWNet::OpenFile(std::string NameFile)
{
	
}

NeuralNet* RWNet::ReadNetFromFile(std::string filename)
{
	EVOLUTION_FILE_READ.open(filename);
	NeuralNet* nwNN=new NeuralNet();
	string line;
	getline(EVOLUTION_FILE_READ, line);
	getline(EVOLUTION_FILE_READ, line);
	if (line == "NODE:")
	{
		int ID, type, AcF;
		while (getline(EVOLUTION_FILE_READ, line))
		{
			if (line == "ENDNODE") break;
			string line_value;
			stringstream ss(line);
			getline(ss, line_value, ',');
			ID = stoi(line_value);
			getline(ss, line_value, ',');
			type = stoi(line_value);
			getline(ss, line_value, ',');
			AcF = stoi(line_value);
		}
	}
	getline(EVOLUTION_FILE_READ, line);
	if (line == "GEN:")
	{
		int W, NL, NR;
		while (getline(EVOLUTION_FILE_READ, line))
		{
			if (line == "ENDGEN") break;
			int leftN, rightN, ID, type, enable,w;
			string line_value;
			stringstream ss(line);
			getline(ss,line_value,',');
			ID = stoi(line_value);
			getline(ss, line_value, ',');
			leftN = stoi(line_value);
			getline(ss, line_value, ',');
			rightN = stoi(line_value);
			getline(ss, line_value, ',');
			type = stoi(line_value);
			getline(ss, line_value, ',');
			enable = stoi(line_value);
			getline(ss, line_value, ',');
			w = stod(line_value);
			if (!type)
				nwNN->Weight[rightN][leftN] = w;
			else
				nwNN->Weight[rightN][-1*leftN] = w;
		}
	}
	return nwNN;
}

RWNet::RWNet()
{
	setlocale(LC_ALL, "");
	time_t seconds = time(NULL);
	tm* timeinfo = new tm();
	localtime_s(timeinfo, &seconds);
	std::string namefile = std::to_string(timeinfo->tm_year + 1900) + "_" + std::to_string(timeinfo->tm_mon + 1) + "_" +
		std::to_string(timeinfo->tm_mday) + "_" +
		std::to_string(timeinfo->tm_hour) + "_" + std::to_string(timeinfo->tm_min) + "_" +
		std::to_string(timeinfo->tm_sec) + "_" + ".txt";
	EVOLUTION_FILE_WRITE.open(namefile);
}


RWNet::~RWNet()
{
}
