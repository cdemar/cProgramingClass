//#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SUITS 4
#define FACES 13
#define CARDS 52

#define HANDSIZE 7
#define TABLESIZE 5

#define TRUE 1
#define FALSE 0

enum Play
{
    HIGH_CARD,
    PAIR,
    TWO_PAIR,
    THREE_OF_KIND,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_OF_KIND,
    STRAIGHT_FLUSH,
    ROYAL_FLUSH
};

static int pot = 0;
static int handCount = -1; //make initially 0
static int card = 1; //card to be dealt
static int foldCount = 0;
static int playerNum = 0;
static FILE *fptr = NULL;
static int round = 0;

struct card
{
    unsigned int suit;
    unsigned int face;
};

struct player
{
    struct card hand[HANDSIZE];
    int money;
    int currentBet;
    int fold;
    enum Play bestPlay;
};

// initialize suit array
const char *suit[SUITS] =
{ "Hearts", "Diamonds", "Clubs", "Spades" };

// initialize face array
const char *face[FACES] =
{ "Deuce", "Three", "Four", "Five",
    "Six", "Seven", "Eight", "Nine",
    "Ten", "Jack", "Queen", "King", "Ace" };

void shuffle(unsigned int deckArray[][FACES]);

// should deal 2 cards to each player’s hand array
void dealHands(struct player playersArray[], unsigned int deckArray[][FACES]);

// should deal the same card to each player’s hand array and to the table array and display table // array
void dealTable(struct player playersArray[], struct card tableArray[TABLESIZE], unsigned int deckArray[][FACES]);

// should display menu bet and fold for each player, should make sure everyone bets the same //amount in the end, it should adjust each player’s current bet, when finished it should increase //the pot, if all fold except one should jump to end and that player gets the pot
void bet(struct player playersArray[], struct card tableArray[]);

// assign the enum inside the player structure with their best possible play
void findPlays(struct player playersArray[]);

//  should compare all the players best possible plays, should also take care of ties, should //assign player amounts and reset the pot
void findBestPlay(struct player playersArray[]);

void displayTable(struct card tableArray[], const char *face[], const char *suit[]);
void displayCards(struct player p, const char *face[], const char *suit[]);

void emptyDeckArray(unsigned int deckArray[][FACES]);

void copyArr(const struct card hand[], struct card copy[]);
void static clear(void);

// displays amounts in players Array
void displayAmount(struct player playersArray[]);

int isFourOfAKind(const struct card hand[]);
int isThreeOfAKind(const struct card hand[]);
int isFlush(const struct card hand[]);
int isStraight(const struct card hand[]);
int isFullHouse(const struct card hand[]);
int isTwoPair(const struct card hand[]);
int isPair(const struct card hand[]);
int getHighCard(const struct card hand[], int size);
int isStraightFlush(const struct card hand[]);
int isRoyalFlush(const struct card hand[]);


int main()
{
    if ((fptr = fopen("/Users/corydemar/Desktop/Programing/Language_C/finalProj/finalProj/report.txt", "w")) == NULL)
        puts("File could not be opened");
    
    srand((unsigned int)time(NULL));
    
    // initialize deck array
    unsigned int deckArray[SUITS][FACES] = { 0 };
    
    do
    {
        puts("Enter player number, between 2 to 4");
        scanf("%d", &playerNum);
    } while (playerNum > 4 || playerNum < 2);
    
    fprintf(fptr, "%s %d", "Number of player: ", playerNum);
    fprintf(fptr, "%s", "\n");
    fflush(fptr);
    
    // an array full of pointers to struct player(have to change displayCards)
    struct player *playersArray = malloc(playerNum * sizeof(struct player));
    struct card tableArray[TABLESIZE];
    
    //set defaults
    for (int i = 0; i < playerNum; i++)
    {
        playersArray[i].fold = FALSE;
        playersArray[i].money = 50;
        playersArray[i].currentBet = 0;
    }
    
    int check = 1;
    
    while (check == 1)
    {
        round++;
        fprintf(fptr, "\n\n%s %d\n", "ROUND", round);
        
        emptyDeckArray(deckArray);
        
        shuffle(deckArray);
        
        dealHands(playersArray, deckArray);
        
        bet(playersArray, tableArray);
        
        fprintf(fptr, "\n%s\n", "Cards on Table");
        
        if (foldCount != playerNum - 1)
        {
            for (int i = 0; i < 3; i++)
                dealTable(playersArray, tableArray, deckArray);
            
            fprintf(fptr, "%s", "\n");
            bet(playersArray, tableArray);
        }
        
        if (foldCount != playerNum - 1)
        {
            dealTable(playersArray, tableArray, deckArray);
            fprintf(fptr, "%s", "\n");
            bet(playersArray, tableArray);
        }
        
        if (foldCount != playerNum - 1)
        {
            dealTable(playersArray, tableArray, deckArray);
            fprintf(fptr, "%s", "\n");
            bet(playersArray, tableArray);
        }
        
        findPlays(playersArray);
        displayAmount(playersArray);
        
        int count = 0;
        for (int i = 0; i < playerNum; i++)
        {
            if (playersArray[i].money == 0)
                count++;
        }
        
        if (count == (playerNum - 1))
            check = 0;
        
        handCount = -1;
    }
    
    
    // outside of while loop
    for (int i = 0; i < playerNum; i++)
    {
        if (playersArray[i].money > 0)
        {
            printf("Player %d Won\n", i + 1);
            fprintf(fptr, "\nPLAYER %d WON\n", i + 1);
            fflush(fptr);
        }
    }
    
    system("pause");
    fflush(fptr);
    round = 0;
    
    return 0;
}

