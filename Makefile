OBJS += thc.o
OBJS += playgame.o
OBJS += engine.o

CXX = g++ -m64 -std=c++11
CXXFLAGS = -I. -O3 -Wall -g -fopenmp

default: playchess benchmark

playchess: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

benchmark: thc.o engine.o benchmark.o
	$(CXX) $(CXXFLAGS) -o $@ thc.o engine.o benchmark.o

%.o: %.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@

clean:
	/bin/rm -rf *~ *.o $(APP_NAME) *.class