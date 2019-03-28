#include "keyboard.h"
#include "exceptions.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"
#include "termlib.h"
#include "vga_terminal.h"

static u8 s_i8keycode = 0;
static u8 s_i8keycode2 = 0;
static u32 s_ctrlkeys = 0;

static u8 s_keymap[128] = {
	0, 0x1B, 
	'1','2',
	'3','4','5','6',
	'7','8','9','0',
	'-','=',
	0x08,
	0x09,
	'Q','W','E','R',
	'T','Y','U','I',
	'O','P','[',']',
	0x0A,0,
	'A','S','D','F',
	'G','H','J','K',
	'L',';','\'','`',
	0,
	'\\',
	'Z','X','C','V',
	'B','N','M',',',
	'.','/',
	0,
	'*',
	0,
	' ',
	0,0,0,0,
	0,0,0,0,
	0,0,0,0,
	0,
};

static char* shift_numbers = "!@#$%^&*()_+";

static void check_keys()
{
	if((s_ctrlkeys & (KEY_LSHIFT|KEY_LALT) == (KEY_LSHIFT|KEY_LALT))
		&& s_i8keycode2 == 0x2C)
	{
		//If we're entering rescue mode, we must send EOI!
		pic_send_eoi(1);
		enter_rescue_mode(1);
	}
}

static void _keyboard_ctrlkeys_update(u8 keychar,u8 event)
{
	u8 ctrlkey;
	
	ctrlkey = 0;
	if(keychar == KEY_RSHIFT) ctrlkey = CTRLKEYS_RSHIFT;
	else if(keychar == KEY_LSHIFT) ctrlkey = CTRLKEYS_LSHIFT;
	else if(keychar == KEY_LALT) ctrlkey = CTRLKEYS_LALT;
	
	if(event) s_ctrlkeys &= ~ctrlkey; //Released
	else s_ctrlkeys |= ctrlkey; //Pressed
}

void keyboard_interrupt(int idx)
{
	u8 status;
	u8 keycode,keychar;
	
	status = inb(0x64);
	if(status & 1)
	{
		keycode = inb(0x60);
		keychar = keycode & ~0x80;
		s_i8keycode2 = keychar;
		if(keychar == KEY_RSHIFT
			|| keychar == KEY_LSHIFT
			|| keychar == KEY_LALT)
		{
			_keyboard_ctrlkeys_update(keychar,keycode>>7);	
			s_i8keycode = 0;
		}
		else if(!(keycode & 0x80))
		{
			s_i8keycode = keychar;
			//kprintf("%c",keyboard_ascii());
		}
	}
	check_keys();
	
	pic_send_eoi(1);
}

u8 keyboard_scan_code()
{
	return s_i8keycode;
}

u8 keyboard_ascii()
{
	u8 ascii;
	
	if(s_i8keycode >= sizeof(s_keymap)) return 0;
	ascii = s_keymap[s_i8keycode];
	
	if(s_ctrlkeys & (CTRLKEYS_LSHIFT|CTRLKEYS_RSHIFT))
	{
		if((s_i8keycode >= 0x02) && (s_i8keycode <= 0x0D))
		{
			if(s_i8keycode >= 2)
				ascii = shift_numbers[s_i8keycode-2];
		}
		if(ascii == '[') ascii = '{';
		else if(ascii == ']') ascii = '}';
		else if(ascii == '\\') ascii = '|';
		else if(ascii == ';') ascii = ':';
		else if(ascii == '\'') ascii = '"';
		else if(ascii == ',') ascii = '<';
		else if(ascii == '.') ascii = '>';
		else if(ascii == '/') ascii = '?';
		else if(ascii == '`') ascii = '~';
	}
	else if(ascii >= 0x41 && ascii <= 0x5A) ascii += 0x20;
	
	return ascii;
}

void keyboard_reset_irq1_vars()
{
	s_i8keycode = 0;
}

void keyboard_enable()
{
	pic_set_mask(1,1);
}

void keyboard_disable()
{
	pic_set_mask(1,0);
}

void keyboard_install()
{
	pic_set_mask(1,1);
	idt_setup(_int9_handler,keyboard_interrupt,33,IDT_TYPE_INTERRUPT);
}
