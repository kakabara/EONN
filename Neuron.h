#pragma once
/*



*/
class Neuron
{
public:
	unsigned		ID;
	bool			TYPE[3] = {false,false,false};			// {Input=[1,0,0]; Hidden=[0,1,0]; Output=[0,0,1]}
	unsigned		ActivationFunction;	//Выбираем из массива Functions

	Neuron(int type,unsigned id,unsigned acf);
	Neuron(Neuron* c);
	~Neuron();
};

