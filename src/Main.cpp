#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <vector>
#include <array>

#define STDIN_FILENO 0
#define NB_ENABLE 1
#define NB_DISABLE 0

#define Sleep(x) usleep(x*1000)

int kbhit() 
{
	struct timeval tv;
	fd_set fds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
	return FD_ISSET(STDIN_FILENO, &fds);
}

void nonblock(int state)
{
	struct termios ttystate;
	
	tcgetattr(STDIN_FILENO, &ttystate);
	
	if (state == NB_ENABLE)
	{
		ttystate.c_lflag &= ~ICANON;
		ttystate.c_cc[VMIN] = 1;
	} else if (state == NB_DISABLE)
	{
		ttystate.c_lflag |= ICANON;
	}
	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

int getch()
{
	return fgetc(stdin);
}

using namespace std;

bool gameOver;
const int width = 40;
const int height = 25;
int snakeX, snakeY, fruitX, fruitY, score;
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;
const int INTERVAL = 200;
char head;
vector<array<int,2>> snake;

void Setup()
{
	gameOver = false;
	snakeX = height / 2;
	snakeY = width / 2;
	score = 0;
	dir = STOP;
	srand(time(NULL));
	fruitX = (rand() % (height - 2)) + 1;
	fruitY = (rand() % (width - 2)) + 1;
	head = 'O';
	array<int,2> t = {snakeX, snakeY};
	snake.emplace(snake.begin(),t);
}

bool btail(int x,int y)
{
	vector<array<int,2>>::iterator it;
	for(it=snake.begin()+1;it!=snake.end();it++)
	{
		if ((*it).at(0)==x && (*it).at(1)==y)
		{
			
			return true;
		}
	}
	return false;
}

void Draw()
{
	//очищение экрана
	system("clear");
	
	for (int i=0; i<height; i++)
	{
		for (int j=0; j<width; j++)
		{
			if (i>0 && j>0 && i<height-1 && j<width-1)
				if (i == snakeX && j == snakeY)
				{
					cout << head;
				} else {
					if (btail(i,j))
					{
						cout << "@";
					} else {
						if (i == fruitX && j == fruitY)
						{
							cout << "X";
						}
						else
						{
							cout << " ";
						}
					}
				}
			else
				cout << "#";
		}
		cout << endl;
	}
	cout << "Score = " << score << endl;
	
}

void Input()
{
	switch (getch())
	{
		case 'w':if (dir != DOWN)
		{
			dir = UP;
			head = '^';
		}
		break;
		case 'a':if (dir != RIGHT)
		{
			dir = LEFT;
			head = '<';
		}
		break;
		case 's':if (dir != UP)
		{
			dir = DOWN;
			head = 'v';
		}
		break;
		case 'd':if (dir != LEFT)
		{
			dir = RIGHT;
			head = '>';
		}
		break;
		case 'q':nonblock(NB_DISABLE);
				 gameOver = true;
				 break;
		default: break;
	}
}

void Logic()
{
	switch (dir)
	{
		case UP:snakeX--;
				if (snakeX == 0) snakeX = height-2;
				break;
		case DOWN:snakeX++;
				if (snakeX == height-1) snakeX = 1;
				break;
		case LEFT:snakeY--;
				if (snakeY == 0) snakeY = width - 2;
				break;
		case RIGHT:snakeY++;
				if (snakeY == width-1) snakeY = 1;
				break;
		default:break;
	}
	if (snakeX == fruitX && snakeY == fruitY)
	{
		array<int,2> t = {snakeX, snakeY};
		snake.insert(snake.begin(),t);
		do
		{
			fruitX = (rand() % (height - 2)) + 1;
			fruitY = (rand() % (width - 2)) + 1;
		} while (btail(fruitX,fruitY));
		score+=100;
	} else {
		array<int,2> t = {snakeX, snakeY};
		snake.insert(snake.begin(),t);
		snake.pop_back();
	}
	if (btail(snakeX,snakeY))
	{
		gameOver = true;
	}
}

int main()
{
	Setup();
	nonblock(NB_ENABLE);
	while(!gameOver)
	{
		Draw();
		if (kbhit() != 0)
			Input();
		Logic();
		Sleep(INTERVAL);
	}
	cout << "You died!\n";
	return 0;
}