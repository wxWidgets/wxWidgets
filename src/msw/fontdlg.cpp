/////////////////////////////////////////////////////////////////////////////
// Name:        fontdlg.cpp
// Purpose:     wxFontDialog class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "fontdlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/defs.h"
#include "wx/utils.h"
#include "wx/dialog.h"
#endif

#include "wx/fontdlg.h"

#include <windows.h>

#ifndef __WIN32__
#include <commdlg.h>
#endif

#include "wx/msw/private.h"
#include "wx/cmndata.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define wxDIALOG_DEFAULT_X 300
#define wxDIALOG_DEFAULT_Y 300

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxFontDialog, wxDialog)
#endif

/*
 * wxFontDialog
 */


wxFontDialog::wxFontDialog(void)
{
  dialogParent = NULL;
}

wxFontDialog::wxFontDialog(wxWindow *parent, wxFontData *data)
{
  Create(parent, data);
}

bool wxFontDialog::Create(wxWindow *parent, wxFontData *data)
{
  dialogParent = parent;
  
  if (data)
    fontData = *data;
  return TRUE;
}

int wxFontDialog::ShowModal(void)
{
    CHOOSEFONT chooseFontStruct;
    LOGFONT logFont;

    DWORD flags = CF_TTONLY | CF_SCREENFONTS | CF_NOSIMULATIONS;

    memset(&chooseFontStruct, 0, sizeof(CHOOSEFONT));

    chooseFontStruct.lStructSize = sizeof(CHOOSEFONT);
    chooseFontStruct.hwndOwner = (HWND) (dialogParent ? (HWND) dialogParent->GetHWND() : NULL);
    chooseFontStruct.lpLogFont = &logFont;

    if (fontData.initialFont.Ok())
    {
      flags |= CF_INITTOLOGFONTSTRUCT;
      wxFillLogFont(&logFont, & fontData.initialFont);
    }

    chooseFontStruct.iPointSize = 0;
    chooseFontStruct.rgbColors = RGB((BYTE)fontData.fontColour.Red(), (BYTE)fontData.fontColour.Green(), (BYTE)fontData.fontColour.Blue());

    if (!fontData.GetAllowSymbols())
      flags |= CF_ANSIONLY;
    if (fontData.GetEnableEffects())
      flags |= CF_EFFECTS;
    if (fontData.GetShowHelp())
      flags |= CF_SHOWHELP;
    if (!(fontData.minSize == 0 && fontData.maxSize == 0))
    {
      chooseFontStruct.nSizeMin = fontData.minSize;
      chooseFontStruct.nSizeMax = fontData.maxSize;
      flags |= CF_LIMITSIZE;
    }

    chooseFontStruct.Flags = flags;
    chooseFontStruct.nFontType = SCREEN_FONTTYPE;
    bool success = (ChooseFont(&(chooseFontStruct)) != 0);

    // Restore values
    if (success)
    {
      fontData.fontColour.Set(GetRValue(chooseFontStruct.rgbColors), GetGValue(chooseFontStruct.rgbColors),
       GetBValue(chooseFontStruct.rgbColors));
      fontData.chosenFont = wxCreateFontFromLogFont(&logFont);
    }

    return success ? wxID_OK : wxID_CANCEL;
}

