# ncurses-tetris

![main](https://user-images.githubusercontent.com/51525202/84583941-fda46600-ae39-11ea-85ba-a3e8a7e99525.png)

<br/>

![in-game](https://user-images.githubusercontent.com/51525202/84583942-fe3cfc80-ae39-11ea-87ca-4a58f77d9116.png)


**src**
- main.c : 메인 함수.
- color.c : curses 내장 함수인 color를 사용.
- help.c : 게임 설명서.
- mode.c : 터미널 입출력.
- game.c : 화면의 구성, 점수 및 콤보 합산, 시간 측정 및 난이도 조절과 같은 게임의 진행과 관련된 함수.
- level.c : 난이도 선택 옵션.
- menu.c : 게임의 메인 메뉴를 출력.
- record.c : 기록을 저장하고 정렬 및 출력.  

<br/>

**input**
- space 	: 블록 떨어뜨리기
- i	: 회전
- j : 왼쪽 이동
- l	: 오른쪽 이동
- k	: 하단 이동
- z	: 아이템 사용
- p	: 일시 정지
