/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.cpp
// Purpose:     wxTextCtrl
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "textctrl.h"
#endif

#include "wx/defs.h"

#if wxUSE_TEXTCTRL

#ifdef __DARWIN__
  #include <sys/types.h>
  #include <sys/stat.h>
#else
  #include <stat.h>
#endif

#include "wx/msgdlg.h"

#if wxUSE_STD_IOSTREAM
    #if wxUSE_IOSTREAMH
        #include <fstream.h>
    #else
        #include <fstream>
    #endif
#endif

#include "wx/app.h"
#include "wx/dc.h"
#include "wx/button.h"
#include "wx/toplevel.h"
#include "wx/textctrl.h"
#include "wx/notebook.h"
#include "wx/tabctrl.h"
#include "wx/settings.h"
#include "wx/filefn.h"
#include "wx/utils.h"

#if defined(__BORLANDC__) && !defined(__WIN32__)
  #include <alloc.h>
#elif !defined(__MWERKS__) && !defined(__GNUWIN32) && !defined(__DARWIN__)
  #include <malloc.h>
#endif

#ifndef __DARWIN__
#include <Scrap.h>
#endif
#include <MacTextEditor.h>
#include <ATSUnicode.h>
#include <TextCommon.h>
#include <TextEncodingConverter.h>
#include "wx/mac/uma.h"

#define TE_UNLIMITED_LENGTH 0xFFFFFFFFUL
#if TARGET_API_MAC_OSX
 #define wxMAC_USE_MLTE 1
 #if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2
 #define wxMAC_USE_MLTE_HIVIEW 1
 #else
 #define wxMAC_USE_MLTE_HIVIEW 0
 #endif
#else
 // there is no unicodetextctrl on classic, and hopefully MLTE works better there
 #define wxMAC_USE_MLTE 1
 #define wxMAC_USE_MLTE_HIVIEW 0
#endif

#if wxMAC_USE_MLTE

TXNFrameOptions FrameOptionsFromWXStyle( long wxStyle )
{
    TXNFrameOptions frameOptions =
        kTXNDontDrawCaretWhenInactiveMask ;
    if ( ! ( wxStyle & wxTE_NOHIDESEL ) )
        frameOptions |= kTXNDontDrawSelectionWhenInactiveMask ;

    if ( wxStyle & wxTE_MULTILINE )
    {
        if ( ! ( wxStyle & wxTE_DONTWRAP ) )
            frameOptions |= kTXNAlwaysWrapAtViewEdgeMask ;
        else
        {
            frameOptions |= kTXNAlwaysWrapAtViewEdgeMask ;
            frameOptions |= kTXNWantHScrollBarMask ;
        }

        if ( !(wxStyle & wxTE_NO_VSCROLL ) )
            frameOptions |= kTXNWantVScrollBarMask ;
    }
    else
        frameOptions |= kTXNSingleLineOnlyMask ;
    return frameOptions ;
}

void AdjustAttributesFromWXStyle( TXNObject txn , long wxStyle , bool visible )
{
    TXNControlTag iControlTags[3] = { kTXNDoFontSubstitution, kTXNWordWrapStateTag };
    TXNControlData iControlData[3] = { {false}, {kTXNNoAutoWrap} };
    int toptag = 2 ;
#if TARGET_API_MAC_OSX
    iControlTags[2] = kTXNVisibilityTag ;
    iControlData[2].uValue = visible ;
    toptag++ ;
#endif        
    
    if ( wxStyle & wxTE_MULTILINE )
    {
        if (wxStyle & wxTE_DONTWRAP)
            iControlData[1].uValue = kTXNNoAutoWrap ;
        else
            iControlData[1].uValue = kTXNAutoWrap ;
        
    }
    verify_noerr( TXNSetTXNObjectControls( txn, false, toptag,
                                        iControlTags, iControlData )) ;

    Str255 fontName ;
    SInt16 fontSize ;
    Style fontStyle ;

    GetThemeFont(kThemeSystemFont , GetApplicationScript() , fontName , &fontSize , &fontStyle ) ;

    TXNTypeAttributes typeAttr[] =
    {
        {   kTXNQDFontNameAttribute , kTXNQDFontNameAttributeSize , { (void*) fontName } } ,
        {   kTXNQDFontSizeAttribute , kTXNFontSizeAttributeSize , { (void*) (fontSize << 16) } } ,
        {   kTXNQDFontStyleAttribute , kTXNQDFontStyleAttributeSize , {  (void*) normal } } ,
    } ;

    verify_noerr( TXNSetTypeAttributes (txn, sizeof( typeAttr ) / sizeof(TXNTypeAttributes) , typeAttr,
          kTXNStartOffset,
          kTXNEndOffset) );

}

#if !wxMAC_USE_MLTE_HIVIEW

// CS:TODO we still have a problem getting properly at the text events of a control because under Carbon
// the MLTE engine registers itself for the key events thus the normal flow never occurs, the only measure for the
// moment is to avoid setting the true focus on the control, the proper solution at the end would be to have
// an alternate path for carbon key events that routes automatically into the same wx flow of events

/* part codes */

/* kmUPTextPart is the part code we return to indicate the user has clicked
    in the text area of our control */
#define kmUPTextPart 1


/* routines for using existing user pane controls.
    These routines are useful for cases where you would like to use an
    existing user pane control in, say, a dialog window as a scrolling
    text edit field.*/

/* Utility Routines */

/* kUserClickedToFocusPart is a part code we pass to the SetKeyboardFocus
    routine.  In our focus switching routine this part code is understood
    as meaning 'the user has clicked in the control and we need to switch
    the current focus to ourselves before we can continue'. */
#define kUserClickedToFocusPart 100

/* STPTextPaneVars is a structure used for storing the the mUP Control's
    internal variables and state information.  A handle to this record is
    stored in the pane control's reference value field using the
    SetControlReference routine. */

typedef struct {
        /* OS records referenced */
    TXNObject fTXNRec; /* the txn record */
    TXNFrameID fTXNFrame; /* the txn frame ID */
    ControlRef fUserPaneRec;  /* handle to the user pane control */
    WindowPtr fOwner; /* window containing control */
    GrafPtr fDrawingEnvironment; /* grafport where control is drawn */
        /* flags */
    Boolean fInFocus; /* true while the focus rect is drawn around the control */
    Boolean fIsActive; /* true while the control is drawn in the active state */
    Boolean fTXNObjectActive; /* reflects the activation state of the text edit record */
	Boolean fFocusDrawState; /* true if focus is drawn (default: true) */ 
        /* calculated locations */
    Rect fRBounds; /* control bounds */
    Rect fRTextArea; /* area where the text is drawn */
    Rect fRFocusOutline;  /* rectangle used to draw the focus box */
    Rect fRTextOutline; /* rectangle used to draw the border */
    RgnHandle fRTextOutlineRegion; /* background region for the text, erased before calling TEUpdate */
        /* our focus advance override routine */
    EventHandlerUPP handlerUPP;
    EventHandlerRef handlerRef;
    bool fNoBorders ;
    bool fMultiline ;
    bool fVisible ;
} STPTextPaneVars;

/* mUPOpenControl initializes a user pane control so it will be drawn
    and will behave as a scrolling text edit field inside of a window.
    This routine performs all of the initialization steps necessary,
    except it does not create the user pane control itself.  theControl
    should refer to a user pane control that you have either created
    yourself or extracted from a dialog's control heirarchy using
    the GetDialogItemAsControl routine.  */
OSStatus mUPOpenControl(STPTextPaneVars* &handle, ControlRef theControl, long wxStyle);




/* Univerals Procedure Pointer variables used by the
    mUP Control.  These variables are set up
    the first time that mUPOpenControl is called. */
ControlUserPaneDrawUPP gTPDrawProc = NULL;
ControlUserPaneHitTestUPP gTPHitProc = NULL;
ControlUserPaneTrackingUPP gTPTrackProc = NULL;
ControlUserPaneIdleUPP gTPIdleProc = NULL;
ControlUserPaneKeyDownUPP gTPKeyProc = NULL;
ControlUserPaneActivateUPP gTPActivateProc = NULL;
ControlUserPaneFocusUPP gTPFocusProc = NULL;