void wxFillLogFont(LOGFONT *logFont, wxFont *font)
{
    BYTE ff_italic;
    int ff_weight = 0;
    int ff_family = 0;
    wxString ff_face("");

    switch (font->GetFamily())
    {
      case wxSCRIPT:     ff_family = FF_SCRIPT ;
                         ff_face = "Script" ;
                         break ;
      case wxDECORATIVE: ff_family = FF_DECORATIVE;
                         break;
      case wxROMAN:      ff_family = FF_ROMAN;
                         ff_face = "Times New Roman" ;
                         break;
      case wxTELETYPE:
      case wxMODERN:     ff_family = FF_MODERN;
                         ff_face = "Courier New" ;
                         break;
      case wxSWISS:      ff_family = FF_SWISS;
                         ff_face = "Arial";
                         break;
      case wxDEFAULT:
      default:           ff_family = FF_SWISS;
                         ff_face = "MS Sans Serif" ; 
    }

    if (font->GetStyle() == wxITALIC || font->GetStyle() == wxSLANT)
      ff_italic = 1;
    else
      ff_italic = 0;

    if (font->GetWeight() == wxNORMAL)
      ff_weight = FW_NORMAL;
    else if (font->GetWeight() == wxLIGHT)
      ff_weight = FW_LIGHT;
    else if (font->GetWeight() == wxBOLD)
      ff_weight = FW_BOLD;

    // Have to get screen DC Caps, because a metafile will return 0.
    HDC dc2 = ::GetDC(NULL);
    int ppInch = ::GetDeviceCaps(dc2, LOGPIXELSY);
    ::ReleaseDC(NULL, dc2);

    // New behaviour: apparently ppInch varies according to
    // Large/Small Fonts setting in Windows. This messes
    // up fonts. So, set ppInch to a constant 96 dpi.
    ppInch = 96;

#if FONT_SIZE_COMPATIBILITY
    // Incorrect, but compatible with old wxWindows behaviour
    int nHeight = (font->GetPointSize()*ppInch/72);
#else
    // Correct for Windows compatibility
    int nHeight = - (font->GetPointSize()*ppInch/72);
#endif

    bool ff_underline = font->GetUnderlined();

    ff_face = font->GetFaceName();

    logFont->lfHeight = nHeight;
    logFont->lfWidth = 0;
    logFont->lfEscapement = 0;
    logFont->lfOrientation = 0;
    logFont->lfWeight = ff_weight;
    logFont->lfItalic = ff_italic;
    logFont->lfUnderline = (BYTE)ff_underline;
    logFont->lfStrikeOut = 0;
    logFont->lfCharSet = ANSI_CHARSET;
    logFont->lfOutPrecision = OUT_DEFAULT_PRECIS;
    logFont->lfClipPrecision = CLIP_DEFAULT_PRECIS;
    logFont->lfQuality = PROOF_QUALITY;
    logFont->lfPitchAndFamily = DEFAULT_PITCH | ff_family;
    strcpy(logFont->lfFaceName, ff_face);
}

wxFont wxCreateFontFromLogFont(LOGFONT *logFont) // , bool createNew)
{
  int fontFamily = wxSWISS;
  int fontStyle = wxNORMAL;
  int fontWeight = wxNORMAL;
  int fontPoints = 10;
  bool fontUnderline = FALSE;
  char *fontFace = NULL;

//  int lfFamily = logFont->lfPitchAndFamily & 0xF0;
  int lfFamily = logFont->lfPitchAndFamily;
  if (lfFamily & FIXED_PITCH)
    lfFamily -= FIXED_PITCH;
  if (lfFamily & VARIABLE_PITCH)
    lfFamily -= VARIABLE_PITCH;
  
  switch (lfFamily)
  {
    case FF_ROMAN:
      fontFamily = wxROMAN;
      break;
    case FF_SWISS:
      fontFamily = wxSWISS;
      break;
    case FF_SCRIPT:
      fontFamily = wxSCRIPT;
      break;
    case FF_MODERN:
      fontFamily = wxMODERN;
      break;
    case FF_DECORATIVE:
      fontFamily = wxDECORATIVE;
      break;
    default:
      fontFamily = wxSWISS;
      break;
  }
  switch (logFont->lfWeight)
  {
    case FW_LIGHT:
      fontWeight = wxLIGHT;
      break;
    case FW_NORMAL:
      fontWeight = wxNORMAL;
      break;
    case FW_BOLD:
      fontWeight = wxBOLD;
      break;
    default:
      fontWeight = wxNORMAL;
      break;
  }
  if (logFont->lfItalic)
    fontStyle = wxITALIC;
  else
    fontStyle = wxNORMAL;

  if (logFont->lfUnderline)
    fontUnderline = TRUE;
    
  if  (logFont->lfFaceName)
    fontFace = logFont->lfFaceName;

  HDC dc2 = ::GetDC(NULL);

  if ( logFont->lfHeight < 0 )
	logFont->lfHeight = - logFont->lfHeight;
  fontPoints = abs(72*logFont->lfHeight/GetDeviceCaps(dc2, LOGPIXELSY));
  ::ReleaseDC(NULL, dc2);

//  if ( createNew )
  	return wxFont(fontPoints, fontFamily, fontStyle, fontWeight, fontUnderline, fontFace);
//  else
//	return wxTheFontList->FindOrCreateFont(fontPoints, fontFamily, fontStyle, fontWeight, fontUnderline, fontFace);
}


