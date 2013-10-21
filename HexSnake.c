#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

#define XMAX 10          // min is 3
#define YMAX 20          // min is 5
#define SNAKESIZEMAX 200 // should be xmax * ymax

void draw_hex(int xapple, int yapple, int** snake);
void move_zig(int** snake, int direction_zig); // for great justice
void printArray(int array[], int lim);
int is_snake(int** snake, int i, int j);
int random_number(int min, int max);

int snakesize;

int main(){
	int i = 0;
	int** snake;
	int xapple, yapple, apple_in_snake;
	int direction_zig = 2; // defaults to lower right b/c that's initial snake setup
	int dead = 0;
	srand((unsigned)time(NULL));
	
	// snake memory & coordinate setup
	snake = (int**) malloc(SNAKESIZEMAX * sizeof(int*));
	for (i = 0; i <= SNAKESIZEMAX; i++) // the <= is so that there's a 0,0 at the very end
		snake[i] = (int*) calloc(2,sizeof(int));
	snake[3][0] = 1; 
	snake[3][1] = 1;
	snake[2][0] = 1;
	snake[2][1] = 2;
	snake[1][0] = 2;
	snake[1][1] = 3;
	snake[0][0] = 2;
	snake[0][1] = 4;
	snakesize = 4;

	// initial apple setup
	xapple = random_number(1,XMAX);
	if (YMAX%2 == 0)
		yapple = random_number(1,YMAX-1);
	else
		yapple = random_number(1,YMAX);
	
	// primary game loop
	while (dead == 0){
		draw_hex(xapple, yapple, snake);
		Sleep(200);
		if (_kbhit()){
			switch (_getche()){
			case '8':
				direction_zig = 0; break; // TODO: Add more control options, like qwe asd or uio jkl
			case '9':
				direction_zig = 1; break; // TODO: Perhaps prevent player from going backwards on themselves?
			case '6':
				direction_zig = 2; break;
			case '5':
				direction_zig = 3; break;
			case '4':
				direction_zig = 4; break;
			case '7':
				direction_zig = 5; break;
			}
		}
		system("cls");                    // TODO: is there a better way to clear the screen? Flickers quite a bit
		
		// apple eaten                       TODO: maybe make the snake grow when apple is eaten, and not the "tick" after? Will need to look at direction for this
		if ((snake[0][0] == xapple) && (snake[0][1] == yapple)){
			snakesize++;
			
			do { // generates new apple coordinates and ensures they don't intersect with snake
				apple_in_snake = 0;
				xapple = random_number(1,XMAX);
				if (YMAX%2 == 0)
					yapple = random_number(1,YMAX-1);
				else
					yapple = random_number(1,YMAX);

				for (i = 0; i <= snakesize; i++) {
					if ((xapple == snake[i][0]) && (yapple == snake[i][1])){
						apple_in_snake = 1;
						break;
					}
				}
			} while (apple_in_snake == 1);
		}

		move_zig(snake, direction_zig);

		// death detection
		if ((snake[0][0] <= 0) || (snake[0][1] <= 0) || (snake[0][0] > XMAX) || (snake[0][1] >= YMAX)) // boundary check
			dead = 1;
		else {
			for (i = 1; i <= snakesize; i++) {  // self intersection check
				if ((snake[0][0] == snake[i][0]) && (snake[0][1] == snake[i][1])){
					dead = 1;
					break;
				}
			}
		}
	}
	
	draw_hex(xapple, yapple, snake);
	printf("\nGame Over");
	if ((snakesize - 4) < 2)
		printf("\nBummer.");
	getchar();
	return 0;
}

void draw_hex(int xapple, int yapple, int** snake){
	int x, y, top, bot;

	for (x = 1; x <= XMAX; x++){ // draws the very top of the grid
		printf(" __   ");
			if (x == XMAX)
				printf("\n");
	}
	for (y = 1; y <= YMAX; y++){
		for (x = 1; x <= XMAX; x++){ // top of the hexes
			// checks if snake is in hex
			top = is_snake(snake, x, y);
			if (y%2 == 0)
				bot = is_snake(snake, x+1, y);
			else
				bot = is_snake(snake, x, y-1);

			// draws
			if      (((y) == (yapple)) && ((x) == xapple) && (bot == 1))
				printf("/()\\##");
			else if (((y) == (yapple)) && ((x) == xapple))
				printf("/()\\__");
			else if ((top == 1) && (bot == 1))
				printf("/##\\##");
			else if (top == 1)
				printf("/##\\__");
			else if (bot == 1)
				printf("/  \\##");
			else
				printf("/  \\__");
			if (x == XMAX)
				if (y == 1)
					printf(" %d\n",y);
				else
					printf("/%d\n",y);
		}
		y++; // moves us to lower half. Well, we're really drawing both halves in each section, but it just doesn't look that way
		for (x = 1; x <= XMAX; x++){ // draws the bottom of the hexes
			// checks if snake is in hex
			top = is_snake(snake, x, y);
			bot = is_snake(snake, x, y-1);

			// draws
			if      (((y) == (yapple)) && ((x) == xapple) && (bot == 1))
				printf("\\##/()");
			else if (((y) == (yapple)) && ((x) == xapple))
				printf("\\__/()");
			else if ((top == 1) && (bot == 1))
				printf("\\##/##");
			else if (top == 1)
				printf("\\__/##");
			else if (bot == 1)
				printf("\\##/  ");
			else
				printf("\\__/  ");
			if (x == XMAX)
				if ((y == YMAX) || (y == (YMAX+1)))
					printf("\n");
				else
					printf("\\%d\n",y);
		}
	}
	for (x = 1; x <= XMAX; x++){
		printf(" %d    ",x); // maybe take the axis out in the final version, but I like 'em here for now
		if (x == XMAX)
			printf("\n\nScore: %d\n", snakesize - 4);
	}
}

