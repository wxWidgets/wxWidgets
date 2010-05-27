/////////////////////////////////////////////////////////////////////////////
// Name:        commun/maskedfield.cpp
// Purpose:
// Author:      Julien Weinzorn
// Id:          $Id: ????????????????????$
// Copyright:   (c) 2010 Julien Weinzorn
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#endif

#include "wx/maskededit.h"
#include "wx/maskedfield.h"




wxMaskedEdit::wxMaskedEdit()
{

}
    
wxMaskedEdit::wxMaskedEdit(const wxMaskedEdit& maskedEdit)
{
    m_mask.Add(new wxMaskedField( maskedEdit.GetMask(), maskedEdit.GetFormatCode()
                                , maskedEdit.GetDefaultValue()));
}
    

wxMaskedEdit::wxMaskedEdit( wxString& mask 
                          , const wxString& formatCode
                          , const wxString& defaultValue)
{
    m_mask.Add(new wxMaskedField(mask, formatCode, defaultValue));
}

#if 0
wxMaskedEdit::wxMaskedEdit( const wxString& mask , const wxArrayString& formatCode
                          , const wxString& defaultValue)
{
}

#endif