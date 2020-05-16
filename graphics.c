#include <stdio.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "stm32f0xx.h"
#include "stm32f0_discovery.h"

#include "graphics.h"
#include "engine.h"

//------------------------------- DRAWING/COLOR -------------------------------

// fills the screen with black spaces, makes screen "blank"
void blank()
{
    blank_section(0, 0, screen_x, screen_y);
}

// "blank" a section of the screen starting
// from x1, x2, size of w x h
void blank_section(int x1, int y1, int w, int h)
{
    color_section(x1, y1, w, h, blank_symbol, blank_color);
}

// fills a w x h section with top left corner
// at (x1,y1) with a c colored s symbol
void color_section(int x1, int y1, int w, int h, int s, int c)
{
    for(int y = y1; y < y1 + h && y < screen_y; y++)
    {
        for(int x = x1; x < x1 + w && x < screen_x; x++)
        {
            screen[x][y] = s;
            color[x][y] = c;
        }
    }
}

// draw a box where:
// the top left corner is at (x,y)
// has a width (x) of w and a height (y) of h
// the color of the box is c
void draw_box(int x, int y, int w, int h, int c)
{
    // top and bottom of box
    draw_hline(x, y, w, '-', c);
    draw_hline(x, y + h - 1, w, '-', c);

    draw_vline(x, y, h, '|', c);
    draw_vline(x + w - 1, y, h, '|', c);

    // corners
    screen[x][y] = '+';                 // top left
    screen[x + w - 1][y] = '+';         // top right
    screen[x][y + h - 1] = '+';         // bottom left
    screen[x + w - 1][y + h - 1] = '+'; // bottom right
    color[x + w - 1][y + h - 1] = c;
}

// draw a horizontal line of c colored s symbols length l from (x,y)
void draw_hline(int x, int y, int l, int s, int c)
{
    for(int i = x; i < x + l && i < screen_x; i++)
    {
        screen[i][y] = s;
        color[i][y] = c;
    }
}

// draw a vertical line of c colored s symbols length l from (x,y)
void draw_vline(int x, int y, int l, int s, int c)
{
    for(int i = y; i < y + l && i < screen_y; i++)
    {
        screen[x][i] = s;
        color[x][i] = c;
    }
}

// draw the border of the game
void draw_border(int c)
{
    draw_box(0, 0, screen_x, screen_y, c);
    draw_box(0, screen_y - 4, screen_x, 4, border_color);
    msg(screen_x / 2 - 10, 0, game_name_color, game_name);
}

// draws the heads up display in color c
void draw_HUD(int c)
{
    for(int i = 0; i * HUD_element_size < screen_x; i++)
    {
        if(i == 4){ draw_box(i * HUD_element_size, 0, HUD_element_size, 3, c); }
        else{ draw_box(i * HUD_element_size, 0, HUD_element_size + 1, 3, c); }
    }
    msg(screen_x / 2 - 10, 0, game_name_color, game_name);
}

// draw player at coordinates (x,y) in color c
void draw_character(int x, int y, int c)
{
    msg(x, y, c, "#");
}

//----------------------------------- SCREEN ---------------------------------

// CSI is the "Control Sequence Introducer" (Escape left-square-bracket)
// The terminal recognizes this as the start of a cursor control sequence
#define CSI "\e["

// Print a string with no newline.
static void putstr(const char *s)
{
    while(*s)
        putchar(*s++);
}

// Print an integer.
static void putint(int x) {
        char buf[20];
        int off;
        int sign = 1;
        if (x < 0) {
                sign = -1;
                x = -x;
        }
        for(off=0; off < 18; off++) {
                buf[off] = (x % 10) + '0';
                x = x / 10;
                if (x == 0)
                        break;
        }
        if (sign == -1) {
                buf[++off] = '-';
        }
        for(     ; off >= 0; off--)
                putchar(buf[off]);
}

// Put the cursor in the upper left.
void home()
{
  putstr(CSI "1;1H");
}

// Clear the screen.
void clear()
{
  home();
  fgbg(0xf0);
  int x;
  for(x=0; x<4000; x++)
    putchar(' ');
}

// Set the cursor position to (x,y) [zero-based indexing]
void setpos(int x, int y)
{
  putstr(CSI); putint(y+1); putstr(";"); putint(x+1); putstr("H");
}

// Make the cursor invisible.
void cursor_off()
{
  putstr(CSI "?25l");
}

// Make the cursor visible.
void cursor_on()
{
  putstr(CSI "?25h");
}

// Set the foreground color to rgb.
// Each component must have the value 0 ... 5.
void fg(int r, int g, int b)
{
  putstr(CSI "38;5;"); putint(16 + 36 * r + 6*g + b);
}

// Set the foreground color to rgb.
// Each component must have the value 0 ... 5.
void bg(int r, int g, int b)
{
  putstr(CSI "48;5;"); putint(16 + 36 * r + 6*g + b);
}