// one place for calculating all
static void TPCalculateBounds(STPTextPaneVars *varsp, const Rect& bounds) 
{
    SetRect(&varsp->fRBounds, bounds.left, bounds.top, bounds.right, bounds.bottom);
    SetRect(&varsp->fRFocusOutline, bounds.left, bounds.top, bounds.right, bounds.bottom);
    // eventually make TextOutline inset 1,1
    SetRect(&varsp->fRTextOutline, bounds.left, bounds.top, bounds.right, bounds.bottom);
    if ( !varsp->fNoBorders )
    {
    	SetRect(&varsp->fRTextArea, bounds.left + 2 , bounds.top + (varsp->fMultiline ? 0 : 2) ,
        bounds.right - (varsp->fMultiline ? 0 : 2), bounds.bottom - (varsp->fMultiline ? 0 : 2));
    }
    else
    {
        SetRect(&varsp->fRTextArea, bounds.left , bounds.top ,
            bounds.right, bounds.bottom);
    }
}

OSStatus MLTESetObjectVisibility( STPTextPaneVars *varsp, Boolean vis , long wxStyle)
{
    OSStatus err = noErr ;
#if TARGET_API_MAC_OSX
    TXNControlTag iControlTags[1] = { kTXNVisibilityTag };
    TXNControlData iControlData[1] = {{ vis }};
    err = ::TXNSetTXNObjectControls( varsp->fTXNRec, false, 1, iControlTags, iControlData );
#endif
    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(varsp->fUserPaneRec);
    if ( vis && textctrl )
    {
        Rect bounds ;
        UMAGetControlBoundsInWindowCoords( varsp->fUserPaneRec, &bounds);
        TPCalculateBounds( varsp , bounds ) ;
        wxMacWindowClipper cl(textctrl) ;
        TXNSetFrameBounds( varsp->fTXNRec, varsp->fRTextArea.top, varsp->fRTextArea.left,
            varsp->fRTextArea.bottom, varsp->fRTextArea.right, varsp->fTXNFrame);
        TXNShowSelection( varsp->fTXNRec, kTXNShowStart);
    }
    return err ;
}

// make sure we don't miss changes as carbon events are not available for these under classic
static void TPUpdateVisibility(ControlRef theControl) {
    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(theControl);
    if ( textctrl == NULL )
        return ;

    STPTextPaneVars *varsp = (STPTextPaneVars *) textctrl->m_macTXNvars ;

    Rect bounds ;
    UMAGetControlBoundsInWindowCoords(theControl, &bounds);
    if ( textctrl->MacIsReallyShown() != varsp->fVisible )
    {
        // invalidate old position
        // InvalWindowRect( GetControlOwner( theControl ) , &varsp->fRBounds ) ;
        varsp->fVisible = textctrl->MacIsReallyShown() ;
    }
    if ( !EqualRect( &bounds , &varsp->fRBounds ) )
    {
        // old position
        Rect oldBounds = varsp->fRBounds ;
        TPCalculateBounds( varsp , bounds ) ;
        // we only recalculate when visible, otherwise scrollbars get drawn at incorrect places
        if ( varsp->fVisible )
        {
            wxMacWindowClipper cl(textctrl) ;
            TXNSetFrameBounds( varsp->fTXNRec, varsp->fRTextArea.top, varsp->fRTextArea.left,
                varsp->fRTextArea.bottom, varsp->fRTextArea.right, varsp->fTXNFrame);
        }
        InvalWindowRect( GetControlOwner( theControl ) , &oldBounds ) ;
        InvalWindowRect( GetControlOwner( theControl ) , &varsp->fRBounds ) ;
    }
}

// make correct activations
static void TPActivatePaneText(STPTextPaneVars *varsp, Boolean setActive) {

    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(varsp->fUserPaneRec);
    if (varsp->fTXNObjectActive != setActive && textctrl->MacIsReallyShown() ) 
    {
        varsp->fTXNObjectActive = setActive;
        TXNActivate(varsp->fTXNRec, varsp->fTXNFrame, varsp->fTXNObjectActive);
        if (varsp->fInFocus)
            TXNFocus( varsp->fTXNRec, varsp->fTXNObjectActive);
    }
}

// update focus outlines
static void TPRedrawFocusOutline(STPTextPaneVars *varsp) {

    /* state changed */
	if (varsp->fFocusDrawState != (varsp->fIsActive && varsp->fInFocus)) 
	{ 
		varsp->fFocusDrawState = (varsp->fIsActive && varsp->fInFocus);
		DrawThemeFocusRect(&varsp->fRFocusOutline, varsp->fFocusDrawState);
	}
}

// update TXN focus state
static void TPFocusPaneText(STPTextPaneVars *varsp, Boolean setFocus) {
    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(varsp->fUserPaneRec);

    if (varsp->fInFocus != setFocus && textctrl->MacIsReallyShown()) {
        varsp->fInFocus = setFocus;
        TXNFocus( varsp->fTXNRec, varsp->fInFocus);
    }
}

// draw the control
static pascal void TPPaneDrawProc(ControlRef theControl, ControlPartCode thePart) {
        /* set up our globals */

    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(theControl);
    if ( textctrl == NULL )
        return ;
    TPUpdateVisibility( theControl ) ;
  
    STPTextPaneVars *varsp = (STPTextPaneVars *) textctrl->m_macTXNvars ;
    if ( textctrl->MacIsReallyShown() )
    {
        wxMacWindowClipper clipper( textctrl ) ;
        TXNDraw(varsp->fTXNRec, NULL);
        if ( !varsp->fNoBorders )
        	DrawThemeEditTextFrame(&varsp->fRTextOutline, varsp->fIsActive ? kThemeStateActive: kThemeStateInactive);
        TPRedrawFocusOutline( varsp ) ;
    }

}


/* TPPaneHitTestProc is called when the control manager would
    like to determine what part of the control the mouse resides over.
    We also call this routine from our tracking proc to determine how
    to handle mouse clicks. */
static pascal ControlPartCode TPPaneHitTestProc(ControlRef theControl, Point where) {
    ControlPartCode result;
        /* set up our locals and lock down our globals*/
    result = 0;
    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(theControl);
    if ( textctrl == NULL )
        return 0 ;
    TPUpdateVisibility( theControl ) ;
    STPTextPaneVars *varsp = (STPTextPaneVars *) textctrl->m_macTXNvars ;
    if (textctrl->MacIsReallyShown() ) 
    {
        if (PtInRect(where, &varsp->fRBounds))
            result = kmUPTextPart;
        else 
            result = 0;
    }
    return result;
}





/* TPPaneTrackingProc is called when the mouse is being held down
    over our control.  This routine handles clicks in the text area
    and in the scroll bar. */
static pascal ControlPartCode TPPaneTrackingProc(ControlRef theControl, Point startPt, ControlActionUPP actionProc) {
	
    ControlPartCode partCodeResult;
	/* make sure we have some variables... */
    partCodeResult = 0;
    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(theControl);
    if ( textctrl == NULL )
        return 0;
    TPUpdateVisibility( theControl ) ;
    STPTextPaneVars *varsp = (STPTextPaneVars *) textctrl->m_macTXNvars ;
    if (textctrl->MacIsReallyShown() ) 
	{
		/* we don't do any of these functions unless we're in focus */
        if ( ! varsp->fInFocus) {
            WindowPtr owner;
            owner = GetControlOwner(theControl);
            ClearKeyboardFocus(owner);
            SetKeyboardFocus(owner, theControl, kUserClickedToFocusPart);
        }
		/* find the location for the click */
        // for compositing, we must convert these into toplevel window coordinates, because hittesting expects them
        if ( textctrl->MacGetTopLevelWindow()->MacUsesCompositing() )
        {
            int x = 0 , y = 0 ;
            textctrl->MacClientToRootWindow( &x , &y ) ;
            startPt.h += x ;
            startPt.v += y ;
        }

        switch (TPPaneHitTestProc(theControl, startPt)) 
        {
			
			/* handle clicks in the text part */
            case kmUPTextPart:
			{ 
				wxMacWindowClipper clipper( textctrl ) ;
				
				EventRecord rec ;
				ConvertEventRefToEventRecord( (EventRef) wxTheApp->MacGetCurrentEvent() , &rec ) ;
				TXNClick( varsp->fTXNRec, &rec );
				
			}
                break;
				
        }
    }
    return partCodeResult;
}


/* TPPaneIdleProc is our user pane idle routine.  When our text field
    is active and in focus, we use this routine to set the cursor. */
