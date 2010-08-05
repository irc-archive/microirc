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

char *strstri(char *t, char *s){//lstrcmpi
   int i, j;
   for(i=0; t[i] != '\0'; i++){
      for(j=0; s[j] != '\0'; j++){
         if(toupper(s[j])==toupper(t[i+j])){
            continue; 
         }else{
            break;
         }
      }
      if (s[j] == '\0'){
         break;
      }
   }
   if (s[j] == '\0'){
      return (i+t);
   }else{
      return '\0';
   }
}

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
    timer_led=timeSetEvent(config.ledinterval, 50, deactivate_led, 0, TIME_ONESHOT|TIME_CALLBACK_FUNCTION|TIME_KILL_SYNCHRONOUS);
    if(timer_led!=NULL){
       set_led(config.lednumber,1);
    }
}

void settext_fromstr(HWND hDlg, int control, char *edittext){
   wchar_t wedittext[IRCPROTOCOL_SIZE_MEDIUM];
   MultiByteToWideChar(IRC_CONFIG_FILE_ENCODING,0,edittext,-1,wedittext,IRCPROTOCOL_SIZE_MEDIUM);
   HWND edithwnd = GetDlgItem(hDlg,control);
   Edit_SetText(edithwnd,wedittext);
}

void settext_fromint(HWND hDlg, int control, int edittext){
   wchar_t wedittext[IRCPROTOCOL_SIZE_SMALL];
   wsprintf(wedittext,L"%d",edittext);
   HWND edithwnd = GetDlgItem(hDlg,control);
   Edit_SetText(edithwnd,wedittext);
}

void gettext_tostr(HWND hDlg, int control, char *edittext, int size){
   wchar_t wedittext[IRCPROTOCOL_SIZE_MEDIUM];
   HWND edithwnd = GetDlgItem(hDlg,control);
   Edit_GetText(edithwnd,wedittext,IRCPROTOCOL_SIZE_MEDIUM);
   WideCharToMultiByte(IRC_CONFIG_FILE_ENCODING,0,wedittext,-1,edittext,size,NULL,NULL);
}

int gettext_toint(HWND hDlg, int control){
   wchar_t wedittext[IRCPROTOCOL_SIZE_SMALL];
   HWND edithwnd = GetDlgItem(hDlg,control);
   Edit_GetText(edithwnd,wedittext,IRCPROTOCOL_SIZE_SMALL);
   return wcstol(wedittext,L'\0',10);
}
