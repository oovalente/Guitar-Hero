#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

#define MAXTILES 5
#define AVG_TILES_PER_LANE 20
#define MAXLANES 6
#define MIN_SCORE -10*NUM_LANES

#define CREATED 0
#define HIT -1
#define ACTIVE 1
#define LIMBO -2



const int FPS = 180;  //180 //velocidade
const int SCREEN_W = 960;
const int SCREEN_H = 640;

int SKY_H;
int TILE_HIT_H;
int TILE_R = 50;

int LANE_W;
int NUM_LANES=3;

int TRACK_W;
int TRACK_LEFT_X;
int TRACK_RIGHT_X;
int KEY1 = ALLEGRO_KEY_1;
int funct;


int score = 0;
float relogio = 0;
int Test;


typedef struct Tile {
	float x,y; //x center, y bottom 
	float dx, dy;
	int lane; 
	int status;
	int delay;
	int id;
	ALLEGRO_COLOR color1;
	ALLEGRO_COLOR color2;
	ALLEGRO_COLOR color3;
} Tile;

void createTile(Tile *T){
	T->lane = rand()%NUM_LANES;
	T->x = TRACK_LEFT_X + LANE_W/2 + LANE_W * T->lane;
	T->y = SKY_H + TILE_R;
	T->status = LIMBO;
	T->delay = rand()%30; //120
	T->color1= al_map_rgb(173,158,8);
	T->color2= al_map_rgb(20,15,128);
	T->color3= al_map_rgb(128,19,15);	
}

void updateTile(Tile *T){
	
	if(T->y < SCREEN_H + LANE_W || T->y <0)
		T->y = T->y +1;
	if(T->y >SCREEN_H*0.90 && T->y< SCREEN_H*0.9016){
		Test=1;
	}
}

 void drawTile1(Tile *T){
		al_draw_filled_circle(T->x, T->y, TILE_R,T->color1); 
	 
 }
 
  void drawTile2(Tile *T){
	 al_draw_filled_circle(T->x, T->y, TILE_R,T-> color2);
 }
 
 void drawTile3(Tile *T){
	 al_draw_filled_circle(T->x, T->y, TILE_R,T->color3);
 }
 
void createMusic(Tile music[], int n){
	int i;
	for(i=0;i < n; i++)
		createTile(&music[i]);
}

void updateStatus(Tile music[], int n){
	int i;
	for(i = 0; i < n; i++){
		if(music[i].delay < relogio){
			music[i].status = ACTIVE;
		}
	}
}

void updateMusic(Tile music[], int n){
	int i;
	for(i = 0; i < n; i++)
		if(music[i].status== ACTIVE)
			updateTile(&music[i]);
}

 void drawMusic(Tile music[], int n){
	 int i;
	 for(i = 0; i < n; i++){
		 if(music[i].status == ACTIVE && music[i].x == TRACK_LEFT_X + LANE_W/2 )
			drawTile1(&music[i]);
		if(music[i].status == ACTIVE && music[i].x == TRACK_LEFT_X + LANE_W/2 + LANE_W )
			drawTile2(&music[i]);
		if(music[i].status == ACTIVE && music[i].x == TRACK_LEFT_X + LANE_W/2 + 2*LANE_W )
			drawTile3(&music[i]);
	}
 }

void init_track() {
	SKY_H = SCREEN_H * 0.25;
	LANE_W = (2*TILE_R) * 1.5;
	TRACK_W = NUM_LANES * LANE_W;
	TRACK_LEFT_X = SCREEN_W/2 - TRACK_W/2;
	TRACK_RIGHT_X = TRACK_LEFT_X + TRACK_W;
}

