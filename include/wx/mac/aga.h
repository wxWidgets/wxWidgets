/////////////////////////////////////////////////////////////////////////////
// Name:        aga.h
// Purpose:     Gray Controls implementation
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _UMA_AGA_H
#define _UMA_AGA_H

ControlHandle AGANewControl(WindowPtr 				owningWindow,
								 const Rect *			boundsRect,
								 ConstStr255Param 		controlTitle,
								 Boolean 				initiallyVisible,
								 SInt16 				initialValue,
								 SInt16 				minimumValue,
								 SInt16 				maximumValue,
								 SInt16 				procID,
								 SInt32 				controlReference) ;
								 
OSErr AGASetControlFontStyle				(ControlHandle 			inControl,
								 const ControlFontStyleRec * inStyle)	;
								 
OSErr AGAGetControlDataSize				(ControlHandle 			inControl,
								 ControlPartCode 		inPart,
								 ResType 				inTagName,
								 Size *					outMaxSize) ;
								 

OSErr AGASetControlData					(ControlHandle 			inControl,
								 ControlPartCode 		inPart,
								 ResType 				inTagName,
								 Size 					inSize,
								 Ptr 					inData) ;

OSErr AGAGetControlData					(ControlHandle 			inControl,
								 ControlPartCode 		inPart,
								 ResType 				inTagName,
								 Size 					inBufferSize,
								 Ptr 					outBuffer,
								 Size *					outActualSize) ;

OSErr AGAGetBestControlRect				(ControlHandle 			inControl,
								 Rect *					outRect,
								 SInt16 *				outBaseLineOffset) ;

SInt16 AGAHandleControlKey				(ControlHandle 			inControl,
								 SInt16 				inKeyCode,
								 SInt16 				inCharCode,
								 SInt16 				inModifiers) ;

ControlPartCode AGAHandleControlClick				(ControlHandle 			inControl,
								 Point 					inWhere,
								 SInt16 				inModifiers,
								 ControlActionUPP 		inAction)	;

void AGAIdleControls					(WindowPtr 				inWindow) ;

void AGADrawControl( ControlHandle inControl ) ;
void AGADeactivateControl( ControlHandle inControl ) ;
void AGAActivateControl( ControlHandle inControl ) ;
OSErr AGASetKeyboardFocus				(WindowPtr 				inWindow,
								 ControlHandle 			inControl,
								 ControlFocusPart 		inPart) ;

OSErr AGACreateRootControl				(WindowPtr 				inWindow,
								 ControlHandle *		outControl) ;
OSErr AGAEmbedControl					(ControlHandle 			inControl,
								 ControlHandle 			inContainer) ;
								 
void AGAUpdateControls( WindowPtr inWindow , RgnHandle inRgn ) ;
OSErr AGAGetRootControl( WindowPtr inWindow , ControlHandle *outControl ) ;
void			AGASetThemeWindowBackground		(WindowRef 				inWindow,
								 ThemeBrush 			inBrush,
								 Boolean 				inUpdate) ;
void AGAMoveControl( ControlHandle inControl , short x , short y ) ;
void AGASizeControl( ControlHandle inControl , short x , short y ) ;


#endif