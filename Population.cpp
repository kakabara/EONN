#include "Population.h"
#include <iterator>
#include <iostream>
#include <algorithm>
#include <omp.h> 
#include <ctime>
using namespace std;




template<class T>
void GnomeSort1(T *A, int N) {
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

void AverageProbability1(double* p, int a, int b)
{
	double s = 0;
	for (int i = b; b >= a; i--)
		s += p[i];
	s = s / (b - a);
	for (int i = b; b >= a; i--)
		p[i] = s;

}


int CountEnableGen(std::vector<Connect *> c)
{
	int a = 0;
	for (int i = 0; i < c.size(); i++)
	{
		if (c[i]->ENABLE)
		{
			a++;
		}
	}
	return a;
}
void PrintSizeNN(NeuralNet* nn)
{
	std::cout << nn->GENOTYPE->Gen.size() << std::endl;
	std::cout << nn->GENOTYPE->Node.size() << std::endl;
}
void Population::CheckGen() //Выравнивание исторических меток по ID
{

	int size = UniqueGen.size();
	INNOVATION_ID = size;
	//UniqueGen.clear();
	std::vector<Connect *>::iterator ptr;
	for (int i = 0; i < COUNT_NN; i++)
	{
		for (int j = 0; j < NN[i]->GENOTYPE->Gen.size(); j++)
		{

			ptr = ContainsByLR(UniqueGen.begin(), UniqueGen.end(), NN[i]->GENOTYPE->Gen[j]); //Совпадают ли связанные нейроны
			if (ptr != UniqueGen.end())
			{ //если совпали типы и разные ID то выравниваем по списку уникальных ID
				if ((*ptr)->INOVATION_ID != NN[i]->GENOTYPE->Gen[j]->INOVATION_ID)
				{
					NN[i]->GENOTYPE->Gen[j]->INOVATION_ID = (*ptr)->INOVATION_ID;
				}
			}
			else
			{
				if (NN[i]->GENOTYPE->Gen[j]->INOVATION_ID != -1)
				{
					auto nwg = new Connect(NN[i]->GENOTYPE->Gen[j]);
					nwg->ENABLE = true;
					UniqueGen.push_back(nwg); //Если такой связи нет, то просто добавляем ссылку
				}
				else
				{
					NN[i]->GENOTYPE->Gen[j]->INOVATION_ID = INNOVATION_ID;
					auto nwg = new Connect(NN[i]->GENOTYPE->Gen[j]);
					nwg->ENABLE = true;
					UniqueGen.push_back(nwg);
					INNOVATION_ID++;
				}
			}
		}
	}
}

std::vector<Connect *>::iterator ContainsByInovID(std::vector<Connect *>::iterator first, std::vector<Connect *>::iterator last, Connect* c)
{
	while (first != last)
	{
		if ((*first)->INOVATION_ID == c->INOVATION_ID && c->TYPE==(*first)->TYPE)
			return first;
		++first;
	}
	return last;
}

/*
int MaxInovID(std::vector<Connect*> first)
{
int max=-1;
for (int i = 0; i < first.size(); i++)
{
if (first[i]->INOVATION_ID > max) max = first[i]->INOVATION_ID;
}
return max;
}*/

int Disjoint(std::vector<Connect *> first, std::vector<Connect *> second)
{
	int d = 0;
	std::sort(first.begin(), first.end());
	std::sort(second.begin(), second.end());
	int min = ((*(--first.end()))->INOVATION_ID <= (*(--second.end()))->INOVATION_ID) ? (*(--first.end()))->INOVATION_ID : (*(--second.end()))->INOVATION_ID;
	if (min == (*(--first.end()))->INOVATION_ID) //Если минимальный элемент лежит в первом векторе то разницу считаем по нему
	{
		for (int i = 0; i < first.size(); i++)
		{
			if (ContainsByInovID(second.begin(), second.end(), first[i]) == second.end())
				d++;
		}
	}
	else
	{
		for (int i = 0; i < second.size(); i++)
		{
			if (ContainsByInovID(first.begin(), first.end(), first[i]) == first.end())
				d++;
		}
	}
	return d;
}

int Excess(std::vector<Connect *> first, std::vector<Connect *> second)
{
	int e = 0;
	int min = ((*(--first.end()))->INOVATION_ID >= (*(--second.end()))->INOVATION_ID) ? (*(--first.end()))->INOVATION_ID : (*(--second.end()))->INOVATION_ID;
	if ((*(--first.end()))->INOVATION_ID == min)
	{
		auto end = --second.end();
		auto buf = end;
		while ((*end)->INOVATION_ID > min && (++buf) != second.begin())
		{
			e++;
			--end;
			buf = end;
		}
	}
	else
	{
		auto end = --first.end();
		while ((*end)->INOVATION_ID > min)
		{
			e++;
			--end;
		}
	}
	return e;
}

double Metric(std::vector<Connect *> first, std::vector<Connect *> second)
{
	unsigned N = (first.size() >= second.size()) ? first.size() : second.size();
	return Disjoint(first, second) / (N * 1.) + Excess(first, second) / (N * 1.);
}

bool BelongFrom(Genome *frmGroup, Genome *comp, double delta)
{
	if (Metric(frmGroup->Gen, comp->Gen) > delta)
		return 0;
	return 1;
}

void Population::Self_ConfigurationSel()
{
	if (PSelRange > PDelta && PSelTour > PDelta)
	{
		double ftour = 0, frange = 0;
		for (int i = 0; i < NumIndSelRange.size(); i++)
			frange += NN[NumIndSelRange[i]]->Fitness;
		for (int i = 0; i < NumIndSelTour.size(); i++)
			ftour += NN[NumIndSelTour[i]]->Fitness;
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

void Population::Self_ConfigurationMut()
{
	if (PMutLow > PDelta && PMutMedium > PDelta && PMutHigh > PDelta)
	{
		double flow = 0, fmedium = 0, fhigh = 0;
		for (int i = 0; i < NumIndMutLow.size(); i++)
			flow += NN[NumIndMutLow[i]]->Fitness;
		for (int i = 0; i < NumIndMutMedium.size(); i++)
			fmedium += NN[NumIndMutMedium[i]]->Fitness;
		for (int i = 0; i < NumIndMutHigh.size(); i++)
			fhigh += NN[NumIndMutHigh[i]]->Fitness;
		flow = flow / NumIndMutLow.size();
		fmedium = fmedium / NumIndMutMedium.size();
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

void Population::Niching()
{
	/*bool flag = false;
	//	Groups[0][0] = 0;
	for (int i = 0; i < COUNT_NN; i++)
	{
		flag = false;
		for (int j = 0; j < Groups.size(); j++)
		{
			for (int q = 0; q < Groups[j].size(); q++)
			{
				if (Groups[j][q] != i)
				{
					if (BelongFrom(NN[Groups[j][q]]->GENOTYPE, NN[i]->GENOTYPE, 5))
					{
						flag = true;
					}
					else
					{
						flag = false;
						break;
					}
				}
				else
				{
					flag = true;
				}
			}
		}
		if (!flag)
			Groups.push_back(std::vector<int>(1, i));
	}*/
	bool flag=false;
	for (int i = 0; i < COUNT_NN; i++)
	{
		for (auto itGroups = Groups.begin(); itGroups != Groups.end(); ++itGroups)
		{
			flag = true;
			//пока не дошли до конца или не нашли промаха
			for (auto itGroupsI = (*itGroups).begin(); itGroupsI != (*itGroups).end() && flag; ++itGroupsI) 
			{
				if (!BelongFrom(NN[*itGroupsI]->GENOTYPE, NN[i]->GENOTYPE, 5))
				{
					flag = false;
				}
			}
			(*itGroups).push_back(i);
		}
		if (!flag) Groups.push_back(std::vector<int>(1, i));
	}
}

NeuralNet *Population::Cross(NeuralNet *Left, NeuralNet *Right)
{
	NeuralNet *nn = new NeuralNet();
	//копирование генов
	std::vector<Connect *>::iterator it;
	std::vector<Neuron *>::iterator itN;
	if (Left->Fitness > Right->Fitness)
	{
		//копирование нейронов
		for (int i = 0; i < Left->GENOTYPE->Node.size(); i++)
		{
			itN = ContainsByIDNeuron(Right->GENOTYPE->Node.begin(), Right->GENOTYPE->Node.end(), Left->GENOTYPE->Node[i]->ID);
			if (itN != Right->GENOTYPE->Node.end())
			{
				if (rand() % 2)
				{
					nn->GENOTYPE->Node.push_back(new Neuron(Left->GENOTYPE->Node[i]));
				}
				else
				{
					nn->GENOTYPE->Node.push_back(new Neuron(*itN));
				}
			}
			else
			{
				nn->GENOTYPE->Node.push_back(new Neuron(Left->GENOTYPE->Node[i]));
			}
			nn->GENOTYPE->COUNT_N_ID = Left->GENOTYPE->COUNT_N_ID;
		}
		//просто копируем гены перовго потомка
		for (int i = 0; i < Left->GENOTYPE->Gen.size(); i++)
		{
			it = ContainsByLR(Right->GENOTYPE->Gen.begin(), Right->GENOTYPE->Gen.end(), Left->GENOTYPE->Gen[i]);
			if (it != Right->GENOTYPE->Gen.end()) //если ген совпадает
			{

				if (rand() % 2) //если такой ген есть в обоих то наугад
				{
					nn->GENOTYPE->Gen.push_back(new Connect(Left->GENOTYPE->Gen[i]));
				}
				else
					nn->GENOTYPE->Gen.push_back(new Connect((*it)));
			}
			else
				nn->GENOTYPE->Gen.push_back(new Connect(Left->GENOTYPE->Gen[i]));
		}
		
	}

	if (Left->Fitness < Right->Fitness)
	{
		//коипируем элементы из второго
		//копирование нейронов
		for (int i = 0; i < Right->GENOTYPE->Node.size(); i++)
		{
			itN = ContainsByIDNeuron(Left->GENOTYPE->Node.begin(), Left->GENOTYPE->Node.end(), Right->GENOTYPE->Node[i]->ID);
			if (itN != Left->GENOTYPE->Node.end())
			{
				if (rand() % 2)
				{
					nn->GENOTYPE->Node.push_back(new Neuron(Right->GENOTYPE->Node[i]));
				}
				else
				{
					nn->GENOTYPE->Node.push_back(new Neuron(*itN));
				}
			}
			else
			{
				nn->GENOTYPE->Node.push_back(new Neuron(Right->GENOTYPE->Node[i]));
			}
		}
		nn->GENOTYPE->COUNT_N_ID = Right->GENOTYPE->COUNT_N_ID;
		//связи
		for (int i = 0; i < Right->GENOTYPE->Gen.size(); i++)
		{
			
			it = ContainsByInovID(Left->GENOTYPE->Gen.begin(), Left->GENOTYPE->Gen.end(), Right->GENOTYPE->Gen[i]);
			if (it != Left->GENOTYPE->Gen.end()) //если ген совпадает
			{

				if (rand() % 2)
				{
					nn->GENOTYPE->Gen.push_back(new Connect(Right->GENOTYPE->Gen[i]));
				}
				else
					nn->GENOTYPE->Gen.push_back(new Connect((*it)));
			}
			else
				nn->GENOTYPE->Gen.push_back(new Connect(Right->GENOTYPE->Gen[i]));
		}
		
	}

	if (Left->Fitness == Right->Fitness)
	{
		if (Left->GENOTYPE->Gen.size() < Right->GENOTYPE->Gen.size())
		{
			for (int i = 0; i < Left->GENOTYPE->Gen.size(); i++)
				nn->GENOTYPE->Gen.push_back(new Connect(Left->GENOTYPE->Gen[i]));

			for (int i = 0; i < Left->GENOTYPE->Node.size(); i++)
				nn->GENOTYPE->Node.push_back(new Neuron(Left->GENOTYPE->Node[i]));

			nn->GENOTYPE->COUNT_N_ID = Left->GENOTYPE->COUNT_N_ID;
		}

		if (Left->GENOTYPE->Gen.size() > Right->GENOTYPE->Gen.size())
		{
			for (int i = 0; i < Right->GENOTYPE->Gen.size(); i++)
				nn->GENOTYPE->Gen.push_back(new Connect(Right->GENOTYPE->Gen[i]));

			for (int i = 0; i < Right->GENOTYPE->Node.size(); i++)
				nn->GENOTYPE->Node.push_back(new Neuron(Right->GENOTYPE->Node[i]));

			nn->GENOTYPE->COUNT_N_ID = Right->GENOTYPE->COUNT_N_ID;
		}

		if (Left->GENOTYPE->Gen.size() == Right->GENOTYPE->Gen.size())
		{
			if (rand() % 2)
			{
				for (int i = 0; i < Left->GENOTYPE->Gen.size(); i++)
					nn->GENOTYPE->Gen.push_back(new Connect(Left->GENOTYPE->Gen[i]));

				for (int i = 0; i < Left->GENOTYPE->Node.size(); i++)
					nn->GENOTYPE->Node.push_back(new Neuron(Left->GENOTYPE->Node[i]));

				nn->GENOTYPE->COUNT_N_ID = Left->GENOTYPE->COUNT_N_ID;
			}
			else
			{
				for (int i = 0; i < Right->GENOTYPE->Gen.size(); i++)
					nn->GENOTYPE->Gen.push_back(new Connect(Right->GENOTYPE->Gen[i]));

				for (int i = 0; i < Right->GENOTYPE->Node.size(); i++)
					nn->GENOTYPE->Node.push_back(new Neuron(Right->GENOTYPE->Node[i]));

				nn->GENOTYPE->COUNT_N_ID = Right->GENOTYPE->COUNT_N_ID;
			}
		}
	}
	nn->GENOTYPE->COUNT_N_ID = nn->GENOTYPE->Node.size();
	return nn;
}

std::vector<Connect *>::iterator Population::HaveConnection(unsigned input, unsigned output, bool type) //глобальный поиск
{
	Connect *c = new Connect(input, output, 1, type, 1);
	auto it = ContainsByLR(UniqueGen.begin(), UniqueGen.end(), c);
	delete c;
	return it;
}

std::vector<Connect *>::iterator Population::HaveConnection(unsigned input, unsigned output, bool type, NeuralNet *nn)
{
	//проверить не существует ли такая связь
	//в текущей нейронной сети
	Connect *c = new Connect(input, output, 1, type, 1);
	auto it = ContainsByLR(nn->GENOTYPE->Gen.begin(), nn->GENOTYPE->Gen.end(), c);
	delete c;
	return it;
}

bool Population::CheckConnection(unsigned input, unsigned output, const NeuralNet *nn)
{
	bool ans = true;
	std::vector<Neuron *>::iterator neuron;
	
	//if ((*neuron)->TYPE[2]) 
	for (int i = 0; i < nn->GENOTYPE->Gen.size(); i++)
	{
		
		if (input == nn->GENOTYPE->Gen[i]->LEFT_NEURON_ID && !nn->GENOTYPE->Gen[i]->TYPE)
		{
			if (nn->GENOTYPE->Gen[i]->RIGHT_NEURON_ID == output)
				return 0;
			//защита от дурака и проверка на тип если на конце стоит оутпут, то мы прошли сеть и связь по данному направлению возможна
			neuron = ContainsByIDNeuron(nn->GENOTYPE->Node.begin(), nn->GENOTYPE->Node.end(), nn->GENOTYPE->Gen[i]->RIGHT_NEURON_ID);

			if (neuron != nn->GENOTYPE->Node.end() && (*neuron)->TYPE[2])
			{
			}
			else
			{
				ans = ans && CheckConnection(nn->GENOTYPE->Gen[i]->RIGHT_NEURON_ID, output, nn);	
			}
		}
		if (!ans)
		{
			//nn->BadNeuron.push_back((int)input);
			return 0;
		}
	}
	return ans;
}


void Population::Mutation(NeuralNet *nn)
{
	nn->SetOutputInputHiddenNeuron();
	int q = sqrt(nn->GENOTYPE->Node.size());
	int NumTry = q;
	double deltasqrGen, deltaGen, deltaNode;
	//switch (PARAM_MUTATION)
	{
//	case 0:
		deltaGen = 1. / 3. / nn->GENOTYPE->Gen.size();
		deltaNode = 1. / 3. / nn->GENOTYPE->Node.size();
		deltasqrGen = 1. / 3. / q;
		//break;
//	case 1:
	/*	deltaGen = 1. / nn->GENOTYPE->Gen.size();
		deltaNode = 1. / nn->GENOTYPE->Node.size();
		deltasqrGen = 1. / q;
	//	break;
	//case 2:
		deltaNode = 3. / nn->GENOTYPE->Node.size();
		deltaGen = 3. / nn->GENOTYPE->Gen.size();
		deltasqrGen = 3. / q;
	//	break;*/

	}
	nn->GENOTYPE->COUNT_N_ID = nn->GENOTYPE->Node.size();
	//случайная связь
	int inNeuron, outNeuron;
	//на первм месте input нейрон или hidden на втором idden или output
	bool flag = true;
	//мутация с добавлением несуществующей свзи
	bool type;

	for (int i = 0; i < q;i++) //пока не удастся сделать связь. Стоит ли добавить колличество попыток
	{
		NumTry--;
		if (randfloat() < deltasqrGen)
		{
			if (nn->ID_HIDDEN_NEURON.size() > 0)
			{
				type = rand() % 2; //реккурентная связь или нет
			}
			else
				type = 0;
			if (type) //выбираем по типу
			{
				//выбираем из определённого набора нейронов

				inNeuron = randint(0, nn->ID_OUTPUT_NEURON.size() + nn->ID_HIDDEN_NEURON.size() - 1);
				if (inNeuron < nn->ID_HIDDEN_NEURON.size())
					inNeuron = nn->ID_HIDDEN_NEURON[inNeuron];
				else
					inNeuron = nn->ID_OUTPUT_NEURON[inNeuron - nn->ID_HIDDEN_NEURON.size()];

				outNeuron = nn->ID_HIDDEN_NEURON[randint(0, nn->ID_HIDDEN_NEURON.size() - 1)];
			}

			else
			{
				//выбираем из поределенного набора нейронов только инпут нейрон из набора инпутов и хидденов
				inNeuron = randint(0, nn->ID_INPUT_NEURON.size() + nn->ID_HIDDEN_NEURON.size() - 1);
				//оутпут нейрон из хидденов и оутпутов
				outNeuron = randint(0, nn->ID_HIDDEN_NEURON.size() + nn->ID_OUTPUT_NEURON.size() - 1);
				//если нейрон выпал два раза, то рекурентная связь

				if (inNeuron < nn->ID_INPUT_NEURON.size())
					inNeuron = nn->ID_INPUT_NEURON[inNeuron];
				else
					inNeuron = nn->ID_HIDDEN_NEURON[inNeuron - nn->ID_INPUT_NEURON.size()];

				if (outNeuron < nn->ID_HIDDEN_NEURON.size())
					outNeuron = nn->ID_HIDDEN_NEURON[outNeuron];
				else
					outNeuron = nn->ID_OUTPUT_NEURON[outNeuron - nn->ID_HIDDEN_NEURON.size()];
				if (inNeuron == outNeuron) type = 1;
			}

			auto c = HaveConnection(inNeuron, outNeuron, type);
			if (c != UniqueGen.end()) //проверяем глобально	не придумали ли мы велосипед
			{
				if (HaveConnection(inNeuron, outNeuron, type, nn) == nn->GENOTYPE->Gen.end()) // проверяем внутри сети на всякий случий
				{
					if (CheckConnection(outNeuron, inNeuron, nn))
						nn->GENOTYPE->Gen.push_back(new Connect(*(c))); //добавляем из глобального списка
				}
			}
			else //если такой связи ещё нет
			{
				if (HaveConnection(inNeuron, outNeuron, type, nn) == nn->GENOTYPE->Gen.end())
				{
					if (type) //если рекурентная то просто добавляем
					{
						nn->GENOTYPE->Gen.push_back(new Connect(inNeuron, outNeuron, INNOVATION_ID, true, true));
						UniqueGen.push_back(new Connect(inNeuron, outNeuron, INNOVATION_ID, true, true));
						INNOVATION_ID++;

					}
					else
					{
						bool ans = CheckConnection(outNeuron, inNeuron, nn);
						if (ans)
						{
							nn->GENOTYPE->Gen.push_back(new Connect(inNeuron, outNeuron, INNOVATION_ID, false, true)); //вставить добавление нейрона в сеть
							UniqueGen.push_back(new Connect(inNeuron, outNeuron, INNOVATION_ID, false, true));
							INNOVATION_ID++;

						}
					}
				}
			}
		}
	}
	//конец мутации с созданием новой связи
	std::vector<Connect *> newConnect = std::vector<Connect *>();
	//создание промежуточной связи
	double prob = 0;
	
	//
	int n = nn->GENOTYPE->Gen.size();
	for (int i = 0; i < n; i++)
	{
		//если это не рекурентная связь
		if (!nn->GENOTYPE->Gen[i]->TYPE)
		{
			prob = randfloat();
			if (deltaGen >= prob)
			{
				//cout << "Mutation" <<" "<<delta<<" "<<prob<< endl;
				//выкючаем текущую связь
				nn->GENOTYPE->Gen[i]->ENABLE = false;
				//создаём промежуточный нейрон
				Neuron *ad = new Neuron(1, nn->GENOTYPE->COUNT_N_ID, randint(0, 9));
				nn->GENOTYPE->Node.push_back(ad);
				//добавляем его
				nn->ID_HIDDEN_NEURON.push_back(ad->ID);
				nn->GENOTYPE->COUNT_N_ID++;

				Connect *newGen1 = new Connect(nn->GENOTYPE->Gen[i]->LEFT_NEURON_ID, ad->ID, INNOVATION_ID, 0, 1);
				//создаём новую связь (1 часть)
				INNOVATION_ID++;
				auto c = HaveConnection(newGen1->LEFT_NEURON_ID, newGen1->RIGHT_NEURON_ID, newGen1->TYPE);
				if (c != UniqueGen.end()) //проверяем глобально
				{
					(*c)->ENABLE = true;
					Connect* q = new Connect(*c);
					nn->GENOTYPE->Gen.push_back(q); //добавляем из глобального списка
					delete newGen1;
					INNOVATION_ID--;
				}
				else
				{
					newGen1->ENABLE = true;
					//newConnect.push_back(newGen1);
					nn->GENOTYPE->Gen.push_back(newGen1);//вставить добавление нейрона в сеть
					UniqueGen.push_back(new Connect(newGen1));
				}
				//(2 часть)
				Connect *newGen2 = new Connect(ad->ID, nn->GENOTYPE->Gen[i]->RIGHT_NEURON_ID, INNOVATION_ID, 0, 1);
				INNOVATION_ID++;
				c = HaveConnection(newGen2->LEFT_NEURON_ID, newGen2->RIGHT_NEURON_ID, newGen2->TYPE);
				if (c != UniqueGen.end()) //проверяем глобально
				{
					(*c)->ENABLE = true;
					Connect* q = new Connect(*c);
					nn->GENOTYPE->Gen.push_back(q); //добавляем из глобального списка
					delete newGen2;
					INNOVATION_ID--;
				}
				else
				{
					newGen2->ENABLE = true;
					//newConnect.push_back(newGen2);
					nn->GENOTYPE->Gen.push_back(newGen2);//вставить добавление нейрона в сеть
					UniqueGen.push_back(new Connect(newGen2));
				}
			}
		}
	}
	//конец промежуточной связи
	//добавить мутацию функции активации

	for (int i = 0; i < nn->GENOTYPE->Node.size(); i++)
	{
		prob = randfloat();

		if (deltaNode >= prob)
		{
			nn->GENOTYPE->Node[i]->ActivationFunction =  randint(0,9);
		}
	}
	nn->CreateWeightArray();
	
}

void Population::MutationAll(NeuralNet** arr)
{
	double prob, probc;
	double rnd;
	NumIndMutHigh.clear();
	NumIndMutMedium.clear();
	NumIndMutLow.clear();
		for (int i = 0; i < COUNT_NN; i++)
		{
			probc = randfloat();
			if (probc < PMutLow)
			{
				PARAM_MUTATION=0; //low
				NumIndMutLow.push_back(i);
			}
			if (probc > PMutLow && probc < (PMutMedium + PMutLow))
			{
				PARAM_MUTATION = 1; //medium
				NumIndMutMedium.push_back(i);
			}
			if (probc >(PMutLow + PMutMedium))
			{
				PARAM_MUTATION = 3; //high
				NumIndMutHigh.push_back(i);
			}
			Mutation(arr[i]);
			CheckGen();
		
		}
}

void Population::Selection()
{
	double prob;
	NumIndSelRange.clear();
	NumIndSelTour.clear();
	std::vector<int> ParentPool = std::vector<int>();
	for (int i = 0; i < COUNT_NN; i++)
	{
		prob = randfloat();

		if (prob > PSelTour) //выбрали ранговую
		{
			NumIndSelRange.push_back(i);
			double **Arr;
			Arr = new double*[COUNT_NN];
			for (int i = 0; i < COUNT_NN; i++)
			{
				Arr[i] = new double[2];
				Arr[i][0] = NN[i]->Fitness;
				Arr[i][1] = i;
			}

			GnomeSort1(Arr, COUNT_NN);


			double *p = new double[COUNT_NN];
			for (int i = 0; i < COUNT_NN; i++)
				p[i] = ((i + 1)*2.0) / (COUNT_NN*COUNT_NN + COUNT_NN);   //������� ����������� ��������� ��� ����� ���������� ��������� �������
			int a, b;
			b = COUNT_NN - 1;
			bool c = false;


			for (int i = COUNT_NN - 1; i > 0; i--)    //����� ���������� ����������� ��� ���������� ����������� ��������� ������� ����������� ����� ����� ���������
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
						AverageProbability1(p, a, b);
						c = false;
					}

				}
			}


			double r, s;
			s = 0;
			r = randfloat();

			for (int j = 0; j < COUNT_NN; j++)
			{
				s += p[j];
				if (r < s)
				{
					ParentPool.push_back(j);
					break;
				}

			}
			s = 0;
			r = randfloat();

			for (int j = 0; j < COUNT_NN; j++)
			{
				s += p[j];
				if (r < s)
				{
					ParentPool.push_back(j);
					break;
				}

			}

			delete p;
			for (int i = 0; i < COUNT_NN; i++)
				delete Arr[i];
			delete Arr;

		}
		else {
			NumIndSelTour.push_back(i);
			double sumRang = 0;
			//определить размер турнира
			int sizeTour = 1 + pow(COUNT_NN, 0.5);//sqrt
			if (sizeTour % 2) sizeTour--; //если чётное то уменьшаем на единицу
			std::vector<int> Pool = std::vector<int>();
			int rnd;

			Pool.clear();
			for (int j = 0; j < sizeTour; j++)
			{
				rnd = randint(0, COUNT_NN - 1);
				if (find(Pool.begin(), Pool.end(), rnd) == Pool.end())
					Pool.push_back(rnd);
				else j--;
			}
			ParentPool.push_back(WinnerTournament(Pool));

			Pool.clear();
			for (int j = 0; j < sizeTour; j++)
			{
				rnd = randint(0, COUNT_NN - 1);
				if (find(Pool.begin(), Pool.end(), rnd) == Pool.end())
					Pool.push_back(rnd);
				else j--;
			}
			ParentPool.push_back(WinnerTournament(Pool));
		}
	}
	for (int i = 0; i < COUNT_NN; i++)
	{
		//NeuralNet* q =;
		NextGeneration[i] = Cross(NN[ParentPool[i * 2]], NN[ParentPool[i * 2 + 1]]);
		//delete q;

	}
}
//для следующих поколений ghjuhfvvsможно групировать сети
std::vector<int> Population::SelectionInGroups(std::vector<int> part,int count_ind)
{
	/*
	std::vector<int> ParentPool;
	double sumRang = 0;
	//определить размер турнира
	int sizeTour = pow(COUNT_NN,0.5);//sqrt
	if (sizeTour % 2) sizeTour--; //если чётное то уменьшаем на единицу
	std::vector<int> Pool;
	int rnd;
	for (int i = 0; i < COUNT_NN * 2; i++)
	{
		for (int j = 0; j < sizeTour; j++)
		{
			rnd = randint(0, COUNT_NN);
			if (find(Pool.begin(), Pool.end(), rnd) == Pool.end())
				Pool.push_back(rnd);
			else j--;
		}
		ParentPool.push_back(WinnerTournament(Pool));

	}
	return ParentPool;
	*/
	return std::vector<int>();
} 



void Population::TrainNet()
{
	
#pragma omp parallel
	{
		srand(int(time(NULL)) ^ omp_get_thread_num()+rand());
#pragma omp  for
		
		for (int i = 0; i < COUNT_NN; i++)
		{
			int nInd, nGenr;
			nInd = 200 +NN[i]->GENOTYPE->Gen.size() * 2;
			nGenr = 200 +NN[i]->GENOTYPE->Gen.size() * 2;
			NN[i]->SetOutputInputHiddenNeuron();
			GeneticAlgoritm* ga = new GeneticAlgoritm(NN[i]->size, -1, 1, 0.0001, nInd,nGenr, 1, 2, 1, DATA, NN[i]);
			ga->Evolution();
			cout << "Element:" << i << " "<< ga->GetBestValue() << endl;
			//	ga->SetWeight();
			ga->CopyWeightInNN(NN[i],ga->Best);
		}
	}
}

void Population::GTNextGeneration()
{
	for (int i=0;i<COUNT_NN;i++)
	{
		//[i]->~NeuralNet();
	//	if (NN[i]->NoneOld >= MAX_NO_CHANGE)
		//{
		//	NN[i] = new NeuralNet(COUNT_INPUT, COUNT_OUTPUT);
	//	}
	//	else
	//	{
			NN[i] = new NeuralNet(NextGeneration[i]);
	//	}
		//NN[i]->CreateWeightArray();
		NN[i]->SetOutputInputHiddenNeuron();
		//delete NextGeneration[i];
	}
}

void Population::SelectBest()
{
	for (int i = 0; i < COUNT_NN; i++)
	{
		if (NN[i]->Fitness > Bestvalue)
		{
			Bestvalue = NN[i]->Fitness;
			Best = new NeuralNet(NN[i]);
		}
	}
}

void Population::PrintBestValue()
{
	cout << Bestvalue << endl;
}

void Population::GetFitness(/*NeuralNet** arr*/)
{
#pragma omp parallel
	{
#pragma omp for 
		for (int i = 0; i < COUNT_NN; i++)
		{
			Data* data = new Data(*DATA);
			NN[i]->ZerosMeanNeuron();
			NN[i]->OldFitness = NN[i]->Fitness;
			NN[i]->Fitness = OptimizeFunction1(data, NN[i], 2);
		//	if (NN[i]->Fitness < NN[i]->OldFitness) 
		//		NN[i]->NoneOld++;
		//	else
			//	NN[i]->NoneOld = 0;
#pragma omp critical 
			cout<<"Fitnes #"<<i<<"=" << NN[i]->Fitness << endl;
			delete data;
		}
	}
}

void Population::DeleteNNs(NeuralNet **nn)
{
	for (int i = 0; i < COUNT_NN; i++)
	{
		NN[i]->~NeuralNet();
	}
}

Population::Population(unsigned cInd, unsigned cIn, unsigned cOut, unsigned paramMutation,int cnt_gnr)
{
	//Groups = std::vector<std::vector<int>>();
	//Groups.push_back(std::vector<int>());
	COUNT_INPUT = cIn;
	COUNT_OUTPUT = cOut;
	INNOVATION_ID = 0;
	UniqueGen = std::vector<Connect *>();
	PARAM_MUTATION = paramMutation;
	COUNT_NN = cInd;
	COUNT_GENERATION = cnt_gnr;
	NN = new NeuralNet *[cInd];
	NextGeneration = new NeuralNet *[cInd];
	Bestvalue = DBL_MIN;
	PMutLow=1./3;
	PMutHigh=1./3;
	PMutMedium=1./3;
	PSelTour= 1./2;
	PSelRange= 1./2;
	for (int i = 0; i < cInd; i++)
	{
		NN[i] = new NeuralNet(cIn, cOut);
		NextGeneration[i] = new NeuralNet();
	}
	CheckGen();
	
}

Population::~Population()
{
}
/*
void Population::WriteBestSolutionInFile(int numbgeneration)
{
	EVOLUTION_FILE << "Generation №" << numbgeneration << endl;
	Neuron* s;
	
	for (std::map<unsigned, std::vector<int>>::iterator i = Best->VectorDependence.begin(); i != Best->VectorDependence.end(); ++i)
	{
		EVOLUTION_FILE << i->first<<"("<< s->ActivationFunction <<")" << ":";	
		for (std::vector<int>::iterator j = i->second.begin(); j != i->second.end(); ++j)
		{
			EVOLUTION_FILE << *j << "("<<Best->Weight[i->first][*j]<<")" << ",";
		}
		EVOLUTION_FILE << std::endl;
	}
	EVOLUTION_FILE << "END" << std::endl;
}
*/
std::vector<Connect *>::iterator Population::ContainsByLR(std::vector<Connect *>::iterator first, std::vector<Connect *>::iterator last, const Connect *c) //Поиск элемента в связях по нейронам
{

	while (first != last)
	{
		if ((*first)->LEFT_NEURON_ID == c->LEFT_NEURON_ID && (*first)->RIGHT_NEURON_ID == c->RIGHT_NEURON_ID && (*first)->TYPE == c->TYPE)
			return first;
		else
			++first;
	}
	return last;
}



int Population::WinnerTournament(std::vector<int> Pool)
{
	int n = Pool.size();
	Pool = GnomeSortAndMaxFitness(Pool,n);
	n = Pool.size();
	if (n == 1) return Pool[0];
	else
	{
		return randint(0,n-1);
	}
}

std::vector<int> Population::GnomeSortAndMaxFitness(std::vector<int> A, int N) {
	
	std::vector<int> R = std::vector<int>();
	int i = 0;
	while (i < N) {
		if (i == 0 || NN[A[i - 1]]->Fitness >= NN[A[i]]->Fitness) ++i;
		else {
			int Temp = A[i];
			A[i] =A[i - 1];
			A[i - 1] = Temp;
			--i;
		}
	}
	R.push_back(A[0]);
	for (int i=1;i<N;i++)
		if (NN[A[i]]->Fitness==R[0])
			R.push_back(A[i]);
	return R;
}