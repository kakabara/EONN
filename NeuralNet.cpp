#include "NeuralNet.h"
#include <iterator>
#include <vector>
#include <iostream>
void NeuralNet::SetOutputInputHiddenNeuron()			//хотим знать какие нейроны входные какие выходные
{
	ID_HIDDEN_NEURON.clear();
	ID_INPUT_NEURON.clear();
	ID_OUTPUT_NEURON.clear();
	for (int i = 0; i < GENOTYPE->Node.size(); i++)
	{
		if (GENOTYPE->Node[i]->TYPE[0]) ID_INPUT_NEURON.push_back(GENOTYPE->Node[i]->ID);
		if (GENOTYPE->Node[i]->TYPE[2]) ID_OUTPUT_NEURON.push_back(GENOTYPE->Node[i]->ID);
		if (GENOTYPE->Node[i]->TYPE[1]) ID_HIDDEN_NEURON.push_back(GENOTYPE->Node[i]->ID);
	}
	AnswerVector.clear();
	//for (int i = 0; i < ID_OUTPUT_NEURON.size(); i++)
		
}

std::vector<Neuron *>::iterator ContainsByIDNeuron(std::vector<Neuron *>::iterator first, std::vector<Neuron *>::iterator last, unsigned ID)
{
	while (first != last)
	{
		if ((*first)->ID == ID)
			return first;
		++first;
	}
	return last;
}

void NeuralNet::SetLayers()
{
	PairNeuronLayers.clear();
	for (int i = 0; i < GENOTYPE->Node.size(); i++)
	{
		if (GENOTYPE->Node[i]->TYPE[0])
			PairNeuronLayers[GENOTYPE->Node[i]->ID] = 0;
		else
			PairNeuronLayers[GENOTYPE->Node[i]->ID] = -1;
	}
	HiddenLayers.clear();
	bool flag = true;
	bool flag2;
	int j = 0;
	while (flag)
	{
		for (auto i = VectorDependence.begin(); i != VectorDependence.end(); i++)
		{
			j = 0;
			flag2 = true;
			while (flag2)
			{								//проверяем нейроны для прямого соединения
				if (i->second[j] >= 0) {	//проверяет можно ли посчитатть значение нейрона (готовы ли нейроны от которых он зависит?)
					if (PairNeuronLayers[i->second[j]] == -1)
					{
						flag2 = false;
						break;
					}
				}
				j++;
				if (j == i->second.size()) break;	//если все нейроны готовы

			}
			//если все значения от которых нейрон зависит готовы
			if (flag2)
			{
				int maxL = 0;
				for (auto q = 0; q < i->second.size(); q++)				//сумматор на входе
				{
					if (PairNeuronLayers[i->second[q]] > maxL)
					{
						maxL = PairNeuronLayers[i->second[q]];
					}
				}
				PairNeuronLayers[i->first] = maxL + 1;
			}
			
		}
		flag = false;
		for (int i = 0; i < ID_OUTPUT_NEURON.size(); i++)
		{
			if (PairNeuronLayers[ID_OUTPUT_NEURON[i]] == -1)
			{
				flag = true;
				break;
			}
	
		}
	}
	for (auto i = PairNeuronLayers.begin(); i != PairNeuronLayers.end(); ++i)
	{
		HiddenLayers[i->second].clear();
	}
		for (auto i = PairNeuronLayers.begin(); i != PairNeuronLayers.end(); ++i)
		{
			HiddenLayers[i->second].push_back(i->first);
		}
}
void PrintVectorDeoendence(std::map<unsigned, std::vector<int>> mp)
{
	for (auto i=mp.begin();i!=mp.end();++i)
	{
		std::cout << i->first << ":";
		for (auto j=i->second.begin();j!=i->second.end();++j)
		{
			std::cout << *j << ",";
		}
		std::cout << std::endl;
	}
	std::cout << "END" << std::endl;
}
void NeuralNet::CreateMeanNeuronArray()
{	//Создаём массивы куда будет записывать значения выходов нейронов
	MeanNeuron.clear();
	PreviusMeanNeuron.clear();
	for (int i = 0; i < GENOTYPE->Node.size(); i++)
	{
		MeanNeuron.insert(std::pair<int, double>(GENOTYPE->Node[i]->ID,0));
		if (!GENOTYPE->Node[i]->TYPE[0])
		{
			PreviusMeanNeuron.insert(std::pair<int, double>(GENOTYPE->Node[i]->ID, 0));
		}
	}
}

