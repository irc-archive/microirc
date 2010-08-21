/*
* functions.h
*
* This file contains some generic functions.
*
* Copyright(C) 2009-2010, Diogo Reis <diogoandre12@gmail.com>
* Copyright(C) 2010-2010, José Pedroso <josedpedroso@gmail.com>
*
* This code is licenced under the GPL version 2. For details see COPYING.txt file.
*/

void set_led(int led_num, int state){
   NLED_SETTINGS_INFO settings;
   settings.LedNum=led_num;
   settings.OffOnBlink=state;
   NLedSetDevice(NLED_SETTINGS_INFO_ID,&settings);
}

VOID CALLBACK deactivate_led(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2){
   timer_led=NULL;
   set_led(config.lednumber,0);
}

void activate_led(){
   if(timer_led!=NULL){
      return;
   }
   timer_led=timeSetEvent(config.ledinterval, 50, deactivate_led, 0, TIME_ONESHOT|TIME_CALLBACK_FUNCTION);
   if(timer_led!=NULL){
      set_led(config.lednumber,1);
   }
}
