CXX ?= g++
NAVIO = ../includes/Navio
INCLUDES = -I ../includes/Navio
HACKFLIGHT = $(wildcard ../includes/HACKFLIGHT/*.hpp)

all:
	$(MAKE) -C ../includes/Navio all
	$(CXX) -std=gnu++11 $(INCLUDES) Drone.cpp -L$(NAVIO)  -lnavio  -lrt -lpthread -lpigpio -lpthread -L. -luWS -lssl -lcrypto -lz -luv -o Drone || $(MAKE) pigpio

clean:
	rm -f Drone
