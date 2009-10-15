
#include "minimal.h"
#include "profiler.h"


#ifdef PROFILER_PSX4ALL
#include "timeval.h"
#endif

extern int     psx4all_emulating;

#ifdef __WIN32__
int __errno;
#endif

int gp2x_sdlwrapper_bpp=8;

void		*gp2x_sdlwrapper_screen_pixels=NULL;
SDL_Surface	*gp2x_sdlwrapper_screen=NULL;
SDL_Surface	*hw_screen=NULL;
double		gp2x_sdlwrapper_ticksdivisor=1.0;

unsigned long	gp2x_ticks_per_second = 1000;

extern int soundcard;
extern int closing_sound;
extern void init_sound(unsigned rate, int stereo, int bits);
extern void quit_sound(void);

int gp2x_double_buffer=0;

u8 *backscreen = NULL;
u8 *frontscreen = NULL;

#ifdef DEBUG
FILE* fdbg;
#endif

/* The font is generated from Xorg 6x10-L1.bdf */
static unsigned char gp2x_fontf[256][10] = {
{ 0x00>>2, 0xA8>>2, 0x00>>2, 0x88>>2, 0x00>>2, 0x88>>2, 0x00>>2, 0xA8>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x20>>2, 0x70>>2, 0xF8>>2, 0x70>>2, 0x20>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0xA8>>2, 0x54>>2, 0xA8>>2, 0x54>>2, 0xA8>>2, 0x54>>2, 0xA8>>2, 0x54>>2, 0xA8>>2, 0x54>>2, },
{ 0x00>>2, 0x90>>2, 0x90>>2, 0xF0>>2, 0x90>>2, 0x90>>2, 0x78>>2, 0x10>>2, 0x10>>2, 0x10>>2, },
{ 0x00>>2, 0xE0>>2, 0x80>>2, 0xC0>>2, 0x80>>2, 0xB8>>2, 0x20>>2, 0x30>>2, 0x20>>2, 0x20>>2, },
{ 0x00>>2, 0x70>>2, 0x80>>2, 0x80>>2, 0x70>>2, 0x70>>2, 0x48>>2, 0x70>>2, 0x48>>2, 0x48>>2, },
{ 0x00>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0xF0>>2, 0x78>>2, 0x40>>2, 0x70>>2, 0x40>>2, 0x40>>2, },
{ 0x00>>2, 0x20>>2, 0x50>>2, 0x20>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x20>>2, 0x20>>2, 0xF8>>2, 0x20>>2, 0x20>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x90>>2, 0xD0>>2, 0xD0>>2, 0xB0>>2, 0x90>>2, 0x40>>2, 0x40>>2, 0x40>>2, 0x78>>2, },
{ 0x00>>2, 0x90>>2, 0x90>>2, 0x60>>2, 0x40>>2, 0x78>>2, 0x10>>2, 0x10>>2, 0x10>>2, 0x10>>2, },
{ 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0xE0>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0xE0>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x3C>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, },
{ 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x3C>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0xFC>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, },
{ 0xFC>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0xFC>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0xFF>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0xFC>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0xFC>>2, },
{ 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x3C>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, },
{ 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0xE0>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, },
{ 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0xFC>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0xFC>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, },
{ 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, },
{ 0x00>>2, 0x18>>2, 0x60>>2, 0x80>>2, 0x60>>2, 0x18>>2, 0x00>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xC0>>2, 0x30>>2, 0x08>>2, 0x30>>2, 0xC0>>2, 0x00>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0xF8>>2, 0x50>>2, 0x50>>2, 0x50>>2, 0x50>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x08>>2, 0x10>>2, 0xF8>>2, 0x20>>2, 0xF8>>2, 0x40>>2, 0x80>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x30>>2, 0x48>>2, 0x40>>2, 0xE0>>2, 0x40>>2, 0x48>>2, 0xB0>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x20>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x50>>2, 0x50>>2, 0x50>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x50>>2, 0x50>>2, 0xF8>>2, 0x50>>2, 0xF8>>2, 0x50>>2, 0x50>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x70>>2, 0xA0>>2, 0x70>>2, 0x28>>2, 0x70>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x48>>2, 0xA8>>2, 0x50>>2, 0x20>>2, 0x50>>2, 0xA8>>2, 0x90>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x40>>2, 0xA0>>2, 0xA0>>2, 0x40>>2, 0xA8>>2, 0x90>>2, 0x68>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x10>>2, 0x20>>2, 0x40>>2, 0x40>>2, 0x40>>2, 0x20>>2, 0x10>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x40>>2, 0x20>>2, 0x10>>2, 0x10>>2, 0x10>>2, 0x20>>2, 0x40>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x88>>2, 0x50>>2, 0xF8>>2, 0x50>>2, 0x88>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x20>>2, 0x20>>2, 0xF8>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x30>>2, 0x20>>2, 0x40>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0xF8>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x20>>2, 0x70>>2, 0x20>>2, 0x00>>2, },
{ 0x00>>2, 0x08>>2, 0x08>>2, 0x10>>2, 0x20>>2, 0x40>>2, 0x80>>2, 0x80>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x50>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x50>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x60>>2, 0xA0>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x08>>2, 0x30>>2, 0x40>>2, 0x80>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF8>>2, 0x08>>2, 0x10>>2, 0x30>>2, 0x08>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x10>>2, 0x30>>2, 0x50>>2, 0x90>>2, 0xF8>>2, 0x10>>2, 0x10>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF8>>2, 0x80>>2, 0xB0>>2, 0xC8>>2, 0x08>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x30>>2, 0x40>>2, 0x80>>2, 0xB0>>2, 0xC8>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF8>>2, 0x08>>2, 0x10>>2, 0x10>>2, 0x20>>2, 0x40>>2, 0x40>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x08>>2, 0x10>>2, 0x60>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x20>>2, 0x70>>2, 0x20>>2, 0x00>>2, 0x20>>2, 0x70>>2, 0x20>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x20>>2, 0x70>>2, 0x20>>2, 0x00>>2, 0x30>>2, 0x20>>2, 0x40>>2, 0x00>>2, },
{ 0x00>>2, 0x08>>2, 0x10>>2, 0x20>>2, 0x40>>2, 0x20>>2, 0x10>>2, 0x08>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0xF8>>2, 0x00>>2, 0xF8>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x40>>2, 0x20>>2, 0x10>>2, 0x08>>2, 0x10>>2, 0x20>>2, 0x40>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x10>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x98>>2, 0xA8>>2, 0xB0>>2, 0x80>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x50>>2, 0x88>>2, 0x88>>2, 0xF8>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF0>>2, 0x48>>2, 0x48>>2, 0x70>>2, 0x48>>2, 0x48>>2, 0xF0>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF0>>2, 0x48>>2, 0x48>>2, 0x48>>2, 0x48>>2, 0x48>>2, 0xF0>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF8>>2, 0x80>>2, 0x80>>2, 0xF0>>2, 0x80>>2, 0x80>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF8>>2, 0x80>>2, 0x80>>2, 0xF0>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x80>>2, 0x80>>2, 0x98>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0xF8>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x38>>2, 0x10>>2, 0x10>>2, 0x10>>2, 0x10>>2, 0x90>>2, 0x60>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x88>>2, 0x90>>2, 0xA0>>2, 0xC0>>2, 0xA0>>2, 0x90>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x88>>2, 0x88>>2, 0xD8>>2, 0xA8>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x88>>2, 0x88>>2, 0xC8>>2, 0xA8>>2, 0x98>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF0>>2, 0x88>>2, 0x88>>2, 0xF0>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0xA8>>2, 0x70>>2, 0x08>>2, 0x00>>2, },
{ 0x00>>2, 0xF0>>2, 0x88>>2, 0x88>>2, 0xF0>>2, 0xA0>>2, 0x90>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x80>>2, 0x70>>2, 0x08>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF8>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x50>>2, 0x50>>2, 0x50>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0xA8>>2, 0xA8>>2, 0xD8>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x88>>2, 0x88>>2, 0x50>>2, 0x20>>2, 0x50>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x88>>2, 0x88>>2, 0x50>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF8>>2, 0x08>>2, 0x10>>2, 0x20>>2, 0x40>>2, 0x80>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x40>>2, 0x40>>2, 0x40>>2, 0x40>>2, 0x40>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x80>>2, 0x80>>2, 0x40>>2, 0x20>>2, 0x10>>2, 0x08>>2, 0x08>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x10>>2, 0x10>>2, 0x10>>2, 0x10>>2, 0x10>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x50>>2, 0x88>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0xF8>>2, 0x00>>2, },
{ 0x20>>2, 0x10>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x70>>2, 0x08>>2, 0x78>>2, 0x88>>2, 0x78>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x80>>2, 0x80>>2, 0xB0>>2, 0xC8>>2, 0x88>>2, 0xC8>>2, 0xB0>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0x80>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x08>>2, 0x08>>2, 0x68>>2, 0x98>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0xF8>>2, 0x80>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x30>>2, 0x48>>2, 0x40>>2, 0xF0>>2, 0x40>>2, 0x40>>2, 0x40>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x78>>2, 0x88>>2, 0x88>>2, 0x78>>2, 0x08>>2, 0x88>>2, 0x70>>2, },
{ 0x00>>2, 0x80>>2, 0x80>>2, 0xB0>>2, 0xC8>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x00>>2, 0x60>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x08>>2, 0x00>>2, 0x18>>2, 0x08>>2, 0x08>>2, 0x08>>2, 0x48>>2, 0x48>>2, 0x30>>2, },
{ 0x00>>2, 0x80>>2, 0x80>>2, 0x88>>2, 0x90>>2, 0xE0>>2, 0x90>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x60>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0xD0>>2, 0xA8>>2, 0xA8>>2, 0xA8>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0xB0>>2, 0xC8>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0xB0>>2, 0xC8>>2, 0x88>>2, 0xC8>>2, 0xB0>>2, 0x80>>2, 0x80>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x68>>2, 0x98>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x08>>2, 0x08>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0xB0>>2, 0xC8>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x70>>2, 0x80>>2, 0x70>>2, 0x08>>2, 0xF0>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x40>>2, 0x40>>2, 0xF0>>2, 0x40>>2, 0x40>>2, 0x48>>2, 0x30>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x50>>2, 0x50>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0xA8>>2, 0xA8>>2, 0x50>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x88>>2, 0x50>>2, 0x20>>2, 0x50>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x08>>2, 0x88>>2, 0x70>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0xF8>>2, 0x10>>2, 0x20>>2, 0x40>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x18>>2, 0x20>>2, 0x10>>2, 0x60>>2, 0x10>>2, 0x20>>2, 0x18>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x60>>2, 0x10>>2, 0x20>>2, 0x18>>2, 0x20>>2, 0x10>>2, 0x60>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x48>>2, 0xA8>>2, 0x90>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x00>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x20>>2, 0x78>>2, 0xA0>>2, 0xA0>>2, 0xA0>>2, 0x78>>2, 0x20>>2, 0x00>>2, },
{ 0x00>>2, 0x30>>2, 0x48>>2, 0x40>>2, 0xE0>>2, 0x40>>2, 0x48>>2, 0xB0>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x88>>2, 0x70>>2, 0x50>>2, 0x70>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x88>>2, 0x88>>2, 0x50>>2, 0x20>>2, 0xF8>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x80>>2, 0xE0>>2, 0x90>>2, 0x48>>2, 0x38>>2, 0x08>>2, 0x70>>2, 0x00>>2, },
{ 0x50>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0xA8>>2, 0xC8>>2, 0xA8>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x38>>2, 0x48>>2, 0x58>>2, 0x28>>2, 0x00>>2, 0x78>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x24>>2, 0x48>>2, 0x90>>2, 0x48>>2, 0x24>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x78>>2, 0x08>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x78>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0xE8>>2, 0xC8>>2, 0xC8>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0xF8>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x50>>2, 0x20>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x20>>2, 0x20>>2, 0xF8>>2, 0x20>>2, 0x20>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x30>>2, 0x48>>2, 0x10>>2, 0x20>>2, 0x78>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x70>>2, 0x08>>2, 0x30>>2, 0x08>>2, 0x70>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0x20>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0xC8>>2, 0xB0>>2, 0x80>>2, 0x00>>2, },
{ 0x00>>2, 0x78>>2, 0xE8>>2, 0xE8>>2, 0x68>>2, 0x28>>2, 0x28>>2, 0x28>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x20>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x10>>2, 0x20>>2, },
{ 0x20>>2, 0x60>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x30>>2, 0x48>>2, 0x48>>2, 0x30>>2, 0x00>>2, 0x78>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x90>>2, 0x48>>2, 0x24>>2, 0x48>>2, 0x90>>2, 0x00>>2, 0x00>>2, },
{ 0x40>>2, 0xC0>>2, 0x40>>2, 0x40>>2, 0xE4>>2, 0x0C>>2, 0x14>>2, 0x3C>>2, 0x04>>2, 0x00>>2, },
{ 0x40>>2, 0xC0>>2, 0x40>>2, 0x40>>2, 0xE8>>2, 0x14>>2, 0x04>>2, 0x08>>2, 0x1C>>2, 0x00>>2, },
{ 0xC0>>2, 0x20>>2, 0x40>>2, 0x20>>2, 0xC8>>2, 0x18>>2, 0x28>>2, 0x78>>2, 0x08>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x00>>2, 0x20>>2, 0x20>>2, 0x40>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x40>>2, 0x20>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0xF8>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0x20>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0xF8>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0xF8>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x48>>2, 0xB0>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0xF8>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x50>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0xF8>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0xF8>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x3C>>2, 0x50>>2, 0x90>>2, 0x9C>>2, 0xF0>>2, 0x90>>2, 0x9C>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0x88>>2, 0x70>>2, 0x20>>2, 0x40>>2, },
{ 0x40>>2, 0xF8>>2, 0x80>>2, 0x80>>2, 0xF0>>2, 0x80>>2, 0x80>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0xF8>>2, 0x80>>2, 0x80>>2, 0xF0>>2, 0x80>>2, 0x80>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0xF8>>2, 0x80>>2, 0x80>>2, 0xF0>>2, 0x80>>2, 0x80>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x50>>2, 0xF8>>2, 0x80>>2, 0x80>>2, 0xF0>>2, 0x80>>2, 0x80>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x40>>2, 0x20>>2, 0x70>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0x20>>2, 0x70>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x70>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x50>>2, 0x00>>2, 0x70>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF0>>2, 0x48>>2, 0x48>>2, 0xE8>>2, 0x48>>2, 0x48>>2, 0xF0>>2, 0x00>>2, 0x00>>2, },
{ 0x28>>2, 0x50>>2, 0x88>>2, 0xC8>>2, 0xA8>>2, 0x98>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x40>>2, 0x20>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0x20>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x28>>2, 0x50>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x50>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x88>>2, 0x50>>2, 0x20>>2, 0x50>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x98>>2, 0x98>>2, 0xA8>>2, 0xC8>>2, 0xC8>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x40>>2, 0x20>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0x20>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x50>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0x20>>2, 0x88>>2, 0x88>>2, 0x50>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x80>>2, 0xF0>>2, 0x88>>2, 0xF0>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x90>>2, 0xA0>>2, 0x90>>2, 0x88>>2, 0xB0>>2, 0x00>>2, 0x00>>2, },
{ 0x40>>2, 0x20>>2, 0x00>>2, 0x70>>2, 0x08>>2, 0x78>>2, 0x88>>2, 0x78>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0x20>>2, 0x00>>2, 0x70>>2, 0x08>>2, 0x78>>2, 0x88>>2, 0x78>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x00>>2, 0x70>>2, 0x08>>2, 0x78>>2, 0x88>>2, 0x78>>2, 0x00>>2, 0x00>>2, },
{ 0x28>>2, 0x50>>2, 0x00>>2, 0x70>>2, 0x08>>2, 0x78>>2, 0x88>>2, 0x78>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x50>>2, 0x00>>2, 0x70>>2, 0x08>>2, 0x78>>2, 0x88>>2, 0x78>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x20>>2, 0x70>>2, 0x08>>2, 0x78>>2, 0x88>>2, 0x78>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x78>>2, 0x14>>2, 0x7C>>2, 0x90>>2, 0x7C>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0x80>>2, 0x88>>2, 0x70>>2, 0x20>>2, 0x40>>2, },
{ 0x40>>2, 0x20>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0xF8>>2, 0x80>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0x20>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0xF8>>2, 0x80>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0xF8>>2, 0x80>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x50>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0xF8>>2, 0x80>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x40>>2, 0x20>>2, 0x00>>2, 0x60>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x40>>2, 0x00>>2, 0x60>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x00>>2, 0x60>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x50>>2, 0x00>>2, 0x60>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xC0>>2, 0x30>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x28>>2, 0x50>>2, 0x00>>2, 0xB0>>2, 0xC8>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x40>>2, 0x20>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0x20>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x28>>2, 0x50>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x50>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x20>>2, 0x00>>2, 0xF8>>2, 0x00>>2, 0x20>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x78>>2, 0x98>>2, 0xA8>>2, 0xC8>>2, 0xF0>>2, 0x00>>2, 0x00>>2, },
{ 0x40>>2, 0x20>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0x20>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x50>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x10>>2, 0x20>>2, 0x88>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x08>>2, 0x88>>2, 0x70>>2, },
{ 0x00>>2, 0x00>>2, 0x80>>2, 0xF0>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0xF0>>2, 0x80>>2, 0x80>>2, },
{ 0x00>>2, 0x50>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x08>>2, 0x88>>2, 0x70>>2, },
};

