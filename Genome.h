#pragma once
#include "Connect.h"
#include "Neuron.h"
#include <vector>
class Genome
{
public:
	std::vector<Neuron*>	Node;
	std::vector<Connect*>	Gen;
	unsigned				COUNT_ACF;
	unsigned				COUNT_INOV_ID;
	unsigned				COUNT_N_ID;
	Genome(unsigned countIn, unsigned countOut,unsigned acf);
	Genome(Genome* c);
	Genome();
	~Genome();
};