static pascal void TPPaneIdleProc(ControlRef theControl) {
        /* set up locals */
    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(theControl);
    if ( textctrl == NULL )
        return ;
    TPUpdateVisibility( theControl ) ;
    STPTextPaneVars *varsp = (STPTextPaneVars *) textctrl->m_macTXNvars ;
    if (textctrl->MacIsReallyShown()) {
            /* if we're not active, then we have nothing to say about the cursor */
        if (varsp->fIsActive) {
            Rect bounds;
            Point mousep;

            wxMacWindowClipper clipper( textctrl ) ;
            GetMouse(&mousep);
                /* there's a 'focus thing' and an 'unfocused thing' */
            if (varsp->fInFocus) {
                    /* flash the cursor */
                SetPort(varsp->fDrawingEnvironment);
                TXNIdle(varsp->fTXNRec);
                /* set the cursor */
                if (PtInRect(mousep, &varsp->fRTextArea)) {
                    RgnHandle theRgn;
                    RectRgn((theRgn = NewRgn()), &varsp->fRTextArea);
                    TXNAdjustCursor(varsp->fTXNRec, theRgn);
                    DisposeRgn(theRgn);
                 }
                 else
                 {
                 // SetThemeCursor(kThemeArrowCursor);
                 }
            } else {
                /* if it's in our bounds, set the cursor */
                UMAGetControlBoundsInWindowCoords(theControl, &bounds);
                if (PtInRect(mousep, &bounds))
                {
                //    SetThemeCursor(kThemeArrowCursor);
                }
            }
        }
    }
}


/* TPPaneKeyDownProc is called whenever a keydown event is directed
    at our control.  Here, we direct the keydown event to the text
    edit record and redraw the scroll bar and text field as appropriate. */
static pascal ControlPartCode TPPaneKeyDownProc(ControlRef theControl,
                            SInt16 keyCode, SInt16 charCode, SInt16 modifiers) {

    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(theControl);
    if ( textctrl == NULL )
        return 0;
    TPUpdateVisibility( theControl ) ;

    STPTextPaneVars *varsp = (STPTextPaneVars *) textctrl->m_macTXNvars ;
    if (varsp->fInFocus) 
    {
            /* turn autoscrolling on and send the key event to text edit */
        wxMacWindowClipper clipper( textctrl ) ;
        EventRecord ev ;
        memset( &ev , 0 , sizeof( ev ) ) ;
        ev.what = keyDown ;
        ev.modifiers = modifiers ;
        ev.message = (( keyCode << 8 ) & keyCodeMask ) + ( charCode & charCodeMask ) ;
        TXNKeyDown( varsp->fTXNRec, &ev);
    }
    return kControlEntireControl;
}


/* TPPaneActivateProc is called when the window containing
    the user pane control receives activate events. Here, we redraw
    the control and it's text as necessary for the activation state. */
static pascal void TPPaneActivateProc(ControlRef theControl, Boolean activating) {
        /* set up locals */
    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(theControl);
    
    if ( textctrl == NULL )
        return ;
    TPUpdateVisibility( theControl ) ;
    
    STPTextPaneVars *varsp = (STPTextPaneVars *) textctrl->m_macTXNvars ;

    varsp->fIsActive = activating;
    wxMacWindowClipper clipper( textctrl ) ;
    TPActivatePaneText(varsp, varsp->fIsActive && varsp->fInFocus);
        /* redraw the frame */
    if ( textctrl->MacIsReallyShown() )
    {
        if ( !varsp->fNoBorders )
        	DrawThemeEditTextFrame(&varsp->fRTextOutline, varsp->fIsActive ? kThemeStateActive: kThemeStateInactive);
        TPRedrawFocusOutline( varsp ) ;
    }
}


/* TPPaneFocusProc is called when every the focus changes to or
    from our control.  Herein, switch the focus appropriately
    according to the parameters and redraw the control as
    necessary.  */
static pascal ControlPartCode TPPaneFocusProc(ControlRef theControl, ControlFocusPart action) {
    ControlPartCode focusResult;

    focusResult = kControlFocusNoPart;
    wxTextCtrl* textctrl = (wxTextCtrl*) GetControlReference(theControl);
    if ( textctrl == NULL )
        return 0;
    TPUpdateVisibility( theControl ) ;
    STPTextPaneVars *varsp = (STPTextPaneVars *) textctrl->m_macTXNvars ;
        /* if kControlFocusPrevPart and kControlFocusNextPart are received when the user is
        tabbing forwards (or shift tabbing backwards) through the items in the dialog,
        and kControlFocusNextPart will be received.  When the user clicks in our field
        and it is not the current focus, then the constant kUserClickedToFocusPart will
        be received.  The constant kControlFocusNoPart will be received when our control
        is the current focus and the user clicks in another control.  In your focus routine,
        you should respond to these codes as follows:

        kControlFocusNoPart - turn off focus and return kControlFocusNoPart.  redraw
            the control and the focus rectangle as necessary.

        kControlFocusPrevPart or kControlFocusNextPart - toggle focus on or off
            depending on its current state.  redraw the control and the focus rectangle
            as appropriate for the new focus state.  If the focus state is 'off', return the constant
            kControlFocusNoPart, otherwise return a non-zero part code.
        kUserClickedToFocusPart - is a constant defined for this example.  You should
            define your own value for handling click-to-focus type events. */
         /* calculate the next highlight state */
    switch (action) {
        default:
        case kControlFocusNoPart:
            TPFocusPaneText(varsp, false);
            focusResult = kControlFocusNoPart;
            break;
        case kUserClickedToFocusPart:
            TPFocusPaneText(varsp, true);
            focusResult = 1;
            break;
        case kControlFocusPrevPart:
        case kControlFocusNextPart:
            TPFocusPaneText(varsp, ( ! varsp->fInFocus));
            focusResult = varsp->fInFocus ? 1 : kControlFocusNoPart;
            break;
    }
    TPActivatePaneText(varsp, varsp->fIsActive && varsp->fInFocus);
    /* redraw the text fram and focus rectangle to indicate the
    new focus state */
    if ( textctrl->MacIsReallyShown() )
    {
        wxMacWindowClipper c( textctrl ) ;
        if ( !varsp->fNoBorders )
        	DrawThemeEditTextFrame(&varsp->fRTextOutline, varsp->fIsActive ? kThemeStateActive: kThemeStateInactive);
        TPRedrawFocusOutline( varsp ) ;
    }
    return focusResult;
}


/* mUPOpenControl initializes a user pane control so it will be drawn
    and will behave as a scrolling text edit field inside of a window.
    This routine performs all of the initialization steps necessary,
    except it does not create the user pane control itself.  theControl
    should refer to a user pane control that you have either created
    yourself or extracted from a dialog's control heirarchy using
    the GetDialogItemAsControl routine.  */
OSStatus mUPOpenControl(STPTextPaneVars* &handle, ControlRef theControl, long wxStyle )
{
    Rect bounds;
    WindowRef theWindow;
    STPTextPaneVars *varsp;
    OSStatus err = noErr ;

        /* set up our globals */
    if (gTPDrawProc == NULL) gTPDrawProc = NewControlUserPaneDrawUPP(TPPaneDrawProc);
    if (gTPHitProc == NULL) gTPHitProc = NewControlUserPaneHitTestUPP(TPPaneHitTestProc);
    if (gTPTrackProc == NULL) gTPTrackProc = NewControlUserPaneTrackingUPP(TPPaneTrackingProc);
    if (gTPIdleProc == NULL) gTPIdleProc = NewControlUserPaneIdleUPP(TPPaneIdleProc);
    if (gTPKeyProc == NULL) gTPKeyProc = NewControlUserPaneKeyDownUPP(TPPaneKeyDownProc);
    if (gTPActivateProc == NULL) gTPActivateProc = NewControlUserPaneActivateUPP(TPPaneActivateProc);
    if (gTPFocusProc == NULL) gTPFocusProc = NewControlUserPaneFocusUPP(TPPaneFocusProc);

        /* allocate our private storage */
    varsp = (STPTextPaneVars *) malloc(sizeof(STPTextPaneVars));
    handle = varsp ;    

        /* set the initial settings for our private data */
    varsp->fMultiline = wxStyle & wxTE_MULTILINE ;
    varsp->fNoBorders = wxStyle & wxNO_BORDER ;
    varsp->fInFocus = false;
    varsp->fIsActive = true;
    varsp->fTXNObjectActive = false; 
    varsp->fFocusDrawState = false ;
    varsp->fUserPaneRec = theControl;
    varsp->fVisible = true ;
    
    theWindow = varsp->fOwner = GetControlOwner(theControl);

    varsp->fDrawingEnvironment = (GrafPtr)  GetWindowPort(theWindow);

        /* set up the user pane procedures */
    SetControlData(theControl, kControlEntireControl, kControlUserPaneDrawProcTag, sizeof(gTPDrawProc), &gTPDrawProc);
    SetControlData(theControl, kControlEntireControl, kControlUserPaneHitTestProcTag, sizeof(gTPHitProc), &gTPHitProc);
    SetControlData(theControl, kControlEntireControl, kControlUserPaneTrackingProcTag, sizeof(gTPTrackProc), &gTPTrackProc);
    SetControlData(theControl, kControlEntireControl, kControlUserPaneIdleProcTag, sizeof(gTPIdleProc), &gTPIdleProc);
    SetControlData(theControl, kControlEntireControl, kControlUserPaneKeyDownProcTag, sizeof(gTPKeyProc), &gTPKeyProc);
    SetControlData(theControl, kControlEntireControl, kControlUserPaneActivateProcTag, sizeof(gTPActivateProc), &gTPActivateProc);
    SetControlData(theControl, kControlEntireControl, kControlUserPaneFocusProcTag, sizeof(gTPFocusProc), &gTPFocusProc);
    
        /* calculate the rectangles used by the control */
    UMAGetControlBoundsInWindowCoords(theControl, &bounds);
    varsp->fRTextOutlineRegion = NewRgn() ;
    TPCalculateBounds( varsp , bounds ) ;

        /* set up the drawing environment */
    SetPort(varsp->fDrawingEnvironment);

        /* create the new edit field */

    TXNFrameOptions frameOptions = FrameOptionsFromWXStyle( wxStyle ) ;

    verify_noerr(TXNNewObject(NULL, varsp->fOwner, &varsp->fRTextArea,
        frameOptions ,
        kTXNTextEditStyleFrameType,
        kTXNTextensionFile,
        kTXNSystemDefaultEncoding,
        &varsp->fTXNRec, &varsp->fTXNFrame, (TXNObjectRefcon) varsp));

    AdjustAttributesFromWXStyle( varsp->fTXNRec , wxStyle , varsp->fVisible ) ;
        /* perform final activations and setup for our text field.  Here,
        we assume that the window is going to be the 'active' window. */
    TPActivatePaneText(varsp, varsp->fIsActive && varsp->fInFocus);
        /* all done */
    return err;
}

