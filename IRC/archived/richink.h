//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//
//				Copyright (c) 1996-1998  Microsoft Corporation
//
//	Module:	RichInk.h
//
//	Description:
//
//		This file contains the public interfaces to the richink.dll
//
//
//
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


#ifndef _RICHINK_
#define	_RICHINK_

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


//__________________________________________________________________________
//							
//	StringInFile
//	
//	Checks for string in a file
//
#ifdef _WIN32_WCE_EMULATION
int WINAPI StringInFile (
#else
int StringInFile (
#endif
	TCHAR		*pwszFileName, 
	TCHAR		*pwszSearchString
);

//__________________________________________________________________________
//							
//	StringInMemFile
//	
//	Checks for string in an memory file
//
#ifdef _WIN32_WCE_EMULATION
int WINAPI StringInMemFile (
#else
int StringInMemFile (
#endif
	LPBYTE		lpMemFile, 
	DWORD		dwCount, 
	TCHAR		*pwszSearchString
);


#ifdef _WIN32
#	define	_WPAD	/##/
#else
#	define	_WPAD	WORD
#endif

//#define cchTextLimitDefault 32767 // $$$ [a-kevpau] who uses this?

//--------------------------------
// RichInk messages
//
#define WM_CONTEXTMENU			0x007B
#define WM_PRINTCLIENT			0x0318
#if defined(RE_EM_GETLIMITTEXT)
#ifndef EM_GETLIMITTEXT
#define EM_GETLIMITTEXT			(WM_USER + 37)
#endif
#endif // RE_EM_GETLIMITTEXT


#if defined(RE_EM_POSFROMCHAR)
#ifndef EM_POSFROMCHAR	
#define EM_POSFROMCHAR			(WM_USER + 38)
#endif // RE_EM_POSFROMCHAR

#if defined(RE_EM_CHARFROMPOS)
#define EM_CHARFROMPOS			(WM_USER + 39)
#endif // RE_EM_CHARFROMPOS

#endif

#if defined(RE_EM_SCROLLCARET)
#ifndef EM_SCROLLCARET
#define EM_SCROLLCARET			(WM_USER + 49)
#endif
#endif // RE_EM_SCROLLCARET

#define EM_CANPASTE				(WM_USER + 50)
#define EM_DISPLAYBAND			(WM_USER + 51)
#define EM_EXGETSEL				(WM_USER + 52)
#define EM_EXLIMITTEXT			(WM_USER + 53)
#define EM_EXLINEFROMCHAR		(WM_USER + 54)
#define EM_EXSETSEL				(WM_USER + 55)
#define EM_FINDTEXT				(WM_USER + 56)
#define EM_FORMATRANGE			(WM_USER + 57)
#define EM_GETCHARFORMAT		(WM_USER + 58)
#define EM_GETEVENTMASK			(WM_USER + 59)
#define EM_GETOLEINTERFACE		(WM_USER + 60)
#define EM_GETPARAFORMAT		(WM_USER + 61)
#define EM_GETSELTEXT			(WM_USER + 62)
#define EM_HIDESELECTION		(WM_USER + 63)
#define EM_PASTESPECIAL			(WM_USER + 64)
#define EM_REQUESTRESIZE		(WM_USER + 65)
#define EM_SELECTIONTYPE		(WM_USER + 66)
#define EM_SETBKGNDCOLOR		(WM_USER + 67)
#define EM_SETCHARFORMAT		(WM_USER + 68)
#define EM_SETEVENTMASK			(WM_USER + 69)
#define EM_SETOLECALLBACK		(WM_USER + 70)
#define EM_SETPARAFORMAT		(WM_USER + 71)
#define EM_SETTARGETDEVICE		(WM_USER + 72)
#define EM_STREAMIN				(WM_USER + 73)
#define EM_STREAMOUT			(WM_USER + 74)
#define EM_GETTEXTRANGE			(WM_USER + 75)
#define EM_FINDWORDBREAK		(WM_USER + 76)
#define EM_SETOPTIONS			(WM_USER + 77)
#define EM_GETOPTIONS			(WM_USER + 78)
#define EM_FINDTEXTEX			(WM_USER + 79)
#define EM_GETWORDBREAKPROCEX	(WM_USER + 80)
#define EM_SETWORDBREAKPROCEX	(WM_USER + 81)

/* Far East specific messages */
#define EM_SETPUNCTUATION		(WM_USER + 100)
#define EM_GETPUNCTUATION		(WM_USER + 101)
#define EM_SETWORDWRAPMODE		(WM_USER + 102)
#define EM_GETWORDWRAPMODE		(WM_USER + 103)
#define EM_SETIMECOLOR			(WM_USER + 104)
#define EM_GETIMECOLOR			(WM_USER + 105)
#define EM_SETIMEOPTIONS		(WM_USER + 106)
#define EM_GETIMEOPTIONS		(WM_USER + 107)

/* new Pegasus outline mode messages */
#define EM_OUTLINE			    (WM_USER + 200)			// Outline mode msgs
#define EM_GETSCROLLPOS         (WM_USER + 201)			// Get and restore
#define EM_SETSCROLLPOS         (WM_USER + 202)
#define EM_SETFONTSIZE          (WM_USER + 203)			// Set new size

//$$$ New messages
#define EM_SELECT			    (WM_USER + 204)			// Select tool
#define EM_SPACE		        (WM_USER + 205)			// Space tool
#define	EM_SELECTALL			(WM_USER + 206)			// Select all contents of doc
#define	EM_SELECTTAP			(WM_USER + 207)			// Tap selection
#define	EM_SELECTYIELD			(WM_USER + 208)			// Yield selection
#define	EM_GETSELECTBOUNDS		(WM_USER + 209)			// Get the selection bounds
#define	EM_SELECTED				(WM_USER + 210)			// Something selected?
//#define	EM_TRANSLATESELECTION	(WM_USER + 211)			// Translate selection
#define	EM_GETSTYLE				(WM_USER + 212)			// Get style
#define	EM_SETSTYLE				(WM_USER + 213)			// Set style
#define	EM_SELECTSHOW			(WM_USER + 214)			// Show the selection
#define	EM_FINDSTRING			(WM_USER + 215)			// Find string
#define	EM_FINDNEXTSTRING		(WM_USER + 216)			// Find next string
#define	EM_REPLACESTRING		(WM_USER + 217)			// Replace string
#define	EM_REPLACEALLSTRING		(WM_USER + 218)			// Replace all strings
#define	EM_CASE					(WM_USER + 219)			// Set the case
#define	EM_INSERTASCII			(WM_USER + 220)			// Insert Ascii
#define	EM_PAGEBREAK			(WM_USER + 222)			// Insert Page break
#define	EM_GETDRAWBORDER		(WM_USER + 223)			// Get the draw border
#define	EM_SETDRAWBORDER		(WM_USER + 224)			// Set the draw border
#define	EM_GETLINEHEIGHT		(WM_USER + 225)			// Get the lineheight
#define	EM_SETLINEHEIGHT		(WM_USER + 226)			// Set the lineheight
#define	EM_GETWRAPMODE			(WM_USER + 227)			// Get the wrap mode (0 = window, 1 = page)

#define	EM_SETWRAPWIDTH			(WM_USER + 228)			// Set the wrap width in TWIPS (0 = wrap to window width)
#define	EM_GETDEFAULTFONT		(WM_USER + 229)			// Get the default font
#define	EM_SETDEFAULTFONT		(WM_USER + 230)			// Set the default font
#define	EM_GETCOLOR				(WM_USER + 231)			// Get the color
#define	EM_SETCOLOR				(WM_USER + 232)			// Set the color
#define	EM_DRAWHANDLEHIT		(WM_USER + 233)			// Draw selection hit?
#define	EM_UNDOEVENT			(WM_USER + 234)			// Undo last event
#define	EM_REDOEVENT			(WM_USER + 235)			// Redo last event
#define	EM_GETFINDREC			(WM_USER + 236)			// Get the find record
#define	EM_SETFINDCURSOR		(WM_USER + 237)			// Set the find cursor
#define	EM_ZOOM					(WM_USER + 238)			// Zoom to next line height
#define EM_CONVERT				(WM_USER + 239)			// Convert the bugger
#define EM_GETPEN				(WM_USER + 240)			// Get the pen style
#define EM_SETPEN				(WM_USER + 241)			// Set the pen style
#define EM_PEN					(WM_USER + 242)			// Set to pen state
#define EM_SMARTINK				(WM_USER + 243)			// Switch to SmartInk mode
#define EM_WRITINGINK			(WM_USER + 244)			// Switch to Writing mode
#define EM_DRAWINGINK			(WM_USER + 245)			// Switch to Drawing mode
#define EM_CANREDO				(WM_USER + 246)			// Can redo
#define EM_FLUSH				(WM_USER + 247)			// Flush the ink input
#define EM_QUIT					(WM_USER + 248)			// Quit the ink dll
#define EM_GETSELECTFINDSTRING	(WM_USER + 249)			// Get the selected string
#define EM_TYPINGVIEW			(WM_USER + 250)			// Typing only view
#define EM_WRITINGVIEW			(WM_USER + 251)			// Writing and Typing view
#define EM_DRAWINGVIEW			(WM_USER + 252)			// Writing and Typing view
#define EM_OUTLINEVIEW			(WM_USER + 253)			// Outline view
#define EM_GETVIEW				(WM_USER + 254)			// Get the view type
#define EM_GETOUTLINEVIEWLEVEL	(WM_USER + 255)			// Get outline view level
#define EM_SETOUTLINEVIEWLEVEL	(WM_USER + 256)			// Set outline view level
#define EM_OUTLINESELECT		(WM_USER + 257)			// Command for outline selections
#define EM_GETFAREAST			(WM_USER + 258)			// Get far east flag
#define EM_SETFAREAST			(WM_USER + 259)			// Set far east flag
#define EM_JOINSELECTION		(WM_USER + 260)			// Join the word breaks
#define EM_REVERTSELECTION		(WM_USER + 261)			// Revert back to ink words
#define EM_SETRECOGINITIONSTRING (WM_USER + 262)			// Set the recoginition string
#define EM_EXTVSCROLL			(WM_USER + 263)			// External ink control scroll bar
#define EM_TEXTSELECTED			(WM_USER + 264)			// External ink control scroll bar
#define EM_FLUSHIME				(WM_USER + 265)			// Flush ime string
#define EM_IMECLEAR				(WM_USER + 266)			// Flush ime string
#define EM_SELECTMODE			(WM_USER + 267)			// Is it in select mode
#define EM_GETZOOM				(WM_USER + 268)			// Returns the zoom level
#define EM_ISEMPTY				(WM_USER + 269)			// Returns the zoom level
#define EM_GETHEAPHANDLE		(WM_USER + 270)			// Returns the heap handle.
#define EM_SETHEAPHANDLE		(WM_USER + 271)			// Set the heap handle.
#define EM_SETHEAPMAXSIZE		(WM_USER + 272)			// Set the maximum heap size.
#define EM_FREEUNDO				(WM_USER + 273)			// Free undo/redo buffer.
#define EM_GETFILENAME			(WM_USER + 274)			// Get filename chars from document.

#define EM_SETAUTOSCROLL		(WM_USER + 275)			// Set the auto scroll boolean to wParam.
#define EM_GETAUTOSCROLL		(WM_USER + 276)			// Get the auto scroll boolean.
#define EM_SETAUTOCAPS			(WM_USER + 277)			// Set the auto capitalization boolean to wParam.
#define EM_GETAUTOCAPS			(WM_USER + 278)			// Get the auto capitalization boolean.
#define EM_SETINKSMOOTHING		(WM_USER + 279)			// Set the ink smoothing boolean to wParam.
#define EM_GETINKSMOOTHING		(WM_USER + 280)			// Get the ink smoothing boolean.

#define EM_COLOR_INDEX_TO_RGB   (WM_USER + 281)			// Color Palette Conversion
#define EM_COLOR_RGB_TO_INDEX   (WM_USER + 282)			// Color Palette Conversion
#define EM_PRINT				(WM_USER + 283)			// The print message
#define EM_SETVIEW				(WM_USER + 284)			// Set the view type
#define	EM_INSERTWORDSPACE		(WM_USER + 285)			// Insert word space at selection.
#define	EM_INSERTBLANKLINES		(WM_USER + 286)			// Insert blank lines at the selection.
#define	EM_SETPAGESTYLE			(WM_USER + 287)			// Set the page style (ruled lines, etc.)
#define EM_SETINKLAYER			(WM_USER + 288)			// Set the ink layer, writing, drawing, smart ink.

#define	EM_GETZOOMPERCENT		(WM_USER + 289)			// Get the % of zoom
#define	EM_SETZOOMPERCENT		(WM_USER + 290)			// Set the % of zoom

#define	EM_ISSELMULTOBJ			(WM_USER + 291)			// Does the selection contain multiple objects?
#define	EM_GETSELGROUPING		(WM_USER + 292)			// Does the selection contain a group? (FALSE no, TRUE yes; lParam return whether selection only contains group)
#define	EM_GROUPSELECTION		(WM_USER + 293)			// Groups the objects in the selection.
#define	EM_UNGROUPSELECTION		(WM_USER + 294)			// Ungroups the objects in the selection.

#define	EM_SETSELTOLINE			(WM_USER + 295)			// Translates the selection to a line
#define	EM_SETSELTOCIRCLE		(WM_USER + 296)			// Translates the selection to a circle
#define	EM_SETSELTORECTANGLE	(WM_USER + 297)			// Translates the selection to a rectangle
#define	EM_SETSELTOTRIANGLE		(WM_USER + 298)			// Translates the selection to a triangle

#define	EM_SETSELTOALIGNTOP					(WM_USER + 299)	// Aligns selected objects
#define	EM_SETSELTOALIGNBOTTOM				(WM_USER + 300)	// Aligns selected objects
#define	EM_SETSELTOALIGNLEFT				(WM_USER + 301)	// Aligns selected objects
#define	EM_SETSELTOALIGNRIGHT				(WM_USER + 302)	// Aligns selected objects
#define	EM_SETSELTOALIGNCENTERVERTICALLY	(WM_USER + 303)	// Aligns selected objects
#define	EM_SETSELTOALIGNCENTERHORIZONTALLY	(WM_USER + 304)	// Aligns selected objects
		
#define	EM_SETSELZORDER_BRINGFORWARD	(WM_USER + 305)	// Brings the selected object forward in zorder one level
#define	EM_SETSELZORDER_SENDBACKWARD	(WM_USER + 306)	// Sends the selected object backward in zorder one level
#define	EM_SETSELZORDER_BRINGTOFRONT	(WM_USER + 307)	// Brings the selected object all the way to the front of the zorder
#define	EM_SETSELZORDER_SENDTOBACK		(WM_USER + 308)	// Sends the selected object all the way to the bottom of the zorder

#define EM_SETSIPSTATE			(WM_USER + 309)			// Set the SIP state for the document.
#define EM_GETSIPSTATE			(WM_USER + 310)			// Get the SIP state of the document.
#define EM_ISINK				(WM_USER + 311)			// Returns TRUE if there's ink objects.
#define EM_ISSTYLE				(WM_USER + 312)			// Returns TRUE if there's style info.
#define EM_ISPROCESSINGINK		(WM_USER + 313)			// Returns TRUE if the engine is processing ink.

#define EM_GETPRINTPAGES		(WM_USER + 314)			// Returns the number of pages that will be printed.
#define EM_PRINTPAGE			(WM_USER + 315)			// Prints page number given in wParam using the hdc given in lParam.

#define EM_GETCANSELECTALL		(WM_USER + 316)			// Returns TRUE if the is anything to select

#define EM_GETDOCMARGINS		(WM_USER + 317)			// Gets document margins, used when printing.
#define EM_SETDOCMARGINS		(WM_USER + 318)			// Sets document margins, used when printing.
#define EM_SETWRAPMODE			(WM_USER + 319)			// Sets the wrap mode, used when printing.
#define EM_GETPAGESIZE			(WM_USER + 320)			// Gets the size of page, used when printing.	
#define EM_SETPAGESIZE			(WM_USER + 321)			// Sets the size of page, used when printing.
#define EM_WRAPFORPRINTING		(WM_USER + 322)			// Wrap the document for printing.
#define	EM_GETPAGESTYLE			(WM_USER + 323)			// Get the page style (ruled lines, etc.)
#define EM_WRAPDOCUMENT			(WM_USER + 324)			// Re-wrap the document after printing.
#define EM_SETPRINTPAGEOFFSET	(WM_USER + 325)			// Set the offset for the first printed page 
														// (wParam -- offset in device units, lParam -- hdc of printer)
#define EM_ENABLEUNDO			(WM_USER + 326)			// Enable or Disable logging undo transactions.
														// (wParam - true for enable, false for disable)
#define EM_UPDATESCROLLBARS		(WM_USER + 326)			// Enable or Disable logging undo transactions.


//--------------------------------
// Rich Ink Settings
//

// Outline commands
#define OL_DEMOTE			1
#define OL_PROMOTE			2
#define OL_SUBPOINT			3
#define OL_MOVEDOWN			4
#define OL_MOVEUP			5
#define OL_EXPAND			6
#define OL_COLLAPSE			7

// Outline view level
#define OL_ALL				0
#define OL_LEVEL1			1
#define OL_LEVEL2			2
#define OL_LEVEL3			3
#define OL_LEVEL4			4
#define OL_LEVEL5			5
#define OL_LEVEL6			6
#define OL_LEVEL7			7
#define OL_LEVEL8			8

// Define the richink wrap modes.
#define RI_WRAPTOPAGE			0
#define RI_WRAPTOWINDOW			1

// View types for EM_SETVIEW message.
// WARNING:  ANY CHANGES MUST BE MATCHED IN NPVIEW.H
#define VT_TYPINGVIEW		0
#define VT_OUTLINEVIEW		1
#define VT_WRITINGVIEW		2
#define VT_DRAWINGVIEW		3

// Define the ink layers for EM_SETINKLAYER message.
// WARNING:  ANY CHANGES MUST BE MATCHED IN NPVIEW.H
#define VL_SMARTINK				0
#define VL_WRITINGINK			1
#define VL_DRAWINGINK			2

// Page styles used by EM_SETPAGESTYLE message.
// WARNING:  ANY CHANGES MUST BE MATCHED IN NPPAGE.H
//  OR styles with the attribute PS_DOTTEDLINES to 
//	  display dotted lines.
#define PS_LEFTMARGIN			0x0000
#define PS_TOPMARGIN			0x0001
#define PS_RULEDLINES			0x0002
#define PS_GRIDLINES			0x0004
#define PS_TOPLEFTMARGIN		0x0008
#define PS_NONE					0x0010
#define PS_DOTTEDLINES			0x0020
#define PS_YELLOWBACKGROUND		0x0040

// Define zoom levels
#define ZL_75				0		// 75% - 8 lines
#define ZL_100				1		// 100% HPC is 6 lines
#define ZL_125				2		// 125% - 5 lines
#define ZL_150				3		// 150% - 4 lines
#define ZL_200				4		// 200% - 3 lines
#define ZL_300				5		// 300%

//--------------------------------
// RichInk messages for Parent
// WM_COMMAND messages:
#define PM_START				10000
#define PM_PEN					(PM_START)				// Set it back to pen
#define PM_SELECT				(PM_START+1)			// Selection mode
#define PM_SPACE				(PM_START+2)			// Spacing tool
#define PM_SELECTCHANGE			(PM_START+3)			// Update selection buttons
#define PM_SELECTNONE			(PM_START+4)			// No selection
#define PM_STYLE				(PM_START+5)			// Style dialogs.
#define PM_UPDATECMDBAR			(PM_START+6)			// Update the command bar
#define PM_ACCELERATOR			(PM_START+7)			// Accelerator key combination possibly entered
#define PM_ZOOMCHANGE			(PM_START+8)			// Zoom level has changed, lParam contains new zoom level

// GuyBark: Allow a way to notify the parent of system keyup and command presses.
#define PM_ACCELERATORSYSCMD    (PM_START+20)
#define PM_ACCELERATORSYSKEYUP  (PM_START+21)

// WM_NOTIFY messages;
#define NM_START				10100
#define NM_TRACKEVENTDONE		(NM_START+1)			// Select or space tracking event is done.
#define NM_VIEWCHANGE			(NM_START+2)			// Notification allows parent to set properties for zoom, page style and ink layer, return TRUE if you want to change anything
#define NM_NEXTDOC				(NM_START+3)			// Goto the next doc.
#define NM_PREVDOC				(NM_START+4)			// Goto the previous doc.

// mask values
#define VIEWCHANGE_ZOOM			0x1
#define VIEWCHANGE_PAGESTYLE	0x2
#define VIEWCHANGE_INKLAYER		0x4

typedef struct tagNMHDR_VIEWCHANGE
{
	NMHDR	nmhdr;
	UINT	mask;
	UINT	uView;			// The view just changed to
	UINT	uZoom;			// The zoom percent or level to change to
	UINT	uPageStyle;		// Set the line style shown in view
	UINT	uInkLayer;		// Set the ink layer in this view
} NMHDR_VIEWCHANGE, *PNMHDR_VIEWCHANGE;

// outline mode wparam valies
#define EMO_EXIT                0       // enter normal mode,  lparam ignored
#define EMO_ENTER               1       // enter outline mode, lparam ignored
#define EMO_PROMOTE             2       // LOWORD(lparam) == 0 ==>
                                        // promote  to body-text
                                        // LOWORD(lparam) != 0 ==>
                                        // promote/demote current selection
                                        // by indicated number of levels
#define EMO_EXPAND              3       // HIWORD(lparam) = EMO_EXPANDSELECTION
                                        // -> expands selection to level
                                        // indicated in LOWORD(lparam)
                                        // LOWORD(lparam) = -1/+1 corresponds
                                        // to collapse/expand button presses
                                        // in winword (other values are
                                        // equivalent to having pressed these
                                        // buttons more than once)
                                        // HIWORD(lparam) = EMO_EXPANDDOCUMENT
                                        // -> expands whole document to
                                        // indicated level
#define EMO_MOVESELECTION       4       // LOWORD(lparam) != 0 -> move current
                                        // selection up/down by indicated
                                        // amount

// EMO_EXPAND options
#define EMO_EXPANDSELECTION     0
#define EMO_EXPANDDOCUMENT      1


/* new notifications */

#define EN_MSGFILTER			0x0700
#define EN_REQUESTRESIZE		0x0701
#define EN_SELCHANGE			0x0702
#define EN_DROPFILES			0x0703
#define EN_PROTECTED			0x0704
#define EN_CORRECTTEXT			0x0705		// PenWin specific
#define EN_STOPNOUNDO			0x0706		// Back to new notifications
#define EN_IMECHANGE			0x0707		// Far East specific notifications
#define EN_SAVECLIPBOARD		0x0708		// Back to new notifications
#define EN_OLEOPFAILED			0x0709
#define EN_PARAGRAPHEXPANDED    0x0800
#define EN_ZOOM				    0x0801

/* event notification masks */
#define ENM_NONE				0x00000000
#define ENM_CHANGE				0x00000001
#define ENM_UPDATE				0x00000002
#define ENM_SCROLL				0x00000004
#define ENM_KEYEVENTS			0x00010000
#define ENM_MOUSEEVENTS			0x00020000
#define ENM_REQUESTRESIZE		0x00040000
#define ENM_SELCHANGE			0x00080000
#define ENM_DROPFILES			0x00100000
#define ENM_PROTECTED			0x00200000
#define ENM_CORRECTTEXT			0x00400000		// Pen specific

/* Far East specific notification mask */
#define ENM_IMECHANGE			0x00800000

/* new edit control styles */
#define ES_SAVESEL				0x00008000
#define ES_SUNKEN				0x00004000		// No sunken border support
#define ES_DISABLENOSCROLL		0x00002000
#define ES_SELECTIONBAR			0x01000000		// same as WS_MAXIMIZE
#define ES_EX_NOCALLOLEINIT		0x01000000		// Extended style

// Hack!!! set waitcursor code should be resided in the ink control - sunyw
// note this has same values as ES_OEMCONVERT which is not applicable to richink
#define ES_NOWAITCURSOR			0x0400L		// do not put up wait cursor

/* edit control options */
#define ECO_AUTOWORDSELECTION	0x00000001  //ES_CENTER
#define ECO_AUTOVSCROLL			0x00000040
#define ECO_AUTOHSCROLL			0x00000080
#define ECO_NOHIDESEL			0x00000100
#define ECO_READONLY			0x00000800
#define ECO_WANTRETURN			0x00001000
#define ECO_SAVESEL				0x00008000
#define ECO_SELECTIONBAR		0x01000000


// REVIEW: do these need to be exclusive with window styles?
#define ECO_OUTLINEMODE         0x00000002  // ES_RIGHT
#define ECO_WRAPTOWINDOW        0x00000020  // ES_PASSWORD

/* ECO operations */
#define ECOOP_SET				0x0001
#define ECOOP_OR				0x0002
#define ECOOP_AND				0x0003
#define ECOOP_XOR				0x0004

    // REVIEW: these should not be exposed
/* new word break function actions */
#define WB_CLASSIFY			3
#define WB_MOVEWORDLEFT		4
#define WB_MOVEWORDRIGHT	5
#define WB_LEFTBREAK		6
#define WB_RIGHTBREAK		7

/* Far East specific flags */
#define WB_MOVEWORDPREV		4
#define WB_MOVEWORDNEXT		5
#define WB_PREVBREAK		6
#define WB_NEXTBREAK		7


#define PC_FOLLOWING		1
#define	PC_LEADING			2
#define	PC_OVERFLOW			3
#define	PC_DELIMITER		4
#define WBF_WORDWRAP		0x010
#define WBF_WORDBREAK		0x020
#define	WBF_OVERFLOW		0x040	
#define WBF_LEVEL1			0x080
#define	WBF_LEVEL2			0x100
#define	WBF_CUSTOM			0x200


/* word break flags (used with WB_CLASSIFY) */
#define WBF_CLASS			((BYTE) 0x0F)
#define WBF_ISWHITE			((BYTE) 0x10)
#define WBF_BREAKLINE		((BYTE) 0x20)
#define WBF_BREAKAFTER		((BYTE) 0x40)


/* new data types */

#ifdef _WIN32
/* extended edit word break proc (character set aware) */
//typedef LONG (*EDITWORDBREAKPROCEX)(char *pchText, LONG cchText, BYTE bCharSet, INT action);
#endif


/* all character format measurements are in twips */
typedef struct _charformat
{
	UINT		cbSize;
	_WPAD		_wPad1;
	DWORD		dwMask;
	DWORD		dwEffects;
	LONG		yHeight;
	LONG		yOffset;			/* > 0 for superscript, < 0 for subscript */
	COLORREF	crTextColor;
	BYTE		bCharSet;
	BYTE		bPitchAndFamily;
	char		szFaceName[LF_FACESIZE];
	_WPAD		_wPad2;
} CHARFORMAT;

/* CHARFORMAT masks */
#define CFM_BOLD		0x00000001
#define CFM_ITALIC		0x00000002
#define CFM_UNDERLINE	0x00000004
#define CFM_STRIKEOUT	0x00000008
#define CFM_PROTECTED	0x00000010
#define CFM_SIZE		0x80000000
#define CFM_COLOR		0x40000000
#define CFM_FACE		0x20000000
#define CFM_OFFSET		0x10000000
#define CFM_CHARSET		0x08000000

/* CHARFORMAT effects */
#define CFE_BOLD		0x0001
#define CFE_ITALIC		0x0002
#define CFE_UNDERLINE	0x0004
#define CFE_STRIKEOUT	0x0008
#define CFE_PROTECTED	0x0010
/* NOTE: CFE_AUTOCOLOR corresponds to CFM_COLOR, which controls it */
#define CFE_AUTOCOLOR	0x40000000

//#define yHeightCharPtsMost 1638


/* EM_SETCHARFORMAT wParam masks */
#define SCF_SELECTION	0x0001
#define SCF_WORD		0x0002

typedef struct _charrange
{
	LONG	cpMin;
	LONG	cpMax;
    BOOL    fBOL;
} CHARRANGE;

typedef struct _textrange
{
	CHARRANGE chrg;
	LPSTR lpstrText;	/* allocated by caller, zero terminated by RichEdit */
} TEXTRANGE;


typedef DWORD (CALLBACK *EDITSTREAMCALLBACK)(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);

// From PWord-US, for handling passwords.
typedef struct
{
    BOOL        bReadEncrypted;
	int			iKey;
	DWORD		dwCacheSize;
	DWORD		dwCacheOffset;
	LPBYTE		pCache;
}
PWD_PASSWORDDATA;

#ifndef V1_BUILD
typedef struct _editstream
{
	DWORD dwCookie;		/* user value passed to callback as first parameter */
	DWORD dwError;		/* last error */
    EDITSTREAMCALLBACK pfnCallback;
} EDITSTREAM;
#else
typedef struct _editstream
{
	DWORD dwCookie;		/* user value passed to callback as first parameter */
	DWORD dwError;		/* last error */
#ifdef TARGET_NT
	BOOL  bLoss;		// Added by V-GUYB: for identifying that some loss occurred during rtf stream in
    BOOL  wExtraTab;    // Added by V-GUYB: for handling extra tab on bulleted lines during rtf stream in.
#endif // TARGET_NT
    EDITSTREAMCALLBACK pfnCallback;
} EDITSTREAM;
#endif

/* stream formats */

#define SF_UNKNOWN		-1
#define SF_TEXT			0x0001
#define SF_RTF			0x0002
#define SF_RTFNOOBJS	0x0003		// outbound only 
#define SF_TEXTIZED		0x0004		// outbound only 
#define SF_UTEXT		0x0005
#define SF_RTFDIB   	0x0006		// GuyBark: For writing out pictures as dibs instead of metafiles.
#define SF_UNDO         0x0010
#ifndef V1_BUILD
#define SFF_PWD			0x0800		// From PWord-US, which must interact with new richedit control.
#define SF_PWD  (SF_RTF | SFF_PWD | 0x010000)
#else
#define SF_PWD			0x005
#endif

#ifdef TARGET_NT
// V-GUYB Added to allow extra tabs to be ignored in input stream
#define RE_EXTRATAB_NOACTION    0x0001
#define RE_EXTRATAB_DETECT      0x0002
#define RE_EXTRATAB_BYPASS      0x0003
#endif // TARGET_NT

/* Flag telling stream operations to operate on the selection only */
/* EM_STREAMIN will replace the current selection */
/* EM_STREAMOUT will stream out the current selection */
#define SFF_SELECTION	0x8000

/* Flag telling stream operations to operate on the common RTF keyword only */
/* EM_STREAMIN will accept the only common RTF keyword */
/* EM_STREAMOUT will stream out the only common RTF keyword */
#define SFF_PLAINRTF	0x4000

//__________________________
//
//	Zoom bits
//
#define ZOOM_ALL		0x000F
#define ZOOM_NARROW		0x0001
#define ZOOM_COLLEGE	0x0002
#define ZOOM_STANDARD	0x0004
#define ZOOM_WIDE		0x0008


typedef struct _findtext
{
	CHARRANGE chrg;
	LPSTR lpstrText;
} FINDTEXT;

typedef struct _findtextex
{
	CHARRANGE chrg;
	LPSTR lpstrText;
	CHARRANGE chrgText;
} FINDTEXTEX;

typedef struct _formatrange
{
	HDC hdc;
	HDC hdcTarget;
	RECT rc;
	RECT rcPage;
	CHARRANGE chrg;
} FORMATRANGE;

/* all paragraph measurements are in twips */

#define MAX_TAB_STOPS 32
#define lDefaultTab 720

typedef struct _paraformat
{
	UINT	cbSize;
	_WPAD	_wPad1;
	DWORD	dwMask;
	WORD	wNumbering;
	WORD	wReserved;
	LONG	dxStartIndent;
	LONG	dxRightIndent;
	LONG	dxOffset;
	WORD	wAlignment;
	SHORT	cTabCount;
	LONG	rgxTabs[MAX_TAB_STOPS];
} PARAFORMAT;

/* PARAFORMAT mask values */
#define PFM_STARTINDENT			0x00000001
#define PFM_RIGHTINDENT			0x00000002
#define PFM_OFFSET				0x00000004
#define PFM_ALIGNMENT			0x00000008
#define PFM_TABSTOPS			0x00000010
#define PFM_NUMBERING			0x00000020
#define PFM_OFFSETINDENT		0x80000000

/* PARAFORMAT numbering options */
#define PFN_BULLET		0x0001

/* PARAFORMAT alignment options */
#define PFA_LEFT	0x0001
#define PFA_RIGHT	0x0002
#define PFA_CENTER	0x0003

/* notification structures */

#ifndef WM_NOTIFY
#define WM_NOTIFY				0x004E

typedef struct _nmhdr
{
	HWND	hwndFrom;
	_WPAD	_wPad1;
	UINT	idFrom;
	_WPAD	_wPad2;
	UINT	code;
	_WPAD	_wPad3;
} NMHDR;
#endif  /* !WM_NOTIFY */

typedef struct _msgfilter
{
	NMHDR	nmhdr;
	UINT	msg;
	_WPAD	_wPad1;
	WPARAM	wParam;
	_WPAD	_wPad2;
	LPARAM	lParam;
} MSGFILTER;

typedef struct _reqresize
{
	NMHDR nmhdr;
	RECT rc;
} REQRESIZE;

typedef struct _selchange
{
	NMHDR nmhdr;
	CHARRANGE chrg;
	WORD seltyp;
} SELCHANGE;

#define SEL_EMPTY		0x0000
#define SEL_TEXT		0x0001
#define SEL_OBJECT		0x0002
#define SEL_MULTICHAR	0x0004
#define SEL_MULTIOBJECT	0x0008

/* clipboard formats - use as parameter to RegisterClipboardFormat() */
#define CF_RTF TEXT("Rich Text Format")
#define CF_RTFNOOBJS TEXT("Rich Text Format Without Objects")
#define CF_RETEXTOBJ TEXT("RichEdit Text and Objects")

// Unsigned integers needed for structures below
typedef unsigned char			U8;		//  8-bit unsigned
typedef unsigned short			U16;	// 16-bit unsigned
typedef unsigned long			U32;	// 32-bit unsigned

//__________________________________
//
// Pen Settings
//
typedef struct _pensettings
{
	U16						color;
	U16						weight;
	U16						lineStyle;
#ifdef ENABLE_FILL
	U16						bgColor;
	U16						fillPattern;
#endif // ENABLE_FILL
} PENSETTINGS, *P_PENSETTINGS;

// For passing printer info to richink wndproc.
typedef struct _PrintPass
{
	U8		orientation;		// Portrait or landscape mode.
	BOOL	fPrintSelection;	// Print only the selection if true.
	HDC		hdcPrint;
	RECT	rcPrintMargin;
	U32		printBegLine;		// Beginning line to print in document.
	U32		printEndLine;		// Ending line to print in document.
} PRINTPASS, *P_PRINTPASS;

//________________________________
//
//	Prototypes
//
#ifdef _WIN32_WCE_EMULATION
void WINAPI InitRichInkDLL();
#else
void InitRichInkDLL();
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* !_RICHINK_ */

