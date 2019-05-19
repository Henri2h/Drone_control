CXX ?= g++
NAVIO = ../../includes/Navio
INCLUDES = -I ../../includes/Navio

all:
	$(MAKE) -C ../../includes/Navio all
	$(CXX) $(INCLUDES) Barometer.cpp -L$(NAVIO) -lnavio -o Barometer

clean:
	rm -f Barometer