#else
struct STPTextPaneVars
{
} ;

#endif

static void SetTXNData( STPTextPaneVars *varsp, TXNObject txn , const wxString& st , TXNOffset start , TXNOffset end )
{
#if wxUSE_UNICODE
#if SIZEOF_WCHAR_T == 2
	size_t len = st.Len() ;
    TXNSetData( txn , kTXNUnicodeTextData,  (void*)st.wc_str(), len * 2,
      start, end);
#else
	wxMBConvUTF16BE converter ;
	ByteCount byteBufferLen = converter.WC2MB( NULL , st.wc_str() , 0 ) ;
	UniChar *unibuf = (UniChar*) malloc(byteBufferLen) ;
	converter.WC2MB( (char*) unibuf , st.wc_str() , byteBufferLen ) ;
    TXNSetData( txn , kTXNUnicodeTextData,  (void*)unibuf, byteBufferLen ,
      start, end);
	free( unibuf ) ;
#endif
#else
	wxCharBuffer text =  st.mb_str(wxConvLocal)  ;
    TXNSetData( txn , kTXNTextData,  (void*)text.data(), strlen( text ) ,
      start, end);
#endif  
}


#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl, wxControl)

BEGIN_EVENT_TABLE(wxTextCtrl, wxControl)
    EVT_DROP_FILES(wxTextCtrl::OnDropFiles)
    EVT_CHAR(wxTextCtrl::OnChar)
    EVT_MENU(wxID_CUT, wxTextCtrl::OnCut)
    EVT_MENU(wxID_COPY, wxTextCtrl::OnCopy)
    EVT_MENU(wxID_PASTE, wxTextCtrl::OnPaste)
    EVT_MENU(wxID_UNDO, wxTextCtrl::OnUndo)
    EVT_MENU(wxID_REDO, wxTextCtrl::OnRedo)

    EVT_UPDATE_UI(wxID_CUT, wxTextCtrl::OnUpdateCut)
    EVT_UPDATE_UI(wxID_COPY, wxTextCtrl::OnUpdateCopy)
    EVT_UPDATE_UI(wxID_PASTE, wxTextCtrl::OnUpdatePaste)
    EVT_UPDATE_UI(wxID_UNDO, wxTextCtrl::OnUpdateUndo)
    EVT_UPDATE_UI(wxID_REDO, wxTextCtrl::OnUpdateRedo)
END_EVENT_TABLE()
#endif

// Text item
void wxTextCtrl::Init()
{
  m_macTXN = NULL ;
  m_macTXNvars = NULL ;

  m_editable = true ;
  m_dirty = false;

  m_maxLength = TE_UNLIMITED_LENGTH ;
}

wxTextCtrl::~wxTextCtrl()
{
#if wxMAC_USE_MLTE
    SetControlReference((ControlRef)m_macControl, 0) ;
#if !wxMAC_USE_MLTE_HIVIEW
    TXNDeleteObject((TXNObject)m_macTXN);
#endif
    /* delete our private storage */
    free(m_macTXNvars);
    /* zero the control reference */
#endif
}


bool wxTextCtrl::Create(wxWindow *parent, wxWindowID id,
           const wxString& str,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    m_macIsUserPane = FALSE ;
    
    m_macTXN = NULL ;
    m_macTXNvars = NULL ;
    m_editable = true ;

    // base initialization
    if ( !wxTextCtrlBase::Create(parent, id, pos, size, style & ~(wxHSCROLL|wxVSCROLL), validator, name) )
        return FALSE;

    wxSize mySize = size ;

    Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;    

    if ( m_windowStyle & wxTE_MULTILINE )
    {
        wxASSERT_MSG( !(m_windowStyle & wxTE_PROCESS_ENTER),
                      wxT("wxTE_PROCESS_ENTER style is ignored for multiline text controls (they always process it)") );

        m_windowStyle |= wxTE_PROCESS_ENTER;
    }

    wxString st = str ;
    wxMacConvertNewlines13To10( &st ) ;
#if wxMAC_USE_MLTE
    {
#if wxMAC_USE_MLTE_HIVIEW
        HIRect hr = { bounds.left , bounds.top , bounds.right - bounds.left , bounds.bottom- bounds.top } ;
        HIViewRef scrollView = NULL ;
        TXNFrameOptions frameOptions = FrameOptionsFromWXStyle( style ) ;

        if ( frameOptions & (kTXNWantVScrollBarMask|kTXNWantHScrollBarMask) )
        {
            HIScrollViewCreate(( frameOptions & kTXNWantHScrollBarMask ? kHIScrollViewOptionsHorizScroll : 0) | 
                ( frameOptions & kTXNWantVScrollBarMask ? kHIScrollViewOptionsVertScroll: 0 ) , &scrollView ) ;
           
        	HIViewSetFrame( scrollView, &hr );
        	HIViewSetVisible( scrollView, true );
        }
        HIViewRef textView ;
        HITextViewCreate( NULL , 0, frameOptions , (ControlRef*) &textView ) ;
        m_macTXN = HITextViewGetTXNObject( textView) ;
        AdjustAttributesFromWXStyle( (TXNObject) m_macTXN , style , true ) ;
        HIViewSetVisible( (ControlRef) textView , true ) ;
        if ( scrollView )
        {
            HIViewAddSubview( scrollView , textView ) ;
            m_macControl = (WXWidget) scrollView ;
        }
        else
        {
            m_macControl = (WXWidget) textView ;
        }
#else
        short featurSet;

        featurSet = kControlSupportsEmbedding | kControlSupportsFocus  | kControlWantsIdle
                | kControlWantsActivate | kControlHandlesTracking | kControlHasSpecialBackground
                | kControlGetsFocusOnClick | kControlSupportsLiveFeedback;
            /* create the control */
        m_macControl = (WXWidget) ::NewControl(MAC_WXHWND(parent->MacGetTopLevelWindowRef()), &bounds, "\p", true , featurSet, 0, featurSet, kControlUserPaneProc,  (long) this );
            /* set up the mUP specific features and data */
        wxMacWindowClipper c(this) ;
        STPTextPaneVars *varsp ;
        mUPOpenControl( varsp, (ControlRef) m_macControl, m_windowStyle );
        m_macTXNvars = varsp ;
        m_macTXN =  varsp->fTXNRec ;
#endif

        if ( style & wxTE_PASSWORD )
        {
            UniChar c = 0xA5 ;
            verify_noerr(TXNEchoMode( (TXNObject) m_macTXN , c , 0 , true )) ;
        }
    }
    MacPostControlCreate(pos,size) ;
        
#if !wxMAC_USE_MLTE_HIVIEW
    if ( MacIsReallyShown() )
        MLTESetObjectVisibility( (STPTextPaneVars*) m_macTXNvars, true , GetWindowStyle() ) ;
#endif

    {
      	wxMacWindowClipper clipper( this ) ;
#if !wxMAC_USE_MLTE_HIVIEW
        TPUpdateVisibility( (ControlRef) m_macControl ) ;
#endif
        SetTXNData( (STPTextPaneVars *)m_macTXNvars , (TXNObject) m_macTXN , st , kTXNStartOffset, kTXNEndOffset ) ;

        TXNSetSelection( (TXNObject) m_macTXN, 0, 0);
        TXNShowSelection( (TXNObject) m_macTXN, kTXNShowStart);
    }
    
    // in case MLTE is catching events before we get the chance to do so, we'd have to reintroduce the tlw-handler in front :
    // parent->MacGetTopLevelWindow()->MacInstallTopLevelWindowEventHandler() ;

    SetBackgroundColour( *wxWHITE ) ;

    TXNBackground tback;
    tback.bgType = kTXNBackgroundTypeRGB;
    tback.bg.color = MAC_WXCOLORREF( GetBackgroundColour().GetPixel() );
    TXNSetBackground( (TXNObject) m_macTXN , &tback);

    if ( m_windowStyle & wxTE_READONLY)
    {
        SetEditable( false ) ;
    }
#else
    wxMacCFStringHolder cf(st , m_font.GetEncoding()) ;
    CreateEditUnicodeTextControl( MAC_WXHWND(parent->MacGetTopLevelWindowRef()), &bounds , cf , style & wxTE_PASSWORD , NULL , (ControlRef*) &m_macControl ) ;
    MacPostControlCreate(pos,size) ;
#endif
        

    return TRUE;
}

