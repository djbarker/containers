all:
	make default

debug:
	g++ example.cpp -std=c++11 -o example_d -g

default:
	g++ example.cpp -std=c++11 -o example -g