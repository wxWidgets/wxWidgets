/////////////////////////////////////////////////////////////////////////////
// Name:        uma.h
// Purpose:     Universal MacOS API
// Author:      Stefan Csomor
// Modified by:
// Created:     03/02/99
// RCS-ID:      $Id: 
// Copyright:   (c) Stefan Csomor
// Licence:   	LGPL licence
/////////////////////////////////////////////////////////////////////////////

#ifndef H_UMA
#define H_UMA

// define this to be 1 if you have the 8.6 libs (weak linked)

#define UMA_USE_8_6 0 

#if __POWERPC__
#define UMA_USE_APPEARANCE 1
#define UMA_USE_WINDOWMGR 1
#else
#define UMA_USE_APPEARANCE 0
#define UMA_USE_WINDOWMGR 0
#endif

#if !UMA_USE_8_6 && UMA_USE_WINDOWMGR
#undef UMA_USE_WINDOWMGR
#define UMA_USE_WINDOWMGR 0
#endif

#if !TARGET_CARBON
//  this is now defined in the latest headers
//	typedef short MenuItemIndex  ;
#endif

void UMAInitToolbox( UInt16 inMoreMastersCalls) ;
bool UMAHasAppearance() ;
long UMAGetAppearanceVersion() ;

// process manager

long UMAGetProcessMode() ;
bool UMAGetProcessModeDoesActivateOnFGSwitch() ;

// menu manager

void 			UMASetMenuTitle( MenuRef menu , StringPtr title ) ;
UInt32 			UMAMenuEvent( EventRecord *inEvent ) ;
void 			UMAEnableMenuItem( MenuRef inMenu , MenuItemIndex item ) ;	
void 			UMADisableMenuItem( MenuRef inMenu , MenuItemIndex item ) ;	
void			UMAAppendSubMenuItem( MenuRef menu , StringPtr label , SInt16 submenuid ) ;
void			UMAInsertSubMenuItem( MenuRef menu , StringPtr label , MenuItemIndex item , SInt16 submenuid  ) ;
void			UMAAppendMenuItem( MenuRef menu , StringPtr label , SInt16 key= 0, UInt8 modifiers = 0 ) ;
void			UMAInsertMenuItem( MenuRef menu , StringPtr label , MenuItemIndex item , SInt16 key = 0 , UInt8 modifiers = 0 ) ;
void			UMASetMenuItemText( MenuRef menu , MenuItemIndex item , StringPtr label ) ;

MenuRef			UMANewMenu( SInt16 menuid , StringPtr label ) ;
void 			UMADisposeMenu( MenuRef menu ) ;

// handling the menubar

void			UMADeleteMenu( SInt16 menuId ) ;
void			UMAInsertMenu( MenuRef insertMenu , SInt16 afterId ) ;
void			UMADrawMenuBar() ;

// quickdraw

void			UMAShowWatchCursor() ;
void			UMAShowArrowCursor() ;

void			UMAPrOpen() ;
void			UMAPrClose() ;

// window manager

GrafPtr		UMAGetWindowPort( WindowRef inWindowRef ) ;
void		 	UMADisposeWindow( WindowRef inWindowRef ) ;
void 			UMASetWTitleC( WindowRef inWindowRef , const char *title ) ;
void 			UMAGetWTitleC( WindowRef inWindowRef , char *title ) ;

void 			UMADrawGrowIcon( WindowRef inWindowRef ) ;
void			UMAShowWindow( WindowRef inWindowRef ) ;
void			UMAHideWindow( WindowRef inWindowRef ) ;
void			UMAShowHide( WindowRef inWindowRef , Boolean show) ;
void 			UMASelectWindow( WindowRef inWindowRef ) ;
void			UMABringToFront( WindowRef inWindowRef ) ;
void			UMASendBehind( WindowRef inWindowRef , WindowRef behindWindow ) ;
void			UMACloseWindow(WindowRef inWindowRef) ;

// appearance manager

void 			UMADrawControl( ControlHandle inControl ) ;

void 			UMAActivateControl( ControlHandle inControl ) ;
void			UMADeactivateControl( ControlHandle inControl ) ;
void			UMAApplyThemeBackground			(ThemeBackgroundKind 	inKind,
								 const Rect *			bounds,
								 ThemeDrawState 		inState,
								 SInt16 				inDepth,
								 Boolean 				inColorDev);
void			UMASetThemeWindowBackground		(WindowRef 				inWindow,
								 ThemeBrush 			inBrush,
								 Boolean 				inUpdate)	;
ControlHandle UMANewControl(WindowPtr 				owningWindow,
								 const Rect *			boundsRect,
								 ConstStr255Param 		controlTitle,
								 Boolean 				initiallyVisible,
								 SInt16 				initialValue,
								 SInt16 				minimumValue,
								 SInt16 				maximumValue,
								 SInt16 				procID,
								 SInt32 				controlReference)	;
void UMADisposeControl (ControlHandle 			theControl)	;
void UMAHiliteControl	(ControlHandle 			theControl,
								 ControlPartCode 		hiliteState)	;
void UMAShowControl						(ControlHandle 			theControl)	;
void UMAHideControl						(ControlHandle 			theControl);
void UMASetControlVisibility			(ControlHandle 			inControl,
								 Boolean 				inIsVisible,
								 Boolean 				inDoDraw);

