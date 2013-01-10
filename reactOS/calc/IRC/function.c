#include "irc.h"

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
   return HIDPIMulDiv(argX, g_config.LOG_PIXELS_X, HIDPI);
}

inline int SCALEY(int argY){
   return HIDPIMulDiv(argY, g_config.LOG_PIXELS_Y, HIDPI);
}

inline int UNSCALEX(int argX){
   return HIDPIMulDiv(argX, HIDPI, g_config.LOG_PIXELS_X);
}

inline int UNSCALEY(int argY){
   return HIDPIMulDiv(argY, HIDPI, g_config.LOG_PIXELS_Y);
}

void refresh_client_sizes(int width, int height){
   resize.BORDER = SCALEX(1);
   resize.CLOSETAB_WIDTH = SCALEX(20);
   resize.CLOSETAB_HEIGHT = SCALEY(20);
   resize.BUTTONCHAT_WIDTH = SCALEY(40);
   resize.BUTTONCHAT_HEIGHT = SCALEY(20);
   resize.TABCONTROLCHAT_WIDTH = width-(BORDER+BORDER+BORDER+CLOSETAB_WIDTH);
   resize.TABCONTROLCHAT_HEIGHT = SCALEY(20);
   resize.EDITCHAT_WIDTH = width-(BORDER+BORDER+BORDER+BUTTONCHAT_WIDTH);
   resize.EDITCHAT_HEIGHT = SCALEY(20);
   resize.TABALL_HEIGHT = height-(BORDER+BORDER+BORDER+BORDER+TABCONTROLCHAT_HEIGHT+EDITCHAT_HEIGHT);
   resize.TABTALK_STATUS_WIDTH = width-(BORDER+BORDER);
   resize.TABNICK_CHAT_WIDTH = SCALEX(70);
   resize.TABTALK_CHAT_WIDTH = width-(BORDER+BORDER+BORDER+TABNICK_CHAT_WIDTH);

   resize.CLOSETAB_TOP = BORDER;
   resize.CLOSETAB_LEFT = BORDER+BORDER+TABCONTROLCHAT_WIDTH;
   resize.BUTTONCHAT_TOP = BORDER+BORDER+BORDER+TABCONTROLCHAT_HEIGHT+TABALL_HEIGHT;
   resize.BUTTONCHAT_LEFT = BORDER+BORDER+EDITCHAT_WIDTH;
   resize.TABCONTROLCHAT_TOP = BORDER;
   resize.TABCONTROLCHAT_LEFT = BORDER;
   resize.EDITCHAT_TOP = BORDER+BORDER+BORDER+TABCONTROLCHAT_HEIGHT+TABALL_HEIGHT;
   resize.EDITCHAT_LEFT = BORDER;
   resize.TABTALK_TOP = BORDER+BORDER+TABCONTROLCHAT_HEIGHT;
   resize.TABTALK_LEFT = BORDER;
   resize.TABNICK_TOP = BORDER+BORDER+TABCONTROLCHAT_HEIGHT;
   resize.TABNICK_LEFT = BORDER+BORDER+TABTALK_CHAT_WIDTH;
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
