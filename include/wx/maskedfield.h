/////////////////////////////////////////////////////////////////////////////
// Name:        wx/maskedfield.h
// Purpose:
// Author:      Julien Weinzorn
// Id:          $Id: ????????????????????$
// Copyright:   (c) 2010 Julien Weinzorn
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MASKED_FIELD_H_
#define _WX_MASKED_FIELD_H_

#include "wx/defs.h"
#include <stdlib.h>

#define wxDEFAULT_FORMAT_CODES (wxT("F"))

class wxMaskedField
{
public:

    wxMaskedField();
 
    wxMaskedField(const wxMaskedField& maskedField);
    
    wxMaskedField( wxString& mask        
                 , const wxString& formatCodes  = wxDEFAULT_FORMAT_CODES
                 , const wxArrayString& choices = NULL
                 , const bool  autoSelect = false
                 , const wxChar groupChar = ' ',const wxChar decimalPoint = '.'
                 , const bool useParensForNegatives = false);
    

    bool IsEmpty(const wxString& string) const;

    bool IsCharValid(const wxChar maskChar, const wxChar character) const;
    bool IsValid(const wxString& string) const;

    wxString GetMask() const;

    wxString GetFormatCodes() const;

    wxUniChar GetGroupChar() const;

    wxUniChar GetDecimalPoint() const;

    bool IsParensForNegatives() const;

    bool IsAutoSelect() const;
    
private:
    // The mask in the field see wxMaskedEdit
    wxString m_mask;
    
    // The format codes, see wxMaskedEdit
    wxString m_formatCodes;
    
    // The default choices
    wxArrayString m_choices;
    bool m_autoSelect;
    
    // The default groupchar if the field is a integer
    wxUniChar m_groupChar;
    
    // The decimal point
    wxUniChar m_decimalPoint;
    
    // Use (###) in place of -###
    bool m_useParensForNegatives;

    bool IsNumber(const wxChar character) const;
    bool IsLowerCase(const wxChar character) const;    
    bool IsUpperCase(const wxChar character) const;
    bool IsPunctuation(const wxChar character) const;
};
#endif