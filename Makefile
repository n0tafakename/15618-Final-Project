# APP_NAME=demo
# OBJS += demo.o

APP_NAME=playchess
OBJS += thc.o
OBJS += playgame.o
OBJS += engine.o

CXX = g++ -m64 -std=c++11
CXXFLAGS = -I. -O3 -Wall -Wno-unknown-pragmas -g

default: $(APP_NAME)

$(APP_NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@

clean:
	/bin/rm -rf *~ *.o $(APP_NAME) *.class