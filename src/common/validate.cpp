/////////////////////////////////////////////////////////////////////////////
// Name:        validate.cpp
// Purpose:     wxValidator
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "validate.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/validate.h"

const wxValidator wxDefaultValidator;

#if !USE_SHARED_LIBRARY
    IMPLEMENT_DYNAMIC_CLASS(wxValidator, wxEvtHandler)
#endif

// VZ: personally, I think TRUE would be more appropriate - these bells are
//     _annoying_
bool wxValidator::ms_isSilent = FALSE;

wxValidator::wxValidator()
{
  m_validatorWindow = (wxWindow *) NULL;
}

wxValidator::~wxValidator()
{
}


