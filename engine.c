#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "stm32f0xx.h"

#include "engine.h"
#include "graphics.h"

//------------------------- game data ------------------------------------------

// project-wide variables
char screen[screen_x][screen_y];    // characters
char color[screen_x][screen_y];     // colors

int player_x = starting_x;          // x coordinate of player
int player_y = starting_y;          // y coordinate of player
int player_color = starting_color;  // staring color of player character
int earned_gold = 0;

enum
{
    SPLASH,
    RUNNING
} phase = SPLASH;       // current phase of the game

#define wait_ticks 5    // number of ticks to wait before accepting input
int splash_ticks;       // number of ticks passed

int health_cost = base_health_cost;
int mana_cost = base_mana_cost;
int armor_cost = base_armor_cost;

// flags for determining menu state
bool muted = false;
int controls = 0;
bool paused = false;
bool shop = false;

//--------------------------------level data------------------------------------

#define num_levels 3
#define columns 80
#define rows 15
int level = 1;

const char levels[num_levels][rows][columns] =
{
        {   // level 1
                "+--------------+---------+----+----------------------------------+",
                "|              |              |          x   $    |    |$ $  x   |",
                "|           $  |     $   |    |    +-----------  -+    +----x   x|",
                "| x  |   x|    |    |    |    x    |              |            * |",
                "|    |x x |    |    |    |    |    |  x$ ---------+---------+    |",
                "|  $ | x  |     x   | $  |    +----+                        |    |",
                "|    |    +---------+----+    |         +---------+    |    |  $ |",
                "|    |        x         $     |    |    |       x |    |    |    |",
                "|    +---------+-  -----------+ x  |    |    |    |    |    |    |",
                "|x   |  $      |               x x |    |  $ |    |    |  $ |  x |",
                "|    |    |    +---------     |$$ x|    +----+         +----+    |",
                "|         |           $       | x$ |    x         |   x          |",
                "+---------+-------------------+----+--------------+--------------+"
        },

        {   // level 2
                "    $|           |    x    |        |$     |       |    | $ |        |$       ",
                "  ---+--   |x ---+--       +--      |      |   |   |                 |$    *  ",
                "        x  | x  $|    |    |     | x|   $   x  |   |  ------+   |    +--+- x  ",
                "  ---+-----+---  |    |  --+     |x +----------+   |x       | $ |       |     ",
                "    x|                |    |  ---+                 |  --+- -+---+---    |    x",
                "---x |  --------------+          |      x |    ----+    |  x         x  |---x ",
                "     |     x          +-------- -+--------+--   x       |   |   +----   |     ",
                "   --+--+--+--+  --+--+$    x    |        |   ---+   +--+- -+---+   $   |   --",
                "     x  |  |  |  x |  |      |     x |    |      |   |      |     |     |x x  ",
                "------     |  +---    |   ---+---    |       x   +---+--  --+-----+  ---+     ",
                "        |     |$      |      |       | $  |      |$         |    $|    $|  x  ",
                "  |  +--+--   |     | |    x |    ---+----+---+--++---+  ---+ -+--+ -+--+--   ",
                "  |$ |       -+--- -+-+----  |  |    |    | x |   |  $|     |  |     |        ",
                "$ |  |    |     x            |$ |       |   |   |   |$|  |   x    |     |     ",
                "--+--+----+------------------+--+-------+---+---+---+-+--+--------+-----+-----",
        },

        {   // level 3
                "+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+",
                "|$    x  |    $   |  x     x    $          x        $     $x   |",
                "+  +--+  +  +  +--+  +--+--+--+--+  +--+--+--+--+  +--+--+--+  +",
                "|     |   x |        |     |   x |      x       |     |      x |",
                "+--+  +  +--+  +--+--+  +  +  +  +  +--+--+--+  +--+  +  +--+--+",
                "|     |x |            $ |   $ |  |      $ |  |  |  x  |   $ |  |",
                "+  +--+  +  +--+  +--+--+--+--+  +--+  +--+--+  +--+--+--+  +--+-----------+",
                "|x |  $  |    x| $|    x     $   |  |  |     |     $  |  |    x  $$$$$$$$ *|",
                "+  +--+--+  +--+  +  +--+--+--+  +  +  +  +  +--+x +  +--+--+  +-----------+",
                "|        |  |     |  x  |   x |  |  |  $  |       x|      $    |",
                "+  +--+  +  +  +--+--+  +--+  +  +--+  +--+--+  +  +--+  +--+--+",
                "|  |  x  | x|  x     |        |   x     $    |x |         xx|  |",
                "+  +  +--+  +--+--+--+--+  +  +--+--+--+--+  + x+--+  +--+  +--+",
                "|$ |     x    $         $  |  $                  $ |  |  |   $ |",
                "+--+--+--------------------+--------------+--+--+--+--+--+--+--+"
        }

};

