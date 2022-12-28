#include <iostream>
#include <conio.h>
#include <windows.h>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <fstream>



// system variables

HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
std::vector<short> valid_ascii = {13, 27, 28, 83, 100, 102, 114, 224};

short cur_x, cur_y, game_x, game_y;
bool skip;
int temp;

char table[61][21];
bool visible[61][21], flag[61][21];
short col, row, mines;
short dir[5] = {0, 1, 0, -1, 0};

time_t timer, last_time;
std::string current_timer;
bool first_click, win;

std::vector<std::pair<short, short>> stack;
const int birth = 25112004;

bool from_save;

struct from_save {
	char table[61][21];
	bool visible[61][21], flag[61][21];

	short col, row, mines;
	short game_x, game_y;

	std::string current_timer;
	bool first_click;

	short score_b, score_i, score_e;
} save;



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

	for (short i = 0; i < valid_ascii.size(); ++i) {
		if (ascii == valid_ascii[i]) {
			valid = true;
			break;
		}
	}

	if (valid == false) {
		return 0;
	}

	// esc
	if (ascii == 27) {
		at(0, 28);
		exit(0);
	}

	// read arrow
	if (ascii == 224) {
		ascii = getch();
	}

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


// convert number to string
std::string i2s(time_t x) {
	std::stringstream ss;
	ss << x;
	std::string sf = ss.str();
	return sf;
}


// convert string to number
time_t s2i(std::string ss) {
	time_t c;
	std::istringstream(ss) >> c;
	return c;
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
void flood_fill(short x, short y);
bool check_win();

void read_file();
void write_file();


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
	from_save = false;
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
		print("Load save", 7, 16, 15);

		print("<up/down arrow> to change the selection", 80, 8, 15);
		print("<enter> to confirm the selection", 80, 10, 15);
		print("<esc> to exit", 80, 12, 15);

		print(">", cur_x, cur_y, 15);

		read_file();

		print("LEADERBOARD", 7, 20);
		print("Beginner:", 7, 22);
		print("Intermediate:", 7, 24);
		print("Expert:", 7, 26);

		if (save.score_b != 0) {
			print(i2s(save.score_b), 17, 22, 10);
		}

		if (save.score_i != 0) {
			print(i2s(save.score_i), 21, 24, 11);
		}

		if (save.score_e != 0) {
			print(i2s(save.score_e), 15, 26, 12);
		}

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

				if (key == 72) {
					cur_y = cur_y - 2;
					if (cur_y < 8) cur_y = 16;
				} else {
					cur_y = cur_y + 2;
					if (cur_y > 16) cur_y = 8;
				}

				print(">", cur_x, cur_y, 15);
			}

			if (key == 13) {

				if (cur_y == 8) {
					col = row = 9;
					mines = 10;
					play();
					return;
				}

				if (cur_y == 10) {
					col = row = 16;
					mines = 40;
					play();
					return;
				}

				if (cur_y == 12) {
					col = 30;
					row = 16;
					mines = 99;
					play();
					return;
				}

				if (cur_y == 14) {
					std::string temp;

					do {
						system("cls");
						print("Number of rows [2, 20]:", 7, 8, 15);
						at(7, 9);
						std::cin >> temp;
						row = s2i(temp);
					} while (row < 2 || 20 < row);

					do {
						system("cls");
						print("Number of columns [2, 60]:", 7, 8, 15);
						at(7, 9);
						std::cin >> temp;
						col = s2i(temp);
					} while (col < 2 || 60 < col);

					do {
						system("cls");
						at(7, 8);
						SetConsoleTextAttribute(h_console, 15);
						std::cout << "Number of mines [1, " << std::min(col * row - 1, 999) << "]:";
						SetConsoleTextAttribute(h_console, 7);
						at(7, 9);
						std::cin >> temp;
						mines = s2i(temp);
					} while (mines < 1 || col * row - 1 < mines);

					play();
					return;
				}

				if (cur_y == 16 && save.col != 0) {
					from_save = true;
					col = save.col;
					row = save.row;
					mines = save.mines;
					play();
					return;
				}
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
		print_game();
		keystroke_game();
		from_save = false;
	}
}


