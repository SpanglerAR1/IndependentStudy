// FinancialRegisterDataTypes
// By Adam R. Spangler, created on 3/18/2015
#define	__FRDT_VERSION_ID	"0.0.1"
// Last updated: 3/18/2015

#include<time.h>

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
	creditcard,
	liability,
	asset
};

// fintranstype = Financial Transaction Type
enum	fintranstype
{
	income,
	expense,
	transfer
};

// budgcat = budget category
struct	budgcat_t
{
	char*		name;
	fint		amount;
	time_t		date;	// If date is 0, then assumed to be monthly
};

// Declaration, so that I can reference account_t in fintrans_t
struct account_t;

struct	fintrans_t
{
	fintranstype	type;
	account_t*	account;
	account_t*	to_account;	// Ignored unless type == transfer
	fint		amount;
	time_t		date;
	budgcat_t	category;
	char*		description;
	char*		outsideparty;	// Ignored if transfer
};

struct	account_t
{
	accounttype 	type;
	char*		name;
	fint		balance;
	time_t		createdate;
	fintrans_t**	fintransls;
	int		numtrans;	// Number of transactions in fintransls
};

struct	financialregister_t
{
	int 		num_accounts;
	account_t**	accountlist;
	int		num_categories;
	budgcat_t**	categorylist;
	int		num_transactions;
	fintrans_t**	transactionlist;
};


#endif
