CXX ?= g++
NAVIO = ../../includes/Navio
INCLUDES = -I $(NAVIO)

all:
	$(MAKE) -C $(NAVIO) all
	$(CXX) -std=c++11 $(INCLUDES) AHRS.cpp -L$(NAVIO) -lnavio -o AHRS -lrt -lpthread

clean:
	rm -f AHRS
