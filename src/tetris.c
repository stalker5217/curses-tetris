#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define DRAW 1
#define REMOVE 0

#define TRUE 1
#define FLASE 0

#define UP 'i'
#define DOWN 'k'
#define RIGHT 'l'
#define LEFT 'j'
#define PAUSE  'p'
#define ITEM 'z'

#define WIDTH 10
#define HEIGHT 20

#define MAX_ITEM 3

typedef struct __Point {
	int x, y;
} Point;

void initTable(); //테이블 초기화
void drawTable(); //테두리 그리기
void drawBoard(); //현재 테이블 상태 그리기
void g_start(int); //게임 시작
void printBrick(int show); //블록 그리기, show가 1이면 그리고 0이면 지움
int  collision(int x, int y, int b, int r);// 충돌검사 x좌표 y좌표 블록번호 회전번호
int drop(); // 블록 바닥으로 떨어뜨리기
int pkey(); // 키 조작
void testFull(); // 깨질 라인 검사
void draw_next_box(); // 다음 블록 테두리
void show_next(int); // 다음 블록 그리기
void show_score(WINDOW*); // 점수 그리기
void set_nonblock_mode(int); // puase 할 때 블로킹 모드 잠깐 킴
void _pause(); // 멈춤 기능
void drawLevel(int); //레벨 그리기
void get_item(); // 아이템 획득
void use_item(); // 아이템 사용
void drawCombo();
void drawItem();

struct __Point block[7][4][4] = { //[블록번호][회전번호][좌표]
	{ { 0,0,1,0,2,0,-1,0},{0,0,0,1,0,-1,0,-2 },{ 0,0,1,0,2,0,-1,0 },{ 0,0,0,1,0,-1,0,-2 } },

	{ { 0,0,1,0,0,1,1,1 },{ 0,0,1,0,0,1,1,1 },{ 0,0,1,0,0,1,1,1},{ 0,0,1,0,0,1,1,1 } },

	{ { 0,0,-1,0,0,-1,1,-1 },{ 0,0,0,1,-1,0,-1,-1 },{ 0,0,-1,0,0,-1,1,-1 },{ 0,0,0,1,-1,0,-1,-1 } },

	{ { 0,0,-1,-1,0,-1,1,0 },{ 0,0,-1,0,-1,1,0,-1 },{ 0,0,-1,-1,0,-1,1,0 },{ 0,0,-1,0,-1,1,0,-1 } },

	{ { 0,0,-1,0,1,0,-1,-1 },{ 0,0,0,-1,0,1,-1,1 },{ 0,0,-1,0,1,0,1,1 },{ 0,0,0,-1,0,1,1,-1 } },

	{ { 0,0,1,0,-1,0,1,-1 },{ 0,0,0,1,0,-1,-1,-1 },{ 0,0,1,0,-1,0,-1,1 },{ 0,0,0,-1,0,1,1,1 } },

	{ { 0,0,-1,0,1,0,0,1 },{ 0,0,0,-1,0,1,1,0 },{ 0,0,-1,0,1,0,0,-1 },{ 0,0,-1,0,0,-1,0,1 } },

};

typedef struct __tile{
	int key; // empty, brick, wall
	int bri_num; //  벽돌이 존재할시 color를 나타내기 위한 벽돌의 번호 값
} tile;

enum { EMPTY=0, BRICK, WALL };
enum { SLOW=1, CLEAR, MINUS};
char *arTile[3] = { "  ", "  ", "##" };
tile table[WIDTH + 2][HEIGHT + 2]; // 테이블
int nx, ny; //이동중인 벽 좌표
int brick, rotate;
int score=0, combo=0;
int gameover = 0;
int suspend = 0;
int sleep_num;
int item[MAX_ITEM];

pthread_mutex_t score_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t draw_mutex = PTHREAD_MUTEX_INITIALIZER;

int single_tetris(int level) {
	int result;

	gameover = 0;
	score = 0;
	combo = 0;
	sleep_num = 4000;
	item[0] = 0;
	item[1] = 0;
	item[2] = 0;
	initTable();
	clear();
	drawTable();
	draw_next_box();
	drawLevel(level);
	drawCombo();
	drawItem();
	start_color();
	setting_color();
	g_start(level);
	result = score;
	return result;
}

