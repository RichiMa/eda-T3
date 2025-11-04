CXX = g++
CXXFLAGS = -std=gnu++11 -O2 -Wall -Wextra -Iinclude

SRC = src/main.cpp src/expr_tree.cpp src/parser.cpp
BIN = EdaCal

all: $(BIN)

$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC)

clean:
	rm -f $(BIN)
