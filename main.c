#include <stdio.h>

#include "stm32f0xx.h"
#include "stm32f0_discovery.h"

#include "engine.h"     // game logic
#include "graphics.h"   // interfacing with terminal and drawing
#include "music.h"      // playing music

MIDI_Player *mp;
//extern const uint8_t midifile[];

void TIM2_IRQHandler(void)
{
    // Animate the game on every TIM2 update
    TIM2->SR &= ~0x1;
    // continuously replay music
    //if(mp ->nexttick >= MAXTICKS)
        //mp = midi_init(midifile);
    animate();
}

// game timer
void setup_timer2()
{
    // initialize TIM2 to run at TPS Hz
    RCC -> APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2 -> PSC = 1 - 1;
    TIM2 -> ARR = 48000000 / TPS - 1;

    // enable interrupt
    TIM2 -> DIER |= TIM_DIER_UIE;
    TIM2 -> CR1 |= TIM_CR1_CEN;
    NVIC_SetPriority(TIM2_IRQn, TIM2_PRIORITY);
}

int main(void)
{
    // prepare for initialization
    tty_init();
    raw_mode();
    cursor_off();
    setup_timer2();

    // initialization
    splash();
    render();

    // music
    init_sine();
    init_DAC();
    init_DMA();
    init_TIM6();

    //mp = midi_init(midifile);
    // The default rate for a MIDI file is 2 beats per second
    // with 48 ticks per beat.  That's 500000/48 microseconds.
    init_TIM3(10417);
    // run game
    for(;;)
    {
        asm("wfi");
    }
    return 0;
}
