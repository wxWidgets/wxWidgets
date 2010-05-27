/////////////////////////////////////////////////////////////////////////////
// Name:        wx/maskededit.h
// Purpose:     wxMaskedEdit interface
// Author:      Julien Weinzorn
// RCS-ID:      $Id: ??????????? $
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MASKED_EDIT_H_
#define _WX_MASKED_EDIT_H_

#include "wx/defs.h"
#include <stdlib.h>
#include "wx/colour.h"
#include "wx/dynarray.h"
#include "wx/maskedfield.h"

class wxMaskedField;
WX_DECLARE_OBJARRAY(wxMaskedField, fieldsArray);


class wxMaskedEdit
{
public:

    wxMaskedEdit();
    
    wxMaskedEdit(const wxMaskedEdit& maskedEdit);
    

    wxMaskedEdit( wxString& mask 
                , const wxString& formatCode   = wxDEFAULT_FORMAT_CODES
                , const wxString& defaultValue = wxDEFAULT_VALUE);
                
  
    wxMaskedEdit( const wxString& mask , const wxArrayString& formatCode = NULL
                , const wxString& defaultValue  = wxEmptyString);
    
    
    
    
    //    Return the value of the wxTextEntry associated
    //    without the mask
    wxString GetPlainValue();
    
    
    //Clear the mask and the cursor return in the beginning of the mask
    void ClearValue();
    
 
    //Set the value in the text control associated
    bool SetValue(wxString value);
    
    //Test if the current sequence is valid
    bool IsValid() const;
    
    //Test if the current sequence is empty 
    bool IsEmpty() const;
    
    //Set the mask to a new value. 
    void SetMask(wxString mask);
    
    wxString GetMask() const;
    
    wxString GetFormatCode() const;

    wxString GetDefaultValue() const;
    
    wxArrayString GetChoices() const;
    
    bool AddDefaultChoice(wxString choice);

    bool AddDefaultChoices(const wxArrayString& choices);  

    int GetNumberOfFields();
    
    void SetEmptyBackgroundColour(const wxColour& colour);
     
    void SetInvalidBackgroundColour(const wxColour& colour);       

    void SetValidBackgroundColour(const wxColour& colour);
  
    void SetMask(int fieldIndex, wxString mask);
    
    wxString GetMask(int fieldIndex) const;
    
    wxArrayString GetFormatCodes(int fieldIndex) const;
    
    wxArrayString GetChoices(int fieldIndex) const;        
        
    bool AddDefaultChoice(int fieldIndex, wxString choice);

    bool AddDefaultChoices(int fieldIndex, const wxArrayString& choices);  
    
private:
    fieldsArray m_mask;
    int m_cursor_field;
    int m_cursor_inside_field;

        
};   

#endif