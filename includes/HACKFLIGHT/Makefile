CXX = g++
CFLAGS = -std=c++11 -Wno-psabi -hpp -I .

SRC=$(wildcard */*.hpp)
OBJECTS = $(SRC:.hpp=.o) 

%.o: %.cpp
	$(CXX) $(CFLAGS) -o $@ $< 

all: $(OBJECTS)
	
	ar rcs libnavio.a $(OBJECTS)

clean:
	rm -f */*.o *.a