// Fills the deck array with zeros
void emptyDeckArray(unsigned int deckArray[][FACES])
{
    for (int i = 0; i < SUITS; i++)
    {
        for (int j = 0; j < FACES; j++)
            deckArray[i][j] = 0;
    }
}

// should deal 2 cards to each player’s hand array
void dealHands(struct player playersArray[], unsigned int deckArray[][FACES])
{
    fprintf(fptr, "\n%s \n", "Players' hands: \n");
    
    //loop through each player
    for (unsigned int i = 0; i < playerNum; i++)
    {
        fprintf(fptr, "%s %d %s\n", "Player ", i + 1, "hands:");
        
        // deal 2 times each player
        for (unsigned int t = 0; t < 2; ++t)
        {
            // loop through rows of deckArray
            for (unsigned int row = 0; row < SUITS; ++row)
            {
                // loop through columns of deckArray for current row
                for (unsigned int column = 0; column < FACES; ++column)
                {
                    // if slot contains current card, display card
                    if (deckArray[row][column] == card)
                    {
                        playersArray[i].hand[t].face = column;
                        playersArray[i].hand[t].suit = row;
                        
                        fprintf(fptr, "%s of %s\n", face[column], suit[row]);
                    }
                }
            }
            card++; //next card to be dealt
        }
        fprintf(fptr, "%s", "\n");
    }
    fflush(fptr);
}

// should deal the same card to each player’s hand array
//and to the table array and display table array
void dealTable(struct player playersArray[], struct card tableArray[TABLESIZE], unsigned int deckArray[][FACES])
{
    if (handCount == 4)
        handCount = 0;
    else
        ++handCount;
    
    if (handCount == 3)
    {
        fprintf(fptr, "\n%s \n", "Cards on table:");
        for (int i = 0; i < 3; i++)
            fprintf(fptr, "%s of %s \n", face[tableArray[i].face], suit[tableArray[i].suit]);
    }
    else if (handCount == 4)
    {
        fprintf(fptr, "\n%s \n", "Cards on table:");
        for (int i = 0; i < 4; i++)
            fprintf(fptr, "%s of %s \n", face[tableArray[i].face], suit[tableArray[i].suit]);
    }
    
    for (unsigned int row = 0; row < SUITS; ++row)
    {
        // loop through columns of deckArray for current row
        for (unsigned int column = 0; column < FACES; ++column)
        {
            // if slot contains current card, display card
            if (deckArray[row][column] == card)
            {
                tableArray[handCount].face = column;
                tableArray[handCount].suit = row;
                fprintf(fptr, "%s of %s\n", face[column], suit[row]);
                
                //deal the same card to each player
                for (unsigned int i = 0; i < playerNum; i++)
                {
                    //first 2 slot in player's hand is occupied after dealhand,
                    //dealtable deals card from 3rd slot in player's hand
                    playersArray[i].hand[handCount + 2].face = column;
                    playersArray[i].hand[handCount + 2].suit = row;
                }
                // increment handcount so next time func called will place in next slot
            }
        }
    }
    card++; //next card to be dealt
    
    fflush(fptr);
}

