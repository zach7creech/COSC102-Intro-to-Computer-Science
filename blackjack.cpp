/*
* Author: Zachery Creech
* COSC102 Fall 2018
* This program simulates a game of blackjack between a dealer and a single player. It deals two cards
* to each the dealer and the player, then allows the player to choose to hit or stand. Over 21 points
* is a bust and automatic loss. After the player stands, the dealer chooses to hit or stand until its
* total score is greater than or equal to 17. Whoever has the most points or does not bust wins. The 
* player also has the option to split their hand if their initial hand contains two of the same value
* card. The dealer will split only when its initial hand contains two aces or two 8s of any suit. The
* dealer will choose to have aces count as 11 only when it makes their total score 20 or 21, and the 
* player has the option to choose 1 or 11 for the value of an ace regardless of total score.
*/

/*
* Disclaimer: I know that I'm not supposed to use non-constant global variables, and I never would in
* actual practice. However, this is a single file that only I am editing, and I make it explicit with
* commenting exactly where those variables are referenced in this project. The only way I could have
* avoided using global variables at this stage in the project would have been including all 8 of those 
* variables as parameters in the functions where they are used: hitOrStand() and finalScore(). 
* hitOrStand() already has a large number of parameters, so I wanted to avoid adding another 8. I 
* I recognize there are many other ways, such as using structs (which we hadn't learned in class yet), 
* but working with what I had already done so far and not wanting to start from scratch, this is the
* best solution I could think of. I understand if my score is deducted as a result, but I wanted to make
* my reasoning clear.
*/

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <ctime>
#include <vector>

using namespace std;

int getRandom(int minimum, int maximum); //random number generator
string dealCard(int deck[], int user); //deals a card to a chosen user
int scoreHand(int deck[], int user); //scores hand of chosen user, also handles choosing value of ace for dealer
string displayCards(const vector<string> &userCards); //creates single string from a given vector of strings for printing in 20 character field
void replaceDeckIndex(int deck[], vector<string> &userCards, string cardName, int user, int newUser); 
//reassigns deck indexes when splitting a hand, takes deck to use, the user's hand (vector) to edit by reference, the user's card (string) to compare to when searching for card to replace, the user's ID (either 1 for dealer or 2 for player) and the owner of the new hand (either 3 for dealerSplit or 4 for playerSplit)
void hitOrStand(int deck[], int user, int &userScore, string &userHand, vector<string> &userCards, bool splitD, bool splitP); 
// hitOrStand takes a deck to use, the user who's hitting or standing (player or dealer), the user's score to be edited by reference, the user's hand (string) to be edited by reference, the user's hand (vector) to be edited by reference, bool to tell if dealer has split, and bool to tell if player has split
int blackjack(int dealerScore, int playerScore, int dealerSplitScore, int playerSplitScore); //checks for blackjacks, ending the game if it occurs, takes all scores
void initializeDeck(int deck[]); //initializes deck to elements of all value 0
int checkAce(int deck[], int player); //allows player to choose value of ace, follows every instance of player being given option to hit, takes deck to use and current user
void finalScore(int dealerScore, int playerScore, int dealerSplitScore, int playerSplitScore);

string dealerHand, playerHand, dealerSplitHand, playerSplitHand; //global variables are referenced ONLY in main(), hitOrStand(), and finalScore() for ease of access
int dealerScore = 25, playerScore = 25, dealerSplitScore = 25, playerSplitScore = 25; //global variables are referenced ONLY in main(), hitOrStand(), and finalScore() for ease of access; initialized to greater than 21 to score properly. If the score is greater than 21, the checker skips it as a viable score (so if it isn't used or is a bust, it skips)

