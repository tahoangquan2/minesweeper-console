#include <iostream>
#include <conio.h>
#include <windows.h>
#include <string>
#include <time.h>
#include <stdlib.h>



// system variables

HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

short valid_ascii[6] = {13, 27, 28, 100, 102, 224};
short cur_x, cur_y, game_x, game_y;

char table[65][25];
bool visible[65][25];
short col, row;
short mines;



// system functions

// set the console cursor to a specific position in library windows.h
void at(short x, short y) {
	COORD coord_console = {x, y};
	SetConsoleCursorPosition(h_console, coord_console);
}


// return ascii of the key stroke
short input() {
	short ascii = getch();
	bool valid = false;

	for (short i = 0; i < 6; ++i) {
		if (ascii == valid_ascii[i]) {
			valid = true;
			break;
		}
	}

	if (valid == false) return 0;

	// esc
	if (ascii == 27) {
		at(0, 28);
		exit(0);
	}

	// read arrow
	if (ascii == 224) ascii = getch();

	return ascii;
}


// print characters at a position with color
void print(std::string message, short pos_x, short pos_y, short color = 7) {
	at(pos_x, pos_y);
	SetConsoleTextAttribute(h_console, color);
	std::cout << message;
	SetConsoleTextAttribute(h_console, 7);
	at(119, 0);
}


// random number in a range [l, r]
short random(short l, short r) {
	return l + rand() % (r - l + 1);
}



// game functions

void setup_menu();
void print_menu();
void keystroke_menu();

void print_game(short x, short y, bool e, bool c);
void play();
void setup_game();
void generate();
std::pair<std::string, short> display(std::string s, bool e, bool c, bool v);
void print_game();
void keystroke_game();


int main() {
	while (true) {
		setup_menu();
		print_menu();
	}
	std::string credit = "made by tahoangquan2";
	return 0;
}



// reset all variables in menu
void setup_menu() {
	cur_x = 5;
	cur_y = 8;
}


// menu: minesweeper + mode + save
void print_menu() {

	print("Beginner", 7, 8, 10);
	print("Intermediate", 7, 10, 11);
	print("Expert", 7, 12, 12);
	print("Custom", 7, 14, 13);

	print("<up/down arrow> to change the selection", 80, 8, 15);
	print("<enter> to confirm the selection", 80, 10, 15);
	print("<esc> to exit", 80, 12, 15);

	print(">", 5, 8, 15);

	while (true) {
		keystroke_menu();
	}
}


// check keyboard in the menu
void keystroke_menu() {
	if (kbhit()) {
		short key = input();

		if (key == 72 || key == 80) {
			print(" ", cur_x, cur_y);

			if (key == 72)
				cur_y = std::max(cur_y - 2, 8);
			else
				cur_y = std::min(cur_y + 2, 14);

			print(">", cur_x, cur_y, 15);
		}

		if (key == 13) play();
	}
}



// custom print function in game for shorter code
void print_game(short x, short y, bool e, bool c) {
	std::string s = std::string(1, table[x][y]);
	std::pair<std::string, short> output = display(s, e, c, visible[x][y]);
	print(output.first, x + 6, y + 4, output.second);
}


// start playing the game
void play() {
	setup_game();
	generate();
	print_game();

	while(true) {
		keystroke_game();
	}
}


// reset all variables in game
void setup_game() {
	game_x = 1;
	game_y = 1;
	col = 9;
	row = 9;
	mines = 10;

	memset(visible, false, sizeof(visible));

	for (short y = 1; y <= 20; ++y) {
		for (short x = 1; x <= 60; ++x) {
			table[x][y] = '0';
		}
	}
}



