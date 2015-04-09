// FinancialRegister.cpp
// By Adam R. Spangler, created on 3/18/2015
#define	__FR_VERSION_ID	"0.0.1"
// Last updated: 4/7/2015

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "FinancialRegisterDataTypes.cpp"
#include "FinancialRegisterMainMenu.cpp"

int 	yorn		(const char* question);
int	init		(financialregister_t& reg,FILE* infile);
void	savefile	(financialregister_t& reg,FILE* outfile);

int main(int argc, char* argv[])
{
	printf("Welcome to Adam Spangler's Financial Register Program, version %s\n",__FR_VERSION_ID);

	// Need to:
	// Find a .reg file or create one
	// Initialize the Register object
	// Prompt the user with a main menu
	// Save to the .reg file on closing

	// First step: Locate the .reg file
	char* filename = NULL;
	FILE* regfile = NULL;
	if(argc == 1)
	{
		printf("You did not specify a .reg file.\n");
		if(yorn("Would you like to create one (y/n)? "))
		{
			while(1)
			{
				printf("Please enter a name for the new file: \n");
				size_t filenamenumchars = 0;
				getline(&filename,&filenamenumchars,stdin);
				// Because getline doesn't have the good sense to remove the newline character
				char* newlinesearch = filename;
				while(*newlinesearch != '\n') newlinesearch++;
				*newlinesearch = '\0';
				regfile = fopen(filename,"wx");
				if(regfile == NULL)
				{
					printf("File already exists.\n");
					continue;
				}
				printf("File %s opened successfully.\n",filename);
			}
		}
		else
		{
			printf("Financial Register must be run with a .reg file.\n");
		}
	}
	else if(argc == 2)
	{
		regfile = fopen(argv[1],"r");
		if(regfile == NULL)
		{
			printf("Failed to open file %s.\n",argv[1]);
			return 1;
		}
		printf("File %s opened successfully.\n",argv[1]);
		filename = strdup(argv[1]);
	}
	else
	{
		printf("You may only specify one .reg file.\n");
		return 1;
	}

	printf("The filename is %s\n",filename);

	// Second step: Load values from the .reg file and initialize the register object
	financialregister_t Register;
	Register.numaccounts = 0;
	Register.firstaccount = NULL;
	Register.lastaccount = NULL;

	if(init(Register,regfile))
	{
		printf("Error reading file.\n");
		return 1;
	}

	//system("clear");
	list_accts(Register);
	while(mainmenu(Register))
	{
		//system("clear");
		list_accts(Register);
	}
	fclose(regfile);
	printf("File Closed\n");
	regfile = fopen(filename,"w");
	printf("File %s reopened for writing\n",filename);
	savefile(Register,regfile);
	return 0;
}