int main()
{
	int dealer = 1, player = 2, dealerSplit = 3, playerSplit = 4, newScore; //assigns possible hand owners with number to indicate card ownership in deck[]
	int deck[52];
	string isSplitting; //checks if player wants to split
	vector<string> dealerCards, playerCards, dealerSplitCards, playerSplitCards; //all possible hands with owner identifications
	bool splitD = false, splitP = false; //flags for when each player decides to split

	srand(time(nullptr)); //seeds rand()
	initializeDeck(deck);

	dealerCards.push_back(dealCard(deck, dealer)); //deals first card to dealer
	dealerCards.push_back(dealCard(deck, dealer)); //deals second card to dealer
	if((dealerCards.at(0)[0] == dealerCards.at(1)[0]) && (dealerCards.at(0)[0] == 'A' || dealerCards.at(0)[0] == '8')) //if both cards in original cards (dealer vector) are ace or 8, split
	{
		splitD = true; //flag to choose correct console output
		dealerSplitCards.push_back(dealerCards.at(1)); //makes second card of original cards (dealer vector) the first card of new cards (dealerSplit vector)
		replaceDeckIndex(deck, dealerCards, dealerCards.at(1), dealer, dealerSplit); //changes the index value of the first card of new hand from dealer to dealerSplit
		dealerSplitCards.push_back(dealCard(deck, dealerSplit)); //deals new second card to new cards (dealerSplit vector)
		dealerCards.push_back(dealCard(deck, dealer)); //deals new second card to original cards (dealer vector)
		dealerHand = displayCards(dealerCards); //creates string of cards (dealer vector)
		dealerScore = scoreHand(deck, dealer); //scores dealer original hand
		dealerSplitHand = displayCards(dealerSplitCards); //creates string of cards (dealerSplit vector)
		dealerSplitScore = scoreHand(deck, dealerSplit); //scores dealer split hand
	}
	else
	{
		dealerHand = displayCards(dealerCards); //creates string of cards (dealer vector)
		dealerScore = scoreHand(deck, dealer); //scores dealer hand
	}
	
	playerCards.push_back(dealCard(deck, player)); //deals first card to player
	playerCards.push_back(dealCard(deck, player)); //deals second card to player
	playerHand = displayCards(playerCards); //creates string of cards (from player vector) for printing in 20 character field
	playerScore = scoreHand(deck, player); //scores player hand
		
	cout << "Dealer has cards     : " << setw(20) << left << dealerHand << '(' << dealerScore << ')' << '\n';
	if(splitD) //if dealer splits, print second hand as well
		cout << "Dealer also has cards: " << setw(20) << left << dealerSplitHand << '(' << dealerSplitScore << ')' << '\n';
	cout << "You have cards       : " << setw(20) << left << playerHand << '(' << playerScore << ')' << '\n';
	
	if(playerCards.at(0)[0] == playerCards.at(1)[0]) //if player is dealt cards of same number, i.e. AS and AC or 5C and 5H, etc., allows player to split hand
	{
		while(!(isSplitting == "yes" || isSplitting == "no")) //error checks, continuously asks player to input choice to split or not
		{
			cout << "Would you like to split? yes/no ";
			cin >> isSplitting;
		}	
		if(isSplitting == "yes") //mimics what happens when dealer splits, but for player cards (player vector and playerSplit vector)
		{
			splitP = true; //flag to choose correct console output
			playerSplitCards.push_back(playerCards.at(1)); //makes second card of original hand the first card of new hand
			replaceDeckIndex(deck, playerCards, playerCards.at(1), player, playerSplit); //changes the index value of the first card of new hand from dealer to dealerSplit
			playerSplitCards.push_back(dealCard(deck, playerSplit)); //deals new second card to new hand
			playerCards.push_back(dealCard(deck, player)); //deals new second card to original hand
			playerHand = displayCards(playerCards);
			playerScore = scoreHand(deck, player);
			playerSplitHand = displayCards(playerSplitCards);
			playerSplitScore = scoreHand(deck, playerSplit);
			cout << "\nDealer has cards     : " << setw(20) << left << dealerHand << '(' << dealerScore << ')' << '\n';
			if(splitD) //if dealer splits, print second hand as well
				cout << "Dealer also has cards: " << setw(20) << left << dealerSplitHand << '(' << dealerSplitScore << ')' << '\n';
			cout << "You have cards       : " << setw(20) << left << playerHand << '(' << playerScore << ')' << '\n';
			cout << "You also have cards  : " << setw(20) << left << playerSplitHand << '(' << playerSplitScore << ')' << '\n';
		}
	}
	newScore = checkAce(deck, player); //gives player option to count ace as 11 points or 1 point
	if(newScore != playerScore) //if player chooses to change score from default ace value (1) to 11, change score and print to screen before asking to hit or stand
	{
		playerScore = newScore;
		cout << "New score is (" << playerScore << ") and you are holding " << playerHand << '\n';
	}
	if(splitP) //does same as previous if statement but for player's second hand if it exists
	{
		newScore = 0;
		newScore = checkAce(deck, playerSplit);
		if(newScore != playerSplitScore)
		{
			playerSplitScore = newScore;
			cout << "New score for 2nd hand is (" << playerSplitScore << ") and you are holding " << playerSplitHand << '\n';
		}
	}
	
	if(blackjack(dealerScore, playerScore, dealerSplitScore, playerSplitScore) == 0) //checks for blackjacks
		return 0;
	
	hitOrStand(deck, player, playerScore, playerHand, playerCards, splitD, splitP); //asks player to hit or stand
	if(blackjack(dealerScore, playerScore, dealerSplitScore, playerSplitScore) == 0) //checks for blackjacks following player's turn
		return 0;
	
	if(playerScore > 21 && !splitP) //checks for busts following player's turn
	{	
		cout << "Player busts, dealer wins!\n";
		return 0;
	}
	else if(playerScore > 21 && splitP) //outputs correct message and allows player to continue playing if a split hand exists and the player busts their first hand 
		cout << "Player busts 1st hand!\n";
	if(splitP) //prompts player to now choose to hit or stand on their split hand if it exists
	{
		hitOrStand(deck, playerSplit, playerSplitScore, playerSplitHand, playerSplitCards, splitD, splitP);
		if(blackjack(dealerScore, playerScore, dealerSplitScore, playerSplitScore) == 0) //checks for blackjacks following player's second hand if it exists
			return 0;
	
		if(playerSplitScore > 21) //if player busts 2nd hand but not 1st, prints appropriate message and allows game to continue
			cout << "Player busts 2nd hand!\n";
		if(playerSplitScore > 21 && playerScore > 21) //if player has bust both hands, ends game
		{
			cout << "Player busts 2nd hand, dealer wins!\n";
			return 0;
		}
	}
	hitOrStand(deck, dealer, dealerScore, dealerHand, dealerCards, splitD, splitP); //gets dealer to hit or stand on his hand
	if(blackjack(dealerScore, playerScore, dealerSplitScore, playerSplitScore) == 0) //checks for blackjacks following dealer's first hand
		return 0;
	
	if(dealerScore > 21 && !splitD) //if dealer busts, player wins
	{
		cout << "Dealer busts, player wins!\n";
		return 0;
	}
	else if(dealerScore > 21 && splitD) //if dealer busts but has a split hand, allows them to continue playing
		cout << "Dealer busts 1st hand!\n";
	if(splitD) //prompts dealer to now to choose to hit or stand on their split hand if it exists
	{	
		hitOrStand(deck, dealerSplit, dealerSplitScore, dealerSplitHand, dealerSplitCards, splitD, splitP);
		if(blackjack(dealerScore, playerScore, dealerSplitScore, playerSplitScore) == 0) //checks for blackjacks following dealer's second hand if it exists
			return 0;

		if(dealerSplitScore > 21 && dealerScore > 21)
		{
			cout << "Dealer busts 2nd hand, player wins!\n";
			return 0;
		}
	}
	cout << "\nDealer has cards: " << setw(20) << left << dealerHand << '(' << dealerScore << ')' << '\n';
	if(splitD)
		cout << "Dealer also has cards: " << setw(20) << left << dealerSplitHand << '(' << dealerSplitScore << ')' << '\n';
	cout << "You have cards  : " << setw(20) << left << playerHand << '(' << playerScore << ')' << '\n';
	if(splitP)
		cout << "You also have cards: " << setw(20) << left << playerSplitHand << '(' << playerSplitScore << ')' << '\n';
	
	finalScore(dealerScore, playerScore, dealerSplitScore, playerSplitScore); //decides winner

	return 0;
}

