/*
 * display.c
 *
 *  Created on: 11/11/2016
 *      Author: marc.albajar
 */

#include "ZedboardOLED.h"
#include <stdio.h>
#include <string.h>
#include "sleep.h"



void connection_ok()
{

	clear();
	print_message("Connected",1);
}

void connection_failure()
{

	clear();
	print_message("ERROR: Cannot",2);
	print_message("connect",3);
}

void sequence_received_ok(){

	clear();
	print_message("Seq. received",2);
}

void sequence_received_failure(){

	clear();
	print_message("Sequence",1);
	print_message("not received",2);
}

void ready2play(void)
{
	clear();
	print_message("Want to play?",1);
	print_message("Y-Press a button",2);
	print_message("N-Press a switch",3);

}

void play_again(void){

	clear();
	print_message("Play again?",1);
	print_message("Y-Press a button",2);
	print_message("N-Press a switch",3);

}
void waiting_answer(void)
{
	clear();

		print_message("Waiting opponent",1);
		print_message("...",3);



}

void opponent_dont_want_to_play()
{
	clear();
	print_message("Opponent gave up",1);
	print_message("Closing game",2);
	sleep(5);
	clear();
}
void you_dont_want_to_play()
{
	clear();
	print_message("You gave up",1);
	print_message("Closing game...",2);
	sleep(5);
	clear();
}


void countdown2play(void)
{

	clear();
	print_message("      3...",1);
	sleep(1);

	clear();
	print_message("      2...",1);
	sleep(1);

	clear();
	print_message("      1...",1);
	sleep(1);

	clear();
	print_message("      GO!",1);
	sleep(1);

	clear();
	//print_message("   PLAYING",1); //extra: mostrar cronometre i leds to go.

}

void wrong_switch(){

	print_message("Wrong switch",3);
}

void update_leds2go(int count)
{
	clear();

	if (count==0){
		print_message("20 leds to go",1);
	}else if (count==1){
		print_message("19 leds to go",1);
	}else if (count==2){
		print_message("18 leds to go",1);
	}else if (count==3){
		print_message("17 leds to go",1);
	}else if (count==4){
		print_message("16 leds to go",1);
	}else if (count==5){
		print_message("15 leds to go",1);
	}else if (count==6){
		print_message("14 leds to go",1);
	}else if (count==7){
		print_message("13 leds to go",1);
	}else if (count==8){
		print_message("12 leds to go",1);
	}else if (count==9){
		print_message("11 leds to go",1);
	}else if (count==10){
		print_message("10 leds to go",1);
	}else if (count==11){
		print_message("9 leds to go",1);
	}else if (count==12){
		print_message("8 leds to go",1);
	}else if (count==13){
		print_message("7 leds to go",1);
	}else if (count==14){
		print_message("6 leds to go",1);
	}else if (count==15){
		print_message("5 leds to go",1);
	}else if (count==16){
		print_message("4 leds to go",1);
	}else if (count==17){
		print_message("3 leds to go",1);
	}else if (count==18){
		print_message("2 leds to go",1);
	}else if (count==19){
		print_message("1 leds to go",1);
	}else if (count==20){
		print_message("FINISH!",1);
	}

}

void disp_winner(void)
{
	clear();
	print_message("You WIN! :-)",1);
	sleep(5);
}

void disp_loser(void)
{
	clear();
	print_message("You lose :-(",1);
	sleep(5);
}
