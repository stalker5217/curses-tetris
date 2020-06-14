#include <curses.h>

int single_tetris(int);
int display_menu();
void write_record(int, int);
void print_record();
void set_canon_mode(int);
void set_echo_mode(int);
void set_nonblock_mode(int);
int level_choice();
void help();

int main(){
	int choice;
	int s;
	int level = 1; // 난이도 디폴트 값 1
	
	initscr();
	
	while(1){
		set_echo_mode(0);
		set_canon_mode(0);
		set_nonblock_mode(0);
		choice = display_menu();
		switch(choice){
			case 1:
				set_canon_mode(0);
				set_nonblock_mode(1);
				s = single_tetris(level);
				set_echo_mode(1);
				set_nonblock_mode(0);
				set_canon_mode(0);
				write_record(s, level);
				break;
			case 2:	
				level = level_choice();
				break;	
			case 3:
				print_record();
				break;	
			case 4:
				help();
				break;
			case 5:
				set_canon_mode(1);
				set_echo_mode(1);
				set_nonblock_mode(0);
				endwin();
				return 0;

		}
	}
}
