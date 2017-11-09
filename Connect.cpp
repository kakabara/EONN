#include "Connect.h"






bool Connect::operator<(Connect & comp)
{
	return INOVATION_ID < comp.INOVATION_ID;
}

Connect::Connect(unsigned left, unsigned right, unsigned inovation, bool type, bool enable)
{
	LEFT_NEURON_ID = left;
	RIGHT_NEURON_ID = right;
	INOVATION_ID = inovation;
	TYPE = type;
	ENABLE = enable;
}

Connect::Connect(const Connect * c)
{
	LEFT_NEURON_ID = (int)c->LEFT_NEURON_ID;
	RIGHT_NEURON_ID = (int)c->RIGHT_NEURON_ID;
	INOVATION_ID = (int)c->INOVATION_ID;
	TYPE = (int)c->TYPE;
	ENABLE = true;
}

Connect::~Connect()
{
}
