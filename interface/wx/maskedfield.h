/////////////////////////////////////////////////////////////////////////////
// Name:        wx/maskedField.h
// Purpose:     interface of wxMaskedField
// Author:      Julien Weinzorn
// Created:        2010-05-24
// RCS-ID:      $Id: ??????????? $
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxMaskedField

    This class managed individual fields in a masked Edit.
    Each field have an index beginning at 0, it indicate its position in a mask.

    A instantiated mask can be associated with a mask with SetFieldParameters. 
    This class offer a list of optional parameters to specialized the field.
    

    @library{wxbase}

    @see wxMaskedEdit, wxTextEntry
*/

class wxMaskedField
{
public:

    /**
        Default constructor.
        The field is initialised to "" with an default value equal to "".    
    */
    wxMaskedField();
    
    /**
        Copy constructor.
        @param maskedField maskField copy
    */
    wxMaskedField(const wxMaskedField& maskedField);
    
    /**
        General constructor, all options can be enable
        @param field 
            Part of mask, for see what is a valid value: @see wxMaskedEdit.
        @param formatCodes
            The formatCode used in the field, for see what is a valid value: @see wxMaskedEdit.
        @param choices
            A list of possible choice for the field.
        @param autoSelect
            If autoselect is set at true, when user enter a key, the mask attempt to autocomplete
            the field.
        @param groupChar 
            GroupChar offer the possibility to change the character between integer value.
        @param decimalPoint
            decimalPoint is used to change the decimal poin in a floating number.

        @param useParensForNegatives
            Set it at true, () is used instead of - for negative number.
    */
    wxMaskedField( const wxString& mask        
                 , const wxString& formatCodes  = wxT("F")
                 , const wxArrayString& choices = NULL
                 , const bool  autoSelect = false,
                 , const wxUniChar& groupChar = '', const wxUniChar& decimalPoint = '.'
                 , const bool useParensForNegatives = false);
    
    /**
        Indicates if the string is considered empty for the field
        @param string
            storing to be tested
    */
    IsEmpty(const wxString& string) const;

    /**
        this method is usefull to convert a string in an other string
        which follow the format codes.
        @param string the original string
        @return a new string which the format codes apply
    */
    wxString ApplyFormatCodes(const wxString& string);
    
    /**
        Indicates if the string is valid for the field
        @param string
            string to be tested
    */
    IsValid(const wxString& string) const;
 
    /**
        This method add a new default choice. If the choice is not correct 
        compared to the mask, this method return false.
        @param choice the new choice value
        @return if the choice is added or not
    **/
    bool AddChoice(const wxString& choice);

    /**
        This method add a list of default choice. 
        If one element of the list is not correct, this method
        return false and nothing is added.

        @param choices the  choices value.
        @return if the list is added or not.
    */
    bool AddChoices(const wxArrayString& choices); 


    /**
        Return the default value.
        @return the default value.
    */
    wxString GetDefaultValue() const; 

    /**
        This method set the mask. It control to if the choices is already 
        correct. If it is not correct, the choice is deleted
        @param mask the new mask.
    */
    void SetMask(const wxString& mask);

    /**
        @return the current mask.
    */
    wxString GetMask() const;
    /**
        @return the current format codes.
    */
    wxString GetFormatCodes() const;
    /**
        @return the current group char.
    */
    wxUniChar GetGroupChar() const;
    
    /**
        @return the current decimal point.
    */
    wxUniChar GetDecimalPoint() const;

    /**
        @return a list with the default choices.
    */
    wxArrayString GetChoices() const;

    /**
        test if parens is used.
        @return if parens is used.
     */
    bool IsParensForNegatives() const;

    /**
        test if auto select is available.
        @return result of the test.
    */
    bool IsAutoSelect() const;

    /**
        This method is used to convert a string in an other string without the 
        mask. If the string is not valid compared to the mask, 
        an emptyString is returned.

        @param string the string with the mask.
        @return a new string without the mask.
    */
    wxString GetPlainValue(const wxString& string);
   

    /**
        This method test if a character is valid compared in a maskChar.

        @param maskChar the mask represented by a character.
        @param character the tested character.
        @return if the character is valid from the mask.
    */
    bool IsCharValid(const wxChar maskChar, const wxChar character) const;

}
