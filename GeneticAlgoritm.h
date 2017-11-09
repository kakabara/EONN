#pragma once
#include <vector>
#include "Data.h"
#include "NeuralNet.h"
#include "MyRand.h"
#include "OptimizationFunction.h"
#include <omp.h> 
class GeneticAlgoritm
{
public:
	GeneticAlgoritm(unsigned n,double a,double b,double eps,unsigned ind,unsigned countGeneration,int prmS, int prmM, int prmC, Data* data, NeuralNet* net);
	void SetWeight();
	~GeneticAlgoritm();
	void					CopyWeightInNN(NeuralNet * net, double * arr);
	std::vector<int>		GnomeSortAndMaxFitness(std::vector<int> A, int N);
	int						WinnerTournament(std::vector<int> Pool);
	void					CopyWeightToNet(std::vector<double> W);
	double*					Best;
	//double					OptimizeFunction(Data * data, NeuralNet * net);
	std::vector<double>		Weight;
	double					OptimizeFunction(Data * data, NeuralNet * net, int n);
	double					GetBestValue();
	void					Evolution();
protected:
	Data*					DATA;
	NeuralNet*				NNet;
	bool**					Genes;
	bool**					Children;
	double					*Fitness;
	double					*HelArr;
	double					BestValue;
	double					PDelta = 1. / 20;
	double					PMutLow;
	double					PMutHigh;
	double					PMutMedium;
	double					PCross1;
	double					PCross2;
	double					PCross3;
	double					PSelTour;
	double					PSelRange;
	std::vector<int>		NumIndSelRange;
	std::vector<int>		NumIndSelTour;
	std::vector<int>		NumIndMutHigh;
	std::vector<int>		NumIndMutLow;
	std::vector<int>		NumIndMutMedium;
	std::vector<int>		NumIndCross1;
	std::vector<int>		NumIndCross2;
	std::vector<int>		NumIndCross3;
	void					Self_ConfigurationSel();
	void					Self_ConfigurationMut();
	void					Self_ConfigurationCross();
	double**				Coordinate;
	int						*IndexPoolParents;
	int						COUNT_GENERATION;
	int						ParamSelection;
	int						ParamMutation;
	int						ParamCrossing;
	
private:
	unsigned				COUNT_COORDINATE;
	unsigned				COUNT_BIT;
	unsigned				COUNT_GEN;
	unsigned				COUNT_IND;
	double					A, B;
	//vector<int> SelectPartPopulation();
	void Start();
	void ConvertToGenes(double * x, int index);
	void PrintBest();
	void PrintCoordinate();
	double *ConvertToCoordinateInd(bool * vec);
	void ConvertToCoordinateArray();
	void FuncFit();
	
	void SelectParent();
	void Cros();
	void Mutation();
	void SelectBest();
	void SelectNextGeneration();
	void GoToNextGeneration();
	
	
	
};

