# Compiler settings - Can change to clang++ if preferred
CXX = g++
CXXFLAGS = -std=c++11 -Wall -pthread -g

# Build targets
TARGET = threadpool
SRC = thread-pool.cc Semaphore.cc tptest.cc

# Default target
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Si querés compilar específicamente con el test personalizado:
custom:
	$(CXX) $(CXXFLAGS) -o $(TARGET) thread-pool.cc Semaphore.cc tptest.cc

# Clean up build artifacts
clean:
	rm -f $(TARGET)

.PHONY: all clean custom