int getRandom(int minimum, int maximum) //gets random number between min and max, inclusive
{
	int deckIndex = (rand() % (maximum - minimum + 1)) + minimum;
	return deckIndex;
}

string dealCard(int deck[], int player) //chooses an undealt card from a deck of 52 cards and assigns it to a player
{
	char suits[] = {'S', 'H', 'D', 'C'}; //0-12 is spades, 13-25 is hearts, 26-38 is diamonds, and 39-51 is clubs
	string  num[] = {"A","2","3","4","5","6","7","8","9","10","J","Q","K"};
	int index;
	while(true) //gets a random index between 0-51, inclusive, and checks if it already has a player assigned to it
	{
		index = getRandom(0, 51);
		if(deck[index] == 0) //if deck[index] != 0, then whatever number it is represents the player that currently holds it
		{	
			deck[index] = player;
			break; //will check for an undealt card forever until it finds one
		}
	}
	int cardNum = index % 13; //the index of the undealt card modulo 13 returns the index of the appropriate value from num[]; e.g. index = 43 is 5C, 43 % 13 = 4, num[4] = 5
	int suit = index / 13; //the index of the undealt card divided by 13 returns a truncated int index, the appropriate value from suits[]; e.g. 43 / 13 = 3, suits[3] = C
	string card = num[cardNum] + suits[suit]; //combines the values from num[] and suits[] into one string that the function can return
	return card;
}

