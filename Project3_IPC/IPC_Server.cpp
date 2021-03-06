// IPC_Server.cpp
// By Adam R. Spangler, created on 4/26/2015
#define	__IPC_VERSION_ID	"0.0.1"
// Last updated: 5/3/2015

#include	<stdio.h>
#include	<errno.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<netdb.h>

#define MAX_INT		50000000
#define JOB_SIZE	 1000000
#define PORT		3000
#define MAX_CLIENTS	24

void exiterr(const char* message)
{
	perror(message);
	exit(EXIT_FAILURE);
}

void* xmalloc(size_t size)
{
	void* newptr = malloc(size);
	if(newptr == NULL) exiterr("Malloc error");
	else return newptr;
}

//These two functions, along with some of main(), come from the GNU C library manual
int 	make_socket		(uint16_t port);
void	send_job		(int filedes);
int		receive_results		(int filedes);


typedef	uint32_t	primeint;

primeint	maxint;
primeint	jobsize;
int		port;

primeint	numprimes;	// Number of primes found
primeint	numints;	// Number of ints actually checked
primeint	latest;		// Next int waiting to be sent in a job

int main(int argc, char* argv[])
{
	printf("Welcome to Adam Spangler's prime number generator, version %s\n",__IPC_VERSION_ID);
	if(argc == 1) printf("Argument order: max int, job size, port.\n");
	// Parse arguments
	switch(argc)
	{
		default:
		case 1:
			maxint = MAX_INT;
			jobsize = JOB_SIZE;
			port = PORT;
			break;
		case 2:
			if(atol(argv[1]) > 0) maxint = atol(argv[1]);
			else maxint = MAX_INT;
			jobsize = JOB_SIZE;
			port = PORT;
			break;
		case 3:
			if(atol(argv[1]) > 0) maxint = atol(argv[1]);
			else maxint = MAX_INT;
			if(atol(argv[2]) > 0) jobsize = atol(argv[2]);
			else jobsize = JOB_SIZE;
			port = PORT;
			break;
		case 4:
			if(atol(argv[1]) > 0) maxint = atol(argv[1]);
			else maxint = MAX_INT;
			if(atol(argv[2]) > 0) jobsize = atol(argv[2]);
			else jobsize = JOB_SIZE;
			if(atol(argv[3]) > 0) port = atol(argv[3]);
			else port = PORT;
	}

	printf("Today, I will be generating primes up to %d, ",(int)maxint);
	printf("using a batch size of %d integers.\n",(int)jobsize);

	latest = 1;
	numprimes = 0;
	numints = 0;

	// Set up listening functionality
	int sock = make_socket(port);
	if(listen(sock,MAX_CLIENTS) < 0) exiterr("Failed to set up listening functionality on the server side\n");
	fd_set allfds;
	fd_set waitingfds;
	// Initialize allfds to zero
	FD_ZERO(&allfds);
	// Add sock to the active file descriptor set
	FD_SET(sock, &allfds);
	waitingfds = allfds;

	int numclients = 0;
	while((latest <= maxint) || numclients)
	{
		/* Block until input arrives on one or more active sockets. */
		//printf("Waiting for input\n");
		waitingfds = allfds;
		if(select(FD_SETSIZE, &waitingfds, NULL, NULL, NULL) < 0) exiterr("Select error: failed to wait for input on server side\n");
		for (int i = 0; i < FD_SETSIZE; ++i)
		{
			if(FD_ISSET(i,&waitingfds))
			{
				if (i == sock)
				{
					// Connection request on original socket.
					int receivesocket;
					struct sockaddr_in clientname;
					size_t size;
					size = sizeof(clientname);
					receivesocket = accept (sock,(struct sockaddr *) &clientname,(socklen_t *)&size);
					if (receivesocket < 0) exiterr("Incoming connection attempt failed, server accept function\n");
					printf ("Server: connect from host %s, port %d.\n",inet_ntoa (clientname.sin_addr),ntohs(clientname.sin_port));
					FD_SET(receivesocket,&allfds);
					send_job(receivesocket);
					numclients++;
				}
				else
				{
					// Data arriving on an already-connected socket.
					if(receive_results(i) < 0)
					{
						close(i);
						numclients--;
						FD_CLR(i,&allfds);
					}
					else send_job(i);
				}
			}
		}
	}
	return 0;
}

int make_socket(uint16_t sockport)
{
	/* Create the socket. */
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) exiterr("Failed to create a new socket\n");

	/* Give the socket a name. */
	struct sockaddr_in name;
	name.sin_family = AF_INET;
	name.sin_port = htons(sockport);
	name.sin_addr.s_addr = INADDR_ANY;
	if (bind(sock,(struct sockaddr *) &name, sizeof(name)) < 0) exiterr("Failed to bind to the new socket\n");
	return sock;
}

void send_job(int filedes)
{
	primeint jobstart = 0;
	primeint jobend = 0;
	if(latest < maxint)
	{
		jobstart = latest;
		jobend = latest + jobsize;
		if(jobend > maxint) jobend = maxint;
		latest = jobend + 1;
	}
	jobstart = htonl(jobstart);
	jobend = htonl(jobend);
	if(write(filedes,&jobstart,sizeof(primeint)) == -1) exiterr("Write operation failed in send_job");
	if(write(filedes,&jobend,sizeof(primeint)) == -1) exiterr("Write operation failed in send_job");
}

int receive_results(int filedes)
{
	primeint workerints = 0;
	primeint workerprimes = 0;
	size_t nbytes = read(filedes,&workerints,sizeof(primeint));
	if(nbytes < 0) exiterr("Server side read error");
	else if (nbytes == 0) return -1; // End-of-file.
	else
	{
		read(filedes,&workerprimes,sizeof(primeint));
		numints += ntohl(workerints);
		numprimes += ntohl(workerprimes);
	}
	printf("Number of ints checked: %d\tNumber of primes: %d\n",(int)numints,(int)numprimes);
}
