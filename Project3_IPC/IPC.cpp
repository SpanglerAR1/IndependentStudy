// IPC.cpp
// By Adam R. Spangler, created on 4/25/2015
#define	__IPC_VERSION_ID	"0.0.1"
// Last updated: 4/25/2015

#include<stdio.h>
#include<unistd.h>
#include<time.h>
#include<pthread.h>
#include<stdlib.h>

#define MAX_INT			5000000
#define	REMOTE_HOSTS		4
#define LOCAL_THREADS		12

int	isMaster;

typedef	unsigned long long int	primeint;

struct	threadarg_t
{
	int		threadid;
	primeint	startnum;
	primeint	endnum;
};

threadarg_t	threadargarray[LOCAL_THREADS];
primeint	numprimes;
primeint	intschecked[LOCAL_THREADS];

void*	chkprimes	(void* threadarg);
int	isprime		(primeint num);

int main(int argc, char* argv[])
{
	if((argc == 2) && (argv[1] == "master"))
	{
		num_threads = atol(argv[1]);
		if(num_threads == 0) num_threads = NUM_THREADS;
	}
	else num_threads = NUM_THREADS;
	printf("Welcome to Adam Spangler's prime number generator, version %s\n",__FR_VERSION_ID);
	printf("Today, I will be generating primes up to %d, ",MAX_INT);
	printf("using %d threads.\n",num_threads);

	clock_t start = clock();
	clock_t end;
	double cpu_time_used = 0;

	numprimes = 0;
	intschecked[0] = 0;
	pthread_t threads[NUM_THREADS];
	threadargarray[0].startnum = 0;
	threadargarray[0].endnum = MAX_INT / num_threads;
	threadargarray[0].threadid = 0;
	printf("Worker 0 will be iterating through %d to %d\n",threadargarray[0].startnum,threadargarray[0].endnum);
	for(int n = 1; n < num_threads; n++)
	{
		threadargarray[n].startnum = threadargarray[n-1].endnum + 1;
		threadargarray[n].endnum = MAX_INT/num_threads * (n + 1);
		printf("Worker %d will be iterating through %d to %d\n",n,threadargarray[n].startnum,threadargarray[n].endnum);
		intschecked[n] = 0;
		threadargarray[n].threadid = n;
	}

	for(int i = 0; i < num_threads; i++)
	{
		if(pthread_create(&threads[i],NULL,chkprimes,(void*)&threadargarray[i])) printf("Thread creation error!\n");
		else printf("Thread %d creation succeeded\n",i);
	}

	primeint numintschkd = 0;
	while(1)
	{
		//end = clock();
		//cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
		//printf("Primes per second: %0.2f\t",((double) numprimes) / cpu_time_used);
		for(int j = 0; j < num_threads; j++)
		{
			printf("Worker %d: %d\t",j,intschecked[j]);
			numintschkd = numintschkd + intschecked[j];
			intschecked[j] = 0;
		}
		printf("%d integers examined.\n",(int)numintschkd);
		primeint oldnumprimes = numprimes;
		sleep(1);
		primeint newnumprimes = numprimes;
		if(oldnumprimes == newnumprimes) break;
	}

	printf("\n");
	pthread_exit(NULL);
}

void*	chkprimes	(void* threadarg)
{
	threadarg_t* input_arguments = (threadarg_t*)threadarg;
	primeint startnum = input_arguments->startnum;
	primeint endnum = input_arguments->endnum;
	primeint currentnum = startnum;
	while(currentnum <= endnum)
	{
		if(isprime(currentnum)) numprimes++;
		intschecked[input_arguments->threadid] += 1;
		currentnum++;
	}
	pthread_exit(NULL);
}

int isprime(primeint num)
{
	if(num <= 1) return 0; // zero and one are not prime
	if((num == 2) || (num == 3)) return 1;
	if(!(num & 1)) return 0; // If a binary number is even, i.e. has a 0 in the LSB, it is divisible by 2 and not prime. 
	unsigned int i;
	for (i=5; i*i<=num; i=i+2) if (num % i == 0) return 0;
	return 1;
}

