/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/comboboxcmn.cpp
// Purpose:     wxComboBox
// Author:      Julien Weinzorn
// Modified by:
// Created:     Jui-11-2010
// RCS-ID:      $Id: combocmn.cpp 64863 2010-07-10 12:38:40Z JWE $
// Copyright:   (c) 2005 Julien Weinzorn
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// ----------------------------------------------------------------------------
// include the platform-dependent header defining the real class
// ----------------------------------------------------------------------------

#if defined(__WXUNIVERSAL__)
    #include "wx/univ/combobox.h"
#elif defined(__WXMSW__)
    #include "wx/msw/combobox.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/combobox.h"
#elif defined(__WXGTK20__)
    #include "wx/gtk/combobox.h"
#elif defined(__WXGTK__)
    #include "wx/gtk1/combobox.h"
#elif defined(__WXMAC__)
    #include "wx/osx/combobox.h"
#elif defined(__WXCOCOA__)
    #include "wx/cocoa/combobox.h"
#elif defined(__WXPM__)
    #include "wx/os2/combobox.h"
#endif

#ifndef WX_PRECOMP
#endif


// ----------------------------------------------------------------------------
// Mask methods
// ----------------------------------------------------------------------------

void wxComboBox::SetMask(const wxMaskedEdit& mask)
{
        m_maskCtrl = wxMaskedEdit(mask);
        
    if(m_maskCtrl.GetMask().Cmp(wxEmptyString) != 0)
    {
        ChangeValue(m_maskCtrl.GetDefaultValue());
        SetBackgroundColour(m_maskCtrl.GetEmptyBackgroundColour());
        Connect(wxID_ANY, wxEVT_CHAR, wxKeyEventHandler(wxComboBox::KeyPressedMask));
        Connect(wxID_ANY, wxEVT_LEFT_DOWN, wxMouseEventHandler(wxComboBox::MouseClickedMask));
    }
    else
    {
         Disconnect(wxID_ANY, wxEVT_CHAR, wxKeyEventHandler(wxComboBox::KeyPressedMask));
         Disconnect(wxID_ANY, wxEVT_LEFT_DOWN, wxMouseEventHandler(wxComboBox::MouseClickedMask));
    }
}

void wxComboBox::ApplyMask()
{
    wxString string = GetValue();
    wxString formatString;
    unsigned int cursor = GetInsertionPoint();
    bool invalid;

    if(string != m_maskCtrl.GetEmptyMask())
    {
        
        formatString = m_maskCtrl.ApplyFormatCodes(string.Mid(0, GetInsertionPoint()));

    
        invalid = !m_maskCtrl.IsValid(formatString);

        //If the string is not valid
        if(invalid)
        {
            SetBackgroundColour(m_maskCtrl.GetInvalidBackgroundColour());
            Replace(cursor -1, cursor, m_maskCtrl.GetFillChar());
    
            SetInsertionPoint ( cursor - 1);
                
        }
        else
        {
            //If the test is upper or lower case after Applying formats codes
            if(formatString.Cmp(string) != 0)
            {
               if(m_maskCtrl.GetFormatCode().Find('-') == wxNOT_FOUND)
               {
                    Replace(0, formatString.Len() , formatString);
               }
            }

            SetBackgroundColour(m_maskCtrl.GetValidBackgroundColour());
        }

    }


}

