#include <iostream>
#include <ncurses.h>
#include <list>
#include <fstream>
#include <vector>

using namespace std;

int SIZE_X, SIZE_Y;
int cursor_x, cursor_y;
unsigned char* world;

const unsigned char OPEN = '.'; //An open space
const unsigned char WALL = '#'; //A wall space; impassable
const unsigned char WATER = '~'; //Water, you don't know how to swin
const unsigned char ROCK = '&'; // Rocks, you are too scared to climb.
const unsigned char YOU = '@'; //I wonder
const unsigned char GOLD = '$'; //Gold, grants random amount between 5 and 15
const unsigned char PORTAL = 'O'; // PORTALS
bool game_on;

const unsigned int TIMEOUT = 750;

const int UP = 65;
const int DOWN = 66;
const int LEFT = 68;
const int RIGHT = 67;

int index(int i, int j) {
	while(i < 0) i+=SIZE_X;
	while(j < 0) j+=SIZE_Y;
	if(i >= SIZE_X) i%=SIZE_X;
	if(j >= SIZE_Y) i%=SIZE_Y;
	return (i*SIZE_Y+j);
}


//Prints the entire world, bolding the square the cursor is on
void print_world() {
	int color = 0;
	for (int i = 0; i < SIZE_X; i++) {
		for (int j = 0; j < SIZE_Y; j++) {
			color = 1;
			//Current cursor position color/ attrib
			if (i == cursor_x && j == cursor_y){
				attron(A_UNDERLINE | A_BOLD);
				attron(COLOR_PAIR(color));
				mvaddch(i, j, YOU);
				attroff(COLOR_PAIR(color));
				attroff(A_UNDERLINE | A_BOLD);
				continue;	
			}
			//The rest of the map colors
			if (world[index(i,j)] == WALL)
				color = 5;
			else if (world[index(i,j)] == WATER)
				color = 2;
			else if(world[index(i,j)] == ROCK)
				color = 3;
			else if(world[index(i,j)] == GOLD)
				color = 4;
			else if(world[index(i,j)] == PORTAL)
				color = 6;
			attron(COLOR_PAIR(color));
			mvaddch(i,j,world[index(i,j)]);
			attroff(COLOR_PAIR(color) );
			attroff(A_UNDERLINE | A_BOLD);
		}
	}
}

//Very fragile code, needs error checking.
void dump_map(string strin) {
	ofstream str(strin);
	for(int i = 0; i < (SIZE_X * SIZE_Y); i++)
		str << world[i] << endl;
	str.close();
}

//Also fragile code, needs error checking.
bool load_map(string mapName) {
	ifstream str(mapName);
	if(!str) return false;
	int counter = 0;
	while(str >> world[counter]) {
		counter++;
	}
	str.close();
	return true;
}

void reset_world() {
	for (int i = 0; i < SIZE_X; i++) {
		for (int j = 0; j < SIZE_Y; j++) {
			if (i == 0 || j == 0 || i == SIZE_X - 1 || j == SIZE_Y - 1) //Is edge
				world[index(i,j)] = WALL;
			else
				world[index(i,j)] = OPEN;
		}
	}
}

