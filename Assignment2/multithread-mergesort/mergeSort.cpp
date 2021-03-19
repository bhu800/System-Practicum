#include <iostream>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
using namespace std;

// --------initialize--------
int* arr;
int n,threadsCount,maxLevel;
int counter;	// check threads used

// ------- structs -------
// struct to pass information to thread
struct info{
	int lo,hi,level;	// low index, high index, tree-level(root=0)
	info(int arg1,int arg2,int arg3):lo(arg1),hi(arg2),level(arg3){}
};


// -------------- Functions -------------------------
// initialize array with random numbers
void initializeArray(){
	for(int i=0;i<n;i++)
		arr[i]=rand();
}

// print array
void print(){
	for(int i=0;i<n;i++)
		cout<<arr[i]<<" ";
	cout<<"\n";
}

// merge 2 sorted arrays in-place
// idea:- place lower element in correct place and shift rest array
void merge(int lo,int mid,int hi){
	int i=lo,j=mid;
	while(i<j && j<=hi){
		if(arr[i]<=arr[j])
			i++;
		else{
			int value=arr[j];
			for(int k=j;k>i;k--)
				arr[k]=arr[k-1];
			arr[i]=value;

			i++;j++;
		}
	}

}

// in-place merge-sort function
void mergeSort(int lo,int hi){
	if(lo>=hi)
		return ;

	int mid=lo+(hi-lo)/2;

	// sort left subarray
	mergeSort(lo,mid);

	// sort right subarray
	mergeSort(mid+1,hi);

	// merge 2 sorted subarray
	merge(lo,mid+1,hi);
}

// thread functions
void* mergeSortThread(void* arg){
	info* data=(info*)arg;

	int lo=data->lo,hi=data->hi,level=data->level;
	if(lo>=hi)
		return NULL;

	int mid=lo+(hi-lo)/2;

	// if, can create threads for two subarrays
	if(level<=maxLevel){
		// left subarray create thread & pass args
		info* temp=new info(lo,mid,level+1);
		pthread_t* thread1=new pthread_t;
		counter--;
		pthread_create(thread1,NULL,mergeSortThread,temp);

		// right subarray create thread & pass args
		info* temp2=new info(mid+1,hi,level+1);
		pthread_t* thread2=new pthread_t;
		counter--;
		pthread_create(thread2,NULL,mergeSortThread,temp2);

		// wait for 2 threads created to complete task
		pthread_join(*thread1,NULL);
		pthread_join(*thread2,NULL);

		// delete dynamic pointers
		delete thread1;
		delete thread2;

		// merge
		merge(lo,mid+1,hi);
	}
	else	// if, no further thread creation possible
		mergeSort(lo,hi);

	// delete dynamic vars
	delete data;

	return NULL;
}



// ------ main ----------
int main(int argc, char* argv[]){

	if(argc!=3)
		fprintf(stderr, "2 args expected [no-of-threads size-of-input-array]\n");

	// input
	threadsCount=stoi(argv[1]),n=stoi(argv[2]);

	// allocate array
	arr=new int[n]();
	
	// find max level with threads for each subarray
	maxLevel=(int)(log2 (threadsCount+1)) - 1;
	// round threads to nearest perfect tree / like nearest 2 power
	threadsCount=2*(int)pow(2,maxLevel) - 1;counter=threadsCount;
	// find max level which can create threads
	maxLevel--;

	// get values in array (already random)
	// initializeArray();


	// main --> 1 thread
	counter--;
	// array bounds
	int lo=0,hi=n-1,mid=lo+(hi-lo)/2;


	// ----start time----------
	long long start_s,start_us, end_s,end_us;
    struct timeval timecheck;

    gettimeofday(&timecheck, NULL);
    start_s = (long long)timecheck.tv_sec;start_us= (long long)timecheck.tv_usec;

	// 1 thread
	if(maxLevel<0){
		mergeSort(0,n-1);

		// print();
	}
	else{	// more threads
		// left subarray sort by new thread
		info* temp=new info(lo,mid,1);
		pthread_t* thread1=new pthread_t;
		counter--;
		pthread_create(thread1,NULL,mergeSortThread,temp);

		// right subarray sort by new thread
		info* temp2=new info(mid+1,hi,1);
		pthread_t* thread2=new pthread_t;
		counter--;
		pthread_create(thread2,NULL,mergeSortThread,temp2);

		// wait for 2 threads to complete task
		pthread_join(*thread1,NULL);
		pthread_join(*thread2,NULL);

		merge(lo,mid+1,hi);

		// print();

		// delete dynamic vars
		delete thread1;
		delete thread2;
	}

	// ---- end time------------
	gettimeofday(&timecheck, NULL);
    end_s = (long long)timecheck.tv_sec;end_us=(long long)timecheck.tv_usec;

  	long long mega=1e6;
    printf("%lld microseconds\n",(end_s*mega+end_us)-(start_s*mega+start_us));


	// delete dynamic array
	delete[] arr;

	// check if more threads used ?
	if(counter < 0)
		fprintf(stderr, "More threads used then expected\n");

	return 0;
}