// FinancialRegisterDataTypes
// By Adam R. Spangler, created on 3/18/2015
#define	__FRDT_VERSION_ID	"0.0.1"
// Last updated: 3/18/2015

#include<stdlib.h>
#include<error.h>
#include<errno.h>

#ifndef	__FRDT_GUARD
#define	__FRDT_GUARD

// I'm creating a fixed-point type for account balances and transaction amounts.
// It's simply an int, but it reminds me that fint represents cents, rather than dollars,
// and will need to be converted before displaying to the user. fint = financial integer
typedef int fint;

enum	accounttype
{
	checking,
	savings,
	creditcard
};

// fintranstype = Financial Transaction Type
enum	fintranstype
{
	income,
	expense
};

struct fintrans_t;
struct account_t;
struct financialregister_t;

struct	fintrans_t
{
	fintranstype	type;
	fint		amount;
	char*		description;
	char*		outsideparty;	// Ignored if transfer
	fintrans_t*	previousfintrans;
	fintrans_t*	nextfintrans;
};

struct	account_t
{
	accounttype 	type;
	char*		name;
	fint		balance;
	int		numfintrans;
	fintrans_t*	firstfintrans;
	fintrans_t*	lastfintrans;
	account_t*	previousaccount;
	account_t*	nextaccount;
};

struct	financialregister_t
{
	int		numaccounts;
	account_t*	firstaccount;
	account_t*	lastaccount;
};

void* xmalloc(size_t size)
{
	void* newptr = 0;
	newptr = malloc(size);
	if(newptr == 0)
	{
		error(1,errno,"call to malloc failed");
	}
	return newptr;
}

// This function comes via the GNU C Library Manual.
int yorn(const char* question)
{
	fputs(question,stdout);
	while (1)
	{
		int c, answer;
		/* Write a space to separate answer from question. */
		fputc(' ', stdout);
		/* Read the first character of the line.
		This should be the answer character, but might not be. */
		c = tolower(getc(stdin));
		answer = c;
		/* Discard rest of input line. */
		while((c != '\n') && (c != EOF)) c = getc (stdin);
		/* Obey the answer if it was valid. */
		if(answer == 'y') return 1;
		if(answer == 'n') return 0;
		/* Answer was invalid: ask for valid answer. */
		fputs ("Please answer y or n:", stdout);
	}
}
#endif
