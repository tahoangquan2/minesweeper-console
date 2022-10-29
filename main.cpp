#include <iostream>
#include <conio.h>
#include <windows.h>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <vector>



// system variables

HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

short valid_ascii[7] = {13, 27, 28, 100, 102, 114, 224};
short cur_x, cur_y, game_x, game_y;

char table[65][25];
bool visible[65][25], flag[65][25];
short col, row;
short mines;
short dir[5] = {0, 1, 0, -1, 0};

short win;
int temp;
bool skip, first_click;

std::vector<std::pair<short, short>> stack;



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

	for (short i = 0; i < 7; ++i) {
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


// print cells at a position with color
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

void print_cell(short x, short y, bool e, bool c);
void play();
void setup_game();
void generate();
std::pair<std::string, short> display(std::string s, bool e, bool c, bool v, bool f);
void print_game();
void keystroke_game();
void end_game();
void flood_fill();


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
	while (true) {
		std::cout << "\t\t#   #  #####  #   #  #####  #####  #   #  #####  #####  #####  #####  #####\n";
		std::cout << "\t\t## ##    #    ##  #  #      #      #   #  #      #      #   #  #      #   #\n";
		std::cout << "\t\t# # #    #    # # #  #####  #####  # # #  #####  #####  #####  #####  #####\n";
		std::cout << "\t\t#   #    #    #  ##  #          #  ## ##  #      #      #      #      #  # \n";
		std::cout << "\t\t#   #  #####  #   #  #####  #####  #   #  #####  #####  #      #####  #   #\n";

		print("Beginner", 7, 8, 10);
		print("Intermediate", 7, 10, 11);
		print("Expert", 7, 12, 12);
		print("Custom", 7, 14, 13);

		print("<up/down arrow> to change the selection", 80, 8, 15);
		print("<enter> to confirm the selection", 80, 10, 15);
		print("<esc> to exit", 80, 12, 15);

		print(">", cur_x, cur_y, 15);

		keystroke_menu();

		system("cls");
	}
}


// check keyboard in the menu
void keystroke_menu() {
	while (true) {
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

			if (key == 13) {
				if (cur_y == 8) {
					col = row = 9;
					mines = 10;
				}

				if (cur_y == 10) {
					col = row = 16;
					mines = 40;
				}

				if (cur_y == 12) {
					col = 30;
					row = 16;
					mines = 99;
				}

				if (cur_y == 14) {

					do {
						system("cls");
						print("Number of rows [2, 20]:", 7, 8, 15);
						at(7, 9);
						std::cin >> row;
					} while (row < 2 || 20 < row);

					do {
						system("cls");
						print("Number of columns [2, 60]:", 7, 8, 15);
						at(7, 9);
						std::cin >> col;
					} while (col < 2 || 60 < col);

					do {
						system("cls");
						at(7, 8);
						SetConsoleTextAttribute(h_console, 15);
						std::cout << "Number of mines [1, " << col * row - 1 << "]:";
						SetConsoleTextAttribute(h_console, 7);
						at(7, 9);
						std::cin >> mines;
					} while (mines < 1 || col * row - 1 < mines);

				}

				play();

				return;
			}
		}
	}
}



// custom print function in game for shorter code
void print_cell(short x, short y, bool e, bool c) {
	std::string s = std::string(1, table[x][y]);
	std::pair<std::string, short> output = display(s, e, c, visible[x][y], flag[x][y]);
	print(output.first, x + 6, y + 4, output.second);
}


// start playing the game
void play() {
	skip = false;

	while (skip == false) {
		setup_game();
		// first click can be a mine, move to keystroke_game
		// generate();
		print_game();
		keystroke_game();
	}
}


