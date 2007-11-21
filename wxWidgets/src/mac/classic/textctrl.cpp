/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/textctrl.cpp
// Purpose:     wxTextCtrl
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TEXTCTRL

#include "wx/textctrl.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/dc.h"
    #include "wx/button.h"
    #include "wx/settings.h"
    #include "wx/msgdlg.h"
    #include "wx/toplevel.h"
#endif

#ifdef __DARWIN__
    #include <sys/types.h>
    #include <sys/stat.h>
#else
    #include <stat.h>
#endif

#if wxUSE_STD_IOSTREAM
    #if wxUSE_IOSTREAMH
        #include <fstream.h>
    #else
        #include <fstream>
    #endif
#endif

#include "wx/notebook.h"
#include "wx/tabctrl.h"
#include "wx/filefn.h"

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

extern wxControl *wxFindControlFromMacControl(ControlHandle inControl ) ;

// CS:TODO we still have a problem getting properly at the text events of a control because under Carbon
// the MLTE engine registers itself for the key events thus the normal flow never occurs, the only measure for the
// moment is to avoid setting the true focus on the control, the proper solution at the end would be to have
// an alternate path for carbon key events that routes automatically into the same wx flow of events

/* part codes */

/* kmUPTextPart is the part code we return to indicate the user has clicked
    in the text area of our control */
#define kmUPTextPart 1

/* kmUPScrollPart is the part code we return to indicate the user has clicked
    in the scroll bar part of the control. */
#define kmUPScrollPart 2


/* routines for using existing user pane controls.
    These routines are useful for cases where you would like to use an
    existing user pane control in, say, a dialog window as a scrolling
    text edit field.*/

/* mUPOpenControl initializes a user pane control so it will be drawn
    and will behave as a scrolling text edit field inside of a window.
    This routine performs all of the initialization steps necessary,
    except it does not create the user pane control itself.  theControl
    should refer to a user pane control that you have either created
    yourself or extracted from a dialog's control heirarchy using
    the GetDialogItemAsControl routine.  */
OSStatus mUPOpenControl(ControlHandle theControl, long wxStyle);

/* Utility Routines */

enum {
    kShiftKeyCode = 56
};

/* kUserClickedToFocusPart is a part code we pass to the SetKeyboardFocus
    routine.  In our focus switching routine this part code is understood
    as meaning 'the user has clicked in the control and we need to switch
    the current focus to ourselves before we can continue'. */
#define kUserClickedToFocusPart 100


/* kmUPClickScrollDelayTicks is a time measurement in ticks used to
    slow the speed of 'auto scrolling' inside of our clickloop routine.
    This value prevents the text from wizzzzzing by while the mouse
    is being held down inside of the text area. */
#define kmUPClickScrollDelayTicks 3


/* STPTextPaneVars is a structure used for storing the the mUP Control's
    internal variables and state information.  A handle to this record is
    stored in the pane control's reference value field using the
    SetControlReference routine. */

typedef struct {
        /* OS records referenced */
    TXNObject fTXNRec; /* the txn record */
    TXNFrameID fTXNFrame; /* the txn frame ID */
    ControlHandle fUserPaneRec;  /* handle to the user pane control */
    WindowPtr fOwner; /* window containing control */
    GrafPtr fDrawingEnvironment; /* grafport where control is drawn */
        /* flags */
    Boolean fInFocus; /* true while the focus rect is drawn around the control */
    Boolean fIsActive; /* true while the control is drawn in the active state */
    Boolean fTEActive; /* reflects the activation state of the text edit record */
    Boolean fInDialogWindow; /* true if displayed in a dialog window */
        /* calculated locations */
    Rect fRTextArea; /* area where the text is drawn */
    Rect fRFocusOutline;  /* rectangle used to draw the focus box */
    Rect fRTextOutline; /* rectangle used to draw the border */
    RgnHandle fTextBackgroundRgn; /* background region for the text, erased before calling TEUpdate */
        /* our focus advance override routine */
    EventHandlerUPP handlerUPP;
    EventHandlerRef handlerRef;
    bool fMultiline ;
} STPTextPaneVars;




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

/* TPActivatePaneText activates or deactivates the text edit record
    according to the value of setActive.  The primary purpose of this
    routine is to ensure each call is only made once. */
static void TPActivatePaneText(STPTextPaneVars **tpvars, Boolean setActive) {
    STPTextPaneVars *varsp;
    varsp = *tpvars;
    if (varsp->fTEActive != setActive) {

        varsp->fTEActive = setActive;

        TXNActivate(varsp->fTXNRec, varsp->fTXNFrame, varsp->fTEActive);

        if (varsp->fInFocus)
            TXNFocus( varsp->fTXNRec, varsp->fTEActive);
    }
}


/* TPFocusPaneText set the focus state for the text record. */
static void TPFocusPaneText(STPTextPaneVars **tpvars, Boolean setFocus) {
    STPTextPaneVars *varsp;
    varsp = *tpvars;
    if (varsp->fInFocus != setFocus) {
        varsp->fInFocus = setFocus;
        TXNFocus( varsp->fTXNRec, varsp->fInFocus);
    }
}


/* TPPaneDrawProc is called to redraw the control and for update events
    referring to the control.  This routine erases the text area's background,
    and redraws the text.  This routine assumes the scroll bar has been
    redrawn by a call to DrawControls. */
