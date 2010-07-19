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


wxMaskedEdit::wxMaskedEdit()
{

}


wxMaskedEdit::wxMaskedEdit(const wxMaskedEdit& maskedEdit)
{

    wxArrayString formatCodes;
    unsigned int it;

    for(it = 0; it < maskedEdit.GetNumberOfFields(); it++)
        formatCodes.Add(maskedEdit.GetFormatCodes(it));

    Create(maskedEdit.GetMask(), formatCodes, maskedEdit.GetDefaultValue());

    for(it = 0; it < GetNumberOfFields(); it++)
    {
       AddChoices(it, maskedEdit.GetChoices(it));

    }
}
    

wxMaskedEdit::wxMaskedEdit( const wxString& mask 
                          , const wxString& formatCode
                          , const wxString& defaultValue)
{
    wxArrayString formatCodes;
    formatCodes.Add(formatCode);
    
    Create(mask, formatCodes ,defaultValue);
}

wxMaskedEdit::wxMaskedEdit( const wxString& mask , const wxArrayString& formatCode
                          , const wxString& defaultValue)
{
    Create(mask, formatCode, defaultValue);
}

wxMaskedEdit::~wxMaskedEdit()
{
    m_mask.Clear();
}

void wxMaskedEdit::Create( const wxString& mask 
                         , const wxArrayString& formatCode
                         , const wxString& defaultValue)
{
    wxString tmp;

    if(mask.Find('|') == wxNOT_FOUND)
    {
        if(formatCode.IsEmpty())
            m_mask.Add(new wxMaskedField(mask, wxT("F"), defaultValue)); 
        else
            m_mask.Add(new wxMaskedField(mask, formatCode[0], defaultValue));
        
        m_maskValue = mask;
        m_isNumber = m_mask[0]->IsNumber();
    }
    else
    {
        unsigned int it;
        unsigned int numberOfFields = 0;
        
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
                else if(formatCode.GetCount() == 1)
                    m_mask.Add(new wxMaskedField(tmp, formatCode[0]));
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

        
        SetDefaultValue(defaultValue);
        m_maskValue = GetMask();
        m_isNumber = false;
    }


    m_emptyBg   = wxColour(255,255,255);
    m_invalidBg = wxColour(255,255,255);
    m_validBg   = wxColour(255,255,255);
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
        
        formatTmp = m_mask[fieldIndex]->ApplyFormatCodes(tmp);

        while(!m_mask[fieldIndex]->IsValid(formatTmp) 
            && tmp.Len() != 0)
        {
            tmp.RemoveLast();
            formatTmp = m_mask[fieldIndex]->ApplyFormatCodes(tmp);
        }

        res << formatTmp;
        alreadyUsed << tmp;
        fieldIndex++;
    } 

    if(tmp.Cmp(wxT("")) == 0 
      || alreadyUsed.Cmp(string) != 0)
        res = string;


    return res;
}

wxString wxMaskedEdit::GetPlainValue(const wxString& string)
{
    wxString res;
    wxString tmp;
    unsigned int it;
    unsigned int itMask;
    unsigned int fieldIndex = 0;
    wxString input;


    if(m_mask.GetCount() == 1)
    {
        res = m_mask[0]->GetPlainValue(string);
    }
    else
    {
        for(it = 0, itMask = 0; it < string.Len(); it++, itMask++)
        {
            input << string[it];
         
            if(m_maskValue[itMask] == '|')
            {
                input.RemoveLast();
                res << m_mask[fieldIndex]->GetPlainValue(input);
                fieldIndex++;
                input.Empty();
                it--;
            }
        }
    }

    res << m_mask[fieldIndex]->GetPlainValue(input);

    return res;
}

void wxMaskedEdit::ClearValue()
{
    m_mask.Clear();
    m_maskValue.Clear();
}
    
bool wxMaskedEdit::IsValid(const wxString& string) const
{
    wxString tmp;
    unsigned int it = 0;
    unsigned int itMask = 0;
    unsigned int fieldNumber = 0;
    unsigned int numberOfSlash = 0;
    unsigned int numberOfPipe = 0;
    bool res = true;
    
    tmp = GetMask();
    numberOfSlash = tmp.Replace('\\', ' ');
    numberOfPipe  = tmp.Replace('|', ' ');

    if(string.Len() > m_maskValue.Len() - numberOfSlash - numberOfPipe
       && m_mask[0]->GetFormatCodes().Find('-') == wxNOT_FOUND)
        res =  false;
    
    tmp.Empty();

    for(itMask = 0, it = 0; itMask < m_maskValue.Len() && res && it < string.Len() ; it++, itMask++)
    {
        if(m_maskValue[itMask] != '|')
        {
            if(m_maskValue[itMask] == '\\')
                itMask++;

            tmp << string[it];
        }
        else
        {
            it--;
            
            if(!m_mask[fieldNumber]->IsValid(tmp))
                res = false;
            
            fieldNumber++;       
            tmp.Clear();
        }
            
    }

    tmp << string.Mid(it);
    res = res && m_mask[fieldNumber]->IsValid(tmp);

    return res;
}
 
