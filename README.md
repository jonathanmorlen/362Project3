# 362Project3
Documentation

ECE 362 Project 3 Game with Music
Jonathan Morlen
jmorlen@purdue.edu


## Compilation

To compile this game, add the .c files to the "src" folder of a new eclipse 
project, add the .h files to the "inc" folder and then build and run. 

There are necessary project-wide variables and #define's in the .h files.

When compiling, go to 
Project > Properties > C/C++ Build > Settings > Optimization
and change the release optimization level to -Os to optimize for size

This game is 80 x 21 @ 230,400 Baud so adjust the terminal settings accordingly.

## Summary

This game is a top-down maze game, similar to Super Bomberman on the Super NES.
It has health, mana (a type of life force), armor, and gold elements from RPG 
games. It incorporates types and supereffectiveness from Pokemon games as well.

## Controls

* w - Up
* a - Left
* s - Down
* d - Right
* spacebar - change type

* p - Pause the game
* q - Quit the game
* m - Mute the music
* r - Show/Hide a condensed version of the hotkeys and controls breakdown
* e - Open shop to purchase powerups

## Game Elements

* 'x' - enemy of a random color, all equal probabilities (red, green, blue). 
      Counter the enemy by changing to the appropriate type.
* '$' - Gold in the maze, gives a set bounty and requires no type to pick up.
* '*' - Portal to the next level. Walk over to win the level, earn the level 
	  reward and teleport to the beginning of the next level, keeping all 
	  earned gold.

## Goal

The player's character is a magenta (configurable) pound sign (#) that starts 
in the bottom left corner of the maze

The goal of this game is to use the movement controls with the type changing to 
"counter" the type of the enemy, represented by an 'x', the player encounters by 
walking over.

   Fire (red) beats Grass (green)
Grass (green) beats Water (blue)
 Water (blue) beats Fire (red)
     
Use the spacebar to transition between the different types to counter the type
of the 'x' you are walking over. If successful the enemy will be countered and
the player will earn a gold bounty for doing so. This gold can then be spent at 
the shop (explained later).

If the player is unsuccessful at countering the enemy, the player will receive
damage mitigated by their armor rating. If the player's health reaches zero, 
the player is respawned at the beginning of the current level with full health,
but will lose a bit of mana. If the player's 
mana reaches zero, the game is over and the player will be respawned at the 
first level with 0 gold and default stats.

Once the player makes their way through the maze and into the portal, they are 
rewarded with a scaling level bounty of gold and are teleported to the next 
level's start area.

## Interface

**HUD:**
The heads up display (HUD) of the game is at the top of the screen. Here the 
player's health, mana, armor, and gold are displayed, as well as if the music
is muted or not.

**Controls:**
At the bottom of the screen is the space reserved for the in-game documentation.
Pressing 'r' will toggle the visibility of this documentation.

**Shop:**
Just above the in-game documentation, there is a shop. If the player presses 'e'
the shop will show/hide itself. If the shop is open, the player can purchase
powerups using the keys '1', '2', and '3' to buy it. Everytime the player buys
an item, the cost of it will go up. When purchasing items the bonus is immediate
(like a heal if purchasing health) as well as permanent for the game (increased 
maximum health on next spawn).

**Pause:**
If the player presses 'p' to pause the game, everything will stop and at the
bottom of the HUD, the word "PAUSED" will appear to give the player a visual
indication of the state of the game.

**Mute:**
If the player presses 'm' to toggle the mute function of the game's music,
the music element in the HUD will toggle on/off to give the player a visual 
indication of the state of the game's music.

**Quit:**
If the player presses 'q' to quit the game, all gold will be lost, health and 
mana reset, and the game returned to the splash screen, ready to be played 
again.

## Configuration

Almost everything about this game is configurable, for example

**Player:**
* Starting position
* Color
* Death penalty
* Starting stats (health, mana, armor, gold)
	
**Enemy:**
* $ value
* Bounty
* Damage
* Type colors
	
**Game Data:**
* Game ticks per second
* Screen size
* Game colors and symbols
	
**Shop:**
* Prices
* Price increases
* Powerup size