//places cards in random spots of the deck array
void shuffle(unsigned int deckArray[][FACES])
{
    // for each of the cards, choose slot of deck randomly
    for (unsigned int card = 1; card <= CARDS; ++card)
    {
        unsigned int row;    // row number
        unsigned int column; // column number
        
        // choose new random location until unoccupied slot found
        do
        {
            row = rand() % SUITS;
            column = rand() % FACES;
        } while (deckArray[row][column] != 0); // end do...while
        
        // place card number in chosen slot of deck
        deckArray[row][column] = card;
    }
}

// Displays the cards dealt to the table
void displayTable(struct card tableArray[], const char *face[], const char *suit[])
{
    printf("Cards on table:\n");
    
    //handCount initially is -1
    // will be increased 5 times during the game
    for (int i = 0; i <= handCount; i++)
        printf("%s of %s \n", face[tableArray[i].face], suit[tableArray[i].suit]);
    
    puts("\n");
}


//display cards of one player, used in bet
void displayCards(struct player p, const char *face[], const char *suit[])
{
    printf("Players' hands: \n");
    for (int u = 0; u < 2; u++)
        printf("%s of %s \n", face[p.hand[u].face], suit[p.hand[u].suit]);
    
    puts("\n");
}

/*Makes sure every bets the highest amount a player bets, limited by the lowest money an
 active player has. Asks if they want to initially fold and bet and then there is a second
 iteration that makes sure every bets the same amount*/
void bet(struct player playersArray[], struct card tableArray[])
{
    int input;
    int amount = 0;
    int highest = 0;
    
    // find the lowest amount of money players can bet that is not zero.
    int lowest = playersArray[0].money;
    int x = 1;
    while (lowest == 0)
    {
        if (playersArray[x].money != 0)
            lowest = playersArray[x].money;
        
        x++;
        if (x == playerNum)
            break;
    }
    for (int i = 0; i < playerNum; i++)
    {
        if (playersArray[i].fold == TRUE)
            continue;
        
        if (playersArray[i].money == 0)
            goto ENDBET;
        
        if (playersArray[i].money < lowest)
            lowest = playersArray[i].money;
    }
    
    // initial setting of ammounts and fold variables
    int j = 0;
    for (j = 0; j < playerNum; j++)
    {
        if (playersArray[j].fold == TRUE)
            continue;
        
        if (foldCount == playerNum - 1)
            goto ENDBET;
        
        do
        {
            fprintf(fptr, "%s %d ", "Player ", j + 1);
            displayCards(playersArray[j], face, suit);
            displayTable(tableArray, face, suit);
            printf("Player %d , Current Money %d , Enter a Command.\n", j + 1, playersArray[j].money);
            puts("0 - Bet");
            puts("1 - Fold");
            scanf("%d", &input);
        } while (input < 0 || input > 1);
        
        if (input == 0)
            fprintf(fptr, "%s", " bets");
        
        if (input == 1)
            fprintf(fptr, "%s\n", " folds");
        
        switch (input)
        {
            case 0:
                do
                {
                    printf("Enter Bet between 0 - %d: \n", lowest);
                    scanf("%d", &amount);
                } while (amount < 0 || amount > lowest);
                
                fprintf(fptr, " %d\n", amount);
                
                playersArray[j].fold = FALSE;
                playersArray[j].currentBet = amount;
                pot += amount;
                playersArray[j].money -= amount;
                break;
            case 1:
                playersArray[j].currentBet = 0;
                playersArray[j].fold = TRUE;
                ++foldCount;
        }
        clear();
    }
    
    
    // quick loop to get the current highest bet
    highest = playersArray[0].currentBet;
    for (int i = 1; i < playerNum; i++)
    {
        if (playersArray[i].fold == TRUE)
            continue;
        
        if (highest == 0)
            highest = playersArray[i].currentBet;
        
        if (playersArray[i].currentBet > highest)
            highest = playersArray[i].currentBet;
    }
    
    fprintf(fptr, "\n%s %d\n", "Current highest bet is ", highest);
    
    //another loop to make sure everyone bet the same amount or folds,
    // change to while, probably keep a count of players OK to proceed
    // increment after fix they current amount
    // decrement by player number except the one who raises the bet (have to check everyone except them)
    int difference = 0;
    for (int i = 0; i < playerNum; i++)
    {
        if (playersArray[i].fold == TRUE || playersArray[i].currentBet == highest)
            continue;
        else
        {
            fprintf(fptr, "%s %d ", "Player", i + 1);
            
            difference = highest - playersArray[i].currentBet;
            printf("This was the highest bet, %d, you must fold or match that bet by betting %d.\n", highest, difference);
            do
            {
                displayCards(playersArray[i], face, suit);
                displayTable(tableArray, face, suit);
                printf("Player %d , Current Money %d : Enter a Command.\n", i + 1, playersArray[i].money);
                puts("0 - Bet");
                puts("1 - Fold");
                scanf("%d", &input);
            } while (input < 0 || input > 1);
            
            if (input == 0)
                fprintf(fptr, "%s", "Bet");
            
            if (input == 1)
                fprintf(fptr, "%s\n", "Fold");
            
            switch (input)
            {
                case 0:
                    do
                    {
                        puts("Enter difference amount: ");
                        scanf("%d", &amount);
                        printf("Amount: %d\n", amount);
                    } while (amount != difference);
                    
                    fprintf(fptr, " another %d to match %d\n", amount, highest);
                    
                    playersArray[i].fold = FALSE;
                    playersArray[i].currentBet = amount;
                    pot += amount;
                    playersArray[i].money -= amount;
                    break;
                case 1:
                    if ((pot/playerNum) != amount)
                    {
                        pot -= amount;
                        playersArray[i].money += amount;
                    }
                    playersArray[i].currentBet = 0;
                    playersArray[i].fold = TRUE;
                    foldCount++;
            }
        }
        clear();
    }
    fflush(fptr);
ENDBET:;
}