static pascal void TPPaneDrawProc(ControlRef theControl, ControlPartCode thePart) {
    STPTextPaneVars **tpvars, *varsp;
    char state;
    Rect bounds;
        /* set up our globals */

    tpvars = (STPTextPaneVars **) GetControlReference(theControl);
    if (tpvars != NULL) {
        state = HGetState((Handle) tpvars);
        HLock((Handle) tpvars);
        varsp = *tpvars;

            /* save the drawing state */
        SetPort((**tpvars).fDrawingEnvironment);
           /* verify our boundary */
        GetControlBounds(theControl, &bounds);

        wxMacWindowClipper clipper( wxFindControlFromMacControl(theControl ) ) ;
        if ( ! EqualRect(&bounds, &varsp->fRFocusOutline) ) {
            // scrollbar is on the border, we add one
            Rect oldbounds = varsp->fRFocusOutline ;
            InsetRect( &oldbounds , -1 , -1 ) ;

            if ( IsControlVisible( theControl ) )
                InvalWindowRect( GetControlOwner( theControl ) , &oldbounds ) ;
            SetRect(&varsp->fRFocusOutline, bounds.left, bounds.top, bounds.right, bounds.bottom);
            SetRect(&varsp->fRTextOutline, bounds.left, bounds.top, bounds.right, bounds.bottom);
            SetRect(&varsp->fRTextArea, bounds.left + 2 , bounds.top + (varsp->fMultiline ? 0 : 2) ,
                bounds.right - (varsp->fMultiline ? 0 : 2), bounds.bottom - (varsp->fMultiline ? 0 : 2));
            RectRgn(varsp->fTextBackgroundRgn, &varsp->fRTextOutline);
            if ( IsControlVisible( theControl ) )
                TXNSetFrameBounds(  varsp->fTXNRec, varsp->fRTextArea.top, varsp->fRTextArea.left,
                    varsp->fRTextArea.bottom, varsp->fRTextArea.right, varsp->fTXNFrame);
            else
                TXNSetFrameBounds(  varsp->fTXNRec, varsp->fRTextArea.top + 30000 , varsp->fRTextArea.left + 30000 ,
                    varsp->fRTextArea.bottom + 30000 , varsp->fRTextArea.right + 30000 , varsp->fTXNFrame);

        }

        if ( IsControlVisible( theControl ) )
        {
            /* update the text region */
            RGBColor white = { 65535 , 65535 , 65535 } ;
            RGBBackColor( &white ) ;
            EraseRgn(varsp->fTextBackgroundRgn);
            TXNDraw(varsp->fTXNRec, NULL);
                /* restore the drawing environment */
                /* draw the text frame and focus frame (if necessary) */
            DrawThemeEditTextFrame(&varsp->fRTextOutline, varsp->fIsActive ? kThemeStateActive: kThemeStateInactive);
            if ((**tpvars).fIsActive && varsp->fInFocus)
                DrawThemeFocusRect(&varsp->fRFocusOutline, true);
                /* release our globals */
            HSetState((Handle) tpvars, state);
        }
    }
}


/* TPPaneHitTestProc is called when the control manager would
    like to determine what part of the control the mouse resides over.
    We also call this routine from our tracking proc to determine how
    to handle mouse clicks. */
static pascal ControlPartCode TPPaneHitTestProc(ControlHandle theControl, Point where) {
    STPTextPaneVars **tpvars;
    ControlPartCode result;
    char state;
        /* set up our locals and lock down our globals*/
    result = 0;
    tpvars = (STPTextPaneVars **) GetControlReference(theControl);
    if (tpvars != NULL && IsControlVisible( theControl) ) {
        state = HGetState((Handle) tpvars);
        HLock((Handle) tpvars);
            /* find the region where we clicked */
        if (PtInRect(where, &(**tpvars).fRTextArea)) {
            result = kmUPTextPart;
        } else result = 0;
            /* release oure globals */
        HSetState((Handle) tpvars, state);
    }
    return result;
}





/* TPPaneTrackingProc is called when the mouse is being held down
    over our control.  This routine handles clicks in the text area
    and in the scroll bar. */
static pascal ControlPartCode TPPaneTrackingProc(ControlHandle theControl, Point startPt, ControlActionUPP actionProc) {
    STPTextPaneVars **tpvars, *varsp;
    char state;
    ControlPartCode partCodeResult;
        /* make sure we have some variables... */
    partCodeResult = 0;
    tpvars = (STPTextPaneVars **) GetControlReference(theControl);
    if (tpvars != NULL && IsControlVisible( theControl ) ) {
            /* lock 'em down */
        state = HGetState((Handle) tpvars);
        HLock((Handle) tpvars);
        varsp = *tpvars;
            /* we don't do any of these functions unless we're in focus */
        if ( ! varsp->fInFocus) {
            WindowPtr owner;
            owner = GetControlOwner(theControl);
            ClearKeyboardFocus(owner);
            SetKeyboardFocus(owner, theControl, kUserClickedToFocusPart);
        }
            /* find the location for the click */
        switch (TPPaneHitTestProc(theControl, startPt)) {

                /* handle clicks in the text part */
            case kmUPTextPart:
                {   SetPort((**tpvars).fDrawingEnvironment);
                    wxMacWindowClipper clipper( wxFindControlFromMacControl(theControl ) ) ;
#if !TARGET_CARBON
                    TXNClick( varsp->fTXNRec, (const EventRecord*) wxTheApp->MacGetCurrentEvent());
#else
                    EventRecord rec ;
                    ConvertEventRefToEventRecord( (EventRef) wxTheApp->MacGetCurrentEvent() , &rec ) ;
                    TXNClick( varsp->fTXNRec, &rec );
#endif
                }
                break;

        }

        HSetState((Handle) tpvars, state);
    }
    return partCodeResult;
}


/* TPPaneIdleProc is our user pane idle routine.  When our text field
    is active and in focus, we use this routine to set the cursor. */
static pascal void TPPaneIdleProc(ControlHandle theControl) {
    STPTextPaneVars **tpvars, *varsp;
        /* set up locals */
    tpvars = (STPTextPaneVars **) GetControlReference(theControl);
    if (tpvars != NULL && IsControlVisible( theControl ) ) {
            /* if we're not active, then we have nothing to say about the cursor */
        if ((**tpvars).fIsActive) {
            char state;
            Rect bounds;
            Point mousep;
                /* lock down the globals */
            state = HGetState((Handle) tpvars);
            HLock((Handle) tpvars);
            varsp = *tpvars;
                /* get the current mouse coordinates (in our window) */
            SetPortWindowPort(GetControlOwner(theControl));
            wxMacWindowClipper clipper( wxFindControlFromMacControl(theControl ) ) ;
            GetMouse(&mousep);
                /* there's a 'focus thing' and an 'unfocused thing' */
            if (varsp->fInFocus) {
                    /* flash the cursor */
                SetPort((**tpvars).fDrawingEnvironment);
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
                GetControlBounds(theControl, &bounds);
                if (PtInRect(mousep, &bounds))
                {
                //    SetThemeCursor(kThemeArrowCursor);
                }
            }

            HSetState((Handle) tpvars, state);
        }
    }
}