int scoreHand(int deck[], int user) //scores a user's hand
{
	int card;
	int score = 0;
	bool ace = false; //flag for if ace is detected in dealer hand

	for(int i = 0; i < 52; i++) //goes through every index in the deck to find the cards that match the player's number
	{
		if(deck[i] == user)
		{
			card = (i % 13) + 1; //finds the face value at a given index; e.g. index = 43 is 5C worth 5 points, (43 % 13) + 1 = 5
			if(card < 10) //all cards with face value less than 10 add that face value to the total score
				score += card;
			else if(card >= 10)
				score += 10; //cards 10 and up (J = 11, Q = 12, K = 13) add 10 to the total score
			if(card == 1) //if card in hand is ace, ace = true
				ace = true;
		}
	}
	if(ace && (user == 1 || user == 3) && ((score + 10) == 21 || (score + 10) == 20)) //when scoring dealer's hand(s), makes ace worth 11 points when total will add up to 20 or 21
		score += 10;

	return score;
}

string displayCards(const vector<string> &userCards) //gets cards from hand vectors and puts them in one string to print in 20-character field
{
	string hand = userCards.at(0);
	for(int i = 1; i < userCards.size(); i++)
	{
		hand += (' ' + userCards.at(i));
	}
	
	return hand;
}

void replaceDeckIndex(int deck[], vector<string> &userCards, string cardName, int user, int newUser) //when splitting a hand, reassigns the second card from initial hand to new 'split' hand
{
	char suits[] = {'S', 'H', 'D', 'C'}; //0-12 is spades, 13-25 is hearts, 26-38 is diamonds, and 39-51 is clubs
	string  num[] = {"A","2","3","4","5","6","7","8","9","10","J","Q","K"};
	
	userCards.pop_back(); //removes second hard that was split into new hand from original hand (vector)
	
	for(int i = 0; i < 52; i++) //searches through entire deck for cards assigned to original hand
	{
		if(deck[i] == user)
		{
			int cardNum = i % 13; 
			int suit = i / 13;
			string card = num[cardNum] + suits[suit];
			if(card == cardName) //compares found card to the card to replace, if it's a match, it replaces the index (original player ID) with new one (split hand)
				deck[i] = newUser;
		}	
	}
}

