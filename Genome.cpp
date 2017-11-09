#include "Genome.h"


using namespace std;



Genome::Genome(unsigned countIn, unsigned countOut,unsigned acf)
{
	COUNT_ACF = acf;
	
	COUNT_INOV_ID = 0;
	Node = vector<Neuron*>();
	Neuron* n = new Neuron(0, 0, -1); //bias
	COUNT_N_ID = 1;
	Node.push_back(n);
	
	for (int i = 0; i < countIn; i++)
	{
		 n = new Neuron(0,COUNT_N_ID,-1);
		COUNT_N_ID++;
		Node.push_back(n);
	}
	
	for (int i = 0; i < countOut; i++)
	{
		 n = new Neuron(2, COUNT_N_ID, rand() % COUNT_ACF);
		COUNT_N_ID++;
		Node.push_back(n);
	}
	Gen = vector<Connect*>();
	
	for (int i = 0; i < countIn+1; i++)
		for (int j=0;j<countOut;j++)
		{
			Connect* c = new Connect(Node[i]->ID,Node[countIn+1+j]->ID,-1,0,1);
			Gen.push_back(c);
		}
}

Genome::Genome(Genome * c)
{
	Node = vector<Neuron*>();
	Gen = vector<Connect*>();
	COUNT_INOV_ID = c->COUNT_INOV_ID;
	for (int i = 0; i < c->Node.size(); i++)
	{
		Neuron* n = new Neuron(c->Node[i]);
		Node.push_back(n);
	}
	for (int i = 0; i < c->Gen.size(); i++)
	{
		Connect* n=new Connect(c->Gen[i]);
		Gen.push_back(n);
	}
	
	
}

Genome::Genome()
{
	Node = vector<Neuron*>();
	Gen = vector<Connect*>();
}

Genome::~Genome()
{
	for (int i = 0; i <Node.size(); i++)
	{
		delete Node[i];
	}
	for (int i = 0; i <Gen.size(); i++)
	{
		delete Gen[i];
	}
	
}
