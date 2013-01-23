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

/*
void set_led(int led_num, int state){
   NLED_SETTINGS_INFO settings;
   settings.LedNum=led_num;
   settings.OffOnBlink=state;
   NLedSetDevice(NLED_SETTINGS_INFO_ID,&settings);
}

VOID CALLBACK deactivate_led(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2){
   timer_led=NULL;
   set_led(config.led_number,0);
}

void activate_led(){
   if(timer_led!=NULL){
      return;
   }
   timer_led=timeSetEvent(config.led_interval, 50, deactivate_led, 0, TIME_ONESHOT|TIME_CALLBACK_FUNCTION);
   if(timer_led!=NULL){
      set_led(config.led_number,1);
   }
}
*/

const wchar_t* StaticLoadString(unsigned int var){
    static wchar_t buffer[IRC_SIZE_SMALL];
    LoadString(config.h_instance, var, buffer, IRC_SIZE_SMALL);
    return buffer;
}

char *alias_tokens(char *str, char character, int n_tokens, char **d_tokens, int *s_tokens){
   int in = 0;
   while(*str!=CHAR_TERMINATOR){
      while(*str==character){
         str++;
      }
      while(*str!=character && *str!=CHAR_TERMINATOR){
         if(in==0){
            d_tokens[*s_tokens] = str;
            (*s_tokens)++;
            in = 1;
         }
         str++;
      }
      if(in){
         if(*str!=CHAR_TERMINATOR){
            *str=CHAR_TERMINATOR;
            str++;
         }else{
            return NULL;
         }
         n_tokens--;
         if(n_tokens<=0){
            return str;
         }
         in = 0;
      }
   }
   return NULL;
}

void parsing_alias(irc_t *irc, char *chat_destination, char *chat_text){
   char temp_chat_text[IRC_SIZE_MEDIUM];
   char *send[2];
   strcpy(temp_chat_text,chat_text);
   char *d_tokens[10];
   int s_tokens = 0;
   alias_tokens(temp_chat_text,CHAR_SPACE,10,d_tokens,&s_tokens);
   if(strcmp(d_tokens[0],"/join")==0){
      //char *lol[1]={"PRIVMSG dark_ice lol"};
      send[0] = d_tokens[1];
      irc_send_message(irc,SEND_JOIN,send,1);
   }
}

int update_title(wchar_t *window_title, wchar_t *cmd_line){
   if(wcslen(cmd_line)==0){
      wcscat(window_title,L" Profile Manager");
   }else{
      wchar_t *end = wcsstr(cmd_line,L".ini");
      if(end==NULL){
         return -1;
      }
      wchar_t *start = wcsrchr(cmd_line,'\\');
      if(start==NULL){
         start=cmd_line;
      }else{
         start++;
      }
      wcscat(window_title,L" ");
      wcsncat(window_title,start,end-start);
   }
   return 0;
}

int validate_name(wchar_t *str){
   while(*str!='\0'){
      if(*str<48 || *str>122){
         return -1;
      }
      if(*str>57 && *str<65){
         return -1;
      }
      if(*str>90 && *str<96){
         return -1;
      }
      str++;
   }
   return 0;
}

void wfile_to_fullpath(wchar_t *wfilename, wchar_t *wfullpath){
   wcsncpy(wfullpath,config.module_path,IRC_SIZE_SMALL);
   wcscpy(wcsrchr(wfullpath,'\\')+1,wfilename);
}

int winiparser_load(iniparser_t *iniparser, wchar_t *wfilename){
   wchar_t wfullpath[IRC_SIZE_SMALL];
   char fullpath[IRC_SIZE_SMALL];
   wfile_to_fullpath(wfilename,wfullpath);
   WideCharToMultiByte(IRC_FILE_PATH_ENCODING,0,wfullpath,-1,fullpath,IRC_SIZE_SMALL,NULL,NULL);
   return iniparser_load(iniparser,fullpath);
}

int winiparser_store(iniparser_t *iniparser, wchar_t *wfilename){
   wchar_t wfullpath[IRC_SIZE_SMALL];
   char fullpath[IRC_SIZE_SMALL];
   wfile_to_fullpath(wfilename,wfullpath);
   WideCharToMultiByte(IRC_FILE_PATH_ENCODING,0,wfullpath,-1,fullpath,IRC_SIZE_SMALL,NULL,NULL);
   return iniparser_store(iniparser,fullpath);
}

int get_screen_caps_x(){
   HDC hDC = GetDC(NULL);
   if(hDC == NULL){
      return -1;
   }
   int x = GetDeviceCaps(hDC, LOGPIXELSX);
   ReleaseDC(NULL, hDC);
   return x;
}