//-----------------------------------HUD----------------------------------------

// names of the HUD elements
const char* HUD_elements[HUD_size] =
{
        "Health",
        "Mana",
        "Armor",
        "Gold",
        "Music"
};

// initial values of the HUD elements
int HUD_values[HUD_size] =
{
        starting_health,    // Health
        starting_mana,      // Mana
        starting_armor,     // Armor
        starting_gold,      // Gold
        1                   // Music
};

// colors of the HUD elements
const int HUD_colors[HUD_size] =
{
        health_color,                        // color of the HUD elements
        mana_color,
        armor_color,
        gold_color,
        music_color
};

int* const health = &HUD_values[0];   //
int* const mana = &HUD_values[1];     //
int* const armor = &HUD_values[2];    // const pointers to all of the
int* const gold = &HUD_values[3];     // values of the HUD
int* const music = &HUD_values[4];    //

// initial bonuses to player stats
int bonus_health = 0;
int bonus_mana = 0;
int bonus_armor = 0;

// initialize game
void init()
{
    blank();
    draw_border(border_color);

    init_HUD();

    // draw character
    player_x = starting_x;
    player_y = starting_y;
    player_color = starting_color;
    draw_character(player_x, player_y, player_color);

    // draw level
    init_level();

    // randomly color enemies
    for(int y = 0; y < screen_y; y++)
    {
        for(int x = 0; x < screen_x; x++)
        {
            if(screen[x][y] == 'x')
            {
                // 1/3 chance for each color to appear
                int r = rand() % 30;
                if(r >= 0)
                    color[x][y] = fire_color;
                if(r >= 10)
                    color[x][y] = water_color;
                if(r >= 20)
                    color[x][y] = grass_color;
            }
        }
    }

    // show menus
    show_controls();
    show_shop();
    show_paused();
}

// find current level and draw it on the screen
void init_level()
{
    for(int i = 0; i < rows; i++)
    {
        msg(1, i + 3, level_color, levels[level - 1][i]);
    }
}

// checks controls flag to see which version of the
// in game documentation should be displayed
void show_controls()
{
    // show expanded controls documentation
    if(controls == 1)
    {
        shop = false;
        const char* text1 = "\'p\' - un/pause   \'q\' - quit game   \'m\' - toggle music   \'r\' - un/hide hotkeys ";
        const char* text2 = "wasd - movement    \'e\' - open shop   spacebar - change to next color  ";
        blank_section(1, ground_level + 1, screen_x - 2, 2);
        msg(1, screen_y - 3, text_color, text1);
        msg(1, screen_y - 2, text_color, text2);
    }

    // show the minimized controls documentation
    else if(controls == 0)
    {
        // clear expanded version and redraw affected parts of the border
        blank_section(1, ground_level, screen_x - 2, 5);
        draw_border(border_color);
        draw_HUD(border_color);

        // draw minimized version
        const char* text1 = "Press \'e\' to show shop";
        const char* text2 = "Press \'r\' to show controls";
        msg(1, screen_y - 3, text_color, text1);
        msg(1, screen_y - 2, text_color, text2);
    }

    // hide controls completely, only set by other menus
    else if(controls == -1)
    {
        blank_section(1, ground_level + 1, screen_x - 2, 2);
    }
}

// checks paused flag to see if the word "PAUSED" should be displayed at the
// bottom of the HUD or not
void show_paused()
{
    // show "PAUSED
    if(paused)
    {
        msg(screen_x / 2 - 3, 2, red, "PAUSED");
    }

    // show HUD border
    else
    {
        msg(screen_x / 2 - 3, 2, border_color,"------");
    }
}