void hitOrStand(int deck[], int user, int &userScore, string &userHand, vector<string> &userCards, bool splitD, bool splitP) //dealer = 1 dealerSplit = 3 player = 2 playerSplit = 4, function handles hitting and standing for both dealer and player
{
	string choice = "";
	int newUserScore; //used to compare player's score before and after checking for value of ace
	
	if(user == 2 || user == 4)
	{
		while(playerScore != 21 || playerSplitScore != 21)
		{
			if(user == 2 && !splitP)
				cout << "Hit or stand         ? "; //player's turn when not splitting
			if(user == 2 && splitP)
				cout << "Hit or stand for 1st hand? "; //player's first hand when splitting
			if(user == 4)
				cout << "Hit or stand for 2nd hand? "; //player's second hand when splitting
			if(user == 2 || user == 4)
			{	
				if(!(cin >> choice)) //error checking
					continue;
				else 
				{
					if(choice == "stand") //ends player's turn
					{
						cout << '\n';
						break;
					}
					else if(choice == "hit")
					{
						userCards.push_back(dealCard(deck, user)); //adds card to user's cards (vector), puts it in user's actual cards by reference
						userHand = displayCards(userCards); //creates string of all cards to print, puts it in user's actual hand by reference
						userScore = scoreHand(deck, user); //scores user's hand, puts it in user's actual score by reference
						cout << "\nDealer has cards: " << setw(20) << left << dealerHand << '(' << dealerScore << ')' << '\n'; //both split and original variations of player/dealerHand and player/dealerScore are global variables to avoid having to include them as another parameter in hitOrStand()
						if(splitD)
							cout << "Dealer also has cards: " << setw(20) << left << dealerSplitHand << '(' << dealerSplitScore << ')' << '\n';	
						cout << "You have cards  : " << setw(20) << left << playerHand << '(' << playerScore << ')' << '\n';
						if(splitP)
							cout << "You also have cards  : " << setw(20) << left << playerSplitHand << '(' << playerSplitScore << ')' << '\n';
						newUserScore = checkAce(deck, user); 
						if(newUserScore != userScore) //if player chooses to value ace at 11, prints new scores and hands to screen
						{
							userScore = newUserScore;
							cout << "\nDealer has cards: " << setw(20) << left << dealerHand << '(' << dealerScore << ')' << '\n';
							if(splitD)
								cout << "Dealer also has cards: " << setw(20) << left << dealerSplitHand << '(' << dealerSplitScore << ')' << '\n';	
							cout << "You have cards  : " << setw(20) << left << playerHand << '(' << playerScore << ')' << '\n';
							if(splitP)
								cout << "You also have cards  : " << setw(20) << left << playerSplitHand << '(' << playerSplitScore << ')' << '\n';
						}
					}
				}	
			}
			if(userScore >= 21)
				break;
		}
	}
	else if(user == 1 || user == 3) //when it's the dealer's turn, make it hit until its score goes over 17
	{
		while(userScore <= 17)
		{
			userCards.push_back(dealCard(deck, user));
			userHand = displayCards(userCards);
			userScore = scoreHand(deck, user);
			cout << "Dealer hits     : " << setw(20) << left << userHand << '(' << userScore << ')' << '\n';
		}
	}
}

int blackjack(int dealerScore, int playerScore, int dealerSplitScore, int playerSplitScore) //aside from first call after the very first round of dealing, function will always follow an instance of hitOrStand() and will always be contained in an if statement to either continue or stop main(), finds blackjacks and stops the game when they occur
{
	if((dealerScore == 21 || dealerSplitScore == 21) && (playerScore == 21 || playerSplitScore == 21)) //in the unlikely event that both the player and dealer split their hands and are both dealt blackjacks
	{
		cout << "Player and dealer draw.\n";
		return 0;
	}
	else if(dealerScore == 21 || dealerSplitScore == 21)
	{
		cout << "Dealer wins!\n";
		return 0;
	}
	else if(playerScore == 21 || playerSplitScore == 21)
	{
		cout << "Player wins!\n";
		return 0;
	}
	else
		return 1;
}