int init(financialregister_t& reg,FILE* infile)
{
	int nextchar = getc(infile);
	if(nextchar == EOF)
	{
		printf("The file is empty.\n");
		return 0;
	}

	fseek(infile,0,SEEK_SET);
	char* nextline = NULL;
	size_t nextlinechars = 0;
	getline(&nextline,&nextlinechars,infile);
	//printf("Read line: %s\n",nextline);
	if(strcmp(nextline,"Accounts:\n")) return 1;
	getline(&nextline,&nextlinechars,infile);
	//printf("Read line: %s\n",nextline);
	if(!strcmp(nextline,"End Accounts\n"))
	{
		printf("According to the file, there are no accounts.\n");
	}
	else
	{
		while(strcmp(nextline,"End Accounts\n"))
		{
			char* nextchar = nextline;
			while(*(++nextchar) != ':') if((*nextchar == '\n') || (*nextchar == EOF)) return 1;
			nextchar++;
			// Nextchar is now positioned on the first character of the name of the account
			char* newlinesearch = nextchar;
			while(*newlinesearch != '\n') newlinesearch++;
			*newlinesearch = '\0';
			account_t* newaccount = (account_t*)malloc(sizeof(account_t));
			if(reg.firstaccount == NULL)
			{
				reg.firstaccount = newaccount;
				reg.lastaccount = newaccount;
				newaccount->previousaccount = NULL;
				newaccount->nextaccount = NULL;
				reg.numaccounts++;
			}
			else
			{
				reg.lastaccount->nextaccount = newaccount;
				newaccount->previousaccount = reg.lastaccount;
				reg.lastaccount = newaccount;
				newaccount->nextaccount = NULL;
				reg.numaccounts++;
			}
			newaccount->name = strdup(nextchar);
			//printf("New Account name, as read by program: %s\n",newaccount->name);

			getline(&nextline,&nextlinechars,infile);
			//printf("Next line read: %s\n",nextline);
			newlinesearch = nextline;
			while(*newlinesearch != '\n') newlinesearch++;
			*newlinesearch = '\0';
			nextchar = nextline;
			while(*(++nextchar) != ':') if((*nextchar == '\n') || (*nextchar == EOF)) return 1;
			nextchar++;
			//printf("Account type string: %s\n",nextchar);
			// nextchar is now positioned on the first character of the account type
			if(!strcmp(nextchar,"Checking")) newaccount->type = checking;
			else if(!strcmp(nextchar,"Savings")) newaccount->type = savings;
			else if(!strcmp(nextchar,"CreditCard")) newaccount->type = creditcard;
			else return 1;

			//printf("The new account's type is %d\n",newaccount->type);

			getline(&nextline,&nextlinechars,infile);
			newlinesearch = nextline;
			while(*newlinesearch != '\n') newlinesearch++;
			*newlinesearch = '\0';
			nextchar = nextline;
			while(*(++nextchar) != ':') if((*nextchar == '\n') || (*nextchar == EOF)) return 1;
			nextchar++;
			// nextchar is now positioned on the first character of the account balance
			newaccount->balance = strtol(nextchar,NULL,10);
			//printf("The new account's balance is %d\n",newaccount->balance);

			getline(&nextline,&nextlinechars,infile);
			if(strcmp(nextline,"Begin Transactions:\n")) return 1;
			getline(&nextline,&nextlinechars,infile);
			//printf("Next line read: %s\n",nextline);
			newaccount->numfintrans = 0;
			if(!strcmp(nextline,"End Transactions\n"))
			{
				newaccount->firstfintrans = NULL;
				newaccount->lastfintrans = NULL;
			}
			else
			{
				while(strcmp(nextline,"End Transactions\n"))
				{
					fintrans_t* newfintrans = (fintrans_t*)malloc(sizeof(fintrans_t));
					if(newaccount->firstfintrans = NULL)
					{
						newaccount->firstfintrans = newfintrans;
						newaccount->lastfintrans = newfintrans;
						newfintrans->previousfintrans = NULL;
						newfintrans->nextfintrans = NULL;
					}
					else
					{
						newaccount->lastfintrans->nextfintrans = newfintrans;
						newfintrans->previousfintrans = newaccount->lastfintrans;
						newfintrans->nextfintrans = NULL;
						newaccount->lastfintrans = newfintrans;
					}
					newaccount->numfintrans++;
					int newfintranstype = strtol(strtok(nextline,":\n"),NULL,10);
					if(newfintranstype) newfintrans->type = income;
					else newfintrans->type = expense;
					newfintrans->amount = strtol(strtok(NULL,":\n"),NULL,10);
					newfintrans->description = strdup(strtok(NULL,":\n"));
					newfintrans->outsideparty = strdup(strtok(NULL,":\n"));

					getline(&nextline,&nextlinechars,infile);
				}//while(strcmp(nextline,"End Transactions\n");
			}// if(strcmp(nextline,"End Transactions\n");
			getline(&nextline,&nextlinechars,infile);
			//printf("Last account line read: %s\n",nextline);
		}
	}

	fflush(infile);
	fclose(infile);
	printf("Initialization Complete...\n");
	return 0;
}



void savefile(financialregister_t& reg, FILE* outfile)
{
	printf("Saving...\n");
	fseek(outfile,0,SEEK_SET);
	printf("The call to fseek succeeded\n");
	if(fputs("Accounts:\n",outfile) == EOF) return;
	account_t* currentaccount = reg.firstaccount;
	printf("Now processing account %s\n",currentaccount->name);
	while(currentaccount != NULL)
	{
		fprintf(outfile,"Account Name:%s\n",currentaccount->name);

		fprintf(outfile,"Account Type:");
		switch(currentaccount->type)
		{
			case checking:
				fprintf(outfile,"Checking\n");
				break;
			case savings:
				fprintf(outfile,"Savings\n");
				break;
			case creditcard:
				fprintf(outfile,"CreditCard\n");
		}

		fprintf(outfile,"Balance (cents):%d\n",currentaccount->balance);

		fprintf(outfile,"Begin Transactions:\n");
		fintrans_t* currentfintrans = currentaccount->firstfintrans;
		if(currentfintrans == NULL) fprintf(outfile,"End Transactions\n");
		else
		{
			while(currentfintrans != NULL)
			{
				fprintf(outfile,"%d:%d:%s:%s\n",currentfintrans->type,currentfintrans->amount,currentfintrans->description,currentfintrans->outsideparty);
				currentfintrans = currentfintrans->nextfintrans;
			}
			fprintf(outfile,"End Transactions\n");
		}

		currentaccount = currentaccount->nextaccount;
	}

	fprintf(outfile,"End Accounts\n");
	fputc(EOF,outfile);
	fflush(outfile);
	fclose(outfile);
	return;
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
