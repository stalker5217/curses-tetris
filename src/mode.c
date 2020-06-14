#include <fcntl.h>
#include <termios.h>

#define ON 1
#define OFF 0

void set_nonblock_mode(int s){
	int termflags;
	termflags = fcntl(0, F_GETFL);

	if(s == OFF){ // 모드 오프
		termflags &= ~O_NONBLOCK;
	}
	else{ // 모드 온
		termflags |= O_NONBLOCK;
	}
	fcntl(0, F_SETFL,termflags);
}

void set_canon_mode(int s){
	struct termios oldt, newt;

	tcgetattr(0, &oldt);
	newt = oldt;
	if(s == OFF) // 모드 오프
	{
		newt.c_lflag &= ~ICANON;
	}
	else // 모드 온
	{
		newt.c_lflag |= ICANON;
		newt.c_cc[VMIN] = 0;
	}
	tcsetattr(0, TCSANOW, &newt);
}

void set_echo_mode(int s){
	struct termios oldt, newt;

	tcgetattr(0, &oldt);
	newt = oldt;

	if(s == OFF){
		newt.c_lflag &= ~ECHO;
	}
	else{
		newt.c_lflag |= ECHO;
	}
	tcsetattr(0, TCSANOW, &newt);
}
