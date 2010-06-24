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
    Create( maskedField.GetMask()
          , maskedField.GetFormatCodes()
          , maskedField.GetDefaultValue()
          , maskedField.GetChoices()
          , maskedField.IsAutoSelect()
          , maskedField.GetGroupChar()
          , maskedField.GetDecimalPoint()
          , maskedField.IsParensForNegatives());
}

wxMaskedField::wxMaskedField( const wxString& mask
                 , const wxString& formatCodes
                 , const wxString& defaultValue
                 , const wxArrayString& choices
                 , const bool  autoSelect
                 , const wxChar groupChar, const wxChar decimalPoint
                 , const bool useParensForNegatives)
{
    Create(mask, formatCodes, defaultValue, choices
         , autoSelect, groupChar, decimalPoint, useParensForNegatives);
}

bool wxMaskedField::Create( const wxString& mask
                 , const wxString& formatCodes
                 , const wxString& defaultValue
                 , const wxArrayString& choices
                 , const bool  autoSelect
                 , const wxChar groupChar, const wxChar decimalPoint
                 , const bool useParensForNegatives)
{
    unsigned int it;
    unsigned int indexOfAcc = 0;
    unsigned int indexOfBackAcc = 0;
    unsigned long numberOfOccurences = 0;
    bool res = true;

    m_mask = wxT("");

    for(it = 0; it < mask.Len(); it++)
    {
        if(it == 0 && (mask[it] == '{' || mask[it] == '}'))
        {
            m_mask = wxT("");
            it = mask.Len();
            res = false;
        }
        else if(mask[it] == '{')
        {
            if(indexOfAcc == 0 && indexOfBackAcc == 0 
            && mask[it -1] != '\\' 
            &&( mask[it-1] == 'a' || mask[it-1] == 'A'
             || mask[it-1] == 'N' || mask[it-1] == 'C' 
             || mask[it-1] == '#' || mask[it-1] == '&' 
             || mask[it-1] == 'X' || mask[it-1] == '*'))
            {
                if(it == 1 || m_mask[it -2] != '\\')
                    indexOfAcc = it;
            }
            else
            {
                m_mask = wxT("");
                it = mask.Len();
                res = false;
            }
        }
        else if(mask[it] == '}')
        {
            if(mask[it - 1] != '\\' 
            && (indexOfAcc == 0 || indexOfBackAcc != 0))
            {
                m_mask = wxT("");
                it = mask.Len();
                res = false;
            }
            else
            {
                indexOfBackAcc = it;
                wxString tmp;

                tmp = mask.SubString(indexOfAcc + 1 , indexOfBackAcc -1) ;

                if( tmp.ToULong(&numberOfOccurences))
                {
                    for(unsigned int i = 1; i < numberOfOccurences; i++)
                    {
                        m_mask << mask[indexOfAcc - 1];
                    }   
                    indexOfAcc     = 0;
                    indexOfBackAcc = 0;
                }
                else
                {
                    m_mask = wxT("");
                    it = mask.Len();
                    res = false;
                }
            }
        }
        else if(indexOfAcc == 0)
        {
            m_mask << mask[it];
        }
    }

    if(indexOfAcc != 0 || indexOfAcc != 0)
    {
        m_mask = wxT("");
        res = false;
    }


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

    return res;
}