void wxTextCtrl::MacVisibilityChanged() 
{
#if wxMAC_USE_MLTE && !wxMAC_USE_MLTE_HIVIEW
    MLTESetObjectVisibility((STPTextPaneVars*) m_macTXNvars , MacIsReallyShown() , GetWindowStyle() ) ;
    if ( !MacIsReallyShown() )
        InvalWindowRect( GetControlOwner( (ControlHandle) m_macControl ) , &((STPTextPaneVars *)m_macTXNvars)->fRBounds ) ;
#endif
}

void wxTextCtrl::MacEnabledStateChanged() 
{
}


wxString wxTextCtrl::GetValue() const
{
    wxString result ;
#if wxMAC_USE_MLTE
    OSStatus err ;
    Size actualSize = 0;
    {
#if wxUSE_UNICODE
        Handle theText ;
        err = TXNGetDataEncoded( ((TXNObject) m_macTXN), kTXNStartOffset, kTXNEndOffset, &theText , kTXNUnicodeTextData );
        // all done
        if ( err )
        {
            actualSize = 0 ;
        }
        else
        {
            actualSize = GetHandleSize( theText ) / sizeof( UniChar) ;
            if ( actualSize > 0 )
            {
                wxChar *ptr = NULL ;
#if SIZEOF_WCHAR_T == 2		
                ptr = new wxChar[actualSize + 1 ] ;		
                wxStrncpy( ptr , (wxChar*) *theText , actualSize ) ;
                
#else
                SetHandleSize( theText , ( actualSize + 1 ) * sizeof( UniChar ) ) ;
				HLock( theText ) ;
                (((UniChar*)*theText)[actualSize]) = 0 ;
				wxMBConvUTF16BE converter ;
				size_t noChars = converter.MB2WC( NULL , (const char*)*theText , 0 ) ;
				ptr = new wxChar[noChars + 1] ;
				
				noChars = converter.MB2WC( ptr , (const char*)*theText , noChars ) ;
				ptr[noChars] = 0 ;
				HUnlock( theText ) ;
#endif
                ptr[actualSize] = 0 ;
                result = wxString( ptr ) ;
                delete[] ptr ;
            }
            DisposeHandle( theText ) ;
        }
#else
        Handle theText ;
        err = TXNGetDataEncoded( ((TXNObject) m_macTXN), kTXNStartOffset, kTXNEndOffset, &theText , kTXNTextData );
        // all done
        if ( err )
        {
            actualSize = 0 ;
        }
        else
        {
            actualSize = GetHandleSize( theText ) ;
            if ( actualSize > 0 )
            {
                HLock( theText ) ;
                result = wxString( *theText , wxConvLocal , actualSize ) ;
                HUnlock( theText ) ;
            }
            DisposeHandle( theText ) ;
        }
#endif
    }
#else
    CFStringRef value = NULL ;
    Size    actualSize = 0 ;

    verify_noerr( GetControlData( (ControlRef) m_macControl , 0, GetWindowStyle() & wxTE_PASSWORD ? 
        kControlEditTextPasswordCFStringTag : kControlEditTextCFStringTag, 
                    sizeof(CFStringRef), &value, &actualSize ) );
    if ( value )
    {
        wxMacCFStringHolder cf(value) ;
        result = cf.AsString() ;
    }
#endif
    wxMacConvertNewlines10To13( &result ) ;
    return result ;
}

void wxTextCtrl::GetSelection(long* from, long* to) const
{    
#if wxMAC_USE_MLTE
   TXNGetSelection( (TXNObject) m_macTXN , (TXNOffset*) from , (TXNOffset*) to ) ;
#else
    ControlEditTextSelectionRec sel ;
    Size actualSize ;
    verify_noerr( GetControlData( (ControlRef) m_macControl , 0, kControlEditTextSelectionTag, 
                    sizeof(ControlEditTextSelectionRec), &sel, &actualSize ) );
    if ( from ) *from = sel.selStart ;
    if ( to ) *to = sel.selEnd ;
#endif
}

void wxTextCtrl::SetValue(const wxString& str)
{
    // optimize redraws
    if ( GetValue() == str )
        return ;

    wxString st = str ;
    wxMacConvertNewlines13To10( &st ) ;
#if wxMAC_USE_MLTE
    {
        wxMacWindowClipper c( this ) ;
        bool formerEditable = m_editable ;
        if ( !formerEditable )
            SetEditable(true) ;

#if !wxMAC_USE_MLTE_HIVIEW
        // otherwise scrolling might have problems ?
        TPUpdateVisibility( ( (STPTextPaneVars *)m_macTXNvars)->fUserPaneRec ) ;
#endif
        SetTXNData( (STPTextPaneVars *)m_macTXNvars , (TXNObject) m_macTXN , st , kTXNStartOffset, kTXNEndOffset ) ;
        TXNSetSelection( (TXNObject) m_macTXN, 0, 0);
        TXNShowSelection( (TXNObject) m_macTXN, kTXNShowStart);
        if ( !formerEditable )
            SetEditable(formerEditable) ;
    }
#else
    wxMacCFStringHolder cf(st , m_font.GetEncoding() ) ;
    CFStringRef value = cf ;
    verify_noerr( SetControlData(  (ControlRef) m_macControl , 0, GetWindowStyle() & wxTE_PASSWORD ? 
        kControlEditTextPasswordCFStringTag : kControlEditTextCFStringTag, 
        sizeof(CFStringRef), &value ) );
#endif
}

void wxTextCtrl::SetMaxLength(unsigned long len)
{
    m_maxLength = len ;
}

bool wxTextCtrl::SetFont( const wxFont& font )
{
    if ( !wxTextCtrlBase::SetFont( font ) )
        return FALSE ;
        
#if wxMAC_USE_MLTE        
    wxMacWindowClipper c( this ) ;
    bool formerEditable = m_editable ;
    if ( !formerEditable )
        SetEditable(true) ;

    TXNTypeAttributes typeAttr[4] ;
    Str255 fontName = "\pMonaco" ;
    SInt16 fontSize = 12 ;
    Style fontStyle = normal ;
    int attrCounter = 0 ;

    wxMacStringToPascal( font.GetFaceName() , fontName ) ;
    fontSize = font.MacGetFontSize() ;
    fontStyle = font.MacGetFontStyle() ;

    typeAttr[attrCounter].tag = kTXNQDFontNameAttribute ;
    typeAttr[attrCounter].size = kTXNQDFontNameAttributeSize ;
    typeAttr[attrCounter].data.dataPtr = (void*) fontName ;
    typeAttr[attrCounter+1].tag = kTXNQDFontSizeAttribute ;
    typeAttr[attrCounter+1].size = kTXNFontSizeAttributeSize ;
    typeAttr[attrCounter+1].data.dataValue =  (fontSize << 16) ;
    typeAttr[attrCounter+2].tag = kTXNQDFontStyleAttribute ;
    typeAttr[attrCounter+2].size = kTXNQDFontStyleAttributeSize ;
    typeAttr[attrCounter+2].data.dataValue = fontStyle ;
    attrCounter += 3 ;
    /*
    typeAttr[attrCounter].tag = kTXNQDFontColorAttribute ;
    typeAttr[attrCounter].size = kTXNQDFontColorAttributeSize ;
    typeAttr[attrCounter].data.dataPtr = (void*) &color ;
    color = MAC_WXCOLORREF(GetForegroundColour().GetPixel()) ;
    attrCounter += 1 ;
    */
    verify_noerr( TXNSetTypeAttributes ((TXNObject)m_macTXN, attrCounter , typeAttr, kTXNStartOffset,kTXNEndOffset) );

    if ( !formerEditable )
        SetEditable(formerEditable) ;
#endif
    return true ;
}

