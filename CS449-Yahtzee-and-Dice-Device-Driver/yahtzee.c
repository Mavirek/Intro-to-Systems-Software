#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>


int roll();
void reroll();
void printDie(int dice[]);
int placeCategories();
void printScores();
int cmp(const void *a, const void *b); //compare function for qsort()
int validate(int cat,int select); //validate to prevent rechoosing a category

int dice[5];
int upperSec[6];
int lowerSec[7];
//upperScore and lowerScore used to calculate total score in printScores()
int upperScore; //used to calculate upper bonus and total score in printScores()
int lowerScore; //used with upperScore to calculate total score
int upperBonus; //used to keep track of when to assign bonus
int rerollBoolean; //used to prompt user for a second reroll depending if they accept the first reroll
int category;
int main()
{
	int i=0;
	for(i=0;i<6;i++)
	{
		upperSec[i]=-1;
	}
	for(i=0;i<7;i++)
	{
		lowerSec[i]=-1;
	}
	upperScore=0;
	lowerScore=0;
	upperBonus=0;
	rerollBoolean=1;
	//srand(time(NULL));

	int turn=0;
	for(turn=0;turn<13;turn++){
		printf("Turn %d\n\n",turn+1);
		int i=0;
		printf("Your roll:\n\n");
		for(i=0; i<5; i++)
		{
			dice[i]=roll();
			printf("%d ",dice[i]);
		}
		printf("\n\n");
		reroll(); //first reroll prompt
		//if the user doesn't want to reroll the first time,
		//assume they don't want to reroll the second time
		if(rerollBoolean!=0) 
			reroll(); //second reroll prompt
		qsort(dice, 5, sizeof(int), cmp);
		//printDie(dice);
		while(!placeCategories()){}
		printScores();
	}
	printf("Thanks for playing Yahtzee!\n");
	return 0;
}

int roll()
{
	//return rand()%6+1;
	
	char rand;
	int x=open("/dev/dice",O_RDONLY);
	if(x<1)
		printf("Error\n");
	read(x,&rand,sizeof(unsigned char));
	close(x);
	return rand;
	
}

void reroll()
{
	//parse input for die to reroll
	char input[11];//max input size is numberOfDie*2
	char *tokenizeInput;//for space delimiter purposes
	printf("Which dice to reroll? ");
	fgets(input, sizeof(input), stdin);
	if(strcmp(input,"0\n")==0) //user does not want to reroll
	{
		rerollBoolean=0; //don't prompt user for second reroll
		return;
	}
	tokenizeInput=strtok(input, " ");
	while(tokenizeInput!=NULL)
	{
		dice[atoi(tokenizeInput)-1]=roll();
		tokenizeInput=strtok(NULL, " ");
	}
	printf("\nYour reroll is:\n");
	printDie(dice);

}

void printDie(int dice[])
{
	int i=0;
	for(i=0; i<5; i++)
	{
		printf("%d ",dice[i]);
	}
	printf("\n\n");
}

int cmp(const void *a, const void *b)
{
	return (*(int*)a - *(int*)b);
}