bool wxMaskedEdit::IsEmpty(const wxString& string) 
{
    return string.Cmp(GetEmptyMask()) == 0;   
}

bool wxMaskedEdit::SetMask( const wxString& mask)
{
    bool res = true;

    if(m_mask.GetCount() <= 1 && mask.Find(wxT("|")) == wxNOT_FOUND)
        res = m_mask[0]->SetMask(mask);
    else
    {
        res = false;

    }
    return res;
}

wxString wxMaskedEdit::GetMask() const
{
    unsigned int it;
    wxString res;

    for(it = 0; it < m_mask.GetCount(); it++)
    {
        res << m_mask[it]->GetMask();
        res << wxT("|");
    }
    res.RemoveLast();

    return res;
}

wxString wxMaskedEdit::GetFormatCode() const
{
    wxString res = wxEmptyString;
    if(m_mask.GetCount() <= 1)
        res = m_mask[0]->GetFormatCodes();
    
    return res;
}

wxString wxMaskedEdit::GetDefaultValue() const
{
    unsigned int it;
    wxString res;

    for(it = 0; it < m_mask.GetCount(); it++)
    {
        res << m_mask[it]->GetDefaultValue();
    }

    return res;
}

bool wxMaskedEdit::SetDefaultValue(const wxString& defaultValue)
{
    unsigned int it;
    unsigned int fieldIndex = 0;
    wxString tmp;
    wxString formatTmp;
    wxString alreadyUsed = wxEmptyString;
    bool res = true;

    if(!IsValid(defaultValue))
    {
        res = false;
    }
    else
    {
        if(m_mask.GetCount() == 1)
        {
            res = m_mask[0]->SetDefaultValue(defaultValue);
        }
        else
        {
            for(it = 0; it < defaultValue.Len() 
                && fieldIndex < m_mask.GetCount() && res;it++)
            {
                tmp = defaultValue;
                
                if(alreadyUsed != wxEmptyString)
                    tmp.Replace(alreadyUsed, wxEmptyString, false);
                    
                while(!m_mask[fieldIndex]->SetDefaultValue(tmp)
                    && tmp.Len() != 0)
                {
                    tmp.RemoveLast();
                }

                if(tmp.Len() == 0)
                {
                    res = false;
                }
                alreadyUsed << tmp;
                fieldIndex++;
            } 
        }
    }
    return res;
}

wxArrayString wxMaskedEdit::GetChoices() const
{
    if(m_mask.GetCount() == 1)
        return m_mask[0]->GetChoices();
    else
        return wxArrayString();
}

bool wxMaskedEdit::AddChoice(const wxString& choice)
{
    bool res = true;
    res = m_mask[0]->AddChoice(choice);

    return res;
}

bool wxMaskedEdit::AddChoices(const wxArrayString& choices)  
{
    bool res = true;
    res = m_mask[0]->AddChoices(choices);

    return res;
}

wxString wxMaskedEdit::GetNextChoices() const
{
    wxString res;

    res = m_mask[0]->GetNextChoices();

    return res;

}
    
wxString wxMaskedEdit::GetCurrentChoices() const
{
    wxString res;

    res = m_mask[0]->GetCurrentChoices();

    return res;
}

wxString wxMaskedEdit::GetPreviousChoices() const
{
    wxString res;

    res = m_mask[0]->GetPreviousChoices();

    return res;

}
wxString wxMaskedEdit::GetChoice(unsigned int fieldIndex, unsigned int index)
{
    wxString res;

    if(fieldIndex > m_mask.GetCount())
        res = wxEmptyString;
    else
        res = m_mask[fieldIndex]->GetChoice(index);

    return res;
}

wxString wxMaskedEdit::GetNextChoices(unsigned int fieldIndex) const
{
    wxString res;

    if(fieldIndex >= m_mask.GetCount())
        res = wxEmptyString;
    else
        res = m_mask[fieldIndex]->GetNextChoices();

    return res;

}
    
wxString wxMaskedEdit::GetCurrentChoices(unsigned int fieldIndex) const
{
    wxString res;

    if(fieldIndex >= m_mask.GetCount())
        res = wxEmptyString;
    else
        res = m_mask[fieldIndex]->GetCurrentChoices();

    return res;
}

wxString wxMaskedEdit::GetPreviousChoices(unsigned int fieldIndex) const
{
    wxString res;

    if(fieldIndex >= m_mask.GetCount())
        res = wxEmptyString;
    else
        res = m_mask[fieldIndex]->GetPreviousChoices();

    return res;

}