void draw_scenario(ALLEGRO_DISPLAY *display) {
	
	int i;
	
	
	//screen
	ALLEGRO_COLOR BKG_COLOR = al_map_rgb(15,15,15); 
	//transformar em var. global
	al_set_target_bitmap(al_get_backbuffer(display));
	al_clear_to_color(BKG_COLOR);   
	
	//sky
	al_draw_filled_rectangle(0, 0, SCREEN_W, SKY_H, al_map_rgb(0,0,0));
	
	//desenha faixa da nota
	al_draw_filled_rectangle(TRACK_LEFT_X, SKY_H, SCREEN_W - TRACK_LEFT_X, SCREEN_H, al_map_rgb(27,29,32));
	
	// desenha linha critica
	al_draw_line(TRACK_LEFT_X, SCREEN_H - (2*TILE_R), TRACK_RIGHT_X, SCREEN_H - (2*TILE_R), al_map_rgb(25,92,25), 2);  //desenha (x inicial, y inicial, x final, y final)
  
   //desenha a pista:
   al_draw_line(TRACK_LEFT_X, SKY_H, TRACK_LEFT_X, SCREEN_H, al_map_rgb(255,255,255), 10); 
   al_draw_line(TRACK_RIGHT_X, SKY_H, TRACK_RIGHT_X, SCREEN_H, al_map_rgb(255,255,255), 10); 
   for(i=1; i<NUM_LANES; i++) {
	   al_draw_line(TRACK_LEFT_X+(LANE_W*i), SKY_H, TRACK_LEFT_X+(LANE_W*i), SCREEN_H, al_map_rgb(255,255,255), 3); 
   }
   
}

int main(int argc, char **argv){
	srand(time(NULL));
	al_init_image_addon();

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;

	int i=0;

	char my_score[10];
	char msn[25];
 
	//----------------------- rotinas de inicializacao ---------------------------------------
   if(!al_init()) {
      fprintf(stderr, "failed to initialize allegro!\n");
      return -1;
   }
   
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }
 
   timer = al_create_timer(1.0 / FPS);
   if(!timer) {
      fprintf(stderr, "failed to create timer!\n");
      return -1;
   }
 
   display = al_create_display(SCREEN_W, SCREEN_H);
   if(!display) {
      fprintf(stderr, "failed to create display!\n");
      al_destroy_timer(timer);
      return -1;
   }


	//inicializa o modulo allegro que carrega as fontes
    al_init_font_addon();
	//inicializa o modulo allegro que entende arquivos tff de fontes
    al_init_ttf_addon();


	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 48, 1);  
	ALLEGRO_FONT *size_64 = al_load_font("hero.ttf", 53, 1);
	

	event_queue = al_create_event_queue();
   if(!event_queue) {
      fprintf(stderr, "failed to create event_queue!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
   }
   
   al_install_keyboard();
   
	//registra na fila de eventos que eu quero identificar quando a tela foi alterada
   al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila de eventos que eu quero identificar quando o tempo alterou de t para t+1
   al_register_event_source(event_queue, al_get_timer_event_source(timer));
   
   al_register_event_source(event_queue, al_get_keyboard_event_source());   
   
  

	//reinicializa a tela
   al_flip_display();
	//inicia o temporizador
   al_start_timer(timer);

	init_track();
	//Tile bota;
	//createTile(&bota);
    int w = 30;
	Tile nota[w];
    createMusic(nota, w);
	
	
   int playing = 1;
   //enquanto playing for verdadeiro, faca:
   while(playing) {
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);
	  
		if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			printf("\n%d", ev.keyboard.keycode);
			for(i = 0 ; i< w; i++){
			switch(ev.keyboard.keycode){
				
				case ALLEGRO_KEY_Q:
				//
				if(Test == 1){
					score = score +16;
					printf("\a");
					printf("\nTECLA Q?");
					Test = 0;
				}
				else{
				al_stop_timer(timer);
				al_draw_circle(TRACK_LEFT_X + LANE_W/2, SCREEN_H - TILE_R, TILE_R, al_map_rgb(173,158,8),5);
				al_flip_display();
				al_rest(0.0003);
				al_start_timer(timer);
				}
				break;
				
				case ALLEGRO_KEY_W:
				if(Test == 1){
					score = score +16;
					printf("\a");
					printf("\nTECLA W?");
					Test = 0;
				}
				else{
					al_stop_timer(timer);
					al_draw_circle(TRACK_LEFT_X + LANE_W/2+ LANE_W, SCREEN_H - TILE_R, TILE_R,al_map_rgb(20,15,128),5);
					al_flip_display();
					al_rest(0.0003);
					al_start_timer(timer);
				}
				break;
				
				case ALLEGRO_KEY_E:
				if(Test == 1){
					score = score +16;
					printf("\a");
					printf("\nTECLA E?");
					Test = 0;
				}
				else{
					al_stop_timer(timer);
					al_draw_circle(TRACK_LEFT_X + LANE_W/2+ 2*LANE_W, SCREEN_H - TILE_R, TILE_R, al_map_rgb(128,19,15),5);
					al_flip_display();
					al_rest(0.0003);
					al_start_timer(timer);
				}
				break;
	
				default:
					score --;
			}
			}
			score = score -4;
		}
				
		if(ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE || score < -64) { //NAO alcancou a pontuacao minima encerra o jogo
				playing = 0;
			}
			
		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		else if(ev.type == ALLEGRO_EVENT_TIMER) {
			draw_scenario(display);
	
			relogio += 1.0/FPS;
			
			//updateTile(&bota);
			//drawTile(&bota);
			updateStatus(nota, w);
			updateMusic(nota, w);
			drawMusic(nota, w);
		
			
			//SCORE
			sprintf(my_score, "%d", score);
			al_draw_text(size_32, al_map_rgb(71, 0, 230), SCREEN_W-100, 20, 0, my_score);
			
			//MENSAGEM
			sprintf(msn, "GUITAR HERO", funct);
			al_draw_text(size_64, al_map_rgb(255, 255, 255), SCREEN_H*0.55, 25, 0, msn);
			
			//MENSAGEM
			sprintf(msn, "ENDGAME", funct);
			al_draw_text(size_64, al_map_rgb(71, 0, 230), SCREEN_H*0.55, 95, 0, msn);
			
			
			/*//MENSAGEM
			sprintf(msn, "ajude  thanos  passar  na  matéria  de  PDS , ganhe  o  jogo  para  conseguir  as  joias  do  infinito  e  trazer  equilibrio  a  grade", funct);
			al_draw_text(size_28, al_map_rgb(255, 255, 255), TRACK_LEFT_X*0.2, SKY_H*1.1, 0, msn);
			//reinicializo a tela
			 al_flip_display();*/
		
			 al_flip_display();
			
			// FECHAR JOGO
			if(i == w*50){
				playing = 0;
			}	
			i++;
		}
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE ) {
			playing = 0;
		}
	}
