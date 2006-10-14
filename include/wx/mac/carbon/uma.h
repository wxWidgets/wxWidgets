/////////////////////////////////////////////////////////////////////////////
// Name:        uma.h
// Purpose:     Universal MacOS API
// Author:      Stefan Csomor
// Modified by:
// Created:     03/02/99
// RCS-ID:      $Id:
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef H_UMA
#define H_UMA

#include "wx/mac/private.h"

void UMAInitToolbox( UInt16 inMoreMastersCalls, bool isEmbedded) ;
void UMACleanupToolbox() ;
long UMAGetSystemVersion() ;

bool UMASystemIsInitialized() ;
void UMASetSystemIsInitialized(bool val);

// process manager

long UMAGetProcessMode() ;
bool UMAGetProcessModeDoesActivateOnFGSwitch() ;

#if wxUSE_GUI

// menu manager

MenuRef         UMANewMenu( SInt16 id , const wxString& title , wxFontEncoding encoding) ;
void             UMASetMenuTitle( MenuRef menu , const wxString& title , wxFontEncoding encoding) ;
UInt32             UMAMenuEvent( EventRecord *inEvent ) ;
void             UMAEnableMenuItem( MenuRef inMenu , MenuItemIndex item , bool enable ) ;

void            UMAAppendSubMenuItem( MenuRef menu , const wxString& title , wxFontEncoding encoding , SInt16 submenuid ) ;
void            UMAInsertSubMenuItem( MenuRef menu , const wxString& title , wxFontEncoding encoding , MenuItemIndex item , SInt16 submenuid ) ;
void            UMAAppendMenuItem( MenuRef menu , const wxString& title , wxFontEncoding encoding , wxAcceleratorEntry *entry = NULL  ) ;
void            UMAInsertMenuItem( MenuRef menu , const wxString& title , wxFontEncoding encoding , MenuItemIndex item , wxAcceleratorEntry *entry = NULL ) ;
void             UMASetMenuItemShortcut( MenuRef menu , MenuItemIndex item , wxAcceleratorEntry *entry ) ;

void            UMASetMenuItemText(  MenuRef menu,  MenuItemIndex item, const wxString& title , wxFontEncoding encoding ) ;

// quickdraw

void            UMAShowWatchCursor() ;
void            UMAShowArrowCursor() ;

// window manager

GrafPtr        UMAGetWindowPort( WindowRef inWindowRef ) ;
void             UMADisposeWindow( WindowRef inWindowRef ) ;
void             UMASetWTitle( WindowRef inWindowRef , const wxString& title , wxFontEncoding encoding) ;

void             UMADrawGrowIcon( WindowRef inWindowRef ) ;
void            UMAShowHide( WindowRef inWindowRef , Boolean show) ;

// appearance manager

void             UMADrawControl( ControlRef inControl ) ;

void             UMAEnableControl( ControlRef inControl ) ;
void            UMADisableControl( ControlRef inControl ) ;
void             UMAActivateControl( ControlRef inControl ) ;
void            UMADeactivateControl( ControlRef inControl ) ;
//                                 ControlPartCode         hiliteState)    ;
void UMAShowControl                        (ControlRef             theControl)    ;
void UMAHideControl                        (ControlRef             theControl);
void UMAActivateControl                    (ControlRef             inControl);
void UMADeactivateControl                (ControlRef             inControl);
void UMASetControlTitle( ControlRef inControl , const wxString& title , wxFontEncoding encoding) ;

void UMAMoveControl( ControlRef inControl , short x , short y ) ;
void UMASizeControl( ControlRef inControl , short x , short y ) ;
// control hierarchy

Rect * UMAGetControlBoundsInWindowCoords(ControlRef theControl, Rect *bounds) ;

// keyboard focus
OSErr UMASetKeyboardFocus                (WindowPtr                 inWindow,
                                 ControlRef             inControl,
                                 ControlFocusPart         inPart)    ;

// events

void UMAUpdateControls( WindowPtr inWindow , RgnHandle inRgn ) ;
OSErr UMAGetRootControl( WindowPtr inWindow , ControlRef *outControl ) ;

// handling control data
bool            UMAIsWindowFloating( WindowRef inWindow ) ;
bool            UMAIsWindowModal( WindowRef inWindow ) ;

void UMAHighlightAndActivateWindow( WindowRef inWindowRef , bool inActivate ) ;

OSStatus UMAGetHelpMenu(
  MenuRef *        outHelpMenu,
  MenuItemIndex *  outFirstCustomItemIndex);      /* can be NULL */

// Appearance Drawing

OSStatus UMADrawThemePlacard( const Rect *inRect , ThemeDrawState inState ) ;

// Clipboard support

OSStatus UMAPutScrap( Size size , OSType type , void *data ) ;

#define GetWindowUpdateRgn( inWindow , updateRgn ) GetWindowRegion( inWindow , kWindowUpdateRgn, updateRgn )

#endif // wxUSE_GUI

#endif
