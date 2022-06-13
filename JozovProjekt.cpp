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
	string ime; 
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
			cout << "Koliko igraca(1 ili 2)" << endl; 
			int numPlayers; 
			cin >> numPlayers; 
			for (int i = 0; i < numPlayers; i++) { 
				cout << "Novi igrac(N) ili postojeci(E)?" << endl; 
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
			cout << "Unesite 1 ili 2" << endl; 
			menu = 0; 
			cin.ignore(); 
		}
	} while (menu != 2); 

	bool cond = false; 
	int size = players.size();
	do {
		for (int i = 1; i < players.size(); i++) {
			if (players[i].info.money < 5) {
				cout << players[i].info.username << " vise nema para. Zelite li dodati pare?, da(Y) ili ne (N)?" << endl; 
				cin >> input; 
				if (input == 'Y' || input == 'y') { 
					cout << "Koliko para bi ste htijeli dodati?" << endl; 
					cin >> players[i].info.money; 
				}
				else { 
					save(players[i]); 
					players.erase(players.begin() + (i)); 
					i--;

				}
			}

			if (cond && players.size() > 1) {
				cout << players[i].info.username << " Zelite li nastaviti igrati(P) ili odustati(Q)" << endl;
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
	cout << "The rules of blackjack are fairly simple and your only opponent in the game is the dealer. Each player is dealt a two cards face up while the dealer only has one card face up. The goal is to have your two cards added total to be higher than the dealer’s two cards and under 21. If you go over 21 you “bust”, or lose, or if the dealers two cards added together are higher than yours you also lose.	 If your two card total is equal to the dealers then it is a “stand-off” or a tie. Cards values are usually face value, 2 is 2 and 10 is 10, while all face cards, jack/queen/king, are also 10. The only exception to this rule are Aces, they can take on the value of 1 or 11. To get your two cards total you simply add your two cards together. If you have any combination of an Ace or any card that is 10 points then you have what is called blackjack, 21 pts. Getting blackjack means you get paid more if you win. With all of that being said if you’re not satisfied with your two card total then you can take extra cards, called taking a hit, and for each hit you get you get one more card. The dealer does this as well but has a strict set of rules to follow on whether or not to take a hit. If a dealer’s hand is less than or equal to 16 or a “soft” 17, meaning they have an ace and a 6, they must take a hit. If it’s 17 or higher the dealer must stand. So now that we know the dealer rules there are a few options for users. As I stated earlier you can take hits to increase your card total. You may also split your cards and double down. Splitting can be done when your first two cards are of equal value and can only be split from the original hand, split hands cannot be split, the bet has to be equal or greater than the original bet for each hand. For each time you split you will receive and additional card for that hand and then you play like regular blackjack. Users may also double down which consists of a user placing another bet of equal or lesser value when their first two cards total is equal to 9, 10, or 11 without aces. After doubling down you will only get one additional card. Some of you may have realized that if the dealer gets a blackjack or 21 you pretty much always lose, unless you yourself have blackjack. There is a way around this and it’s called insurance. If the dealer is dealt an ace face up then the dealer will ask if you want to take out insurance, equal to half of your original bet, to insure your hand if the dealer has blackjack and only when he has blackjack and helps insure you don’t lose money if he does have blackjack, insurance pays 2 to 1 so your insurance bet will cover the loss of your hand if you bet half." << endl; //displays the rules
}

void displayMenu() {
	cout << "Dobrodosli u blackjack!" << endl; 
	cout << "Odaberite jednu od opcija" << endl;
	cout << "1) Pravila" << endl; 
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
		cout << "Pare: " << players[i].info.money << setw(10) << "Ulozene pare: " << players[1].bet << endl; 
	}
	for (int i = 0; i < (players.size() * 2); i++) {
		players[(i % players.size())].hand.push_back(deal());
		if ((i % players.size()) == 0 && (i % 2) == 0) { 
			players[(i % players.size())].hand[(i % 2)].up = false; 
		}
	}

	for (int i = 1; i < players.size(); i++) {
		cout << players[i].info.username << " ima: " << score(players[i].hand) << setw(10) << endl;
	}

	for (int i = 0; i < players.size(); i++) {
		cout << players[i].info.username << " karte:" << endl;
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
					cout << players[i].info.username << " zbroj karti: " << score(players[i].hand) << endl; 
					cout << "Zelite li udvostručiti(D), podjeli(L),hit(H), ili stand(S)?" << endl; 
				}
				else if (((players[i].hand[0].value >= 10 && players[i].hand[1].value >= 10) || (players[i].hand[0].value == players[i].hand[1].value)) && players[i].hand.size() == 2) { 
					cout << players[i].info.username << "zbroj karti: " << score(players[i].hand) << endl; 
					cout << "Zelite li podjeliti(L) vase karte, hit(H), ili stand(S)?" << endl; 
				}
				else if (players[i].hand.size() == 2 && score(players[i].hand) >= 9 && score(players[i].hand) <= 11 && !(hasAce(players[i].hand))) { 
					cout << players[i].info.username << " zbroj karti: " << score(players[i].hand) << endl; 
					cout << "Zelite li udvostručiti(D), hit(H), ili stand(S)?" << endl; 
				}
				else { 
					cout << players[i].info.username << " zbroj karti: " << score(players[i].hand) << endl; 
					cout << "Hit(H) ili Stand(S)?"; 
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
	}

	dealer_play(players[0]); 

	players[0].hand[0].up = true; 

	for (int i = 0; i < players.size(); i++) {
		cout << players[i].info.username << " zborj karti: " << score(players[i].hand) << " karte: ";
		printCards(players[i].hand);
	}

	for (int i = 1; i < players.size(); i++) {
		if (score(players[i].hand) > 21) { 
			cout << "Presli ste 21! ";
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
	cout << "Vase korisnicko ime?" << endl;
	fstream input; 
	string filename; 
	do {
		cin >> user.username; 
		filename = user.username + ".dat"; 
		input.open(filename); 
		if (!input.fail()) { 
			cout << "Ovo ime je zauzeto. Izaberite neko drugo." << endl; 
		}
	} while (!input.fail()); 

	cout << "Unesite lozinku." << endl; 
	cin.ignore();
	getline(cin, user.password); 
	cin.ignore(); 

	cout << "Vase ime?." << endl; 
	getline(cin, user.ime); 

	cout << "Ponovo unesite lozinku." << endl; 
	cin >> user.email; 

	cout << "Koliko para imate?" << endl;
	cin >> user.money; 

	user.total_played = 0; 
	user.wins = 0; 
	user.win_percentage = 0; 

	input.close(); 
	return user; 
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
	output << user.info.username << endl << user.info.password << endl << user.info.ime << endl << user.info.email << endl << user.info.money << endl << user.info.total_played << endl << user.info.wins << endl << setprecision(3) << percent; //writes the data to the file

	output.close();
}

account load() {
	string username, password; 
	string filename; 
	account user; 
	ifstream input; 

	do {
		cout << "Koje je vase korisnicko ime?" << endl; 
		cin >> username; 
		filename = username + ".dat"; 
		input.open(filename); 
	} while (input.fail()); 

	getline(input, user.username); 
	getline(input, user.password); 

	do {
		cout << "Koja je vasa lozinka" << endl; 
		cin >> password; 
	} while (!(password == user.password));

	getline(input, user.ime);
	getline(input, user.email);
	input >> user.money;
	input >> user.total_played;
	input >> user.wins;
	input >> user.win_percentage;

	input.close();
	return user; 
}

void insurance(vector<player>& players) {
	int size = players.size(); 

	for (int i = 1; i < size; i++) {
		int bet;
		players[i].insurance_bet = 0;
		if ((players[i].info.money - (players[i].bet / 2)) >= 0) { 
			do {
				cin.ignore();
				cout << "How much would you like to bet for insurance, up to " << (players[i].bet / 2) << players[i].info.username << "?(0 if you don't want to take out insurance)" << endl;
				cin >> bet; 
			} while (!(bet <= (players[i].bet / 2) && bet >= 0)); 

			if (bet != 0) { 
				players[i].insurance_bet = bet;
				players[i].info.money -= bet; 
			}
		}
	}

	if (score(players[0].hand) == 21) { 
		for (int i = 1; i < size; i++) {
			if (players[i].insurance_bet != 0) { 
				players[i].info.money += (players[i].insurance_bet * 2); 
				players[i].insurance_bet = 0; 
			}
		}
	}
	else {
		for (int i = 1; i < size; i++) {
			if (players[i].insurance_bet != 0) { 
				players[i].insurance_bet = 0; 
			}
		}
	}
}

void bet(player& user) {
	int bet;
	do {
		cout << user.info.username << endl; 
		cout << "Koliko zelite uloziti? (Mora biti vise od " << MIN_BET << " ili manje ili jednako " << user.info.money << ")" << endl; 
		cin >> bet; 
	} while (!(bet >= MIN_BET && bet <= user.info.money));
	user.info.money -= bet; 
	user.bet = bet; 
}
void split(player& dealer, player& user) {
	player split;
	vector<player> players; 
	players.push_back(dealer);

	split.bet = user.bet; 
	user.info.money -= user.bet;
	split.hand.push_back(user.hand[0]); 
	split.hand.push_back(deal()); 
	user.hand[0] = deal();
	split.info.username = user.info.username;
	players.push_back(split); 

	printCards(players[1].hand); 

	char input; 
	do {
		for (int i = 1; i < players.size(); i++) {
			if (score(players[i].hand) > 21) { 
				input = 'S'; 
			}
			else {
				cout << "Hit(H) ili Stand(S):"; 
				cin >> input; 
			}
			if (input == 'H' || input == 'h') {
				players[i].hand.push_back(hitMe());
				printCards(players[i].hand);
				cout << players[i].info.username << "Zbroj karata je sada " << score(players[i].hand) << endl;
			}
		}
	} while (!(input == 'S' || input == 's')); 

	dealer_play(players[0]); 

	for (int i = 1; i < players.size(); i++) {
		cout << players[i].info.username << " zbroj karata: " << score(players[i].hand) << " karte:" << endl;
		printCards(players[i].hand);
	}

	for (int i = 1; i < players.size(); i++) {
		if (score(players[i].hand) > 21) {
			cout << "Presli ste 21!"; 
		}
		payout(players[0], players[i]); 
	}
}

void payout(player dealer, player& user) {
	if (winner(dealer, user) == 1) { 
		if (score(user.hand) == 21 && hasAce(user.hand) && user.hand.size() == 2) { 
			user.info.money += ((user.bet * 3) / 2); 
			user.bet = 0; 
			cout << user.info.username << " Pobjeda!" << endl;
		}
		else {
			user.info.money += (user.bet * 2); 
			user.bet = 0; 
			cout << user.info.username << " Pobjeda!" << endl;
		}
	}
	else if (winner(dealer, user) == 0) { 
		user.info.money += user.bet; 
		user.bet = 0;
		cout << user.info.username << " Izjednaceno!" << endl;
	}
	else { 
		user.bet = 0;  
		cout << user.info.username << " Izgubili ste!" << endl;
	}
}
int winner(player dealer, player& user) {
	if (score(dealer.hand) == score(user.hand)) { 
		return 0;
	}
	else if (((score(dealer.hand) < score(user.hand)) && (score(user.hand) <= 21)) || (score(dealer.hand) > 21 && score(user.hand) <= 21)) { 
		return 1;
	}
	else { 
		return -1;
	}
}
void doubleDown(player dealer, player& user) {
	int bet; 
	do {
		cout << "Koliko zelite uloziti? (MOra biti vise od " << MIN_BET << " ili manje ili jednako od " << user.info.money << ")" << endl; 
		cin >> bet; 
	} while (!(bet > MIN_BET && bet <= user.bet && bet <= user.info.money)); 

	user.bet += bet; 
	user.info.money -= bet; 
	user.hand.push_back(deal()); 
	payout(dealer, user); 	
}
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


void printCards(vector<card> hand) {
	const string CARD_VALUES[14] = { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "X" }; 

	for (int i = 0; i < hand.size(); i++) {
	
		if (hand[i].up) { 
			cout << CARD_VALUES[(hand[i].value - 1)] << printSuit(hand[i]) << " ";
		}
		else { 
			cout << CARD_VALUES[13] << CARD_VALUES[13] << " ";
		}
	}
	cout << endl;
}

bool hasAce(vector<card> hand) {
	bool has_ace = false; 
	for (int i = 0; i < hand.size(); i++) {
		if (hand[i].value == 1) { 
			has_ace = true; 
		}
	}

	return has_ace;
}
int score(vector<card> hand) {
	int total = 0; 	
	for (int i = 0; i < hand.size(); i++) {
		if (hand[i].value >= 10) { 
			total += 10; 
		}
		else {
			total += hand[i].value; 
		}
	}

	if (hasAce(hand) && total <= 11) { 
		total += 10; 
	}

	return total; 
}

card hitMe() {
	return deal(); 
}

card deal() {
	card new_card; 

	new_card.value = 1 + rand() % 13; 
	int suit = rand() % 4;
	switch (suit) { 
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

	new_card.up = true; 

	return new_card; 
}
