/////////////////////////////////////////////////////////////////////////////
// Name:        sergdi.h
// Purpose:     Serialization: GDI classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     July 1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __SERGDI_H__
#define __SERGDI_H__

#ifdef __GNUG__
#pragma interface
#endif

#include <wx/serbase.h>
#include "sercore.h"

DECLARE_SERIAL_CLASS(wxBitmap, wxObject)
DECLARE_SERIAL_CLASS(wxGDIObject, wxObject)
DECLARE_SERIAL_CLASS(wxRegion, wxGDIObject)
DECLARE_SERIAL_CLASS(wxColour, wxObject)
DECLARE_SERIAL_CLASS(wxFont, wxGDIObject)
DECLARE_SERIAL_CLASS(wxPen, wxGDIObject)
DECLARE_SERIAL_CLASS(wxBrush, wxGDIObject)
DECLARE_SERIAL_CLASS(wxImageList, wxObject)

DECLARE_ALIAS_SERIAL_CLASS(wxPenList, wxList)
DECLARE_ALIAS_SERIAL_CLASS(wxBrushList, wxList)
DECLARE_ALIAS_SERIAL_CLASS(wxFontList, wxList)
DECLARE_ALIAS_SERIAL_CLASS(wxColourDatabase, wxList)
DECLARE_ALIAS_SERIAL_CLASS(wxBitmapList, wxList)

#endif
