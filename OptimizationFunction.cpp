#include "OptimizationFunction.h"
#include <iostream>
using namespace std;
double sgn(double x)
{
	return (x > 0) - (x < 0);
}

double OptimizeFunction1(Data* data, NeuralNet* net,int param)
{
	data->SetToZeroTTV();
	double K0 = 100;
	double K = K0;
	double sum = 0;
	double ans;
	double c;
	int n,n1=0;
	switch (param)
	{
	case 1:
		n = data->COUNT_TRAIN_EXAMPLE;
		data->NextExampleTrain();
		break;
	case 2:
		n = data->COUNT_TEST_EXAMPLE;
		data->NextExampleTest();
		break;
	case 3:
		n = data->COUNT_VALID_EXAMPLE;
		data->NextExampleValid();
		break;
	}
	for (int i = 0; i < n; i++)
	{
		net->SetInputNeuron(data->InputVector);
		net->GetAnswer();
		ans = net->AnswerVector[0];
		c = 0;
		for (int j = 0; j < data->OutputVector.size(); j++)
			c += data->OutputVector[j];
	//	if (param == 3) {
	//	if (n!=3)	if (fabs(ans) <= 0.1) ans = 0;
	//		if (c != 0 && ans != 0)
	//		{
	//			if (c == sgn(ans))
	//				sum++;
	//			n1++;
	//		}
		//}
	//	else
	//	{
			if (c != sgn(ans))
				sum++;

	//	}

		if (i + 1 < n) 
		{
			switch (param)
			{
			case 1:
				data->NextExampleTrain();
				break;
			case 2:
				data->NextExampleTest();
				break;
			case 3:
				data->NextExampleValid();
				break;
			}
		}
	//	cout << i << endl;
	}
	//cout << log(sum) << endl;
	//if (param == 3) {
	//	if (n1 == 0) return 0;
	//	return sum / n1*100.;
//	}
	//else
//	{
		return 1 / (sum+1);
//	}

}