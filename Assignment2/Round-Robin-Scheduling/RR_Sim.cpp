#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <iomanip>

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
void simulateRoundRobinScheduler(int N, vector<int> arrival, vector<int> burst, int time_quantum, int btw_tq_comp = false)
{
    int t = 0, idx = 0, cpu_idle = 0;
	vector<int> completion(N), first_run(N), rem_burst(burst.begin(), burst.end());
	if (!btw_tq_comp) cout<<"\nRound-Robin Schduling - "<<endl;
	while(1)
	{
		int flag1 = 1, flag2 = 1;

		for (int i=0; i<N; i++)
		{
			if (rem_burst[(idx+i)%N] != 0)
			{
				flag1 = 0;

				if (arrival[(idx+i)%N] <= t)
				{
					flag2 = 0;
					idx = (idx+i)%N;
					if (!btw_tq_comp) cout<<idx<<" ";

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

		if (flag2 && !flag1) 
		{
			if (!btw_tq_comp) cout<<"<idle ("<<t<<")> ";
			t++;
			cpu_idle++;
			idx = 0;
		}
		else idx++;
		if (flag1) break;
	}
	if (!btw_tq_comp) cout<<endl;
	float ATT = 0, AWT = 0, ART = 0;
	if (!btw_tq_comp) cout<<"\n===== Scheduling Metrices for each process =====\n";
	for (int i=0; i<N; i++)
	{
		int TT = completion[i] - arrival[i];
		int WT = TT - burst[i];
		int RT = first_run[i] - arrival[i];

		ATT += TT;
		AWT += WT;
		ART += RT;

		if (!btw_tq_comp) cout<<"Process "<<i+1<<" - \n";
		if (!btw_tq_comp) cout<<"		Start time - "<<arrival[i]<<" \n";
		if (!btw_tq_comp) cout<<"		Completion time - "<<completion[i]<<" \n";
		if (!btw_tq_comp) cout<<"		Turnaround time - "<<TT<<" \n";
		if (!btw_tq_comp) cout<<"		Waiting time - "<<WT<<" \n";
		if (!btw_tq_comp) cout<<"		Response time - "<<RT<<" \n";
	}
	ATT /= N;
	AWT /= N;
	ART /= N;

	if (!btw_tq_comp) cout<<"\n===== Average Scheduling Metrices =====\n";
	if (!btw_tq_comp) cout<<"Average Turnaround time - "<<ATT<<" \n";
	if (!btw_tq_comp) cout<<"Average Waiting time - "<<AWT<<" \n";
	if (!btw_tq_comp) cout<<"Average Response time - "<<ART<<" \n";
	if (!btw_tq_comp) cout<<endl;

	float t_end = *max_element(completion.begin(), completion.end());
	float throughput = N/t_end;
	float cpu_utilization = ((t_end - cpu_idle)/t_end)*100;

	if (!btw_tq_comp) cout<<"Throughput - "<<throughput<<" \n";
	if (!btw_tq_comp) cout<<"CPU Utilization - "<<cpu_utilization<<"%\n";
	if (!btw_tq_comp) cout<<"\n===============================\n";
	cout << std::fixed << std::setprecision(1);
	if (btw_tq_comp)
	{
		cout<<time_quantum<<" \t "<<ATT<<" \t "<<AWT<<" \t "<<ART<<"\n";
	}
}

vector<int> makeRandomArray(int n, int l, int h)
{
	vector<int> V(n);
	srand((unsigned)time(0));
	for (int i=0; i<n; i++)
	{
		V[i] = l + (rand()%(h-l+1));
	}
	return V;
}

void compareForMultipleTimeQuantums()
{
	int N, tq;
	cout<<"Enter number of processes - ";
	cin>>N;
	cout<<"Enter number of time quantums - ";
	cin>>tq;
	vector<int> arr(N), burst(N), timeQ(tq);
	cout<<"Enter arrival times - ";
	for (int i=0; i<N; i++) cin>>arr[i];
	cout<<"Enter burst time - ";
	for (int i=0; i<N; i++) cin>>burst[i];
	cout<<"Enter time quantums - ";
	for (int i=0; i<tq; i++) cin>>timeQ[i];
	cout<<"TQ \t ATT \t AWT \t ART\n";
	for (int i=0; i<tq; i++) simulateRoundRobinScheduler(N, arr, burst, timeQ[i], true);
}

int main()
{
	// int N, tq;
	// cout<<"Enter number of processes - ";
	// cin>>N;
	// cout<<"Enter time quantum - ";
	// cin>>tq;
	// vector<int> arr(N), burst(N);
	// cout<<"Enter arrival times - ";
	// for (int i=0; i<N; i++) cin>>arr[i];
	// cout<<"Enter burst time - ";
	// for (int i=0; i<N; i++) cin>>burst[i];
	// simulateRoundRobinScheduler(N, arr, burst, tq);

	compareForMultipleTimeQuantums();

    return 0;
}