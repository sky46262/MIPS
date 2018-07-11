cc = g++
prog = main
($prog) : main.cpp
	g++ main.cpp parser.cpp program.cpp statement.cpp -o code -std=c++14 -O2
clean :
	rm -rf code