unsigned int wxMaskedEdit::NumberOfChoices()
{
    unsigned int res = 0;
    unsigned int it;
    for(it = 0; it < m_mask.GetCount(); it++)
    {
        res += m_mask[it]->GetNumberOfChoices();
    }

    return res;

}

unsigned int wxMaskedEdit::GetNumberOfFields() const
{
    return m_mask.GetCount();
}

void wxMaskedEdit::SetEmptyBackgroundColour(const wxColour& colour)
{
    m_emptyBg = colour;
}
     
void wxMaskedEdit::SetInvalidBackgroundColour(const wxColour& colour)       
{
    m_invalidBg = colour;
}

void wxMaskedEdit::SetValidBackgroundColour(const wxColour& colour)
{
    m_validBg = colour;
}

wxColour wxMaskedEdit::GetEmptyBackgroundColour() const
{
    return m_emptyBg;
}

wxColour wxMaskedEdit::GetInvalidBackgroundColour() const       
{
    return m_invalidBg;
}

wxColour wxMaskedEdit::GetValidBackgroundColour() const
{
    return m_validBg;
}

bool wxMaskedEdit::SetMask(unsigned int fieldIndex, wxString& mask)
{
    bool res = true;

    if(fieldIndex >= m_mask.GetCount())
        res = false;
    else
        m_mask[fieldIndex]->SetMask(mask);

    return res;
}



wxString wxMaskedEdit::GetMask(unsigned int fieldIndex) const
{
    if(fieldIndex >= m_mask.GetCount())
        return wxEmptyString;

    return m_mask[fieldIndex]->GetMask();

}
wxString wxMaskedEdit::GetFormatCodes(unsigned int fieldIndex) const
{
    if(fieldIndex >= m_mask.GetCount())
        return wxEmptyString;
    
    return m_mask[fieldIndex]->GetFormatCodes();
}

wxArrayString wxMaskedEdit::GetChoices(unsigned int fieldIndex) const
{
    if(fieldIndex > m_mask.GetCount())
    {
        return wxArrayString();
    }
    else
        return m_mask[fieldIndex]->GetChoices();
}
 
bool wxMaskedEdit::AddChoice(unsigned int fieldIndex, const wxString& choice)
{
    bool res = true;

    if(fieldIndex < m_mask.GetCount())
    {
        res = m_mask[fieldIndex]->AddChoice(choice);
    }
    else
        res = false;

    return res;

}

bool wxMaskedEdit::AddChoices(unsigned int fieldIndex, const wxArrayString& choices)
{
    bool res = true;
    if(fieldIndex < m_mask.GetCount())
        res = m_mask[fieldIndex]->AddChoices(choices);
    else
        res = false;

    return res;
} 

wxString wxMaskedEdit::GetEmptyMask()const
{
    unsigned int it;
    wxString res = wxEmptyString;

    for(it = 0; it < m_mask.GetCount(); it++)
    {
        res << m_mask[it]->GetEmptyMask();
    }
    return res;

}

unsigned int wxMaskedEdit::GetFieldIndex(unsigned int position)
{
    unsigned int it;
    unsigned int res = 0;

    for(it = 0; it < m_mask.GetCount(); it++)
    {
        if(position >= GetMinFieldPosition(it) 
        && position <= GetMaxFieldPosition(it))
        {
            res = it;
            it = m_mask.GetCount();
        }
    }

    return res;

}

unsigned int wxMaskedEdit::GetMinFieldPosition(unsigned int fieldIndex)
{
    unsigned int res = 0;
    unsigned int currentField = 0;

    if(fieldIndex >= m_mask.GetCount())
    {
        return 0;
    }

    while(fieldIndex > currentField && currentField < m_mask.GetCount())
    {
        res += m_mask[currentField]->GetMask().Len();
        currentField++;

    }

    return res;


}

unsigned int wxMaskedEdit::GetMaxFieldPosition(unsigned int fieldIndex)
{
    unsigned int res = 0;

    res = GetMinFieldPosition(fieldIndex);
    
    if(fieldIndex < m_mask.GetCount())
    {
        res += m_mask[fieldIndex]->GetMask().Len() - 1;
    }
    
    return res;

}

bool wxMaskedEdit::IsNumber()
{
    return m_mask.GetCount() == 1 && m_mask[0]->IsNumber();
}

wxString wxMaskedEdit::GetDecimalPoint()
{
    wxString res;
    if(m_mask.GetCount() != 1)
    {
        res = wxEmptyString;
    }
    else
    {
        res = m_mask[0]->GetDecimalPoint();
    }

    return res;
}

void wxMaskedEdit::SetFillChar(wxChar newFillChar)
{
    unsigned int it;

    for(it = 0; it < m_mask.GetCount(); it++)
    {
        m_mask[it]->SetFillChar(newFillChar);
    }
}

wxChar wxMaskedEdit::GetFillChar()
{
    return m_mask[0]->GetFillChar();
}
