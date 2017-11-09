#include "GeneticAlgoritm.h"
#include <cmath>
#include <vector>
#include <iostream>
using namespace std;
#include <map>


template<class T>
void GnomeSort(T *A, int N) {
	int i = 0;
	while (i < N) {
		if (i == 0 || A[i - 1][0] >= A[i][0]) ++i;
		else {
			T Temp = A[i];
			A[i] = A[i - 1];
			A[i - 1] = Temp;
			--i;
		}
	}
}

void AverageProbability(double* p, int a, int b)
{
	double s = 0;
	for (int i = b; b >= a; i--)
		s += p[i];
	s = s / (b - a);
	for (int i = b; b >= a; i--)
		p[i] = s;

}


GeneticAlgoritm::GeneticAlgoritm(unsigned n, double a, double b, double eps, unsigned ind, 
								 unsigned count_generation,int prmS,int prmM,int prmC, Data* data, NeuralNet* net)
{
	DATA = new Data(*data);
	NNet = net;
	COUNT_GENERATION = count_generation;
	A = a;
	B = b;
	COUNT_IND = ind;
	COUNT_COORDINATE = n;
	COUNT_BIT = (log(abs(b - a) / eps) / log(2) + 1);
	COUNT_GEN = COUNT_BIT*COUNT_COORDINATE;
	ParamCrossing = prmC;
	ParamMutation = prmM;
	ParamSelection = prmS;
	Best = new double[COUNT_COORDINATE];
	Fitness = new double[COUNT_IND];
	//IndexPoolParents = new int[2 * COUNT_IND];
	Genes = new bool*[COUNT_IND];
	Coordinate = new double*[COUNT_IND];
	Children = new bool*[COUNT_IND*2];
	HelArr = new double[COUNT_COORDINATE];
	PSelTour = 1. / 2;
	PSelRange = 1. / 2;
	PMutHigh = 1. / 3;
	PMutMedium = 1. / 3;
	PMutLow = 1. / 3;
	PCross1 = 1. / 3;
	PCross2 = 1. / 3;
	PCross3 = 1. / 3;
	for (int i = 0; i < COUNT_IND; i++) {

		Genes[i] = new bool[COUNT_GEN];
		Children[i*2] = new bool[COUNT_GEN];
		Children[i*2+1] = new bool[COUNT_GEN];
		Coordinate[i] = new double[COUNT_COORDINATE];
	}
}
//�������� ����� ������� ������ evolution
void GeneticAlgoritm::SetWeight()
{
	Weight.clear();
	for (int i = 0; i < COUNT_COORDINATE; i++)
	{
		Weight.push_back(Best[i]);
	}
}



GeneticAlgoritm::~GeneticAlgoritm()
{
	for (int i = 0; i < COUNT_IND; i++)
	{
		delete Genes[i];
		delete Children[i * 2 + 1];
		delete Children[i * 2];
		delete Coordinate[i];
	}
	delete Genes;
	delete Children;
	delete Coordinate;
	delete Best;
	delete Fitness;
	delete HelArr;
}



void GeneticAlgoritm::Self_ConfigurationSel()
{
	if (PSelRange > PDelta && PSelTour > PDelta)
	{
		double ftour = 0, frange = 0;
		for (int i = 0; i < NumIndSelRange.size(); i++)
			frange += Fitness[NumIndSelRange[i]];
		for (int i = 0; i < NumIndSelTour.size(); i++)
			ftour += Fitness[NumIndSelTour[i]];
		frange = frange / NumIndSelRange.size();
		ftour = ftour / NumIndSelTour.size();
		if (ftour != frange)
		{
			if (ftour > frange)
			{
				PSelRange -= (2. / COUNT_GENERATION / 2);
				PSelTour += (2. / COUNT_GENERATION / 2);
			}
			else
			{
				PSelTour -= (2. / COUNT_GENERATION / 2);
				PSelRange += (2. / COUNT_GENERATION / 2);
			}
		}
	}
	
}

