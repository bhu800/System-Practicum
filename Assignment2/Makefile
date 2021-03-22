all: mergeSort RR

mergeSort: ./multithread-mergesort/mergeSort.cpp
	g++ ./multithread-mergesort/mergeSort.cpp -o mergeSort -lpthread

RR: ./Round-Robin-Scheduling/RR_Sim.cpp
	g++ ./Round-Robin-Scheduling/RR_Sim.cpp -o RR


clean:
	rm mergeSort RR