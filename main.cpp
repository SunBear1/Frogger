#define _USE_MATH_DEFINES
#define SCREEN_WIDTH	1000
#define SCREEN_HEIGHT	850
#define SIZE 128
#define carquantity 4
#ifdef __cplusplus
extern "C"
#endif
#include<math.h>
#include<stdio.h>
#include <cstdlib>
#include<string.h>
#include <time.h>
extern "C" {
#include"../Frog/SDL2-2.0.10/include/SDL.h"
#include"../Frog/SDL2-2.0.10/include/SDL_main.h"
}
const double log_length = 133;
const double turtle_length = 160;
const double log_long_length = 266;
struct 
{
	int lives = 5;
	double frogX = SCREEN_WIDTH / 2;
	double frogY = SCREEN_HEIGHT - 25;
	double timebar = 0;
	double worldTime=0;
	double points = 0;
	double bee_position;
	double bonus_showtime = 0;
	bool babyfrog_taken = false;
	bool babyfrog_available = true;
	bool bee_available = true;
	bool double_bonus_taken = false;
	bool frog_bonus_taken = false;
	bool bee_bonus_taken = false;
}s;
void DrawString(SDL_Surface *screen, int x, int y, const char *text,SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
		};
	};
// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt �rodka obrazka sprite na ekranie
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
	};
// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
	};
// rysowanie linii o d�ugo�ci l w pionie (gdy dx = 0, dy = 1) 
// b�d� poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
		};
	};
// rysowanie prostok�ta o d�ugo�ci bok�w l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};
void Reset()
{
	s.frogX = SCREEN_WIDTH / 2;
	s.frogY = SCREEN_HEIGHT - 25;
	s.timebar = 0;
	s.worldTime = 0;
	s.bonus_showtime = 0;
	if (s.babyfrog_taken == true)
		s.babyfrog_available = false;
		
}
//--------------------------------------------
//player just reached border of the map, back to start position
void OutOfMap()
{
	if (s.frogX < 0 || s.frogX > SCREEN_WIDTH)
	{
		s.frogX = SCREEN_WIDTH / 2;
		s.frogY = SCREEN_HEIGHT - 25;
		s.lives--;
		Reset();
	}
}
//-------------------------------------//-------
//player is on the left side of log/turtle.  'Z' means its a turtle, 'K' means its a log
void Attachleft(double* object,char type)
{
	if(type=='z')
		s.frogX = *object-turtle_length/3;
	else if(type=='k')
		s.frogX = *object - log_length/3;
	OutOfMap();
}
//--------------------------------------------
//player is in the middle of log/turtle
void Attach(double* object)
{
	s.frogX = *object;
	OutOfMap();
}
//--------------------------------------------
//player is on the right side of log/turtle. 'Z' means its a turtle, 'K' means its a log
void Attachright(double* object,char type)
{
	if (type == 'z')
		s.frogX = *object + turtle_length / 3;
		
	else if (type == 'k')
		s.frogX = *object + log_length / 3;
	OutOfMap();
}
//--------------------------------------------
//object is spawning and going from the right to left side of the screen. If object passes one screen length, 
//set X cordinate back to right side of the map. Speed is the value, how fast object is moving.
void SpawnLeft(double *object,SDL_Surface* screen, SDL_Surface* pic, int height,double speed)
{
	*object -= speed;
	if ((*object > -160) && (*object < SCREEN_WIDTH + 160))
		DrawSurface(screen, pic, (int)*object, height);
	if (*object < -(1) * SCREEN_WIDTH)
		*object = SCREEN_WIDTH + 160;
}
//--------------------------------------------
//object is spawning and going from the left to right side of the screen. If object passes one screen length, 
//set X cordinate back to left side of the map. Speed is the value, how fast object is moving.
void SpawnRight(double* object, SDL_Surface* screen, SDL_Surface* pic, int height, double speed)
{
	*object += speed;
	if ((*object > -160) && (*object < SCREEN_WIDTH + 160))
		DrawSurface(screen, pic, *object, height);
	if (*object > 2 * SCREEN_WIDTH)
		*object = -160;
}
//--------------------------------------------
//player stepped on a object. 'Z' means he fell into water cause of turtle dive, 'C' he hit a car
void Hit(double *object,int height,char type)
{
	if (s.frogY == height)
	{
		if (type == 'z')
		{
			if ((*object - 80 < s.frogX) && (*object + 80 > s.frogX))
			{
				Reset();
				s.lives--;
			}
		}
		if(type == 'c')
		{
			if ((*object - 30 < s.frogX) && (*object + 30 > s.frogX))
			{
				Reset();
				s.lives--;
			}
		}	
	}
}
//--------------------------------------------
//if bee wasnt already eaten, it spawns in 5 locations depending on a X which is random 
void bee_spawn(int x, SDL_Surface* screen, SDL_Surface* pic)
{
	if (s.bee_available==true)
	{
			if (x == 0)
				DrawSurface(screen, pic, SCREEN_WIDTH / 2 - 430, 100);
			else if (x == 1)
				DrawSurface(screen, pic, SCREEN_WIDTH / 2 - 220, 100);
			else if (x == 2)
				DrawSurface(screen, pic, SCREEN_WIDTH / 2 + 1, 100);
			else if (x == 3)
				DrawSurface(screen, pic, SCREEN_WIDTH / 2 + 220, 100);
			else if (x == 4)
				DrawSurface(screen, pic, SCREEN_WIDTH / 2 + 430, 100);
	}
}
//--------------------------------------------
//if babyfrog wasnt already picked up,display baby frog on a road or on the player
void baby_frog_spawn(SDL_Surface* screen, SDL_Surface* pic)
{
	if (s.babyfrog_available == true)
	{
		if (s.babyfrog_taken == true)
			DrawSurface(screen, pic, s.frogX, s.frogY); //player picked up the bbayfrog
		else if ((s.frogX == 610) && (s.frogY == 705))
			s.babyfrog_taken = true; //player stepped on a babyfrog
		else if (s.babyfrog_taken == false)
			DrawSurface(screen, pic, SCREEN_WIDTH-390, SCREEN_HEIGHT - 145);
	}
}