void show_shop()
{
    // show shop menu
    if(shop)
    {
        controls = -1;
        blank_section(1, ground_level + 1, screen_x - 3, 2);
        char buffer[40];
        msg(2, ground_level + 1, text_color, "Item: (1) Health    (2) Mana    (3) Armor");
        sprintf(buffer,                      "Cost: $%d          $%d        $%d", health_cost, mana_cost, armor_cost);
        msg(2, ground_level + 2, text_color, buffer);
    }

    // hide shop menu
    else
    {
        controls = 0;
        blank_section(2, ground_level + 1, screen_x - 3, 2);
        show_controls();
    }
}

// initialize the HUD at the start of the game
void init_HUD()
{
    // draw all HUD element boxes
    draw_HUD(border_color);

    // populate all HUD elements with values
    for(int i = 0; i < HUD_size; i++)
        update_HUD(HUD_elements[i], HUD_values[i]);
}

// update an element of the HUD to the value specified
void update_HUD(const char* element, int value)
{
    // search for element index
    int i;
    for(i = 0; i < HUD_size; i++)
    {
        if(!strcasecmp(element, HUD_elements[i]))
        {
            break;
        }
    }

    // write new value for element
    char buffer[HUD_element_size];
    HUD_values[i] = value;

    if(i == 4)  // special case for music being on/off not numbers
        if(value) sprintf(buffer, "%s: ON", HUD_elements[i]);   // music on
        else sprintf(buffer, "%s: OFF", HUD_elements[i]);       // music off
    else
        sprintf(buffer, "%s: %d", HUD_elements[i], value);      // display value

    // create buffer of blank symbols
    char padded_buffer[HUD_element_size];
    memset(padded_buffer, blank_symbol, HUD_element_size);

    // split HUD element into the label and its value
    const char* space = strchr(buffer, ' ');
    space++;
    const int index = space - buffer;

    // move label to padded buffer
    memmove(padded_buffer, buffer, index - 1);

    // move value to padded buffer
    int value_length = strlen(buffer) - index;
    if(i == 4) { value_length++; }  // compensate for music being shorter
    memmove(&padded_buffer[HUD_element_size - 4 - value_length], &buffer[index], value_length);
    padded_buffer[HUD_element_size - 4] = '\0';

    // display HUD element
    msg(3 + (HUD_element_size * i), 1, HUD_colors[i], padded_buffer);
}

// display message when losing and reset
void lose()
{
    *mana -= death_penalty;

    // game loss
    if(*mana < 1)
    {
        msg(screen_x / 2 - 15, screen_y / 2 - 2, loss_color, "                          ");
        msg(screen_x / 2 - 15, screen_y / 2 - 1, loss_color, "         YOU LOSE         ");
        msg(screen_x / 2 - 15, screen_y / 2    , loss_color, "    Time to try again     ");
        msg(screen_x / 2 - 15, screen_y / 2 + 1, loss_color, " Press any key to restart ");
        msg(screen_x / 2 - 15, screen_y / 2 + 2, loss_color, "                          ");

        *mana = 0;
        update_HUD("mana", *mana);
        render();
        freeze();
        level = 1;
        earned_gold = 0;

        // reset stats
        *gold = starting_gold;
        update_HUD("gold", *gold);
        *mana = starting_mana + bonus_mana;
        bonus_health = 0;
        bonus_mana = 0;
        bonus_armor = 0;
    }

    // level loss
    else
    {
        msg(screen_x / 2 - 15, screen_y / 2 - 2, loss_color, "                          ");
        msg(screen_x / 2 - 15, screen_y / 2 - 1, loss_color, "      Out of health       ");
        msg(screen_x / 2 - 15, screen_y / 2    , loss_color, "                          ");
        msg(screen_x / 2 - 15, screen_y / 2 + 1, loss_color, " Press any key to respawn ");
        msg(screen_x / 2 - 15, screen_y / 2 + 2, loss_color, "                          ");

        render();
        freeze();
        update_HUD("mana", *mana);
    }
    *health = starting_health + bonus_health;
    update_HUD("health", *health);
    init();
}

