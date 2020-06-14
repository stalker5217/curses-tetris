#include <curses.h>

int level_choice(){
	int level;
	int b;

	clear();
	mvprintw(LINES/2 - 1, COLS/2 -12, "%s", "Plesase Enter a level(1~5) >>  ");
	move(LINES-1, COLS-1);
	refresh();
	level = getchar();
	level -= 48;
	while(level <= 0 || level > 5){
		level = getchar();
		level -= 48;
	}
	mvprintw(LINES/2,     COLS/2 -12, "You choose level %d (y/n)", level);
	move(LINES-1, COLS-1);
	refresh();
	
	b = getchar();
	if(b == 'y')
		return level;
	return 1;
}
