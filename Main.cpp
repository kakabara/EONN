#include "Population.h"
#include <iostream>
#include <ctime>
#include <cmath>
using namespace std;

int main()
{
	setlocale(0,NULL);
	srand(time(NULL));
	int numInput, NumOutput, gener, ind;
	//string namefile;
	/*
	cout << "NumINput" << endl;
	cin >> numInput;
	cout << "NumOutput" << endl;
	cin >> NumOutput;*/
	cout << "Num generation" << endl;
	cin >> gener;
	cout << "numind" << endl;
	cin >> ind;/*
	cout << "namefile" << endl;
	string namefile;
	cin >> namefile;*/
	numInput = 11;
	NumOutput = 1;
	/*gener = 10;
	ind = 10;*/
	string namefile = "1.csv";
	Data* data = new Data(namefile,numInput,NumOutput,0.7,0.15,0.15);
	RWNet* rw = new RWNet();
	Population *p = new Population(ind, numInput, NumOutput, 2,gener);
	p->DATA =new Data(*data);
	p->MutationAll(p->NN);
	p->TrainNet();
	p->GetFitness();
//	p->Self_ConfigurationMut();
	
	p->SelectBest();
	
	double valid;
	cout << "Best on Valid: " << OptimizeFunction1(data, p->Best, 3) << endl;
	data->SetToZeroTTV();
	rw->WriteNetToFile(-1, p->Best);

	for (int i = 0; i < p->COUNT_GENERATION; i++)
	{
		cout << "NUmber evolution:" <<i<< endl;
		p->Selection();
		p->MutationAll(p->NextGeneration);
		p->GTNextGeneration();
		p->TrainNet();
		p->GetFitness();
	//	p->Self_ConfigurationMut();
		//p->Self_ConfigurationSel();
		p->SelectBest();
		rw->WriteNetToFile(i, p->Best);
		valid = OptimizeFunction1(data, p->Best, 3);
		data->SetToZeroTTV();
		rw->EVOLUTION_FILE_WRITE << valid << endl;
		cout << "Best on Valid: " << valid << endl;
		p->PrintBestValue();
	}
	rw->EVOLUTION_FILE_WRITE.close();
	system("pause");
	return 0;
}