static gp2x_font gp2x_default_font;

void (*gp2x_printfchar)(gp2x_font *f, unsigned char c);

void gp2x_printfchar15(gp2x_font *f, unsigned char c)
{
  unsigned short *dst=&((unsigned short*)sdlscreen->pixels)[f->x+f->y*(sdlscreen->pitch>>1)],w,h=f->h;
//unsigned char  *src=f->data[ (c%16)*f->w + (c/16)*f->h ];
  unsigned char  *src=&f->data[c*10];

 if(f->solid)
         while(h--)
         {
          w=f->wmask;
          while(w)
          {
           if( *src & w ) *dst++=f->fg; else *dst++=f->bg;
           w>>=1;
          }
          src++;

          dst+=(sdlscreen->pitch>>1)-(f->w);
         }
 else
         while(h--)
         {
          w=f->wmask;
          while(w)
          {
           if( *src & w ) *dst=f->fg;
           dst++;
           w>>=1;
          }
          src++;

          dst+=(sdlscreen->pitch>>1)-(f->w);
         }
}

void gp2x_printf(gp2x_font *f, int x, int y, const char *format, ...)
{
 char buffer[4096]; int c; gp2x_font *g=&gp2x_default_font;
 va_list  args;

 va_start(args, format);
 vsprintf(buffer, format, args);

 if(f!=NULL) g=f;

 if(x<0) x=g->x; else g->x=x;
 if(y<0) y=g->y; else g->y=y;

 for(c=0;buffer[c];c++)
 {
  switch(buffer[c])
  {
   case '\b': g->x=x;g->y=y; break;

   case '\n': g->y+=g->h;
   case '\r': g->x=x;
              break;

   default:   gp2x_printfchar(g, (unsigned char)buffer[c]);
              g->x+=g->w;
              break;
  }
 }

 //gp2x_video_flip_single();
}