/* TPPaneKeyDownProc is called whenever a keydown event is directed
    at our control.  Here, we direct the keydown event to the text
    edit record and redraw the scroll bar and text field as appropriate. */
static pascal ControlPartCode TPPaneKeyDownProc(ControlHandle theControl,
                            SInt16 keyCode, SInt16 charCode, SInt16 modifiers) {
    STPTextPaneVars **tpvars;
    tpvars = (STPTextPaneVars **) GetControlReference(theControl);
    if (tpvars != NULL) {
        if ((**tpvars).fInFocus) {
                /* turn autoscrolling on and send the key event to text edit */
            SetPort((**tpvars).fDrawingEnvironment);
            wxMacWindowClipper clipper( wxFindControlFromMacControl(theControl ) ) ;
            EventRecord ev ;
            memset( &ev , 0 , sizeof( ev ) ) ;
            ev.what = keyDown ;
            ev.modifiers = modifiers ;
            ev.message = (( keyCode << 8 ) & keyCodeMask ) + ( charCode & charCodeMask ) ;
            TXNKeyDown( (**tpvars).fTXNRec, &ev);
        }
    }
    return kControlEntireControl;
}


/* TPPaneActivateProc is called when the window containing
    the user pane control receives activate events. Here, we redraw
    the control and it's text as necessary for the activation state. */
static pascal void TPPaneActivateProc(ControlHandle theControl, Boolean activating) {
    Rect bounds;
    STPTextPaneVars **tpvars, *varsp;
    char state;
        /* set up locals */
    tpvars = (STPTextPaneVars **) GetControlReference(theControl);
    if (tpvars != NULL) {
        state = HGetState((Handle) tpvars);
        HLock((Handle) tpvars);
        varsp = *tpvars;
            /* de/activate the text edit record */
        SetPort((**tpvars).fDrawingEnvironment);
        wxMacWindowClipper clipper( wxFindControlFromMacControl(theControl ) ) ;
        GetControlBounds(theControl, &bounds);
        varsp->fIsActive = activating;
        TPActivatePaneText(tpvars, varsp->fIsActive && varsp->fInFocus);
            /* redraw the frame */
        if ( IsControlVisible( theControl ) )
        {
            DrawThemeEditTextFrame(&varsp->fRTextOutline, varsp->fIsActive ? kThemeStateActive: kThemeStateInactive);
            if (varsp->fInFocus)
                DrawThemeFocusRect(&varsp->fRFocusOutline, varsp->fIsActive);
        }
        HSetState((Handle) tpvars, state);
    }
}


/* TPPaneFocusProc is called when every the focus changes to or
    from our control.  Herein, switch the focus appropriately
    according to the parameters and redraw the control as
    necessary.  */