void GeneticAlgoritm::Self_ConfigurationMut()
{
	if (PMutLow > PDelta && PMutMedium > PDelta && PMutHigh > PDelta)
	{
		double flow = 0, fmedium = 0, fhigh = 0;
		for (int i = 0; i < NumIndMutLow.size(); i++)
			flow += Fitness[NumIndMutLow[i]];
		for (int i = 0; i < NumIndMutMedium.size(); i++)
			fmedium += Fitness[NumIndMutMedium[i]];
		for (int i = 0; i < NumIndMutHigh.size(); i++)
			fhigh += Fitness[NumIndMutHigh[i]];
		flow =flow / NumIndMutLow.size();
		fmedium =fmedium / NumIndMutMedium.size();
		fhigh = fhigh / NumIndMutHigh.size();
		double maxf = flow;
		if (fmedium > maxf) maxf = fmedium;
		if (fhigh > maxf) maxf = fhigh;
		if (flow != fmedium && fmedium != fhigh)
		{
			if (maxf == flow && maxf == fmedium)
			{
				PMutLow += (2. / COUNT_GENERATION / 3 / 2);
				PMutMedium += (2. / COUNT_GENERATION / 3 / 2);
				PMutHigh -= (2. / COUNT_GENERATION / 3);
			}
			else
				if (maxf == flow && maxf == fhigh)
				{
					PMutLow += (2. / COUNT_GENERATION / 3 / 2);
					PMutMedium -= (2. / COUNT_GENERATION / 3);
					PMutHigh += (2. / COUNT_GENERATION / 3 / 2);
				}
				else
					if (maxf == fhigh && maxf == fmedium)
					{
						PMutLow -= (2. / COUNT_GENERATION / 3);
						PMutMedium += (2. / COUNT_GENERATION / 3 / 2);
						PMutHigh += (2. / COUNT_GENERATION / 3 / 2);
					}
					else
					{
						if (maxf == flow)
						{
							PMutLow += (2. / COUNT_GENERATION / 3);
							PMutMedium -= (2. / COUNT_GENERATION / 3 / 2);
							PMutHigh -= (2. / COUNT_GENERATION / 3 / 2);
						}
						if (maxf == fmedium)
						{
							PMutLow -= (2. / COUNT_GENERATION / 3 / 2);
							PMutMedium += (2. / COUNT_GENERATION / 3);
							PMutHigh -= (2. / COUNT_GENERATION / 3 / 2);
						}
						if (maxf == fhigh)
						{
							PMutLow -= (2. / COUNT_GENERATION / 3 / 2);
							PMutMedium -= (2. / COUNT_GENERATION / 3 / 2);
							PMutHigh += (2. / COUNT_GENERATION / 3);
						}
					}
		}
	}
}