// reset all variables in game
void setup_game() {
	game_x = 1;
	game_y = 1;
	first_click = true;
	win = 0;

	stack.clear();

	memset(visible, false, sizeof(visible));
	memset(flag, false, sizeof(flag));

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

	for (short i = 1; i <= mines; ++i) {
		do {
			ran_x = random(1, col);
			ran_y = random(1, row);
		} while (table[ran_x][ran_y] == '*' || (ran_x == game_x && ran_y == game_y));

		table[ran_x][ran_y] = '*';
	}

	for (short y = 1; y <= row; ++y) {
		for (short x = 1; x <= col; ++x) {
			if (table[x][y] != '0') continue;

			for (short j = std::max(1, y - 1); j <= std::min(row, (short)(y + 1)); ++j) {
				for (short i = std::max(1, x - 1); i <= std::min(col, (short)(x + 1)); ++i) {
					if (table[i][j] == '*') ++table[x][y];
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


// return the color / e = game end / c = chosen / v = visble / f = flag
std::pair<std::string, short> display(std::string s, bool e, bool c, bool v, bool f) {

	if (c == false) { // unchosen cell

		if (f == true) {
			if (e == false) return std::make_pair("f", 110);
			else if (s == "*") return std::make_pair("f", 46);
			return std::make_pair("f", 206);
		}

		if (s == " ") {
			if (v == false) return std::make_pair(s, 136);
			return std::make_pair(s, 119);
		}

		if ('1' <= s[0] && s[0] <= '8' && v == true) {
			if (s == "1") return std::make_pair(s, 113);
			if (s == "2") return std::make_pair(s, 114);
			if (s == "3") return std::make_pair(s, 116);
			if (s == "4") return std::make_pair(s, 117);
			if ('5' <= s[0] && s[0] <= '8') return std::make_pair(s, 112);
		}

		if (e == true && s == "*") return std::make_pair(s, 112);

		return std::make_pair(s, 136);

	} else { // chosen cell

		if (f == true) return std::make_pair("f", 230);

		if ('1' <= s[0] && s[0] <= '8' && v == true) {
			if (s == "1") return std::make_pair(s, 241);
			if (s == "2") return std::make_pair(s, 242);
			if (s == "3") return std::make_pair(s, 244);
			if (s == "4") return std::make_pair(s, 245);
			if ('5' <= s[0] && s[0] <= '8') return std::make_pair(s, 240);
		}

		if (e == true && s == "*") return std::make_pair(s, 64);

		return std::make_pair(" ", 255);

	}

	return std::make_pair(s, 0);
}


// print the grid
void print_game() {
	system("cls");

	print("<right/up/left/down arrow> to move", 80, 8, 15);
	print("<d> to discorver the cell", 80, 10, 15);
	print("<f> to flag/unflag the cell", 80, 12, 15);
	print("<esc> to exit", 80, 14, 15);

	for (short y = 1; y <= row; ++y) {
		for (short x = 1; x <= col; ++x) {
			print_cell(x, y, false, false);
		}
	}

	// initial chosen cell
	print_cell(1, 1, false, true);
	temp = 22125080;
}


// fill all the adjacent cells
void flood_fill(short x, short y) {
	for (short i = 0; i < 4; ++i) {
		short new_x = x + dir[i], new_y = y + dir[i + 1];
		if (new_x < 1 || col < new_x || new_y < 1 || row < new_y) continue;

		if (visible[new_x][new_y] == false && table[new_x][new_y] == ' ') {
			visible[new_x][new_y] = true;

			print_cell(new_x, new_y, false, false);

			flood_fill(new_x, new_y);
		}

		if (visible[new_x][new_y] == false && table[new_x][new_y] != ' ' && table[new_x][new_y] != '*') {
			visible[new_x][new_y] = true;

			print_cell(new_x, new_y, false, false);
		}
	}
}


// check keyboard during gameplay
void keystroke_game() {
	while (true) {
		if (kbhit()) {
			short key = input();

			// arrow
			if (72 <= key && key <= 80) {
				print_cell(game_x, game_y, false, false);

				if (key == 72) game_y = std::max(game_y - 1, 1);
				if (key == 80) game_y = std::min(game_y + 1, (int)(row));
				if (key == 75) game_x = std::max(game_x - 1, 1);
				if (key == 77) game_x = std::min(game_x + 1, (int)(col));

				print_cell(game_x, game_y, false, true);
			}

			// d
			if (key == 100 && flag[game_x][game_y] == false) {
				if (visible[game_x][game_y] == true && '0' <= table[game_x][game_y] && table[game_x][game_y] <= '8') {
					short count_flag = 0;

					for (short y = std::max(1, game_y - 1); y <= std::min(row, (short)(game_y + 1)); ++y) {
						for (short x = std::max(1, game_x - 1); x <= std::min(col, (short)(game_x + 1)); ++x) {
							if (flag[x][y] == true) ++count_flag;
						}
					}

					if ((short)(table[game_x][game_y]) - (short)('0') <= count_flag) {
						for (short y = std::max(1, game_y - 1); y <= std::min(row, (short)(game_y + 1)); ++y) {
							for (short x = std::max(1, game_x - 1); x <= std::min(col, (short)(game_x + 1)); ++x) {
								if (flag[x][y] == false && visible[x][y] == false) {

									visible[x][y] = true;

									if (table[x][y] == '*') stack.push_back(std::make_pair(x, y));

									if (table[x][y] == ' ') {
										flood_fill(game_x, game_y);
									}

									print_cell(x, y, false, false);

								}
							}
						}
					}
				}

				if (first_click == true) {
					first_click = false;
					generate();
				}

				visible[game_x][game_y] = true;

				if (table[game_x][game_y] == '*' || stack.size()) {
					end_game();
					return;
				}

				print_cell(game_x, game_y, false, true);

				if (table[game_x][game_y] == ' ') {
					flood_fill(game_x, game_y);
				}
			}

			// f
			if (key == 102 && visible[game_x][game_y] == false) {
				flag[game_x][game_y] ^= 1;

				print_cell(game_x, game_y, false, true);
			}
		}
	}
}

void end_game() {
	system("cls");

	for (short y = 1; y <= row; ++y) {
		for (short x = 1; x <= col; ++x) {
			print_cell(x, y, true, (game_x == x && game_y == y));
		}
	}

	while (stack.size()) {
		short x = stack[0].first, y = stack[0].second;
		print_cell(x, y, true, true);
		stack.pop_back();
	}

	print("<r> to restart", 80, 8, 15);
	print("<f> to return to menu", 80, 10, 15);
	print("<esc> to exit", 80, 12, 15);
	// print("<shift s> to save", 80, 14, 15);

	while (true) {
		if (kbhit()) {
			short key = input();

			if (key == 114) return;

			if (key == 102) {
				skip = true;
				return;
			}
		}
	}
}