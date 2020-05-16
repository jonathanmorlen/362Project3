#ifndef __MUSIC_H__
#define __MUSIC_H__

#include <stdint.h>

//------------------------------- MUSIC ---------------------------------------

// The DAC rate
#define RATE 10000

// N powers of the the 12th-root of 2.
#define STEP1 1.05946309436
#define STEP2 (STEP1*STEP1)
#define STEP3 (STEP2*STEP1)
#define STEP4 (STEP3*STEP1)
#define STEP5 (STEP4*STEP1)
#define STEP6 (STEP5*STEP1)
#define STEP7 (STEP6*STEP1)
#define STEP8 (STEP7*STEP1)
#define STEP9 (STEP8*STEP1)

// Macros for computing the fixed point representation of
// the step size used for traversing a wavetable of size N
// at a rate of RATE to produce tones of various doublings
// and halvings of 440 Hz.  (The "A" above middle "C".)
#define A14    ((13.75   * N/RATE) * (1<<16)) /* A0 */
#define A27    ((27.5    * N/RATE) * (1<<16)) /* A1 */
#define A55    ((55.0    * N/RATE) * (1<<16)) /* A2 */
#define A110   ((110.0   * N/RATE) * (1<<16)) /* A3 */
#define A220   ((220.0   * N/RATE) * (1<<16)) /* A4 */
#define A440   ((440.0   * N/RATE) * (1<<16)) /* A5 */
#define A880   ((880.0   * N/RATE) * (1<<16)) /* A6 */
#define A1760  ((1760.0  * N/RATE) * (1<<16)) /* A7 */
#define A3520  ((3520.0  * N/RATE) * (1<<16)) /* A8 */
#define A7040  ((7040.0  * N/RATE) * (1<<16)) /* A9 */
#define A14080 ((14080.0 * N/RATE) * (1<<16)) /* A10 */

void init_DMA();
void init_TIM6();
void init_TIM3(int n);
void init_DAC();
void init_sine();

//---------------------------------- MIDI -------------------------------------

typedef struct {
  const uint8_t *file_start;
  int length;
  uint8_t format;
  uint8_t tracks;
  uint16_t divisions;
} MIDI_Header;

#define MAXTICKS 0x10000000
typedef struct {
  int ticks;
  int nexttick;
} MIDI_Player;

// The core functions to interpret and play MIDI file contents...
MIDI_Player *midi_init(const uint8_t *array);
void midi_play(void);

// Functions you may implement to handle events in the real world.
void set_tempo(int time, int value, const MIDI_Header *hdr);
void note_off(int time, int chan, int key, int velo);
void note_on(int time, int chan, int key, int velo);

void key_pressure(int time, int chan, int key, int val);
void control_change(int time, int chan, int ctrl, int val);
void program_change(int time, int chan, int prog);
void channel_pressure(int time, int chan, int val);
void pitch_wheel_change(int time, int chan, int val);
void text_event(const char *str, int len);
void copyright_notice(const char *str, int len);
void track_name(const char *str, int len);
void lyric(int time, const char *str, int len);
void marker(int time, const char *str, int len);
void end_of_track(int time);
void smpte_offset(int time, int hr, int mn, int se, int fr, int ff);
void time_signature(int time, int n, int d, int b, int c);
void key_signature(int time, int8_t sf, int mi);

// Debugging.
void midi_error(int time, const uint8_t *p, const uint8_t *file_start, const char *msg);
void unknown_meta(int time, int offset, int type, int len, const uint8_t *buf);
void header_info(MIDI_Header *hdr);
void file_offset_info(int time);
void track_info(int num, const uint8_t *p, int len, const MIDI_Header *hdr);
void sysex_info(int time, const uint8_t *p, const uint8_t *buf, int len,
                const MIDI_Header *hdr);

#endif /* __MUSIC_H__ */
