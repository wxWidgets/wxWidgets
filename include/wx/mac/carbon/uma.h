/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/uma.h
// Purpose:     Universal MacOS API
// Author:      Stefan Csomor
// Modified by:
// Created:     03/02/99
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef H_UMA
#define H_UMA

#include "wx/mac/private.h"

void UMAInitToolbox( UInt16 inMoreMastersCalls, bool isEmbedded) ;
long UMAGetSystemVersion() ;

// process manager

long UMAGetProcessMode() ;
bool UMAGetProcessModeDoesActivateOnFGSwitch() ;

#if wxUSE_GUI

// menu manager

MenuRef         UMANewMenu( SInt16 id , const wxString& title , wxFontEncoding encoding) ;
void             UMASetMenuTitle( MenuRef menu , const wxString& title , wxFontEncoding encoding) ;
void             UMAEnableMenuItem( MenuRef inMenu , MenuItemIndex item , bool enable ) ;

void            UMAAppendSubMenuItem( MenuRef menu , const wxString& title , wxFontEncoding encoding , SInt16 submenuid ) ;
void            UMAInsertSubMenuItem( MenuRef menu , const wxString& title , wxFontEncoding encoding , MenuItemIndex item , SInt16 submenuid ) ;
void            UMAAppendMenuItem( MenuRef menu , const wxString& title , wxFontEncoding encoding , wxAcceleratorEntry *entry = NULL  ) ;
void            UMAInsertMenuItem( MenuRef menu , const wxString& title , wxFontEncoding encoding , MenuItemIndex item , wxAcceleratorEntry *entry = NULL ) ;
void             UMASetMenuItemShortcut( MenuRef menu , MenuItemIndex item , wxAcceleratorEntry *entry ) ;

void            UMASetMenuItemText(  MenuRef menu,  MenuItemIndex item, const wxString& title , wxFontEncoding encoding ) ;

// quickdraw

void            UMAShowWatchCursor() ;

// control hierarchy

Rect * UMAGetControlBoundsInWindowCoords(ControlRef theControl, Rect *bounds) ;

// events

void UMAUpdateControls( WindowPtr inWindow , RgnHandle inRgn ) ;
OSErr UMAGetRootControl( WindowPtr inWindow , ControlRef *outControl ) ;

void UMAHighlightAndActivateWindow( WindowRef inWindowRef , bool inActivate ) ;

// Retrieves the Help menu handle. Warning: As a side-effect this functions also
// creates the Help menu if it didn't exist yet.
OSStatus UMAGetHelpMenu(
  MenuRef *        outHelpMenu,
  MenuItemIndex *  outFirstCustomItemIndex);      /* can be NULL */

// Same as UMAGetHelpMenu, but doesn't create the Help menu if UMAGetHelpMenu hasn't been called yet.
OSStatus UMAGetHelpMenuDontCreate(
  MenuRef *        outHelpMenu,
  MenuItemIndex *  outFirstCustomItemIndex);      /* can be NULL */

// Appearance Drawing

#define GetWindowUpdateRgn( inWindow , updateRgn ) GetWindowRegion( inWindow , kWindowUpdateRgn, updateRgn )

// Quartz 

CGDataProviderRef UMACGDataProviderCreateWithCFData( CFDataRef data );
CGDataConsumerRef UMACGDataConsumerCreateWithCFData( CFMutableDataRef data );

#endif // wxUSE_GUI

#endif