// scrolls the console down
void static clear()
{
    for (int i = 0; i <= 25; i++)
        puts("");
}
// assign the enum inside the player structure with their best possible play
// then calls the function to decide the winner
void findPlays(struct player playersArray[])
{
    for (int i = 0; i < playerNum; i++)
    {
        if (playersArray[i].fold == TRUE)
        {
            playersArray[i].bestPlay = -1;
            printf("Player %d folded.\n", i + 1);
            fprintf(fptr, "Player %d folded.\n", i + 1);
            
            continue;
        }
        else
        {
            // everyone will have a highest card by default
            playersArray[i].bestPlay = HIGH_CARD;
            
            if (isPair(playersArray[i].hand))
                playersArray[i].bestPlay = PAIR;
            
            if (isTwoPair(playersArray[i].hand))
                playersArray[i].bestPlay = TWO_PAIR;
            
            if (isThreeOfAKind(playersArray[i].hand))
                playersArray[i].bestPlay = THREE_OF_KIND;
            
            if (isStraight(playersArray[i].hand))
                playersArray[i].bestPlay = STRAIGHT;
            
            if (isFlush(playersArray[i].hand))
                playersArray[i].bestPlay = FLUSH;
            
            if (isFullHouse(playersArray[i].hand))
                playersArray[i].bestPlay = FULL_HOUSE;
            
            if (isFourOfAKind(playersArray[i].hand))
                playersArray[i].bestPlay = FOUR_OF_KIND;
            
            if (isStraightFlush(playersArray[i].hand))
                playersArray[i].bestPlay = STRAIGHT_FLUSH;
            
            if (isRoyalFlush(playersArray[i].hand))
                playersArray[i].bestPlay = ROYAL_FLUSH;
            
            switch (playersArray[i].bestPlay)
            {
                case HIGH_CARD:
                    printf("player %d got a high Card.\n", i + 1);
                    fprintf(fptr, "\nPlayer %d got a high Card.\n", i + 1);
                    break;
                case PAIR:
                    printf("player %d got a Pair.\n", i + 1);
                    fprintf(fptr, "\nPlayer %d got a Pair.\n", i + 1);
                    break;
                case TWO_PAIR:
                    printf("player %d got a Two Pair.\n", i + 1);
                    fprintf(fptr, "\nPlayer %d got a Two Pair.\n", i + 1);
                    break;
                case THREE_OF_KIND:
                    printf("player %d got a Three of a kind.\n", i + 1);
                    fprintf(fptr, "\nPlayer %d got a Three of a kind.\n", i + 1);
                    break;
                case STRAIGHT:
                    printf("player %d got a Straight.\n", i + 1);
                    fprintf(fptr, "\nPlayer %d got a Straight.\n", i + 1);
                    break;
                case FLUSH:
                    printf("player %d got a Flush.\n", i + 1);
                    fprintf(fptr, "\nPlayer %d got a Flush.\n", i + 1);
                    break;
                case FULL_HOUSE:
                    printf("player %d got a Full House.\n", i + 1);
                    fprintf(fptr, "\nPlayer %d got a Full House.\n", i + 1);
                    break;
                case FOUR_OF_KIND:
                    printf("player %d got a Four of a Kind.\n", i + 1);
                    fprintf(fptr, "\nPlayer %d got a Four of a Kind.\n", i + 1);
                    break;
                case STRAIGHT_FLUSH:
                    printf("player %d got a Straight Flush.\n", i + 1);
                    fprintf(fptr, "\nPlayer %d got a Straight Flush.\n", i + 1);
                    break;
                case ROYAL_FLUSH:
                    printf("player %d got a Royal Flush.\n", i + 1);
                    fprintf(fptr, "\nPlayer %d got a Royal Flush.\n", i + 1);
            }
        }
    }
    fflush(fptr);
    findBestPlay(playersArray);
}

