SRC = main.cpp Design.cpp FPGA.cpp FpgaTile.cpp Net.cpp RRNode.cpp Solution.cpp
OBJ = $(SRC:.cpp=.o)

CXX = g++
CFLAGS = -Wall -Wextra -std=c++17 -pedantic
CFLAGS += -g

all: main

main: $(OBJ)
	$(CXX) $(CFLAGS) -o main $(OBJ)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -f main *.o