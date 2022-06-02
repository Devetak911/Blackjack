#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
using namespace std;


enum card_suit {
	S, H, D, C
};


int MIN_BET = 5; 


struct account { 
	string name; 
	string email; 
	string username;
	string password; 

	int money; 


	int wins; 
	int total_played; 
	double win_percentage; 
};

struct card { 
	int value; 
	card_suit suit; 
	bool up; 
};

struct player { 
	account info; 
	vector<card> hand;
	int bet; 
	int insurance_bet; 
};


card deal();
card hitMe();
int score(vector<card>);
bool hasAce(vector<card>);
void printCards(vector<card>);
char printSuit(card);
void doubleDown(player, player&);
int winner(player, player&);
void payout(player, player&);
void split(player&, player&);
void bet(player&);
void insurance(vector<player>&);
account load();
void save(player);
account create();
void play(vector<player>&);
void clear(vector<card>&);
void dealer_play(player&);
void displayMenu();
void displayRules();

int main() {
	unsigned int time_seed = time(0); 
	srand(time_seed); 
	player user; 
	player dealer; 
	vector<player> players; 
	char input; 

	dealer.info.username = "Dealer"; 
	players.push_back(dealer); 

	int menu; 

	do {
		displayMenu(); 
		cin >> menu; 

		if (menu == 1) { 
			displayRules(); 
		}
		else if (menu == 2) { 
			cout << "How many players(1 or 2)" << endl; 
			int numPlayers; 
			cin >> numPlayers; 
			for (int i = 0; i < numPlayers; i++) { 
				cout << "Are you a new player(N) or existing player(E)?" << endl; 
				cin >> input; 

				if (input == 'N' || input == 'n') { 
					user.info = create(); 
					save(user); 
					players.push_back(user); 
				}
				else if (input == 'E' || input == 'e') { 
					user.info = load(); 
					players.push_back(user);
				}

			}
		}
		else { 
			cout << "Please enter 1 or 2" << endl; 
			menu = 0; 
			cin.ignore(); 
		}
	} while (menu != 2); 

	bool cond = false; 
	int size = players.size();
	do {
		for (int i = 1; i < players.size(); i++) {
			if (players[i].info.money < 5) { 
				cout << players[i].info.username << " is currently out of money. Would you like to add some, yes(Y) or (N)?" << endl; 
				cin >> input; 
				if (input == 'Y' || input == 'y') { 
					cout << "How much would you like to add?" << endl; 
					cin >> players[i].info.money; 
				}
				else { 
					save(players[i]); 
					players.erase(players.begin() + (i)); 
					i--;

				}
			}

			if (cond && players.size() > 1) {
				cout << players[i].info.username << " Would you like to keep playing(P) or quit(Q)" << endl;
				cin >> input; 
				if (input == 'q' || input == 'Q') { 
					save(players[i]); 
					players.erase(players.begin() + (i)); 
					i--;
				}
			}
		}
		play(players); 
		cond = true; 
	} while (players.size() > 1);
	

	return 0;
}

	

void displayRules() {
	cout << "The rules of blackjack are fairly simple and your only opponent in the game is the dealer. Each player is dealt a two cards face up while the dealer only has one card face up. The goal is to have your two cards added total to be higher than the dealer’s two cards and under 21. If you go over 21 you “bust”, or lose, or if the dealers two cards added together are higher than yours you also lose.	 If your two card total is equal to the dealers then it is a “stand-off” or a tie. Cards values are usually face value, 2 is 2 and 10 is 10, while all face cards, jack/queen/king, are also 10. The only exception to this rule are Aces, they can take on the value of 1 or 11. To get your two cards total you simply add your two cards together. If you have any combination of an Ace or any card that is 10 points then you have what is called blackjack, 21 pts. Getting blackjack means you get paid more if you win. With all of that being said if you’re not satisfied with your two card total then you can take extra cards, called taking a hit, and for each hit you get you get one more card. The dealer does this as well but has a strict set of rules to follow on whether or not to take a hit. If a dealer’s hand is less than or equal to 16 or a “soft” 17, meaning they have an ace and a 6, they must take a hit. If it’s 17 or higher the dealer must stand.  So now that we know the dealer rules there are a few options for users.  As I stated earlier you can take hits to increase your card total. You may also split your cards and double down. Splitting can be done when your first two cards are of equal value and can only be split from the original hand, split hands cannot be split, the bet has to be equal or greater than the original bet for each hand. For each time you split you will receive and additional card for that hand and then you play like regular blackjack.  Users may also double down which consists of a user placing another bet of equal or lesser value when their first two cards total is equal to 9, 10, or 11 without aces. After doubling down you will only get one additional card. Some of you may have realized that if the dealer gets a blackjack or 21 you pretty much always lose, unless you yourself have blackjack. There is a way around this and it’s called insurance. If the dealer is dealt an ace face up then the dealer will ask if you want to take out insurance, equal to half of your original bet, to insure your hand if the dealer has blackjack and only when he has blackjack and helps insure you don’t lose money if he does have blackjack, insurance pays 2 to 1 so your insurance bet will cover the loss of your hand if you bet half." << endl; //displays the rules
}

