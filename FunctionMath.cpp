#include <cmath>
double threshold(double x)
{
	if (x < 0) return 0;
	else return 1;
}

double signum(double x)
{
	if (x > 0) return 1;
	else return -1;
}

double sigmoid(double x)
{
	return 1. / (1 + exp(x));
}

double semilinearly(double x)
{
	if (x > 0) return x;
	else return 0;
}

double linearly(double x)
{
	return x;
}

double RB(double x)
{
	return exp(-pow(x, 2));

}

double semilinearWithSaturation(double x)
{
	if (x <= 0) return 0;
	else
	{
		if (x >= 1) return 1;
		else return x;
	}
}

double linearWithSaturation(double x)
{
	if (x <= -1) return -1;
	else
	{
		if (x >= 1) return 1;
		else return x;
	}
}

double hTan(double x)
{
	return tanh(x);
}

double triangular(double x)
{
	if (abs(x) > 0) return 0;
	else return (1. - abs(x));
}