// reset all variables in game
void setup_game() {
	if (from_save == false) {
		game_x = 1;
		game_y = 1;
	} else {
		game_x = save.game_x;
		game_y = save.game_y;
	}

	first_click = true;
	win = false;
	current_timer = "000";
	stack.clear();

	if (from_save == false) {
		memset(visible, false, sizeof(visible));
		memset(flag, false, sizeof(flag));

		for (short y = 1; y <= 20; ++y) {
			for (short x = 1; x <= 60; ++x) {
				table[x][y] = '0';
			}
		}
	} else {
		for (short y = 1; y <= row; ++y) {
			for (short x = 1; x <= col; ++x) {
				table[x][y] = save.table[x][y];
				visible[x][y] = save.visible[x][y];
				flag[x][y] = save.flag[x][y];
			}
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
			temp = 22125080;
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

	print("TIME 000", 90, 5, 15);
	print("<right/up/left/down arrow> to move", 80, 8, 15);
	print("<d> to discorver the cell", 80, 10, 15);
	print("<f> to flag/unflag the cell", 80, 12, 15);
	print("<esc> to exit", 80, 14, 15);
	print("<shift> + <s> to save", 80, 16, 15);

	for (short y = 1; y <= row; ++y) {
		for (short x = 1; x <= col; ++x) {
			print_cell(x, y, false, false);
		}
	}

	if (from_save == false) {
		print_cell(1, 1, false, true);
	} else {
		print_cell(game_x, game_y, false, true);
		current_timer = save.current_timer;
		print(current_timer, 95, 5, 15);
	}
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

					if ((short)(table[game_x][game_y]) - (short)('0') == count_flag) {
						for (short y = std::max(1, game_y - 1); y <= std::min(row, (short)(game_y + 1)); ++y) {
							for (short x = std::max(1, game_x - 1); x <= std::min(col, (short)(game_x + 1)); ++x) {
								if (flag[x][y] == false && visible[x][y] == false) {

									if (table[x][y] == '*') stack.push_back(std::make_pair(x, y));

									if (table[x][y] == ' ') {
										flood_fill(x, y);
									}

									visible[x][y] = true;

									print_cell(x, y, false, false);

								}
							}
						}
					}
				}

				if (first_click == true) {
					first_click = false;
					if (from_save == false || (from_save == true && save.first_click == true)) {
						generate();
					}

					last_time = time(NULL);
					if (from_save == false) {
						timer = time(NULL);
					} else {
						timer = time(NULL) - s2i(current_timer);
					}
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

				if (first_click == true) {
					first_click = false;

					last_time = time(NULL);

					if (from_save == false) {
						timer = time(NULL);
					} else {
						timer = time(NULL) - s2i(current_timer);
					}
				}

				print_cell(game_x, game_y, false, true);
			}

			// S
			if (key == 83) {
				save.col = col;
				save.row = row;
				save.mines = mines;
				save.current_timer = current_timer;
				save.first_click = first_click;
				save.game_x = game_x;
				save.game_y = game_y;

				for (short j = 1; j <= row; ++j) {
					for (short i = 1; i <= col; ++i) {
						save.table[i][j] = table[i][j];
						save.visible[i][j] = visible[i][j];
						save.flag[i][j] = flag[i][j];
					}
				}

				from_save = false;
				write_file();

				print("SAVE COMPLETE", 80, 18, 15);
			} else {
				print("             ", 80, 18, 15);
			}

			// check win after keyboard hit
			if(check_win()) {
				// to not print selected bomb
				game_x = game_y = 0;
				win = true;
				end_game();
				return;
			}

		} else if (first_click == false) {
			if (time(NULL) != last_time) {
				current_timer = i2s(time(NULL) - timer);

				if(current_timer == "999") {
					game_x = game_y = 0;
					end_game();
					return;
				}

				last_time = time(NULL);
				while (current_timer.size() < 3) current_timer = "0" + current_timer;

				print(current_timer, 95, 5, 15);
			}
		}
	}
}


