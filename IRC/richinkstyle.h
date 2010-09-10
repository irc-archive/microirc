/*
* irc.cpp
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
typedef struct _richinkstyle
{
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