void displayMenu() {
	cout << "Welcome to the game of Blackjack!" << endl; 
	cout << "Please select an option below." << endl; 
	cout << "1) Rules" << endl; 
	cout << "2) Play" << endl; 
}
void dealer_play(player& dealer) {
	if ((score(dealer.hand) < 17) || (score(dealer.hand) == 17 && hasAce(dealer.hand))) { 
		dealer.hand.push_back(hitMe()); 
	}
}

void play(vector<player>& players) {
	char input; 

	for (int i = 1; i < players.size(); i++) { 
		bet(players[i]); 
		cout << "Money: " << players[i].info.money << setw(10) << "Bet: " << players[1].bet << endl; 
	}


	for (int i = 0; i < (players.size() * 2); i++) {
		players[(i % players.size())].hand.push_back(deal());
		if ((i % players.size()) == 0 && (i % 2) == 0) {
			players[(i % players.size())].hand[(i % 2)].up = false; 
		}
	}

	for (int i = 1; i < players.size(); i++) {
		cout << players[i].info.username << " has: " << score(players[i].hand) << setw(10) << endl;
	}

	for (int i = 0; i < players.size(); i++) {
		cout << players[i].info.username << " Cards:" << endl;
		printCards(players[i].hand);
	}

	bool cont = true; 
	for (int i = 1; i < players.size(); i++) { 
		do {
			if (players[0].hand[1].value == 1 && cont) { 
				insurance(players); 
				if (score(players[0].hand) == 21) { 
					players[0].hand[0].up = true; 

					printCards(players[0].hand); 

					for (int i = 1; i < players.size(); i++) {
						payout(players[0], players[i]);
					}
					input = 'S'; 
				}
				cont = false; 
			}
			if (players[0].hand[1].value >= 10 && cont) { 
				if (score(players[0].hand) == 21) {
					players[0].hand[0].up = true;

					printCards(players[0].hand); 
					for (int i = 1; i < players.size(); i++) {
						payout(players[0], players[i]);
					}
					input = 'S'; 				
				}
				cont = false; 
			}
			if (score(players[0].hand) <= 21) {
				if (((players[i].hand[0].value >= 10 && players[i].hand[1].value >= 10) || players[i].hand[0].value == players[i].hand[1].value) && players[i].hand.size() == 2 && score(players[i].hand) == 10) {
					cout << players[i].info.username << " score: " << score(players[i].hand) << endl; 
					cout << "Would you like to Double Down(D), split(L),take a hit(H), or stay(S), default is to take a stay?" << endl; 
				}
				else if (((players[i].hand[0].value >= 10 && players[i].hand[1].value >= 10) || (players[i].hand[0].value == players[i].hand[1].value)) && players[i].hand.size() == 2) { 
					cout << players[i].info.username << " score: " << score(players[i].hand) << endl; 
					cout << "Would you like to split(L) your cards, take a hit(H), or stay(S), default is to take a stay?" << endl; 
				}
				else if (players[i].hand.size() == 2 && score(players[i].hand) >= 9 && score(players[i].hand) <= 11 && !(hasAce(players[i].hand))) { 
					cout << players[i].info.username << " score: " << score(players[i].hand) << endl;
					cout << "Would you like to Double Down(D), take a hit(H), or stay(S), default is to take a stay?" << endl; 
				}
				else { 
					cout << players[i].info.username << " score: " << score(players[i].hand) << endl; 
					cout << "Hit(H) or Stay(S), default is to take a stay?"; 
				}
				cin >> input; 
				switch (input) { 
				case 'L': 
					split(players[0], players[i]);
					printCards(players[i].hand); 
					break;
				case 'D':
					doubleDown(players[0], players[i]); 
					input = 'S'; 
					break;
				case 'H':
					players[i].hand.push_back(hitMe());
					printCards(players[i].hand); 
					cout << players[i].info.username << " score is now " << score(players[i].hand) << endl; 
					break;
				default: 
					input = 's'; 
				}
				if (score(players[i].hand) > 21) { 
					input = 'S'; 
				}
			}
		} while (!(input == 'S' || input == 's')); 

	dealer_play(players[0]); 

	players[0].hand[0].up = true; 
	for (int i = 0; i < players.size(); i++) {
		cout << players[i].info.username << " score: " << score(players[i].hand) << " Cards: ";
		printCards(players[i].hand);
	}

	for (int i = 1; i < players.size(); i++) {
		if (score(players[i].hand) > 21) { 
			cout << "You busted! ";
		}
		int win = winner(players[0], players[i]); 
		if (win == 1) {
			players[i].info.wins += 1; 
		}
		payout(players[0], players[i]); 
		clear(players[i].hand); 
		players[i].info.total_played += 1; 	
	}

	clear(players[0].hand); 
}
void clear(vector<card>& hand) {
	hand.clear();
}

account create() {
	account user;
	cout << "What is the username you'd like to use?" << endl;
	fstream input; 
	string filename; 
	do {
		cin >> user.username; 
		filename = user.username + ".dat"; 
		input.open(filename);
		if (!input.fail()) { 
			cout << "This username is already taken. Please choose another one." << endl; 
		}
	} while (!input.fail());

	cout << "Please enter a password." << endl; 
	cin.ignore(); 
	getline(cin, user.password); 
	cin.ignore(); 

	cout << "Please enter how much money you'd like to deposit" << endl; 
	cin >> user.money; 

¸¸	
}
void save(player user) {
	ofstream output; 
	string filename = user.info.username + ".dat";

	output.open(filename, ios::out); 

	if (!output) {
		cerr << "Output file will not open and data could not be saved. " << endl;
	}

	double percent = 0;
	if (user.info.total_played == 0) {
		percent = 0; 
	}
	else {
		percent = ((double)user.info.wins / user.info.total_played) * 100; 
	}
	output << user.info.username << endl << user.info.password << endl << user.info.name << endl << user.info.money << endl << user.info.total_played << endl << user.info.wins << endl << setprecision(3) << percent; //writes the data to the file

	output.close(); 
}


account load() {
	string username, password; 
	string filename; 
	account user; 
	ifstream input;  

	do {
		cout << "What is your username?" << endl; 
		cin >> username; 
		filename = username + ".dat"; 
		input.open(filename); 
	} while (input.fail()); 

	getline(input, user.username); 
	getline(input, user.password); 

	do {
		cout << "What is your password?" << endl; //asks for the password
		cin >> password; //takes it in
	} while (!(password == user.password)); //checks password and keeps checking until the user gets it right

	/**
		Takes in all the user info
	*/
	getline(input, user.name);
	input >> user.money;
	input >> user.total_played;
	input >> user.wins;
	input >> user.win_percentage;

	input.close(); //close out the file
	return user; //returns the account info
}
/**
	if the dealer is displaying an Ace it asks all the players if they want to take out insurance
*/
void insurance(vector<player>& players) {
	int size = players.size(); //gets the size of the vector

	for (int i = 1; i < size; i++) {
		int bet;
		players[i].insurance_bet = 0;
		if ((players[i].info.money - (players[i].bet / 2)) >= 0) { //can the player even make an insurance bet
			do {
				cin.ignore();
				cout << "How much would you like to bet for insurance, up to " << (players[i].bet / 2) << players[i].info.username << "?(0 if you don't want to take out insurance)" << endl; //asks if they want to make an insurance bet
				cin >> bet; //takes it in
			} while (!(bet <= (players[i].bet / 2) && bet >= 0)); //continues to do this until they input the right stuff

			if (bet != 0) { //did they make a insurance bet
				players[i].insurance_bet = bet; //we have the insurance bet set
				players[i].info.money -= bet; //we take it out of their money pile
			}
		}
	}

	if (score(players[0].hand) == 21) { //the dealer has 21
		for (int i = 1; i < size; i++) {
			if (players[i].insurance_bet != 0) { //if they took out insurance
				players[i].info.money += (players[i].insurance_bet * 2); //pays out 2 to 1
				players[i].insurance_bet = 0; //clears out the insurance bet
			}
		}
	}
	else {
		for (int i = 1; i < size; i++) {
			if (players[i].insurance_bet != 0) { //if they took out insurance
				players[i].insurance_bet = 0; //clears out the insurance bet
			}
		}
	}
}

/**
   Asks the user what they would like to bet and returns it
*/
void bet(player& user) {
	int bet;
	do {
		cout << user.info.username << endl; //so we know what player is betting
		cout << "How much would you like to bet? (Must be greater than the " << MIN_BET << " and less than " << user.info.money << ")" << endl; //we tell them what they can bet
		cin >> bet; //takes in their bet
	} while (!(bet >= MIN_BET && bet <= user.info.money)); //repeat until they get it right
	user.info.money -= bet; //subtract the bet from their money stock pile
	user.bet = bet; //set their bet
}
/**
	If the user chooses to split their cards
	The split hand is completely played here
*/
void split(player& dealer, player& user) {
	player split;
	vector<player> players; //creates a new player for the split
	players.push_back(dealer);

	split.bet = user.bet; // takes the extra bet
	user.info.money -= user.bet; //takes out the bet from the user money pile
	split.hand.push_back(user.hand[0]); //takes the first card from the user and gives it to the split player
	split.hand.push_back(deal()); //gives the split person a new card
	user.hand[0] = deal(); //gives the user a new card
	split.info.username = user.info.username;
	players.push_back(split); //pushes the split player onto the vect

	printCards(players[1].hand); //prints out the new cards

	char input; //what the input is
	do {
		for (int i = 1; i < players.size(); i++) {
			if (score(players[i].hand) > 21) { //if they bust
				input = 'S'; //they are done
			}
			else {
				cout << "Hit(H) or Stay(S):"; //otherwise we ask them if they want to take a hit
				cin >> input; //take in their input
			}
			/**
				If they take a hit the below code gives them a new card, prints out their new cards and their new score
			*/
			if (input == 'H' || input == 'h') {
				players[i].hand.push_back(hitMe());
				printCards(players[i].hand);
				cout << players[i].info.username << " score is now " << score(players[i].hand) << endl;
			}
		}
	} while (!(input == 'S' || input == 's')); //we repeat this until they stop taking hits or bust

	dealer_play(players[0]); //dealer plays now since we need to take care of this now rather than later

	/**
		prints the split players cards and score again
	*/
	for (int i = 1; i < players.size(); i++) {
		cout << players[i].info.username << " score: " << score(players[i].hand) << " Cards:" << endl;
		printCards(players[i].hand);
	}

	/**
		payouts the split player
	*/
	for (int i = 1; i < players.size(); i++) {
		if (score(players[i].hand) > 21) {
			cout << "You busted!"; //tells them they busted
		}
		payout(players[0], players[i]); //plays out the player
	}
}
/**
	Typical pay out rules. If the player wins he gets 2 to 1 odds.
*/
void payout(player dealer, player& user) {
	if (winner(dealer, user) == 1) { //if the player won
		if (score(user.hand) == 21 && hasAce(user.hand) && user.hand.size() == 2) { //if the player has blackjack it's a 3:2 payout
			user.info.money += ((user.bet * 3) / 2); //I think this is 3:2 odds
			user.bet = 0; //clears out the bet
			cout << user.info.username << " won!" << endl;
		}
		else {
			user.info.money += (user.bet * 2); //adds the bet to the players stash of cash
			user.bet = 0; //clears out the bet
			cout << user.info.username << " won!" << endl;
		}
	}
	else if (winner(dealer, user) == 0) { //they tied
		user.info.money += user.bet; //players money goes back to his pile
		user.bet = 0; //clears out the bet
		cout << user.info.username << " tied!" << endl;
	}
	else { //the dealer won.
		user.bet = 0; //player didn't win so all we need to do is clear out. 
		cout << user.info.username << " lost!" << endl;
	}
}
/**
	Figures out if the dealer or the user won
	returns 1 if the player won
	returns -1 if the dealer won
	returns 0 if they tie
*/
int winner(player dealer, player& user) {
	if (score(dealer.hand) == score(user.hand)) { //they had a stand off
		return 0;
	}
	else if (((score(dealer.hand) < score(user.hand)) && (score(user.hand) <= 21)) || (score(dealer.hand) > 21 && score(user.hand) <= 21)) { //user won
		return 1;
	}
	else { //dealer typically wins
		return -1;
	}
}
/**
	Double down
	If the user chooses to double down we take in the new amount betted, deal one more card, and figure out if the user won
	takes in the user and dealer players and returns nothing
*/
void doubleDown(player dealer, player& user) {
	int bet; //so we can store the new bet
	do {
		cout << "How much would you like to bet? (Must be greater than the " << MIN_BET << " and less than " << user.info.money << ")" << endl; //asks how much they want to bet
		cin >> bet; //takes it in
	} while (!(bet > MIN_BET && bet <= user.bet && bet <= user.info.money)); //we repeat this until they get it right

	user.bet += bet; //add the new bet to the original
	user.info.money -= bet; //takes out their double down bet from the money
	user.hand.push_back(deal()); //gives the user one more card
	payout(dealer, user); //pays out the player based on if he/she won		
}
/**
Returns the Character version of the suit
Fairly self-explanatory...takes in a number between 0-4 and returns the suit based off that number
*/
char printSuit(card new_card) {
	switch (new_card.suit) {
	case 0:
		return 'S';
	case 1:
		return 'H';
	case 2:
		return 'D';
	case 3:
		return 'C';
	}
}

/**
Prints the cards to the screen
*/
void printCards(vector<card> hand) {
	const string CARD_VALUES[14] = { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "X" }; //makes it easier to print

	for (int i = 0; i < hand.size(); i++) {
		/**cout << "----------" <<  endl
			<< "|" << ((hand[i].up)?(hand[i].value):('X')) << ((hand[i].up)?((printSuit(hand[i]))):('X')) << setw(6) << "|" << endl
				<< "|" << setw(8) << "|" << endl
				<< "|" << setw(8) << "|" << endl
				<< "|" << setw(8) << "|" << endl
				<< "|" << setw(6) << ((hand[i].up)?(hand[i].value):('X')) <<  ((hand[i].up)?(printSuit(hand[i])):('X')) << "|" << endl
				<< "----------" << endl;*/
		if (hand[i].up) { //if the hand is face up we print this
			cout << CARD_VALUES[(hand[i].value - 1)] << printSuit(hand[i]) << " ";
		}
		else { //if it's face down we print XX
			cout << CARD_VALUES[13] << CARD_VALUES[13] << " ";
		}
	}
	cout << endl;
}
/**
	Lets us know if the hand has an ace
*/
bool hasAce(vector<card> hand) {
	bool has_ace = false; //For now we say there is no ace in the hand
	for (int i = 0; i < hand.size(); i++) {
		if (hand[i].value == 1) { //we have an ace
			has_ace = true; //so we set this to true
		}
	}

	return has_ace;
}
/**
	Gets the score for the user
	We treat Aces initially as 1 and then later check to see if the hand contains an Ace
*/
int score(vector<card> hand) {
	int total = 0; //setting up the total value
	for (int i = 0; i < hand.size(); i++) {
		if (hand[i].value >= 10) { //if it's 10, J, Q, or K
			total += 10; //adds 10 to the toal
		}
		else {
			total += hand[i].value; //adds the value to the total 
		}
	}

	if (hasAce(hand) && total <= 11) { //if the hand has an ace and we won't bust
		total += 10; //add 10
	}

	return total; //return the total
}
//gets a new card for the player
card hitMe() {
	return deal(); //add another card to the players hand
}
/**
	takes in nothing and returns a card
	makes a new card and assigns it a random value between 1-13
	assigns a suit to the card as well
	returns the card
*/
card deal() {
	card new_card; //card we will be returning

	new_card.value = 1 + rand() % 13; //makes sure the random number is between 1 and 13 for the value
	int suit = rand() % 4;
	switch (suit) { //makes sure the random number is between 1 and 4 for the suit
	case 0:
		new_card.suit = S;
		break;
	case 1:
		new_card.suit = H;
		break;
	case 2:
		new_card.suit = D;
		break;
	case 3:
		new_card.suit = C;
		break;
	}

	new_card.up = true; //we'll change it later if it's the dealers face down card

	return new_card; //returning the card
}
