# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS := -lboost_system -lpthread

# Output binary
TARGET := bin/StompClient

# Object files
OBJS := bin/StompClient.o bin/ConnectionHandler.o bin/Threads.o bin/KeyboardInput.o bin/Frame.o bin/Utilities.o bin/StompProtocol.o bin/ClientDataBase.o bin/event.o 

# Default target
all: $(TARGET)

# Build the target executable
$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compile object files
bin/StompClient.o: src/StompClient.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

bin/ConnectionHandler.o: src/ConnectionHandler.cpp include/ConnectionHandler.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

bin/Threads.o: src/Threads.cpp include/Threads.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

bin/KeyboardInput.o: src/KeyboardInput.cpp include/KeyboardInput.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

bin/Frame.o: src/Frame.cpp include/Frame.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

bin/Utilities.o: src/Utilities.cpp include/Utilities.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

bin/StompProtocol.o: src/StompProtocol.cpp include/StompProtocol.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

bin/ClientDataBase.o: src/ClientDataBase.cpp include/ClientDataBase.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

bin/event.o: src/event.cpp include/event.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<



# Clean target
.PHONY: clean
clean:
	rm -f bin/*.o $(TARGET)
