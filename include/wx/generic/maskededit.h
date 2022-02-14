/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/maskededit.h
// Purpose:     wxMaskedEdit interface
// Author:      Manuel Martin
// Modified by:
// Created:     2012-07-30
// RCS-ID:      $Id$
// Copyright:   (c) Manuel Martin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MASKED_EDIT_H_
#define _WX_MASKED_EDIT_H_

#include "wx/defs.h"

#if wxUSE_MASKED_EDIT

#include "wx/colour.h"
#include "wx/vector.h"

#if wxUSE_TEXTCTRL
    #include "wx/textctrl.h"
#endif // wxUSE_TEXTCTRL

#if wxUSE_COMBOBOX
    #include "wx/combobox.h"
#endif // wxUSE_COMBOBOX


//Colours used for different cases.
struct wxMaskedEditColours
{
    //When everything is OK
    wxColour colOKForeground;
    wxColour colOKBackground;
    //When something is wrong
    wxColour colInvaldForeground;
    wxColour colInvaldBackground;
    //When nothing is filled, empty control's value
    wxColour colBlankForeground;
    wxColour colBlankBackground;
};

//An object to store flags
class wxMaskedEditFieldFlags
{
public:
    wxMaskedEditFieldFlags(wxAlignment alignment = wxALIGN_LEFT)
    {
        //Only left or right alignment
        m_alignment = alignment == wxALIGN_RIGHT ? wxALIGN_RIGHT : wxALIGN_LEFT;
        m_fillChar = wxChar(' ');
        m_paddingChar = wxChar('\0');
    }

    //Setters for all flags. Return the object itself, so that calls to them
    // can be chained

    wxMaskedEditFieldFlags& SetAlignment(wxAlignment alignment)
    {
        //Only left or right alignment
        m_alignment = alignment == wxALIGN_RIGHT ? wxALIGN_RIGHT : wxALIGN_LEFT;
        return *this;
    }

    wxMaskedEditFieldFlags& SetFillChar(wxChar fillchar)
    {
        if ( !wxIsprint(fillchar) )
        {
            wxFAIL_MSG("Warning: Unprintable character in masked flags");
        }
        m_fillChar = fillchar;
        return *this;
    }

    wxMaskedEditFieldFlags& SetPaddingChar(wxChar paddingchar)
    {
        if ( paddingchar != '\0' && !wxIsprint(paddingchar) )
        {
            wxFAIL_MSG("Warning: Unprintable character in masked flags");
        }
        m_paddingChar = paddingchar;
        return *this;
    }

    //Accessors
    wxAlignment GetAlignment() const
    {   return m_alignment; }

    wxChar GetFillChar() const
    {   return m_fillChar; }

    wxChar GetPaddingChar() const
    {   return m_paddingChar; }

private:
    wxAlignment m_alignment;
    wxChar m_fillChar;
    wxChar m_paddingChar;
};

class wxMaskedEdit;
//For field test
typedef long wxMaskedFieldFunc(const wxMaskedEdit*, size_t, void*);
//For whole value test
typedef long wxMaskedFunc(const wxMaskedEdit*, void*);