void GeneticAlgoritm::Self_ConfigurationCross()
{
	if (PCross1 > PDelta && PCross2 > PDelta && PCross3 > PDelta)
	{
		double fcross1 = 0, fcross2 = 0, fcross3 = 0;
		for (int i = 0; i < NumIndCross1.size(); i++)
			fcross1 += Fitness[NumIndCross1[i]];
		for (int i = 0; i < NumIndCross2.size(); i++)
			fcross2 += Fitness[NumIndCross2[i]];
		for (int i = 0; i < NumIndCross3.size(); i++)
			fcross3 += Fitness[NumIndCross3[i]];
		fcross1 =fcross1 / NumIndCross1.size();
		fcross2 =fcross2 / NumIndCross2.size();
		fcross3 =fcross3 / NumIndCross3.size();
		double maxf = fcross1;
		if (fcross2 > maxf) maxf = fcross2;
		if (fcross3 > maxf) maxf = fcross3;
		if (fcross1 != fcross2 && fcross2 != fcross3)
		{
			if (maxf == fcross1 && maxf == fcross2)
			{
				PCross1 += (2. / COUNT_GENERATION / 3 / 2);
				PCross2 += (2. / COUNT_GENERATION / 3 / 2);
				PCross3 -= (2. / COUNT_GENERATION / 3);
			}
			else
				if (maxf == fcross1 && maxf == fcross3)
				{
					PCross1 += (2. / COUNT_GENERATION / 3 / 2);
					PCross2 -= (2. / COUNT_GENERATION / 3);
					PCross3 += (2. / COUNT_GENERATION / 3 / 2);
				}
				else
					if (maxf == fcross3 && maxf == fcross2)
					{
						PCross1 -= (2. / COUNT_GENERATION / 3);
						PCross2 += (2. / COUNT_GENERATION / 3 / 2);
						PCross3 += (2. / COUNT_GENERATION / 3 / 2);
					}
					else
					{
						if (maxf == fcross1)
						{
							PCross1 += (2. / COUNT_GENERATION / 3);
							PCross2 -= (2. / COUNT_GENERATION / 3 / 2);
							PCross3 -= (2. / COUNT_GENERATION / 3 / 2);
						}
						if (maxf == fcross2)
						{
							PCross1 -= (2. / COUNT_GENERATION / 3 / 2);
							PCross2 += (2. / COUNT_GENERATION / 3);
							PCross3 -= (2. / COUNT_GENERATION / 3 / 2);
						}
						if (maxf = fcross3)
						{
							PCross1 -= (2. / COUNT_GENERATION / 3 / 2);
							PCross2 -= (2. / COUNT_GENERATION / 3 / 2);
							PCross3 += (2. / COUNT_GENERATION / 3);
						}
					}
		}
	}

}
	

void GeneticAlgoritm::Start()
{
	for (int i = 0; i < COUNT_IND; i++)
		for (int j = 0; j < COUNT_GEN; j++)
			Genes[i][j] = (rand() % 2);
	ConvertToCoordinateArray();
	for (int i = 0; i < COUNT_COORDINATE; i++) {
		Best[i] = Coordinate[0][i];
	}
	BestValue = OptimizeFunction(DATA,NNet,0);
}

void GeneticAlgoritm::ConvertToGenes(double *x, int index)
{
	int buff;

	for (int i = 0; i < COUNT_COORDINATE; i++)
	{
		buff = (x[i] - A) / abs(A - B) / pow(2, COUNT_BIT);
		for (int j = 0; j < COUNT_BIT; j++)
		{
			Genes[index][COUNT_GEN - (i*COUNT_BIT + j)] = buff % 2;
			buff = buff / 2;
		}
	}
	//Genes[index] = a;
}

void GeneticAlgoritm::PrintBest()
{
#pragma omp critical 
	{
		cout << COUNT_COORDINATE << endl;
	for (int i = 0; i < COUNT_COORDINATE; i++)
		cout << Best[i] << " ";
	cout << endl;
	}
}

void GeneticAlgoritm::PrintCoordinate()
{
	//for (int i = 0; i < COUNT_IND; i++) {
		//cout << i << ": ";
	//	for (int j = 0; j < COUNT_COORDINATE; j++) {
		//	cout << Coordinate[i][j] << " ";
	//	}
		//cout << "F= " << Function(ArrayToVector(Coordinate[i], COUNT_COORDINATE), 2) << endl;
	//}
}

double* GeneticAlgoritm::ConvertToCoordinateInd(bool* vec)
{
	double t = abs(A - B) / pow(2, COUNT_BIT);
	for (int j = 0; j < COUNT_COORDINATE; j++)
	{
		HelArr[j] = 0;
		for (int k = 0; k < COUNT_BIT; k++)
		{
			HelArr[j] += int(vec[COUNT_BIT*j + k]) * pow(2, COUNT_BIT - k - 1.0);
		}
		HelArr[j] = A + HelArr[j] * t;
	}
	return HelArr;
}

