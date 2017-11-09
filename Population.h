#pragma once
#include "NeuralNet.h"
#include "Data.h"
#include "GeneticAlgoritm.h"
#include "MyRand.h"
#include "OptimizationFunction.h"
#include <string>
#include <fstream>
#include "RWNet.h"
std::vector<Neuron *>::iterator ContainsByIDNeuron(std::vector<Neuron *>::iterator first, std::vector<Neuron *>::iterator last, unsigned ID);
class Population
{
public:
	Data*								DATA;
	std::vector<Connect*>				UniqueGen;
	std::vector<std::vector<int>>		Groups;
	NeuralNet*							Best;
	NeuralNet**							NN;
	NeuralNet**							NextGeneration;
	unsigned						    COUNT_NN;	
	int									INNOVATION_ID;
	int									COUNT_INPUT, COUNT_OUTPUT;
	int									MAX_NO_CHANGE = 5;
	int									COUNT_GENERATION;
	unsigned							PARAM_MUTATION; //1:
	double								PDelta = 1. / 20;
	double								PMutLow;
	double								PMutHigh;
	double								PMutMedium;
	double								PSelTour;
	double								PSelRange;

	std::vector<int>		NumIndSelRange;
	std::vector<int>		NumIndSelTour;
	std::vector<int>		NumIndMutHigh;
	std::vector<int>		NumIndMutLow;
	std::vector<int>		NumIndMutMedium;
	void					Self_ConfigurationSel();
	void					Self_ConfigurationMut();
	unsigned							PARAM_CROSS=0;
	unsigned							NUM_TRY_ADDNEURON = 5;
	void								Niching();
	void							    CheckGen();
	NeuralNet*							Cross(NeuralNet* Left,NeuralNet* Right);
	std::vector<Connect*>::iterator		HaveConnection(unsigned input, unsigned output, bool type);
	std::vector<Connect*>::iterator		HaveConnection(unsigned input, unsigned output, bool type, NeuralNet * nn);
	bool CheckConnection(unsigned input, unsigned output, const NeuralNet * nn);

	void								Mutation(NeuralNet* nn);
	void								MutationAll(NeuralNet** arr);
	void								Selection();
	std::vector<int>					SelectionInGroups(std::vector<int> part, int count_ind);
	void								TrainNet();
	void								GTNextGeneration();
	void								SelectBest();
	void								PrintBestValue();
	//void GetFitness(NeuralNet ** arr);
	double								Bestvalue;
	void								GetFitness();
	void								DeleteNNs(NeuralNet** nn);
	Population(unsigned cInd, unsigned cIn, unsigned cOut,unsigned paramMutation,int count_gnrtn);
	~Population();
	//void WriteBestSolutionInFile(int numbgeneration);
private:
	std::vector<Connect*>::iterator ContainsByLR(std::vector<Connect*>::iterator first, std::vector<Connect*>::iterator last, const Connect * c);
	int WinnerTournament(std::vector<int> Pool);
	std::vector<int> GnomeSortAndMaxFitness(std::vector<int> A, int N);
	//std::vector<int> GnomeSort(std::vector<int> A, int N);
};