static pascal ControlPartCode TPPaneFocusProc(ControlHandle theControl, ControlFocusPart action) {
    ControlPartCode focusResult;
    STPTextPaneVars **tpvars, *varsp;
    char state;
        /* set up locals */
    focusResult = kControlFocusNoPart;
    tpvars = (STPTextPaneVars **) GetControlReference(theControl);
    if (tpvars != NULL ) {
        state = HGetState((Handle) tpvars);
        HLock((Handle) tpvars);
        varsp = *tpvars;
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
                TPFocusPaneText(tpvars, false);
                focusResult = kControlFocusNoPart;
                break;
            case kUserClickedToFocusPart:
                TPFocusPaneText(tpvars, true);
                focusResult = 1;
                break;
            case kControlFocusPrevPart:
            case kControlFocusNextPart:
                TPFocusPaneText(tpvars, ( ! varsp->fInFocus));
                focusResult = varsp->fInFocus ? 1 : kControlFocusNoPart;
                break;
        }
        TPActivatePaneText(tpvars, varsp->fIsActive && varsp->fInFocus);
        /* redraw the text fram and focus rectangle to indicate the
        new focus state */
        if ( IsControlVisible( theControl ) )
        {
           /* save the drawing state */
            SetPort((**tpvars).fDrawingEnvironment);
            wxMacWindowClipper clipper( wxFindControlFromMacControl(theControl ) ) ;
            DrawThemeEditTextFrame(&varsp->fRTextOutline, varsp->fIsActive ? kThemeStateActive: kThemeStateInactive);
            DrawThemeFocusRect(&varsp->fRFocusOutline, varsp->fIsActive && varsp->fInFocus);
        }
            /* done */
        HSetState((Handle) tpvars, state);
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
OSStatus mUPOpenControl(ControlHandle theControl, long wxStyle )
{
    Rect bounds;
    WindowRef theWindow;
    STPTextPaneVars **tpvars, *varsp;
    OSStatus err = noErr ;
    RGBColor rgbWhite = {0xFFFF, 0xFFFF, 0xFFFF};
    TXNBackground tback;

        /* set up our globals */
    if (gTPDrawProc == NULL) gTPDrawProc = NewControlUserPaneDrawUPP(TPPaneDrawProc);
    if (gTPHitProc == NULL) gTPHitProc = NewControlUserPaneHitTestUPP(TPPaneHitTestProc);
    if (gTPTrackProc == NULL) gTPTrackProc = NewControlUserPaneTrackingUPP(TPPaneTrackingProc);
    if (gTPIdleProc == NULL) gTPIdleProc = NewControlUserPaneIdleUPP(TPPaneIdleProc);
    if (gTPKeyProc == NULL) gTPKeyProc = NewControlUserPaneKeyDownUPP(TPPaneKeyDownProc);
    if (gTPActivateProc == NULL) gTPActivateProc = NewControlUserPaneActivateUPP(TPPaneActivateProc);
    if (gTPFocusProc == NULL) gTPFocusProc = NewControlUserPaneFocusUPP(TPPaneFocusProc);

        /* allocate our private storage */
    tpvars = (STPTextPaneVars **) NewHandleClear(sizeof(STPTextPaneVars));
    SetControlReference(theControl, (long) tpvars);
    HLock((Handle) tpvars);
    varsp = *tpvars;
        /* set the initial settings for our private data */
    varsp->fMultiline = wxStyle & wxTE_MULTILINE ;
    varsp->fInFocus = false;
    varsp->fIsActive = true;
    varsp->fTEActive = true; // in order to get a deactivate
    varsp->fUserPaneRec = theControl;
    theWindow = varsp->fOwner = GetControlOwner(theControl);

    varsp->fDrawingEnvironment = (GrafPtr)  GetWindowPort(theWindow);

    varsp->fInDialogWindow = ( GetWindowKind(varsp->fOwner) == kDialogWindowKind );
        /* set up the user pane procedures */
    SetControlData(theControl, kControlEntireControl, kControlUserPaneDrawProcTag, sizeof(gTPDrawProc), &gTPDrawProc);
    SetControlData(theControl, kControlEntireControl, kControlUserPaneHitTestProcTag, sizeof(gTPHitProc), &gTPHitProc);
    SetControlData(theControl, kControlEntireControl, kControlUserPaneTrackingProcTag, sizeof(gTPTrackProc), &gTPTrackProc);
    SetControlData(theControl, kControlEntireControl, kControlUserPaneIdleProcTag, sizeof(gTPIdleProc), &gTPIdleProc);
    SetControlData(theControl, kControlEntireControl, kControlUserPaneKeyDownProcTag, sizeof(gTPKeyProc), &gTPKeyProc);
    SetControlData(theControl, kControlEntireControl, kControlUserPaneActivateProcTag, sizeof(gTPActivateProc), &gTPActivateProc);
    SetControlData(theControl, kControlEntireControl, kControlUserPaneFocusProcTag, sizeof(gTPFocusProc), &gTPFocusProc);
        /* calculate the rectangles used by the control */
    GetControlBounds(theControl, &bounds);
    SetRect(&varsp->fRFocusOutline, bounds.left, bounds.top, bounds.right, bounds.bottom);
    SetRect(&varsp->fRTextOutline, bounds.left, bounds.top, bounds.right, bounds.bottom);
    SetRect(&varsp->fRTextArea, bounds.left + 2 , bounds.top + (varsp->fMultiline ? 0 : 2) ,
        bounds.right - (varsp->fMultiline ? 0 : 2), bounds.bottom - (varsp->fMultiline ? 0 : 2));
        /* calculate the background region for the text.  In this case, it's kindof
        and irregular region because we're setting the scroll bar a little ways inside
        of the text area. */
    RectRgn((varsp->fTextBackgroundRgn = NewRgn()), &varsp->fRTextOutline);

        /* set up the drawing environment */
    SetPort(varsp->fDrawingEnvironment);

        /* create the new edit field */

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

    if ( wxStyle & wxTE_READONLY )
        frameOptions |= kTXNReadOnlyMask ;

    TXNNewObject(NULL, varsp->fOwner, &varsp->fRTextArea,
        frameOptions ,
        kTXNTextEditStyleFrameType,
        kTXNTextensionFile,
        kTXNSystemDefaultEncoding,
        &varsp->fTXNRec, &varsp->fTXNFrame, (TXNObjectRefcon) tpvars);

    if ( !IsControlVisible( theControl ) )
        TXNSetFrameBounds(  varsp->fTXNRec, varsp->fRTextArea.top + 30000 , varsp->fRTextArea.left + 30000 ,
            varsp->fRTextArea.bottom + 30000 , varsp->fRTextArea.right + 30000 , varsp->fTXNFrame);


    if ( (wxStyle & wxTE_MULTILINE) && (wxStyle & wxTE_DONTWRAP) )
    {
        TXNControlTag tag = kTXNWordWrapStateTag ;
        TXNControlData dat ;
        dat.uValue = kTXNNoAutoWrap ;
        TXNSetTXNObjectControls( varsp->fTXNRec , false , 1 , &tag , &dat ) ;
    }
        Str255 fontName ;
        SInt16 fontSize ;
        Style fontStyle ;

        GetThemeFont(kThemeSmallSystemFont , GetApplicationScript() , fontName , &fontSize , &fontStyle ) ;

        TXNTypeAttributes typeAttr[] =
        {
            {   kTXNQDFontNameAttribute , kTXNQDFontNameAttributeSize , { (void*) fontName } } ,
            {   kTXNQDFontSizeAttribute , kTXNFontSizeAttributeSize , { (void*) (fontSize << 16) } } ,
            {   kTXNQDFontStyleAttribute , kTXNQDFontStyleAttributeSize , {  (void*) normal } } ,
        } ;

    err = TXNSetTypeAttributes (varsp->fTXNRec, sizeof( typeAttr ) / sizeof(TXNTypeAttributes) , typeAttr,
          kTXNStartOffset,
          kTXNEndOffset);
        /* set the field's background */

    tback.bgType = kTXNBackgroundTypeRGB;
    tback.bg.color = rgbWhite;
    TXNSetBackground( varsp->fTXNRec, &tback);

        /* unlock our storage */
    HUnlock((Handle) tpvars);
        /* perform final activations and setup for our text field.  Here,
        we assume that the window is going to be the 'active' window. */
    TPActivatePaneText(tpvars, varsp->fIsActive && varsp->fInFocus);
        /* all done */
    return err;
}




IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl, wxTextCtrlBase)

BEGIN_EVENT_TABLE(wxTextCtrl, wxTextCtrlBase)
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