// ----------------------------------------------------------------------------
// Base class for all masked edit controls
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMaskedEdit
{
private:
    // All configurable parameters are stored here.
    class wxMaskedEditParams
    {
    public:
        wxMaskedEditParams()
        {
            ResetFields(0);
            controlFunc = NULL;
            controlFuncParams = NULL;
        };
        ~wxMaskedEditParams() {};

        //Reset the per field parameters.
        void ResetFields(size_t numFields)
        {
            fieldsFlags.clear();
            fieldsFlags.resize(numFields);
            fieldsFuncs.clear();
            fieldsFuncs.resize(numFields, NULL);
            fieldFuncsParams.clear();
            fieldFuncsParams.resize(numFields, NULL);
        }

        //The mask.
        wxString mask;

        //Colours used for different cases. The whole control is coloured.
        wxMaskedEditColours colours;

        //Fields settings containers.
        //Flags
        wxVector<wxMaskedEditFieldFlags> fieldsFlags;

        //Functions to test a field
        wxVector<wxMaskedFieldFunc*> fieldsFuncs;
        //And their parameters
        wxVector<void*> fieldFuncsParams;

        //Function for the whole control
        wxMaskedFunc* controlFunc;
        //And its parameters
        void* controlFuncParams;
    };

public:
    //ctor.
    wxMaskedEdit()
    { m_bellOnError = true; }

    //dtor.
    ~wxMaskedEdit() {};

    //Control configuration -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
    //Setters for configuration parameters. Replace old ones.
    //Sets the mask. If no field is defined, it returns false.
    /* Characters and their meaning:
      #    Required digit.
      9    Optional digit.
      A    Required alpha character.
      a    Optional alpha character.
      N    Required alphanumeric character (i.e. letter or digit).
      n    Optional alphanumeric character.
      C    Required character (any with keycode > 31).
      c    Optional character (any with keycode > 31).
      X    Required hexadecimal character [0-F]
      x    Optional hexadecimal character [0-F]
      {x}  The previous character (one of the above) is taken x times. This
           is an "expander": #{3} expands into ###, X{4} expands into XXXX.
      >    Convert all following characters to upper case.
      <    Convert all following characters to lower case.
      ^    Switch off upper/lower case conversion.
      \    Escape the next character and use it as a literal.
     Notice the above specifications (except 'C' and 'c') are tested using
     wxIsxxx functions. So, they are locale dependent.
     Any other char is a literal. It is shown in the control, but it can
     not be edited.
     Literals are also used as fields separators.
     '|'  is a special literal. It's invisible, takes no space, but still
     is a fields separator.

     Space is not a character (except for C and c commands).
    */

    bool SetMask(const wxString& mask);

    //Set the colours for different cases.
    void SetMaskedColours(const wxMaskedEditColours& colours);

    //Set the flags for a field.
    void SetFieldFlags(size_t index, const wxMaskedEditFieldFlags& flags)
    {
        m_params.fieldsFlags.at(index) = flags;
    }

    //Set all fields with the same flags.
    void SetAllFieldsFlags(const wxMaskedEditFieldFlags& flags);

    //Set the test function for a field.
    void SetFieldFunction(size_t index, wxMaskedFieldFunc* fn, void* param);

    //Set all fields with the same functions.
    void SetAllFieldsFunction(wxMaskedFieldFunc* fn, void* param);

    //Set the test function for the whole control.
    void SetControlFunction(wxMaskedFunc* fn, void* param);

    //Set all parameters at once, replacing old ones.
    bool SetParams(const wxMaskedEditParams& params);

    //Set the control's value from a undecorated text.
    bool SetPlainValue(const wxString& plainValue);

    //Sets the value for a field.
    bool SetFieldValue(size_t index, const wxString& plainValue);

    //Check characters, fields and the whole control's value.
    long GetInvalidFieldIndex() const;

    //Bell on error.
    void SetBellOnError(bool bell)
    {
        m_bellOnError = bell;
    }


    //Accessors -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
    //Get the number of fields.
    size_t GetFieldsCount() const
    {
        return m_arrPosS.size();
    }

    //Get control's value, without decorations.
    wxString GetAllFieldsValue() const;

    //Get the current text in a field. Empty cells are filled with the padding
    //char set for this field.
    wxString GetFieldValue(size_t index) const;

    //Get the mask. The string passed at SetMask()
    wxString GetMask() const
    {
        return m_params.mask;
    }

    //Get the colours for different cases.
    wxMaskedEditColours GetMaskedColours() const
    {
        return m_params.colours;
    }

    //Get the flags used in a field.
    wxMaskedEditFieldFlags GetFieldFlags(size_t index) const
    {
        return m_params.fieldsFlags.at(index);
    }

    //Get the parameters.
    wxMaskedEditParams GetParams() const
    {
        return m_params;
    }

    //Event handling
    void OnKeyDown(wxKeyEvent& event);
    void OnChar(wxKeyEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
#if wxUSE_COMBOBOX
    void OnComboSelect(wxCommandEvent& event);
    void OnComboDrop(wxCommandEvent& event);
#endif // wxUSE_COMBOBOX

protected:
    //Like SetFieldValue(), but operates on inText, not in control
    wxString SetFieldOnText(size_t index,
                            const wxString& plainValue,
                            const wxString& inText);

    //Like GetFieldValue(), but operates on inText, not in control
    wxString GetFieldOnText(size_t index,
                            const wxString& inText,
                            wxChar chPad) const;

    //If the cell has not been filled
    bool CellIsEmpty(long curpos) const
    {
        return !m_filled[(size_t)curpos];
    }

    //Get the internal type for a char in the expanded mask
    int GetMaskedType(size_t pos) const
    {
        return m_maskIDs[pos];
    }

    //Default colours
    void GetDefaultColours(wxColour* defFgCol, wxColour* defBgCol);

    //Handle the selection, returning the new possible control's value
    wxString HandleSelection(long *pos, bool *changed);

    //Our control derives from wxTextEntry, but not this class. Get a pointer to it
    wxTextEntry* GetTextEntry() const;

    //Update colours, depending on empty/valid/invalid cases
    void UpdateMaskedColours();

    //Join characters and align the field
    void NormalizeField(size_t index, wxString* inText);

    //Test if the field has a free cell for a char.
    bool HasFieldRoom(size_t index) const;

    //Find the field in which the cursor is.
    int FindField(long curpos) const;
    //Find the field to insert a char in the given position.
    int GuessField(long pos) const;

    //A reasonable position for caret in the previous field to where the caret is.
    long PosForFieldPrev(long curpos) const;
    //A reasonable position for caret in the next field to where the caret is.
    long PosForFieldNext(long curpos) const;

    //Move caret from current position to the foreseeable one
    long MoveCaret(long curPos, bool toRight);

    //Insert a char at an intended position. Returns false if insertion is not
    // possible (invalid char or no room available)
    bool InsertAtPos(wxChar iChar, long* pos, size_t index, wxString* inText);

    //Process a char, coming from OnChar() or from DoPaste()
    bool ProcessChar(wxChar aChar, long* pos, wxString* inText, bool* changed);

    //Cutting
    void DoCut();
    //Pasting
    void DoPaste();

    //Set the proper string for SetValue/ChangeValue
    wxString SetMaskedValue(const wxString& value);

    //Show the colours
    void UseColours(int whatColours);

    enum
    {
        BlankColour = 1,
        OKColour,
        InvalidColour
    };

    //Bell
    void DoBell()
    {
        if ( m_bellOnError )
            wxBell();
    }

    //General Test Functions -  -  -  -  -  -  -  -  -
    //Test the character according with the mask.
    bool CharAgreesWithMask(wxChar testChar, long pos) const;

    //Return the char converted to upper/lower case according with the mask
    wxChar ToUppLowCase(wxChar ch, size_t pos);

    //Test the character.
    bool TestChar(wxChar* tryChar, long pos);

    //Tell if field passed all of its tests.
    long IsFieldValid(size_t index) const;

    //Containers -  -  -  -  -  -  -  -  -  -  -  -
    //The configuration container
    wxMaskedEditParams m_params;

    //The expanded mask
    wxString m_ExpMask;
    //Positions in m_ExpMask where each field starts
    wxVector<long> m_arrPosS;
    //Positions in m_ExpMask where each field finishes
    wxVector<long> m_arrPosF;
    //Internal representation of the mask, with identifiers.
    wxVector<int> m_maskIDs;
    //If each cell is filled with a char or it is empty
    wxVector<bool> m_filled;
    //Backup copy while temporary use
    wxVector<bool> m_fillBck;

    //The real control
    wxControl* msk_control;

    //Bell on error. Default is true
    bool m_bellOnError;

    //The mask identifiers
    enum
    {
        DigitReq = 1, //'#'
        DigitOpt,     //'9'
        AlphaReq,     //'A'
        AlphaOpt,     //'a'
        AlpNumReq,    //'N'
        AlpNumOpt,    //'n'
        AnyCharReq,   //'C'
        AnyCharOpt,   //'c'
        HexReq,       //'H'
        HexOpt,       //'H'
        Literal,
        UppChar = 100, //'>'
        LowChar = 200  //'<'
    };

};

// ----------------------------------------------------------------------------
// Masked edit controls
// ----------------------------------------------------------------------------

#if wxUSE_TEXTCTRL
//wxMaskedEditText is only for wxTextCtrl
class WXDLLIMPEXP_CORE wxMaskedEditText : public wxMaskedEdit,
                                         public wxTextCtrl
{
public:
    //Constructors
    wxMaskedEditText()
    {
        MaskInit( 0 );
    }

    wxMaskedEditText(wxWindow *parent, wxWindowID id,
                     const wxString& value = wxEmptyString,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = 0,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxTextCtrlNameStr)
         : wxTextCtrl(parent, id, value, pos, size, style, validator, name)
    {
        MaskInit( style );
        MaskPostInit();
    }

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxTextCtrlNameStr)
    {
        MaskInit( style );

        if ( ! wxTextCtrl::Create(parent, id, value, pos, size, style,
                                  validator, name) )
        {
            return false;
        }
        MaskPostInit();
        return true;
    }

    virtual ~wxMaskedEditText() {};

    //Override SetValue()
    virtual void SetValue(const wxString& value)
    {
        wxTextCtrl::SetValue( SetMaskedValue(value) );
        //Test the new value and set colours
        UpdateMaskedColours();
    }

    //Override ChangeValue()
    virtual void ChangeValue(const wxString& value)
    {
        wxTextCtrl::ChangeValue( SetMaskedValue(value) );
        //Test the new value and set colours
        UpdateMaskedColours();
    }

private:
    void MaskInit( long style )
    {
        //Only single line control
        if ( style & wxTE_MULTILINE )
        {
            wxFAIL_MSG("Multiline wxMaskedEditText not allowed yet");
        }
    }
    void MaskPostInit();
};
#endif // wxUSE_TEXTCTRL

