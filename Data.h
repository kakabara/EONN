#pragma once
#include <vector>
#include <string.h>
#include <fstream>
class Data
{
public:
	std::vector<std::vector<double>> ArrayData;
	std::vector<double> InputVector;
	std::vector<double> OutputVector;
	void ReadData(std::string NameFile);
	void SetToZeroTTV();
	void NextExampleTrain();
	void NextExampleTest();
	void NextExampleValid();
	int COUNT_INPUT;
	int COUNT_OUTPUT;
	int COUNT_EXAMPLE;
	int COUNT_TRAIN_EXAMPLE;
	int COUNT_TEST_EXAMPLE;
	int COUNT_VALID_EXAMPLE;
	unsigned NUM_TEST_EXAMPLE;
	unsigned NUM_TRAIN_EXAMPLE;
	unsigned NUM_VALID_EXAMPLE;
	double PERCENT_TRAIN;
	double PERCENT_TEST;
	double PERCENT_VALID;
	Data(std::string NameFile,unsigned input,unsigned output,double ptrain,double ptest,double pvalid);
	~Data();
protected:
	
	
};