// display message to transition between levels
void next_level()
{
    // game win
    if(level == num_levels)
    {
        earned_gold += level_gold * level;
        *gold += level_gold * level;
        msg(screen_x / 2 - 10, screen_y / 2 - 2, win_color, "                          ");
        msg(screen_x / 2 - 10, screen_y / 2 - 1, win_color, "         YOU WIN          ");
        char buffer[27];
        sprintf(buffer, "    Gold Earned: %*d", -9, earned_gold);
        msg(screen_x / 2 - 10, screen_y / 2    , win_color, buffer);
        msg(screen_x / 2 - 10, screen_y / 2 + 1, win_color, " Press any key to replay  ");
        msg(screen_x / 2 - 10, screen_y / 2 + 2, win_color, "                          ");
        level = 1;
        update_HUD("gold", *gold);
        render();
        freeze();

        // reset stats
        *gold = 0;
        update_HUD("gold", *gold);
        *health = starting_health;
        update_HUD("health", *health);
        *mana = starting_mana;
        update_HUD("mana", *mana);
        *armor = starting_armor;
        update_HUD("armor", *armor);

        bonus_health = 0;
        bonus_mana = 0;
        bonus_armor = 0;

        init();
    }

    // level win
    else
    {
        controls = 0;
        msg(screen_x / 2 - 10, screen_y / 2 - 2, win_color, "                          ");
        msg(screen_x / 2 - 10, screen_y / 2 - 1, win_color, "     Congratulations      ");
        msg(screen_x / 2 - 10, screen_y / 2    , win_color, "   On to the next level   ");
        msg(screen_x / 2 - 10, screen_y / 2 + 1, win_color, " Press any key to proceed ");
        msg(screen_x / 2 - 10, screen_y / 2 + 2, win_color, "                          ");

        earned_gold += level_gold * level;
        *gold += level_gold * level;
        level++;

        update_HUD("gold", *gold);
        render();
        freeze();
        init();
    }
}

// used for seeding RNG
int get_seed(void)
{
    return TIM2->CNT;
}

// display the initial splash screen
void splash(void)
{
    blank();
    int left_side = 2;
    msg(left_side, 1,  white, " ________ __                                           __              __ ");
    msg(left_side, 2,  white, "/        /  |                                         /  |            /  |");
    msg(left_side, 3,  white, "$$$$$$$$/$$ | ______  _____  ____   ______  _______  _$$ |_    ______ $$ |");
    msg(left_side, 4,  white, "$$ |__   $$ |/      \\/     \\/    \\ /      \\/       \\/ $$   |  /      \\$$ |");
    msg(left_side, 5,  white, "$$    |  $$ /$$$$$$  $$$$$$ $$$$  /$$$$$$  $$$$$$$  $$$$$$/   $$$$$$  $$ |");
    msg(left_side, 6,  white, "$$$$$/   $$ $$    $$ $$ | $$ | $$ $$    $$ $$ |  $$ | $$ | __ /    $$ $$ |");
    msg(left_side, 7,  white, "$$ |_____$$ $$$$$$$$/$$ | $$ | $$ $$$$$$$$/$$ |  $$ | $$ |/  /$$$$$$$ $$ |");
    msg(left_side, 8,  white, "$$       $$ $$       $$ | $$ | $$ $$       $$ |  $$ | $$  $$/$$    $$ $$ |");
    msg(left_side, 9,  white, "$$$$$$$$/$$/ $$$$$$$/$$/  $$/  $$/ $$$$$$$/$$/   $$/   $$$$/  $$$$$$$/$$/ ");
    msg(left_side, 10, white, " __       __                            ");
    msg(left_side, 11, white, "/  \\     /  |                           ");
    msg(left_side, 12, white, "$$  \\   /$$ | ______  ________  ______  ");
    msg(left_side, 13, white, "$$$  \\ /$$$ |/      \\/        |/      \\ ");
    msg(left_side, 14, white, "$$$$  /$$$$ |$$$$$$  $$$$$$$$//$$$$$$  |");
    msg(left_side, 15, white, "$$ $$ $$/$$ |/    $$ | /  $$/ $$    $$ |");
    msg(left_side, 16, white, "$$ |$$$/ $$ /$$$$$$$ |/$$$$/__$$$$$$$$/ ");
    msg(left_side, 17, white, "$$ | $/  $$ $$    $$ /$$      $$       |");
    msg(left_side, 18, white, "$$/      $$/ $$$$$$$/$$$$$$$$/ $$$$$$$/ ");
    draw_box(0, 0, screen_x, screen_y, border_color);

    msg(42, 12, splash_color, "Change color to counter the enemy x's");

    // list different counters in appropriate colors
    msg(53, 14, fire_color,          "fire");
    msg(58, 14, splash_color,             "beats");
    msg(64, 14, grass_color,                    "grass");

    msg(52, 15, grass_color,        "grass");
    msg(58, 15, splash_color,             "beats");
    msg(64, 15, water_color,                    "water");

    msg(52, 16, water_color,        "water");
    msg(58, 16, splash_color,             "beats");
    msg(64, 16, fire_color,                     "fire");

    msg(42, 18, splash_color, "       Press any key to begin!      ");
}