void gp2x_printf_init(gp2x_font *f, int w, int h, void *data, int fg, int bg, int solid)
{
 gp2x_printfchar=gp2x_printfchar15;
 f->x=f->y=0;
 f->wmask=1<<(w-1);
 f->w=w;
 f->h=h;
 f->data=(unsigned char *)data;
 f->fg=fg;
 f->bg=bg;
 f->solid=solid;
}

SDL_Surface* screen_real;

#include <sys/resource.h>
#include <sys/time.h>

void gp2x_init(int ticks_per_second, int bpp, int rate, int bits, int stereo, int hz, int solid_font)
{
	setpriority(PRIO_PROCESS, 0, -20);
	
#ifdef DEBUG
#if 0
        char df[100];
        struct timeval tv;
        gettimeofday(&tv, NULL);
        sprintf(df, "debug_%d.txt", (int)tv.tv_sec);
	fdbg = fopen(df, "w");
#else
	fdbg = fopen("debug.txt", "w");
#endif
#endif

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		DEBUGF("SDL_Init failed: %s\n", SDL_GetError());
		exit(-1);
	}
#ifdef RENDER_DOUBLE
	screen_real = SDL_SetVideoMode(640, 480, 16, SDL_DOUBLEBUF);
	if (!screen_real) {
		DEBUGF("SDL_SetVideoMode failed: %s\n", SDL_GetError());
		exit(-1);
	}
	gp2x_sdlwrapper_screen = SDL_CreateRGBSurface(0,
		320, 240, 16, screen_real->format->Rmask,
					  screen_real->format->Gmask,
					  screen_real->format->Bmask,
					  screen_real->format->Amask);

	if(SDL_MUSTLOCK(screen_real)) SDL_LockSurface(screen_real);