int get_screen_caps_y(){
   HDC hDC = GetDC(NULL);
   if(hDC == NULL){
      return -1;
   }
   int y = GetDeviceCaps(hDC, LOGPIXELSY);
   ReleaseDC(NULL, hDC);
   return y;
}

inline int HIDPIMulDiv(int x, int y, int z){
   return ((((abs(x)*(y))+((z)>>1))/(z))*(((x)<0)?-1:1));
}

inline int SCALEX(int argX){
   return HIDPIMulDiv(argX, config.LOG_PIXELS_X, HIDPI);
}

inline int SCALEY(int argY){
   return HIDPIMulDiv(argY, config.LOG_PIXELS_Y, HIDPI);
}

inline int UNSCALEX(int argX){
   return HIDPIMulDiv(argX, HIDPI, config.LOG_PIXELS_X);
}

inline int UNSCALEY(int argY){
   return HIDPIMulDiv(argY, HIDPI, config.LOG_PIXELS_Y);
}

void refresh_client_sizes(int width, int height){
   resize.BORDER = SCALEX(1);
   resize.CLOSETAB_WIDTH = SCALEX(20);
   resize.CLOSETAB_HEIGHT = SCALEY(20);
   resize.BUTTONCHAT_WIDTH = SCALEY(40);
   resize.BUTTONCHAT_HEIGHT = SCALEY(20);
   resize.TABCONTROLCHAT_WIDTH = width-(resize.BORDER+resize.BORDER+resize.BORDER+resize.CLOSETAB_WIDTH);
   resize.TABCONTROLCHAT_HEIGHT = SCALEY(20);
   resize.EDITCHAT_WIDTH = width-(resize.BORDER+resize.BORDER+resize.BORDER+resize.BUTTONCHAT_WIDTH);
   resize.EDITCHAT_HEIGHT = SCALEY(20);
   resize.TABALL_HEIGHT = height-(resize.BORDER+resize.BORDER+resize.BORDER+resize.BORDER+resize.TABCONTROLCHAT_HEIGHT+resize.EDITCHAT_HEIGHT);
   resize.TABTALK_STATUS_WIDTH = width-(resize.BORDER+resize.BORDER);
   resize.TABNICK_CHAT_WIDTH = SCALEX(150);
   resize.TABTALK_CHAT_WIDTH = width-(resize.BORDER+resize.BORDER+resize.BORDER+resize.TABNICK_CHAT_WIDTH);

   resize.CLOSETAB_TOP = resize.BORDER;
   resize.CLOSETAB_LEFT = resize.BORDER+resize.BORDER+resize.TABCONTROLCHAT_WIDTH;
   resize.BUTTONCHAT_TOP = resize.BORDER+resize.BORDER+resize.BORDER+resize.TABCONTROLCHAT_HEIGHT+resize.TABALL_HEIGHT;
   resize.BUTTONCHAT_LEFT = resize.BORDER+resize.BORDER+resize.EDITCHAT_WIDTH;
   resize.TABCONTROLCHAT_TOP = resize.BORDER;
   resize.TABCONTROLCHAT_LEFT = resize.BORDER;
   resize.EDITCHAT_TOP = resize.BORDER+resize.BORDER+resize.BORDER+resize.TABCONTROLCHAT_HEIGHT+resize.TABALL_HEIGHT;
   resize.EDITCHAT_LEFT = resize.BORDER;
   resize.TABTALK_TOP = resize.BORDER+resize.BORDER+resize.TABCONTROLCHAT_HEIGHT;
   resize.TABTALK_LEFT = resize.BORDER;
   resize.TABNICK_TOP = resize.BORDER+resize.BORDER+resize.TABCONTROLCHAT_HEIGHT;
   resize.TABNICK_LEFT = resize.BORDER+resize.BORDER+resize.TABTALK_CHAT_WIDTH;
}

void refresh_manager_sizes(){
   resize.STATIC_WIDTH = SCALEX(75);
   resize.STATIC_HEIGHT = SCALEY(20);
   resize.STATIC_TOP = SCALEY(5);
   resize.STATIC_LEFT = SCALEX(5);
   resize.STATIC_LEFT_SECOND = SCALEX(170);

   resize.RADIO_WIDTH = SCALEX(180);
   resize.RADIO_HEIGHT = SCALEY(20);
   resize.RADIO_TOP_DISTANCE = SCALEY(25);
   resize.RADIO_TOP = SCALEY(20);
   resize.RADIO_LEFT = SCALEX(5);
}