//  should compare all the players best possible plays, takes care if a tie occurs between plays
// and high cards, should assign player amounts and reset the pot
void findBestPlay(struct player playersArray[])
{
    //functions that should only be used in this function
    int ties(struct player playersArray[], int winner);
    int checkHighCards(int highCards[], int * tiedHandPtr);
    
    if (foldCount != playerNum - 1)
    {
        //Finding the best hand out of all the players
        int winHand = playersArray[0].bestPlay;
        for (int i = 1; i < playerNum; i++)
        {
            if (playersArray[i].bestPlay > winHand && playersArray[i].fold == FALSE)
                winHand = playersArray[i].bestPlay;
        }
        
        //How many people involved in the tie
        int count = ties(playersArray, winHand);
        
        /*Want to set winner index so that we can
         specify who in the playersArray should be
         given the pot of money, made an array in
         case multiple players win the pot*/
        int * winnerIndex = malloc(playerNum * sizeof(int));
        
        //fill with zeros
        for (int i = 0; i < playerNum; i++)
            winnerIndex[i] = 0;
        
        /*
         if a tie occured have to check high cards of those involved,
         it is possible to have tie between high cards as well
         */
        if (count > 1)
        {
            // variable sized array to hold each players highest card
            int * highCards = malloc(playerNum * sizeof(int));
            
            /*
             Initialize array to be filled with -1, if we
             see a -1 we know they are not apart of the tie
             */
            for (int i = 0; i < playerNum; i++)
                highCards[i] = -1;
            
            /*
             Checking the highcard of players involved in the tie
             Only looking at the two delt cards because everyone has
             the cards on the table, wouldn't be efficient to check.
             Want to create 2D array of all players cards in case
             there is a tie between high Cards, in getHighCards
             record what high card was but invalidate the value in
             deltCards for if another tie occurs.
            
             2d array done using double pointers, deltCards holds
             array of pointers to an array of pointers that point to
             card structures first allocate memory for rows, playerNum
             rows that hold struct card pointers, second allocate memory
             for the columns, 2 card structs.
             */
            struct card ** deltCards = (struct card **)malloc(playerNum * sizeof(struct card *));
            for (int i = 0; i < playerNum; i++)
                deltCards[i] = (struct card *)malloc(2 * sizeof(struct card));
            
            // looks at players apart of the tie
            for (int i = 0; i < playerNum; i++)
            {
                if (playersArray[i].bestPlay == winHand && playersArray[i].fold == FALSE)
                {
                    // set deltCards in case of tie
                    deltCards[i][0].face = playersArray[i].hand[0].face;
                    deltCards[i][0].suit = playersArray[i].hand[0].suit;
                    deltCards[i][1].face = playersArray[i].hand[1].face;
                    deltCards[i][1].suit = playersArray[i].hand[1].suit;
                    
                    /* recording the highest card in high cards array
                     pass row to getHighCards, like passing a hand of 2 card*/
                    highCards[i] = getHighCard(deltCards[i], 2);
                    
                    // invalidating the card face that was the highCard in case of second tie
                    if (deltCards[i][0].face == highCards[i])
                        deltCards[i][0].face = -1;
                    else
                        deltCards[i][1].face = -1;
                }
            }
            
            /* if -1 there is a tie between the highCards
             if it is not -1 then we have the index of the winner*/
            int tiedHand = 0;
            int * tiedHandPtr = &tiedHand;
            int index = 0;
            if ((index = checkHighCards(highCards, tiedHandPtr)) == -1)
            {
                /*since there is a tie between delt cards of players
                 we need to resolve that tie, by looking at there next
                 highest card between players*/
                for (int i = 0; i < playerNum; i++)
                {
                    if (highCards[i] == tiedHand)
                    {
                        // pick the card that isn't -1
                        if (deltCards[i][0].face == -1)
                            highCards[i] = deltCards[i][1].face;
                        else
                            highCards[i] = deltCards[i][0].face;
                    }
                    else
                        highCards[i] = -1;
                }
                
                /* if we get a -1 then the pot must be split between multiple
                 players, if we get anything other than -1 that is the index
                 of the winning player*/
                if ((index = checkHighCards(highCards, tiedHandPtr)) == -1)
                {
                    //have a final tie, have to mark who is tied
                    for (int i = 0; i < playerNum; i++)
                    {
                        if (highCards[i] == tiedHand)
                            winnerIndex[i] = 1;
                    }
                }
            }
            
            // sets the winning player with a 1
            if (index != -1)
                winnerIndex[index] = 1;
            
            free(highCards);
            free(deltCards);
        }
        // no tie occured, one player won the pot, just have to set winnerIndex
        else
        {
            for (int i = 0; i < playerNum; i++)
            {
                if (playersArray[i].bestPlay == winHand)
                    winnerIndex[i] = 1;
            }
        }
        
        //end section, display winner, distribute pot and reset
        
        // check how many apart of the tie
        count = 0;
        for (int i = 0; i < playerNum; i++)
        {
            if (winnerIndex[i] == 1)
                ++count;
        }
        
        if (count == 1)
        {
            for (int i = 0; i < playerNum; i++)
            {
                if (winnerIndex[i] == 1)
                {
                    printf("The winner is %d and won %d\n", i + 1, pot);
                    fprintf(fptr, "\nThe winner is %d and won %d\n", i + 1, pot);
                    playersArray[i].money += pot;
                }
            }
        }
        else
        {
            fprintf(fptr, "\nWe have a tie!");
            for (int i = 0; i < playerNum; i++)
            {
                if (winnerIndex[i] == 1)
                {
                    printf("The winner is %d and won %d.\n", i + 1, pot / count);
                    fprintf(fptr, "\nThe winner is %d and won %d.\n", i + 1, pot / count);
                    playersArray[i].money += pot / count;
                }
            }
        }
        free(winnerIndex);
    }
    else
    {
        for (int i = 0; i < playerNum; i++)
        {
            if (playersArray[i].fold == FALSE)
            {
                printf("The winner is %d and won %d\n", i + 1, pot);
                fprintf(fptr, "\nThe winner is %d and won %d\n", i + 1, pot);
                playersArray[i].money += pot;
                break;
            }
        }
    }
    puts("");
    fprintf(fptr, "\n");
    
    pot = 0;
    for (int i = 0; i < playerNum; i++)
    {
        if (playersArray[i].money > 0 && playersArray[i].fold == TRUE)
        {
            playersArray[i].fold = FALSE;
            --foldCount;
        }
        else if (playersArray[i].money == 0 && playersArray[i].fold == FALSE)
        {
            playersArray[i].fold = TRUE;
            ++foldCount;
        }
    }
    fflush(fptr);
}

