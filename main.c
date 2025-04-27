#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define GAME_OVER_TEXT "Game over.\nYour score: %d points.\n" \
                                        "Press any key to quit game"

int score = 0;

struct {
	int len;
	int x[100];
	int y[100];
}snake;

struct {
	int x;
	int y;
}fruit;

enum { key_escape = 27 };

void hide_snake() 
{
	int i;
	int j;
	
	for (i = 0; i < snake.len; i++)
		for (j = 0; j < 2; j++)
			mvaddch(snake.y[i], snake.x[i]+j, ' ');
}

void create_fruit(int max_x, int max_y) 
{
	int x = rand() % max_x;
	int y = rand() % max_y;
	fruit.x = x;
	fruit.y = y;
	attrset(COLOR_PAIR(2));
	mvaddstr(y, x, "  ");
	attroff(COLOR_PAIR(2));
}

void hide_fruit()
{
	mvaddstr(fruit.y, fruit.x, "  ");
}

bool check_on_fruit()
{
	int i;
	int j;
	for (i = 0; i < snake.len; i++) {
		for (j = 0; j < 2; j++) {
			if (snake.y[i] == fruit.y && (snake.x[i]+j == fruit.x || \
									snake.x[i] == fruit.x + j))
					return true;
		}
	}
	return false;
}

bool check_on_wall(int max_x, int max_y)
{
    return snake.x[0] >= max_x - 1 || snake.x[0] <= 0 
            || snake.y[0] >= max_y || snake.y[0] <= 0;
}

bool check_on_self(int max_x, int max_y)
{
    for (int i = 1; i < snake.len; i++)
        if (snake.x[0] == snake.x[i] && snake.y[0] == snake.y[i])
            return true;
    return false;
}

void draw_snake() 
{	
	int i;
	int j;

	attrset(COLOR_PAIR(3));
	mvaddstr(snake.y[0], snake.x[0], "  ");
	attroff(COLOR_PAIR(3));

	attrset(COLOR_PAIR(1));		
	for (i = 1; i < snake.len; i++)
		for (j = 0; j < 2; j++)
			mvaddch(snake.y[i], snake.x[i]+j, ' '); 
	attroff(COLOR_PAIR(1));
}

void move_snake(int dx, int dy) 
{
	int i;

	/* Shifting coords from tail to head. Head coords changing by dx, dy. */
	for (i = snake.len - 1; i > 0; i--) {
		snake.x[i] = snake.x[i-1];
		snake.y[i] = snake.y[i-1];
	}

	snake.x[0] += dx * 2;
	snake.y[0] += dy;
}

void game_over()
{
    clear();
    char msg[100]; 
    sprintf(msg, GAME_OVER_TEXT, score);
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);
    char buf[100];
    int buf_i = 0;
    int str_num = 0;
    for (int i = 0; i < strlen(msg); i++) {
        if (msg[i] == '\n') {
            mvaddstr(max_y / 2 + str_num++, max_x / 2 - strlen(buf) / 2, buf);
            memset(buf, '\0', 100);
            buf_i = 0;
        } else {
            buf[buf_i++] = msg[i];
        }
    }

    mvaddstr(max_y / 2 + str_num, max_x / 2 - strlen(buf) / 2, buf);
    timeout(-1);
    getch();
}

int main() 
{
    printf("Enter snake length: ");
    scanf("%d", &snake.len); 
	initscr();
	cbreak();
	keypad(stdscr, 1);
	noecho();
	srand(time(NULL));
	curs_set(0);
	timeout(100);
	wchar_t key;
	int dx = -1, dy = 0, i;
	int max_x, max_y;
	getmaxyx(stdscr, max_y, max_x);

	if (!has_colors()) {
		printf("Your terminal does not support color.\n");
		return -1;
	}
	start_color();
	
	init_pair(1, COLOR_WHITE, COLOR_BLUE);
	init_pair(2, COLOR_WHITE, COLOR_RED);
	init_pair(3, COLOR_WHITE, COLOR_GREEN);

	/* Initialaze a snake. */
	for (i = 1; i <= 5; i++) {
		snake.x[i-1] = 20+i;
		snake.y[i-1] = 20;
	}

	draw_snake();
	create_fruit(max_x,max_y);	
	while ((key = getch()) != key_escape) {
		switch(key) {
		case 'w':
		case KEY_UP:
			dy = (dy == 1) ? dy : -1;
			dx = 0;
			break;
		case 's':
		case KEY_DOWN:
			dy = (dy == -1) ? dy : 1;
			dx = 0;
			break;
		case 'd':
		case KEY_RIGHT:
			dx = (dx == -1) ? dx : 1;
			dy = 0;
			break;
		case 'a':
		case KEY_LEFT:
			dx = (dx == 1) ? dx : -1;
			dy = 0;
			break;
		case 'q':
			endwin();
			return 0;
		}

		hide_snake();
		move_snake(dx,dy);
        if (check_on_wall(max_x, max_y) || check_on_self(max_x, max_y)) {
            game_over();
            endwin();
            return 0;
        }

		if (check_on_fruit()) {
            score++;
			hide_fruit();
			snake.x[snake.len] = snake.x[snake.len-1] + dx;
		  	snake.y[snake.len] = snake.y[snake.len-1] + dy;
			snake.len++;	
			create_fruit(max_x, max_y);	
		}
	
		draw_snake();
	}

	endwin();
	return 0;	
}