static void SetTXNData( TXNObject txn , const wxString& st , TXNOffset start , TXNOffset end )
{
#if wxUSE_UNICODE
#if SIZEOF_WCHAR_T == 2
    size_t len = st.length() ;
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

// Text item
void wxTextCtrl::Init()
{
  m_macTE = NULL ;
  m_macTXN = NULL ;
  m_macTXNvars = NULL ;
  m_macUsesTXN = false ;

  m_editable = true ;
  m_dirty = false;

  m_maxLength = TE_UNLIMITED_LENGTH ;
}

wxTextCtrl::~wxTextCtrl()
{
    if ( m_macUsesTXN )
    {
        SetControlReference((ControlHandle)m_macControl, 0) ;
        TXNDeleteObject((TXNObject)m_macTXN);
        /* delete our private storage */
        DisposeHandle((Handle) m_macTXNvars);
        /* zero the control reference */
    }
}

const short kVerticalMargin = 2 ;
const short kHorizontalMargin = 2 ;

bool wxTextCtrl::Create(wxWindow *parent, wxWindowID id,
           const wxString& str,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    m_macTE = NULL ;
    m_macTXN = NULL ;
    m_macTXNvars = NULL ;
    m_macUsesTXN = false ;
    m_editable = true ;

    m_macUsesTXN = ! (style & wxTE_PASSWORD ) ;

    m_macUsesTXN &= (TXNInitTextension != (void*) kUnresolvedCFragSymbolAddress) ;

    // base initialization
    if ( !wxTextCtrlBase::Create(parent, id, pos, size, style & ~(wxHSCROLL|wxVSCROLL), validator, name) )
        return false;

    wxSize mySize = size ;
    if ( m_macUsesTXN )
    {
        m_macHorizontalBorder = 5 ; // additional pixels around the real control
        m_macVerticalBorder = 3 ;
    }
    else
    {
        m_macHorizontalBorder = 5 ; // additional pixels around the real control
        m_macVerticalBorder = 5 ;
    }


    Rect bounds ;
    Str255 title ;
    /*
    if ( mySize.y == -1 )
    {
        mySize.y = 13 ;
        if ( m_windowStyle & wxTE_MULTILINE )
            mySize.y *= 5 ;

        mySize.y += 2 * m_macVerticalBorder ;
    }
    */
    MacPreControlCreate( parent , id ,  wxEmptyString , pos , mySize ,style, validator , name , &bounds , title ) ;

    if ( m_windowStyle & wxTE_MULTILINE )
    {
        wxASSERT_MSG( !(m_windowStyle & wxTE_PROCESS_ENTER),
                      wxT("wxTE_PROCESS_ENTER style is ignored for multiline text controls (they always process it)") );

        m_windowStyle |= wxTE_PROCESS_ENTER;
    }

    if ( m_windowStyle & wxTE_READONLY)
    {
        m_editable = false ;
    }

    wxString st = str ;
    wxMacConvertNewlines13To10( &st ) ;
    if ( !m_macUsesTXN )
    {
        m_macControl = (WXWidget) ::NewControl( MAC_WXHWND(parent->MacGetRootWindow()) , &bounds , "\p" , false , 0 , 0 , 1,
            (style & wxTE_PASSWORD) ? kControlEditTextPasswordProc : kControlEditTextProc , (long) this ) ;
        long size ;
        ::GetControlData((ControlHandle)  m_macControl , 0, kControlEditTextTEHandleTag , sizeof( TEHandle ) , (char*)((TEHandle *)&m_macTE) , &size ) ;

    }
    else
    {
        short featurSet;

        featurSet = kControlSupportsEmbedding | kControlSupportsFocus  | kControlWantsIdle
                | kControlWantsActivate | kControlHandlesTracking | kControlHasSpecialBackground
                | kControlGetsFocusOnClick | kControlSupportsLiveFeedback;
            /* create the control */
        m_macControl = (WXWidget) ::NewControl(MAC_WXHWND(parent->MacGetRootWindow()), &bounds, "\p", false , featurSet, 0, featurSet, kControlUserPaneProc, 0);
            /* set up the mUP specific features and data */
        mUPOpenControl((ControlHandle) m_macControl, m_windowStyle );
    }
    MacPostControlCreate() ;

    if ( !m_macUsesTXN )
    {
        wxCharBuffer text = st.mb_str(wxConvLocal) ;
        ::SetControlData( (ControlHandle) m_macControl, 0, ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag , strlen(text) , text ) ;
    }
    else
    {
        STPTextPaneVars **tpvars;
            /* set up locals */
        tpvars = (STPTextPaneVars **) GetControlReference((ControlHandle) m_macControl);
            /* set the text in the record */
        m_macTXN =  (**tpvars).fTXNRec ;
        SetTXNData( (TXNObject) m_macTXN , st , kTXNStartOffset, kTXNEndOffset ) ;
        m_macTXNvars = tpvars ;
        m_macUsesTXN = true ;
        TXNSetSelection( (TXNObject) m_macTXN, 0, 0);
        TXNShowSelection( (TXNObject) m_macTXN, kTXNShowStart);
    }

    return true;
}

wxString wxTextCtrl::GetValue() const
{
    Size actualSize = 0;
    wxString result ;
    OSStatus err ;
    if ( !m_macUsesTXN )
    {
        err = ::GetControlDataSize((ControlHandle) m_macControl, 0,
            ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag, &actualSize ) ;

        if ( err )
            return wxEmptyString ;

        if ( actualSize > 0 )
        {
            wxCharBuffer buf(actualSize) ;
            ::GetControlData( (ControlHandle) m_macControl, 0,
                ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag,
                actualSize , buf.data() , &actualSize ) ;
            result = wxString( buf , wxConvLocal) ;
        }
    }
    else
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
                wxChar *ptr = result.GetWriteBuf(actualSize*sizeof(wxChar)) ;
#if SIZEOF_WCHAR_T == 2
                wxStrncpy( ptr , (wxChar*) *theText , actualSize ) ;
#else
                wxMBConvUTF16BE converter ;
                HLock( theText ) ;
                converter.MB2WC( ptr , (const char*)*theText , actualSize ) ;
                HUnlock( theText ) ;
#endif
                ptr[actualSize] = 0 ;
                result.UngetWriteBuf( actualSize *sizeof(wxChar) ) ;
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
    wxMacConvertNewlines10To13( &result ) ;
    return result ;
}

void wxTextCtrl::GetSelection(long* from, long* to) const
{
  if ( !m_macUsesTXN )
  {
    *from = (**((TEHandle) m_macTE)).selStart;
    *to = (**((TEHandle) m_macTE)).selEnd;
   }
   else
   {
        TXNGetSelection( (TXNObject) m_macTXN , (TXNOffset*) from , (TXNOffset*) to ) ;
   }
}

void wxTextCtrl::SetValue(const wxString& str)
{
    wxString st = str ;
    wxMacConvertNewlines13To10( &st ) ;
    if ( !m_macUsesTXN )
    {
        wxCharBuffer text =  st.mb_str(wxConvLocal) ;
        ::SetControlData( (ControlHandle) m_macControl, 0, ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag , strlen(text) , text ) ;
    }
    else
    {
        bool formerEditable = m_editable ;
        if ( !formerEditable )
            SetEditable(true) ;
        SetTXNData( (TXNObject) m_macTXN , st , kTXNStartOffset, kTXNEndOffset ) ;
        TXNSetSelection( (TXNObject) m_macTXN, 0, 0);
        TXNShowSelection( (TXNObject) m_macTXN, kTXNShowStart);
        if ( !formerEditable )
            SetEditable(formerEditable) ;
    }
    MacRedrawControl() ;
}

void wxTextCtrl::SetMaxLength(unsigned long len)
{
    m_maxLength = len ;
}

bool wxTextCtrl::SetStyle(long start, long end, const wxTextAttr& style)
{
    if ( m_macUsesTXN )
    {
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
#ifdef __WXDEBUG__
            OSStatus status =
#endif // __WXDEBUG__
                TXNSetTypeAttributes ((TXNObject)m_macTXN, attrCounter , typeAttr, start,end);
            wxASSERT_MSG( status == noErr , wxT("Couldn't set text attributes") ) ;
        }
        if ( !formerEditable )
            SetEditable(formerEditable) ;
    }
    return true ;
}

