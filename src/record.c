#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>

#define FILENAME "record"
#define RECORDNUM 10
#define NAMESIZE 11

typedef struct __record_form{
	char name[NAMESIZE];
	int score;
	int level;
} record;

void getName(record*);

void write_record(int score, int level){
	FILE *fp;
	record cur;
	record temp;	
	record highScore[RECORDNUM];
	int cnt = 0;
	int i, j;

	cur.score = score;
	cur.level = level;

	fp = fopen(FILENAME, "rb");

	if(fp == NULL){ 
		fp = fopen(FILENAME, "wb");
		getName(&cur);
		fwrite(&cur, sizeof(struct __record_form), 1, fp);
		fclose(fp);
		return;
	}

	while(1){
		fread(&temp, sizeof(struct __record_form), 1, fp);

		if(feof(fp))
			break;

		highScore[cnt] = temp;
		cnt++;
	}

	fclose(fp);	

	if(cnt ==  RECORDNUM){
		if(highScore[RECORDNUM-1].score > score){
			return;
		}
		else{
			getName(&cur);
			if(cnt == RECORDNUM){
				for(i = 0 ; i < RECORDNUM ; i++){
					if(highScore[i].score < score)
						break;
				}

				for(j = RECORDNUM-2 ; j >= i ; j--){
					highScore[j+1] = highScore[j];
				}
				highScore[i] = cur;
			}
		}
	}
	else{
		getName(&cur);
		for(i = 0 ; i < cnt ; i++){
			if(highScore[i].score < score)
				break;
		}
		cnt++;
		for(j = cnt-2 ; j >= i ; j--){
			highScore[j+1] = highScore[j];
		}
		highScore[i] = cur;

	}

	fp = fopen(FILENAME, "wb");

	if(fp == NULL){
		endwin();
		fprintf(stderr, "cannot write to file\n");
		exit(1);
	}

	for(i = 0 ; i < cnt ; i++)
		fwrite(&highScore[i], sizeof(struct __record_form), 1, fp);

	fclose(fp);
}

void getName(record *r){
	char temp[100];
	clear();
	mvprintw(LINES/2-1,COLS/2 - 12, "%s", "Congratulation! New Record");
	mvprintw(LINES/2, COLS/2 - 12, "%s", "Please Enter Name >>  ");
	refresh();
	fscanf(stdin, "%s", temp);
	getchar();
	temp[NAMESIZE-1] = '\0';
	strcpy(r->name, temp);
}

void print_record(){
	record highScore[10];
	record temp;
	FILE *fp;
	int cnt=0;
	int i;

	fp = fopen(FILENAME, "rb");

	if(fp == NULL){
		endwin();
		fprintf(stderr, "cannot open file\n");
		exit(1);
	}

	while(1){
		fread(&temp, sizeof(struct __record_form), 1, fp);
		if(feof(fp))
			break;

		highScore[cnt] = temp;
		cnt++;
	}

	clear();
	mvprintw(LINES/2 -6 , COLS/2 -23 , "**********************HIGH SCORE*********************");
	mvprintw(LINES/2 -5 , COLS/2 -23 , "%-8s%-18s%-10s%-10s", "Rank", "Name", "Level", "Score");
	mvprintw(LINES/2 -4 , COLS/2 -23 , "-----------------------------------------------------");
	for(i = 0 ; i < cnt ; i++)
		mvprintw(LINES/2-3+i, COLS/2 -23, "%-8d%-18s%-10d%-10d", i+1, highScore[i].name, highScore[i].level, highScore[i].score);
	mvprintw(LINES/2 -3+i , COLS/2 -23 , "-----------------------------------------------------");
	move(LINES-1, COLS-1);
	fclose(fp);
	refresh();
	getchar();
}
