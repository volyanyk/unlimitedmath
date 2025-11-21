CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = -lncurses

SRC = main.cpp Game.cpp MathGenerator.cpp UI.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = unlimitedmath

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) tests

test: MathGenerator.o
	$(CXX) $(CXXFLAGS) tests.cpp MathGenerator.o -o tests
	./tests

.PHONY: all clean test
