#pragma once


#include <map>
#include <cmath>
#include "FunctionMath.h"

typedef double(*AMathFunction)(double x);



#include "Genome.h"
class NeuralNet
{
public:
	unsigned							COUNT_ACF;
	unsigned							COUNT_INPUT;
	unsigned							COUNT_OUTPUT;
	int									NoneOld;
	int									size;
	std::vector<int>					ID_INPUT_NEURON;
	std::vector<int>					ID_OUTPUT_NEURON;
	std::vector<int>					ID_HIDDEN_NEURON;
	std::vector<double>					AnswerVector;
	//std::vector<int>					BadNeuron;
	double								Fitness;
	double								OldFitness;
	Genome*								GENOTYPE;
	unsigned							CountNeuron;
	std::map<int, std::vector<int>>		HiddenLayers;
	std::map<int, int>					PairNeuronLayers;

	std::map<int,double>				MeanNeuron;
	std::map<int,double>				PreviusMeanNeuron;
	std::map<int,std::map<int,double>>	Weight;
	void								SetOutputInputHiddenNeuron();
	void								SetLayers();
	void								CreateMeanNeuronArray();	//создание массивов значения нейронов
	void ZerosMeanNeuron();
	void								RefreshMeanNeuron();		//обновление состояние нейронов для нового предсказания
	void								SetInputNeuron(std::vector<double> InputArray);
	std::map<unsigned,std::vector<int>> VectorDependence;
	void								GetVectorDependence();
	void								CreateWeightArray();
	void								GetAnswer();
	void								SetAnswerVector();
	AMathFunction MathFunctions[10] = { threshold, signum, sigmoid, semilinearly, linearly, RB, semilinearWithSaturation, linearWithSaturation, hTan, triangular };
	NeuralNet(unsigned countIn,unsigned countOut);
	NeuralNet(NeuralNet* Copy);
	NeuralNet();
	~NeuralNet();
};

std::vector<Neuron *>::iterator ContainsByIDNeuron(std::vector<Neuron *>::iterator first, std::vector<Neuron *>::iterator last, unsigned ID);