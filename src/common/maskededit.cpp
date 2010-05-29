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

    m_maskValue = maskedEdit.GetMask();
    m_cursorField = 0;
    m_cursorInsideField = 0;

}
    

wxMaskedEdit::wxMaskedEdit( wxString& mask 
                          , const wxString& formatCode
                          , const wxString& defaultValue)
{
    m_mask.Add(new wxMaskedField(mask, formatCode, defaultValue));
    m_maskValue = mask;
    m_cursorField = 0;
    m_cursorInsideField = 0;
}

#if 0
wxMaskedEdit::wxMaskedEdit( const wxString& mask , const wxArrayString& formatCode
                          , const wxString& defaultValue)
{
}

#endif

wxString wxMaskedEdit::GetPlainValue(wxString string)
{
    wxString res;
    wxString tmp;
    unsigned int it;
    unsigned int itMask;
    unsigned int fieldNumber = 0;

    for(itMask = 0, it = 0; itMask < m_maskValue.Len(); it++, itMask++)
    {
        if(m_maskValue != '|')
        {
            tmp << string[it];
        }
        else
        {
            it--;
            tmp = m_mask[fieldNumber].GetPlainValue(tmp);
            
            if(tmp == wxEmptyString)
                return wxEmptyString;
            
            res << tmp;
            tmp.Clear();
        }
            
        
    }

    tmp = m_mask[fieldNumber].GetPlainValue(tmp);
    
    if(tmp == wxEmptyString)
        return tmp;
            
    res << tmp;
    tmp.Clear();

    return res;
}

void wxMaskedEdit::ClearValue()
{
    m_mask.Clear();
    m_maskValue.Clear();
    m_cursorField = 0;
    m_cursorInsideField = 0;

}
    
bool wxMaskedEdit::IsValid(wxString string) const
{
    wxString tmp;
    unsigned int it;
    unsigned int itMask;
    unsigned int fieldNumber = 0;

    for(itMask = 0, it = 0; itMask < m_maskValue.Len(); it++, itMask++)
    {
        if(m_maskValue != '|')
        {
            tmp << string[it];
        }
        else
        {
            it--;
            
            
            if(!m_mask[fieldNumber].IsValid(tmp))
                return false;
           
            tmp.Clear();
        }
            
        
    }
            
    if(!m_mask[fieldNumber].IsValid(tmp))
        return false;

    tmp.Clear();

    return true;
}
 
#if 0
//FIXME  what is an empty string for the mask
bool wxMaskedEdit::IsEmpty(wxString string) const
{
    bool res;
    unsigned int it;



   
}
#endif
