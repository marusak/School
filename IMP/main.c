/*
 *  Vytah opat nechodi
 *  main.c
 *
 *  Main module of Elevator controling written for FitKit.
 *
 *  Author: Matej Marusak, xmarus06@stud.fit.vutbr.cz
 *
 *  9.12.2016
 *  95% original (lines 305-329 are from Demo - klavesnice s prerusenim)
 *  Created as school project at VUT FIR Brno
*/

#include <fitkitlib.h>
#include <stdio.h>
#include <irq/fpga_interrupt.h>
#include <keyboard/keyboard.h>

// Define states of elevator
#define STAY	0
#define UP	1
#define DOWN	2
#define	ERR	3

// Define floors
#define	m_ONE	0
#define ONE	1
#define	TWO	2
#define	THREE	3

// All the controling variables
int i;
int j;
unsigned cnt;
unsigned fixing;
unsigned err_cnt;
unsigned err_way;
unsigned needed_time;
unsigned needed_way;
unsigned opening;
unsigned closing;
unsigned waiting;
int changed = 0;
char stops[4] = {' ', ' ', ' ', ' '};
typedef struct
{
	unsigned int state: 2;
	unsigned int floor: 2;
	unsigned int s_m_1: 1;
	unsigned int s_1: 1;
	unsigned int s_2: 1;
	unsigned int s_3: 1;
} control;

// Helpul strings for better printing
char *states[4] = {"STAY", " UP ", "DOWN", " ERR"};
char *floors[4] = {"-1", "1 ", "2 ", "3 "};
char doors[4] = {'C', 'O', 'C', 'C'};

control c;

void print_user_help(void){}

//Add next stopping floor
void stop_at(char stop){
	for (i = 0; i < 4; i++){
		if (stops[i] == stop)
			break;
		if (stops[i] == ' '){
		       stops[i] = stop;
	       	       break;
		}
	}
}

//Squash stops to front of array
void squash_stops(){
	for (i = 0; i < 3; i++){
		if (stops[i] == ' '){
			for (j = i; j < 3; j++){
				stops[j] = stops[j+1];
				stops[j+1] = ' ';
			}
		}
	}
}

//Remove stopping floor
void remove_stop(char stop){
	for (i = 0; i < 4; i++){
		if (stops[i] == stop)
			break;
	}
	if (i > 3)
		return;
	stops[i] = ' ';
	squash_stops();
}

//Print all stopping floors for debuging purposes
void debug_stops(){
	term_send_str("DEBUG stops: ");
	for (i = 0; i < 4; i++){
		term_send_char(stops[i]);
	}
	term_send_crlf();
}

// Return true if stop floor has any request on it
int waiting_at_stop(char stop){
	for (i = 0; i < 4; i++){
		if (stops[i] == stop)
			return 1;
	}
	return 0;
}



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
	// If elevator if broken and not repair signal - ignore all other options
	if (c.state == ERR && ch != 'C'){
		term_send_str_crlf("Vytah opat nechodi...");
		continue;
	}
	switch(ch) {
		case '1':
		case '2':
			if (!c.s_m_1){
			    if (c.state == STAY && c.floor == 0 && !closing)
				    continue;
			    if (closing && c.floor == 0){
				    closing = 0;
				    opening = 1;
				    cnt = 100 - cnt;
			    }
			    else{
				    c.s_m_1 = 1;
				    changed = 1;
				    stop_at('0');
			    }
			}
			break;
		case '4':
		case '5':
			if (!c.s_1){
			    if (c.state == STAY && c.floor == 1 && !closing)
				    continue;
			    if (closing && c.floor == 1){
				    closing = 0;
				    opening = 1;
				    cnt = 100 - cnt;
			    }
			    else{
				    c.s_1 = 1;
				    changed = 1;
				    stop_at('1');
			    }
			}
			break;
		case '7':
		case '8':
			if (!c.s_2){
			    if (c.state == STAY && c.floor == 2 && !closing)
				    continue;
			    if (closing && c.floor == 2){
				    closing = 0;
				    opening = 1;
				    cnt = 100 - cnt;
			    }
			    else{
				    c.s_2 = 1;
				    changed = 1;
				    stop_at('2');
			    }
			}
			break;
		case '*':
		case '0':
			if (!c.s_3){
			    if (c.state == STAY && c.floor == 3 && !closing)
				    continue;
			    if (closing && c.floor == 3){
				    closing = 0;
				    opening = 1;
				    cnt = 100 - cnt;
			    }
			    else{
				    c.s_3 = 1;
				    changed = 1;
				    stop_at('3');
				}
			}
			break;
		case 'B':
			if (c.state != ERR){
			    err_way = c.state;
			    c.state = ERR;
			    changed = 1;
			    err_cnt = cnt;
			}
			break;
		case 'C':
			if (c.state != ERR){
				term_send_str_crlf("Elevator is not broken. Nothing to repare.");
			}
			else{
				if (doors[c.floor] == 'O' && c.floor != 1){
					closing = 1;
				}
				fixing = 1;
				c.s_m_1 = 0;
				c.s_1 = 0;
				c.s_2 = 0;
				c.s_3 = 0;
				remove_stop('0');
				remove_stop('1');
				remove_stop('2');
				remove_stop('3');
				cnt = 0;
			}
			break;
		default:
      			term_send_str("Pressed unmapped key ");
      			term_send_char(ch);
      			term_send_crlf();
	}
    }
  }
}

