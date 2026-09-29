CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -lpthread
TARGET = planificador

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET)

clean:
	rm -f $(TARGET)