// --------------------------------------------------
// Loading sprite (SDL_Surface) from picture
SDL_Surface* LoadSprite(char* ImagePath)
{
	SDL_Surface* sprite = NULL;
	sprite = SDL_LoadBMP(ImagePath);
	if (sprite == NULL) {
		printf("SDL_LoadBMP(%s) error: %s\n", ImagePath, SDL_GetError());
		SDL_Quit();
		return NULL;
	}
	else
		return sprite;
}

// --------------------------------------------------
//all combinations with displaying bonuses
void DisplayBonus(double delta, char* text, char* bonus_text, SDL_Surface* screen, SDL_Surface* charset, int offset,char x)
{
	if (s.bonus_showtime < 50)
	{
		if (x == 'd')
		{
			s.frog_bonus_taken = true;
			s.bee_bonus_taken = true;
		}
		if (x == 'b')
			s.double_bonus_taken = true;
		if (x == 'f')
			s.double_bonus_taken = true;
		s.bonus_showtime += 1;
		sprintf(text, bonus_text);
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2 + offset, SCREEN_HEIGHT - 780, text, charset);
	}
	else
	{
		if (x == 'b')
		{
			s.bee_bonus_taken = true;
			s.double_bonus_taken = true;
		}
		if (x == 'f')
		{
			s.frog_bonus_taken = true;
			s.double_bonus_taken = true;
		}
		if (x == 'd')
		{
			s.double_bonus_taken = true;
			s.frog_bonus_taken = true;
			s.double_bonus_taken = true;
		}
		s.bonus_showtime = 0;
	}
		
}

// --------------------------------------------------
// saving scores into a file
void SaveHighScore(char* player) {
	FILE* savefile;
	savefile = fopen("highscores.txt", "a+");
	fprintf(savefile, "%s %.2f \n", player, s.points);
	fclose(savefile);
}

// --------------------------------------------------
// Loading scores from a file
void LoadHighScore(char* text,SDL_Surface* screen, SDL_Surface* charset) {
	FILE* loadfile;
	float score = 0;
	char nick[SIZE];
	loadfile = fopen("highscores.txt", "r");
	for (int i = 0; i < 10; i++)
	{
		fscanf(loadfile, "%s", nick);
		fscanf(loadfile, "%f", &score);
		//score = (double)scoretxt;
		//fscanf(loadfile, "%f", score);
		sprintf(text, "%d : %s %.2f", i + 1, nick, score);
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 + (10 * i), text, charset);
	}
	fclose(loadfile);
}