void g_start(int level){
	int nFrame, nStay;
	int next_brick;
	pthread_t time_thread;
	void *timer(void *);
	
	nFrame = 200;
	srand(time(NULL));
	next_brick = rand() % 7;
	pthread_create(&time_thread, NULL, timer, (void*)&level);
	while (1) {
		brick = next_brick;
		next_brick = rand() % 7;
		nx = WIDTH / 2; //첫위치는 중간
		ny = 3;
		rotate = 0;

		move(LINES, COLS);
		printBrick(TRUE);
		show_next(next_brick);
		

		if (collision(nx, ny, brick, rotate) != EMPTY){
			gameover = 1;
			pthread_join(time_thread, NULL);
			break; //게임오버
		}

		nStay = nFrame;
		while (2) {
			if (--nStay == 0) {
				nStay = nFrame;
				if (drop())
					break;
			}
			
			if (pkey())
				break;
			usleep(sleep_num/level);
		}
	}
}

void* timer(void *level){
	int time = 0;
	WINDOW *timer_win;	
	
	pthread_mutex_lock(&draw_mutex);
	timer_win = subwin(stdscr, 15, 15, 10, 3*WIDTH + 5);
	mvwprintw(timer_win, 0, 0, "%s", "time");
	mvwprintw(timer_win, 1, 0, "%d", time);
	mvwin(timer_win, LINES-1, COLS-1);
	wrefresh(timer_win);
	pthread_mutex_unlock(&draw_mutex);
	while(suspend == 0){
		usleep(100000);
		time++;
		
		pthread_mutex_lock(&draw_mutex);
		mvwprintw(timer_win, 1, 0, "         ");
		mvwprintw(timer_win, 1, 0, "%.1f", time/(double)10);	
		pthread_mutex_unlock(&draw_mutex);
		
		pthread_mutex_lock(&score_mutex);
		score += 5*(*(int*)level);
		pthread_mutex_unlock(&score_mutex);
		show_score(timer_win);
		
		if(time % 10 == 0 || sleep_num > 100) // 일정 주기마다 속도가 빨라짐
			sleep_num -= 1;
		
		if(gameover == 1)
			return NULL;
	}
}

void drawLevel(int level){
	pthread_mutex_lock(&draw_mutex);
	mvprintw(10, 3*WIDTH + 17, "level : %d", level);
	refresh();
	pthread_mutex_unlock(&draw_mutex);
}


void initTable() { //테이블 초기화
	int x, y;

	for (x = 0; x < WIDTH + 2; x++) { //테두리 생성
		for (y = 0; y < HEIGHT + 2; y++) {
			table[x][y].key = (y == 0 || y == HEIGHT + 1 || x == 0 || x == WIDTH + 1) ? WALL : EMPTY;
			table[x][y].bri_num = -1;
		}
	}
}

void drawTable() { // 테이블 그리기
	int x, y;
	pthread_mutex_lock(&draw_mutex);
	for (x = 0; x < WIDTH + 2; x++)
		for (y = 0; y < HEIGHT + 2; y++)
			mvprintw(1+y,5+x*2, "%s", arTile[table[x][y].key]);
	move(LINES-1, COLS-1);
	refresh();
	pthread_mutex_unlock(&draw_mutex);
}

void drawBoard() { // 테이블보고 전체 벽돌 그리기
	int x, y;
	pthread_mutex_lock(&draw_mutex);
	for (x = 0; x < WIDTH + 1; x++)
		for (y = 0; y < HEIGHT + 1; y++){
			if(table[x][y].bri_num != -1)
				attron(COLOR_PAIR(table[x][y].bri_num+1));
			else
				attron(COLOR_PAIR(8));
			mvprintw(1+y,5+x*2, "%s", arTile[table[x][y].key]);
	}
	move(LINES-1, COLS-1);
	refresh();
	pthread_mutex_unlock(&draw_mutex);
}