bool wxTextCtrl::SetDefaultStyle(const wxTextAttr& style)
{
    wxTextCtrlBase::SetDefaultStyle( style ) ;
    SetStyle( kTXNUseCurrentSelection , kTXNUseCurrentSelection , GetDefaultStyle() ) ;
    return true ;
}

// Clipboard operations
void wxTextCtrl::Copy()
{
    if (CanCopy())
    {
      if ( !m_macUsesTXN )
      {
            TECopy( ((TEHandle) m_macTE) ) ;
            ClearCurrentScrap();
            TEToScrap() ;
            MacRedrawControl() ;
        }
        else
        {
            ClearCurrentScrap();
            TXNCopy((TXNObject)m_macTXN);
            TXNConvertToPublicScrap();
        }
    }
}

void wxTextCtrl::Cut()
{
    if (CanCut())
    {
        if ( !m_macUsesTXN )
        {
            TECut( ((TEHandle) m_macTE) ) ;
            ClearCurrentScrap();
            TEToScrap() ;
            MacRedrawControl() ;
        }
        else
        {
            ClearCurrentScrap();
            TXNCut((TXNObject)m_macTXN);
            TXNConvertToPublicScrap();
        }
        wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, m_windowId);
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent(event);
      }
}

void wxTextCtrl::Paste()
{
    if (CanPaste())
    {
        if ( !m_macUsesTXN )
        {
            TEFromScrap() ;
            TEPaste( (TEHandle) m_macTE ) ;
            MacRedrawControl() ;
        }
        else
        {
            TXNConvertFromPublicScrap();
            TXNPaste((TXNObject)m_macTXN);
            SetStyle( kTXNUseCurrentSelection , kTXNUseCurrentSelection , GetDefaultStyle() ) ;
        }
        wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, m_windowId);
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
        return false;

#if TARGET_CARBON
    OSStatus err = noErr;
    ScrapRef scrapRef;

    err = GetCurrentScrap( &scrapRef );
    if ( err != noTypeErr && err != memFullErr )
    {
        ScrapFlavorFlags    flavorFlags;
        Size                byteCount;

        if (( err = GetScrapFlavorFlags( scrapRef, 'TEXT', &flavorFlags )) == noErr)
        {
            if (( err = GetScrapFlavorSize( scrapRef, 'TEXT', &byteCount )) == noErr)
            {
                return true ;
            }
        }
    }
    return false;

#else
    long offset ;
    if ( GetScrap( NULL , 'TEXT' , &offset ) > 0 )
    {
        return true ;
    }
#endif
    return false ;
}

void wxTextCtrl::SetEditable(bool editable)
{
    if ( editable != m_editable )
    {
        m_editable = editable ;
        if ( !m_macUsesTXN )
        {
            if ( editable )
                UMAActivateControl( (ControlHandle) m_macControl ) ;
            else
                UMADeactivateControl((ControlHandle)  m_macControl ) ;
        }
        else
        {
            TXNControlTag tag[] = { kTXNIOPrivilegesTag } ;
            TXNControlData data[] = { { editable ? kTXNReadWrite : kTXNReadOnly } } ;
            TXNSetTXNObjectControls( (TXNObject) m_macTXN , false , sizeof(tag) / sizeof (TXNControlTag) , tag , data ) ;
        }
    }
}

void wxTextCtrl::SetInsertionPoint(long pos)
{
    SetSelection( pos , pos ) ;
}

void wxTextCtrl::SetInsertionPointEnd()
{
    wxTextPos pos = GetLastPosition();
    SetInsertionPoint(pos);
}

long wxTextCtrl::GetInsertionPoint() const
{
    long begin,end ;
    GetSelection( &begin , &end ) ;
    return begin ;
}

wxTextPos wxTextCtrl::GetLastPosition() const
{
    if ( !m_macUsesTXN )
    {
        return (**((TEHandle) m_macTE)).teLength ;
    }
    else
    {
        Handle theText ;
        long actualsize ;
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
        return actualsize ;
    }
}

void wxTextCtrl::Replace(long from, long to, const wxString& str)
{
    wxString value = str ;
    wxMacConvertNewlines13To10( &value ) ;
    if ( !m_macUsesTXN )
    {
        ControlEditTextSelectionRec selection ;

        selection.selStart = from ;
        selection.selEnd = to ;
        ::SetControlData((ControlHandle)  m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection ) ;
        TESetSelect( from , to  , ((TEHandle) m_macTE) ) ;
        TEDelete( ((TEHandle) m_macTE) ) ;
        TEInsert( value , value.length() , ((TEHandle) m_macTE) ) ;
    }
    else
    {
        bool formerEditable = m_editable ;
        if ( !formerEditable )
            SetEditable(true) ;
        TXNSetSelection( ((TXNObject) m_macTXN) , from , to ) ;
        TXNClear( ((TXNObject) m_macTXN) ) ;
        SetTXNData( (TXNObject) m_macTXN , str , kTXNUseCurrentSelection, kTXNUseCurrentSelection ) ;
        if ( !formerEditable )
            SetEditable( formerEditable ) ;
    }
    Refresh() ;
}

