#ifndef __BUTTON_H__
#define __BUTTON_H__


enum button_state_n{
	BUTTON_RELEASED,
	BUTTON_PRESSED
};
extern enum button_state_n button_state;
void Button_Mgmt();

#endif