#else
	gp2x_sdlwrapper_screen = SDL_SetVideoMode(320, 240, 16, SDL_HWSURFACE /* | SDL_DOUBLEBUF */);
	if (!gp2x_sdlwrapper_screen) {
		DEBUGF("SDL_SetVideoMode failed: %s\n", SDL_GetError());
		SDL_Quit();
		exit(-1);
	}
#endif

	if(SDL_MUSTLOCK(sdlscreen)) SDL_LockSurface(sdlscreen);

	SDL_WM_SetCaption("psx4all - SDL Version", "psx4all");

	if(gp2x_sdlwrapper_screen == NULL)
	{
		SDL_Quit();
		exit(0);
		return;
	}
	gp2x_sdlwrapper_bpp=bpp;
	gp2x_sdlwrapper_screen_pixels=gp2x_sdlwrapper_screen->pixels;

	/*SDL_EventState(SDL_ACTIVEEVENT,SDL_IGNORE);
	SDL_EventState(SDL_MOUSEMOTION,SDL_IGNORE);
	SDL_EventState(SDL_MOUSEBUTTONDOWN,SDL_IGNORE);
	SDL_EventState(SDL_MOUSEBUTTONUP,SDL_IGNORE);
	SDL_EventState(SDL_SYSWMEVENT,SDL_IGNORE);
	SDL_EventState(SDL_VIDEORESIZE,SDL_IGNORE);
	SDL_EventState(SDL_USEREVENT,SDL_IGNORE);
	SDL_ShowCursor(SDL_DISABLE);*/

	//init font
	gp2x_printf_init(&gp2x_default_font,6,10,gp2x_fontf,0xFFFF,0x0000,solid_font);

	atexit(gp2x_deinit);

	backscreen = (u8*)malloc(sdlscreen->pitch * sdlscreen->h);
	frontscreen = (u8*)sdlscreen->pixels;
}

