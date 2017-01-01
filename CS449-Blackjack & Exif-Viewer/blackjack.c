//Allen Poon
//CS449 
//Project 1 Part 1 Blackjack Implementation

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int drawCard(int cards[]);
void printPlayer(int numPcards, int player[], int pSum);
void printDealer(int numDcards, int dealer[]);
int calc(int numCards, int hand[]);

int main()
{
	srand((unsigned int)time(NULL));
	int end=0; //determines if the game continues, 0=false, 1=true
	char choice[100]; //player input for hit/stand
	int hit=0, stand=0; //0=false, 1=true depending on player choice
	int player[7];
	int dealer[7];
	int cards[]={1,2,3,4,5,6,7,8,9,10,10,10,10};
	
	//initialize player and dealer hands
	player[0]=drawCard(cards);
	player[1]=drawCard(cards);
	dealer[0]=drawCard(cards);
	dealer[1]=drawCard(cards);
	
	int numPcards=2, numDcards=2; //number of cards in player's hand and dealer's hand respectively
	
	int pSum=calc(numPcards, player); //total value of player's hand
	int dSum=calc(numDcards, dealer); //value of dealer's hand
	
	printf("The dealer:\n? + %d\n\n", dealer[1]);
	printf("You:\n%d + %d = %d\n", player[0], player[1], pSum);
	
	do{
		if(pSum==21)
		{
			printf("Player wins by blackjack!");
			end=1;
		}
		else if(dSum==21)
		{
			printf("Dealer wins by blackjack!");
			end=1;
		}
		else
		{
			printf("\nWould you like to \"hit\" or \"stand\"? ");
			scanf("%s", choice);
			hit=strncmp(choice,"hit",3);
			stand=strncmp(choice,"stand",5);
			while(stand!=0 && hit!=0) //verifies proper input
			{
				printf("\nPlease enter \"hit\" or \"stand\" only: ");
				scanf("%s",choice);
				hit=strncmp(choice,"hit",3);
				stand=strncmp(choice,"stand",5);
			}
			if(dSum<17)
			{
				dealer[numDcards]=drawCard(cards);
				numDcards++;
				//printf("dealer hit\n");
			}
			printDealer(numDcards, dealer);
			if(strncmp(choice,"hit",3)==0)
			{
				player[numPcards]=drawCard(cards);
				numPcards++;
				printPlayer(numPcards, player, pSum);
				//printf("passed player hit\n");
			}
			/*
			else if(strncmp(choice, "stand", 5)==0)
				printf("\n\nYou:\n%d + %d = ", player[numPcards-2], player[numPcards-1]);
			*/
			pSum=calc(numPcards, player);
			dSum=calc(numDcards, dealer);
			printf("%d ",pSum);

			
			if(pSum>21)
			{
				printf("BUSTED!\n\nYou busted. Dealer wins.");
				end=1;
			}
			else if(dSum>21)
			{
				printf("\n\nDealer busted. You win.");
				end=1;
			}
			else if(pSum==21 && dSum==21) //tie
			{
				printf("Both players got blackjack. Push!");
				end=1;
			}
			
		}
		printf("\n");
	}while(end==0);
	printf("\nDealer's hand: %d",dealer[0]);
	for(int i=1;i<numDcards;i++)
	{
		printf(" + %d",dealer[i]);
	}
	return 0;
}

int drawCard(int cards[])
{
	int randIndex=rand()%13;
	int randCard=cards[randIndex]; //store card in player's or dealer's hand
	return randCard;
}

void printPlayer(int numPcards, int player[], int pSum)
{
	printf("\nYou:\n%d + %d = ", pSum, player[numPcards-1]);
}

void printDealer(int numDcards, int dealer[])
{
	printf("\nThe dealer:\n?");
	for(int i=1; i<numDcards; i++)
	{
		printf(" + %d",dealer[i]);
	}
	printf("\n\n");
}

int calc(int numCards, int hand[])
{
	int total=0;
	int aces=0;
	for(int i=0; i<numCards; i++)
	{
		if(hand[i]==1) //ace was drawn
		{
			total+=11; //count ace value as 11 by default
			aces++;
		}
		else
			total+=hand[i];
	}
	for(int f=0; f<aces; f++)
	{
		if(total>21)
		{
			total-=10; //adjust the total based on the number of aces in hand
		}
	}
	return total;
}