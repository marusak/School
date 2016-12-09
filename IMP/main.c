/*******************************************************************************
   user_main: main for application using keyboard with interrupt
   Copyright (C) 2009 Brno University of Technology,
                      Faculty of Information Technology
   Author(s): Jan Markovic <xmarko04 AT stud.fit.vutbr.cz>

   LICENSE TERMS

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
   3. All advertising materials mentioning features or use of this software
      or firmware must display the following acknowledgement:

        This product includes software developed by the University of
        Technology, Faculty of Information Technology, Brno and its
        contributors.

   4. Neither the name of the Company nor the names of its contributors
      may be used to endorse or promote products derived from this
      software without specific prior written permission.

   This software or firmware is provided ``as is'', and any express or implied
   warranties, including, but not limited to, the implied warranties of
   merchantability and fitness for a particular purpose are disclaimed.
   In no event shall the company or contributors be liable for any
   direct, indirect, incidental, special, exemplary, or consequential
   damages (including, but not limited to, procurement of substitute
   goods or services; loss of use, data, or profits; or business
   interruption) however caused and on any theory of liability, whether
   in contract, strict liability, or tort (including negligence or
   otherwise) arising in any way out of the use of this software, even
   if advised of the possibility of such damage.

   $Id$


*******************************************************************************/

#include <fitkitlib.h>
#include <stdio.h>
#include <irq/fpga_interrupt.h>
#include <keyboard/keyboard.h>

#define STAY	0
#define UP	1
#define DOWN	2
#define	ERR	3

#define	m_ONE	0
#define ONE	1
#define	TWO	2
#define	THREE	3


int changed = 0;
char *floors[4] = {"-1", "1 ", "2 ", "3 "};
char *states[4] = {"STAY", " UP ", "DOWN", " ERR"};

typedef struct
{
	unsigned int state: 2;
	unsigned int floor: 2;
	unsigned int s_m_1: 1;
	unsigned int s_1: 1;
	unsigned int s_2: 1;
	unsigned int s_3: 1;
} control;

control c;

void print_user_help(void){}

/*******************************************************************************
 * Obsluha klavesnice
*******************************************************************************/
void keyboard_idle()
{
  unsigned char ch;
  while (keyboard_in_buf() != 0)
  {
    if ((ch = keyboard_get_char())!=0)
    {
	switch(ch) {
		case '1':
		case '2':
			if (!c.s_m_1){
			    changed = 1;
			    c.s_m_1 = 1;
			}
			break;
		case '4':
		case '5':
			if (!c.s_1){
			    c.s_1 = 1;
			    changed = 1;
			}
			break;
		case '7':
		case '8':
			if (!c.s_2){
			    c.s_2 = 1;
			    changed = 1;
			}
			break;
		case '*':
		case '0':
			if (!c.s_3){
			    c.s_3 = 1;
			    changed = 1;
			}
			break;
		case 'B':
			if (c.state != ERR){
			    c.state = ERR;
			    changed = 1;
			}
			break;
		case 'C':
			//TODO
			break;
		default:
      			term_send_str("Pressed unmapped key ");
      			term_send_char(ch);
      			term_send_crlf();

	}
    }
  }
}

void print_state(){
   term_send_str_crlf("******************************");
   term_send_str_crlf("*=========ON==FLOORS=========*");
   term_send_str_crlf("*      |  STATE   |  ACTUAL  *");
   term_send_str("*  -1  |  ");
   term_send_str(states[c.state]);
   term_send_str("    |     ");
   term_send_str(floors[c.floor]);
   term_send_str_crlf("   *");
   term_send_str("*   1  |  ");
   term_send_str(states[c.state]);
   term_send_str("    |     ");
   term_send_str(floors[c.floor]);
   term_send_str_crlf("   *");
   term_send_str("*   2  |  ");
   term_send_str(states[c.state]);
   term_send_str("    |     ");
   term_send_str(floors[c.floor]);
   term_send_str_crlf("   *");
   term_send_str("*   3  |  ");
   term_send_str(states[c.state]);
   term_send_str("    |     ");
   term_send_str(floors[c.floor]);
   term_send_str_crlf("   *");
   term_send_str_crlf("*========INSIDE=CABIN========*");
   term_send_str("*   STATE :      ");
   term_send_str(states[c.state]);
   term_send_str_crlf("        *");
   term_send_str("*   ACTUAL :        ");
   term_send_str(floors[c.floor]);
   term_send_str_crlf("       *");
   term_send_str_crlf("* VISITING | -1 | 1 | 2 | 3 |*");
   term_send_str("*          | ");
   if (c.s_m_1)
	   term_send_str(" O | ");
   else
	   term_send_str("   | ");
   if (c.s_1)
	   term_send_str("O | ");
   else
	   term_send_str("  | ");
   if (c.s_2)
	   term_send_str("O | ");
   else
	   term_send_str("  | ");
   if (c.s_3){
	   term_send_str("O |*");
	   term_send_crlf();
   }
   else
	   term_send_str_crlf("  |*");
   term_send_str_crlf("******************************");
   term_send_crlf();
}




/*******************************************************************************
 * Inicializace periferii/komponent po naprogramovani FPGA
*******************************************************************************/
void fpga_initialized()
{
  term_send_str_crlf("IRQ Init");
  fpga_interrupt_init(BIT2);
}

/*******************************************************************************
 * Dekodovani a vykonani uzivatelskych prikazu
*******************************************************************************/
unsigned char decode_user_cmd(char *cmd_ucase, char *cmd)
{
  return CMD_UNKNOWN;
}

/*******************************************************************************
 * Obsluha preruseni z FPGA
*******************************************************************************/
void fpga_interrupt_handler(unsigned char bits)
{
  if (bits & BIT2)
    keyboard_handle_interrupt(); 
}

/*******************************************************************************
 * Hlavni funkce
*******************************************************************************/
int main(void)
{
  c.state = STAY;
  c.floor = ONE;
  c.s_m_1 = 0;
  c.s_1 = 0;
  c.s_2 = 0;
  c.s_3 = 0;
  unsigned int cnt = 0;

  initialize_hardware();
  keyboard_init();

  while (1)
  {
    delay_ms(10);
    terminal_idle();                   // obsluha terminalu
    keyboard_idle();                   // obsluha klavesnice
    if (changed){
	    print_state();
	    changed = 0;
    }
  }         
}
