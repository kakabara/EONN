#pragma once
#include <fstream>
#include <string>
#include "Population.h"

class RWNet
{
public:
	std::ifstream EVOLUTION_FILE_READ;
	std::ofstream EVOLUTION_FILE_WRITE;
	std::string filename;
	void WriteNetToFile(int NumbGeneration,NeuralNet* Net);
	void OpenFile(std::string NameFile);
	NeuralNet * ReadNetFromFile(std::string filname);
	RWNet();
	~RWNet();
};