void printBrick(int show) { // 현재 벽돌 그리기
	int i;
	pthread_mutex_lock(&draw_mutex);
	attron(COLOR_PAIR(show ? brick+1 : 8));
	for (i = 0; i < 4; i++)
		mvprintw(1+(block[brick][rotate][i].y + ny), 5+(block[brick][rotate][i].x + nx)*2, "%s", arTile[show ? BRICK : EMPTY]);
	attron(COLOR_PAIR(8));
	move(LINES-1, COLS-1);
	refresh();
	pthread_mutex_unlock(&draw_mutex);
	//show가 DRAW면 그리고, REMOVE면 지워줌
}

int  collision(int x, int y, int b, int r) { // 충돌검사
	int i;
	int k = EMPTY;

	for (i = 0; i < 4; i++)
		k = (k > table[x + block[b][r][i].x][y + block[b][r][i].y].key) ? k : table[x + block[b][r][i].x][y + block[b][r][i].y].key ;

	return k;
}

int drop() { //한블록 아래로 이동
	if (collision(nx, ny + 1, brick, rotate) != EMPTY) {
		testFull();
		return TRUE;
	}
	printBrick(REMOVE);
	ny++;
	printBrick(DRAW);
	return FALSE;
}

void testFull() {
	int i, x, y, ty;
	int crush = 0; // 콤보 계산을 위한 변수

	for (i = 0; i < 4; i++){
		table[nx + block[brick][rotate][i].x][ny + block[brick][rotate][i].y].key = BRICK;
		table[nx + block[brick][rotate][i].x][ny + block[brick][rotate][i].y].bri_num = brick;
	}

	for (y = 1; y < HEIGHT + 1; y++) {
		for(x = 1 ; x < WIDTH+1;x++)
			if(table[x][y].key != BRICK){
				break;
			}
		if(x==WIDTH+1){ // 한줄이 꽉 차 있으면
			for (ty = y; ty > 1; ty--) {
				for (x = 1; x < WIDTH + 1; x++) {
					table[x][ty].key = table[x][ty - 1].key;
					table[x][ty].bri_num = table[x][ty - 1].bri_num;
				}
			}
			crush = 1;
			combo++;
			if(combo == 1) // 5의 배수일 경우 아이템 획득
				get_item();
			pthread_mutex_lock(&score_mutex);
			score += 1000 * combo;
			pthread_mutex_unlock(&score_mutex);
			drawBoard();
			drawItem();
			usleep(200000);
		}
	}
	if(crush == 0) // 테스트에서 한줄이라도 깨짐이 없으면 combo를 0 으로 초기화
		combo = 0;

	drawCombo();
}

int pkey() {
	int ch, trot;
	ch = getchar();

	switch (ch) {
	case LEFT:
		if (collision(nx - 1, ny, brick, rotate) == EMPTY) {
			printBrick(REMOVE);
			nx--;
			printBrick(DRAW);
		}
		break;
	case RIGHT:
		if (collision(nx + 1, ny, brick, rotate) == EMPTY) {
			printBrick(REMOVE);
			nx++;
			printBrick(TRUE);
		}
		break;
	
	case UP:
		trot = (rotate == 3 ? 0 : rotate + 1);
		if (collision(nx, ny, brick, trot) == EMPTY) {
			printBrick(REMOVE);
			rotate = trot;
			printBrick(TRUE);
		}
		break;
	case DOWN:
		if (drop()) {
			return TRUE;
		}
		break;
	case PAUSE:
		_pause();
		break;
	case ITEM:
		use_item();
		break;
	case ' ':
		while (drop() == FALSE)
		{
			;
		}
		return TRUE;
	}

	return FALSE;
}	

void _pause(){
	suspend = 1;
	set_nonblock_mode(0);
	pthread_mutex_lock(&draw_mutex);
	attron(COLOR_PAIR(1));
	mvprintw(HEIGHT + 3, 5, "        P A U S E       ");
	attron(COLOR_PAIR(8));
	refresh();
	getchar();
	suspend = 0;
	mvprintw(HEIGHT + 3, 5, "                        ");
	set_nonblock_mode(1);
	pthread_mutex_unlock(&draw_mutex);
}