// disable TIM2, wait for a key press
void freeze(void)
{
    TIM2->CR1 &= ~TIM_CR1_CEN;
    while(!available());
    char c = getchar();
    TIM2->CR1 |= TIM_CR1_CEN;
    if(c == 'm' || c == 'r' || c == 'e')
    {
        if(c == 'm')
        {
            if(muted)
                DAC->CR |= DAC_CR_EN1;
            else
                DAC->CR &= ~DAC_CR_EN1;
            muted = !muted;
            *(music) ^= 0b1;
            update_HUD("Music", *music);
        }
        if(c == 'r')
        {
            controls = !controls;
            show_controls();
        }
        if(c == 'e')
        {
            shop = !shop;
            show_shop();
        }
        render();
    }
}

// dump the screen and color arrays to the terminal screen
void render(void)
{
    home();
    int col = color[0][0];
    fgbg(col);
    for(int y = 0; y < screen_y; y++)
    {
        setpos(0, y);
        for(int x = 0; x < screen_x; x++)
        {
            if(screen[x][y] == '$')
                color[x][y] = gold_color;
            if (color[x][y] != col)
            {
                col = color[x][y];
                fgbg(col);
            }
            putchar(screen[x][y]);
        }
    }
    fflush(stdout);
}

// print a color message at screen coordinate (x,y)
void msg(int x, int y, int c, const char *text)
{
    int len = strlen(text);
    for (int n = 0; (n < len) && (n < screen_x); n++)
    {
        screen[x + n][y] = text[n];
        color[x + n][y] = c;
    }
}

// updates game based on player input
void update(char input)
{
    if(phase == RUNNING)
    {
        // save previous position just in case reversion is needed
        int prev_x = player_x;
        int prev_y = player_y;

        // move in a direction with wasd
        switch(input)
        {
        case 'd':
            player_x++;
            break;
        case 'w':
            player_y--;
            break;
        case 'a':
            player_x--;
            break;
        case 's':
            player_y++;
            break;
        }

        // prevent phasing through walls
        if((screen[player_x][player_y] == '-') || (screen[player_x][player_y] == '+') || (screen[player_x][player_y] == '|'))
        {
            player_x = prev_x;
            player_y = prev_y;
        }

        // limit player movement to playable area
        if(player_x > screen_x - 2)
            player_x = screen_x - 2;
        if(player_x < 1)
            player_x = 1;
        if(player_y > ground_level - 1)
            player_y = ground_level - 1;
        if(player_y < 3)
            player_y = 3;

        // check to see what was stepped on
        if(screen[player_x][player_y] == '$')
        {
            *gold += gold_value;
            earned_gold += gold_value;
            update_HUD("gold", *gold);
        }

        // encountering enemies
        if(screen[player_x][player_y] == 'x')
        {
            // if player is the wrong color
            if(((color[player_x][player_y] == fire_color)   && (player_color != water_color))
            || ((color[player_x][player_y] == water_color)  && (player_color != grass_color))
            || ((color[player_x][player_y] == grass_color)  && (player_color != fire_color)))
            {
                // damage reduced by armor, prevent negative damage from healing
                // damage is always at least 1
                int damage = enemy_damage - *armor;
                if(damage < 0)
                    damage = 1;
                *health -= damage;
                if(*health < 1)
                {
                    *health = 0;
                    update_HUD("health", *health);
                    lose();
                    return;
                }
                update_HUD("health", *health);
            }

            // give bounty to player
            else
            {
                *gold += enemy_bounty;
                earned_gold += enemy_bounty;
                update_HUD("gold", *gold);
            }
        }

        // if player has reached the portal to the next level
        if(screen[player_x][player_y] == '*')
            next_level();

        // if nothing special has been stepped on
        else
        {
            // remove previously drawn character
            msg(prev_x, prev_y, blank_color, " ");
            draw_character(player_x, player_y, player_color);
            prev_x = player_x;
            prev_y = player_y;
        }
    }
}

