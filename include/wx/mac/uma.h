/////////////////////////////////////////////////////////////////////////////
// Name:        uma.h
// Purpose:     Universal MacOS API
// Author:      Stefan Csomor
// Modified by:
// Created:     03/02/99
// RCS-ID:      $Id: 
// Copyright:   (c) Stefan Csomor
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef H_UMA
#define H_UMA

#include "wx/mac/private.h"

void UMAInitToolbox( UInt16 inMoreMastersCalls) ;
void UMACleanupToolbox() ;
long UMAGetSystemVersion() ;
bool UMAHasAppearance() ;
long UMAGetAppearanceVersion() ;
bool UMAHasWindowManager() ;
long UMAGetWindowManagerAttr() ;
bool UMAHasAquaLayout() ;
bool UMASystemIsInitialized() ;

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

// quickdraw

void			UMAShowWatchCursor() ;
void			UMAShowArrowCursor() ;

OSStatus		UMAPrOpen(void *macPrintSession) ;
OSStatus		UMAPrClose(void *macPrintSession) ;

// window manager

GrafPtr		UMAGetWindowPort( WindowRef inWindowRef ) ;
void		 	UMADisposeWindow( WindowRef inWindowRef ) ;
void 			UMASetWTitleC( WindowRef inWindowRef , const char *title ) ;
void 			UMAGetWTitleC( WindowRef inWindowRef , char *title ) ;

void 			UMADrawGrowIcon( WindowRef inWindowRef ) ;
void			UMAShowHide( WindowRef inWindowRef , Boolean show) ;

// appearance manager

void 			UMADrawControl( ControlHandle inControl ) ;

void 			UMAEnableControl( ControlHandle inControl ) ;
void			UMADisableControl( ControlHandle inControl ) ;
void 			UMAActivateControl( ControlHandle inControl ) ;
void			UMADeactivateControl( ControlHandle inControl ) ;
//								 ControlPartCode 		hiliteState)	;
void UMAShowControl						(ControlHandle 			theControl)	;
void UMAHideControl						(ControlHandle 			theControl);
void UMAActivateControl					(ControlHandle 			inControl);
void UMADeactivateControl				(ControlHandle 			inControl);

void UMAMoveControl( ControlHandle inControl , short x , short y ) ;
void UMASizeControl( ControlHandle inControl , short x , short y ) ;
// control hierarchy

// keyboard focus
OSErr UMASetKeyboardFocus				(WindowPtr 				inWindow,
								 ControlHandle 			inControl,
								 ControlFocusPart 		inPart)	;

// events

void UMAUpdateControls( WindowPtr inWindow , RgnHandle inRgn ) ;
OSErr UMAGetRootControl( WindowPtr inWindow , ControlHandle *outControl ) ;

// handling control data
bool			UMAIsWindowFloating( WindowRef inWindow ) ;
bool			UMAIsWindowModal( WindowRef inWindow ) ;

void UMAHighlightAndActivateWindow( WindowRef inWindowRef , bool inActivate ) ;

OSStatus UMAGetHelpMenu(
  MenuRef *        outHelpMenu,
  MenuItemIndex *  outFirstCustomItemIndex);      /* can be NULL */

// Appearance Drawing

OSStatus UMADrawThemePlacard( const Rect *inRect , ThemeDrawState inState ) ;

// accessor helpers

#if !TARGET_CARBON
#define ClearCurrentScrap() ZeroScrap() ;
#define GetApplicationScript() smSystemScript
#else

// calls not in carbon

#endif
#define GetWindowUpdateRgn( inWindow , updateRgn ) GetWindowRegion( inWindow , kWindowUpdateRgn, updateRgn ) 

#endif
