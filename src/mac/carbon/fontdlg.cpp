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

#ifdef __GNUG__
#pragma implementation "fontdlg.h"
#endif

#include "wx/mac/fontdlg.h"
#include "wx/cmndata.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxFontDialog, wxDialog)
#endif

#include "wx/mac/private.h"

#if defined( __WXMAC__ ) && TARGET_API_MAC_OSX && ( MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_2 )

#include <ATSUnicode.h>

#include "wx/msgdlg.h"

pascal OSStatus wxFontDialogEventHandler(	EventHandlerCallRef inHandlerCallRef,
									EventRef event, void* pData)
{
    wxASSERT(GetEventClass(event) == kEventClassFont && 
             GetEventKind(event) == kEventFontSelection);
   
    OSStatus status = noErr;
    
    ATSUFontID fontid;
    Fixed fontsize;
    RGBColor fontcolor;
    FMFontStyle fontstyle;
        
    status = GetEventParameter (event, kEventParamFMFontStyle,
                                    typeFMFontStyle, NULL,
                                    sizeof (FMFontStyle), 
                                    NULL, &(fontstyle)); 
    check_noerr (status); 

    status = GetEventParameter (event, kEventParamATSUFontID,
                                    typeATSUFontID, NULL,
                                    sizeof (ATSUFontID), 
                                    NULL, &(fontid)); 
    check_noerr (status); 

    status = GetEventParameter (event, kEventParamATSUFontSize,
                                    typeATSUSize, NULL,
                                    sizeof (Fixed), 
                                    NULL, &(fontsize)); 

    check_noerr (status);

    status = GetEventParameter (event, kEventParamFontColor,
                                    typeRGBColor, NULL,
                                    sizeof( RGBColor ), NULL, &fontcolor); 
    check_noerr (status);
    
    //TODO:
    //TODO:	Make a utility function for converting ATSU to WX font (what we're doing :))
    //TODO:
        
    //now do the conversion to the wx font data
    wxFontData theFontData;
    wxFont 	   theFont;
 
    //first, the easy part - set the color
    wxColour theColor;
    theColor.Set(&(WXCOLORREF&)fontcolor);
    theFontData.SetColour(theColor);
    
    //
    //now the hard part - convert the atsu font to a wx font
    //
    
    //point size - fixed, but straight conversion, I think
    theFont.SetPointSize(fontsize >> 16);
    
    //font name - first get length, then allocate/copy/deallocate name buffer
    FontNameCode 		theNameCode;
    FontPlatformCode	thePlatformCode; 	
    FontScriptCode		theScriptCode;
    FontLanguageCode	theLanguageCode;    
    //the above have types in SFNTTypes.h

    ByteCount 			theActualLength;
    ATSUGetIndFontName(fontid, 
                       0, //first font in index array
                       0, //need to get length first
                       NULL, //nothin'
                       &theActualLength,
                       &theNameCode,
                       &thePlatformCode,
                       &theScriptCode,
                       &theLanguageCode);
    
    Ptr szBuffer = NewPtr(theActualLength);
    ATSUGetIndFontName(fontid, 
                       0, //first font in index array
                       theActualLength, 
                       szBuffer, 
                       &theActualLength,
                       &theNameCode,
                       &thePlatformCode,
                       &theScriptCode,
                       &theLanguageCode);
    
    //its unicode - convert it to wx's char value and put it in there
    theFont.SetFaceName(wxConvLocal.cMB2WX((char*)szBuffer));
    DisposePtr(szBuffer);
    
    //TODOTODO: Get font family - mayby by the script code?
    theFont.SetFamily(wxDEFAULT);  

    //TODOTODO: Get other styles?  Font weight?
    theFont.SetStyle(((fontstyle & italic) ? wxFONTSTYLE_ITALIC : 0));
    theFont.SetWeight((fontstyle & bold) ? wxBOLD : wxNORMAL);   
    theFont.SetUnderlined(((fontstyle & underline) ? true : false));
    
    //for debugging
    //wxPrintf(wxT("FaceName:%s\nSize:%i\nStyle:%i\n"), theFont.GetFaceName().c_str(), theFont.GetPointSize(),
    //theFont.GetStyle());
    
    //phew!!  We're done - set the chosen font
    theFontData.SetChosenFont(theFont);
    ((wxFontDialog*)pData)->SetData(theFontData);
    
    return status;
}

DEFINE_ONE_SHOT_HANDLER_GETTER( wxFontDialogEventHandler );

//---------------------------
//	Class implementation
//---------------------------

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
    
    //finally, show the font dialog
    if( (status = FPShowHideFontPanel()) == noErr)
    {
        while(FPIsFontPanelVisible()) 
        {
            //yeild so we can get events
            ::wxSafeYield(m_dialogParent, false);
        }
    }
    else
        return wxID_CANCEL;
    
    return wxID_OK;
}


#else
   //10.2+ only
   
   
//
//	no native implementation
//

wxFontDialog::wxFontDialog()
{
    m_dialogParent = NULL;
}

wxFontDialog::wxFontDialog(wxWindow *parent, const wxFontData&  data)
{
    Create(parent, data);
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