/*
 * bls.h
 *
 *  Created on: 12/12/2016
 *      Author: david.villafaina
 */

#ifndef BLS_H_
#define BLS_H_

#include "xparameters.h"
#include "xgpio.h"

int read_button (void);
int read_switches (void);
int play_game (int sequence[]);

#endif /* BLS_H_ */
