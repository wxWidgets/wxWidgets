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

// FIXME watch how to copy constructor
#if 0
wxMaskedEdit::wxMaskedEdit(const wxMaskedEdit& maskedEdit)
{
    m_mask.Add(new wxMaskedField( maskedEdit.GetMask(), maskedEdit.GetFormatCode()
                                , maskedEdit.GetDefaultValue()));

    m_maskValue = maskedEdit.GetMask();
    m_cursorField = 0;
    m_cursorInsideField = 0;

}
#endif
    

wxMaskedEdit::wxMaskedEdit( const wxString& mask 
                          , const wxString& formatCode
                          , const wxString& defaultValue)
{
    m_mask.Add(new wxMaskedField(mask, formatCode, defaultValue));
    m_maskValue = mask;
    m_cursorField = 0;
    m_cursorInsideField = 0;
}

wxMaskedEdit::wxMaskedEdit( const wxString& mask , const wxArrayString& formatCode
                          , const wxString& defaultValue)
{
    wxString tmp;
    unsigned int it;
    unsigned int numberOfFields = 0;
    
    m_maskValue = mask;
    m_cursorField = 0;
    m_cursorInsideField = 0;


    for(it = 0; it < mask.Len(); it++)
    {
        if(mask[it] != '|')
        {
            tmp << mask[it];
        }
        else
        {
            if(formatCode.GetCount() > numberOfFields)
            {
                m_mask.Add(new wxMaskedField(tmp, formatCode[numberOfFields]));
                numberOfFields++;
            }
            else
                m_mask.Add(new wxMaskedField(tmp));
            
            tmp.Clear();
        }


    }
    
    if(formatCode.GetCount() > numberOfFields)
    {
        m_mask.Add(new wxMaskedField(tmp, formatCode[numberOfFields]));
        numberOfFields++;
    }
    else
        m_mask.Add(new wxMaskedField(tmp));

    if(IsValid(defaultValue))
    {
       //FIXME ajout d une valeur par defaut 
    }

}

wxString wxMaskedEdit::ApplyFormatCodes(const wxString& string)
{
    unsigned int it;
    unsigned int fieldIndex = 0;
    wxString res;
    wxString tmp;
    wxString formatTmp;
    wxString alreadyUsed = wxEmptyString; 
    
    for(it = 0; it < string.Len() && fieldIndex < m_mask.GetCount();it++)
    {
        tmp = string;
        
        if(alreadyUsed != wxEmptyString)
            tmp.Replace(alreadyUsed, wxEmptyString);
        
        formatTmp = m_mask[fieldIndex].ApplyFormatCodes(tmp);
  

        while(!m_mask[fieldIndex].IsValid(formatTmp))
        {
            tmp.RemoveLast();
            formatTmp = m_mask[fieldIndex].ApplyFormatCodes(tmp);
        }

        res << formatTmp;
        alreadyUsed << tmp;
        fieldIndex++;
    } 

    return res;
}

// FIXME How to get plain value with mutiple fields
wxString wxMaskedEdit::GetPlainValue(wxString string)
{
    wxString res;
    wxString tmp;
    unsigned int it;
    unsigned int itMask;
    unsigned int fieldIndex = 0;
    wxString input;


    if(m_mask.GetCount() == 1)
    {
        res = m_mask[0].GetPlainValue(string);
    }
    else
    {
        for(it = 0, itMask = 0; it < string.Len(); it++, itMask++)
        {
            input << string[it];
         
            if(m_maskValue[itMask] == '|')
            {
                input.RemoveLast();
                res << m_mask[fieldIndex].GetPlainValue(input);
                fieldIndex++;
                input.Empty();
                it--;
            }
        }
    }

    res << m_mask[fieldIndex].GetPlainValue(input);

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

    if(string.Len() > m_maskValue.Len() 
       || (string.Len() >= m_maskValue.Len() && m_mask.GetCount() > 1))
        return false;

    for(itMask = 0, it = 0; itMask < m_maskValue.Len(); it++, itMask++)
    {
        if(m_maskValue[itMask] != '|' && string.Len() >= itMask)
        {
            if(m_maskValue[itMask] == '\\')
                itMask++;
            tmp << string[it];
        }
        else
        {
            it--;
            
            if(!m_mask[fieldNumber].IsValid(tmp))
                return false;
            fieldNumber++;       
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

bool wxMaskedEdit::SetMask( const wxString& mask)
{
    bool res = true;

    if(m_mask.GetCount() == 1 && !mask.Contains(wxT("|")))
        m_mask[0].SetMask(mask);
    else
        res = false;

    return res;
}

wxString wxMaskedEdit::GetMask() const
{
    return m_maskValue;

}

wxString wxMaskedEdit::GetFormatCode() const
{
    if(m_mask.GetCount() == 1)
        return m_mask[0].GetFormatCodes();
    else
        return wxEmptyString;
}

wxString wxMaskedEdit::GetDefaultValue() const
{
    unsigned int it;
    wxString res;

    for(it = 0; it < m_mask.GetCount(); it++)
    {
        res << m_mask[it].GetDefaultValue();
    }

    return res;
}


wxArrayString wxMaskedEdit::GetChoices() const
{
    if(m_mask.GetCount() == 1)
        return m_mask[0].GetChoices();
    else
        return wxArrayString();
}

bool wxMaskedEdit::AddChoice(wxString& choice)
{
    bool res = true;
    if(m_mask.GetCount() == 1)
         res = m_mask[0].AddChoice(choice);
    else
        res = false;

    return res;
}

bool wxMaskedEdit::AddChoices(const wxArrayString& choices)  
{
    bool res = true;
    if(m_mask.GetCount() == 1)
         res = m_mask[0].AddChoices(choices);
    else
        res = false;

    return res;
}
int wxMaskedEdit::GetNumberOfFields() const
{
    return m_mask.GetCount();
}

bool wxMaskedEdit::SetMask(unsigned int fieldIndex, wxString& mask)
{
    bool res = true;

    if(fieldIndex >= m_mask.GetCount())
        res = false;
    else
        m_mask[fieldIndex].SetMask(mask);

    return res;
}



wxString wxMaskedEdit::GetMask(unsigned int fieldIndex) const
{
    if(fieldIndex >= m_mask.GetCount())
        return wxEmptyString;

    return m_mask[fieldIndex].GetMask();

}
wxString wxMaskedEdit::GetFormatCodes(unsigned int fieldIndex) const
{
    if(fieldIndex >= m_mask.GetCount())
        return wxEmptyString;
    
    return m_mask[fieldIndex].GetFormatCodes();
}

wxArrayString wxMaskedEdit::GetChoices(unsigned int fieldIndex) const
{
    if(fieldIndex >= m_mask.GetCount())
        return wxArrayString();
    else
        return m_mask[fieldIndex].GetChoices();
}
 
bool wxMaskedEdit::AddChoice(unsigned int fieldIndex, const wxString& choice)
{
    bool res = true;

    if(fieldIndex < m_mask.GetCount())
    {
        res = m_mask[fieldIndex].AddChoice(choice);
    }
    else
        res = false;

    return res;

}

bool wxMaskedEdit::AddChoices(unsigned int fieldIndex, const wxArrayString& choices)
{
    bool res = true;
    if(fieldIndex >= m_mask.GetCount())
        res = m_mask[fieldIndex].AddChoices(choices);
    else
        res = false;

    return res;
} 



