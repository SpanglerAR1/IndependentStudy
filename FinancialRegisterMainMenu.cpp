// FinancialRegisterMainMenu.cpp
// By Adam R. Spangler, created on 3/20/2015
#define	__FR_VERSION_ID	"0.0.1"
// Last updated: 3/20/2015

#include <stdio.h>

#include "FinancialRegisterDataTypes.cpp"

int mainmenu(financialregister_t* reg)
{
	int selection = 0
	while(selection 1= 'e')
	{
		printf("Please select an option:\n");
		printf("a. Add a transaction\n");
		printf("b. Manage accounts\n");
		printf("c. Manage budget categories\n");
		printf("h. View transaction history\n");
	}
	return 0;
}