/*Checks our highCards array, if one person won we
 return the index of that player, if a tie occured
 we return -1, fills tiedHand with the face value of the
 card value if a tie occurs*/
int checkHighCards(int highCards[], int * tiedHandPtr)
{
    /*Find the best high card int the array
     and record the index at the same time
     also keep a count of your current best*/
    int best = highCards[0];
    int index = 0;
    int count = 1;
    for (int i = 1; i < playerNum; i++)
    {
        if (highCards[i] == -1)
            continue;
        else if (highCards[i] == best)
        {
            *tiedHandPtr = best;
            count++;
        }
        else if (highCards[i] > best)
        {
            best = highCards[i];
            index = i;
            count = 1;
        }
    }
    
    if (count == 1)
        return index;
    else
        return -1;
}

// Counting the number of players with the winning hand
// 1 means there is no tie, > 1 means a tie occured
int ties(struct player playersArray[], int winHand)
{
    int count = 0;
    for (int i = 0; i < playerNum; i++)
    {
        if (playersArray[i].bestPlay == winHand && playersArray[i].fold == FALSE)
            count++;
    }
    return count;
}

// Displays players money
void displayAmount(struct player playersArray[])
{
    printf("Players' money: \n");
    fprintf(fptr, "Players' money: \n");
    for (int i = 0; i < playerNum; i++)
    {
        printf("Player number %d: %d \n", i + 1, playersArray[i].money);
        fprintf(fptr, "Player number %d: %d \n", i + 1, playersArray[i].money);
    }
    puts("\n");
}

