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
primeint	numintschkd;

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
	char* SERVERHOST = strdup(argv[1]);

	// Create the socket.
	int sock = socket(PF_INET,SOCK_STREAM,0);
	if (sock < 0) exiterr("Socket creation failure, client side\n");

	// Connect to the server.
	struct sockaddr_in servername;
	init_sockaddr(&servername,SERVERHOST,PORT);
	while(0 > connect(sock, (struct sockaddr*)&servername, sizeof(servername)) ) sleep(1);

	// Initialize local data structures and create threads
	numprimes = 0;
	intschecked[0] = 0;
	pthread_t threads[LOCAL_THREADS];

	// Receive a job from the server
	primeint* startnum;
	primeint* endnum;
//	while(1)
//	{
		read(sock,(void*)startnum,sizeof(primeint));
		read(sock,(void*)endnum,sizeof(primeint));
		printf("Startnum = %d\n",(int)*startnum);
		printf("Endnum = %d\n",(int)*endnum);
		while(*startnum > 0)
		{
			numprimes = 0;
			threadargarray[0].startnum = *startnum;
			threadargarray[0].endnum = (*endnum - *startnum) / LOCAL_THREADS + *startnum;
			threadargarray[0].threadid = 0;
			printf("Thread 0: %d to %d\n",(int)threadargarray[0].startnum,(int)threadargarray[0].endnum);
			for(int n = 1; n < LOCAL_THREADS; n++)
			{
				threadargarray[n].startnum = threadargarray[n-1].endnum + 1;
				threadargarray[n].endnum = ((*endnum - *startnum) / LOCAL_THREADS) * (n + 1) + *startnum;
				intschecked[n] = 0;
				threadargarray[n].threadid = n;
				printf("Thread %d: %d to %d\n",n,(int)threadargarray[n].startnum,(int)threadargarray[n].endnum);
			}
			threadargarray[LOCAL_THREADS - 1].endnum = *endnum;
			printf("Thread %d: %d to %d\n",LOCAL_THREADS - 1,(int)threadargarray[LOCAL_THREADS - 1].startnum,(int)threadargarray[LOCAL_THREADS - 1].endnum);

			for(int i = 0; i < LOCAL_THREADS; i++)
				if(pthread_create(&threads[i],NULL,chkprimes,(void*)&threadargarray[i]))
					exiterr("Thread creation error!\n");

			for(int j = 0; j < LOCAL_THREADS; j++)
			{
				pthread_join(threads[j],NULL);
				numintschkd = numintschkd + intschecked[j];
				intschecked[j] = 0;
			}

			write(sock,&numintschkd,sizeof(primeint));
			write(sock,&numprimes,sizeof(primeint));
			numintschkd = 0;
			numprimes = 0;
			read(sock,(void*)startnum,sizeof(primeint));
			read(sock,(void*)endnum,sizeof(primeint));
			printf("Startnum = %d\n",(int)*startnum);
			printf("Endnum = %d\n",(int)*endnum);
		}
//	}
	close(sock);
	pthread_exit(NULL);
}

void init_sockaddr(struct sockaddr_in *name,const char *hostname,uint16_t port)
{
	struct hostent *hostinfo;
	name->sin_family = AF_INET;
	name->sin_port = htons (port);
	hostinfo = gethostbyname (hostname);
	if (hostinfo == NULL) exiterr("Unknown host\n");
	name->sin_addr = *(struct in_addr *) hostinfo->h_addr;
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
	primeint i;
	for (i=5; i*i<=num; i=i+2) if (num % i == 0) return 0;
	return 1;
}
