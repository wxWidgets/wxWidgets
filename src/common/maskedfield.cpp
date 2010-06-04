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

wxMaskedField::wxMaskedField( const wxString& mask
                 , const wxString& formatCodes
                 , const wxString& defaultValue
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
    
    if(IsValid(defaultValue))
    {
        m_defaultValue = defaultValue;
    }
    else
    {
        m_defaultValue = wxT(" ");
    }
    
    for(it = 0; it < choices.size(); it++)
    {
        if(IsValid(choices[it]))
        {
            m_choices.Add(choices[it]);
        }
    }
}

bool wxMaskedField::IsEmpty(const wxString& string) const
{
    return string.Cmp(m_defaultValue) == 0;
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

// FIXME add space
wxString wxMaskedField::ApplyFormatCodes(const wxString& string)
{
    wxString res;
    unsigned int it;

    if(m_formatCodes.Contains(wxT("_")))
    {
        for(it = 0; it < m_mask.Len(); it++)
        {
            if(!IsCharValid(m_mask[it], string[it]))
                res << wxT("");
            else
                res << string[it];
        }
    }


    if(m_formatCodes.Contains(wxT("!")))
    {
        res =  string.Upper();
    }
    else if(m_formatCodes.Contains(wxT("^")))
    {
        res =  string.Lower();
    }
    return res;
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
            if(maskChar == character)
                res = true;
            res = false;
    }

    return res;
}


bool wxMaskedField::IsValid(const wxString& string) const
{
    unsigned int it;
    unsigned int itMask;
    bool res = true;

    if(string.Len() > m_mask.Len())
    {
        res = false;
    }
    
    for(it = 0, itMask = 0; itMask < m_mask.Len() && res && it < string.Len(); it++, itMask++)
    {
        if(m_mask[itMask] == '\\' && it != m_mask.Len() - 1)
        {
            if(string[it] != m_mask[itMask + 1])
                res = false;

            itMask++;
        }
        else if(!IsCharValid(m_mask[itMask], string[it]))
        {
            if(m_formatCodes.Contains('_'))
            {
                it--;
            }
            else
            {
                res = false;
            }
        }
    }
    
    return res;
    
}

bool wxMaskedField::AddChoice(const wxString& choice)
{
    bool res = true;
    if(IsValid(choice))
        m_choices.Add(choice);
    else
        res = false;

    return res;
}

bool wxMaskedField::AddChoices(const wxArrayString& choices)
{
    bool res = true;
    unsigned int it;

    for(it = 0; it < choices.size() && res; it++)
    {
        if(!IsValid(choices[it]))
            res = false;
    }

    for(it = 0; it < choices.size() && res; it++)
    {
        m_choices.Add(choices[it]);
    }

    return res;
}

void wxMaskedField::SetMask(const wxString& mask)
{
    m_mask = mask;
    unsigned int it;

    if(!IsValid(m_defaultValue))
        m_defaultValue = wxDEFAULT_VALUE;

    for(it = 0; it < m_choices.Count(); it++)
    {
        if(!IsValid(m_choices[it]))
        {
            m_choices.RemoveAt(it);
            it --;
        }
    }

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

wxString wxMaskedField::GetDefaultValue() const
{
    return m_defaultValue;
}

wxArrayString wxMaskedField::GetChoices() const
{
    return m_choices;    
}

bool wxMaskedField::IsParensForNegatives() const
{
    return m_useParensForNegatives;
}

bool wxMaskedField::IsAutoSelect() const
{
    return m_autoSelect;
}



wxString wxMaskedField::GetPlainValue(const wxString& string)
{
    unsigned int it;
    unsigned int itMask;
    wxString res;
    wxString formatString = ApplyFormatCodes(string);
    
    if(!IsValid(formatString))
    {
        res = wxEmptyString;
    }
    else
    {
        for(it = 0, itMask = 0; itMask < m_mask.Len(); it++, itMask++)
        {
            if(string[it] != m_mask[it])
            {
                res << string[itMask];

                if(m_mask[itMask] == '\\')
                    itMask++;
            }
            else
            {
                if(m_mask[it] == 'C' || m_mask[it] == 'A' || m_mask[it] == 'a' ||
                   m_mask[it] == 'X' || m_mask[it] == '&' || m_mask[it] == '*' ||
                   m_mask[it] == 'N')
                {
                    res << string[itMask];
                }
            }
        }
    }

    return res;
}