void wxTextCtrl::Remove(long from, long to)
{
    if ( !m_macUsesTXN )
    {
        ControlEditTextSelectionRec selection ;

        selection.selStart = from ;
        selection.selEnd = to ;
        ::SetControlData( (ControlHandle) m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection ) ;
        TEDelete( ((TEHandle) m_macTE) ) ;
    }
    else
    {
        bool formerEditable = m_editable ;
        if ( !formerEditable )
            SetEditable(true) ;
        TXNSetSelection( ((TXNObject) m_macTXN) , from , to ) ;
        TXNClear( ((TXNObject) m_macTXN) ) ;
        if ( !formerEditable )
            SetEditable( formerEditable ) ;
    }
    Refresh() ;
}

void wxTextCtrl::SetSelection(long from, long to)
{
    if ( !m_macUsesTXN )
    {
        ControlEditTextSelectionRec selection ;
        if ((from == -1) && (to == -1))
        {
            selection.selStart = 0 ;
            selection.selEnd = 32767 ;
        }
        else
        {
            selection.selStart = from ;
            selection.selEnd = to ;
        }

        TESetSelect( selection.selStart , selection.selEnd , ((TEHandle) m_macTE) ) ;
        ::SetControlData((ControlHandle)  m_macControl , 0, kControlEditTextSelectionTag , sizeof( selection ) , (char*) &selection ) ;
    }
    else
    {
        STPTextPaneVars **tpvars;
        /* set up our locals */
        tpvars = (STPTextPaneVars **) GetControlReference((ControlHandle) m_macControl);
        /* and our drawing environment as the operation
        may force a redraw in the text area. */
        SetPort((**tpvars).fDrawingEnvironment);
        /* change the selection */
        if ((from == -1) && (to == -1))
            TXNSelectAll((TXNObject) m_macTXN);
        else
            TXNSetSelection( (**tpvars).fTXNRec, from, to);
        TXNShowSelection( (TXNObject) m_macTXN, kTXNShowStart);
    }
}

void wxTextCtrl::WriteText(const wxString& str)
{
    wxString st = str ;
    wxMacConvertNewlines13To10( &st ) ;
    if ( !m_macUsesTXN )
    {
        wxCharBuffer text =  st.mb_str(wxConvLocal) ;
        TEInsert( text , strlen(text) , ((TEHandle) m_macTE) ) ;
    }
    else
    {
        bool formerEditable = m_editable ;
        if ( !formerEditable )
            SetEditable(true) ;
        long start , end , dummy ;
        GetSelection( &start , &dummy ) ;
        SetTXNData( (TXNObject) m_macTXN , st , kTXNUseCurrentSelection, kTXNUseCurrentSelection ) ;
        GetSelection( &dummy , &end ) ;
        SetStyle( start , end , GetDefaultStyle() ) ;
        if ( !formerEditable )
            SetEditable( formerEditable ) ;
    }
    MacRedrawControl() ;
}

void wxTextCtrl::AppendText(const wxString& text)
{
    SetInsertionPointEnd();
    WriteText(text);
}

void wxTextCtrl::Clear()
{
    if ( !m_macUsesTXN )
    {
        ::SetControlData((ControlHandle)  m_macControl, 0, ( m_windowStyle & wxTE_PASSWORD ) ? kControlEditTextPasswordTag : kControlEditTextTextTag , 0 , (char*) ((const char*)NULL) ) ;
    }
    else
    {
        TXNSetSelection( (TXNObject)m_macTXN , kTXNStartOffset , kTXNEndOffset ) ;
        TXNClear((TXNObject)m_macTXN);
    }
    Refresh() ;
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
    if ( m_macUsesTXN )
    {
        hText = 17 ;
    }
    else
    {
        hText = 13 ;
    }
/*
    int cx, cy;
    wxGetCharSize(GetHWND(), &cx, &cy, &GetFont());

    int wText = DEFAULT_ITEM_WIDTH;

    int hText = EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy);

    return wxSize(wText, hText);
*/
    if ( m_windowStyle & wxTE_MULTILINE )
    {
         hText *= 5 ;
    }
    hText += 2 * m_macVerticalBorder ;
    wText += 2 * m_macHorizontalBorder ;
    //else: for single line control everything is ok
    return wxSize(wText, hText);
}

// ----------------------------------------------------------------------------
// Undo/redo
// ----------------------------------------------------------------------------

void wxTextCtrl::Undo()
{
    if (CanUndo())
    {
        if ( m_macUsesTXN )
        {
            TXNUndo((TXNObject)m_macTXN);
        }
    }
}

void wxTextCtrl::Redo()
{
    if (CanRedo())
    {
        if ( m_macUsesTXN )
        {
            TXNRedo((TXNObject)m_macTXN);
        }
    }
}

bool wxTextCtrl::CanUndo() const
{
    if ( !IsEditable() )
    {
        return false ;
    }
    if ( m_macUsesTXN )
    {
        return TXNCanUndo((TXNObject)m_macTXN,NULL);
    }
    return false ;
}

bool wxTextCtrl::CanRedo() const
{
    if ( !IsEditable() )
    {
        return false ;
    }
    if ( m_macUsesTXN )
    {
        return TXNCanRedo((TXNObject)m_macTXN,NULL);
    }
    return false ;
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
    if ( m_macUsesTXN )
    {
        ItemCount lines ;
        TXNGetLineCount((TXNObject)m_macTXN, &lines ) ;
        return lines ;
    }
    else
    {
        wxString content = GetValue() ;

        int count = 1;
        for (size_t i = 0; i < content.length() ; i++)
        {
            if (content[i] == '\r') count++;
        }
        return count;
    }
}

long wxTextCtrl::XYToPosition(long x, long y) const
{
    // TODO
    return 0;
}

bool wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    return false ;
}

void wxTextCtrl::ShowPosition(long pos)
{
#if TARGET_RT_MAC_MACHO && defined(AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER)
    if ( m_macUsesTXN )
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
}

int wxTextCtrl::GetLineLength(long lineNo) const
{
    // TODO change this if possible to reflect real lines
    wxString content = GetValue() ;

    // Find line first
    int count = 0;
    for (size_t i = 0; i < content.length() ; i++)
    {
        if (count == lineNo)
        {
            // Count chars in line then
            count = 0;
            for (size_t j = i; j < content.length(); j++)
            {
                count++;
                if (content[j] == '\n') return count;
            }

            return count;
        }
        if (content[i] == '\n') count++;
    }
    return 0;
}

