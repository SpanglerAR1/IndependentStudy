// SingleThreadPrimeTest.cpp
// By Adam R. Spangler, created on 4/15/2015
#define	__FR_VERSION_ID	"0.0.1"
// Last updated: 4/16/2015

#include<stdio.h>
#include<unistd.h>
#include<time.h>
#include<pthread.h>

#define 	MAX_INT		10000000
#define		NUM_THREADS	2
#typedef	primeint	unsigned long long int

struct	threadarg_t
{
	primeint	startnum;
	primeint	endnum;
};

threadarg_t	threadargarray[NUM_THREADS];
unsigned int	numprimes;

void*	chkprimes	(void* threadarg);
int	isprime		(primeint num);

int main(int argc, char* argv[])
{
	printf("Welcome to Adam Spangler's prime number generator, version %s\n",__FR_VERSION_ID);
	printf("Today, I will be generating primes up to %d, ",MAX_INT);
	printf("using %d threads.\n",NUM_THREADS);

	clock_t start = clock();
	clock_t end;
	double cpu_time_used = 0;

	numprimes = 0;
	pthread_t threads[NUM_THREADS];
	threadargarray[0].startnum = 0;
	threadargarray[0].endnum = MAX_INT/NUM_THREADS;
	for(int n = 1; n < NUM_THREADS; n++)
	{
		threadargarray[n].startnum = threadargarray[n-1] + 1;
		threadargarray[n].endnum = MAX_INT/NUM_THREADS * (n + 1);
	}

	for(int i = 0; i < NUM_THREADS; i++)
	{
		pthread_create(&threads[i],NULL,chkprimes,(void*)&threadargarray[i]);
	}

	while(1)
	{
		cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
		printf("\rPrimes per second: %f",((double) numprimes) / cpu_time_used);
		start = clock();
		sleep(1);
	}

	printf("\n");
	pthread_exit(NULL);
}

void*	chkprimes	(void* threadarg)
{
	threadarg_t* input_arguments = (threadarg_t*)threadarg;
	primeint startnum = input_arguments->startnum;
	primeint endnum = input_arguments->endnum;
	for(primeint currentnum = startnum; currentnum <= endnum; currentnum++); if(isprime(currentnum)) numprimes++;
	pthread_exit(0);
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