/* Will check every permutation of cards, counts
 if a card face occurs twice*/
int isPair(const struct card hand[])
{
    int count = 0;
    for (int i = 0; i < HANDSIZE; i++)
    {
        for (int j = 0; j < HANDSIZE; j++)
        {
            if (hand[i].face == hand[j].face)
                ++count;
            
            if (count == 2)
                return TRUE;
        }
        count = 0;
    }
    return FALSE;
}

/* finds a pair, removes them, then finds another pair.
 Does this operation on a copy of the hand so that the player
 structure does not hold an invalid hand*/
int isTwoPair(const struct card hand[])
{
    void copyArr(const struct card hand[], struct card copy[]);
    int twoPairHelper(const struct card hand[], int *index1, int *index2);
    struct card copy[HANDSIZE];
    copyArr(hand, copy);
    int counter = 100;
    
    if (isPair(copy))
    {
        int index1;
        int index2;
        int * ptr1 = &index1;
        int * ptr2 = &index2;
        // find first pair
        twoPairHelper(copy, ptr1, ptr2);
        // remove card from copy
        copy[index1].face = counter++;
        copy[index2].face = counter++;
        if (isPair(copy))
            return TRUE;
    }
    return FALSE;
}

// copies hand into another struct card array
void copyArr(const struct card hand[], struct card copy[])
{
    for (int i = 0; i < HANDSIZE; i++)
    {
        copy[i].face = hand[i].face;
        copy[i].suit = hand[i].suit;
    }
}

// repeat of pair function but records index of found cards, only runs if there is a pair, used to remove pair in twoPair
int twoPairHelper(const struct card hand[], int *index1, int *index2)
{
    int count = 0;
    for (int i = 0; i < HANDSIZE; i++)
    {
        for (int j = 0; j < HANDSIZE; j++)
        {
            if (hand[i].face == hand[j].face)
            {
                ++count;
                if (count == 1)
                    *index1 = i;
                else
                    *index2 = j;
            }
            if (count == 2)
                return TRUE;
        }
        count = 0;
    }
    return FALSE;
}

/* checks all permutations of cards, returns true when it
 find a card face has occured three times*/
int isThreeOfAKind(const struct card hand[])
{
    int count = 0;
    for (int i = 0; i < HANDSIZE; i++)
    {
        for (int j = 0; j < HANDSIZE; j++)
        {
            if (hand[i].face == hand[j].face)
                ++count;
            
            if (count == 3)
                return TRUE;
        }
        count = 0;
    }
    return FALSE;
}

/* checks all permutations of cards, returns true when it
 find a card face has occured four times*/
int isFourOfAKind(const struct card hand[])
{
    int count = 0;
    for (int i = 0; i < HANDSIZE; i++)
    {
        for (int j = 0; j < HANDSIZE; j++)
        {
            if (hand[i].face == hand[j].face)
                ++count;
            
            if (count == 4)
                return TRUE;
        }
        count = 0;
    }
    return FALSE;
}

/* Checks suit of every card in the hand against the
 suit of the first card, if it doesn't match return false
 else return true*/
int isFlush(const struct card hand[])
{
    unsigned int suit = hand[0].suit;
    for (int i = 0; i < HANDSIZE; i++)
    {
        if (hand[i].suit != suit)
            return FALSE;
    }
    return TRUE;
}

/* checks if hand has sequence of cards starting from the smallest face
 will not run if the smallest card is a jack and checks for a special case
 of ACE, DEUCE, THREE, FOUR, FIVE*/
