#include "Data.h"


#include <sstream>






void Data::ReadData(std::string NameFile)
{
	std::ifstream datafile(NameFile);
	std::string line;
	while (std::getline(datafile, line))
	{
		std::string line_value;
		std::vector<double> line_values;
		std::stringstream ss(line);
		while (std::getline(ss, line_value, ';'))
		{
			line_values.push_back(std::stod(line_value));
		}
		ArrayData.emplace_back(line_values);
	}
}

void Data::SetToZeroTTV()
{
	NUM_TRAIN_EXAMPLE = 0;
	NUM_TEST_EXAMPLE = 0;
	NUM_VALID_EXAMPLE = 0;
}



void Data::NextExampleTrain()
{
	if (NUM_TRAIN_EXAMPLE < COUNT_TRAIN_EXAMPLE) {
		InputVector.clear();
		OutputVector.clear();
		for (int i = 0; i < COUNT_INPUT; i++)
			InputVector.push_back(ArrayData[NUM_TRAIN_EXAMPLE][i]);
		for (int i = COUNT_INPUT; i < COUNT_INPUT + COUNT_OUTPUT; i++)
			OutputVector.push_back(ArrayData[NUM_TRAIN_EXAMPLE][i]);
		NUM_TRAIN_EXAMPLE++;
	}
}

void Data::NextExampleTest()
{
	if (NUM_TEST_EXAMPLE < COUNT_TEST_EXAMPLE) {
		InputVector.clear();
		OutputVector.clear();
		for (int i = 0; i < COUNT_INPUT; i++)
			InputVector.push_back(ArrayData[COUNT_TRAIN_EXAMPLE+NUM_TEST_EXAMPLE][i]);
		for (int i = COUNT_INPUT; i < COUNT_INPUT + COUNT_OUTPUT; i++)
			OutputVector.push_back(ArrayData[COUNT_TRAIN_EXAMPLE + NUM_TEST_EXAMPLE][i]);
		NUM_TEST_EXAMPLE++;
	}
}

void Data::NextExampleValid()
{
	if (NUM_VALID_EXAMPLE < COUNT_VALID_EXAMPLE) {
		if (InputVector.size()>0)
		InputVector.clear();
		if (OutputVector.size()>0)
		OutputVector.clear();
		for (int i = 0; i < COUNT_INPUT; i++)
			InputVector.push_back(ArrayData[COUNT_TRAIN_EXAMPLE + COUNT_TEST_EXAMPLE + NUM_VALID_EXAMPLE][i]);
		for (int i = COUNT_INPUT; i < COUNT_INPUT + COUNT_OUTPUT; i++)
			OutputVector.push_back(ArrayData[COUNT_TRAIN_EXAMPLE + COUNT_TEST_EXAMPLE + NUM_VALID_EXAMPLE][i]);
		NUM_VALID_EXAMPLE++;
	}
}


Data::Data(std::string NameFile, unsigned input, unsigned output, double ptrain, double ptest, double pvalid)
{
	ReadData(NameFile);
	COUNT_INPUT=input;
	COUNT_OUTPUT=output;
	PERCENT_TRAIN = ptrain;
	PERCENT_TEST = ptest;
	PERCENT_VALID = pvalid;
	COUNT_EXAMPLE = ArrayData.size();
	COUNT_TRAIN_EXAMPLE=floor(PERCENT_TRAIN*COUNT_EXAMPLE);
	COUNT_TEST_EXAMPLE=floor(PERCENT_TEST*COUNT_EXAMPLE);
	COUNT_VALID_EXAMPLE=ceil(PERCENT_VALID*COUNT_EXAMPLE);
	InputVector = std::vector<double>();
	OutputVector = std::vector<double>();
	NUM_TEST_EXAMPLE=0;
	NUM_TRAIN_EXAMPLE=0;
	NUM_VALID_EXAMPLE=0;
	
}

Data::~Data()
{
}
