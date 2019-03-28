#ifndef __KEYBOARD_H
#define __KEYBAORD_H

#include "os.h"

#define CTRLKEYS_LSHIFT 	(1<<0)
#define CTRLKEYS_RSHIFT 	(1<<1)
#define CTRLKEYS_LALT 		(1<<2)

typedef enum {
	//Controls keys
	KEY_LSHIFT = 0x2A,
	KEY_RSHIFT = 0x36,
	KEY_LALT = 0x38,
	
	//Numpad
	KEY_NP_7 = 0x47,
	KEY_NP_8,
	KEY_NP_9,
	KEY_NP_MINUS,
	KEY_NP_4,
	KEY_NP_5,
	KEY_NP_6,
	KEY_NP_PLUS,
	KEY_NP_DOT,
	//Functions
	KEY_F1 = 0x3B,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11 = 0x57,
	KEY_F12
} kb_keys_t;

void keyboard_install();
u8 keyboard_scan_code();
u8 keyboard_ascii();

void keyboard_enable();
void keyboard_disable();

void keyboard_reset_irq1_vars();

#endif