void NeuralNet::ZerosMeanNeuron()
{
	for (auto it = PreviusMeanNeuron.begin(); it != PreviusMeanNeuron.end(); ++it)//переносим текущие значения в матрицу предыдущих
	{
		it->second =0;
	}
	for (auto it = MeanNeuron.begin(); it != MeanNeuron.end(); ++it) //текущие значения ставим на максимальное, что не соответствует действительности
		it->second = 0;
}

void NeuralNet::RefreshMeanNeuron()
{
	for (auto it = PreviusMeanNeuron.begin(); it != PreviusMeanNeuron.end(); ++it)//переносим текущие значения в матрицу предыдущих
	{
		it->second = MeanNeuron[it->first];
	}
	for (auto it = MeanNeuron.begin(); it != MeanNeuron.end(); ++it) //текущие значения ставим на максимальное, что не соответствует действительности
		it->second = 0;
}

void NeuralNet::SetInputNeuron(std::vector<double> InputArray)				//подаём данные на вход потом надо вызвать гет предиктион
{
	if (InputArray.size() == 0)
		system("pause");
	for (int i = 0; i < ID_INPUT_NEURON.size(); i++)
	{
		if (ID_INPUT_NEURON[i] == 0) MeanNeuron[0] = 1;  //bias
		else
			MeanNeuron[ID_INPUT_NEURON[i]] = InputArray[i-1];
	}
}

void NeuralNet::GetVectorDependence()			//устанавливаем зависимости
{
	size = 0;
	VectorDependence.clear();
	for (int i = 0; i < GENOTYPE->Node.size(); i++)
		if (!GENOTYPE->Node[i]->TYPE[0])
			VectorDependence.insert(std::pair<unsigned, std::vector<int>>(GENOTYPE->Node[i]->ID, std::vector<int>()));
	for (int i = 0; i < GENOTYPE->Gen.size(); i++)
	{
		if (GENOTYPE->Gen[i]->ENABLE)
		{
			size++;
			if (GENOTYPE->Gen[i]->TYPE)	
				
			{//смотрим обратная ли это связь
				VectorDependence[GENOTYPE->Gen[i]->RIGHT_NEURON_ID].push_back((-1 * GENOTYPE->Gen[i]->LEFT_NEURON_ID));
			}
			else
			{
				VectorDependence[GENOTYPE->Gen[i]->RIGHT_NEURON_ID].push_back(GENOTYPE->Gen[i]->LEFT_NEURON_ID);
				
			}
		}
	}
	//PrintVectorDeoendence(VectorDependence);
	CreateMeanNeuronArray();
	SetLayers();
}

void NeuralNet::CreateWeightArray()			//Создаём веса исходя из генов которые активны
{
	//size = 0;
	GetVectorDependence();
	Weight.clear();
	for (std::map<unsigned, std::vector<int>>::iterator i = VectorDependence.begin(); i != VectorDependence.end(); ++i)
	{
		for (std::vector<int>::iterator j = i->second.begin(); j != i->second.end(); ++j)
			Weight[i->first][*j] = 0;
	}
/*	for (int i = 0; i < GENOTYPE->Gen.size(); i++)
	{
		if (GENOTYPE->Gen[i]->ENABLE)
		{
			if (GENOTYPE->Gen[i]->TYPE)
			{
				Weight[GENOTYPE->Gen[i]->RIGHT_NEURON_ID][-1*GENOTYPE->Gen[i]->LEFT_NEURON_ID] = 0;
				size++;
			}
			else
			{
				Weight[GENOTYPE->Gen[i]->RIGHT_NEURON_ID][GENOTYPE->Gen[i]->LEFT_NEURON_ID] = 0;
				size++;
			}
		}
	}*/
}