bool UMAIsControlActive					(ControlHandle 			inControl);
bool UMAIsControlVisible				(ControlHandle 			inControl);
void UMAActivateControl					(ControlHandle 			inControl);
void UMADeactivateControl				(ControlHandle 			inControl);

OSErr UMAGetBestControlRect				(ControlHandle 			inControl,
								 Rect *					outRect,
								 SInt16 *				outBaseLineOffset);
OSErr UMASetControlFontStyle				(ControlHandle 			inControl,
								 const ControlFontStyleRec * inStyle)	;


void UMAMoveControl( ControlHandle inControl , short x , short y ) ;
void UMASizeControl( ControlHandle inControl , short x , short y ) ;
// control hierarchy

OSErr UMACreateRootControl				(WindowPtr 				inWindow,
								 ControlHandle *		outControl) ;

OSErr UMAEmbedControl					(ControlHandle 			inControl,
								 ControlHandle 			inContainer);

// keyboard focus
OSErr UMASetKeyboardFocus				(WindowPtr 				inWindow,
								 ControlHandle 			inControl,
								 ControlFocusPart 		inPart)	;


// events

ControlPartCode UMAHandleControlClick				(ControlHandle 			inControl,
								 Point 					inWhere,
								 SInt16 				inModifiers,
								 ControlActionUPP 		inAction) ;
SInt16 UMAHandleControlKey				(ControlHandle 			inControl,
								 SInt16 				inKeyCode,
								 SInt16 				inCharCode,
								 SInt16 				inModifiers);
								 
void UMAIdleControls					(WindowPtr 				inWindow)	;

void UMAUpdateControls( WindowPtr inWindow , RgnHandle inRgn ) ;
OSErr UMAGetRootControl( WindowPtr inWindow , ControlHandle *outControl ) ;

// handling control data

OSErr UMASetControlData					(ControlHandle 			inControl,
								 ControlPartCode 		inPart,
								 ResType 				inTagName,
								 Size 					inSize,
								 Ptr 					inData)		;

OSErr UMAGetControlData					(ControlHandle 			inControl,
								 ControlPartCode 		inPart,
								 ResType 				inTagName,
								 Size 					inBufferSize,
								 Ptr 					outBuffer,
								 Size *					outActualSize) ;
OSErr UMAGetControlDataSize				(ControlHandle 			inControl,
								 ControlPartCode 		inPart,
								 ResType 				inTagName,
								 Size *					outMaxSize);


// system 8.0

short 		UMAFindWindow( Point inPoint , WindowRef *outWindow ) ;
OSStatus 	UMAGetWindowFeatures( WindowRef inWindowRef , UInt32 *outFeatures ) ;
OSStatus 	UMAGetWindowRegion( WindowRef inWindowRef , WindowRegionCode inRegionCode , RgnHandle ioWinRgn ) ;
OSStatus 	UMACollapseWindow( WindowRef inWindowRef , Boolean inCollapseIt ) ;
OSStatus 	UMACollapseAllWindows( Boolean inCollapseEm ) ;
Boolean 	UMAIsWindowCollapsed( WindowRef inWindowRef ) ;
Boolean UMAIsWindowCollapsable( WindowRef inWindowRef ) ;

// system 8.5

OSStatus 	UMACreateNewWindow( WindowClass windowClass , WindowAttributes attributes , const Rect *bounds,  WindowRef *outWindow ) ;
OSStatus 	UMAGetWindowClass( WindowRef inWindowRef , WindowClass *outWindowClass ) ;
OSStatus	UMAGetWindowAttributes( WindowRef inWindowRef , WindowAttributes *outAttributes ) ;

void 			UMAShowFloatingWindows() ;
void 			UMAHideFloatingWindows() ;
Boolean 	UMAAreFloatingWindowsVisible() ;
WindowRef UMAFrontWindow() ;
WindowRef UMAFrontNonFloatingWindow() ;

// floaters support

bool			UMAIsWindowFloating( WindowRef inWindow ) ;
bool			UMAIsWindowModal( WindowRef inWindow ) ;
WindowRef UMAGetActiveWindow() ;
WindowRef UMAGetActiveNonFloatingWindow() ;

void UMAHighlightAndActivateWindow( WindowRef inWindowRef , bool inActivate ) ;

#if !TARGET_CARBON
#define GetPortTextFont( p) ((p)->txFont )
#define GetPortTextSize( p) ((p)->txSize )
#define GetPortTextFace( p) ((p)->txFace )
#define GetPortTextMode( p) ((p)->txMode )
#define GetRegionBounds( r , b) ((*b) = (**r).rgnBBox)
#define GetPortBounds( p , b) ((*b) = p->portRect )
#define GetWindowPortBounds( p , b) ((*b) = p->portRect )
#define	GetPortVisibleRegion( p, r ) CopyRgn( p->visRgn , r )
#define GetQDGlobalsWhite( a ) (&((*a) = qd.white))
#define GetQDGlobalsBlack( a ) (&((*a) = qd.black))
#define GetQDGlobalsScreenBits( a ) (*a) = qd.screenBits
#define GetQDGlobalsArrow( a ) (&((*a) = qd.arrow))
#define GetControlBounds( c , b ) ((*b) = (**c).contrlRect )
#define GetPortBitMapForCopyBits( p ) ((BitMap*) &(((CGrafPtr)p)->portPixMap ))
#endif

// Appearance Drawing

OSStatus UMADrawThemePlacard( const Rect *inRect , ThemeDrawState inState ) ;

#endif