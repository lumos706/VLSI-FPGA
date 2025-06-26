SRC = main.cpp Graph.cpp Net.cpp Node.cpp solution.cpp evaluate.cpp
OBJ = $(SRC:.cpp=.o)

CXX = g++ 
CFLAGS = -Wall -O3 -march=native -Wextra -Werror -std=c++17 -pedantic -fopenmp
CFLAGS += -g

all: main

main: $(OBJ)
	$(CXX) $(CFLAGS) -o main $(OBJ)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -f main *.o