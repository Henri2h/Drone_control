CXX ?= g++
NAVIO = ../../includes/Navio
INCLUDES = -I ../../includes/Navio

all:
	$(MAKE) -C ../../includes/Navio all
	$(CXX) -std=gnu++11 $(INCLUDES) Servo.cpp -L$(NAVIO) -lnavio -o Servo

clean:
	rm -f Servo
