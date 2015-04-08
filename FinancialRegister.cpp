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
FILE*	findregfile	(int argc, char* argv[]);
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
	FILE* regfile = findregfile(argc,argv);
	if(regfile == NULL) return 0;

	// Second step: Load values from the .reg file and initialize the register object
	financialregister_t Register;
	if(init(Register,regfile))
	{
		printf("Error reading file.\n");
		return 1;
	}

	while(mainmenu(Register)) {};
	savefile(Register,regfile);
	return 0;
}

FILE*	findregfile(int argc,char* argv[])
{
	if(argc == 1)
	{
		printf("You did not specify a .reg file.\n");
		if(yorn("Would you like to create one (y/n)? "))
		{
			while(1)
			{
				printf("Please enter a name for the new file: \n");
				char* filename = NULL;
				size_t filenamenumchars = 0;
				getline(&filename,&filenamenumchars,stdin);
				// Because getline doesn't have the good sense to remove the newline character
				char* newlinesearch = filename;
				while(*newlinesearch != '\n') newlinesearch++;
				*newlinesearch = '\0';
				FILE* newregfile = fopen(filename,"wx");
				if(newregfile == NULL)
				{
					printf("File already exists.\n");
					continue;
				}
				printf("File %s opened successfully.\n",filename);
				return newregfile;
			}
		}
		else
		{
			printf("Financial Register must be run with a .reg file.\n");
			return NULL;
		}
	}
	else if(argc == 2)
	{
		FILE* newregfile = fopen(argv[1],"r+");
		if(newregfile == NULL)
		{
			printf("Failed to open file %s.\n",argv[1]);
			return NULL;
		}
		printf("File %s opened successfully.\n",argv[1]);
		return newregfile;
	}
	else printf("You may only specify one .reg file.\n");
	return NULL;
}

int init(financialregister_t& reg,FILE* infile)
{
	int nextchar = getc(infile);
	if(nextchar == EOF)
	{
		printf("The file is empty. Initializing a new Register.\n");
		reg.numaccounts = 0;
		reg.firstaccount = NULL;
		reg.lastaccount = NULL;
		printf("Initialization complete.\n");
		return 0;
	}

	fseek(infile,0,SEEK_SET);
	char* nextline = NULL;
	size_t nextlinechars = 0;
	getline(&nextline,&nextlinechars,infile);
	if(strcmp(nextline,"Accounts:\n")) return 1;
	getline(&nextline,&nextlinechars,infile);
	if(strcmp(nextline,"<none>\n"))
	{
		reg.numaccounts = 0;
		reg.firstaccount = NULL;
		reg.lastaccount = NULL;
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
			}
			else
			{
				reg.lastaccount->nextaccount = newaccount;
				newaccount->previousaccount = reg.lastaccount;
				reg.lastaccount = newaccount;
				newaccount->nextaccount = NULL;
			}
			newaccount->name = (char*)malloc(++strlen(nextchar))
			strcpy(newaccount->name,nextchar);

			getline(&nextline,&nextlinechars,infile);
			newlinesearch = nextline;
			while(*newlinesearch != '\n') newlinesearch++;
			*newlinesearch = '\0';
			nextchar = nextline;
			while(*(++nextchar) != ':') if((*nextchar == '\n') || (*nextchar == EOF)) return 1;
			nextchar++;
			// nextchar is now positioned on the first character of the account type
			if(!strcmp(nextchar,"Checking") newaccount->type = Checking;
			else if(!strcmp(nextchar,"Savings") newaccount->type = Savings;
			else if(!strcmp(nextchar,"CreditCard") newaccount->type = CreditCard;
			else return 1;

			getline(&nextline,&nextlinechars,infile);
			newlinesearch = nextline;
			while(*newlinesearch != '\n') newlinesearch++;
			*newlinesearch = '\0';
			nextchar = nextline;
			while(*(++nextchar) != ':') if((*nextchar == '\n') || (*nextchar == EOF)) return 1;
			nextchar++;
			// nextchar is now positioned on the first character of the account balance
			newaccount->balance = strtol(nextchar);

			getline(&nextline,&nextlinechars,infile);
			if(strcmp(nextline,"Begin Transactions:\n") return 1;
			getline(&nextline,&nextlinechars,infile);
			if(!strcmp(nextline,"End Transactions\n")
			{
				newaccount->firstfintrans = NULL;
				newaccount->lastfintrans = NULL;
			}
			else
			{
				//left off here
			}
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
	if(fputs("Accounts:\n",outfile) == EOF) return;
	account_t* currentaccount = reg.firstaccount;
	if(currentaccount == NULL) fprintf(outfile,"End Accounts\n");
	else
	{
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
	}
	fprintf(outfile,"End Accounts");
	fputc(outfile,EOF);
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
