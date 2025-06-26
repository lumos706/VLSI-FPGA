SRC = Object.cpp Arch.cpp Solution.cpp main.cpp Global.cpp
OBJ = $(SRC:.cpp=.o)
CC = g++

CFLAGS = -Wall -Wextra -std=c++17
CFLAGS += -g

all: main

main: $(OBJ)
	$(CC) $(CFLAGS) -o main $(OBJ)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o main