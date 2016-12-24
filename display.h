/*
 * display.h
 *
 *  Created on: 28/11/2016
 *      Author: david.martinez.cid
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

void connection_ok(void);
void connection_failure(void);
void ready2play(void);
void play_again(void);
void countdown2play(void);
void wrong_switch(void);
void waiting_answer(void);
void opponent_dont_want_to_play(void);
void you_dont_want_to_play(void);
void update_leds2go(int);
void disp_winner(void);
void disp_loser(void);
void disp_results(int);
void sequence_received_ok(void);
void sequence_received_failure(void);



#endif /* DISPLAY_H_ */