int placeCategories()
{
	printf("\nPlace dice into:\n1) Upper Section\n2) Lower Section\n\n");
	char input[10];
	int selection, i=0;
	int sum=0;
	printf("Selection? ");
	fgets(input, sizeof(input),stdin);
	category=atoi(input);
	//upper section
	if(category==1)
	{
		printf("\nPlace dice into:\n1) Ones\n2) Twos\n3) Threes\n4) Fours\n5) Fives\n6) Sixes\n\n");
		printf("Selection? ");
		fgets(input,sizeof(input),stdin);
		selection=atoi(input);
		if(!validate(category,selection))
		{
			printf("\nSelection already chosen; please select a different one\n");
			return 0;
		}
		//printf("selection=%d\nsumBefore=%d\n",selection,sum);
		for(i=0;i<5;i++)
		{
			if(dice[i]==selection)
			{
				sum+=selection; //accumulate the user's score from current turn
			}
		}
		//printf("sumAfter=%d\n",sum);
		upperSec[selection-1]=sum;
		upperScore+=sum; //used to keep track of when to assign bonus
		return 1;
	}
	//lower section
	else if(category==2)
	{
		printf("\nPlace dice into:\n1) Three of a kind\n2) Four of a kind\n3) Full House\n4) Small Straight\n5) Large Straight\n6) Yahtzee\n7) Chance\n\n");
		printf("Selection? ");
		fgets(input, sizeof(input), stdin);
		selection=atoi(input);
		if(!validate(category,selection))
		{
			printf("\nSelection already chosen; please select a different one\n\n");
			return 0;
		}
		//three of a kind (selection=1) or four of a kind (selection=2)
		if(selection==1 || selection==2)
		{
			int die=0;
			int freq[6]={0};

			for(i=0;i<5;i++) //find the dice value frequency in each of the 5 dice rolls
			{
				freq[dice[i]-1]++;
			}
			for(i=0;i<6;i++)
			{
				if(selection==1 && freq[i]>=3)
				{
					sum+=(i+1)*freq[i]; //sum = dice value (or x+1) * its frequency
					break;
				}
				else if(selection==2 && freq[i]>=4)
				{
					sum+=(i+1)*freq[i];
					break;
				}
			}
			
			lowerSec[selection-1]=sum;
			lowerScore+=sum;
		}
		//fullhouse
		else if(selection==3)
		{
			//first three die are a 3 of a kind, last two die are a pair
			if((dice[0]==dice[1]) && (dice[1]==dice[2]) && (dice[3]==dice[4]))
			{
				lowerSec[2]=25;
				lowerScore+=25;
			}
			//first two die are a pair, last three die are a 3 of a kind
			else if((dice[0]==dice[1]) && (dice[2]==dice[3]) && (dice[3]==dice[4]))
			{
				lowerSec[2]=25;
				lowerScore+=25;
			}
			else
			{
				lowerSec[2]=0;
			}
		}
		//small straight or large straight
		else if(selection==4 || selection==5)
		{
			int valid=1; //used to compare consecutive length to determine straight or not
			//selection can be used as the "straight-consecutive length"
			for(i=0;i<4;i++)
			{
				//printf("dice[i+1]=%d\tdice[i]+1=%d\n",dice[i+1],dice[i]+1);
				if(dice[i+1]==dice[i]+1) //consecutive
				{
					valid++;
				}
				else if(dice[i]==dice[i+1]) //skip if encounter duplicate --> important for small straight
				{
					continue;
				}
				else //not a straight
				{
					lowerSec[selection-1]=0;
					return;
				}

			}
			if(valid==selection)
			{
				//small straight
				if(selection==4)
				{
					lowerSec[3]=30;
					lowerScore+=30;
				}
				//large straight
				else if(selection==5)
				{
					lowerSec[4]=40;
					lowerScore+=40;
				}
			}
				
		}
		//yahtzee
		else if(selection==6)
		{
			if(dice[0]==dice[1] && dice[1]==dice[2] && dice[2]==dice[3] && dice[3]==dice[4])
			{
				lowerSec[5]=50;
				lowerScore+=50;
			}
			else
				lowerSec[5]=0;
		}
		//chance
		else if(selection==7)
		{
			for(i=0;i<5;i++)
			{
				sum+=dice[i];
			}
			lowerSec[6]=sum;
			lowerScore+=sum;
		}
		return 1;
	}
	else
	{
		printf("Pick any appropriate category (1 or 2)\n");
		return 0;
	}
}
//validate to prevent rechoosing a category
int validate(int cat, int select)
{
	if(cat==1)
	{
		if(upperSec[select-1]>=0)
			return 0;
		return 1;
	}
	else
	{
		if(lowerSec[select-1]>=0)
			return 0;
		return 1;
	}
}

void printScores()
{
	int total=upperScore+lowerScore;
	if(upperScore>=63)
	{
		upperBonus=35;
		total+=35;
	}
	printf("\nYour score so far is: %d\n\n",total);
	
	char output1[5],output2[5];
	snprintf(output1,5,"%d",upperSec[0]);
	snprintf(output2,5,"%d",upperSec[3]);
	printf("Ones: %s\t\t\tFours: %s\n",(upperSec[0]!=-1)?output1:" ",(upperSec[3]!=-1)?output2:" ");
	snprintf(output1,5,"%d",upperSec[1]);
	snprintf(output2,5,"%d",upperSec[4]);
	printf("Twos: %s\t\t\tFives: %s\n",(upperSec[1]!=-1)?output1:" ",(upperSec[4]!=-1)?output2:" ");
	snprintf(output1,5,"%d",upperSec[2]);
	snprintf(output2,5,"%d",upperSec[5]);
	printf("Threes: %s\t\tSixes: %s\n",(upperSec[2]!=-1)?output1:" ",(upperSec[5]!=-1)?output2:" ");
	printf("Upper Section Bonus: %d\n",upperBonus);
	snprintf(output1,5,"%d",lowerSec[0]);
	snprintf(output2,5,"%d",lowerSec[1]);
	printf("Three of a Kind: %s\t\tFour of a Kind: %s\n",(lowerSec[0]!=-1)?output1:" ",(lowerSec[1]!=-1)?output2:" ");
	snprintf(output1,5,"%d",lowerSec[3]);
	snprintf(output2,5,"%d",lowerSec[4]);
	printf("Small Straight: %s\t\tLarge Straight: %s\n",(lowerSec[3]!=-1)?output1:" ",(lowerSec[4]!=-1)?output2:" ");
	snprintf(output1,5,"%d",lowerSec[2]);
	snprintf(output2,5,"%d",lowerSec[5]);
	printf("Full House: %s\t\t\tYahtzee: %s\n",(lowerSec[2]!=-1)?output1:" ",(lowerSec[5]!=-1)?output2:" ");
	snprintf(output1,5,"%d",lowerSec[6]);
	printf("Chance: %s\n\n",(lowerSec[6]!=-1)?output1:" ");
	
	
}