void move_zig(int** snake, int direction_zig){
	int i, xhead, yhead;

	if          (direction_zig == 0) { // calculating the head coordinates
		xhead = snake[0][0];           // top                        // top = 0
		yhead = snake[0][1] - 2; }								     // t r = 1
	else if     (direction_zig == 3) {							     // b r = 2
		xhead = snake[0][0];           // bot                        // bot = 3
		yhead = snake[0][1] + 2; }								     // b l = 4
	else if (   (snake[0][1]%2) == 0){ // EVEN (RIGHT SIDE)          // t l = 5
		if      (direction_zig == 1) { // top right
			xhead = snake[0][0] + 1;
			yhead = snake[0][1] - 1; }
		else if (direction_zig == 2) { // bot right
			xhead = snake[0][0] + 1;
			yhead = snake[0][1] + 1; }
		else if (direction_zig == 4) { // bot left
			xhead = snake[0][0];
			yhead = snake[0][1] + 1; }
		else if (direction_zig == 5) { // top left
			xhead = snake[0][0];
			yhead = snake[0][1] - 1; }
	}
	else {                             // ODD (LEFT SIDE)
		if      (direction_zig == 1) { // top right
			xhead = snake[0][0];
			yhead = snake[0][1] - 1; }
		else if (direction_zig == 2) { // bot right
			xhead = snake[0][0];
			yhead = snake[0][1] + 1; }
		else if (direction_zig == 4) { // bot left
			xhead = snake[0][0] - 1;
			yhead = snake[0][1] + 1; }
		else if (direction_zig == 5) { // top left
			xhead = snake[0][0] - 1;
			yhead = snake[0][1] - 1; }
	}
	
	for(i = (snakesize-1); i > 0; i--){ // moving the elements of each array through the memory. TODO: There should be a more efficient way of doing this...
		snake[i][0] = snake[i-1][0];
		snake[i][1] = snake[i-1][1];
	}

	snake[0][0] = xhead;               // assigning the head
	snake[0][1] = yhead; 
}

// returns 1 if snake in x,y otherwise 0
int is_snake(int** snake, int x, int y){
	int i;
	for(i = 0; i < snakesize; i++)
		if ((snake[i][0] == x) && (snake[i][1] == y))
			return 1;
	return 0;
}

// just for debugging & checking
void printArray( int a[], int lim ){
	int i;
	for (i=0; i < lim; i++){
		printf("%2d ",a[i]);
		if (lim == (i + 1))
			printf("\n");
	}
}

int random_number(int min, int max){ //http://stackoverflow.com/a/9572218/625919
    return (int) (((double)(max-min+1)/RAND_MAX) * rand() + min);
}

/*
http://msdn.microsoft.com/en-us/library/windows/desktop/ms646293(v=vs.85).aspx
http://www.cplusplus.com/forum/beginner/27441/
http://stackoverflow.com/questions/7502476/using-getch-to-hold-command-prompt-open-visual-c-2010
http://www.dreamincode.net/forums/topic/176568-not-stop-on-getch-or-cin/
http://www.cplusplus.com/reference/cstdio/fgetc/
http://stackoverflow.com/questions/12060572/getch-does-not-pause-the-loop
http://cboard.cprogramming.com/game-programming/106066-key-press-funtion.html
http://bytes.com/topic/c/answers/597878-program-read-key-press-values

http://www.gamedev.net/topic/636632-annoying-flickering-of-screen-and-holding-the-control-key-problem/
http://stackoverflow.com/search?q=%5Bc%5D+flicker+game
http://stackoverflow.com/questions/2359170/screen-is-flickering-even-when-using-doublebuffering
http://stackoverflow.com/questions/4754577/double-buffering-in-the-console-display
*/