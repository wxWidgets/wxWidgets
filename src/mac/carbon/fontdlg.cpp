/////////////////////////////////////////////////////////////////////////////
// Name:        fontdlg.cpp
// Purpose:     wxFontDialog class for carbon 10.2+.
// Author:      Ryan Norton
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
#pragma implementation "fontdlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/cmndata.h"
#endif

#include "wx/mac/fontdlg.h"
    
#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxFontDialog, wxDialog)
#endif

#include "wx/mac/private.h"

//Mac OSX 10.2+ only
#if defined( __WXMAC__ ) && TARGET_API_MAC_OSX && ( MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_2 )

#include <ATSUnicode.h>

#include "wx/msgdlg.h"

//color isn't working in carbon impl
#define ISCOLORWORKING 0

// ============================================================================
// implementation
// ============================================================================

// ---------------------------------------------------------------------------
// Carbon event callback(s)
// ---------------------------------------------------------------------------

pascal OSStatus wxFontDialogEventHandler(	EventHandlerCallRef inHandlerCallRef,
									EventRef event, void* pData)
{
    wxASSERT(GetEventClass(event) == kEventClassFont && 
             GetEventKind(event) == kEventFontSelection);
   
    OSStatus status = noErr;
    
    FMFontFamily fontfamily;
    FMFontStyle fontstyle;
    FMFontSize fontsize;
#if ISCOLORWORKING
    RGBColor fontcolor;
#endif
        
    status = GetEventParameter (event, kEventParamFMFontFamily,
                                    typeFMFontFamily, NULL,
                                    sizeof (FMFontFamily), 
                                    NULL, &(fontfamily)); 
    check_noerr (status); 

    status = GetEventParameter (event, kEventParamFMFontStyle,
                                    typeFMFontStyle, NULL,
                                    sizeof (FMFontStyle), 
                                    NULL, &(fontstyle)); 
    check_noerr (status); 

    status = GetEventParameter (event, kEventParamFMFontSize,
                                    typeFMFontSize, NULL,
                                    sizeof (FMFontSize), 
                                    NULL, &(fontsize)); 

    check_noerr (status);

#if ISCOLORWORKING
    status = GetEventParameter (event, kEventParamFontColor,
                                    typeRGBColor, NULL,
                                    sizeof( RGBColor ), NULL, &fontcolor); 
    check_noerr (status);
#endif
    
    //now do the conversion to the wx font data
    wxFontData theFontData;
    wxFont 	   theFont;

#if ISCOLORWORKING 
    //set color
    wxColour theColor;
    theColor.Set(&(WXCOLORREF&)fontcolor);
    theFontData.SetColour(theColor);
#endif
        
    //set size
    theFont.SetPointSize(fontsize);
    
    //set name
    Str255 theFontName;
    GetFontName(fontfamily, theFontName);
    theFont.SetFaceName(wxMacMakeStringFromPascal(theFontName));
      
    //TODOTODO: Get font family - mayby by the script code?
    theFont.SetFamily(wxDEFAULT);  

    //TODOTODO: Get other styles?
    theFont.SetStyle(((fontstyle & italic) ? wxFONTSTYLE_ITALIC : 0));
    theFont.SetWeight((fontstyle & bold) ? wxBOLD : wxNORMAL);   
    theFont.SetUnderlined(((fontstyle & underline) ? true : false));
        
    //phew!!  We're done - set the chosen font
    theFontData.SetChosenFont(theFont);
    ((wxFontDialog*)pData)->SetData(theFontData);
    
    return status;
}

DEFINE_ONE_SHOT_HANDLER_GETTER( wxFontDialogEventHandler );

// ---------------------------------------------------------------------------
// wxFontDialog
// ---------------------------------------------------------------------------

wxFontDialog::wxFontDialog() :
    m_dialogParent(NULL), m_pEventHandlerRef(NULL)
{
}

wxFontDialog::wxFontDialog(wxWindow *parent, const wxFontData&  data)
{
    Create(parent, data);
}

wxFontDialog::~wxFontDialog()
{
    if (m_pEventHandlerRef)
        ::RemoveEventHandler((EventHandlerRef&)m_pEventHandlerRef);
}

void wxFontDialog::SetData(wxFontData& fontdata)
{
    m_fontData = fontdata;
}

