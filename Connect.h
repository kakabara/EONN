#pragma once
class Connect
{
public:
	unsigned		LEFT_NEURON_ID;
	unsigned		RIGHT_NEURON_ID;
	int				INOVATION_ID;
	bool			TYPE;				//0-current 1-feedback
	bool			ENABLE;				//0-disable 1-enable
	bool operator <(Connect &comp);
	Connect(unsigned left,unsigned right,unsigned inovation,bool type,bool enable);
	Connect(const Connect* c);
	~Connect();
};

