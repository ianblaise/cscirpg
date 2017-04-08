#include <iostream>
#include <ncurses.h>
#include <list>
#include <fstream>
#include <vector>

using namespace std;

class location {
    int x, y;
};

class map {
    vector<location> gold_spots;
    vector<location> monser_spots;

};

int SIZE_X, SIZE_Y;
int cursor_x, cursor_y;
unsigned char* world;

const unsigned char OPEN = '.'; //An open space
const unsigned char WALL = '#'; //A wall space; impassable
const unsigned char WATER = '~'; //Water, refreshes thirst
const unsigned char ROCK = '&';
const unsigned char YOU = '@';
const unsigned char GOLD = '$';

bool game_on;

const unsigned int TIMEOUT = 3000;

const int UP = 65;
const int DOWN = 66;
const int LEFT = 68;
const int RIGHT = 67;

int index(int i, int j) {
    while(i < 0) i+= SIZE_X;
    while(j < 0) j+= SIZE_Y;
    if(i >= SIZE_X) i %= SIZE_X;
    if(j >= SIZE_Y) i %= SIZE_Y;
    return (i*SIZE_Y+j);
}



//Prints the entire world, bolding the square the cursor is on
void print_world() {
    for (int i = 0; i < SIZE_X; i++) {
        for (int j = 0; j < SIZE_Y; j++) {
            if (i == cursor_x && j == cursor_y)
                attron(A_UNDERLINE | A_BOLD);
            int color = 1;
            if (world[index(i,j)] == WALL)
                color = 5;
            else if (world[index(i,j)] == WATER)
                color = 2;
else if(world[index(i,j)] == ROCK)
                color = 3;
            else if(world[index(i,j)] == GOLD)
                color = 4;
            attron(COLOR_PAIR(color));
            mvaddch(i,j,world[index(i,j)]);
            attroff(COLOR_PAIR(color) );
            attroff(A_UNDERLINE | A_BOLD);
        }
    }
}

void dump_map(string strin) {
    ofstream str(strin);
    for(int i = 0; i < (SIZE_X * SIZE_Y); i++)
        str << world[i] << endl;
}

bool load_map(string mapName) {
    ifstream str(mapName);
    if(!str) return false;
    int counter = 0;
    while(str >> world[counter]) {
        counter++;
    }
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
    bool draw_mode = true;
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
    init_pair(1,COLOR_GREEN,COLOR_GREEN); //Set up some color pairs
init_pair(2,COLOR_CYAN,COLOR_CYAN);
    init_pair(3,COLOR_BLACK,COLOR_BLACK);
    init_pair(4,COLOR_YELLOW,COLOR_GREEN);
    init_pair(5,COLOR_RED,COLOR_BLACK);
    init_pair(6,COLOR_MAGENTA,COLOR_BLACK);
    cbreak();
    timeout(TIMEOUT);

    world = new unsigned char[SIZE_X * SIZE_Y];
    reset_world();
    load_map(mapName);

    int frame = 0;
    game_on = false;

    int curr_obj = '.';
    noecho();
    while(true) {
        if(!game_on)
            mvprintw(SIZE_X, 0, "Draw Mode");
        else
            mvprintw(SIZE_X,0, "Game Running: Frame: %i", frame++);
        int ch = getch();
        if(ch == 'Q' || ch == 'q') break;
        else if(ch == ERR) {
            ;
        }
        else if(ch == 'u') {
            reset_world();
        }
        else if(ch == UP) {
            cursor_x--;
            if(cursor_x < 0){
                cursor_x = 0;
                continue;
            }
            if(draw_mode) world[index(cursor_x, cursor_y)] = curr_obj;
        }
        else if(ch == LEFT) {
            cursor_y--;
            if(cursor_y < 0) {
                cursor_y = 0;
                continue;
            }
            if(draw_mode) world[index(cursor_x, cursor_y)] = curr_obj;
        }
        else if(ch == DOWN) {
            cursor_x++;
            if(cursor_x >= SIZE_X){
                cursor_x = SIZE_X - 1;
                continue;
            }
if(draw_mode) world[index(cursor_x, cursor_y)] = curr_obj;
        }
        else if(ch == RIGHT) {
            cursor_y++;
            if(cursor_y >= SIZE_Y) {
                cursor_y = SIZE_Y -1;
                continue;
            }
            if(draw_mode) world[index(cursor_x, cursor_y)] = curr_obj;
        }
        else if(ch == WALL || ch == 'a') {
            if(world[index(cursor_x, cursor_y)] == OPEN) {
                world[index(cursor_x, cursor_y)] = WALL;
            }
            if(draw_mode) curr_obj = WALL;
        }
        else if(ch == ROCK || ch == 'r') {
            if(world[index(cursor_x,cursor_y)] == OPEN) {
                world[index(cursor_x,cursor_y)] = ROCK;
            }
            if(draw_mode) curr_obj = ROCK;
        }
        else if(ch == WATER || ch == 'w') {
            if(world[index(cursor_x,cursor_y)] == OPEN) {
                world[index(cursor_x,cursor_y)]= WATER;
            }
            if(draw_mode) curr_obj = WATER;
        }
        else if(ch == '.' && draw_mode) {
            world[index(cursor_x, cursor_y)] = '.';
            curr_obj = '.';
        }
        else if(ch == 'g' && draw_mode) {
            world[index(cursor_x, cursor_y)] = '$';
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
    dump_map(new_map);


    return 0;
}




