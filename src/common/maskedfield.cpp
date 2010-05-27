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

#include "wx/maskedfield.h"

wxMaskedField::wxMaskedField()
{
}

wxMaskedField::wxMaskedField(const wxMaskedField& maskedField)
{
    m_mask         = maskedField.GetMask();
    m_formatCodes  = maskedField.GetFormatCodes();
    m_autoSelect   = maskedField.IsAutoSelect();
    m_decimalPoint = maskedField.GetDecimalPoint();
    m_useParensForNegatives = maskedField.IsParensForNegatives();
}

wxMaskedField::wxMaskedField( wxString& mask
                 , const wxString& formatCodes
                 , const wxArrayString& choices
                 , const bool  autoSelect
                 , const wxChar groupChar, const wxChar decimalPoint
                 , const bool useParensForNegatives)
{
    unsigned int it;

    m_mask         = mask;
    m_formatCodes  = formatCodes;
    m_autoSelect   = autoSelect;
    m_groupChar    = groupChar;
    m_decimalPoint = decimalPoint;
    m_useParensForNegatives = useParensForNegatives;
    
    
    
    for(it = 0; it < choices.size(); it++)
    {
        if(IsValid(choices[it]))
        {
            m_choices.Add(choices[it]);
        }
    }
}

// FIXME change the default value
bool wxMaskedField::IsEmpty(const wxString& string) const
{
    return string.Cmp(wxT(" ")) == 0;
}

bool wxMaskedField::IsNumber(const wxChar character) const
{
    return character >= '0' && character <= '9';
}

bool wxMaskedField::IsLowerCase(const wxChar character)const
{
    return character >= 'a' && character <= 'z';
}

bool wxMaskedField::IsUpperCase(const wxChar character) const
{
    return character >= 'A' && character <= 'Z';
}

bool wxMaskedField::IsPunctuation(const wxChar character) const
{
    return character == ',' || character == '?' || character == ';' 
        || character == '.' || character == ':' || character == '!';
}


bool wxMaskedField::IsCharValid(const wxChar maskChar, const wxChar character) const
{
    bool res;
    
    switch(maskChar)
    {
        case '#':
            res = IsNumber(character);
        break;
        case 'N':
            res = IsNumber(character) || IsLowerCase(character) || IsUpperCase(character);
        break;
        case 'A':
            res = IsUpperCase(character);
        break;
        case 'a':
            res = IsLowerCase(character);
        break;
        case 'C':
            res = IsLowerCase(character) || IsUpperCase(character);
        break;
        case 'X':
            res = IsNumber(character) || IsLowerCase(character) || IsUpperCase(character)
                  || IsPunctuation(character);
        break;
        case '&':
            res = IsPunctuation(character);
        break;
        case '*':
            res = true;
        break;
        default:
            res = false;
    }

    return res;
}


bool wxMaskedField::IsValid(const wxString& string) const
{
    unsigned int it;
    bool res = true;

    if(string.Len() > m_mask.Len())
    {
        res = false;
    }
    
    for(it = 0; it < m_mask.Len() && res; it++)
    {
        if(!IsCharValid(m_mask[it], string[it]))
        {
            res = false;
        }
    }
    
    return res;
    
}

wxString wxMaskedField::GetMask() const
{
    return m_mask;
}

wxString wxMaskedField::GetFormatCodes() const
{
    return m_formatCodes;
}

wxUniChar wxMaskedField::GetGroupChar() const
{
    return m_groupChar;
}

wxUniChar wxMaskedField::GetDecimalPoint() const
{
    return m_decimalPoint;
}

bool wxMaskedField::IsParensForNegatives() const
{
    return m_useParensForNegatives;
}

bool wxMaskedField::IsAutoSelect() const
{
    return m_autoSelect;
}



