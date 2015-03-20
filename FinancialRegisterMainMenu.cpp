// FinancialRegisterMainMenu.cpp
// By Adam R. Spangler, created on 3/20/2015
// Last updated: 3/20/2015

#include <stdio.h>

#include "FinancialRegisterDataTypes.cpp"

int	add_trans	(financialregister_t* reg);
int	man_accts	(financialregister_t* reg);
int	man_categ	(financialregister_t* reg);
int	view_trans	(financialregister_t* reg);
int	man_trans	(financialregister_t* reg);

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
			printf("You want to add a transaction\n");
			break;
		case 'b':
			printf("You want to manage accounts\n");
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
