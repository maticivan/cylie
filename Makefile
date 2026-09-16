CXX = $(shell ls /opt/homebrew/bin/g++-* | sort -V | tail -1)
 
all:
	g++ -O2 -I src -I /usr/local/include -c cylinders.cpp -std=c++23 -lstdc++fs -pthread -fmax-errors=1
	g++ -O2 -o cylie cylinders.o -std=c++23 -lstdc++fs -pthread 

forMac:
	$(CXX) -O2 -I src -c cylinders.cpp -std=c++23 -pthread -DIOF_NO_SPAWN -fmax-errors=1
	$(CXX) -O2 -o cylie cylinders.o -std=c++23 -lstdc++fs -pthread