void GeneticAlgoritm::ConvertToCoordinateArray() {
	double t = abs(A - B) / pow(2, COUNT_BIT);
	for (int i = 0; i < COUNT_IND; i++) {
		ConvertToCoordinateInd(Genes[i]);
		for (int j = 0; j < COUNT_COORDINATE; j++)
			Coordinate[i][j] = HelArr[j];
	}

}

void GeneticAlgoritm::FuncFit() {
	for (int i = 0; i < COUNT_IND; i++) 
		Fitness[i] = OptimizeFunction(DATA, NNet,1);
//		if (Fitness[i] < 0) cout << "FUUUUUTEST" << endl;
//	}
}


void GeneticAlgoritm::CopyWeightInNN(NeuralNet* net, double* arr)
{
	int i = 0;
	for (auto it1 = net->Weight.begin(); it1 != net->Weight.end(); ++it1)
	{
		for (auto it2 = it1->second.begin(); it2 != it1->second.end(); ++it2)
		{
			it2->second = arr[i];
			i++;
		}
	}
}

std::vector<int> GeneticAlgoritm::GnomeSortAndMaxFitness(std::vector<int> A, int N) {
	//std::vector<int> T = A;
	std::vector<int> R;
	int i = 0;
	while (i < N) {
		if (i == 0 || Fitness[A[i - 1]] >= Fitness[A[i]]) 
			++i;
		else {
			int Temp = A[i];
			A[i] = A[i - 1];
			A[i - 1] = Temp;
			--i;
		}
	}
	R.push_back(A[0]);
	for (int i = 1; i<N; i++)
		if (Fitness[A[i]] == Fitness[R[0]])
			R.push_back(A[i]);
	return R;
}

int GeneticAlgoritm::WinnerTournament(std::vector<int> Pool)
{
	int n = Pool.size();
	Pool = GnomeSortAndMaxFitness(Pool, n);
	n = Pool.size();
	if (n == 1) return Pool[0];
	else
	{
		return randint(0, n-1);
	}
}

void GeneticAlgoritm::SelectParent() {

	std::vector<int> Pool = std::vector<int>();
	int sizeTour = pow(COUNT_IND, 0.5);//sqrt
	if (sizeTour % 2) sizeTour++; //���� ������ �� ------ �� �������
	int rnd,ind;
	double prob;
	NumIndSelRange.clear();
	NumIndSelTour.clear();
	for (int i=0;i<COUNT_IND;i++)
	{
		prob = randfloat();

		if (prob > PSelTour) //выбрали ранговую
		{
			NumIndSelRange.push_back(i);
			double **Arr;
			Arr = new double*[COUNT_IND];
			for (int i = 0; i < COUNT_IND; i++)
			{
				Arr[i] = new double[2];
				Arr[i][0] = Fitness[i];
				Arr[i][1] = i;
			}

			GnomeSort(Arr, COUNT_IND);


			double *p = new double[COUNT_IND];
			for (int i = 0; i < COUNT_IND; i++)
				p[i] = ((i + 1)*2.0) / (COUNT_IND*COUNT_IND + COUNT_IND);   //������� ����������� ��������� ��� ����� ���������� ��������� �������
			int a, b;
			b = COUNT_IND - 1;
			bool c = false;


			for (int i = COUNT_IND - 1; i > 0; i--)    //����� ���������� ����������� ��� ���������� ����������� ��������� ������� ����������� ����� ����� ���������
			{
				if (p[i] == p[i - 1])
				{
					if (c) a--;
					else {
						c = true; b = i; a = i - 1;
					}
				}
				else
				{
					if (c) {
						AverageProbability(p, a, b);
						c = false;
					}

				}
			}

			
				double r, s;
					s = 0;
					r = randfloat();

					for (int j = 0; j < COUNT_IND; j++)
					{
						s += p[j];
						if (r < s)
						{
							for (int x = 0; x < COUNT_GEN; x++)
								Children[i*2][x] = Genes[(int)Arr[j][1]][x];
							break;
						}

					}
					s = 0;
					r = randfloat();

					for (int j = 0; j < COUNT_IND; j++)
					{
						s += p[j];
						if (r < s)
						{
							for (int x = 0; x < COUNT_GEN; x++)
								Children[i * 2+1][x] = Genes[(int)Arr[j][1]][x];
							break;
						}

					}
				
				delete p;
				for (int i = 0; i < COUNT_IND; i++)
					delete Arr[i];
				delete Arr;
			
		}
		else
		{
			NumIndSelTour.push_back(i);
			Pool.clear();
			for (int j = 0; j < sizeTour; j++)
			{
				rnd = randint(0, COUNT_IND - 1);
				if (find(Pool.begin(), Pool.end(), rnd) == Pool.end())
					Pool.push_back(rnd);
				else j--;
			}


			ind = WinnerTournament(Pool);
			for (int x = 0; x < COUNT_GEN; x++)
				Children[i*2][x] = Genes[ind][x];
			Pool.clear();
			for (int j = 0; j < sizeTour; j++)
			{
				rnd = randint(0, COUNT_IND - 1);
				if (find(Pool.begin(), Pool.end(), rnd) == Pool.end())
					Pool.push_back(rnd);
				else j--;
			}


			ind = WinnerTournament(Pool);
			for (int x = 0; x < COUNT_GEN; x++)
				Children[i*2+1][x] = Genes[ind][x];
		}
	}
}