bool wxMaskedField::IsEmpty(const wxString& string) const
{
    return string.Cmp(GetEmptyMask()) == 0 ;
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

wxString wxMaskedField::ApplyFormatCodes(const wxString& string)
{
    wxString res;
    wxString verification;
    unsigned int it;
    unsigned int itMask;
    bool tmp = false; //FIXME change name

    if(string.Len() > m_mask.Len())
        return string;

    for(it = 0, itMask = 0; itMask < m_mask.Len(); it++, itMask++)
    {
        if(it < string.Len())
        {
            if(IsCharValid(m_mask[itMask], string[it]))
            {
                res << string[it];
            }
            else if(m_mask[itMask] == '\\' && it != m_mask.Len() - 1)
            {
                if(string[it] == m_mask[itMask + 1])
                    res << string[it] ;

                itMask++;
            }
            else
            {
                if(m_formatCodes.Contains(wxT("!")))
                {
                    if(IsLowerCase(string[it]) && (m_mask[itMask] == 'A'
                       || m_mask[itMask] == 'N' || m_mask[itMask] == 'C' 
                       || m_mask[itMask] == 'X' || m_mask[itMask] == '*'))
                    {
                        res << string.SubString(it, it).Upper();
                        tmp = true;
                    }
                }
                else if(m_formatCodes.Contains(wxT("^")))
                {
                    if(IsUpperCase(string[it]) && (m_mask[itMask] == 'a'
                       || m_mask[itMask] == 'N' || m_mask[itMask] == 'C' 
                       || m_mask[itMask] == 'X' || m_mask[itMask] == '*'))
                    {
                        res << string.SubString(it, it).Lower();
                        tmp = true;
                    }
                }
                
                if(m_formatCodes.Contains(wxT("_")) && string[it] == ' ')
                {
                    res << ' ';
                    tmp = true;
                }
            
            } 
            //verification

            if(string.Len() != 0 && res[it] != string[it] && tmp == false)
            {
                res = string;
                itMask = m_mask.Len();
            }
             
            tmp = false;
        }
    }
    
    if(string.Len() > res.Len())
            res = string;

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
            else
                res = false;
    }

    return res;
}


bool wxMaskedField::IsValid(const wxString& string) const
{
    unsigned int it;
    unsigned int itMask;
    bool res = true;

    if(string.Len() > m_mask.Len() 
      ||( string.Len() == 0 && m_mask.Len() !=0)
      ||( string.Len() != 0 && m_mask.Len() ==0))
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
            if(m_formatCodes.Contains(wxT("_")))
            {
                if(string[it] != ' ')
                {
                    res = false;
                }
            }
            else
            {
                res = false;
            }
        }
    }

    if(it != string.Len())
        res = false;
    
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

wxString wxMaskedField::GetChoice(unsigned int index)
{
    wxString res;

    if(index > m_choices.GetCount() || index < 0)
        res = wxEmptyString;
    else
        res = m_choices[index];

    return res;
}

unsigned int wxMaskedField::GetNumberOfChoices()
{
    return m_choices.GetCount();
}

wxString wxMaskedField::GetNextChoices()
{
    wxString res;

    if(m_choices.GetCount() != 0)
    {
        if(m_choiceIndex < m_choices.GetCount() -1)
            m_choiceIndex++;
        else
            m_choiceIndex = 0;

        res = m_choices[m_choiceIndex];
    }
    else
        res = wxEmptyString;

    return res;
}

wxString wxMaskedField::GetCurrentChoices()
{
    wxString res;

    if(m_choices.GetCount() != 0)
    {
        res = m_choices[m_choiceIndex];
    }
    else
        res = wxEmptyString;

    return res;
}

wxString wxMaskedField::GetPreviousChoices()
{
    wxString res;

    if(m_choices.GetCount() != 0)
    {
        if(m_choiceIndex == 0)
            m_choiceIndex = m_choices.GetCount() - 1;
        else
            m_choiceIndex--;

        res = m_choices[m_choiceIndex];
    }
    else
        res = wxEmptyString;

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


bool wxMaskedField::SetDefaultValue(const wxString& defaultValue)
{
    bool res = true;
    if(IsValid(defaultValue))
    {
        m_defaultValue = defaultValue;
    }
    else
    {
        res = false;
    }

    return res;
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
    
    if(!IsValid(string)||( string.Len() == 0 && m_mask.Len() !=0))
    {
        res = string;
    }
    else
    {
        for(it = 0, itMask = 0; itMask < m_mask.Len(); it++, itMask++)
        {
            if(m_mask[itMask] == '\\')
            {
                itMask++;
            }
            else if(string[it] != m_mask[it])
            {
                res << string[itMask];

                
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

wxString wxMaskedField::GetEmptyMask() const
{
    wxString res = wxEmptyString;
    unsigned int it;
    

    for(it = 0; it < m_mask.Len(); it++)
    {

        if(m_mask[it] != 'C' && m_mask[it] != 'A' 
        && m_mask[it] != 'a' && m_mask[it] != 'X' 
        && m_mask[it] != '&' && m_mask[it] != '*' 
        && m_mask[it] != 'N' && m_mask[it] != '#')
        {
            if(m_mask[it] == '\\' && it + 1 != m_mask.Len())
            {
                it++;
            }
            
            res << m_mask[it]; 
        }
        else
        {
            res << ' ';
        }
    }
    return res;
}
