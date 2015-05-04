// IPC_Client.cpp
// By Adam R. Spangler, created on 4/25/2015
#define	__IPC_VERSION_ID	"0.0.1"
// Last updated: 4/26/2015

#include	<stdio.h>
#include	<errno.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<netdb.h>
#include	<string.h>
#include	<pthread.h>

#define 	PORT		3000
#define 	LOCAL_THREADS	12

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

void	exiterr(const char* message)
{
	perror(message);
	exit(EXIT_FAILURE);
}

void*	xmalloc(size_t size)
{
	void* newptr = malloc(size);
	if(newptr == NULL) exiterr("Call to malloc failed, no mem");
	return newptr;
}

void 	init_sockaddr	(struct sockaddr_in *name,const char *hostname,uint16_t port);

int main(int argc, char* argv[])
{
	if(argc != 2) exiterr("Must specify server address\n");
	char* SERVERHOST = argv[1];

	// Create the socket.
	int sock = socket(PF_INET,SOCK_STREAM,0);
	if (sock < 0) exiterr("Socket creation failure, client side\n");

	// Connect to the server.
	struct sockaddr_in servername;
	init_sockaddr(&servername,SERVERHOST,PORT);
	if (0 > connect(sock, (struct sockaddr*)&servername, sizeof(servername)) ) exiterr("Client side connection failure\n");

	// Initialize local data structures and create threads
	numprimes = 0;
	intschecked[0] = 0;
	pthread_t threads[NUM_THREADS];

	// Receive a job from the server
	primeint* job = (primeint*)xmalloc(sizeof(primeint) * 2);
	do
	{
		read(sock,job,sizeof(primeint) * 2);
		threadargarray[0].startnum = *job;
		threadargarray[0].endnum = MAX_INT / num_threads;
		threadargarray[0].threadid = 0;
		for(int n = 1; n < num_threads; n++)
		{
			threadargarray[n].startnum = threadargarray[n-1].endnum + 1;
			threadargarray[n].endnum = MAX_INT/num_threads * (n + 1);
			intschecked[n] = 0;
			threadargarray[n].threadid = n;
		}
		for(int i = 0; i < num_threads; i++)
			if(pthread_create(&threads[i],NULL,chkprimes,(void*)&threadargarray[i]))
				exiterr("Thread creation error!\n");
		
	}while(*job != 0)


	close(sock);
	exit(EXIT_SUCCESS);
}

void init_sockaddr(struct sockaddr_in *name,const char *hostname,uint16_t port)
{
	struct hostent *hostinfo;
	name->sin_family = AF_INET;
	name->sin_port = htons (port);
	hostinfo = gethostbyname (hostname);
	if (hostinfo == NULL)
	{
		fprintf (stderr, "Unknown host %s.\n", hostname);
		exit (EXIT_FAILURE);
	}
	name->sin_addr = *(struct in_addr *) hostinfo->h_addr;
}







int main(int argc, char* argv[])
{

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

*/