bool wxTextCtrl::SetStyle(long start, long end, const wxTextAttr& style)
{
#if wxMAC_USE_MLTE
    bool formerEditable = m_editable ;
    if ( !formerEditable )
        SetEditable(true) ;
    TXNTypeAttributes typeAttr[4] ;
    Str255 fontName = "\pMonaco" ;
    SInt16 fontSize = 12 ;
    Style fontStyle = normal ;
    RGBColor color ;
    int attrCounter = 0 ;
    if ( style.HasFont() )
    {
        const wxFont &font = style.GetFont() ;
        wxMacStringToPascal( font.GetFaceName() , fontName ) ;
        fontSize = font.GetPointSize() ;
        if ( font.GetUnderlined() )
            fontStyle |= underline ;
        if ( font.GetWeight() == wxBOLD )
            fontStyle |= bold ;
        if ( font.GetStyle() == wxITALIC )
            fontStyle |= italic ;

        typeAttr[attrCounter].tag = kTXNQDFontNameAttribute ;
        typeAttr[attrCounter].size = kTXNQDFontNameAttributeSize ;
        typeAttr[attrCounter].data.dataPtr = (void*) fontName ;
        typeAttr[attrCounter+1].tag = kTXNQDFontSizeAttribute ;
        typeAttr[attrCounter+1].size = kTXNFontSizeAttributeSize ;
        typeAttr[attrCounter+1].data.dataValue =  (fontSize << 16) ;
        typeAttr[attrCounter+2].tag = kTXNQDFontStyleAttribute ;
        typeAttr[attrCounter+2].size = kTXNQDFontStyleAttributeSize ;
        typeAttr[attrCounter+2].data.dataValue = fontStyle ;
        attrCounter += 3 ;

    }
    if ( style.HasTextColour() )
    {
        typeAttr[attrCounter].tag = kTXNQDFontColorAttribute ;
        typeAttr[attrCounter].size = kTXNQDFontColorAttributeSize ;
        typeAttr[attrCounter].data.dataPtr = (void*) &color ;
        color = MAC_WXCOLORREF(style.GetTextColour().GetPixel()) ;
        attrCounter += 1 ;
    }

    if ( attrCounter > 0 )
    {
        verify_noerr( TXNSetTypeAttributes ((TXNObject)m_macTXN, attrCounter , typeAttr, start,end) );
    }
    if ( !formerEditable )
        SetEditable(formerEditable) ;
#endif
    return TRUE ;
}

bool wxTextCtrl::SetDefaultStyle(const wxTextAttr& style)
{
    wxTextCtrlBase::SetDefaultStyle( style ) ;
    SetStyle( kTXNUseCurrentSelection , kTXNUseCurrentSelection , GetDefaultStyle() ) ;
    return TRUE ;
}

// Clipboard operations
void wxTextCtrl::Copy()
{
    if (CanCopy())
    {
#if wxMAC_USE_MLTE
        ClearCurrentScrap();
        TXNCopy((TXNObject)m_macTXN);
        TXNConvertToPublicScrap();
#endif
    }
}

void wxTextCtrl::Cut()
{
    if (CanCut())
    {
#if wxMAC_USE_MLTE
        ClearCurrentScrap();
        TXNCut((TXNObject)m_macTXN);
        TXNConvertToPublicScrap();
#endif
        wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, m_windowId);
        event.SetString( GetValue() ) ;
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent(event);
      }
}

void wxTextCtrl::Paste()
{
    if (CanPaste())
    {
#if wxMAC_USE_MLTE
        TXNConvertFromPublicScrap();
        TXNPaste((TXNObject)m_macTXN);
        SetStyle( kTXNUseCurrentSelection , kTXNUseCurrentSelection , GetDefaultStyle() ) ;
#endif
        wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, m_windowId);
        event.SetString( GetValue() ) ;
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent(event);
    }
}

bool wxTextCtrl::CanCopy() const
{
    // Can copy if there's a selection
    long from, to;
    GetSelection(& from, & to);
    return (from != to);
}

bool wxTextCtrl::CanCut() const
{
    if ( !IsEditable() )
    {
        return false ;
    }
    // Can cut if there's a selection
    long from, to;
    GetSelection(& from, & to);
    return (from != to);
}

bool wxTextCtrl::CanPaste() const
{
    if (!IsEditable())
        return FALSE;

#if wxMAC_USE_MLTE
    return TXNIsScrapPastable() ;
#else
    return true ;
#endif
}

void wxTextCtrl::SetEditable(bool editable)
{
    if ( editable != m_editable )
    {
        m_editable = editable ;
#if wxMAC_USE_MLTE
        TXNControlTag tag[] = { kTXNIOPrivilegesTag } ;
        TXNControlData data[] = { { editable ? kTXNReadWrite : kTXNReadOnly } } ;
        TXNSetTXNObjectControls( (TXNObject) m_macTXN , false , sizeof(tag) / sizeof (TXNControlTag) , tag , data ) ;
#endif
    }
}

void wxTextCtrl::SetInsertionPoint(long pos)
{
    SetSelection( pos , pos ) ;
}

void wxTextCtrl::SetInsertionPointEnd()
{
    long pos = GetLastPosition();
    SetInsertionPoint(pos);
}

long wxTextCtrl::GetInsertionPoint() const
{
    long begin,end ;
    GetSelection( &begin , &end ) ;
    return begin ;
}

long wxTextCtrl::GetLastPosition() const
{
    Handle theText ;
    long actualsize = 0 ;
#if wxMAC_USE_MLTE
    OSErr err = TXNGetDataEncoded( (TXNObject) m_macTXN, kTXNStartOffset, kTXNEndOffset, &theText , kTXNTextData );
    /* all done */
    if ( err )
    {
        actualsize = 0 ;
    }
    else
    {
        actualsize = GetHandleSize( theText ) ;
        DisposeHandle( theText ) ;
    }
#endif
    return actualsize ;
}

void wxTextCtrl::Replace(long from, long to, const wxString& str)
{
#if wxMAC_USE_MLTE
    wxString value = str ;
    wxMacConvertNewlines13To10( &value ) ;

    bool formerEditable = m_editable ;
    if ( !formerEditable )
        SetEditable(true) ;
    TXNSetSelection( ((TXNObject) m_macTXN) , from , to ) ;
    TXNClear( ((TXNObject) m_macTXN) ) ;
    SetTXNData( (STPTextPaneVars *)m_macTXNvars , (TXNObject) m_macTXN , str , kTXNUseCurrentSelection, kTXNUseCurrentSelection ) ;
    if ( !formerEditable )
        SetEditable( formerEditable ) ;

    Refresh() ;
#endif
}

void wxTextCtrl::Remove(long from, long to)
{
#if wxMAC_USE_MLTE
    bool formerEditable = m_editable ;
    if ( !formerEditable )
        SetEditable(true) ;
    TXNSetSelection( ((TXNObject) m_macTXN) , from , to ) ;
    TXNClear( ((TXNObject) m_macTXN) ) ;
    if ( !formerEditable )
        SetEditable( formerEditable ) ;

    Refresh() ;
#endif
}

void wxTextCtrl::SetSelection(long from, long to)
{
#if wxMAC_USE_MLTE
    /* change the selection */
    if ((from == -1) && (to == -1))
    	TXNSelectAll((TXNObject) m_macTXN);
    else
    	TXNSetSelection( (TXNObject) m_macTXN, from, to);
    TXNShowSelection( (TXNObject) m_macTXN, kTXNShowStart);
#else
    ControlEditTextSelectionRec sel ;
    sel.selStart = from ;
    sel.selEnd = to ;
    verify_noerr( SetControlData( (ControlRef) m_macControl , 0, kControlEditTextSelectionTag, 
                    sizeof(ControlEditTextSelectionRec), &sel ) );

#endif
}

