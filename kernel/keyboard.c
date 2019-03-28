#include "keyboard.h"
#include "exceptions.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"
#include "stdlib.h"
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

static char* eng_layout = "qwertyuiop[]\\asdfghjkl;'zxcvbnm`,./";
static char* rus_layout = "йцукенгшщзхъ\\фывапролджэячсмитьёбю.";
static int cur_layout = 0;

static char* shift_numbers = "!@#$%^&*()_+";

static void check_keys()
{
	if((s_ctrlkeys & (CTRLKEYS_LSHIFT|CTRLKEYS_LALT)) 
		== (CTRLKEYS_LSHIFT|CTRLKEYS_LALT))
		cur_layout = !cur_layout;
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

void keyboard_interrupt()
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

static dbcs_t keyboard_layout_get_sym(u8 _chr)
{
	u64 i;
	u8 chr = ktolower(_chr);
	for(i = 0; i < 34; i++)
		if(eng_layout[i] == chr) break;
	if(i == 34) return (dbcs_t)_chr;
	if(cur_layout == 1)
	{
		char* sym = rus_layout;
		while(i--)
			sym += kchrlen(*sym);
		return kchr2dbcs(sym);
	}
	
	return (dbcs_t)_chr;
}

dbcs_t keyboard_sym()
{
	return keyboard_layout_get_sym(keyboard_ascii());
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
	idt_set_handler(33,keyboard_interrupt,IDT_TYPE_INTR);
}
