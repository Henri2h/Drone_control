CXX ?= g++
NAVIO = ../includes/Navio
INCLUDES = -I ../includes/Navio



# extensions #
SRC_EXT = cpp

# code lists #
# Find all source files in the source directory, sorted by
# most recently modified
SOURCES = $(shell find . -name '*.$(SRC_EXT)' | sort -k 1nr | cut -f2-)
# Set the object file names, with the source directory stripped
# from the path, and the build path prepended in its place
OBJECTS = $(SOURCES:$(SRC_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/%.o)

all:
	$(MAKE) -C ../includes/Navio all

	$(CXX) -std=c++11 $(INCLUDES) $(OBJECTS) Drone.cpp -L$(NAVIO) -lnavio  -lrt -lpthread -lpigpio -lpthread -L. -luWS -lssl -lcrypto -lz -luv -lncurses -o Drone 

clean:
	rm -f Drone
