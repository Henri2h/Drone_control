#include "Data.h"

Data::Data(/* args */)
{
	for (size_t i = 0; i < status_length; i++)
	{
		this->status[i] = 0;
	}

	for (size_t i = 0; i < params_length; i++)
	{
		this->parameters[i] = 0;
	}

	for (size_t i = 0; i < gains_length; i++)
	{
		this->controller_gains[i] = 0;
	}
	
}

Data::~Data()
{
}