void GeneticAlgoritm::Cros()
{

//	if (ParamCrossing == 1)
	//{
	NumIndCross1.clear();
	NumIndCross2.clear();
	NumIndCross3.clear();
	double probc,pos1,pos2,pos;
	for (int i = 0; i < COUNT_IND; i++)
	{
		probc = randfloat();
		if (probc < PCross1)
		{
			pos = randint(1,COUNT_GEN)-1;
			for (int j = pos; j < COUNT_GEN; j++)
			{
				swap(Children[i * 2 + 1][j], Children[i * 2][j]);
			}
			NumIndCross1.push_back(i);
		}
		if (probc > PCross1 && probc < (PCross1 + PCross2))
		{
			pos1 = pos2 = 0;
			while (pos1 >= pos2)
			{
				pos1 = rand() % COUNT_GEN;
				pos2 = rand() % COUNT_GEN;
			}
			for (int j = pos1; j < pos2; j++)
				swap(Children[i*2 + 1][j], Children[i*2][j]);
			NumIndCross2.push_back(i);
		}
		if (probc >(PCross1 + PCross2))
		{
			double sw;
			for (int j = 0; j < COUNT_BIT; j++)
			{
				if (rand() % 2)
				{
					sw = Children[i*2][j];
					Children[i*2][j] = Children[i*2 + 1][j];
					Children[i*2 + 1][j] = sw;
				}
			}
			NumIndCross3.push_back(i);
		}
	}
		unsigned r, sw;
		for (int i = 0; i < COUNT_IND; i += 2)
		{
			//r = rand() % COUNT_BIT;
			for (int j = 0; j < COUNT_BIT; j++)
			{
				if (rand() % 2)
				{
					sw = Children[i][j];
					Children[i][j] = Children[i + 1][j];
					Children[i + 1][j] = sw;
				}
			}
		}
	//}

	/*if (ParamCrossing == 2) {
		short unsigned int pos1, pos2;
		for (int i = 1; i < COUNT_IND; i += 2)
		{
			pos1 = pos2 = 0;
			while (pos1 >= pos2)
			{
				pos1 = rand() % COUNT_GEN;
				pos2 = rand() % COUNT_GEN;
			}
			for (int j = pos1; j < pos2; j++)
				swap(Children[i - 1][j], Children[i][j]);
		}
	}

	if (ParamCrossing == 3)
	{
		bool sw;
		for (int i = 0; i < COUNT_IND; i += 2)
		{
			for (int j = 0; j < COUNT_GEN; j++)
			{
				sw = Children[i][j];
				if (!Children[i][j]) { Children[i][j] = Children[i + 1][j]; }
				if (!Children[i + 1][j]) { Children[i + 1][j] = sw; }
			}
		}
	}*/
}

