#ifndef GRAPHICS_H_
#define GRAPHICS_H_

//-------------------------- DRAWING/COLOR -------------------------------------

void blank();
void blank_section(int x1, int y1, int w, int h);
void color_section(int x1, int y1, int w, int h, int s, int c);
void draw_box(int x, int y, int w, int h, int c);
void draw_border(int c);
void draw_hline(int x, int y, int l, int s, int c);
void draw_vline(int x, int y, int l, int s, int c);
void draw_HUD();
void draw_character(int x, int y, int c);

//------------------------------- SCREEN --------------------------------------

// Put the cursor in the upper left.
void home(void);

// Clear the screen.
void clear(void);

// Set the cursor position to x,y (zero-based indexing)
void setpos(int x, int y);

// Turn the cursor off.
void cursor_off(void);

// Turn the cursor on.
void cursor_on(void);

// Set the foreground color to rgb.
// Each component must have the value 0 ... 5.
void fg(int r, int g, int b);

// Set the foreground color to rgb.
// Each component must have the value 0 ... 5.
void bg(int r, int g, int b);

// Set the foreground and background to the byte specified by n.
// The most significant nibble is the foreground.
// The least significant nibble is the background.
// The bits in each nibble specified:
//  IBGR
//  I    High intensity
//   B   Blue
//    G  Green
//     R Red
//
void fgbg(int n);

//--------------------------------- TERMINAL ----------------------------------

void tty_init(void);
void tty_uninit(void);
int  available(void);
int  getkey(void);
void raw_mode(void);
void cooked_mode(void);

//--------------------------------- STACK --------------------------------------

struct fifo {
    char buffer[128];           // circular buffer for the fifo
    volatile uint8_t head;      // the first thing to remove from the fifo
    volatile uint8_t tail;      // the next place to insert a new character
    volatile uint8_t newline;   // offset of last-entered newline
};

int fifo_empty(const struct fifo *f);
int fifo_full(const struct fifo *f);
void fifo_insert(struct fifo *f, char ch);
char fifo_uninsert(struct fifo *f);
int fifo_newline(const struct fifo *f);
char fifo_remove(struct fifo *f);

#endif /* GRAPHICS_H_ */
