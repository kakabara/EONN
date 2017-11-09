#include "Neuron.h"




Neuron::Neuron(int type,unsigned id,unsigned acf)
{
	ID = id; 
	TYPE[0] = false;
	TYPE[1] = false;
	TYPE[2] = false;
	TYPE[type] = true;
	ActivationFunction = acf;
}

Neuron::Neuron(Neuron * c)
{
	ID = c->ID;
	TYPE[0] = c->TYPE[0];
	TYPE[1] = c->TYPE[1];
	TYPE[2] = c->TYPE[2];
	ActivationFunction = c->ActivationFunction;
}

Neuron::~Neuron()
{
}
