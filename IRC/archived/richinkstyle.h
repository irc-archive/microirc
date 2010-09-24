/*
* richinkstyle.h
*
* Original from http://channel9.msdn.com/wiki/mobiledeveloper/richink/
*
*/

#define EM_GETSTYLE (WM_USER + 212) // WPARAM is 0 and LPARAM is a pointer to RICHINKSTYLE 
#define EM_SETSTYLE (WM_USER + 213) // WPARAM is 0 and LPARAM is a pointer to RICHINKSTYLE 

#define RIS_WEIGHT_REGULAR 4
#define RIS_WEIGHT_BOLD    7

#define RIS_ALIGN_LEFT   0
#define RIS_ALIGN_RIGHT  1
#define RIS_ALIGN_CENTER 2

#define RIS_STYLE_NORMAL    0x0000
#define RIS_STYLE_STRIKEOUT 0x0001
#define RIS_STYLE_UNDERLINE 0x0002
#define RIS_STYLE_HIGHLIGHT 0x0404
#define RIS_STYLE_ITALIC    0x0008
#define RIS_STYLE_BOLD      0x0800

#define RIS_DIFF120_STRIKEOUT 0x01
#define RIS_DIFF120_UNDERLINE 0x02
#define RIS_DIFF120_HIGHLIGHT 0x04
#define RIS_DIFF120_ITALIC    0x08
#define RIS_DIFF120_FONTSIZE  0x10
#define RIS_DIFF120_FONTFACE  0x20

#define RIS_DIFF121_BOLD       0x08

#define RIS_DIFF122_COLOR      0x01
#define RIS_DIFF122_BOLD       0x04

// 124 bytes total
typedef struct _richinkstyle{
  DWORD dwUnknown0; // 5 - no selection
                    // 1 - selection length > 0
  WCHAR risFaceName[LF_FACESIZE];
  BYTE  byte068; // Flag bit. Normally 0, 8 if monospaced font?
  BYTE  byte069; // unknown, normally 0
  BYTE  risSize; // Integer for font size
  BYTE  byte071; // unknown, normally 0
  BYTE  risColor; // Integer for text color. Uses the color table output in an RTF file by the [RichInk] control
  BYTE  byte073; // Unknown, normally 0
  BYTE  byte074; // uninitialized
  BYTE  byte075; // uninitialized
  BYTE  risWeight;  // 4 regular (FW_REGULAR/100=4)
                 // 7 bold    (FW_BOLD/100=7)
  BYTE  byte077; // unknown, normally 0
  BYTE  byte078; // uninitialized
  BYTE  byte079; // uninitialized
  BYTE  byte080; // uninitialized
  BYTE  byte081; // uninitialized
  BYTE  byte082; // uninitialized
  BYTE  byte083; // uninitialized
  WORD  risStyle;   // 0x0000 - normal
                 // 0x0001 - strikeout
                 // 0x0002 - underlined
                 // 0x0004 - highlight
                 // 0x0008 - italic
                 // 0x0400 - highlight
                 // 0x0800 - bold is on
  BYTE  byte086; // unknown, normally 0
  BYTE  byte087; // unknown, normally 0
  BYTE  risAlign;   // 0 - normal
                 // 1 - right
                 // 2 - centered
  BYTE  byte089; // unknown, normally 0
  BYTE  byte090; // unknown, normally 0 (255 when the line has bullets)
  BYTE  byte091; // unknown, normally 0
  BYTE  byte092; // unknown, normally 0
  BYTE  byte093; // unknown, normally 0
  BYTE  byte094; // unknown, normally 0
  BYTE  byte095; // unknown, normally 0
  BYTE  byte096; // unknown, always 115 (but actually 145 in my test)
  BYTE  byte097; // unknown, always 12
  BYTE  byte098; // unknown, normally 0 (152 when the line has bullets)
  BYTE  byte099; // unknown, normally 0 (254 when the line has bullets)
  BYTE  byte100; // unknown, normally 0 (208 when the line has bullets)
  BYTE  byte101; // unknown, normally 0 (2 when the line has bullets)
  BYTE  byte102; // unknown, normally 0
  BYTE  byte103; // unknown, normally 0

  // bounding rect for the current selection region
  DWORD dwSelectionX; // pixels*4.5 (bytes 104-107)
  DWORD dwSelectionY; // from the bottom edge, pixels*4.5 (bytes 108-111)
  DWORD dwSelectionW; // pixels*4.5 (bytes 112-115) (0 if nothing selected)
  DWORD dwSelectionH; // pixels*4.5 (bytes 116-119) (one line height if nothing selected)

  BYTE  risDiff120; // Flag byte. Normally 255. If the selection overlaps formatting
                    //regions, 251 means different background highlighting, 247 means
                    //different italics, 253 if different underlining, 245 if different
                    //italics and different underlining, 229 if the font
                    //size/italics/bold/underline changes
                    //- Bits: 2 for underlining, 4 for highlighting, 8 for italics, 16 for
                    //font size change, 32 for font change
  BYTE  risDiff121; // Normally 127. More selection overlap info. If the overlap contains bold, this is 119.
                 // - Bits: 8 for bold
  BYTE  risDiff122; 
                 // 5 if all of the selection has the same format, 
                 // 4 if the selection covers different colors
                 //    clear this bit if you want to set the entire selection to a single color
                 // 1 if the selection covers different bolding
                 //    clear this bit if you want to set the entire selection to a single bolding
  BYTE  byte123; // seen as zero
  BYTE  byte124; // uninitialized
  BYTE  byte125; // uninitialized
  BYTE  byte126; // uninitialized
  BYTE  byte127; // uninitialized
  BYTE  byte128; // uninitialized
  BYTE  byte129; // uninitialized
} RICHINKSTYLE;