bool wxTextCtrl::LoadFile(const wxString& file)
{
    if ( wxTextCtrlBase::LoadFile(file) )
    {
        return TRUE;
    }

    return FALSE;
}

void wxTextCtrl::WriteText(const wxString& str)
{
    wxString st = str ;
    wxMacConvertNewlines13To10( &st ) ;
#if wxMAC_USE_MLTE
    bool formerEditable = m_editable ;
    if ( !formerEditable )
        SetEditable(true) ;
    {
        wxMacWindowStateSaver( this ) ;
        long start , end , dummy ;
        GetSelection( &start , &dummy ) ;
        SetTXNData( (STPTextPaneVars *)m_macTXNvars , (TXNObject) m_macTXN , st , kTXNUseCurrentSelection, kTXNUseCurrentSelection ) ;
        GetSelection( &dummy , &end ) ;
        SetStyle( start , end , GetDefaultStyle() ) ;
    }
    if ( !formerEditable )
        SetEditable( formerEditable ) ;

    MacRedrawControl() ;
#else
    wxMacCFStringHolder cf(st , m_font.GetEncoding() ) ;
    CFStringRef value = cf ;
    SetControlData(  (ControlRef) m_macControl , 0, kControlEditTextInsertCFStringRefTag, 
        sizeof(CFStringRef), &value );
#endif
}

void wxTextCtrl::AppendText(const wxString& text)
{
    SetInsertionPointEnd();
    WriteText(text);
}

void wxTextCtrl::Clear()
{
#if wxMAC_USE_MLTE
    bool formerEditable = m_editable ;
    if ( !formerEditable )
        SetEditable(true) ;
    TXNSetSelection( (TXNObject)m_macTXN , kTXNStartOffset , kTXNEndOffset ) ;
    TXNClear((TXNObject)m_macTXN);

    if ( !formerEditable )
        SetEditable( formerEditable ) ;

    Refresh() ;
#else
    SetValue(wxEmptyString) ;
#endif
}

bool wxTextCtrl::IsModified() const
{
    return m_dirty;
}

bool wxTextCtrl::IsEditable() const
{
    return IsEnabled() && m_editable ;
}

bool wxTextCtrl::AcceptsFocus() const
{
    // we don't want focus if we can't be edited
    return /*IsEditable() && */ wxControl::AcceptsFocus();
}

wxSize wxTextCtrl::DoGetBestSize() const
{
    int wText = 100 ;

    int hText;

    switch( m_windowVariant )
    {
        case wxWINDOW_VARIANT_NORMAL :
            hText = 22 ;
            break ;
        case wxWINDOW_VARIANT_SMALL :
            hText = 19 ;
            break ;
        case wxWINDOW_VARIANT_MINI :
            hText= 15 ;
            break ;
        default :
            hText = 22 ;
            break ; 
    }

    if ( m_windowStyle & wxTE_MULTILINE )
    {
         hText *= 5 ;
    }

    return wxSize(wText, hText);
}

// ----------------------------------------------------------------------------
// Undo/redo
// ----------------------------------------------------------------------------

void wxTextCtrl::Undo()
{
    if (CanUndo())
    {
#if wxMAC_USE_MLTE
        TXNUndo((TXNObject)m_macTXN); 
#endif
    }
}

void wxTextCtrl::Redo()
{
    if (CanRedo())
    {
#if wxMAC_USE_MLTE
        TXNRedo((TXNObject)m_macTXN); 
#endif
    }
}

bool wxTextCtrl::CanUndo() const
{
    if ( !IsEditable() ) 
    {
        return false ; 
    }
#if wxMAC_USE_MLTE
    return TXNCanUndo((TXNObject)m_macTXN,NULL); 
#else
    return false ;
#endif
}

bool wxTextCtrl::CanRedo() const
{
    if ( !IsEditable() ) 
    {
        return false ; 
    }
#if wxMAC_USE_MLTE
    return TXNCanRedo((TXNObject)m_macTXN,NULL); 
#else
    return false ;
#endif
}

// Makes modifie or unmodified
void wxTextCtrl::MarkDirty()
{
    m_dirty = true;
}

void wxTextCtrl::DiscardEdits()
{
    m_dirty = false;
}

int wxTextCtrl::GetNumberOfLines() const
{
    ItemCount lines = 0 ;
#if wxMAC_USE_MLTE
    TXNGetLineCount((TXNObject)m_macTXN, &lines ) ;
#endif
    return lines ;
}

long wxTextCtrl::XYToPosition(long x, long y) const
{
#if wxMAC_USE_MLTE
    Point curpt ;
    
    long lastpos = GetLastPosition() ;
    
    // TODO find a better implementation : while we can get the 
    // line metrics of a certain line, we don't get its starting
    // position, so it would probably be rather a binary search
    // for the start position
    long xpos = 0 ; 
    long ypos = 0 ;
    int lastHeight = 0 ;

    ItemCount n ;
    for ( n = 0 ; n <= (ItemCount) lastpos ; ++n )
    {
        if ( y == ypos && x == xpos )
            return n ;
        
        TXNOffsetToPoint( (TXNObject) m_macTXN,  n , &curpt);

        if ( curpt.v > lastHeight )
        {
            xpos = 0 ;
            if ( n > 0 )
                ++ypos ;
            lastHeight = curpt.v ;
        }
        else
            ++xpos ;
    }
#endif    
    return 0;
}

bool wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
#if wxMAC_USE_MLTE
    Point curpt ;
    
    long lastpos = GetLastPosition() ;
    
    if ( y ) *y = 0 ;
    if ( x ) *x = 0 ;
    
    if ( pos <= lastpos )
    {
        // TODO find a better implementation : while we can get the 
        // line metrics of a certain line, we don't get its starting
        // position, so it would probably be rather a binary search
        // for the start position
        long xpos = 0 ; 
        long ypos = 0 ;
        int lastHeight = 0 ;

        ItemCount n ;
        for ( n = 0 ; n <= (ItemCount) pos ; ++n )
        {
            TXNOffsetToPoint( (TXNObject) m_macTXN,  n , &curpt);

            if ( curpt.v > lastHeight )
            {
                xpos = 0 ;
                if ( n > 0 )
                    ++ypos ;
                lastHeight = curpt.v ;
            }
            else
                ++xpos ;
        }
        if ( y ) *y = ypos ;
        if ( x ) *x = xpos ;
    }
#else
    if ( y ) *y = 0 ;
    if ( x ) *x = 0 ;
#endif    
    return FALSE ;
}

void wxTextCtrl::ShowPosition(long pos)
{
#if wxMAC_USE_MLTE
#if TARGET_RT_MAC_MACHO && defined(AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER)
    {
        Point current ;
        Point desired ;
        TXNOffset selstart , selend ;
        TXNGetSelection(  (TXNObject) m_macTXN , &selstart , &selend) ;
        TXNOffsetToPoint( (TXNObject) m_macTXN,  selstart , &current);
        TXNOffsetToPoint( (TXNObject) m_macTXN,  pos , &desired);
        //TODO use HIPoints for 10.3 and above
        if ( (UInt32) TXNScroll != (UInt32) kUnresolvedCFragSymbolAddress )
        {
            OSErr theErr = noErr;
            SInt32 dv = desired.v - current.v ;
            SInt32 dh = desired.h - current.h ;
            TXNShowSelection(  (TXNObject) m_macTXN , true ) ;
            theErr = TXNScroll( (TXNObject) m_macTXN, kTXNScrollUnitsInPixels , kTXNScrollUnitsInPixels , &dv , &dh );          
            wxASSERT_MSG( theErr == noErr, _T("TXNScroll returned an error!") );
        }
    }
#endif
#endif
}

int wxTextCtrl::GetLineLength(long lineNo) const
{
#if wxMAC_USE_MLTE
    Point curpt ;
    if ( lineNo < GetNumberOfLines() )
    {
        // TODO find a better implementation : while we can get the 
        // line metrics of a certain line, we don't get its starting
        // position, so it would probably be rather a binary search
        // for the start position
        long xpos = 0 ; 
        long ypos = 0 ;
        int lastHeight = 0 ;
        long lastpos = GetLastPosition() ;

        ItemCount n ;
        for ( n = 0 ; n <= (ItemCount) lastpos ; ++n )
        {
            TXNOffsetToPoint( (TXNObject) m_macTXN,  n , &curpt);

            if ( curpt.v > lastHeight )
            {
                if ( ypos == lineNo )
                    return xpos ;
                    
                xpos = 0 ;
                if ( n > 0 )
                    ++ypos ;
                lastHeight = curpt.v ;
            }
            else
                ++xpos ;
        }
    }
#endif
    return 0;
}