void gp2x_change_res(int w, int h)
{
        gp2x_sdlwrapper_screen = SDL_SetVideoMode(w, h, 16, SDL_HWSURFACE);
	/*SDL_EventState(SDL_ACTIVEEVENT,SDL_IGNORE);
	SDL_EventState(SDL_MOUSEMOTION,SDL_IGNORE);
	SDL_EventState(SDL_MOUSEBUTTONDOWN,SDL_IGNORE);
	SDL_EventState(SDL_MOUSEBUTTONUP,SDL_IGNORE);
	SDL_EventState(SDL_SYSWMEVENT,SDL_IGNORE);
	SDL_EventState(SDL_VIDEORESIZE,SDL_IGNORE);
	SDL_EventState(SDL_USEREVENT,SDL_IGNORE);
	SDL_ShowCursor(SDL_DISABLE);*/
}

void gp2x_deinit(void)
{
  SDL_Quit();

#ifdef DEBUG
	fclose(fdbg);
#endif
}

unsigned long keystate = 0;

unsigned long gp2x_joystick_read(void)
{
  //events
  SDL_Event event;
  unsigned long ret = keystate;
  static bool quit = false;

  while(SDL_PollEvent(&event))
  {
	  switch(event.type)
	  {
		  case SDL_QUIT:
		  	gp2x_deinit(); exit(0);

		  	break;
		  case SDL_KEYDOWN:
		  	switch(event.key.keysym.sym)
		  	{
		  		case SDLK_MENU:
					gp2x_deinit(); exit(0);
					break;

				case SDLK_UP:    ret |= GP2X_UP;    break;
				case SDLK_DOWN:  ret |= GP2X_DOWN;  break;
				case SDLK_LEFT:  ret |= GP2X_LEFT;  break;
				case SDLK_RIGHT: ret |= GP2X_RIGHT; break;

				case SDLK_LCTRL: ret |= GP2X_A; break;
				case SDLK_LALT: ret |= GP2X_B; break;
				case SDLK_LSHIFT: ret |= GP2X_Y; break;
				case SDLK_SPACE: ret |= GP2X_X; break;

				case SDLK_TAB: ret |= GP2X_L; break;
				case SDLK_q: ret |= GP2X_VOL_DOWN; break;
				case SDLK_BACKSPACE: ret |= GP2X_R; break;
				case SDLK_r: ret |= GP2X_VOL_UP; break;

				case SDLK_RETURN: ret |= GP2X_START; break;
				case SDLK_ESCAPE: ret |= GP2X_SELECT; break;

				default: break;
			}
			break;
		case SDL_KEYUP:
		  	switch(event.key.keysym.sym)
		  	{
				case SDLK_UP:    ret &= ~GP2X_UP;    break;
				case SDLK_DOWN:  ret &= ~GP2X_DOWN;  break;
				case SDLK_LEFT:  ret &= ~GP2X_LEFT;  break;
				case SDLK_RIGHT: ret &= ~GP2X_RIGHT; break;

				case SDLK_LCTRL: ret &= ~GP2X_A; break;
				case SDLK_LALT: ret &= ~GP2X_B; break;
				case SDLK_LSHIFT: ret &= ~GP2X_Y; break;
				case SDLK_SPACE: ret &= ~GP2X_X; break;

				case SDLK_TAB: ret &= ~GP2X_L; break;
				case SDLK_q: ret &= ~GP2X_VOL_DOWN; break;
				case SDLK_BACKSPACE: ret &= ~GP2X_R; break;
				case SDLK_r: ret &= ~GP2X_VOL_UP; break;

				case SDLK_RETURN: ret &= ~GP2X_START; break;
				case SDLK_ESCAPE: ret &= ~GP2X_SELECT; break;

				default: break;
			}
			break;
		default: break;
	  }
  }

  keystate = ret;
  if ((keystate & (GP2X_SELECT | GP2X_X)) == (GP2X_SELECT | GP2X_X)) {
  	if (!quit) {
  		BACKSCREEN;
		gp2x_video_RGB_clearscreen16();
		gp2x_printf(NULL, 80, 80, "Hold SELECT + X to quit");
		FRONTSCREEN;
		gp2x_video_flip();
		gp2x_timer_delay(1000);
		quit = true;
	}
	else {
		gp2x_deinit();
		exit(0);
	}
  }
  else quit = false;

  return ret;
}