/*
bool [wxTextCtrl::SetStyle(long] start, long end, const [wxTextAttr&] textAttr){
   // TODO: if the selection does not match start/end, save the selection & select start/end
   // get the existing risStyle to it can be modified
   RICHINKSTYLE richinkstyle;
   LRESULT lresult = [::SendMessage(GetHwnd(),] EM_GETSTYLE, 0, (LPARAM)&richinkstyle) != 0;
   if([textAttr.HasFont()]){
      wxFont font = [textAttr.GetFont();]
      const LOGFONT* lf = [&font.GetNativeFontInfo()-&gt;lf;]
      if(lf-&gt;lfWeight == FW_BOLD){
         richinkstyle.risStyle |= RIS_STYLE_BOLD;
         richinkstyle.risWeight = FW_BOLD/100;
      }else{
         richinkstyle.risStyle &= ~RIS_STYLE_BOLD;
         richinkstyle.risWeight = FW_NORMAL/100;
      }
      wxStrncpy( [richinkstyle.risFaceName,] [lf-&gt;lfFaceName,] [WXSIZEOF(richinkstyle.risFaceName)] );
      if(lf-&gt;lfUnderline){
         richinkstyle.risStyle |= RIS_STYLE_UNDERLINE;
      }else{
         richinkstyle.risStyle &= ~RIS_STYLE_UNDERLINE;
      }
      if(lf-&gt;lfItalic){
         richinkstyle.risStyle |= RIS_STYLE_ITALIC;
      }else{
         richinkstyle.risStyle &= ~RIS_STYLE_ITALIC;
      }
      if([lf-&gt;lfStrikeOut]){
         richinkstyle.risStyle |= RIS_STYLE_STRIKEOUT;
      }else{
         richinkstyle.risStyle &= ~RIS_STYLE_STRIKEOUT;
      }
      int height = abs(lf-&gt;lfHeight);
      richinkstyle.risSize = ((height * 72)+48) / 96;
      richinkstyle.risDiff120 |= RIS_DIFF120_STRIKEOUT | RIS_DIFF120_UNDERLINE | RIS_DIFF120_HIGHLIGHT | RIS_DIFF120_ITALIC | RIS_DIFF120_FONTSIZE | RIS_DIFF120_FONTFACE | 0;
      richinkstyle.risDiff121 |= RIS_DIFF121_BOLD | 0;
      richinkstyle.risDiff122 |= RIS_DIFF122_BOLD | 0;
   }
   if([textAttr.HasTextColour()]){
      richinkstyle.risColor = richinkstyle.risSize;
      richinkstyle.risDiff122 |= RIS_DIFF122_COLOR | 0;
   }
   if([textAttr.HasAlignment()]){
      switch[(textAttr.GetAlignment())]{
         case wxTEXT_ALIGNMENT_DEFAULT:
         case wxTEXT_ALIGNMENT_LEFT:
            richinkstyle.risAlign = RIS_ALIGN_LEFT;
            break;
         case wxTEXT_ALIGNMENT_CENTRE:
         case wxTEXT_ALIGNMENT_JUSTIFIED:
            richinkstyle.risAlign = RIS_ALIGN_CENTER;
            break;
         case wxTEXT_ALIGNMENT_RIGHT:
            richinkstyle.risAlign = RIS_ALIGN_RIGHT;
            break;
      }
   }
   // set the modified risStyle
   lresult = [::SendMessage(GetHwnd(),] EM_SETSTYLE, 0, (LPARAM)&richinkstyle) != 0;
   return true;
}

// this function returns a structure that is used by the Dump routines to 
// step from data element to data element in a RICHINKSTYLE structure
char* [GetRisTypes()]{
   static char risTypes[124];
   static bool first = true;
   if (first){
      memset(risTypes, 1, sizeof(risTypes)); // treat all unknown elements as 1 byte
      risTypes[0] = 4; // the size of the member at byte 0 is 4 bytes
      risTypes[4] = 64; // 32 unicode characters starting at byte 4
      risTypes[84] = 2; // the size of the member at byte 84 is 3 bytes
      risTypes[104] = 4; // the size of the member at byte 104 is 4 bytes
      risTypes[108] = 4; // the size of the member at byte 108 is 4 bytes
      risTypes[112] = 4; // the size of the member at byte 112 is 4 bytes
      risTypes[116] = 4; // the size of the member at byte 116 is 4 bytes
      first = false;
   }
   return risTypes;
}

// prints a RICHINKSTYLE in a readable form
void [DumpSelection(] int lineNumber, RICHINKSTYLE* [richInkStyle]){
   BYTE* ris = (BYTE*)richInkStyle;
   const char* risTypes = [GetRisTypes();]
   wxChar line[1024];
   wxChar* p = line;
   p += wxSprintf(p, wxT("%i"), lineNumber);
   int j = 0;
   while(j &lt; 124){
      int dataSize = risTypes[j];
      *p++ = ',';
      switch(dataSize){
         case 1:
            p += wxSprintf(p, wxT("%i"), *((BYTE*)(ris+j)));
            break;
         case 2:
            p += wxSprintf(p, wxT("%i"), *((WORD*)(ris+j)));
            break;
         case 4:
            p += wxSprintf(p, wxT("%i"), *((DWORD*)(ris+j)));
            break;
         case 64:
            p += wxSprintf(p, wxT("%s"), ris+j);
            break;
      }
      j += dataSize;
   }
   *p++ = 0;
   wxLogDebug(wxT("%s"),line);
}

// The first time this function is called, the RICHINKSTYLE is stored for later use
// In subsequent calls, the RICHINKSTYLE is compared to the last RICHINKSTYLE
// and fields that have changed are output
void [DumpDiff(] int lineNumber, RICHINKSTYLE* [richInkStyle]){
   BYTE* ris = (BYTE*)richInkStyle;
   const char* risTypes = [GetRisTypes();]
   wxChar line[1024];
   wxChar* p = line;
   static bool first = true;
   static BYTE lastRis[126];
   if(!first){
      int j = 0;
      while(j &lt; 124){
      int dataSize = risTypes[j];
      *p++ = ',';
      if(dataSize == 64){
         wxChar* last = (wxChar*)(lastRis+j);
         wxChar* current = (wxChar*)(ris+j);
         if(0 != wxStrcmp(last,current)){
            wxSprintf(line, wxT("%i[%i]: '%s'-&gt;'%s'\n"), lineNumber, j, last, current);
            [OutputDebugString(] line );
         }
      }else{
         DWORD last;
         DWORD current;
         switch(dataSize){
            case 1:
            default:
               last = lastRis[j];
               current = ris[j];
               break;
            case 2:
               last = *((WORD*)(lastRis+j));
               current = *((WORD*)(ris+j));
               break;
            case 4:
               last = *((DWORD*)(lastRis+j));
               current = *((DWORD*)(ris+j));
               break;
         }
         if(last != current){
            wxSprintf(line, wxT("%i[%i]: %i-&gt;%i\n"), lineNumber, j, last, current);
            [OutputDebugString(] line );
         }
      }
      j += dataSize;
      }
   }
   first = false;
   memcpy(lastRis, ris, sizeof(lastRis));
}

// dumps style information about every cursor position in the text control
void [wxTextCtrl::Dump()]{
   LRESULT len = [::SendMessage(GetHwnd(),] WM_GETTEXTLENGTH, 0, 0);
   const int selected = 0;
   for(int i = 0; i &lt; len-selected; i++){
      [::SendMessage(GetHwnd(),] EM_SETSEL, i, i+selected);
      RICHINKSTYLE risCurrent;
      [::SendMessage(GetHwnd(),] EM_GETSTYLE, 0, (LPARAM)&risCurrent) != 0;
      [::DumpSelection(] i, (BYTE*)&risCurrent );
      [DumpDiff(] i, (BYTE*)&risCurrent );
   }
}

// dumps style information about the selected text
void [wxTextCtrl::DumpSelection()]{
   static int lineNumber = 0;
   RICHINKSTYLE ris;
   [::SendMessage(GetHwnd(),] EM_GETSTYLE, 0, (LPARAM)&ris) != 0;
   [::DumpSelection(] lineNumber, &ris);
   [DumpDiff(] lineNumber, &ris);
   lineNumber++;
}

bool [wxTextCtrl::ToggleSelectionBold()]{
   if([(GetWindowStyle()] & wxTE_RICH)==0){
      return false;
   }
   // get the existing risStyle to it can be modified
   RICHINKSTYLE richinkstyle;
   LRESULT lresult = [::SendMessage(GetHwnd(),] EM_GETSTYLE, 0, (LPARAM)&richinkstyle) != 0;
   bool bold = richinkstyle.risWeight == FW_BOLD/100; // the selection is bold
   if(bold){
      richinkstyle.risWeight = FW_NORMAL/100;
      richinkstyle.risStyle &= ~RIS_STYLE_BOLD;
   }else{
      richinkstyle.risStyle |= RIS_STYLE_BOLD;
      richinkstyle.risWeight = FW_BOLD/100;
   }
   richinkstyle.risDiff121 |= RIS_DIFF121_BOLD; // not really necessary?
   richinkstyle.risDiff122 |= RIS_DIFF122_BOLD;
   // set the modified risStyle
   lresult = [::SendMessage(GetHwnd(),] EM_SETSTYLE, 0, (LPARAM)&richinkstyle) != 0;
   return true;
}

bool [wxTextCtrl::IsSelectionBold()]{
   if([(GetWindowStyle()] & wxTE_RICH)==0){
      return false;
   }
   // get the existing risStyle to it can be modified
   RICHINKSTYLE richinkstyle;
   LRESULT lresult = [::SendMessage(GetHwnd(),] EM_GETSTYLE, 0, (LPARAM)&richinkstyle) != 0;
   bool bold = richinkstyle.risWeight == FW_BOLD/100; // the selection is bold
   return bold;
}

bool [wxTextCtrl::ToggleSelectionItalic()]{
   if([(GetWindowStyle()] & wxTE_RICH)==0){
      return false;
   }
   // get the existing risStyle to it can be modified
   RICHINKSTYLE richinkstyle;
   LRESULT lresult = [::SendMessage(GetHwnd(),] EM_GETSTYLE, 0, (LPARAM)&richinkstyle) != 0;
   bool italic = (richinkstyle.risStyle&RIS_STYLE_ITALIC) == RIS_STYLE_ITALIC; // the selection is italic
   if(italic){
      richinkstyle.risStyle &= ~RIS_STYLE_ITALIC;
   }else{
      richinkstyle.risStyle |= RIS_STYLE_ITALIC;
   }
   richinkstyle.risDiff120 |= RIS_DIFF120_ITALIC;
   // set the modified risStyle
   lresult = [::SendMessage(GetHwnd(),] EM_SETSTYLE, 0, (LPARAM)&richinkstyle) != 0;
   return true;
}

bool [wxTextCtrl::IsSelectionItalic()]{
   if([(GetWindowStyle()] & wxTE_RICH)==0){
      return false;
   }
   // get the existing risStyle to it can be modified
   RICHINKSTYLE richinkstyle;
   LRESULT lresult = [::SendMessage(GetHwnd(),] EM_GETSTYLE, 0, (LPARAM)&richinkstyle) != 0;
   bool italic = (richinkstyle.risStyle&RIS_STYLE_ITALIC) == RIS_STYLE_ITALIC; // the selection is italic
   return italic;
}

bool [wxTextCtrl::ToggleSelectionUnderline()]{
   if([(GetWindowStyle()] & wxTE_RICH)==0){
      return false;
   }
   // get the existing risStyle to it can be modified
   RICHINKSTYLE richinkstyle;
   LRESULT lresult = [::SendMessage(GetHwnd(),] EM_GETSTYLE, 0, (LPARAM)&richinkstyle) != 0;
   bool underline = (richinkstyle.risStyle&RIS_STYLE_UNDERLINE) == RIS_STYLE_UNDERLINE; // the selection is underlined
   if(underline){
      richinkstyle.risStyle &= ~RIS_STYLE_UNDERLINE;
   }else{
      richinkstyle.risStyle |= RIS_STYLE_UNDERLINE;
   }
   richinkstyle.risDiff120 |= RIS_DIFF120_UNDERLINE;
   // set the modified risStyle
   lresult = [::SendMessage(GetHwnd(),] EM_SETSTYLE, 0, (LPARAM)&richinkstyle) != 0;
   return true;
}

bool [wxTextCtrl::IsSelectionUnderline()]{
   if([(GetWindowStyle()] & wxTE_RICH)==0){
      return false;
   }
   // get the existing risStyle to it can be modified
   RICHINKSTYLE richinkstyle;
   LRESULT lresult = [::SendMessage(GetHwnd(),] EM_GETSTYLE, 0, (LPARAM)&richinkstyle) != 0;
   bool underline = (richinkstyle.risStyle&RIS_STYLE_UNDERLINE) == RIS_STYLE_UNDERLINE; // the selection is underlined
   return underline;
}

bool [wxTextCtrl::ToggleSelectionBullets()]{
   if([(GetWindowStyle()] & wxTE_RICH)==0){
      return false;
   }
   // get the existing risStyle to it can be modified
   RICHINKSTYLE richinkstyle;
   LRESULT lresult = [::SendMessage(GetHwnd(),] EM_GETSTYLE, 0, (LPARAM)&richinkstyle) != 0;
   bool bullets = richinkstyle.byte090 == 255;
   if(bullets){
      richinkstyle.byte090  = 0;
      richinkstyle.byte098  = 0;
      richinkstyle.byte099  = 0;
      richinkstyle.byte100  = 0;
      richinkstyle.byte101  = 0;
   }else{
      richinkstyle.byte090  = 255;
      richinkstyle.byte098  = 152;
      richinkstyle.byte099  = 254;
      richinkstyle.byte100  = 208;
      richinkstyle.byte101  = 2;
   }
   // set the modified risStyle
   lresult = [::SendMessage(GetHwnd(),] EM_SETSTYLE, 0, (LPARAM)&richinkstyle) != 0;
   return true;
}

bool [wxTextCtrl::IsSelectionBullets()]{
   if([(GetWindowStyle()] & wxTE_RICH)==0){
      return false;
   }
   // get the existing risStyle to it can be modified
   RICHINKSTYLE richinkstyle;
   LRESULT lresult = [::SendMessage(GetHwnd(),] EM_GETSTYLE, 0, (LPARAM)&richinkstyle) != 0;
   bool bullets = richinkstyle.byte090 == 255;
   return bullets;
}
*/
