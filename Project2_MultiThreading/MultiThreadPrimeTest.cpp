// SingleThreadPrimeTest.cpp
// By Adam R. Spangler, created on 4/15/2015
#define	__FR_VERSION_ID	"0.0.1"
// Last updated: 4/15/2015

#include<stdio.h>
#include<time.h>
#include<unistd.h>
#include<wait.h>

#define MAX_INT	10000000

int	chkprime	(unsigned long long int num);

int main(int argc, char* argv[])
{
	printf("Welcome to Adam Spangler's prime number generator, version %s\n",__FR_VERSION_ID);
	printf("Today, I will be generating primes up to %d\n",MAX_INT);

	unsigned long long int currentnum;
	unsigned long long int endint;
	int isparent;
	pid_t PID = fork();
	if(PID)
	{
		// Parent Process		
		currentnum = 0;
		endint = MAX_INT / 2;
		isparent = 1;
	}
	else
	{
		currentnum = MAX_INT / 2 + 1;
		endint = MAX_INT;
		isparent = 0;
	}
	
	
	clock_t start = clock();
	clock_t end;
	int numprimes = 0;
	double cpu_time_used = 0;
	
	while(currentnum++ <= endint)
	{
		if(chkprime(currentnum))
		{
			numprimes++;
			//printf("%d\n",(int)currentnum);
		}
		end = clock();
		if((end-start >= 10000) && isparent)
		{
			cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
			printf("\rCurrent Prime: %d",(int)currentnum);
			printf("  Primes per second: %f",((double) numprimes) / cpu_time_used);
			start = clock();
		}
	}
	printf("\n");

	if(isparent) waitpid(PID,NULL,0);
	return 0;
}

int chkprime(unsigned long long int num)
{
	if(num <= 1) return 0; // zero and one are not prime
	if((num == 2) || (num == 3)) return 1;
	if(!(num & 1)) return 0; // If a binary number is even, i.e. has a 0 in the LSB, it is divisible by 2 and not prime. 
	unsigned int i;
	for (i=5; i*i<=num; i=i+2) if (num % i == 0) return 0;
	return 1;
}
