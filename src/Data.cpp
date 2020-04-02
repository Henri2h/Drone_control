#include "Data.h"

Data::Data(/* args */)
{
	for (size_t i = 0; i < status_length; i++)
	{
		this->status[i] = 0;
	}
	
}

Data::~Data()
{
}
