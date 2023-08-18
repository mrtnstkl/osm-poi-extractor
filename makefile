CXX = g++
CXXFLAGS = -std=c++2a -Wall -Wextra -O3
LDFLAGS = -lz -lpthread -lexpat -lbz2
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = poi_extract

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS)

cleano:
	rm -f $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean cleano