//Function for printing current state
void print_state(){
   term_send_str_crlf("*==========ON==FLOORS========*");
   term_send_str_crlf("*    | STATE | CURR | DOOR | *");
   term_send_str("* -1 | ");
   term_send_str(states[c.state]);
   term_send_str("  |   ");
   term_send_str(floors[c.floor]);
   term_send_str("  |  ");
   term_send_char(doors[0]);
   term_send_str_crlf("  | *");
   term_send_str("*  1 | ");
   term_send_str(states[c.state]);
   term_send_str("  |   ");
   term_send_str(floors[c.floor]);
   term_send_str("  |  ");
   term_send_char(doors[1]);
   term_send_str_crlf("  | *");
   term_send_str("*  2 | ");
   term_send_str(states[c.state]);
   term_send_str("  |   ");
   term_send_str(floors[c.floor]);
   term_send_str("  |  ");
   term_send_char(doors[2]);
   term_send_str_crlf("  | *");
   term_send_str("*  3 | ");
   term_send_str(states[c.state]);
   term_send_str("  |   ");
   term_send_str(floors[c.floor]);
   term_send_str("  |  ");
   term_send_char(doors[3]);
   term_send_str_crlf("  | *");
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
   else{
	   term_send_str_crlf("  |*");
   }
   term_send_str_crlf("*============================*");
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

// Start movement of cabin
void start_moving(){
	if (c.floor+48 < stops[0]){
		c.state = UP;
		changed = 1;
	}
	else if (c.floor+48 > stops[0]){
		c.state = DOWN;
		changed = 1;
	}
	else
		remove_stop(stops[0]);
}

//Begin fixing elevator
void fix_elevator(){
	if (err_cnt && err_way != STAY){
	    if (c.floor == 1){
	        needed_time = err_cnt;
		needed_way = ERR;
	    }
	    else if (err_way == UP && c.floor == 0){
		needed_time = 500 - err_cnt;
		needed_way = UP;
	    }
	    else if (err_way == UP && c.floor){
		needed_time = err_cnt + 500;
		needed_way = DOWN;
	    }
	    else if (err_way == DOWN){
		needed_time = 500 - err_cnt;
		needed_way = DOWN;
	    }
 	    err_cnt = 0;
	    fixing = 0;
	}
	else if (c.floor != 1){
	    needed_time = 500;
	    needed_way = c.floor > 1 ? DOWN : UP;
	    fixing = 0;
	}
	else{
	    c.state = STAY;
	    changed = 1;
	    fixing = 0;
	    needed_time = 0;
	    if (doors[1] == 'C'){
	    	opening = 1;
	    }
	}
	cnt = 0;
}

/*******************************************************************************
 * Hlavni funkce
*******************************************************************************/
int main(void)
{
  // Init all variables after reset
  cnt = 0;
  fixing = 0;
  c.state = STAY;
  c.floor = ONE;
  c.s_m_1 = 0;
  c.s_1 = 0;
  c.s_2 = 0;
  c.s_3 = 0;
  opening = 0;
  closing = 0;
  waiting = 0;

  initialize_hardware();
  keyboard_init();
  print_state();
  while (1)
  {
    // Do not run too fast
    delay_ms(10);
    cnt++;
    terminal_idle();                   // obsluha terminalu
    keyboard_idle();                   // obsluha klavesnice
    //This part is described in documentation
    if (closing){
	    if (cnt > 100){
		    cnt = 0;
		    doors[c.floor] = 'C';
		    closing = 0;
		    changed = 1;
	    }
    }

    else if (c.state == ERR && fixing){
	fix_elevator();
    }

    else if (c.state == ERR && needed_time && cnt >= needed_time){
	if (needed_way == UP)
		c.floor++;
    	else if (needed_way == DOWN)
		c.floor--;
	cnt = 0;
	changed = 1;
	fix_elevator();
    }
    
    else if (waiting){
	   if(cnt > 200){
	      cnt = 0;
	      waiting = 0;
	   }
    }
    else if (opening){
	   if(cnt > 100){
		    cnt = 0;
		    doors[c.floor] = 'O';
		    opening = 0;
		    changed = 1;
		    waiting = 1;
	   }
    }


    else if (c.state == STAY && stops[0] != ' '){
	if (doors[c.floor] == 'C'){
		start_moving();
	}
	else{
		closing = 1;
	}
	cnt = 0;
    }
    
    else if (cnt > 500 && (c.state == UP || c.state == DOWN)){
	if (c.state == UP)
		c.floor++;
	else
		c.floor--;
	if (waiting_at_stop((char)(c.floor+48))){
		c.state = STAY;
		opening = 1;
		remove_stop((char)(c.floor+48));
		switch (c.floor){
			case 0:
				c.s_m_1 = 0;
				break;
			case 1:
				c.s_1 = 0;
				break;
			case 2:
				c.s_2 = 0;
				break;
			case 3:
				c.s_3 = 0;
				break;
		}
	}
	cnt = 0;
	changed = 1;
    }
    if (changed){
	    print_state();
	    changed = 0;
    }
  }         
}