wxString wxTextCtrl::GetLineText(long lineNo) const
{
    // TODO change this if possible to reflect real lines
    wxString content = GetValue() ;

    // Find line first
    int count = 0;
    for (size_t i = 0; i < content.length() ; i++)
    {
        if (count == lineNo)
        {
            // Add chars in line then
            wxString tmp;

            for (size_t j = i; j < content.length(); j++)
            {
                if (content[j] == '\n')
                    return tmp;

                tmp += content[j];
            }

            return tmp;
        }
        if (content[i] == '\n') count++;
    }
    return wxEmptyString ;
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
        !( key == WXK_RETURN && ( (m_windowStyle & wxTE_PROCESS_ENTER) || (m_windowStyle & wxTE_MULTILINE) ) )
/*        && key != WXK_PAGEUP && key != WXK_PAGEDOWN && key != WXK_HOME && key != WXK_END */
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
            if (m_windowStyle & wxTE_PROCESS_ENTER)
            {
                wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, m_windowId);
                event.SetEventObject( this );
                event.SetString( GetValue() );
                if ( GetEventHandler()->ProcessEvent(event) )
                    return;
            }
            if ( !(m_windowStyle & wxTE_MULTILINE) )
            {
                wxTopLevelWindow *tlw = wxDynamicCast(wxGetTopLevelParent(this), wxTopLevelWindow);
                if ( tlw && tlw->GetDefaultItem() )
                {
                    wxButton *def = wxDynamicCast(tlw->GetDefaultItem(), wxButton);
                    if ( def && def->IsEnabled() )
                    {
                        wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, def->GetId() );
                        event.SetEventObject(def);
                        def->Command(event);
                        return ;
                   }
                }

                // this will make wxWidgets eat the ENTER key so that
                // we actually prevent line wrapping in a single line
                // text control
                eat_key = true;
            }

            break;

        case WXK_TAB:
            if ( !(m_windowStyle & wxTE_PROCESS_TAB))
            {
                int flags = 0;
                if (!event.ShiftDown())
                    flags |= wxNavigationKeyEvent::IsForward ;
                if (event.ControlDown())
                    flags |= wxNavigationKeyEvent::WinChange ;
                Navigate(flags);
                return;
            }
            else
            {
                // This is necessary (don't know why) or the tab will not
                // be inserted.
                WriteText(wxT("\t"));
            }
            break;
    }

    if (!eat_key)
    {
        // perform keystroke handling
#if TARGET_CARBON
        if ( m_macUsesTXN && wxTheApp->MacGetCurrentEvent() != NULL && wxTheApp->MacGetCurrentEventHandlerCallRef() != NULL )
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

                ::HandleControlKey( (ControlHandle) m_macControl , keycode , keychar , ev->modifiers ) ;
            }
        }
#else
        EventRecord *ev = (EventRecord*) wxTheApp->MacGetCurrentEvent() ;
        short keycode ;
        short keychar ;
        keychar = short(ev->message & charCodeMask);
        keycode = short(ev->message & keyCodeMask) >> 8 ;

        ::HandleControlKey( (ControlHandle) m_macControl , keycode , keychar , ev->modifiers ) ;
#endif
    }
    if ( ( key >= 0x20 && key < WXK_START ) ||
         key == WXK_RETURN ||
         key == WXK_DELETE ||
         key == WXK_BACK)
    {
        wxCommandEvent event1(wxEVT_COMMAND_TEXT_UPDATED, m_windowId);
        event1.SetEventObject( this );
        wxPostEvent(GetEventHandler(),event1);
    }
}

void  wxTextCtrl::MacSuperShown( bool show )
{
    bool former = m_macControlIsShown ;
    wxControl::MacSuperShown( show ) ;
    if ( (former != m_macControlIsShown) && m_macUsesTXN )
    {
        if ( m_macControlIsShown )
            TXNSetFrameBounds( (TXNObject) m_macTXN, (**(STPTextPaneVars **)m_macTXNvars).fRTextArea.top, (**(STPTextPaneVars **)m_macTXNvars).fRTextArea.left,
                (**(STPTextPaneVars **)m_macTXNvars).fRTextArea.bottom,(**(STPTextPaneVars **)m_macTXNvars).fRTextArea.right, (**(STPTextPaneVars **)m_macTXNvars).fTXNFrame);
        else
            TXNSetFrameBounds( (TXNObject) m_macTXN, (**(STPTextPaneVars **)m_macTXNvars).fRTextArea.top + 30000, (**(STPTextPaneVars **)m_macTXNvars).fRTextArea.left,
               (**(STPTextPaneVars **)m_macTXNvars).fRTextArea.bottom + 30000, (**(STPTextPaneVars **)m_macTXNvars).fRTextArea.right, (**(STPTextPaneVars **)m_macTXNvars).fTXNFrame);
    }
}

bool  wxTextCtrl::Show(bool show)
{
    bool former = m_macControlIsShown ;

    bool retval = wxControl::Show( show ) ;

    if ( former != m_macControlIsShown && m_macUsesTXN )
    {
        if ( m_macControlIsShown )
            TXNSetFrameBounds( (TXNObject) m_macTXN, (**(STPTextPaneVars **)m_macTXNvars).fRTextArea.top, (**(STPTextPaneVars **)m_macTXNvars).fRTextArea.left,
                (**(STPTextPaneVars **)m_macTXNvars).fRTextArea.bottom,(**(STPTextPaneVars **)m_macTXNvars).fRTextArea.right, (**(STPTextPaneVars **)m_macTXNvars).fTXNFrame);
        else
            TXNSetFrameBounds( (TXNObject) m_macTXN, (**(STPTextPaneVars **)m_macTXNvars).fRTextArea.top + 30000, (**(STPTextPaneVars **)m_macTXNvars).fRTextArea.left,
               (**(STPTextPaneVars **)m_macTXNvars).fRTextArea.bottom + 30000, (**(STPTextPaneVars **)m_macTXNvars).fRTextArea.right, (**(STPTextPaneVars **)m_macTXNvars).fTXNFrame);
    }

    return retval ;
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
    if ( m_macUsesTXN )
        return true ;
    else
        return wxWindow::MacSetupCursor( pt ) ;
}

#endif
    // wxUSE_TEXTCTRL