void gp2x_video_RGB_clearscreen16(void)
{
  memset(sdlscreen->pixels, 0, sdlscreen->pitch*sdlscreen->h);
}

void gp2x_timer_delay(unsigned long ticks)
{
  SDL_Delay(ticks);
}

void gp2x_timer_delay_raw(unsigned long raws)
{
  //
}

unsigned long gp2x_timer_read(void)
{
  return SDL_GetTicks();
}

unsigned long gp2x_timer_raw(void)
{
  return SDL_GetTicks();
}

void gp2x_video_flip()
{
#ifdef RENDER_DOUBLE
	int i, j;

	for(i=0; i<240; i++)
	{
		for(j=0; j<320; j++)
		{
			((Uint16*)screen_real->pixels)[640*i*2 + 2*j] =
			((Uint16*)screen_real->pixels)[640*i*2 + 2*j + 1] =
			((Uint16*)screen_real->pixels)[640*(i*2+1) + 2*j] =
			((Uint16*)screen_real->pixels)[640*(i*2+1) + 2*j + 1] =
			((Uint16*)sdlscreen->pixels)[320*i + j];
		}
	}

	if(SDL_MUSTLOCK(sdlscreen)) SDL_UnlockSurface(sdlscreen);
	if(SDL_MUSTLOCK(screen_real)) SDL_UnlockSurface(screen_real);


	SDL_Flip(screen_real);

	if(SDL_MUSTLOCK(screen_real)) SDL_LockSurface(screen_real);
#else
	if(SDL_MUSTLOCK(sdlscreen)) SDL_UnlockSurface(sdlscreen);

	SDL_Flip(sdlscreen);
#endif

	if(SDL_MUSTLOCK(sdlscreen)) SDL_LockSurface(sdlscreen);
}