int main() {
	srand(time(NULL));
	bool draw_mode = false;
	SIZE_X = 50;
	SIZE_Y = 200;

	cout << "Load map: ";
	string mapName;
	cin >> mapName;


	cursor_x = SIZE_X/2;
	cursor_y = SIZE_Y/2;

	//start ncurses
	initscr();
	start_color();
	init_pair(1,COLOR_GREEN,COLOR_GREEN);  //for da grass mon
	init_pair(2,COLOR_CYAN,COLOR_CYAN);    //water
	init_pair(3,COLOR_BLACK,COLOR_BLACK);  //rocks?
	init_pair(4,COLOR_YELLOW,COLOR_GREEN); //gold
	init_pair(5,COLOR_RED,COLOR_BLACK);    //things they will pay for
	init_pair(6,COLOR_MAGENTA,COLOR_GREEN);//portals..?
	cbreak();
	timeout(TIMEOUT);

	//Initialize the array that holds the map.
	world = new unsigned char[SIZE_X * SIZE_Y];
	if(!load_map(mapName)) {
		cout << "Map does not exist, loading empty map.\n";
		reset_world();
	}
	int frame = 0;
	game_on = false;

	int curr_obj = '.';

	noecho();
	while(true) {
		print_world();
		if(draw_mode){
			mvprintw(SIZE_X, 0, "Draw Mode Enabled (%c)", curr_obj);
			mvprintw(SIZE_X+1, 0, "d) toggle | u) reset | a) wall | r) rock | w) water | .) grass | g) treasure | p) portal");
		}
		else if(!game_on) 
			mvprintw(SIZE_X, 0, "Paused");
		else
			mvprintw(SIZE_X,0, "Game Running: Frame: %i", frame++);
		int ch = getch();
		if(ch == 'Q' || ch == 'q') break;

		else if(ch == '\n') { 
			game_on = !game_on; 
		}
		//	else if(!game_on) continue;
		else if(ch == 'd') {
			draw_mode = !draw_mode;
			continue;
		}
		else if(ch == ERR) {
			;
		}
		else if(ch == 'u') {
			reset_world();
		}
		else if(ch == UP) {
			if(!draw_mode && world[index(cursor_x-1, cursor_y)] != OPEN) continue;
			cursor_x--;
			if(cursor_x == 0) {
				cursor_x += 1;
				continue;
			}
			if(draw_mode) world[index(cursor_x, cursor_y)] = curr_obj;
		}
		else if(ch == LEFT) {
			if(!draw_mode && world[index(cursor_x, cursor_y-1)] != OPEN) continue;
			cursor_y--;
			if(cursor_y == 0) {
				cursor_y += 1;
				continue;
			}
			if(draw_mode) world[index(cursor_x, cursor_y)] = curr_obj;
		}
		else if(ch == DOWN) {
			if(!draw_mode && world[index(cursor_x+1,cursor_y)] != OPEN) continue;
			cursor_x++;
			if(cursor_x >= SIZE_X - 1){
				cursor_x = SIZE_X - 2;
				continue;
			}
			if(draw_mode) world[index(cursor_x, cursor_y)] = curr_obj;
		}
		else if(ch == RIGHT) {
			if(!draw_mode && world[index(cursor_x, cursor_y+1)] != OPEN) continue;
			cursor_y++;
			if(cursor_y >= SIZE_Y - 1) {
				cursor_y = SIZE_Y - 2;
				continue;
			}
			if(draw_mode) world[index(cursor_x, cursor_y)] = curr_obj;
		}
		if(draw_mode) {
			// Place walls
			if(ch == WALL || ch == 'a') {
				world[index(cursor_x, cursor_y)] = WALL;
				curr_obj = WALL;
			}
			// Place rocks
			else if(ch == ROCK || ch == 'r') {
				world[index(cursor_x,cursor_y)] = ROCK;
				curr_obj = ROCK;
			}
			// Place water..
			else if(ch == WATER || ch == 'w') {
				world[index(cursor_x,cursor_y)]= WATER;
				curr_obj = WATER;
			}
			// Place grass (walkable space)
			else if(ch == '.' && draw_mode) {
				world[index(cursor_x, cursor_y)] = OPEN;
				curr_obj = '.';
			}
			// Place one treasure ($)
			else if(ch == 'g' && draw_mode) {
				world[index(cursor_x, cursor_y)] = GOLD;
			}
			else if(ch == 'p' && draw_mode) { 
				world[index(cursor_x, cursor_y)] = PORTAL;
			}
		}
		clear();
		print_world();
		refresh();
	}

	clear();
	print_world();
	refresh();
	endwin();
	system("clear");

	cout << "Dump map to file: ";
	string new_map;
	cin >> new_map;
	if(new_map == "dont") {
		cout << "be that way." << endl;
		return -1;
	}
	dump_map(new_map);

	return 0;
}