int isStraight(const struct card hand[])
{
    unsigned int smallest = hand[0].face;
    for (int i = 1; i < HANDSIZE; i++)
    {
        if (hand[i].face < smallest)
            smallest = hand[i].face;
    }
    
    if (smallest > 9) // cannot have a straight if the smallest card is a jack
        return FALSE;
    
    // checking for special case, ACE, DEUCE, THREE, FOUR, FIVE
    if (smallest == 0)
    {
        //check for Ace
        for (int i = 0; i < HANDSIZE; i++)
        {
            // found ace and deuce
            if (hand[i].face == 12)
            {
                int found = 0;
                for (int count = 1; count <= 3; count++)
                {
                    for (int j = 0; j < HANDSIZE; j++)
                    {
                        if (hand[j].face == count)
                        {
                            found = 1;
                            break;
                        }
                    }
                    if (found)
                        found = 0;
                    else
                        return FALSE;
                }
                return TRUE;
            }
        }
    }
    
    // Searching for card equal to count, starts at smallest value
    int found = 0;
    for (int count = smallest; count <= smallest + 4; count++)
    {
        for (int j = 0; j < HANDSIZE; j++)
        {
            if (hand[j].face == count)
                found = 1;
        }
        if (found)
            found = 0;
        else
            return FALSE;
    }
    return TRUE;
}

/*Three of a kind of the same face, and a pair of a different face.
 same tech as isTwoPair, finds three of a kind, invalidates them,
 then finds a Pair with different face, all done on a copy of the hand*/
int isFullHouse(const struct card hand[])
{
    int fullHouseHelperTOK(const struct card copy[], int * ptr1, int * ptr2, int * ptr3);
    int fullHouseHelperPair(const struct card copy[], unsigned int dif);
    
    struct card copy[HANDSIZE];
    copyArr(hand, copy);
    
    int counter = 100;
    
    if (isThreeOfAKind(copy))
    {
        unsigned int dif = 0;
        int index1;
        int index2;
        int index3;
        int * ptr1 = &index1;
        int * ptr2 = &index2;
        int * ptr3 = &index3;
        // find first pair
        dif = fullHouseHelperTOK(copy, ptr1, ptr2, ptr3);
        // remove card from copy by changing to not matching number not in set
        copy[index1].face = counter++;
        copy[index2].face = counter++;
        copy[index3].face = counter++;
        if (fullHouseHelperPair(copy, dif))
            return TRUE;
    }
    return FALSE;
}

// finds Three of a Kind and sets pointers to index of found cards
int fullHouseHelperTOK(const struct card copy[], int * ptr1, int * ptr2, int * ptr3)
{
    int count = 0;
    for (int i = 0; i < HANDSIZE; i++)
    {
        for (int j = 0; j < HANDSIZE; j++)
        {
            if (copy[i].face == copy[j].face)
            {
                ++count;
                if (count == 1)
                    *ptr1 = j;
                else if (count == 2)
                    *ptr2 = j;
                else
                    *ptr3 = j;
                
                if (count == 3)
                    return copy[i].face;
            }
        }
        count = 0;
    }
    return FALSE;
}

// finds pair with faces that are not equal to dif
int fullHouseHelperPair(const struct card copy[], unsigned int dif)
{
    int count = 0;
    for (int i = 0; i < HANDSIZE; i++)
    {
        for (int j = 0; j < HANDSIZE; j++)
        {
            if (copy[i].face == copy[j].face && copy[i].face != dif)
                ++count;
            
            if (count == 2)
                return TRUE;
        }
        count = 0;
    }
    return FALSE;
}

// return highest card
// changed to be able to have variable size hand
int getHighCard(const struct card hand[], int size)
{
    unsigned int largest = hand[0].face;
    
    for (int i = 1; i < size; i++)
    {
        if (hand[i].face > largest)
            largest = hand[i].face;
    }
    return largest;
}

// Checks if a straight and a flush
int isStraightFlush(const struct card hand[])
{
    if (isStraight(hand) && isFlush(hand))
        return TRUE;
    else
        return FALSE;
}

// checks if it is a straight flush and the highest card is ACE
int isRoyalFlush(const struct card hand[])
{
    if (isStraightFlush(hand) && getHighCard(hand, HANDSIZE) == 12)
        return TRUE;
    else
        return FALSE;
}