//fim do while
     
	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	
	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	
	al_rest(1);
	
	char my_text[20];
	
	//colore toda a tela de preto
	al_clear_to_color(al_map_rgb(0,0,0));
	sprintf(my_text, "Score: %d", score);
	al_draw_text(size_32, al_map_rgb(200, 0, 30), SCREEN_W/3, SCREEN_H/2, 0, my_text);
	
	
	char buf[5];//tam max da strg
	int record;
	int aux;
		 
	FILE *arquivo;
	arquivo	= fopen ("pontuacao.txt", "r");
	fgets(buf, 5, arquivo);
	
	record = atoi(strtok(buf, " "));
	printf("\nrecord =%d", record);
	fclose(arquivo);
	
	if(record > score){
		sprintf(buf, "record: %d", record);
		al_draw_text(size_32, al_map_rgb(200, 200, 0), SCREEN_W/3, SCREEN_H/3, 0, buf);
	}
	
	else if(record < score){
		sprintf(buf, "record: %d", score);
		al_draw_text(size_32, al_map_rgb(200, 200, 0), SCREEN_W/3, SCREEN_H/3, 0, buf);	
		
	FILE *arquivo;
		arquivo	= fopen ("pontuacao.txt", "w");
		fgets(buf, 5, arquivo);
	
		record = atoi(strtok(buf, " "));
		printf("\nrecord =%d", record);
		
		fprintf(arquivo, "%d", score);
		//arquivo de novo
		fclose(arquivo);
	}
	
	//reinicializa a tela
	al_flip_display();	
    al_rest(2);	

   al_destroy_timer(timer);
   al_destroy_display(display);
   al_destroy_event_queue(event_queue);
   
   return 0;
}