// controls game phase flow
void animate(void)
{
    if (phase == SPLASH)
    {
        // burn ticks and discard input for wait_ticks ticks
        if (splash_ticks < wait_ticks)
        {
            while (available())
            {
                getchar();
            }
            splash_ticks++;
            return;
        }

        // wait for new input to begin another game
        while (!available());
        getchar();

        // seed RNG
        int seed = get_seed();
        srandom(seed);

        // start new game
        init();
        phase = RUNNING;
    }
    if (phase == RUNNING)
    {
        char in = '/';  // burner value
        while (available())
        {
            in = getchar();
        }

        // quit and go to splash screen
        if (in == 'q')
        {
            // reset level
            level = 1;
            earned_gold = 0;

            // reset stats
            *gold = starting_gold;
            update_HUD("gold", *gold);
            *health = starting_health;
            update_HUD("health", *health);
            *mana = starting_mana;
            update_HUD("mana", *mana);
            *armor = starting_armor;
            update_HUD("armor", *armor);

            // display changes
            splash();
            render();
            phase = SPLASH;
            splash_ticks = 0;
        }

        // change color
        if(in == ' ')
        {
            // change color start -> r -> g -> b
            //                       ^_________|
            if(player_color == starting_color)
                player_color = fire_color;
            else if(player_color == fire_color)
                player_color = grass_color;
            else if(player_color == grass_color)
                player_color = water_color;
            else if(player_color == water_color)
                player_color = fire_color;
        }

        // pause game
        if (in == 'p')
        {
            paused = !paused;
            show_paused();
            render();
            freeze();
            paused = !paused;
            show_paused();
            render();
        }

        // toggle controls
        if(in == 'r')
        {
            controls = !controls;
            show_controls();
        }

        // mute music
        if(in == 'm')
        {
            // toggle
            if(muted)
                DAC->CR |= DAC_CR_EN1;
            else
                DAC->CR &= ~DAC_CR_EN1;
            muted = !muted;
            *(music) ^= 0b1;
            update_HUD("Music", *music);

        }
        if(in == 'e')
        {
            shop = !shop;
            show_shop();
        }
        if(shop)
        {
            // buy health
            if(in == '1')
            {
                if(*gold >= health_cost)
                {
                    *gold -= health_cost;
                    health_cost += health_cost_increase;
                    update_HUD("gold", *gold);
                    *health += health_powerup;
                    bonus_health += health_powerup;
                    update_HUD("health", *health);
                }
            }

            // buy mana
            else if(in == '2')
            {
                if(*gold >= mana_cost)
                {
                    *gold -= mana_cost;
                    mana_cost += mana_cost_increase;
                    update_HUD("gold", *gold);
                    *mana += mana_powerup;
                    bonus_mana += mana_powerup;
                    update_HUD("mana", *mana);
                }
            }

            // buy armor
            else if(in == '3')
            {
                if(*gold >= armor_cost)
                {
                    *gold -= armor_cost;
                    armor_cost += armor_cost_increase;
                    update_HUD("gold", *gold);
                    *armor += armor_powerup;
                    bonus_armor += armor_powerup;
                    update_HUD("armor", *armor);
                }
            }
        }
        update(in);
        render();
        return;
    }
}
