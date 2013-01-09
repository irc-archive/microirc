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
