#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <utility>
#include <chrono>
using namespace std;

int n,m,z;
//n is size of matrix and m is the number of threads
//z is the number of rows of matrix handled by each thread

vector <vector<long long> > a,b,ansSequential,ansParallel;
//using vectors as an abstraction for dynamic allocation of memory

void* parallel(void* arg){

	//each slot is the set of z rows on which a particular thread is working
	int slot=*((int*)arg);

	for(int i=slot*z;i<slot*z+z;i++){
		if(i>=n) break;

		//dynamic resizing of the vector
	 	ansParallel[i].resize(n);

	 	//applying the naive algorithm for matrix-multiplication here
		for(int j=0;j<n;j++){
			for(int k=0;k<n;k++){
				ansParallel[i][j]+=a[i][k]*b[k][j];
			}
		}
	}

	//returning dummy variable as return value is not required
	void* dummy;
	pthread_exit(dummy);
}

void sequenctial(){

	for(int i=0;i<n;i++){
		ansSequential[i].resize(n);

		//Naive O(n^3) matirix multiplication algorithm
		//ans(i,j) is dot product of the ith row vector and jth column vector
		for(int j=0;j<n;j++){
			for(int k=0;k<n;k++){
				ansSequential[i][j]+=a[i][k]*b[k][j];
			}
		}
	}	
}

int main(int argc,char* argv[]){


	//validating command line arguments
	if(argc<3){
		printf("Enter Input size and Number of threads\n");
		exit(-1);
	}

	//stoi function converts the string to integer
	n=stoi(argv[1]);
	m=stoi(argv[2]);
	
	z=n/m+(n%m!=0); //ceil of n/m

	a.resize(n);
	b.resize(n);
	ansSequential.resize(n);
	ansParallel.resize(n);

	//filling two 2d arrays with random values
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++){
			a[i].push_back(rand());
			b[i].push_back(rand());
		}
	}


	//SEQUENTIAL

	//using chrono c++ library to measure the time taken by a particular section of the program
	auto startSequential = chrono::steady_clock::now();

	sequenctial();

	auto endSequential = chrono::steady_clock::now();
	auto diffSequential = endSequential - startSequential;
	double totalSequential=chrono::duration <double, milli> (diffSequential).count();



	//PARALLEL
	auto startParallel = chrono::steady_clock::now();

	//array of m threads
	pthread_t* threads;
	threads=(pthread_t*)malloc(m*sizeof(pthread_t));


	//storing the arguments to be passed in thread_create call
	int* posArr;
	posArr=(int*)malloc(m*sizeof(int));
	for(int i=0;i<m;i++) posArr[i]=i;


	//pthread_create(address of the thread, NULL, function to be run by the thread, address of the arguments in the thread function)
	for(int i=0;i<m;i++){
		pthread_create(&(threads[i]),NULL,parallel,&posArr[i]);
	}

	//joining the threads with the main program
	void* status;
	for(int i=0;i<m;i++){
		pthread_join(threads[i],&status);
	}

	auto endParallel = chrono::steady_clock::now();
	auto diffParallel = endParallel - startParallel;
	double totalParallel=chrono::duration <double, milli> (diffParallel).count();




	//Format of the log file" | n | m | T(sequential) | T(parallel) | "

	const char* fName="log.txt";
	FILE* fp=fopen(fName,"a");

	fprintf(fp, "| %d | %d | %f ms | %f ms |\n", n,m,totalSequential,totalParallel);

	fclose(fp);



	// //checker for multithreading bugs
	// for(int i=0;i<n;i++){
	// 	for(int j=0;j<n;j++){
	// 		if(ansParallel[i][j]!=ansSequential[i][j]){
	// 			cout<<ansSequential[i][j]<<" "<<ansParallel[i][j]<<endl;
	// 		}
	// 	}
	// }


	//printing the running time in the console as well
	cout<<"Sequential algorithm: "<<totalSequential<<" ms\n";
	cout<<"Parallel   algorithm: "<<totalParallel<<" ms\n";
	
}