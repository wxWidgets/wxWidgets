/////////////////////////////////////////////////////////////////////////////
// Name:        wx/maskedEdit.h
// Purpose:     wxMaskedEdit interface
// Author:      Julien Weinzorn
// RCS-ID:      $Id: ??????????? $
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxMaskedEdit
    
    This class provides a way to control user input.
    Associate it with a wxComboBox or a wxTextCtrl to obtain this control.
    
    @library{wxbase}

    @see wxTextEntry, wxComboBox, wxTextCtrl
    
    @section Mask and field Format
    
    @subsection Mask format
    
    Several characters can be used to create a mask. Allowed characters are:
    
    @li #   numeric only(0-9).
    @li N   letters and numbers (a-z ; A-Z ; 0-9).
    @li A   upperase letters only (A-Z).
    @li a   lowercase letters only (a-z).
    @li C   all letters (a-z ; A-Z).
    @li X   all letters, digits and punctuation.
    @li &   punctuation only.
    @li *   all visible characters.
    @li |   This character is used to make a separation between two fields in a mask.
            The two fields are treated separetely. See example of code.
    
    A character can be escaped with \, example \A means "literal A"
    
    Example:
    
    @code
        wxMaskedEdit* mask  = new wxMaskedEdit(wxT("###.###.###.###")); // IP address
        wxMaskedEdit* mask1 = new wxMaskedEdit(wxT("## A aaa \####")); // possible valid input: 11 F MOS #572.
        wxMaskedEdit* mask2 = new wxMaskedEdit(wxT("###.|###.|###.|###")); // IP address with fields.
        
        //This example is more interesting,as the mask is subdivided in 4 fields
        //when it is created,. All fields have an index (0,1,2,3).
        //With this separation, each field can have a different treatement like 
        //auto-completion, format code, and other possibilities.
        //See the next section for more information about fields.        
    @endcode
    
    @subsection field format
        
        Field format is the same as mask formats. if a mask is cut in more than one field,
        each field can have its formatcode and a default value. To create a mask with fields,
        constuctor need a mask with pipe. Format codes and default value are optional.
        
        Important note: the default value is for the mask. The constructor cut this value and
        store each parts in the appropriate field. If the default value is uncorrect, it is simply
        ignored. 
        
    Examples:
        
        @code
            wxMaskedEdit* mask  = new wxMaskedEdit(wxT("###.|###.|###.|###"), NULL, wxT("127.0.0.1")); // OK
            wxMaskedEdit* mask1 = new wxMaskedEdit(wxT("###.|###.|###.|###"), NULL, wxT("127001"));    // OK
            wxMaskedEdit* mask2 = new wxMaskedEdit(wxT("###.|###.|###.|###"), NULL, wxT("127.00.1"));  // OK
            wxMaskedEdit* mask3 = new wxMaskedEdit(wxT("###.|###.|###.|###"), NULL, wxT("127.0.0.A")); // default value is ignored.
        @endcode
        
        
    
    @subsection format codes
        Format codes is used to modify the behavior of the mask. An example is a Full name. The mask can be: "CCCCCCCCCCCCCC".
        But in a Full name it is a space but we don't know where. format codes can authorized space in this mask.
        
        Format codes is represented by a string. Each character have a signification.
        List of accepted characters:
        
        @li _   Allow space.
        @li !   Force upper, by default, if the mask authorized only uppercase letters, lower case letters are ignored.
        @li ^   Force lower, by default, if the mask authorized only lowercase letters, upper case letters are ignored.
        @li R   Right align fields. If the control have left align property, the mask is more prioritary.
        @li r   Insert letter in the control by the right, this implies R.
        @li -   Reserved two space for negative number.
        @li F   The control calculate its size from the mask length.
        @li S   Select an entire field when users navigating in a new field.
        
    Examples:
        
        @code
             wxMaskedEdit* mask = new wxMaskedEdit(wxT("######.#####"), wxT("F-_"))); 
             // The floating number can be: 123.12 spaces are integrated before and after the number
             wxMaskedEdit* mask1 = new wxMaskedEdit(wxT("CCCCCCCCCCCCCC"), wxT("F!_")));
             // This mask is used for Full Name control. Possible valid entries are: 
             //Julien Weinzorn or julien weinzorn or JULIEN WEINZORN.
             // All lowercase letters are converted automatically when the user input the characters.
        @endcode

        
        
    
    
*/

class wxMaskedEdit
{
public:

    /**
        Default constructor
        All attributs is set to its default value
    */
    wxMaskedEdit();
    
    /**
        copy constructor
        @param maskedEdit 
            the masked for the copy
    */
    wxMaskedEdit(const wxMasedkEdit& maskedEdit);
    
