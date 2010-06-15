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
#define wxDEFAULT_VALUE   (wxT(" "))

class WXDLLIMPEXP_BASE wxMaskedField
{
public:

    wxMaskedField();
 
    wxMaskedField(const wxMaskedField& maskedField);
    
    wxMaskedField( const wxString& mask        
                 , const wxString& formatCodes  = wxDEFAULT_FORMAT_CODES
                 , const wxString& defaultValue = wxDEFAULT_VALUE
                 , const wxArrayString& choices = wxArrayString()
                 , const bool  autoSelect = false
                 , const wxChar groupChar = ' ',const wxChar decimalPoint = '.'
                 , const bool useParensForNegatives = false);
   
    
    void Create(   const wxString& mask        
                 , const wxString& formatCodes  = wxDEFAULT_FORMAT_CODES
                 , const wxString& defaultValue = wxDEFAULT_VALUE
                 , const wxArrayString& choices = wxArrayString()
                 , const bool  autoSelect = false
                 , const wxChar groupChar = ' ',const wxChar decimalPoint = '.'
                 , const bool useParensForNegatives = false);


    bool IsEmpty(const wxString& string) const;

    wxString ApplyFormatCodes(const wxString& string);
    
    bool IsValid(const wxString& string) const;

    bool AddChoice(const wxString& choice);

    bool AddChoices(const wxArrayString& choices); 

    wxString GetDefaultValue() const; 

    void SetMask(const wxString& mask);

    wxString GetMask() const;

    wxString GetFormatCodes() const;

    wxUniChar GetGroupChar() const;

    wxUniChar GetDecimalPoint() const;

    wxArrayString GetChoices() const;

    bool IsParensForNegatives() const;

    bool IsAutoSelect() const;

    wxString GetPlainValue(const wxString& string);
    
    bool IsCharValid(const wxChar maskChar, const wxChar character) const;

private:
    // The mask in the field see wxMaskedEdit
    wxString m_mask;
    
    // The format codes, see wxMaskedEdit
    wxString m_formatCodes;

    // The default value
    wxString m_defaultValue;
    
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
