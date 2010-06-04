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
    

    wxMaskedEdit( const wxString& mask 
                , const wxString &formatCode   = wxDEFAULT_FORMAT_CODES
                , const wxString &defaultValue = wxDEFAULT_VALUE);
                
  
    wxMaskedEdit( const wxString& mask , const wxArrayString& formatCode = wxArrayString()
                , const wxString& defaultValue  = wxEmptyString);
    
    
    
    
    //    Return the value of the wstring associated
    //    without the mask
    wxString GetPlainValue(wxString string);
    
    
    //Clear the mask and the cursor return in the beginning of the mask
    void ClearValue();
    
 
    //Set the value in the text control associated
    // bool SetValue(wxString value);
    
    //Test if the current sequence is valid
    bool IsValid(wxString string) const;
    
    //Test if the current sequence is empty 
    bool IsEmpty(wxString string) const;
    
    //Set the mask to a new value. 
    bool SetMask(const wxString& mask);

    
    wxString GetMask() const;
    
    wxString GetFormatCode() const;

    wxString GetDefaultValue() const;
 
    wxArrayString GetChoices() const;
 
    bool AddChoice(wxString& choice);
 
    bool AddChoices(const wxArrayString& choices);  

    int GetNumberOfFields() const ;
    
    void SetEmptyBackgroundColour(const wxColour& colour);
     
    void SetInvalidBackgroundColour(const wxColour& colour);       

    void SetValidBackgroundColour(const wxColour& colour);
  
    bool SetMask(unsigned int fieldIndex, wxString& mask);
  
    wxString GetMask(unsigned int fieldIndex) const;

    wxString GetFormatCodes(unsigned int fieldIndex) const;
  
    wxArrayString GetChoices(unsigned int fieldIndex) const;        
 
    bool AddChoice(unsigned int fieldIndex, const wxString& choice);

    bool AddChoices(unsigned int fieldIndex, const wxArrayString& choices);  
    
private:
    fieldsArray m_mask;
    wxString m_maskValue;
    int m_cursorField;
    int m_cursorInsideField;

        
};   

#include <wx/arrimpl.cpp> // This is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(fieldsArray);



#endif
