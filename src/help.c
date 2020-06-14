#include <curses.h>

void help(){
	char ch;

	clear();

	mvprintw( 7, 20, " Rotation is    'i' ");
	mvprintw( 8, 20, " Left     is    'j' ");
	mvprintw( 9, 20, " Right    is    'l' ");
	mvprintw(10, 20, " Down     is    'k' ");
	mvprintw(11, 20, " Drop     is    'space bar");
	mvprintw(12, 20, " Item     is    'z' ");
	mvprintw(13, 20, " Pause    is    'p' ");
	mvprintw(16, 20, " Press a key to Continue..");
	move(LINES-1, COLS-1);
	refresh();

	ch = getchar();
}