// randomize mines and put numbers on the grid
void generate() {
	short ran_x, ran_y;
	srand(time(NULL));

	while (mines--) {
		do {
			ran_x = random(1, col);
			ran_y = random(1, row);
		} while (table[ran_x][ran_y] == '*');

		table[ran_x][ran_y] = '*';
	}

	for (short y = 1; y <= row; ++y) {
		for (short x = 1; x <= col; ++x) {
			if (table[x][y] != '0') continue;

			for (short j = std::max(1, y - 1); j <= std::min(row, (short)(y + 1)); ++j) {
				for (short i = std::max(1, x - 1); i <= std::min(col, (short)(x + 1)); ++i) {
					if (table[i][j] == '*') {
						++table[x][y];
					}
				}
			}
		}
	}

	for (short y = 1; y <= row; ++y) {
		for (short x = 1; x <= col; ++x) {
			if (table[x][y] == '0') table[x][y] = ' ';
		}
	}
}


// return the color / e = if the game have ended / c = if the character is chosen / if the character is visble
std::pair<std::string, short> display(std::string s, bool e, bool c, bool v) {
	// if (e == true) {
		// // end game = true
		// if (s == " ") return std::make_pair(s, 136);
		// if (s == "f") return std::make_pair(s, 102);
		// if (s == "F") return std::make_pair("f", 206);
		// if (s == "1") return std::make_pair(s, 113);
		// if (s == "2") return std::make_pair(s, 114);
		// if (s == "3") return std::make_pair(s, 116);
		// if (s == "4") return std::make_pair(s, 117);
		// if ('5' <= s[0] && s[0] <= '8') return std::make_pair(s, 112);
		// if (s == "+") return std::make_pair(s, 112);
		// if (s == "*") return std::make_pair("+", 64);
	// } else {
	// 	// end game = false
	// 	return std::make_pair(s, 136);
	// }

	if (e == false) {
		// not end game yet

		if (c == false) {
			// unchosen character

			if (s == " ") {
				if (v == false) return std::make_pair(s, 136);
				return std::make_pair(s, 119);
			}

			if (s == "f" || s == "F") return std::make_pair("f", 102);

			if ('1' <= s[0] && s[0] <= '8' && v == true) {
				if (s == "1") return std::make_pair(s, 113);
				if (s == "2") return std::make_pair(s, 114);
				if (s == "3") return std::make_pair(s, 116);
				if (s == "4") return std::make_pair(s, 117);
				if ('5' <= s[0] && s[0] <= '8') return std::make_pair(s, 112);
			}

			return std::make_pair(" ", 136);

		} else {
			// chosen character
			if (s == "f" || s == "F") return std::make_pair("f", 238);

			if ('1' <= s[0] && s[0] <= '8' && v == true) {
				if (s == "1") return std::make_pair(s, 241);
				if (s == "2") return std::make_pair(s, 242);
				if (s == "3") return std::make_pair(s, 244);
				if (s == "4") return std::make_pair(s, 245);
				if ('5' <= s[0] && s[0] <= '8') return std::make_pair(s, 240);
			}

			return std::make_pair(" ", 255);
		}

	} else {
		// end game
	}

	return std::make_pair(s, 0);
}


// print the grid
void print_game() {
	short color;
	system("cls");

	for (short y = 1; y <= row; ++y) {
		for (short x = 1; x <= col; ++x) {
			print_game(x, y, false, false);
		}
	}

	// initial chosen character
	print_game(1, 1, false, true);

	// for (int i = 0; i <= 256; ++i) {
	// 	SetConsoleTextAttribute(h_console, i);
	// 	std::cout << i << " test" << std::endl;
	// } std::cin >> row;
}


// check keyboard during gameplay
void keystroke_game() {
	if (kbhit()) {
		short key = input();

		if (72 <= key && key <= 80) {
			print_game(game_x, game_y, false, false);

			if (key == 72) game_y = std::max(game_y - 1, 1);
			if (key == 80) game_y = std::min(game_y + 1, (int)(row));
			if (key == 75) game_x = std::max(game_x - 1, 1);
			if (key == 77) game_x = std::min(game_x + 1, (int)(col));

			print_game(game_x, game_y, false, true);
		}

		if (key == 100) {
			visible[game_x][game_y] = true;

			print_game(game_x, game_y, false, true);
		}
	}
}