void wxComboBox::KeyPressedMask(wxKeyEvent& event)
{
    int keycode = event.GetKeyCode();
    unsigned int cursor = GetInsertionPoint();
    unsigned int fieldIndex = m_maskCtrl.GetFieldIndex(cursor);
    unsigned int fieldMinPos;
    unsigned int it;
    wxString choice;
    wxString mask = m_maskCtrl.GetEmptyMask()[cursor];

    wxChar fillChar = m_maskCtrl.GetFillChar();

    fieldMinPos = m_maskCtrl.GetMinFieldPosition(fieldIndex);

    if(keycode == WXK_PAGEUP || keycode == WXK_PAGEDOWN)
    {
        if(cursor == GetValue().Len())
        {
            fieldIndex = m_maskCtrl.GetNumberOfFields() - 1;
            fieldMinPos = m_maskCtrl.GetMinFieldPosition(fieldIndex);
        }
        else if(cursor == fieldMinPos && fieldIndex != 0 
        && !m_maskCtrl.GetChoices(fieldIndex - 1).IsEmpty())
        {
           fieldIndex--;
           fieldMinPos = m_maskCtrl.GetMinFieldPosition(fieldIndex);
        }
    }

    switch(keycode)
    {
        case(WXK_PAGEUP):
            choice = m_maskCtrl.GetNextChoices(fieldIndex);   
            if(choice.Cmp(wxEmptyString) != 0)
            {
                Replace(fieldMinPos,
                    fieldMinPos + choice.Len() ,
                    choice);
                SetInsertionPoint(m_maskCtrl.GetMaxFieldPosition(fieldIndex) + 1);
            }
        break;
        case(WXK_PAGEDOWN):
            printf("Page down\n");
            choice = m_maskCtrl.GetPreviousChoices(fieldIndex);

            if(choice.Cmp(wxEmptyString) != 0)
            {
                Replace(fieldMinPos,
                    fieldMinPos + choice.Len(),
                    choice); 
            }
            SetInsertionPoint(m_maskCtrl.GetMaxFieldPosition(fieldIndex) + 1);
        break;
        case(WXK_LEFT):
        case(WXK_RIGHT):
            if(m_maskCtrl.GetFormatCodes(fieldIndex).Find('_') == wxNOT_FOUND)
                    event.Skip();
        break;
        case(WXK_BACK):
            if(cursor > 0)
            {
                wxString mask = m_maskCtrl.GetEmptyMask()[cursor - 1];

                if(mask == fillChar)
                { 
                    Replace(cursor - 1, cursor , wxT(" "));
                    
                    cursor = GetInsertionPoint();
                    SetInsertionPoint(cursor -1);
                }
                else
                {
                    SetInsertionPoint(cursor -1);
                }

            }
        break;
        case(WXK_RETURN):
            if(m_maskCtrl.IsNumber())
            {
                bool firstNumber = false;
                bool decimalPointTest = false;
                unsigned int numberOfSpaceBefore = 0;
                unsigned int numberOfSpaceAfter = 0;
                wxString string = GetValue();
                wxString newString;
                wxString tmp;

                for(it = 0; it < string.Len(); it++)
                {
                    if(string[it] != m_maskCtrl.GetDecimalPoint() 
                    && string[it] != fillChar &&  !firstNumber)
                    {
                        firstNumber = true;
                    }
                    else if(string[it] == m_maskCtrl.GetDecimalPoint())
                    {
                        decimalPointTest = true;
                    }
                    else if(firstNumber && string[it] == fillChar && !decimalPointTest)
                    {
                        numberOfSpaceBefore++;
                    }
                    else if(firstNumber && string[it] == fillChar && decimalPointTest)
                    {
                        numberOfSpaceAfter++;
                    }
                    
                    if(string[it] != fillChar)
                    {
                        newString << string[it];
                    }
                }
                
                for(it = 0; it < numberOfSpaceBefore; it++)
                {
                    tmp << wxT(" ");
                }
                newString.Prepend(tmp);
                tmp.Clear();
                
                for(it = 0; it < numberOfSpaceAfter; it++)
                {
                    tmp << wxT(" ");
                }
                newString.Append(tmp);

                ChangeValue(newString);

                event.Skip();
            }
        break;
        case(WXK_DELETE):
            if(cursor < GetValue().Len() && m_maskCtrl.GetFormatCodes(0).Contains('_'))
            {
                wxString mask = m_maskCtrl.GetEmptyMask()[cursor];

                if(mask == fillChar)
                { 
                    Replace(cursor , cursor + 1 , wxT(" "));
                    
                    cursor = GetInsertionPoint();
                    SetInsertionPoint(cursor);
                }
            }

        break;
        case(WXK_TAB):
            if(m_maskCtrl.GetNumberOfFields() == 1 
            || fieldIndex == m_maskCtrl.GetNumberOfFields() - 1)
            {
                event.Skip();
            }
            else
            {
                SetInsertionPoint(m_maskCtrl.GetMinFieldPosition(fieldIndex + 1)); 
            }
        break;
        case (WXK_NUMPAD0):
        case (WXK_NUMPAD1):
        case (WXK_NUMPAD2):
        case (WXK_NUMPAD3):
        case (WXK_NUMPAD4):
        case (WXK_NUMPAD5):
        case (WXK_NUMPAD6):
        case (WXK_NUMPAD7):
        case (WXK_NUMPAD8):
        case (WXK_NUMPAD9): 
                wxChar ch;

                ch = (wxChar)keycode;
                Replace(cursor, cursor+1, ch);
        break;
        case (WXK_SPACE):
                  
            
            ch = (wxChar)keycode;
            if(m_maskCtrl.GetFormatCode().Find('_') != wxNOT_FOUND)
            {

                  if(mask == fillChar)
                  { 
                      Replace(cursor , cursor + 1 , ch);
                  }
                  else
                  {
                      cursor = GetInsertionPoint();
                      SetInsertionPoint(cursor + 1);
                  }

              }
            else
            {
                  if(mask == fillChar)
                  { 
                      Replace(cursor , cursor + 1 , ch);
                  }
            }
            
        break;
        default:
        {        
            if( keycode < 256 && keycode >= 0 && wxIsprint(keycode) )
            {
                wxString string = GetValue();
                wxChar ch;

                if( !event.ShiftDown() )
                {
                    keycode = wxTolower(keycode);
                }
                
                while(string[cursor] != fillChar
                || (string[cursor] == fillChar && m_maskCtrl.GetMask()[cursor] == ' '))
                {
                    Replace(cursor, cursor + 1, string[cursor]);
                    cursor++;
                }
                
                ch = (wxChar)keycode;
                Replace(cursor, cursor+1, ch);
           }
           else
                event.Skip();
    
        }
    }
    cursor = GetInsertionPoint();
    ApplyMask();
}


void wxComboBox::MouseClickedMask(wxMouseEvent& event)
{
    unsigned int spaceIndex = GetValue().Find(m_maskCtrl.GetFillChar());
   
    if(m_maskCtrl.GetFormatCode().Find('_') != wxNOT_FOUND)
    {
        event.Skip();
    }
    else
    {
        if(!HasFocus())
            SetFocus();
        SetInsertionPoint(spaceIndex);
    }

}
 
