#include <iostream>
#include <vector>

using namespace std;

/*
 * Function: simulates Round-Robin CPU scheduling algorithm (preemptive)
 * and prints various scheduling metrices
 * ---------------------------------------------------------
 * 
 * Input - 
 * 		N: 				number of processes
 * 		arrival: 		arrival time of each process
 * 		burst:			burst time of each process
 * 		time_quantum:	time quantum of Round-Robin algorithm
*/
void simulateRoundRobinScheduler(int N, vector<int> arrival, vector<int> burst, int time_quantum)
{
    int t = 0, idx = 0;
	vector<int> completion(N), first_run(N), rem_burst(burst.begin(), burst.end());
	cout<<"\nRound-Robin Schduling - "<<endl;
	while(1)
	{
		int flag = 1;

		for (int i=0; i<N; i++)
		{
			if (rem_burst[(idx+i)%N] != 0)
			{
				flag = 0;

				if (arrival[(idx+i)%N] <= t)
				{
					idx = (idx+i)%N;
					cout<<idx<<" ";

					if (rem_burst[idx] == burst[idx]) first_run[idx] = t;

					if (rem_burst[idx] <= time_quantum) 
					{
						t += rem_burst[idx];
						rem_burst[idx] = 0;
						completion[idx] = t;
					}
					else 
					{
						t += time_quantum;
						rem_burst[idx] -= time_quantum;
					}

					break;
				}
			}
		}

		idx += 1;

		if (flag) break;
	}
	cout<<endl;
	float ATT = 0, AWT = 0, ART = 0;
	cout<<"\n===== Scheduling Metrices for each process =====\n";
	for (int i=0; i<N; i++)
	{
		int TT = completion[i] - arrival[i];
		int WT = TT - burst[i];
		int RT = first_run[i] - arrival[i];

		ATT += TT;
		AWT += WT;
		ART += RT;

		cout<<"Process "<<i+1<<" - \n";
		cout<<"		Start time - "<<arrival[i]<<" \n";
		cout<<"		Completion time - "<<completion[i]<<" \n";
		cout<<"		Turnaround time - "<<TT<<" \n";
		cout<<"		Waiting time - "<<WT<<" \n";
		cout<<"		Response time - "<<RT<<" \n";
	}
	ATT /= N;
	AWT /= N;
	ART /= N;

	cout<<"\n===== Average Scheduling Metrices =====\n";
	cout<<"Average Turnaround time - "<<ATT<<" \n";
	cout<<"Average Waiting time - "<<AWT<<" \n";
	cout<<"Average Response time - "<<ART<<" \n";
	cout<<endl;
}

int main()
{
    vector<int> arr = {0, 10, 10, 80, 85}, burst = {85, 30, 35, 20, 50};
    simulateRoundRobinScheduler(arr.size(), arr, burst, 10);

    return 0;
}