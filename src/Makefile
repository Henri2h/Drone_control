CXX ?= g++
NAVIO = ../includes/Navio
INCLUDES = -I ../includes/Navio

all:
	$(MAKE) -C ../includes/Navio all
	
	$(CXX) -std=gnu++11 $(INCLUDES) Drone.cpp -L$(NAVIO) -lnavio  -lrt -lpthread -lpigpio -lpthread -L. -luWS -lssl -lcrypto -lz -luv -lncurses -o Drone 

clean:
	rm -f Drone
