CXX ?= g++
NAVIO = ../../includes/Navio
INCLUDES = -I ../../includes/Navio

all:
	$(MAKE) -C ../../includes/Navio all
	$(CXX) -std=gnu++11 $(INCLUDES)  ADC.cpp -L$(NAVIO) -lnavio -o ADC

clean:
	rm -f ADC
