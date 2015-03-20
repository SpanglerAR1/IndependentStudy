// FinancialRegisterMainMenu.cpp
// By Adam R. Spangler, created on 3/20/2015
// Last updated: 3/20/2015

#include <stdio.h>
#include <ctype.h>

#include "FinancialRegisterDataTypes.cpp"

int	add_trans	(financialregister_t& reg);
int	man_accts	(financialregister_t& reg);
int	man_categ	(financialregister_t& reg);
int	view_trans	(financialregister_t& reg);
int	man_trans	(financialregister_t& reg);

int mainmenu(financialregister_t& reg)
{
	printf("Please select an option:\n");
	printf("a. Add a transaction\n");
	printf("b. Manage accounts\n");
	printf("c. Manage budget categories\n");
	printf("h. View transaction history\n");
	printf("t. Manage transactions\n");
	printf("s. Save and quit\n");
	printf("x. Exit w/o saving\n");
	printf("Selection: ");
	int c = getc(stdin);
	int selection = c;
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
			printf("You want to save and quit\n");
			printf("Saving...");
		case 'x':
			printf("Quitting...\n");
			return 0;
		default:
			return 1;
	}
	return 1;
}

int add_trans (financialregister_t& reg)
{
	printf("You want to add a transaction\n");
	return 1;
}


int man_accts (financialregister_t& reg)
{
	if(reg.num_accounts == 0)
	{
		printf("You do not have any accounts yet.\nA new one will be created.\n");
		reg.num_accounts = 1;
		// initialize the account pointer list:
		reg.accountlist = (account_t **)malloc(sizeof(account_t*));
		// create the first account object:
		reg.*accountlist = (account_t *)malloc(sizeof(account_t));
		// create a separate pointer to the new account itself, for easier dereferencing.
		account_t* newaccount = reg.*accountlist;

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
		}

		while(1)
		{
			printf("You do not have any accounts yet.\nPlease enter a name for the new account: ");
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
				return 0;
			}
			newaccount->name = accountname;
		}

		while(1)
		{
			printf("Please enter a starting balance for the account: ");
			char* startingbalance = NULL;
			size_t startingbalancenumchars = 0;
			// left off here.
		}
	}
}

int	man_categ	(financialregister_t* reg);
int	view_trans	(financialregister_t* reg);
int	man_trans	(financialregister_t* reg);