    /**
        general constructor with a single field. If the pipe character is passed 
        in this constructor, it will be ignore.
        @param mask 
            The control masked.
        @param formatCode
            The format code is used to modify the behavior of the mask (see above).         
        @param defaultValue
            The default value for the mask. If this value is not allow 
            for the specified mask ,the default value is set at wxEmptyString.
    */
    wxMaskedEdit( const wxString& mask 
                , const wxString& formatCode   = wxEmptyString
                , const wxString& defaultValue = wxEmptyString);
                
    /**
        General constructor with mutiple fields
        @param mask 
            The control mask.
        @param formatCode
            The formatCode is used to modify the behavior of the mask (see above).
        @param defaultValue
            The default value for the mask. If this value is not allow 
            for the specified mask ,the default value is set at wxEmptyString. 
            This constructor allow for each field its default value.
            
    */
    wxMaskedEdit( const wxString& mask , const wxArrayString& formatCode = NULL
                , const wxString& defaultValue  = wxEmptyString);
    
    
    
    /**
        Return the value of the wxTextEntry associated
        without the mask
    */
    wxString GetPlainValue();
    
    /**
        Clear the mask and the cursor return in the beginning of the mask
    */
    void ClearValue();
    
    /**
        Set the value in the text control associated
        @param value 
            The new text in the control, missing mask characters are inserted
            automatically
        @return return if the value is correctly set or not
    */
    bool SetValue(wxString value);
    
    /**
        Test if the current sequence is valid
        @return return true if the sequence is correct
    */
    bool IsValid() const;
    
    /**
        Test if the current sequence is empty 
        @return return true if the sequence is empty according to the fillChar
    */
    bool IsEmpty() const;
    
    /**
        Set the mask to a new value. This operation coud be change the number of 
        fields, and the choices. If a choices is'nt correct it is suppress.
        @param mask 
            The new mask.
    */
    void SetMask(wxString mask);
    
    /**
        return the current Mask
        @return the current mask
    */
    wxString GetMask() const;
    
    /**
        Return the formatCode. If the mask contain one field, only the first case
        of the array contain the formatCode.
        @return return a wxArrayString with all formats code.
    */
    wxArrayString GetFormatCodes() const;
    
    /**
        This methods return the list of possible choices. If there is more than one field
        , this function return an empty choices because all fields have its own choices.
        @return return a wxArrayString with all possible choice.
    */
    wxArrayString GetChoices() const;
    
    /**
        This methods add a default choice.
        @param choice 
            The choice added.
        @return return if the choice is added or not
    */
    bool AddDefaultChoice(wxString choice);

    /**
        This methods add a list of default choices.
        @param choice 
            The choice added.
        @return return if the choices are added or not
    */
    bool AddDefaultChoices(const wxArrayString& choices);  

    /**
        Provide the number of field
        @return return the number of fields or -1 if it is a single field
    */
    int GetNumberOfFields();
    
    /**
        Change the control background colour when it is empty.
        @param colour 
            the new colour
    */
    void SetEmptyBackgroundColour(const wxColour& colour);
     
    /**
        Change the control background colour when it is invalid.
        @param colour 
            the new colour
    */
    void SetInvalidBackgroundColour(const wxColour& colour);       
    /**
        Change the control background colour when it is valid.
        @param colour 
            the new colour
    */
    void SetValidBackgroundColour(const wxColour& colour);
    
    /**
        Set the mask in a field to a new value. This operation coud be change the choices. 
        If a choices is'nt correct it is suppress.
        @param fieldIndex
            index of the field.
        @param mask 
            The new mask.
    */
    void SetMask(int fieldIndex, wxString mask);
    
    /**
        return the current field mask
        @param fieldIndex
            index of the field.
        @return the current fieldmask
    */
    wxString GetMask(int fieldIndex) const;
    
    /**
        Return the field formatCode. If the mask contain one field, only the first case
        of the array contain the formatCode.
        @param fieldIndex
            index of the field.
        @return return a wxArrayString with all formats code.
    */
    wxArrayString GetFormatCodes(int fieldIndex) const;
    
    /**
        This methods return the list of possible field choices. If there is more than one field
        , this function return an empty choices because all fields have its own choices.
        @param fieldIndex
            index of the field
        @return return a wxArrayString with all possible choice.
    */
    wxArrayString GetChoices(int fieldIndex) const;        
        
    /**
        This methods add a default choice in a field.
        @param fieldIndex
            index of the field    
        @param choice 
            The choice added.
        @return return if the choice is added or not
    */
    bool AddDefaultChoice(int fieldIndex, wxString choice);

    /**
        This methods add a list of default choices.
        @param fieldIndex
            index of the field
        @param choice 
            The choice added.
        @return return if the choices are added or not
    */
    bool AddDefaultChoices(int fieldIndex, const wxArrayString& choices);  
    
        
   
