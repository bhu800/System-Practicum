// run:-
// gcc a.cpp -lm -o a -lpthread


// Note:-
// students are sitting around table
// #students = #spoons = N
// child at index i can acquire spoons at index {i, (i+1)%N}


// ----Mutually Exclusive----
// 1 spoon is used by 1 child

// -----Deadlock Prevention----
// child try to get both spoons, if gets both then continue else drop spoon if has any and try again later

// ----Starvation handling----
// Each child has some priority(or value = timestamp from whcih it's waiting to get next turn)
// if its neigbours are waiting from long let them use spoon first

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
using namespace std;


// -----------------------------------------------------------
// count of students or spoons or threads
const int N=10;

// info passed to each thread
struct data{
	int child_number;

	data(int value=0):child_number(value){}
};

// status of students
char status[N][50]={"Waiting for spoons\n","One spoon acquired\n","Both spoons acquired and eating(20sec)\n","Thinking(<2sec)\n"};
// store status index for each child
int child_status[N];

// child priority to handle starvation
long long child_priority[N];

// spoon mutex_lock (to lock/unlock spoon)
pthread_mutex_t spoon_lock[N];

// 2spoon - lock (only one child can get both spoons at a time)
pthread_mutex_t spoonBoth;

// threads for each student
pthread_t threads[N];

// conditional wait for childs - to wait & let starving child neighbour get spoon first
pthread_cond_t child_wait[N];

// mutex_lock for printing synchronously
pthread_mutex_t general = PTHREAD_MUTEX_INITIALIZER;
// structure of code for synchronous printing:-
// ******
// pthread_mutex_lock(&general); 
// ....code......;
// pthread_mutex_unlock(&general);
// ******

// array of  struct-data (index of child) --> info passed to thread
data which_child[N];

// check allocation of spoons to each child
int allocation_count[N];


int spoon_ct[N];	// count of spoons with each child
long long t0;	// store time in sec

//-----------Functions-----------------------------

// print status/state
// int counter=1;
void print_status(int thread_no=-1,int state=-1){
	// printf("thread_no-%d ; state-%d\n",thread_no,state);
	for(int i=0;i<N;i++)
		printf("S%d: %s",i+1,status[child_status[i]]);
	// printf("%d -------------------\n",counter++);
	printf("------------------\n");
}

// validity to get both spoons -> both spoons available, neighbours not starving
// 20 is time limit for neigbour to wait, which if exceeded neighbour will get priority
// value 20 can be adjusted as per requirements
bool valid(int current,int left,int right){
	return (spoon_ct[left]!=2 && spoon_ct[right]!=2 && !(child_priority[current] - child_priority[left]>=20)
			&& !(child_priority[current] - child_priority[right]>=20));
}

// thread function
void *mythread(void *arg){

	// get parameters of thread
	data* info=(data*)arg;

	// child index
	int index=info->child_number;

	// left & right spoons index
	int l=index,r=(index+1)%N;
	int lc=(index-1+N)%N,rc=(index+1)%N;

	// loop indefinitely
	while(1){

		// get both spoons
		// loop until get both spoons
		while(1){

			// get both spoons
			pthread_mutex_lock(&spoonBoth);

			// wait while can't get BOTH spoons
			while(!valid(index,lc,rc)){
				pthread_cond_wait(&child_wait[index],&spoonBoth);
			}

			// lock both spoons
			pthread_mutex_trylock(&spoon_lock[l]);
			pthread_mutex_trylock(&spoon_lock[r]);

			// update spoon count with this child
			spoon_ct[index]=2;
			pthread_mutex_unlock(&spoonBoth);


			// update status for spoons and print status
			pthread_mutex_lock(&general);
			child_status[index]=1;
			print_status(index,1);
			child_status[index]=2;
			print_status(index,2);
			pthread_mutex_unlock(&general);
			break;
		}

		// Both spoons acquired and eating
		sleep(20);						// wait 20 seconds


		// put down both spoons and think
		pthread_mutex_lock(&general);

		pthread_mutex_unlock(&spoon_lock[l]);		// unlock spoon index l
		pthread_mutex_unlock(&spoon_lock[r]);		// unlock spoon index r

		child_status[index]=3;			// update status
		print_status(index,3);			// print change in status
		pthread_mutex_unlock(&general);
		
		// update and call threads waiting on condition
		child_priority[index]=time(0);		// update priority
		allocation_count[index]++;			// update aloocation/#times eaten
		spoon_ct[index]=0;					// update spoon count with this child(released spoons)

		// signal that it has eaten & neighbours can use the spoons
		pthread_cond_signal(&child_wait[lc]);
		pthread_cond_signal(&child_wait[rc]);

		// think < 2sec
		double random_value = (rand()%2000)/(double)1000;		// get random value < 2
		sleep(random_value);									// wait for <2secs


		// wait again for turn
		pthread_mutex_lock(&general);
		child_status[index]=0;			// update status
		print_status(index,0);			// print change in status
		pthread_mutex_unlock(&general);
	}

	return NULL;
}

// ---analysis function to check fair-allocation---
// pthread_t analysis;
// void *analysisThread(void *arg){

// 	while(1){
// 		pthread_mutex_lock(&general);
// 		printf("*******Analysis********\n");
// 		printf("Allocation count to check fair allocation/starvation:-\n");
// 		for(int i=0;i<N;i++)
// 			printf("%d ",allocation_count[i]);
// 		printf("\n---------\n");
// 		for(int i=0;i<N;i++)
// 			printf("%lld ",child_priority[i]);
// 		printf("\n---------\n");
// 		for(int i=0;i<N;i++)
// 			printf("%d ",spoon_ct[i]);
// 		printf("\n********************\n");
// 		pthread_mutex_unlock(&general);

// 		sleep(50);
// 	}

// 	return NULL;
// }

// -------------------------------------------------------------------------------------

int main(){

	t0=time(0);
	// Initilaize locks & wait conditions
	for(int i=0;i<N;i++){
		spoon_lock[i]=PTHREAD_MUTEX_INITIALIZER;
		child_wait[i]=PTHREAD_COND_INITIALIZER;
		child_priority[i]=t0;
	}

	// initially all waiting
	print_status();

	// create & call threads
	for(int i=0;i<N;i++){
		which_child[i].child_number=i;		// set child index to argument passed to thread
		pthread_create(&threads[i],NULL,mythread,&which_child[i]);	// call thread
	}


	// call Analysis thread
	// pthread_create(&analysis,NULL,analysisThread,NULL);
	
	// dont't exit, wait indefinitely
	pthread_join(threads[0],NULL);

	return 0;
}