wxString wxTextCtrl::GetLineText(long lineNo) const
{
    Point curpt ;
    wxString line ;
#if wxMAC_USE_MLTE
    wxString content = GetValue() ;

    if ( lineNo < GetNumberOfLines() )
    {
        // TODO find a better implementation : while we can get the 
        // line metrics of a certain line, we don't get its starting
        // position, so it would probably be rather a binary search
        // for the start position
        long xpos = 0 ; 
        long ypos = 0 ;
        int lastHeight = 0 ;
        long lastpos = GetLastPosition() ;

        ItemCount n ;
        for ( n = 0 ; n <= (ItemCount)lastpos ; ++n )
        {
            TXNOffsetToPoint( (TXNObject) m_macTXN,  n , &curpt);

            if ( curpt.v > lastHeight )
            {
                if ( ypos == lineNo )
                    return line ;
                    
                xpos = 0 ;
                if ( n > 0 )
                    ++ypos ;
                lastHeight = curpt.v ;
            }
            else
            {
                if ( ypos == lineNo )
                    line += content[n] ;
                ++xpos ;
            }
        }
    }
#endif
    return line ;
}

/*
 * Text item
 */

void wxTextCtrl::Command(wxCommandEvent & event)
{
    SetValue (event.GetString());
    ProcessCommand (event);
}

void wxTextCtrl::OnDropFiles(wxDropFilesEvent& event)
{
    // By default, load the first file into the text window.
    if (event.GetNumberOfFiles() > 0)
    {
        LoadFile(event.GetFiles()[0]);
    }
}

void wxTextCtrl::OnChar(wxKeyEvent& event)
{
    int key = event.GetKeyCode() ;
    bool eat_key = false ;

    if ( key == 'c' && event.MetaDown() )
    {
        if ( CanCopy() )
            Copy() ;
        return ;
    }

    if ( !IsEditable() && key != WXK_LEFT && key != WXK_RIGHT && key != WXK_DOWN && key != WXK_UP && key != WXK_TAB &&
        !( key == WXK_RETURN && ( (m_windowStyle & wxPROCESS_ENTER) || (m_windowStyle & wxTE_MULTILINE) ) )
/*        && key != WXK_PRIOR && key != WXK_NEXT && key != WXK_HOME && key != WXK_END */
        )
    {
        // eat it
        return ;
    }

    // assume that any key not processed yet is going to modify the control
    m_dirty = true;

    if ( key == 'v' && event.MetaDown() )
    {
        if ( CanPaste() )
            Paste() ;
        return ;
    }
    if ( key == 'x' && event.MetaDown() )
    {
        if ( CanCut() )
            Cut() ;
        return ;
    }
    switch ( key )
    {
        case WXK_RETURN:
            if (m_windowStyle & wxPROCESS_ENTER)
            {
                wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, m_windowId);
                event.SetEventObject( this );
                event.SetString( GetValue() );
                if ( GetEventHandler()->ProcessEvent(event) )
                    return;
            }
            if ( !(m_windowStyle & wxTE_MULTILINE) )
            {
                wxWindow *parent = GetParent();
                while( parent && !parent->IsTopLevel() && parent->GetDefaultItem() == NULL ) {
                  parent = parent->GetParent() ;
                }
                if ( parent && parent->GetDefaultItem() )
                {
                    wxButton *def = wxDynamicCast(parent->GetDefaultItem(),
                                                          wxButton);
                    if ( def && def->IsEnabled() )
                    {
                        wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, def->GetId() );
                        event.SetEventObject(def);
                        def->Command(event);
                        return ;
                   }
                }

                // this will make wxWindows eat the ENTER key so that
                // we actually prevent line wrapping in a single line
                // text control
                eat_key = TRUE;
            }

            break;

        case WXK_TAB:
            // always produce navigation event - even if we process TAB
            // ourselves the fact that we got here means that the user code
            // decided to skip processing of this TAB - probably to let it
            // do its default job.
            {
                wxNavigationKeyEvent eventNav;
                eventNav.SetDirection(!event.ShiftDown());
                eventNav.SetWindowChange(event.ControlDown());
                eventNav.SetEventObject(this);

                if ( GetParent()->GetEventHandler()->ProcessEvent(eventNav) )
                    return;

                event.Skip() ;
                return;
            }
            break;
    }

    if (!eat_key)
    {
        // perform keystroke handling
        if ( wxTheApp->MacGetCurrentEvent() != NULL && wxTheApp->MacGetCurrentEventHandlerCallRef() != NULL )
            CallNextEventHandler((EventHandlerCallRef)wxTheApp->MacGetCurrentEventHandlerCallRef() , (EventRef) wxTheApp->MacGetCurrentEvent() ) ;
        else 
        {
            EventRecord rec ;
            if ( wxMacConvertEventToRecord(  (EventRef) wxTheApp->MacGetCurrentEvent() , &rec ) )
            {
                EventRecord *ev = &rec ;
                short keycode ;
                short keychar ;
                keychar = short(ev->message & charCodeMask);
                keycode = short(ev->message & keyCodeMask) >> 8 ;

                ::HandleControlKey( (ControlRef) m_macControl , keycode , keychar , ev->modifiers ) ;
            }
        }
    }
    if ( ( key >= 0x20 && key < WXK_START ) ||
         key == WXK_RETURN ||
         key == WXK_DELETE ||
         key == WXK_BACK)
    {
        wxCommandEvent event1(wxEVT_COMMAND_TEXT_UPDATED, m_windowId);
        event1.SetString( GetValue() ) ;
        event1.SetEventObject( this );
        wxPostEvent(GetEventHandler(),event1);
    }
}

// ----------------------------------------------------------------------------
// standard handlers for standard edit menu events
// ----------------------------------------------------------------------------

void wxTextCtrl::OnCut(wxCommandEvent& WXUNUSED(event))
{
    Cut();
}

void wxTextCtrl::OnCopy(wxCommandEvent& WXUNUSED(event))
{
    Copy();
}

void wxTextCtrl::OnPaste(wxCommandEvent& WXUNUSED(event))
{
    Paste();
}

void wxTextCtrl::OnUndo(wxCommandEvent& WXUNUSED(event))
{
    Undo();
}

void wxTextCtrl::OnRedo(wxCommandEvent& WXUNUSED(event))
{
    Redo();
}

void wxTextCtrl::OnUpdateCut(wxUpdateUIEvent& event)
{
    event.Enable( CanCut() );
}

void wxTextCtrl::OnUpdateCopy(wxUpdateUIEvent& event)
{
    event.Enable( CanCopy() );
}

void wxTextCtrl::OnUpdatePaste(wxUpdateUIEvent& event)
{
    event.Enable( CanPaste() );
}

void wxTextCtrl::OnUpdateUndo(wxUpdateUIEvent& event)
{
    event.Enable( CanUndo() );
}

void wxTextCtrl::OnUpdateRedo(wxUpdateUIEvent& event)
{
    event.Enable( CanRedo() );
}

bool wxTextCtrl::MacSetupCursor( const wxPoint& pt )
{
    return true ;
}

// user pane implementation

void wxTextCtrl::MacControlUserPaneDrawProc(wxInt16 part) 
{
}

wxInt16 wxTextCtrl::MacControlUserPaneHitTestProc(wxInt16 x, wxInt16 y) 
{
    return kControlNoPart ;
}

wxInt16 wxTextCtrl::MacControlUserPaneTrackingProc(wxInt16 x, wxInt16 y, void* actionProc) 
{
    return kControlNoPart ;
}

void wxTextCtrl::MacControlUserPaneIdleProc() 
{
}

wxInt16 wxTextCtrl::MacControlUserPaneKeyDownProc(wxInt16 keyCode, wxInt16 charCode, wxInt16 modifiers) 
{
    return kControlNoPart ;
}

void wxTextCtrl::MacControlUserPaneActivateProc(bool activating) 
{
}

wxInt16 wxTextCtrl::MacControlUserPaneFocusProc(wxInt16 action) 
{
    return kControlNoPart ;
}

void wxTextCtrl::MacControlUserPaneBackgroundProc(void* info) 
{
}

#endif
    // wxUSE_TEXTCTRL