void GeneticAlgoritm::Mutation()
{
	double prob,probc;

	double rnd;
	NumIndMutHigh.clear();
	NumIndMutMedium.clear();
	NumIndMutLow.clear();
	for (int i = 0; i < COUNT_IND; i++) 
	{
		probc = randfloat();
		if (probc < PMutLow)
		{
			prob = 1.0 / (3 * COUNT_GEN); //low
			NumIndMutLow.push_back(i);
		}
		if (probc > PMutLow && probc < (PMutMedium + PMutLow))
		{
			prob = 1.0 / COUNT_GEN; //medium
			NumIndMutMedium.push_back(i);
		}
		if (probc > (PMutLow + PMutMedium))
		{
			prob = 3.0 / COUNT_GEN; //high
			NumIndMutHigh.push_back(i);
		}
		for (int j = 0; j < COUNT_GEN; j++)
		{
			rnd = randfloat();
			if (prob >= rnd) {
				Children[i][j] = !Children[i][j];
			}
		}
	}
}

void GeneticAlgoritm::SelectBest()
{
	//double ff = Function(Best, COUNT_COORDINATE);
	for (int i = 0; i < COUNT_IND; i++)
	{
		if (Fitness[i] >BestValue) {
			BestValue = Fitness[i];
			for (int j = 0; j < COUNT_COORDINATE; j++)
				Best[j] = Coordinate[i][j];
		}
		
	}

}

void GeneticAlgoritm::SelectNextGeneration()
{
	bool b;
	for (int i = 0; i < COUNT_IND; i++) {
		b = rand() % 2;
		if (b)
		{
			for (int j = 0; j < COUNT_GEN; j++)
				Genes[i][j] = Children[i * 2][j];
		}
		else
		{
			for (int j = 0; j < COUNT_GEN; j++)
				Genes[i][j] = Children[i * 2 + 1][j];
		}
	}
}

void GeneticAlgoritm::GoToNextGeneration()
{

	for (int i = 0; i < COUNT_IND; i++) {
		
		Fitness[i] = OptimizeFunction(DATA, NNet,i);
		for (int j = 0; j < COUNT_GEN; j++)
			Genes[i][j] = Children[i][j];
		
	}

}


void GeneticAlgoritm::CopyWeightToNet(vector<double> W)
{
	
	int i = 0;
	for (auto it1 = NNet->Weight.begin(); it1 != NNet->Weight.end(); ++it1)
	{
		for (auto it2 =it1->second.begin(); it2 != it1->second.end(); ++it2)
		{
			it2->second = W[i];
			i++;
		}
	}
}


double GeneticAlgoritm::OptimizeFunction(Data* data,NeuralNet* net,int n)
{
		//net->CreateWeightArray();
	//cout << omp_get_thread_num() << endl;
		CopyWeightInNN(net,Coordinate[n]);
		return OptimizeFunction1(data,net,1);
}

double GeneticAlgoritm::GetBestValue()
{
	return BestValue;
}

void GeneticAlgoritm::Evolution()
{
	//cout<<omp_get_thread_num()<<endl;
	Start();
	FuncFit();
	Self_ConfigurationCross();
	Self_ConfigurationMut();
	Self_ConfigurationSel();
	SelectBest();
	for (int i = 0; i < COUNT_GENERATION; i++)
	{
		SelectParent();
		Cros();
		SelectNextGeneration();
		Mutation();
		ConvertToCoordinateArray();
		FuncFit();
	//	Self_ConfigurationCross();
	//		Self_ConfigurationMut();
	//		Self_ConfigurationSel();
		SelectBest();	
	}
	//cout << "Thread#" << omp_get_thread_num() << "Fitnes BEst=" << BestValue << endl;
}