void NeuralNet::GetAnswer()				//нейронная сеть делает предсказание (заполнение массива MeanNeuron до тех пор пока все оутпут нейроны не получат значение)
{
	/*std::vector<Neuron*>::iterator it;
	double input;
	if (HiddenLayers.size() != 0) {
		for (int i = 1; i < HiddenLayers.size(); i++)
		{
			for (int j = 0; j < HiddenLayers[i].size(); j++)
			{
				input = 0;
				//	std::cout << HiddenLayers[i].size() << std::endl;
				 it = ContainsByIDNeuron(GENOTYPE->Node.begin(), GENOTYPE->Node.end(), HiddenLayers[i][j]);
				if (it != GENOTYPE->Node.end() && !(*it)->TYPE[0])
				{
					//	std::cout << VectorDependence[HiddenLayers[i][j]].size() << std::endl;
					for (int k = 0; k < VectorDependence[HiddenLayers[i][j]].size(); k++)
					{

						if ((VectorDependence[HiddenLayers[i][j]][k]) >= 0)	//Если это прямая связь
						{
							input += MeanNeuron[(VectorDependence[HiddenLayers[i][j]][k])] * Weight[HiddenLayers[i][j]][(VectorDependence[HiddenLayers[i][j]][k])];

						}
						else
						{
							input += PreviusMeanNeuron[-1 * (VectorDependence[HiddenLayers[i][j]][k])] * Weight[HiddenLayers[i][j]][((VectorDependence[HiddenLayers[i][j]][k]))];
						}

					}
					MeanNeuron[HiddenLayers[i][j]] = MathFunctions[(*it)->ActivationFunction](input);  //записываем значение нейрона
				}
			}
		}
	}
	else
	{
		std::cout << "PIZZDEEEC" << std::endl;
	}*/
		bool flag = true;
	bool flag2 = true;
	int j = 0;
	double input = 0;
	
	while (flag)
	{
		for (auto i=VectorDependence.begin();i!=VectorDependence.end();i++)
		{
			j = 0;
			flag2 = true;
			while (flag2)
			{								//проверяем нейроны для прямого соединения
				if (i->second[j] >= 0) {	//проверяет можно ли посчитатть значение нейрона (готовы ли нейроны от которых он зависит?)
					if (MeanNeuron[i->second[j]] == DBL_MAX) 
						flag2 = false;
				}
				j++;
				if (j == i->second.size()) break;	//если все нейроны готовы
				
			}
			//если все значения от которых нейрон зависит готовы
			if (flag2)
			{
				input = 0;
				for (auto q = 0; q < i->second.size(); q++)				//сумматор на входе
				{
					if (i->second[q] >= 0)	//Если это прямая связь
					{
						input += MeanNeuron[i->second[q]] * Weight[i->first][i->second[q]];
						
					}
					else
					{
						input += PreviusMeanNeuron[-1*i->second[q]] * Weight[i->first][(-1*i->second[q])];
					}
					
				}
				MeanNeuron[i->first] = MathFunctions[GENOTYPE->Node[i->first]->ActivationFunction](input);  //записываем значение нейрона
			}
		}
		for (int i = 0; i < ID_OUTPUT_NEURON.size() && flag; i++)
		{
			if (MeanNeuron[ID_OUTPUT_NEURON[i]] != DBL_MAX) flag = false;
			else flag = true;
		}
	}
	SetAnswerVector();
	RefreshMeanNeuron();
}

void NeuralNet::SetAnswerVector()
{
	AnswerVector.clear();
	for (int i = 0; i < ID_OUTPUT_NEURON.size(); i++)
	{
		AnswerVector.push_back(MeanNeuron[ID_OUTPUT_NEURON[i]]);
	}
}

NeuralNet::NeuralNet(unsigned countIn, unsigned countOut)
{
	
	COUNT_INPUT = countIn;
	COUNT_OUTPUT = countOut;
	GENOTYPE = new Genome(countIn,countOut,10);
	COUNT_ACF = 10;	
	NoneOld = 0;
	OldFitness = DBL_MIN;
	SetOutputInputHiddenNeuron();
}

NeuralNet::NeuralNet(NeuralNet * C)
{

	COUNT_INPUT = (int)C->COUNT_INPUT;
	COUNT_OUTPUT = (int)C->COUNT_OUTPUT;
	COUNT_ACF = (int)C->COUNT_ACF;
	Fitness = C->Fitness;
	GENOTYPE = new Genome(C->GENOTYPE);
	NoneOld = C->NoneOld;
	OldFitness = C->OldFitness;
	VectorDependence.clear();
	HiddenLayers.clear();
	PairNeuronLayers.clear();
	GetVectorDependence();
	
	
	SetOutputInputHiddenNeuron();
	
}

NeuralNet::NeuralNet()
{
	GENOTYPE = new Genome();
	COUNT_ACF = 10;
	Fitness = DBL_MIN;
	NoneOld = 0;
	OldFitness = DBL_MIN;
}

NeuralNet::~NeuralNet()
{
	delete GENOTYPE;
}
	