// Set the foreground and background to the byte specified by n.
// The most significant nibble is the foreground.
// The least significant nibble is the background.
// The bits in each nibble specifie:
//  IBGR
//  I    High intensity
//   B   Blue
//    G  Green
//     R Red
//
void fgbg(int n)
{
  int f = (n>>4) & 0xf;
  int b = (n>>0) & 0xf;
  if (f < 8)
    f += 30;
  else
    f += 90 - 8;

  if (b < 8)
    b += 40;
  else
    b += 100 - 8;

  putstr(CSI); putint(f); putstr(";"); putint(b); putstr("m");
}


//--------------------------------- TERMINAL ----------------------------------

#define UNK -1
#define NON_INTR 0
#define INTR 1
int interrupt_mode = UNK;

int __io_putchar(int ch);
static int putchar_nonirq(int ch);
static struct fifo input_fifo;  // input buffer
static struct fifo output_fifo; // output buffer
int echo_mode = 1;              // should we echo input characters?
int line_mode = 1;              // should we wait for a newline?

// This is a version of printf() that will disable interrupts for the
// USART and write characters directly.  It is intended to handle fatal
// exceptional conditions.
static void safe_printf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    char buf[80];
    int len = vsnprintf(buf, sizeof buf, format, ap);
    int saved_txeie = USART1->CR1 & USART_CR1_TXEIE;
    USART1->CR1 &= ~USART_CR1_TXEIE;
    int x;
    for(x=0; x<len; x++) {
        putchar_nonirq(buf[x]);
    }
    USART1->CR1 |= saved_txeie;
    va_end(ap);
}

// Insert a character and echo it.
// (or, if it's a backspace, remove a char and erase it from the line).
// If echo_mode is turned off, just insert the character and get out.
static void insert_echo_char(char ch) {
    if (ch == '\r')
        ch = '\n';
    if (!echo_mode) {
        fifo_insert(&input_fifo, ch);
        return;
    }
    if (ch == '\b' || ch == '\177') {
        if (!fifo_empty(&input_fifo)) {
            char tmp = fifo_uninsert(&input_fifo);
            if (tmp == '\n')
                fifo_insert(&input_fifo, '\n');
            else if (tmp < 32)
                putstr("\b\b  \b\b");
            else
                putstr("\b \b");
        }
        return; // Don't put a backspace into buffer.
    } else if (ch == '\n') {
        __io_putchar('\n');
    } else if (ch == 0){
        putstr("^0");
    } else if (ch == 28) {
        putstr("^\\");
    } else if (ch < 32) {
        __io_putchar('^');
        __io_putchar('A'-1+ch);
    } else {
        __io_putchar(ch);
    }
    fifo_insert(&input_fifo, ch);
}

// Enable the USART RXNE interrupt.
void enable_tty_irq(void)
{
    USART1 -> CR1 |= USART_CR1_RXNEIE;
    NVIC -> ISER[0] |= (1 << USART1_IRQn);
    NVIC -> ISER[0] |= (1 << TIM2_IRQn);
    interrupt_mode = INTR;
    NVIC_SetPriority(USART1_IRQn, UART_PRIORITY);
}


// Transmit 'ch' using USART1
static int putchar_nonirq(int ch)
{
    if(ch == '\n')
    {
        while((USART1 -> ISR & USART_ISR_TXE) == 0);
        USART1 -> TDR = '\r';
    }
    while((USART1 -> ISR & USART_ISR_TXE) == 0);
    USART1 -> TDR = ch;
    return ch;
}

static int getchar_nonirq(void)
{
    if(USART1 -> ISR & USART_ISR_ORE)
    {
        USART1 -> ICR |= USART_ICR_ORECF;
    }
    if(line_mode)
    {
        while(!fifo_newline(&input_fifo))
        {
            while((USART1 -> ISR & USART_ISR_RXNE) == 0);
            insert_echo_char(USART1 -> RDR);
        }
        return fifo_remove(&input_fifo);
    }
    else
    {
        return fifo_remove(&input_fifo);
    }
}

// IRQ invoked for USART1 activity.
void USART1_IRQHandler(void)
{
    if(USART1 -> ISR & USART_ISR_RXNE)
    {
        insert_echo_char(USART1 -> RDR);
    }
    if(USART1 -> ISR & USART_ISR_TXE)
    {
        if(fifo_empty(&output_fifo))
        {
            USART1 -> CR1 &= ~USART_CR1_TXEIE;
        }
        else
        {
            USART1 -> TDR = fifo_remove(&output_fifo);
        }
    }
    if (USART1->ISR & (USART_ISR_ORE|USART_ISR_NE|USART_ISR_FE|USART_ISR_PE)) {
        safe_printf("Problem in USART1_IRQHandler: ISR = 0x%x\n", USART1->ISR);
    }
}

static int getchar_irq(void)
{
    if(line_mode)
    {
        while(!fifo_newline(&input_fifo))
        {
            asm("wfi");
        }
        return fifo_remove(&input_fifo);
    }
    else
    {
        return fifo_remove(&input_fifo);
    }
}

