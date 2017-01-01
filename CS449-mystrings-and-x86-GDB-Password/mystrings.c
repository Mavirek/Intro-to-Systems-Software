#include <string.h>
#include <stdio.h>
#include <stdlib.h>
int printableString(char*);
int printableChar(char);
int main(int argc, char **argv)
{
	if(argc!=2)
	{
		printf("Invalid number of arguments. Please retry and enter one file name as an argument input.\n");
		return 0;
	}
	FILE *f;
	f = fopen(argv[1], "rb");
	if(f==NULL)
	{
		printf("File does not exist. Please retry with a valid file.\n");
		return 0;
	}
	char c;
	int numChars=0;
	char stringTest[4];
	long filePos=0;
	int checkRead=0;

	while(!feof(f))
	{
		filePos=ftell(f);
		//check file read size, if it's <4, then no more strings to read --> end of file
		checkRead=fread(&stringTest, sizeof(char), 4, f);
		if(checkRead<4)
		{
			fclose(f);
			return 0;
		}
		//test if string that was read in is printable
		if(printableString(stringTest))
		{
			//go back to original read-in position of file
			fseek(f,filePos,SEEK_SET);

			while(checkRead>0)
			{
				//start reading in at printable string and onwards
				//at this point, string can be 4 bytes or more
				checkRead=fread(&c, sizeof(char), 1, f);
				if(printableChar(c))
				{
					printf("%c", c);
				}
				else
				{
					printf("\n");
					break;
				}
			}
			
		}
		//tested string is not readable, go back to original read-in position +1 to go to next character and check the next 4 bytes
		else
			fseek(f, ftell(f)-3, SEEK_SET);

	}
	fclose(f);
	return 0;
}
int printableString(char *string)
{
	//tests if string is printable
	for(int x=0; x<4; x++)
	{
		if((string[x]<32 && string[x]!=9) || (string[x]>126 && string[x]!=9))
		{
			return 0; //not printable string if one of the chars is not in between 32 and 126 nor equal to 9
		}
	}
	return 1; //all chars are printable --> string is printable
}
int printableChar(char ch)
{
	return (ch>=32 && ch<=126 || ch==9);
}