void draw_next_box(){
	pthread_mutex_lock(&draw_mutex);
	mvprintw(1, 3* WIDTH + 3, "%s", "*****next*****");
	mvprintw(2, 3* WIDTH + 3, "%s", "*            *");
	mvprintw(3, 3* WIDTH + 3, "%s", "*            *");
	mvprintw(4, 3* WIDTH + 3, "%s", "*            *");
	mvprintw(5, 3* WIDTH + 3, "%s", "*            *");
	mvprintw(6, 3* WIDTH + 3, "%s", "*            *");
	mvprintw(7, 3* WIDTH + 3, "%s", "*            *");
	mvprintw(8, 3* WIDTH + 3, "%s", "**************");
	refresh();
	pthread_mutex_unlock(&draw_mutex);
}

void show_next(int next){
	int i;
	pthread_mutex_lock(&draw_mutex);

	for(i = 0 ; i < 6 ; i++)
		mvprintw(2+i, 3* WIDTH + 4, "%s", "            ");

	attron(COLOR_PAIR(next+1));
	for(i = 0 ; i < 4 ; i++)
		mvprintw(5+block[next][0][i].y, 3* WIDTH + 9 + (block[next][0][i].x * 2), "%s", arTile[BRICK]);

	move(LINES-1, COLS-1);
	attron(COLOR_PAIR(8));
	refresh();
	pthread_mutex_unlock(&draw_mutex);
}

void show_score(WINDOW* t){
	pthread_mutex_lock(&draw_mutex);
	mvwprintw(t, 4, 0, "%s" , "score");
	mvwprintw(t, 5, 0, "%d", score);
	mvwin(t, LINES-1, COLS-1);
	wrefresh(t);
	pthread_mutex_unlock(&draw_mutex);
}

void get_item(){
	if(item[0] == 0)
		item[0] = rand() % 3 + 1;
	else if(item[1] == 0)
		item[1] = rand() % 3 + 1;
	else if(item[2] == 0)
		item[2] = rand() % 3 + 1;
}

void use_item(){
	int x, y, ty;
	if(item[0] == 0)
		return;

	switch(item[0]){
		case SLOW:
			sleep_num + 300;
			break;
		case CLEAR:
			initTable();
			drawBoard();
			break;
		case MINUS:
			for(y = HEIGHT-2 ; y <= HEIGHT ; y++){
				for(ty = y ; ty > 1 ;ty--){
					for(x = 1 ; x < WIDTH + 1 ; x++){
						table[x][ty].key = table[x][ty-1].key;
						table[x][ty].bri_num = table[x][ty-1].bri_num;
					}
				}
			}
			drawBoard();
			break;
	}

	item[0] = item[1];
	item[1] = item[2];
	item[2] = 0;
	drawItem();
}

void drawCombo(){
	pthread_mutex_lock(&draw_mutex);
	attron(COLOR_PAIR(8));
	mvprintw(14, 3*WIDTH + 17, "combo: %d", combo);
	refresh();
	pthread_mutex_unlock(&draw_mutex);
}			

void drawItem(){
	pthread_mutex_lock(&draw_mutex);
	attron(COLOR_PAIR(8));
	mvprintw(17, 3*WIDTH+8, "**Item List**");
	mvprintw(18, 3*WIDTH+8, "%s", item[0] == 1 ? "1. SLOW " : item[0] == 2 ? "1. CLEAR" : item[0] == 3 ? "1. MINUS" : "        ");
	mvprintw(19, 3*WIDTH+8, "%s", item[1] == 1 ? "2. SLOW " : item[1] == 2 ? "2. CLEAR" : item[1] == 3 ? "2. MINUS" : "        ");
	mvprintw(20, 3*WIDTH+8, "%s", item[2] == 1 ? "3. SLOW " : item[2] == 2 ? "3. CLEAR" : item[2] == 3 ? "3. MINUS" : "        ");
	refresh();
	pthread_mutex_unlock(&draw_mutex);
}