static int putchar_irq(char ch)
{
    while(fifo_full(&output_fifo))
    {
        asm("wfi");
    }
    if(ch == '\n')
    {
        fifo_insert(&output_fifo, '\r');
    }
    else
    {
        fifo_insert(&output_fifo, ch);
    }
    if((USART1 -> CR1 & USART_CR1_TXEIE) == 0)
    {
        USART1 -> CR1 |= USART_CR1_TXEIE;
        USART1_IRQHandler();
    }
    if(ch == '\n')
    {
        while(fifo_full(&output_fifo))
        {
            asm("wfi");
        }
        fifo_insert(&output_fifo, '\n');
    }
    return ch;
}

// Called by the Standard Peripheral library for a write()
int __io_putchar(int ch) {
    if (interrupt_mode == INTR)
        return putchar_irq(ch);
    else
        return putchar_nonirq(ch);
}

// Called by the Standard Peripheral library for a read()
int __io_getchar(void) {
    if (interrupt_mode == INTR)
        return getchar_irq();
    else
        return getchar_nonirq();
}

void tty_init(void) {
    // Disable buffers for stdio streams.  Otherwise, the first use of
    // each stream will result in a *malloc* of 2K.  Not good.
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);

    RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA -> MODER |= (0b10 << 2 * 9) | (0b10 << 2 * 10);
    GPIOA -> AFR[1] |= (0b0001 << 4 * 1) | (0b0001 << 4 * 2);

    RCC -> APB2ENR |= RCC_APB2ENR_USART1EN;         // enable clock
    USART1 -> CR1 &= ~USART_CR1_UE;                 // turn off USART
    USART1 -> CR1 &= ~((1 << 28) | (1 << 12));      // 8 bits, 1 stop bit
    USART1 -> CR1 &= ~USART_CR1_PCE;                // no parity
    USART1 -> CR1 &= ~USART_CR1_OVER8;              // 16x oversampling
    USART1 -> BRR = 0xD0;                           // 230.4kB/s
    USART1 -> CR1 |= (USART_CR1_TE | USART_CR1_RE); // tx/rx enable
    USART1 -> CR1 |= USART_CR1_UE;                  // turn on USART

    while((USART1 -> ISR & (USART_ISR_TEACK | USART_ISR_REACK)) == 0);
    interrupt_mode = NON_INTR;
    enable_tty_irq();
}

void raw_mode(void)
{
    line_mode = 0;
    echo_mode = 0;
}

void cooked_mode(void)
{
    line_mode = 1;
    echo_mode = 1;
}

int available(void)
{
    if (interrupt_mode == INTR) {
        if (fifo_empty(&input_fifo))
            return 0;
        return 1;
    } else {
        if ((USART1->ISR & USART_ISR_RXNE) == 0)
            return 0;
        return 1;
    }
}

//--------------------------------- QUEUE -------------------------------------

// Return 1 if the fifo holds no characters to remove
int fifo_empty(const struct fifo *f) {
    if (f->head == f->tail)
        return 1;
    else
        return 0;
}

// Return 1 if the fifo cannot hold any more characters
int fifo_full(const struct fifo *f) {
    uint8_t next = (f->tail + 1) % sizeof f->buffer;
    // We can't let the tail reach the head
    if (next == f->head)
        return 1;
    else
        return 0;
}

// Append a character to the tail of the fifo
// If the fifo is already full, drop the character
void fifo_insert(struct fifo *f, char ch) {
    if (fifo_full(f))
        return; // FIFO is full.  Just drop the new character
    uint8_t next = (f->tail + 1) % sizeof f->buffer;
    if (ch == '\n')
        f->newline = f->tail; // a newline has been inserted
    else if (f->newline == f->tail)
        f->newline = next; // no newline detected yet
    f->buffer[f->tail] = ch;
    f->tail = next;
}

// Remove a character from the *tail* of the fifo.
char fifo_uninsert(struct fifo *f) {
    if (fifo_empty(f))
        return '$'; // something unexpected
    int prev = (f->tail == 0) ? sizeof f->buffer - 1 : f->tail - 1;
    if (f->newline == f->tail)
        f->newline = prev;
    f->tail = prev;
    char ch = f->buffer[f->tail];
    f->buffer[f->tail] = '@'; // something unexpected
    return ch;
}

// Return 1 if the fifo contains at least one newline
int fifo_newline(const struct fifo *f) {
    if (fifo_empty(f))
        return 0;
    if (f->newline != f->tail)
        return 1;
    return 0;
}

// Remove a character from the head of the fifo.
// If the fifo is empty, you get a tilde (~).
char fifo_remove(struct fifo *f) {
    if (fifo_empty(f))
        return '!'; // something unexpected.
    char ch = f->buffer[f->head];
    if (f->newline == f->head)
        f->newline = f->tail; // We just read the last newline.  Clear nl.
    f->buffer[f->head] = '~'; // overwrite removed char with something weird
    uint8_t next = (f->head + 1) % sizeof f->buffer;
    f->head = next;
    return ch;
}