void initializeDeck(int deck[]) //initializes all elements in deck[] to 0
{
	for(int i = 0; i < 52; i++)
		deck[i] = 0;
}

int checkAce(int deck[], int user) //allows player to pick what value to use for an ace //could not include as part of scoreHand() due to timing issues
{
	int card;
	int score = 0;	
	int ace; //user input of 1 or 11 for ace
	bool foundAce = false; //flag for if ace appears, lets user choose value of 1 or 11

	for(int i = 0; i < 52; i++) //goes through every index in the deck to find the cards that match the player's number
	{
		if(deck[i] == user)
		{
			card = (i % 13) + 1; //finds the face value at a given index; e.g. index = 43 is 5C worth 5 points, (43 % 13) + 1 = 5
			if(card == 1)
				foundAce = true;
			if(card < 10) //all cards with face value less than 10 add that face value to the total score
				score += card;
			else if(card >= 10)
				score += 10; //cards 10 and up (J = 11, Q = 12, K = 13) add 10 to the total score
		}
	}	
	if(foundAce && (user == 2 || user == 4) && score + 10 <= 21)
	{
		while(true)
		{
			cout << "Would you like the ace to count as 1 or 11? ";
			if(!(cin >> ace) || (ace != 11 && ace != 1))
				continue;
			else if(ace == 11)
			{
				score += 10;
				break;
			}
			else if(ace == 1)
				break;
		}
	}		
	return score;
}
void finalScore(int dealerScore, int playerScore, int dealerSplitScore, int playerSplitScore)
{
	vector<int> scoresCheck{dealerScore, playerScore, dealerSplitScore, playerSplitScore};
	vector<int> scoresKeep{dealerScore, playerScore, dealerSplitScore, playerSplitScore};
	vector<int> winners;
	int winningScore;

	for(int i = 0; i < scoresCheck.size(); i++)
	{
		if(scoresCheck.at(i) > 21) //finds scores that are a bust or not used and removes it from the vector; the loops restarts to avoid skipping a value from resizing
		{
			scoresCheck.erase(scoresCheck.begin() + i);
			i = 0;
		}
	}
	
	winningScore = scoresCheck.at(0);

	for(int i = 0; i < scoresCheck.size(); i++) //loops through remaining scores and finds the largest ACTUAL score value such as 15, 17, 20, etc.
	{
		if(scoresCheck.at(i) > winningScore)
			winningScore = scoresCheck.at(i);
	}
	for(int i = 0; i < 4; i++) //compares winning score to all existent scores and adds winning player(s) to winners vector
	{
		if(scoresKeep.at(i) == winningScore)
			winners.push_back(i + 1);
	}
	
	if(winners.size() == 1) //if there is only one winner, print that winner //remember, dealer = 1, player = 2, dealerSplit = 3, and playerSplit = 4
	{
		if(winners.at(0) == 1 || winners.at(0) == 3)
			cout << "Dealer wins!\n";
		else
			cout << "Player wins!\n";
	}
	else if(winners.size() == 2) //if there are two winners, check if both winners are hands of the same person. If not, it must be a draw
	{
		if(winners.at(0) == 1 && winners.at(1) == 3)
			cout << "Dealer wins!\n";
		else if(winners.at(0) == 2 && winners.at(1) == 4)
			cout << "Player wins!\n";
		else
			cout << "Player and dealer draw.\n";
	}
	else if(winners.size() > 2) //if there are more than two winners, it must have been a draw since no person can have more than two scores each
		cout << "Player and dealer draw.\n";
}
