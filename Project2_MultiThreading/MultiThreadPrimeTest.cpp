// SingleThreadPrimeTest.cpp
// By Adam R. Spangler, created on 4/15/2015
#define	__FR_VERSION_ID	"0.0.1"
// Last updated: 4/16/2015

#include<stdio.h>
#include<unistd.h>
#include<time.h>
#include<pthread.h>

#define MAX_INT		10000000
#define	NUM_THREADS	16
typedef	unsigned long long int	primeint;

struct	threadarg_t
{
	primeint	startnum;
	primeint	endnum;
};

threadarg_t	threadargarray[NUM_THREADS];
primeint	numprimes;

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
	printf("First element initialized.\n");
	for(int n = 1; n < NUM_THREADS; n++)
	{
		threadargarray[n].startnum = threadargarray[n-1].endnum + 1;
		threadargarray[n].endnum = MAX_INT/NUM_THREADS * (n + 1);
		printf("Element %d initialized.\n",n);
	}

	for(int i = 0; i < NUM_THREADS; i++)
	{
		if(pthread_create(&threads[i],NULL,chkprimes,(void*)&threadargarray[i])) printf("Thread creation error!\n");
		else printf("Thread creation succeeded\n");
	}

	while(1)
	{
		end = clock();
		cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
		printf("Primes per second: %0.2f\n",((double) numprimes) / cpu_time_used);
		primeint oldnumprimes = numprimes;
		sleep(2);
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
		currentnum++;
	}
	for(primeint currentnum = startnum; currentnum <= endnum; currentnum++); if(isprime(currentnum)) numprimes++;
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
