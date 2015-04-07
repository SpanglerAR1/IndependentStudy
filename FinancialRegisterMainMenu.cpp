// FinancialRegisterMainMenu.cpp
// By Adam R. Spangler, created on 3/20/2015
// Last updated: 3/20/2015

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "FinancialRegisterDataTypes.cpp"

void	add_trans	(financialregister_t& reg);
void	man_accts	(financialregister_t& reg);
	void	add_acct	(financialregister_t& reg);
	void	del_acct	(financialregister_t& reg);
	void	mod_acct	(financialregister_t& reg);
void	man_categ	(financialregister_t& reg);
void	view_trans	(financialregister_t& reg);
void	man_trans	(financialregister_t& reg);

int mainmenu(financialregister_t& reg)
{
	printf("Please select an option:\n");
	printf("a. Add a transaction\n");
	printf("b. Manage accounts\n");
	printf("c. Manage budget categories\n");
	printf("h. View transaction history\n");
	printf("t. Manage transactions\n");
	printf("s. Save and quit\n");
	printf("Selection: ");
	int c = getc(stdin);
	int selection = tolower(c);
	// discard extra input and the newline
	while((c != '\n') && (c != EOF)) c = getc(stdin);
	switch(selection)
	{
		case EOF:
		case '\n':
			return 1;
		case 'a':
			add_trans(reg);
			break;
		case 'b':
			man_accts(reg);
			break;
		case 'c':
			printf("You want to manage budget categories\n");
			break;
		case 'h':
			printf("You want to view transaction histories\n");
			break;
		case 't':
			printf("You want to manage transactions\n");
			break;
		case 's':
			return 0;
		default:
			return 1;
	}
	return 1;
}

void add_trans (financialregister_t& reg)
{
	printf("You want to add a transaction\n");
	return;
}


void man_accts (financialregister_t& reg)
{
	if(reg.numaccounts == 0)
	{
		printf("You do not have any accounts yet.\nA new one will be created.\n");
		add_acct(reg);
	}

	printf("You currently have %d accounts\n",reg.numaccounts);
	int accountcounter = 1;
	account_t* currentaccount = reg.firstaccount;
	while(currentaccount != NULL)
	{
		printf("%d: %s\n",accountcounter++,currentaccount->name);
		currentaccount = currentaccount->nextaccount;
	}

	while(1)
	{
		printf("Would you like to:\nd. Delete existing account\nm. Modify existing account\na. Add a new acount\nx. Go back\n");
		printf("Selection: ");
		int c = getc(stdin);
		int selection = tolower(c);
		while((c != '\n') && (c != EOF)) c = getc(stdin);
		switch(selection)
		{
			case EOF:
			case '\n':
				continue;
			case 'd':
				del_acct(reg);
				break;
			case 'm':
				mod_acct(reg);
				break;
			case 'a':
				add_acct(reg);
				break;
			case 'x':
				return;
		}
	}
}

void add_acct(financialregister_t& reg)
{
	printf("Creating new account...\n");
	account_t* newaccount = (account_t*)malloc(sizeof(account_t));
	if(reg.numaccounts == 0)
	{
		reg.firstaccount = newaccount;
		reg.lastaccount = newaccount;
		newaccount->previousaccount = NULL;
		newaccount->nextaccount = NULL;
	}
	else
	{
		reg.lastaccount->nextaccount = newaccount;
		newaccount->previousaccount = reg.lastaccount;
		reg.lastaccount = newaccount;
		newaccount->nextaccount = NULL;
	}
	reg.numaccounts++;

	while(1)
	{
		printf("Will the account be\nC. Checking\nS. Savings\nL. Liability\nA. Asset\nChoose one: ");
		int response = getc(stdin);
		if((response == '\n') || (response == EOF)) continue;
		response = tolower(response);
		switch(response)
		{
			case 'c':
				newaccount->type = checking;
				break;
			case 's':
				newaccount->type = savings;
				break;
			case 'l':
				newaccount->type = liability;
				break;
			case 'a':
				newaccount->type = asset;
				break;
			default:
				continue;
		}
		while(getc(stdin) != '\n'); // Remove the trailing newline
		break;
	}

	while(1)
	{
		printf("Please enter a name for the new account: ");
		char* accountname = NULL;
		size_t accountnamenumchars = 0;
		getline(&accountname,&accountnamenumchars,stdin);
		char* newlinesearch = accountname;
		while(*newlinesearch != '\n') newlinesearch++;
		*newlinesearch = '\0';
		// If the person didn't actually provide a name, but just hit enter:
		if(newlinesearch == accountname)
		{
			printf("Invalid account name.\n");
			continue;
		}
		newaccount->name = accountname;
		break;
	}

	while(1)
	{
		printf("Please enter a starting balance for the account: ");
		char* startingbalancestr = NULL;
		size_t startingbalancestrnumchars = 0;
		getline(&startingbalancestr,&startingbalancestrnumchars,stdin);
		char* tailptr = NULL;
		int startingbalanceint = strtol(startingbalancestr,&tailptr,10);
		fint startingbalance = startingbalanceint * 100;
		if(*tailptr == '.')
		{
			startingbalancestr = ++tailptr;
			int cents = strtol(startingbalancestr,&tailptr,10);
			while(cents >= 100) cents = cents / 10;
			startingbalance = startingbalance + cents;
		}
		newaccount->balance = startingbalance;
		break;
	}

	newaccount->createdate = time(NULL);
	newaccount->numfintrans = 0;
	newaccount->firstfintrans = NULL;
	newaccount->lastfintrans = NULL;

	printf("Your new account, %s,",reg.lastaccount->name);
	printf(" with balance %d.",(reg.lastaccount->balance)/100);
	printf("%d, has been created.\n",(reg.lastaccount->balance)%100);
	return;
}

void del_acct(financialregister_t& reg)
{
	printf("You currently have %d accounts\n",reg.numaccounts);
	int accountcounter = 1;
	account_t* currentaccount = reg.firstaccount;
	while(currentaccount != NULL)
	{
		printf("%d: %s\n",accountcounter++,currentaccount->name);
		currentaccount = currentaccount->nextaccount;
	}
	printf("In del_acct()\n");
	return;
}

void mod_acct(financialregister_t& reg)
{
	printf("In mod_acct\n");
}