bool wxFontDialog::Create(wxWindow *parent, const wxFontData& data)
{
    m_dialogParent = parent;
    m_fontData = data;

    //Register the events that will return this dialog
	EventTypeSpec ftEventList[] = { { kEventClassFont, kEventFontSelection } };

    OSStatus err = noErr;
  
//FIXMEFIXME: Why doesn't it recieve events if there's a parent?
//    if (parent)
//    {
//       err = InstallWindowEventHandler(
//            MAC_WXHWND(parent->GetHandle()),
//            GetwxFontDialogEventHandlerUPP(),
//            GetEventTypeCount(ftEventList), ftEventList, 
//			this, (&(EventHandlerRef&)m_pEventHandlerRef));
//            
//    }
//    else //no parent - send to app
//    {
        err = InstallApplicationEventHandler(
            GetwxFontDialogEventHandlerUPP(),
            GetEventTypeCount(ftEventList), ftEventList, 
			this, (&(EventHandlerRef&)m_pEventHandlerRef));
//	  }
    		
    return err == noErr;
}

bool wxFontDialog::IsShown() const
{
    return FPIsFontPanelVisible();
}

int wxFontDialog::ShowModal()
{
    wxASSERT(!FPIsFontPanelVisible());
    
    //set up initial font
    wxFont theInitialFont = m_fontData.GetInitialFont();

    //create ATSU style
    ATSUStyle theStyle;
    OSStatus status = ATSUCreateStyle(&theStyle);
    check_noerr(status);

    //put stuff into the style - we don't need kATSUColorTag    
    ATSUFontID 				fontid = theInitialFont.MacGetATSUFontID();
    Fixed					fontsize = theInitialFont.MacGetFontSize() << 16;
    ATSUAttributeTag    	theTags[2] = { kATSUFontTag, kATSUSizeTag };
    ByteCount           	theSizes[2] = { sizeof(ATSUFontID), sizeof(Fixed) };
    ATSUAttributeValuePtr   theValues[2] = { &fontid, 
                                             &fontsize };
    
    //set the stuff for the ATSU style
    verify_noerr (ATSUSetAttributes (theStyle, 2, theTags, theSizes, theValues) ); 
    
    //they set us up the bomb!  Set the initial font of the dialog
    SetFontInfoForSelection(kFontSelectionATSUIType,
                            1, 
                            &theStyle,
                            (HIObjectRef) 
                                (m_dialogParent ? 
                                 GetWindowEventTarget(MAC_WXHWND(m_dialogParent->GetHandle())) :
                                 GetApplicationEventTarget())
                            );
    
    //dispose of the style
    status = ATSUDisposeStyle(theStyle);
    check_noerr(status);
    
    //in case the user doesn't choose anything -
    //if he doesn't we'll get a bad font with red text
    m_fontData.SetChosenFont(m_fontData.GetInitialFont());
    m_fontData.SetColour(wxColour(0,0,0));
    
    //finally, show the font dialog
    if( (status = FPShowHideFontPanel()) == noErr)
    {
        while(FPIsFontPanelVisible()) 
        {
            //yeild so we can get events
            wxTheApp->Yield(false);
        }
    }
    else
        return wxID_CANCEL;
    
    return wxID_OK;
}


#else
   //10.2+ only
   
// ---------------------------------------------------------------------------
// wxFontDialog stub for mac OS's without a native font dialog
// ---------------------------------------------------------------------------

wxFontDialog::wxFontDialog()
{
    m_dialogParent = NULL;
}

wxFontDialog::wxFontDialog(wxWindow *parent, const wxFontData&  data)
{
    Create(parent, data);
}

wxFontDialog::~wxFontDialog()
{
    // empty
}

void wxFontDialog::SetData(wxFontData& fontdata)
{
    m_fontData = fontdata;
}

bool wxFontDialog::Create(wxWindow *parent, const wxFontData& data)
{
    m_dialogParent = parent;

    m_fontData = data;

    // TODO: you may need to do dialog creation here, unless it's
    // done in ShowModal.
    return TRUE;
}

bool wxFontDialog::IsShown() const
{
    return false;
}

int wxFontDialog::ShowModal()
{
    // TODO: show (maybe create) the dialog
    return wxID_CANCEL;
}

#endif // 10.2+