int main(int argc, char **argv)
{
	FILE* file;
	char text[SIZE] = {}, player[SIZE] = {};
	int t1 = SDL_GetTicks(), t2, quit = 0, frames = 0, rc, paused = 0, pausedkey = 0, enter = 0, quitgameyes = 1, quitgameno = 1, quitgame = 0,
		space = 0,letter=0,highscoremenu=0,number_of_records=0;
	bool win_condition_1 = false, win_condition_2 = false, win_condition_3 = false, win_condition_4 = false, 
		 win_condition_5 = false;
	double scores[SIZE] = {}, delta, points_hs = 0, logspeed1 = 0.7, logspeed2 = 1, logspeed3 = 0.6, turtlespeed1 = 0.6, turtlespeed2 = 0.6, carspeed1 = 1, carspeed2 = 1,
		   carspeed3=1.5, carspeed4=1, carspeed5=1.8,jumpX=55,jumpY=60,maxjump=1000, log1_1=1500, log1_2=1000,
		log1_3=500, log1_4=0, log2_1=1200, log2_2=800, log2_3=300, log2_4=0, log3_1=800, log3_2=1200, log3_3=400,log3_4=-200,
		turtle1_1=800, turtle1_2=1200, turtle1_3=300, turtle1_4=0, turtle2_1=1000, turtle2_2=1500, turtle2_3=100, turtle2_4=600,
		car1[carquantity] = { 200,700,1200,1700 }, car2[carquantity] = { 0,500,1500,2000 }, car3[carquantity] = { 100,600,1100,1600 }, 
		car4[carquantity] = { 300,800,1300,1800 }, car5[carquantity] = {800,1200,400,200};
	SDL_Event event;
	SDL_Surface *log,*frog,*turtle,*auto2, *auto1,*log_long,*finish,*pause,*menu,*quityes,*quitno,*lose,*win,*bee,
		*diving_turtle,*baby_frog, *screen, *charset;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;
	printf("wyjscie printfa trafia do tego okienka\n");
	printf("printf output goes here\n");
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
		}

	// tryb pe�noekranowy / fullscreen mode
	//rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,&window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
		};
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_SetWindowTitle(window, "FROGGER");
	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_ShowCursor(SDL_DISABLE);
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00), zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00),
		czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00), niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC),
		fioletowy = SDL_MapRGB(screen->format, 0x8A, 0x2B, 0xE2);

	//loading images from files
	charset = LoadSprite("../Frog/cs8x8.bmp");
	if (charset == NULL)
		return 1;
	SDL_SetColorKey(charset, true, 0x000000);

	frog = LoadSprite("../Frog/zaba.bmp");
	if (frog == NULL)
		return 1;

	menu = LoadSprite("../Frog/menu.bmp");
	if (menu == NULL)
		return 1;

	turtle = LoadSprite("../Frog/zolw.bmp");
	if (turtle == NULL)
		return 1;

	diving_turtle = LoadSprite("../Frog/zolwnurek.bmp");
	if (diving_turtle == NULL)
		return 1;
	
	quityes = LoadSprite("../Frog/quityes.bmp");
	if (quityes == NULL)
		return 1;

	baby_frog = LoadSprite("../Frog/babyzaba.bmp");
	if (baby_frog == NULL)
		return 1;

	quitno = LoadSprite("../Frog/quitno.bmp");
	if (quitno == NULL)
		return 1;

	bee = LoadSprite("../Frog/bee.bmp");
	if (bee == NULL)
		return 1;

	log = LoadSprite("../Frog/kloda.bmp");
	if (log == NULL)
		return 1;

	finish = LoadSprite("../Frog/meta.bmp");
	if (finish == NULL)
		return 1;

	log_long = LoadSprite("../Frog/klodalong.bmp");
	if (log_long == NULL)
		return 1;

	pause = LoadSprite("../Frog/pauza.bmp");
	if (pause == NULL)
		return 1;

	win = LoadSprite("../Frog/win.bmp");
	if (win == NULL)
		return 1;

	lose = LoadSprite("../Frog/lose.bmp");
	if (lose == NULL)
		return 1;

	auto1 = LoadSprite("../Frog/auto1.bmp");
	if (auto1 == NULL)
		return 1;

	auto2 = LoadSprite("../Frog/auto2.bmp");
	if (auto2 == NULL)
		return 1;
	
	//random bee location
	srand(time(NULL));
	//s.bee_position = rand() % 5;
	s.bee_position = 1;
	// -------------------------------------------------------------------
	// main program loop
	// -------------------------------------------------------------------

	while (!quit) {

			//time operations
			t2 = SDL_GetTicks();
			delta = (t2 - t1) * 0.001;
			t1 = t2;
			
			//drawing river,shores and finishline
			SDL_FillRect(screen, NULL, czarny);
			DrawRectangle(screen, 0, (45), SCREEN_WIDTH, 400, niebieski, niebieski);
			DrawRectangle(screen, 0, (SCREEN_HEIGHT - 50), SCREEN_WIDTH, 50, fioletowy, fioletowy);
			DrawRectangle(screen, 0, (SCREEN_HEIGHT / 2 + 20), SCREEN_WIDTH, 50, fioletowy, fioletowy);
			DrawRectangle(screen, 0, (45), SCREEN_WIDTH, 80, fioletowy, fioletowy);
			DrawSurface(screen, finish, SCREEN_WIDTH / 2 + 1, 85);

			bee_spawn(s.bee_position, screen, bee);

			//player is on a 1 row of river, attach conditions:
			if (s.frogY == SCREEN_HEIGHT - 445)
			{
				if ((turtle1_1 - 80 < s.frogX) && (turtle1_1 - 27 > s.frogX))
					Attachleft(&turtle1_1,'z');
				else if ((turtle1_1 - 27 < s.frogX) && (turtle1_1 + 26 > s.frogX))
					Attach(&turtle1_1);
				else if ((turtle1_1 + 26 < s.frogX) && (turtle1_1 + 80 > s.frogX))
					Attachright(&turtle1_1,'z');
				else if ((turtle1_2 - 80 < s.frogX) && (turtle1_2 - 27 > s.frogX))
					Attachleft(&turtle1_2,'z');
				else if ((turtle1_2 - 27 < s.frogX) && (turtle1_2 + 26 > s.frogX))
					Attach(&turtle1_2);
				else if ((turtle1_2 + 26 < s.frogX) && (turtle1_2 + 80 > s.frogX))
					Attachright(&turtle1_2,'z');
				else if ((turtle1_3 - 80 < s.frogX) && (turtle1_3 - 27 > s.frogX))
					Attachleft(&turtle1_3,'z');
				else if ((turtle1_3 - 27 < s.frogX) && (turtle1_3 + 26 > s.frogX))
					Attach(&turtle1_3);
				else if ((turtle1_3 + 26 < s.frogX) && (turtle1_3 + 80 > s.frogX))
					Attachright(&turtle1_3,'z');
				else if ((turtle1_4 - 80 < s.frogX) && (turtle1_4 - 27 > s.frogX))
					Attachleft(&turtle1_4,'z');
				else if ((turtle1_4 - 27 < s.frogX) && (turtle1_4 + 26 > s.frogX))
					Attach(&turtle1_4);
				else if ((turtle1_4 + 26 < s.frogX) && (turtle1_4 + 80 > s.frogX))
					Attachright(&turtle1_4,'z');
				else
				{
					Reset();
					s.lives--;
				}
			}

			//1st row of logs
			if (s.frogY == SCREEN_HEIGHT - 505) 
			{

				if ((log1_1 - 67 < s.frogX) && (log1_1 - 22 > s.frogX))
					Attachleft(&log1_1,'k');
				else if ((log1_1 - 22 < s.frogX) && (log1_1 + 22 > s.frogX))
					Attach(&log1_1);
				else if ((log1_1 + 22 < s.frogX) && (log1_1 + 67 > s.frogX))
					Attachright(&log1_1,'k');
				else if ((log1_2 - 67 < s.frogX) && (log1_2 - 22 > s.frogX))
					Attachleft(&log1_2,'k');
				else if ((log1_2 - 22 < s.frogX) && (log1_2 + 22 > s.frogX))
					Attach(&log1_2);
				else if ((log1_2 + 22 < s.frogX) && (log1_2 + 67 > s.frogX))
					Attachright(&log1_2,'k');
				else if ((log1_3 - 67 < s.frogX) && (log1_3 - 22 > s.frogX))
					Attachleft(&log1_3,'k');
				else if ((log1_3 - 22 < s.frogX) && (log1_3 + 22 > s.frogX))
					Attach(&log1_3);
				else if ((log1_3 + 22 < s.frogX) && (log1_3 + 67 > s.frogX))
					Attachright(&log1_3,'k');
				else if ((log1_4 - 67 < s.frogX) && (log1_4 - 22 > s.frogX))
					Attachleft(&log1_4,'k');
				else if ((log1_4 - 22 < s.frogX) && (log1_4 + 22 > s.frogX))
					Attach(&log1_4);
				else if ((log1_4 + 22 < s.frogX) && (log1_4 + 67 > s.frogX))
					Attachright(&log1_4,'k');
				else
				{
					Reset();
					s.lives--;
				}
			}

			//2nd row of logs. There are 5 places where player can stand, because of the long log
			if (s.frogY == SCREEN_HEIGHT - 565) 
			{
				if ((log2_1 - 133 < s.frogX) && (log2_1 - 80 > s.frogX))
				{
						s.frogX = log2_1 - 2* (log_long_length/5); //2 spots to the left
						OutOfMap();
				}
				else if ((log2_1 - 80 < s.frogX) && (log2_1 - 27 > s.frogX))
				{
					s.frogX = log2_1 - log_long_length/5; //1 spot to the left
					OutOfMap();
				}
				else if ((log2_1 - 27 < s.frogX) && (log2_1 + 26 > s.frogX))
				{
					s.frogX = log2_1; //middle
					OutOfMap();
				}
				else if ((log2_1 + 26 < s.frogX) && (log2_1 +  80> s.frogX))
				{
					s.frogX = log2_1 + log_long_length / 5; //1 spot to the right
					OutOfMap();
				}
				else if ((log2_1 + 80 < s.frogX) && (log2_1 + 133 > s.frogX))
				{
					s.frogX = log2_1 + 2 * (log_long_length / 5); //2 spots to the right
					OutOfMap();
				}
				//same thing with the 2nd log
				else if ((log2_2 - 133 < s.frogX) && (log2_2 - 80 > s.frogX))
				{
					s.frogX = log2_2 - 2 * (log_long_length / 5);
					OutOfMap();
				}
				else if ((log2_2 - 80 < s.frogX) && (log2_2 - 27 > s.frogX))
				{
					s.frogX = log2_2 - log_long_length / 5;
					OutOfMap();
				}
				else if ((log2_2 - 27 < s.frogX) && (log2_2 + 26 > s.frogX))
				{
					s.frogX = log2_2;
					OutOfMap();
				}
				else if ((log2_2 + 26 < s.frogX) && (log2_2 + 80 > s.frogX))
				{
					s.frogX = log2_2 + log_long_length / 5;
					OutOfMap();
				}
				else if ((log2_2 + 80 < s.frogX) && (log2_2 + 133 > s.frogX))
				{
					s.frogX = log2_2 + 2 * (log_long_length / 5);
					OutOfMap();
				}
				//and 3rd one
				else if ((log2_3 - 133 < s.frogX) && (log2_3 - 80 > s.frogX))
				{
					s.frogX = log2_3 - 2 * (log_long_length / 5);
					OutOfMap();
				}
				else if ((log2_3 - 80 < s.frogX) && (log2_3 - 27 > s.frogX))
				{
					s.frogX = log2_3 - log_long_length / 5;
					OutOfMap();
				}
				else if ((log2_3 - 27 < s.frogX) && (log2_3 + 26 > s.frogX))
				{
					s.frogX = log2_3;
					OutOfMap();
				}
				else if ((log2_3 + 26 < s.frogX) && (log2_3 + 80 > s.frogX))
				{
					s.frogX = log2_3 + log_long_length / 5;
					OutOfMap();
				}
				else if ((log2_3 + 80 < s.frogX) && (log2_3 + 133 > s.frogX))
				{
					s.frogX = log2_3 + 2 * (log_long_length / 5);
					OutOfMap();
				}
				//4th one
				else if ((log2_4 - 133 < s.frogX) && (log2_4 - 80 > s.frogX))
				{
					s.frogX = log2_4 - 2 * (log_long_length / 5);
					OutOfMap();
				}
				else if ((log2_4 - 80 < s.frogX) && (log2_4 - 27 > s.frogX))
				{
					s.frogX = log2_4 - log_long_length / 5;
					OutOfMap();
				}
				else if ((log2_4 - 27 < s.frogX) && (log2_4 + 26 > s.frogX))
				{
					s.frogX = log2_4;
					OutOfMap();
				}
				else if ((log2_4 + 26 < s.frogX) && (log2_4 + 80 > s.frogX))
				{
					s.frogX = log2_4 + log_long_length / 5;
					OutOfMap();
				}
				else if ((log2_4 + 80 < s.frogX) && (log2_4 + 133 > s.frogX))
				{
					s.frogX = log2_4 + 2 * (log_long_length / 5);
					OutOfMap();
				}
				else
				{
					Reset();
					s.lives--;
				}
			}

			//player jumped on a 2nd line of turtles
			if (s.frogY == SCREEN_HEIGHT - 625)
			{
				if ((turtle2_1 - 80 < s.frogX) && (turtle2_1 - 27 > s.frogX))
					Attachleft(&turtle2_1,'z');
				else if ((turtle2_1 - 27 < s.frogX) && (turtle2_1 + 26 > s.frogX))
					Attach(&turtle2_1);
				else if ((turtle2_1 + 26 < s.frogX) && (turtle2_1 + 80 > s.frogX))
					Attachright(&turtle2_1,'z');
				else if ((turtle2_2 - 80 < s.frogX) && (turtle2_2 - 27 > s.frogX))
					Attachleft(&turtle2_2,'z');
				else if ((turtle2_2 - 27 < s.frogX) && (turtle2_2 + 26 > s.frogX))
					Attach(&turtle2_2);
				else if ((turtle2_2 + 26 < s.frogX) && (turtle2_2 + 80 > s.frogX))
					Attachright(&turtle2_2,'z');
				else if ((turtle2_3 - 80 < s.frogX) && (turtle2_3 - 27 > s.frogX))
					Attachleft(&turtle2_3,'z');
				else if ((turtle2_3 - 27 < s.frogX) && (turtle2_3 + 26 > s.frogX))
					Attach(&turtle2_3);
				else if ((turtle2_3 + 26 < s.frogX) && (turtle2_3 + 80 > s.frogX))
					Attachright(&turtle2_3,'z');
				else if ((turtle2_4 - 80 < s.frogX) && (turtle2_4 - 27 > s.frogX))
					Attachleft(&turtle2_4,'z');
				else if ((turtle2_4 - 27 < s.frogX) && (turtle2_4 + 26 > s.frogX))
					Attach(&turtle2_4);
				else if ((turtle2_4 + 26 < s.frogX) && (turtle2_4 + 80 > s.frogX))
					Attachright(&turtle2_4,'z');
				else
				{
					Reset();
					s.lives--;
				}
			}

			//player jumped on a 3rd line of logs
			else if (s.frogY == SCREEN_HEIGHT - 685)
			{
				if ((log3_1 - 67 < s.frogX) && (log3_1 - 22 > s.frogX))
					Attachleft(&log3_1, 'k');
				else if ((log3_1 - 22 < s.frogX) && (log3_1 + 22 > s.frogX))
					Attach(&log3_1);
				else if ((log3_1 + 22 < s.frogX) && (log3_1 + 67 > s.frogX))
					Attachright(&log3_1, 'k');
				else if ((log3_2 - 67 < s.frogX) && (log3_2 - 22 > s.frogX))
					Attachleft(&log3_2, 'k');
				else if ((log3_2 - 22 < s.frogX) && (log3_2 + 22 > s.frogX))
					Attach(&log3_2);
				else if ((log3_2 + 22 < s.frogX) && (log3_2 + 67 > s.frogX))
					Attachright(&log3_2, 'k');
				else if ((log3_3 - 67 < s.frogX) && (log3_3 - 22 > s.frogX))
					Attachleft(&log3_3, 'k');
				else if ((log3_3 - 22 < s.frogX) && (log3_3 + 22 > s.frogX))
					Attach(&log3_3);
				else if ((log3_3 + 22 < s.frogX) && (log3_3 + 67 > s.frogX))
					Attachright(&log3_3, 'k');
				else if ((log3_4 - 67 < s.frogX) && (log3_4 - 22 > s.frogX))
					Attachleft(&log3_4, 'k');
				else if ((log3_4 - 22 < s.frogX) && (log3_4 + 22 > s.frogX))
					Attach(&log3_4);
				else if ((log3_4 + 22 < s.frogX) && (log3_4 + 67 > s.frogX))
					Attachright(&log3_4, 'k');
				else
				{
					Reset();
					s.lives--;
				}
			}

			//logs are spawning
			SpawnRight(&log3_1, screen, log, SCREEN_HEIGHT - 685, logspeed3);
			SpawnRight(&log3_2, screen, log, SCREEN_HEIGHT - 685, logspeed3);
			SpawnRight(&log3_3, screen, log, SCREEN_HEIGHT - 685, logspeed3);
			SpawnRight(&log3_4, screen, log, SCREEN_HEIGHT - 685, logspeed3);

			//diving turtle
			if (turtle2_1 > 700 && turtle2_1 < 800)
				SpawnLeft(&turtle2_1, screen, diving_turtle, SCREEN_HEIGHT - 625, turtlespeed2);
			else if (turtle2_1 >= 600 && turtle2_1 <= 700)
			{
				Hit(&turtle2_1, SCREEN_HEIGHT - 625,'z');
				turtle2_1 -= turtlespeed2;
			}	
			else if (turtle2_1 >= 500 && turtle2_1<=600)
				SpawnLeft(&turtle2_1, screen, diving_turtle, SCREEN_HEIGHT - 625, turtlespeed2);
			else
				SpawnLeft(&turtle2_1, screen, turtle, SCREEN_HEIGHT - 625, turtlespeed2);

			//regular turtle
			SpawnLeft(&turtle2_2, screen, turtle, SCREEN_HEIGHT - 625, turtlespeed2);

			//diving turtle
			if (turtle2_3 > 500 && turtle2_3 < 600)
				SpawnLeft(&turtle2_3, screen, diving_turtle, SCREEN_HEIGHT - 625, turtlespeed2);
			else if (turtle2_3 >= 400 && turtle2_3 <= 500)
			{
				Hit(&turtle2_3, SCREEN_HEIGHT - 625, 'z');
				turtle2_3 -= turtlespeed2;
			}
			else if (turtle2_3 >= 300 && turtle2_3 <= 400)
				SpawnLeft(&turtle2_3, screen, diving_turtle, SCREEN_HEIGHT - 625, turtlespeed2);
			else
				SpawnLeft(&turtle2_3, screen, turtle, SCREEN_HEIGHT - 625, turtlespeed2);

			//regular turtle 
			SpawnLeft(&turtle2_4, screen, turtle, SCREEN_HEIGHT - 625, turtlespeed2);

			//logs are spawning
			SpawnRight(&log2_1, screen, log_long, SCREEN_HEIGHT - 565, logspeed2);
			SpawnRight(&log2_2, screen, log_long, SCREEN_HEIGHT - 565, logspeed2);
			SpawnRight(&log2_3, screen, log_long, SCREEN_HEIGHT - 565, logspeed2);
			SpawnRight(&log1_1, screen, log, SCREEN_HEIGHT - 505, logspeed1);
			SpawnRight(&log1_2, screen, log, SCREEN_HEIGHT - 505, logspeed1);
			SpawnRight(&log1_3, screen, log, SCREEN_HEIGHT - 505, logspeed1);
			SpawnRight(&log1_4, screen, log, SCREEN_HEIGHT - 505, logspeed1);

			//diving turtle
			if (turtle1_1 > 500 && turtle1_1 < 600)
				SpawnLeft(&turtle1_1, screen, diving_turtle, SCREEN_HEIGHT - 445, turtlespeed1);
			else if (turtle1_1 >= 400 && turtle1_1 <= 500)
			{
				Hit(&turtle1_1, SCREEN_HEIGHT - 445, 'z');
				turtle1_1 -= turtlespeed1;
			}
			else if (turtle1_1 >= 300 && turtle1_1 <= 400)
				SpawnLeft(&turtle1_1, screen, diving_turtle, SCREEN_HEIGHT - 445, turtlespeed1);
			else
				SpawnLeft(&turtle1_1, screen, turtle, SCREEN_HEIGHT - 445, turtlespeed1);

			//regular turtles
			SpawnLeft(&turtle1_2, screen, turtle, SCREEN_HEIGHT - 445, turtlespeed1);
			SpawnLeft(&turtle1_3, screen, turtle, SCREEN_HEIGHT - 445, turtlespeed1);

			//diving turtle
			if (turtle1_4 > 300 && turtle1_4 < 400)
				SpawnLeft(&turtle1_4, screen, diving_turtle, SCREEN_HEIGHT - 445, turtlespeed1);
			else if (turtle1_4 >= 200 && turtle1_4 <= 300)
			{
				Hit(&turtle1_4, SCREEN_HEIGHT - 445, 'z');
				turtle1_4 -= turtlespeed1;
			}
			else if (turtle1_4 >= 100 && turtle1_4 <= 200)
				SpawnLeft(&turtle1_4, screen, diving_turtle, SCREEN_HEIGHT - 445, turtlespeed1);
			else
				SpawnLeft(&turtle1_4, screen, turtle, SCREEN_HEIGHT - 445, turtlespeed1);
			
			//player hit a car
			Hit(&car1[0], SCREEN_HEIGHT - 85,'c');
			Hit(&car1[1], SCREEN_HEIGHT - 85, 'c');
			Hit(&car1[2], SCREEN_HEIGHT - 85, 'c');
			Hit(&car1[3], SCREEN_HEIGHT - 85, 'c');
			Hit(&car2[0], SCREEN_HEIGHT - 145, 'c');
			Hit(&car2[1], SCREEN_HEIGHT - 145, 'c');
			Hit(&car2[2], SCREEN_HEIGHT - 145, 'c');
			Hit(&car2[3], SCREEN_HEIGHT - 145, 'c');
			Hit(&car3[0], SCREEN_HEIGHT - 205, 'c');
			Hit(&car3[1], SCREEN_HEIGHT - 205, 'c');
			Hit(&car3[2], SCREEN_HEIGHT - 205, 'c');
			Hit(&car3[3], SCREEN_HEIGHT - 205, 'c');
			Hit(&car4[0], SCREEN_HEIGHT - 265, 'c');
			Hit(&car4[1], SCREEN_HEIGHT - 265, 'c');
			Hit(&car4[2], SCREEN_HEIGHT - 265, 'c');
			Hit(&car4[3], SCREEN_HEIGHT - 265, 'c');
			Hit(&car5[0], SCREEN_HEIGHT - 325, 'c');
			Hit(&car5[1], SCREEN_HEIGHT - 325, 'c');
			Hit(&car5[2], SCREEN_HEIGHT - 325, 'c');
			Hit(&car5[3], SCREEN_HEIGHT - 325, 'c');

			DrawSurface(screen, frog, s.frogX, s.frogY); //player frog spawning
			baby_frog_spawn(screen, baby_frog); //mechanics of baby frog

			//cars are spawning and moving
			SpawnLeft(&car5[0], screen, auto1, SCREEN_HEIGHT - 325, carspeed5);
			SpawnLeft(&car5[1], screen, auto1, SCREEN_HEIGHT - 325, carspeed5);
			SpawnLeft(&car5[2], screen, auto1, SCREEN_HEIGHT - 325, carspeed5);
			SpawnLeft(&car5[3], screen, auto1, SCREEN_HEIGHT - 325, carspeed5);
			SpawnRight(&car4[0], screen, auto2, SCREEN_HEIGHT - 265, carspeed4);
			SpawnRight(&car4[1], screen, auto2, SCREEN_HEIGHT - 265, carspeed4);
			SpawnRight(&car4[2], screen, auto2, SCREEN_HEIGHT - 265, carspeed4);
			SpawnRight(&car4[3], screen, auto2, SCREEN_HEIGHT - 265, carspeed4);
			SpawnLeft(&car3[0], screen, auto1, SCREEN_HEIGHT - 205, carspeed3);
			SpawnLeft(&car3[1], screen, auto1, SCREEN_HEIGHT - 205, carspeed3);
			SpawnLeft(&car3[2], screen, auto1, SCREEN_HEIGHT - 205, carspeed3);
			SpawnLeft(&car3[3], screen, auto1, SCREEN_HEIGHT - 205, carspeed3);
			SpawnRight(&car2[0], screen, auto2, SCREEN_HEIGHT - 145, carspeed2);
			SpawnRight(&car2[1], screen, auto2, SCREEN_HEIGHT - 145, carspeed2);
			SpawnRight(&car2[2], screen, auto2, SCREEN_HEIGHT - 145, carspeed2);
			SpawnRight(&car2[3], screen, auto2, SCREEN_HEIGHT - 145, carspeed2);
			SpawnLeft(&car1[0], screen, auto1, SCREEN_HEIGHT - 85, carspeed1);
			SpawnLeft(&car1[1], screen, auto1, SCREEN_HEIGHT - 85, carspeed1);
			SpawnLeft(&car1[2], screen, auto1, SCREEN_HEIGHT - 85, carspeed1);
			SpawnLeft(&car1[3], screen, auto1, SCREEN_HEIGHT - 85, carspeed1);

			//player reaches the nest with babyfrog, 
			if (s.frogY == SCREEN_HEIGHT - 745)
			{
				if ((s.frogX < SCREEN_WIDTH / 2 - 404) && (s.frogX > SCREEN_WIDTH / 2 - 456))
				{
					win_condition_1 = true;
					maxjump = 1000;
					if (s.bee_position == 0 && s.bee_available==true)
					{
						s.points += 200;
						s.bee_available = false;
					}
					if (s.babyfrog_taken == true)
					{
						s.babyfrog_available = false;
						s.points += 200;
					}
					s.points += 50 + 10 * (50 - s.worldTime);
					Reset();
				}
				else if ((s.frogX < SCREEN_WIDTH / 2 - 189) && (s.frogX > SCREEN_WIDTH / 2 - 243))
				{
					win_condition_2 = true;
					maxjump = 1000;
					if (s.bee_position == 1 && s.bee_available==true)
					{
						s.points += 200;
						s.bee_available = false;
					}
					if (s.babyfrog_taken == true)
					{
						s.babyfrog_available = false;
						s.points += 200;
					}
					s.points += 50 + 10 * (50 - s.worldTime);
					Reset();
				}
				else if ((s.frogX > SCREEN_WIDTH / 2 - 25) && (s.frogX < SCREEN_WIDTH / 2 + 26))
				{
					win_condition_3 = true;
					maxjump = 1000;
					if (s.bee_position == 2 && s.bee_available==true)
					{
						s.points += 200;
						s.bee_available = false;
					}
					if (s.babyfrog_taken == true)
					{
						s.babyfrog_available = false;
						s.points += 200;
					}
					s.points += 50 + 10 * (50 - s.worldTime);
					Reset();
				}
				else if ((s.frogX > SCREEN_WIDTH / 2 + 190) && (s.frogX < SCREEN_WIDTH / 2 + 245))
				{
					win_condition_4 = true;
					maxjump = 1000;
					if (s.bee_position == 3 && s.bee_available==true)
					{
						s.points += 200;
						s.bee_available = false;
					}
					if (s.babyfrog_taken == true)
					{
						s.babyfrog_available = false;
						s.points += 200;
					}
					s.points += 50 + 10 * (50 - s.worldTime);
					Reset();
				}
				else if ((s.frogX > SCREEN_WIDTH / 2 + 407) && (s.frogX < SCREEN_WIDTH / 2 + 460))
				{
					win_condition_5 = true;
					maxjump = 1000;
					if (s.bee_position == 4 && s.bee_available==true)
					{
						s.points += 200;
						s.bee_available = false;
					}
					if (s.babyfrog_taken == true)
					{
						s.points += 200;
						s.babyfrog_available = false;
					}	
					s.points += 50 + 10 * (50 - s.worldTime);
					Reset();
				}
				else
				{
					s.lives--;
					Reset();
				}
			}

			//displaying frog texture if player reaches one of the nests
			if (win_condition_1)
				DrawSurface(screen, frog, SCREEN_WIDTH / 2 - 435, SCREEN_HEIGHT - 745);
			if (win_condition_2)
				DrawSurface(screen, frog, SCREEN_WIDTH / 2 - 210, SCREEN_HEIGHT - 745);
			if (win_condition_3)
				DrawSurface(screen, frog, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 745);
			if (win_condition_4)
				DrawSurface(screen, frog, SCREEN_WIDTH / 2 + 210, SCREEN_HEIGHT - 745);
			if (win_condition_5)
				DrawSurface(screen, frog, SCREEN_WIDTH / 2 + 435, SCREEN_HEIGHT - 745);
			
			//showing doubled bonus if babyfrog and bee scored in the same place
			if (s.double_bonus_taken == false)
			{
				if (s.bee_position == 4 && win_condition_5 && s.babyfrog_taken)
					DisplayBonus(delta, text, "+400", screen, charset, 430, 'd');
				else if (s.bee_position == 3 && win_condition_4 && s.babyfrog_taken)
					DisplayBonus(delta, text, "+400", screen, charset, 220, 'd');
				else if (s.bee_position == 2 && win_condition_3 && s.babyfrog_taken)
					DisplayBonus(delta, text, "+400", screen, charset, 0, 'd');
				else if (s.bee_position == 1 && win_condition_2 && s.babyfrog_taken)
					DisplayBonus(delta, text, "+400", screen, charset, -220, 'd');
				else if (s.bee_position == 0 && win_condition_1 && s.babyfrog_taken)
					DisplayBonus(delta, text, "+400", screen, charset, -430, 'd');
			}
			//showing bonus +200 for catching the bee
			if (s.bee_bonus_taken == false)
			{
			if (s.bee_position == 4 && win_condition_5)
				DisplayBonus(delta, text, "+200", screen, charset, 430, 'b');
			else if (s.bee_position == 3 && win_condition_4)
				DisplayBonus(delta, text, "+200", screen, charset, 220, 'b');
			else if (s.bee_position == 2 && win_condition_3)
				DisplayBonus(delta, text, "+200", screen, charset, 0, 'b');
			else if (s.bee_position == 1 && win_condition_2)
				DisplayBonus(delta, text, "+200", screen, charset, -220, 'b');
			else if (s.bee_position == 0 && win_condition_1)
				DisplayBonus(delta, text, "+200", screen, charset, -430, 'b');
			}
			//showing bonus +200 points for bringing frog to the safety
			if (s.frog_bonus_taken == false)
			{
			 if (s.babyfrog_taken && win_condition_1)
				DisplayBonus(delta, text, "+200", screen, charset, -430, 'f');
			else if (s.babyfrog_taken && win_condition_2)
				DisplayBonus(delta, text, "+200", screen, charset, -220, 'f');
			else if (s.babyfrog_taken && win_condition_3)
				DisplayBonus(delta, text, "+200", screen, charset, 0, 'f');
			else if (s.babyfrog_taken && win_condition_4)
				DisplayBonus(delta, text, "+200", screen, charset, 220, 'f');
			else if (s.babyfrog_taken && win_condition_5)
				DisplayBonus(delta, text, "+200", screen, charset, 430, 'f');
			}
			
			DrawRectangle(screen, 0, 0, SCREEN_WIDTH, 40, fioletowy, fioletowy);

			//handling the timebar
			DrawRectangle(screen, SCREEN_WIDTH-150, SCREEN_HEIGHT - 12, s.timebar, 12, zielony, zielony);
			if (s.worldTime > 40)
				DrawRectangle(screen, SCREEN_WIDTH-150, SCREEN_HEIGHT - 12, s.timebar, 12, czerwony, czerwony);
			if (s.worldTime > 50)
			{
				Reset();
				s.lives--;
			}

			//display info labels
			sprintf(text, "Czas = %.1lf s  ", s.worldTime);
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2 + 440, SCREEN_HEIGHT - 10, text, charset);
			sprintf(text, "ilosc zyc = %d  ", s.lives);
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2 + 440, SCREEN_HEIGHT - 25, text, charset);
			sprintf(text, "ilosc punktow = %.1lf pkt  ", s.points);
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2 - 390, SCREEN_HEIGHT - 10, text, charset);

			//if player pasued the game set all speed variables to 0, and display pause label
			if (paused == 1)
			{
				DrawSurface(screen, pause, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
				carspeed1 = 0;
				carspeed2 = 0;
				carspeed3 = 0;
				carspeed4 = 0;
				carspeed5 = 0;
				logspeed1 = 0;
				logspeed2 = 0;
				logspeed3 = 0;
				turtlespeed1 = 0;
				turtlespeed2 = 0;
				jumpX = 0;
				jumpY = 0;
				s.timebar += 0;
			}
			else {
				carspeed1 = 1;
				carspeed2 = 1;
				carspeed3 = 1.5;
				carspeed4 = 1;
				carspeed5 = 1.8;
				logspeed1 = 0.4;
				logspeed2 = 0.7;
				logspeed3 = 0.5;
				turtlespeed1 = 0.4;
				turtlespeed2 = 0.4;
				s.worldTime += delta;
				jumpX = 55;
				jumpY = 60;
				s.timebar += 0.018;
			}

			//if player presses P,variable pausekey increments and depending on that pause the game
			if (pausedkey % 2 == 0)
				paused = 0;
			if (pausedkey % 2 == 1)
				paused = 1;

			//if player presses Q,display quitgame screen
			if (quitgame == 1)
			{
				paused = 1;
				if ((quitgameyes == 1) && (quitgameno == 0))
					DrawSurface(screen, quityes, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
				else if ((quitgameyes == 0) && (quitgameno == 1))
					DrawSurface(screen, quitno, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
				else
					DrawSurface(screen, quityes, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
			}

			//if player presses ENTER, display menu screen
			if (enter == 0)
			{
				DrawSurface(screen, menu, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
				paused = 1;
				quitgame = 0;
			}
			
			//if player loses all his lives, display points scored, and lose screen
			if (s.lives == 0)
			{
				DrawSurface(screen, lose, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
				sprintf(text, "ilosc punktow = %.2f pkt  ", s.points);
				DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT - 50, text, charset);
				paused = 1;
			}

			//if player wins display points scored, and win screen
			if (win_condition_1 && win_condition_2 && win_condition_3 && win_condition_4 && win_condition_5)
			{
				DrawSurface(screen, win, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
				sprintf(text, "ilosc punktow = %.2f pkt  ", s.points);
				DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT - 50, text, charset);
				for (int i = 0; i < letter; i++)
				{
					sprintf(text, "%c", player[i]);
					DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2 + (i*10), SCREEN_HEIGHT - 100, text, charset);
				}
				
				paused = 1;
			}
			if (highscoremenu % 2 == 1)
			{
				SDL_FillRect(screen, NULL, czarny);
				LoadHighScore(text,screen, charset);
			}
			//if player clicks spacebar(after wining or losing) this happens:
			if (space == 1)
			{
				Reset();
				s.double_bonus_taken = false;
				s.frog_bonus_taken = false;
				s.bee_bonus_taken = false;
				s.babyfrog_available = true;
				s.babyfrog_taken = false;
				s.bee_available = true;
				s.bee_position = rand() % 5;
				win_condition_1 = 0;
				win_condition_2 = 0;
				win_condition_3 = 0;
				win_condition_4 = 0;
				win_condition_5 = 0;
				maxjump = 1000;
				enter = 0;
				space = 0;
				s.lives = 5;
			}
			
			SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
			SDL_RenderCopy(renderer, scrtex, NULL, NULL);
			SDL_RenderPresent(renderer);

			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					if (win_condition_1 && win_condition_2 && win_condition_3 && win_condition_4 && win_condition_5)
					{
						if (event.key.keysym.sym != SDLK_SPACE)
							player[letter++] = char(event.key.keysym.sym);
						else {
							SaveHighScore(player);
							number_of_records++;
							space = 1;
						}
							
					}
					else if (event.key.keysym.sym == SDLK_ESCAPE)
						quit = 1;
					else if (event.key.keysym.sym == SDLK_p)
					{
						//game will pause now
						pausedkey++;
					}
					else if (event.key.keysym.sym == SDLK_q)
					{
						//quitgame window will now open
						quitgame=1;
					}
					else if (event.key.keysym.sym == SDLK_SPACE)
					{
						//player just left the win/lose screens
						space = 1;
						s.points = 0;
					}
					else if (event.key.keysym.sym == SDLK_h)
					{
						highscoremenu++;
					}
					else if (event.key.keysym.sym == SDLK_RETURN)
					{
						//player left the main menu, or chosed Y/N at the quitgame screen
						enter = 1;
						if (quitgame == 1)
						{
							if ((quitgameyes == 0) && (quitgameno == 1)) //NO
								enter = 1;
							if ((quitgameyes == 1) && (quitgameno == 0)) //YES
							{
								enter = 0;
								s.points = 0;
								s.lives = 5;
								Reset();
							}
							quitgame = 0;
						}
					}
					else if ((event.key.keysym.sym == SDLK_UP) || (event.key.keysym.sym == SDLK_w))
					{
						//player moves forward
						if (s.frogY > 105)
							s.frogY -= jumpY;
						if ((s.frogY < maxjump) && (paused==0))
						{
							//maxium round jump gives 10 points
							maxjump = s.frogY;
							s.points += 10;
						}
					}
					else if ((event.key.keysym.sym == SDLK_DOWN) || (event.key.keysym.sym == SDLK_s))
					{
						//player moves backwards
						if (s.frogY < SCREEN_HEIGHT - 60) //player cant go under the map
							s.frogY += jumpY;
					}
					else if ((event.key.keysym.sym == SDLK_RIGHT) || (event.key.keysym.sym == SDLK_d))
					{
						if (s.frogX < SCREEN_WIDTH - 60) //player cant go behind the map
							s.frogX += jumpX;
						if (quitgame == 1) //player chooses the quitgame menu Y/N options 
						{
							quitgameno = 1;
							quitgameyes = 0;
						}
					}
					else if ((event.key.keysym.sym == SDLK_LEFT) || (event.key.keysym.sym == SDLK_a))
					{
						if (s.frogX > 60) //player cant go behind the map
							s.frogX -= jumpX;
						if (quitgame == 1) //player chooses the quitgame menu Y/N options 
						{
							quitgameyes = 1;
							quitgameno = 0;
						}	
					}
					break;
				case SDL_KEYUP:
					break;
				case SDL_QUIT:
					quit = 1;
					break;
				};
			};
	};
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
	};