#if wxUSE_COMBOBOX
//wxMaskedEditCombo is only for wxComboBox
class WXDLLIMPEXP_CORE wxMaskedEditCombo : public wxMaskedEdit,
                                          public wxComboBox
{
public:
    //Constructors
    wxMaskedEditCombo() {};

    wxMaskedEditCombo(wxWindow *parent, wxWindowID id,
                      const wxString& value = wxEmptyString,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      int n = 0, const wxString choices[] = NULL,
                      long style = 0,
                      const wxValidator& validator = wxDefaultValidator,
                      const wxString& name = wxComboBoxNameStr)
        : wxComboBox(parent, id, value, pos, size, n, choices, style, validator, name)
    {
        mskPostInit();
    }

    wxMaskedEditCombo(wxWindow *parent, wxWindowID id,
                      const wxString& value,
                      const wxPoint& pos,
                      const wxSize& size,
                      const wxArrayString& choices,
                      long style = 0,
                      const wxValidator& validator = wxDefaultValidator,
                      const wxString& name = wxComboBoxNameStr)
        : wxComboBox(parent, id, value, pos, size, choices, style, validator, name)
    {
        mskPostInit();
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0,
                const wxString choices[] = NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr)
    {
        if ( ! wxComboBox::Create(parent, id, value, pos, size,
                                  n, choices, style, validator, name) )
        {
            return false;
        }
        mskPostInit();
        return true;
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr)
    {
        if ( ! wxComboBox::Create(parent, id, value, pos, size,
                                  choices, style, validator, name) )
        {
            return false;
        }
        mskPostInit();
        return true;
    }

    //Override SetValue()
    virtual void SetValue(const wxString& value)
    {
        wxComboBox::SetValue( SetMaskedValue(value) );
        //Test the new value and set colours
        UpdateMaskedColours();
    }

    //Override ChangeValue()
    virtual void ChangeValue(const wxString& value)
    {
        wxComboBox::ChangeValue( SetMaskedValue(value) );
        //Test the new value and set colours
        UpdateMaskedColours();
    }

private:
    void mskPostInit()
    {
        msk_control = this;

        //Set default fixed pitch font
        //Don't use directly m_font because wxWindowBase::SetFont decides
        // it is the same font.
        wxFont font = GetFont();
        font.SetFamily(wxFONTFAMILY_TELETYPE);
        SetFont(font);

        //Event handlers
        Bind(wxEVT_KEY_DOWN, &wxMaskedEdit::OnKeyDown, this);
        Bind(wxEVT_CHAR, &wxMaskedEdit::OnChar, this);
        Bind(wxEVT_COMMAND_TEXT_CUT, &wxMaskedEdit::OnCut, this);
        Bind(wxEVT_COMMAND_TEXT_PASTE, &wxMaskedEdit::OnPaste, this);
        //Also when an item is selected
        Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &wxMaskedEdit::OnComboSelect, this);
        //When the list is shown, change control's colour
        Bind(wxEVT_COMMAND_COMBOBOX_DROPDOWN, &wxMaskedEdit::OnComboDrop, this);
    }
};
#endif // wxUSE_COMBOBOX


// ----------------------------------------------------------------------------
//                            Predefined functions
//
// They return '-1' if all is OK. Otherwise, return the position of the error.
// ----------------------------------------------------------------------------

//Range check
class WXDLLIMPEXP_CORE wxRangeParams
{
public:
    wxRangeParams()
    {
        rmin = rmax = 0;
        base = 10;
    }
    long rmin;
    long rmax;
    int base;
};

WXDLLIMPEXP_CORE long wxMaskedRangeCheck(const wxMaskedEdit* caller,
                                        size_t index,
                                        void* params);

//Date check
struct WXDLLIMPEXP_CORE wxDateParams
{
    size_t dayField;
    size_t monthField;
    size_t yearField;
};

WXDLLIMPEXP_CORE long wxMaskedDateShort(const wxMaskedEdit* caller, void* params);

#endif // wxUSE_MASKED_EDIT

#endif // _WX_MASKED_EDIT_H_