void end_game() {
	system("cls");

	if (from_save == true) {
		save.col = 0;
	}

	if (win == true) {
		if (col == 9 && row == 9 && mines == 10) {
			if (save.score_b != 0) save.score_b = std::min((short)(s2i(current_timer)), save.score_b);
			else save.score_b = s2i(current_timer);
		}

		if (col == 16 && row == 16 && mines == 40) {
			if (save.score_i != 0) save.score_i = std::min((short)(s2i(current_timer)), save.score_i);
			else save.score_i = s2i(current_timer);
		}

		if (col == 30 && row == 16 && mines == 99) {
			if (save.score_e != 0) save.score_e = std::min((short)(s2i(current_timer)), save.score_e);
			else save.score_e = s2i(current_timer);
		}
	}

	write_file();

	print("TIME", 90, 5, 15);
	if (current_timer == "999") print(current_timer, 95, 5, 64);
	else print(current_timer, 95, 5, 15);

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

	time_t last_print = time(NULL);
	short color_win[2] = {46, 234}, current_color = 0;

	// initial printing
	if (win == true) print("winner winner chicken dinner !", 20, 2, color_win[1]);

	while (true) {
		if (kbhit()) {
			short key = input();
			// r
			if (key == 114) return;
			// f
			if (key == 102) {
				skip = true;
				return;
			}
		} else {
			if (win == true) {
				if (last_print + 1 <= time(NULL)) {
					print("winner winner chicken dinner !", 20, 2, color_win[current_color]);
					last_print = time(NULL);
					// swap color
					current_color ^= 1;
				}
			}
		}
	}
}


bool check_win() {
	short count_visible = 0;

	for (short y = 1; y <= row; ++y) {
		for (short x = 1; x <= col; ++x) {
			if (visible[x][y] == true || flag[x][y] == true) ++count_visible;
		}
	}

	return (count_visible == row * col);
}



// read data from z.d file
void read_file() {
	std::ifstream z;
	z.open("z.d");

	z >> save.col;

	// if there is a save
	if (save.col != 0) {
		z >> save.row >> save.mines >> save.current_timer >> save.game_x >> save.game_y >> save.first_click;

		std::string temp;

		getline(z, temp);
		getline(z, temp);
		for (short j = 1; j <= save.row; ++j) {
			for (short i = 1; i <= save.col; ++i) {
				save.table[i][j] = temp[(j - 1) * save.col + i - 1];
			}
		}

		z >> temp;
		for (short j = 1; j <= save.row; ++j) {
			for (short i = 1; i <= save.col; ++i) {
				save.visible[i][j] = (temp[(j - 1) * save.col + i - 1] == '1');
			}
		}

		z >> temp;
		for (short j = 1; j <= save.row; ++j) {
			for (short i = 1; i <= save.col; ++i) {
				save.flag[i][j] = (temp[(j - 1) * save.col + i - 1] == '1');
			}
		}
	}

	z >> save.score_b >> save.score_i >> save.score_e;

	z.close();
}

// write data to z.d file
void write_file() {
	std::ofstream z;
	z.open("z.d");

	z << save.col;
	if (save.col != 0) {
		z << " " << save.row << " " << save.mines << " " << save.current_timer;
		z << " " << save.game_x << " " << save.game_y << " " << save.first_click << "\n";

		for (short j = 1; j <= row; ++j) {
			for (short i = 1; i <= col; ++i) {
				z << save.table[i][j];
			}
		}
		z << "\n";
		for (short j = 1; j <= row; ++j) {
			for (short i = 1; i <= col; ++i) {
				z << save.visible[i][j];
			}
		}
		z << "\n";
		for (short j = 1; j <= row; ++j) {
			for (short i = 1; i <= col; ++i) {
				z << save.flag[i][j];
			}
		}
	}

	z << "\n" << save.score_b << " " << save.score_i << " " << save.score_e;

	z.close();
}