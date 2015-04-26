// IPC_Server.cpp
// By Adam R. Spangler, created on 4/26/2015
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

#define	PORT		1000
#define	MAXMSG		512
#define	MAXCLIENTS	8

//These two functions, along with most of main(), come from the GNU C library manual
int	read_from_client	(int filedes);
int 	make_socket		(uint16_t port);

int main(void)
{
	/* Create the socket and set it up to accept connections. */
	int sock;
	sock = make_socket (PORT);
	if (listen(sock,MAXCLIENTS) < 0)
	{
		perror ("Failed to set up listening functionality on the server side\n");
		exit (EXIT_FAILURE);
	}

	/* Initialize the set of active sockets. */
	fd_set active_fd_set;
	fd_set read_fd_set;
	// Initialize active_fd_set to zero
	FD_ZERO (&active_fd_set);
	// Add sock to the active file descriptor set
	FD_SET (sock, &active_fd_set);

	while (1)
	{
		/* Block until input arrives on one or more active sockets. */
		read_fd_set = active_fd_set;
		if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
		{
			perror ("Select error: failed to wait for input on server side\n");
			exit (EXIT_FAILURE);
		}

		/* Service all the sockets with input pending. */
		int i;
		for (i = 0; i < FD_SETSIZE; ++i)
		{
			if(FD_ISSET(i,&read_fd_set))
			{
				if (i == sock)
				{
					/* Connection request on original socket. */
					int receivesocket;
					struct sockaddr_in clientname;
					size_t size;
					size = sizeof(clientname);
					receivesocket = accept (sock,(struct sockaddr *) &clientname,(socklen_t *)&size);
					if (receivesocket < 0)
					{
						perror ("Incoming connection attempt failed, server accept function\n");
						exit (EXIT_FAILURE);
					}
					fprintf (stderr,"Server: connect from host %s, port %hd.\n",inet_ntoa (clientname.sin_addr),ntohs(clientname.sin_port));
					FD_SET(receivesocket,&active_fd_set);
				}
			}
			else
			{
				/* Data arriving on an already-connected socket. */
				if(read_from_client(i) < 0)
				{
					close(i);
					FD_CLR(i, &active_fd_set);
				}
			}
		}
	}
}

int read_from_client(int filedes)
{
	char buffer[MAXMSG];
	int nbytes;
	nbytes = read(filedes,buffer,MAXMSG);
	if (nbytes < 0)
	{
		perror ("Server side read error\n");
		exit (EXIT_FAILURE);
	}
	else if (nbytes == 0) return -1; // End-of-file.
	else
	{
		/* Data read. */
		fprintf (stderr, "Server: got message: ‘%s’\n", buffer);
		return 0;
	}
}

int make_socket(uint16_t port)
{
	/* Create the socket. */
	int sock;
	sock = socket (PF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror ("Failed to create a new socket\n");
		exit (EXIT_FAILURE);
	}

	/* Give the socket a name. */
	struct sockaddr_in name;
	name.sin_family = AF_INET;
	name.sin_port = htons(port);
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind (sock, (struct sockaddr *) &name, sizeof (name)) < 0)
	{
		perror ("Failed to bind to the new socket\n");
		exit (EXIT_FAILURE);
	}
	return sock;
}


/*
#include<stdio.h>
#include<unistd.h>
#include<time.h>
#include<pthread.h>
#include<stdlib.h>

#define MAX_INT			5000000
#define	REMOTE_HOSTS		4
#define LOCAL_THREADS		12

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

*/
