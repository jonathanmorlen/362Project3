#ifndef ENGINE_H_
#define ENGINE_H_

#include <stdbool.h>

#define debug_mode 0 // run in debug mode or not

#define game_name "Elemental Maze by JPM"

//--------------------------- Configuration -----------------------------------

// player
#define starting_x 4                 // x coordinate of player's start location
#define starting_y ground_level - 3  // y coordinate of player's start location
#define starting_color magenta       // player color
#define death_penalty 20             // how much mana is lost on every death
#if debug_mode
    #define starting_health 100  // debug mode starting health
    #define starting_mana 9999    // debug mode starting mana
    #define starting_armor 9999   // debug mode starting armor
    #define starting_gold 999999  // debug mode starting gold
#else
    #define starting_health 100  // player starting health
    #define starting_mana 100    // player starting mana
    #define starting_armor 10    // player starting armor
    #define starting_gold 0      // player starting gold
#endif

// enemy
#define gold_value 10    // amount of gold for picking up $'s
#define enemy_bounty 2   // amount of gold for countering the enemy
#define enemy_damage 49  // amount of damage dealt when countering an enemy as the wrong color

// game data
#define TPS 8                        // ticks per second of game
#define screen_x 80                  // width of screen
#define screen_y 21                  // height of screen
#define ground_level (screen_y - 4)  // height of ground from bottom of screen
#define HUD_element_size 16          // 80 width / 5 elements = 16 chars per element
#define HUD_size 5                   // 80 width / 16 chars per element = 5 elements
#define level_gold 20

// power up prices
#define base_health_cost 110
#define health_cost_increase 5
#define base_mana_cost 200
#define mana_cost_increase 15
#define base_armor_cost 150
#define armor_cost_increase 20

// power up strengths
#define health_powerup 10
#define mana_powerup 10
#define armor_powerup 5

//------------------------------- Control -------------------------------------

#define TIM2_PRIORITY 3     // game ticks
#define TIM3_PRIORITY 2     // music beats
#define TIM6_PRIORITY 4     // music beat subdivisions
#define UART_PRIORITY 0     // terminal communication
#define DMA_PRIORITY  1     // music data transfer

//------------------------------ Colors ---------------------------------------

// basic colors
#define white 0xf0
#define teal 0xe0
#define magenta 0xd0
#define blue 0xc0
#define yellow 0xb0
#define green 0xa0
#define red 0x90
#define gray 0x80
#define black 0x00

// game colors
#define splash_color white
#define border_color white
#define level_color white
#define game_name_color magenta
#define text_color white
#define loss_color 0x09     // black on red
#define win_color 0x0a      // black on green

// enemy colors
#define fire_color red
#define water_color teal
#define grass_color green

// HUD colors
#define health_color red
#define mana_color teal
#define armor_color gray
#define gold_color yellow
#define music_color blue

#if debug_mode
    #define blank_color red
    #define blank_symbol '%'
#else
    #define blank_color black
    #define blank_symbol ' '
#endif

//--------------------------- Global Variables --------------------------------

extern char screen[screen_x][screen_y];    // characters
extern char color[screen_x][screen_y];     // colors
extern bool muted;

//--------------------------- Functions ---------------------------------------

// state control
void init();
void animate();
void render();
void update(char input);
void freeze();
void init_HUD();

// menus
void show_controls();
void show_paused();
void show_shop();

// display
void splash();
void msg(int x, int y, int color, const char *text);
void lose();
void next_level();
void init_level();

// utility
void update_HUD(const char* element, int value);
int get_seed();


#endif /* ENGINE_H_ */
