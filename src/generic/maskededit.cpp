/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/maskedfield.cpp
// Purpose:     wxMaskedEdit implementation
// Author:      Manuel Martin
// Modified by:
// Created:     2012-07-30
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_MASKED_EDIT

#if wxUSE_COMBOBOX
    #include "wx/scrolbar.h"
#endif // wxUSE_COMBOBOX

#include "wx/clipbrd.h"
#include "wx/generic/maskededit.h"


//////////////////////////  wxMaskedEdit  ////////////////////////////////
/*EXPLANATIONS
  This control must filter chars, according with the mask. We store mask
  information in two ways: the expanded string (see below) 'm_ExpMask' and
  a vector of types (enum: mDigitReq, mDigitOpt, mAlphaReq, etc) 'm_maskIDs'.
  m_ExpMask tells us the literals used between fields. m_maskIDs allows us
  to store also upper/lower commands (by adding some value).

  Fields are homogeneous, which means, for example, that we don't allow a
  digit where only an alpha char is accepted.

  All ways for enter/delete characters (EVT_CHAR, Paste, SetFieldValue, etc)
  end in the same point: calling TextEntry::SetValue(newValue). The user can
  call SetValue(useValue). And before setting it, we must test it, filtering
  chars against the mask.
  Because this new value contains not only the 'typed' chars, but also literal
  chars and the chars used to fill empty positions ("cells"), we can not tell
  just from this value which are literals, which are empty cells and which are
  not. In a common case, space ' ' is used as literal and also as the char for
  empty cells. We annotate used cells in m_filled, carefully setting true/false
  for each position in the value.

  When the user calls SetValue(), we can't make use of this m_filled to test
  the value. But we must fill m_filled. We set false (empty cell) when we
  meet a literal or the char we will use to fill empty cells. The consequence
  is that the 'fillchar' is not accepted in a field that uses it, even if the
  mask in that field allows that fillchar.

  A tricky part in this code is the caret movement. We use PosForFieldPrev(),
  PosForFieldNext() and MoveCaret() combined adequately for each case.

  The functions used to test a field and the one used with the whole control
  don't change the value. They only say right/wrong. If the user has set the
  colours parameter, the control is coloured according this function's return.
  And, of course, also accordingly with the mask (think of required chars).
*/

// Set the mask. If there isn't a field in it, returns false.
/*
  We really don't use the string passed as mask. This is due it may be a
  compressed one. "#-#-#{5}" will expand into "#-#-#####". Instead, we use
  an array of identifiers, with an identifier (see the enum) for each
  expanded position. We also store the expanded mask, so we can have the
  'literals' used as field separators.

  The expanded mask sets the fixed-length user's input. In other words, the
  control's value has always the same length, despite of if some positions
  are empty or not.
  We store the begin/end positions (expanded) for each field (m_arrPosS and
  m_arrPosF). This way, we can quickly know which field corresponds to control's
  cursor position.

  Each field must be homogeneous: all of its commands must allow the same
  chars. So "#9" is valid, while "#C" is not.
*/

//Check the cell to be homogeneous with the field type.
// type1 type2 are valid types (e.g. # 9 or A a)
inline bool IsHomogField(int* firstFieldCmm, int type1, int type2)
{
    if ( *firstFieldCmm == -1)
    {
        *firstFieldCmm = type1; //Field beginning, annotate the command
    }
    else if ( *firstFieldCmm != type1 && *firstFieldCmm != type2)
    {
        wxFAIL_MSG("Unhomogeneous field in mask");
        return false;
    }
    return true;
}


bool wxMaskedEdit::SetMask(const wxString& mask)
{
    //If mask is empty, do nothing
    if ( mask.IsEmpty() )
    {
        wxFAIL_MSG("Empty mask is not allowed");
        return false; //This "mask" is not a mask.
    }

    //Temporary objects. We need them because this 'mask' may be wrong, which
    // means it has really no field at all. If so, we don't change anything.
    wxString   maskTmp;
    wxVector<int> maskIDTmp;
    wxVector<long> fieldsBeg;
    wxVector<long> fieldsEnd;
    //We'll use this string to set the min size of the control
    wxString sForSize;

    //Iterate through the mask
    long pos = 0;
    wxString comchars = "#9AaNnCcXx"; //expandable commands
    bool fieldStarted = false;
    bool lastIsComchars = false;
    int firstFieldCmm = -1; //the first command in a field
    bool isLiteral = true;
    //An integer to add for upper/lower forced cases
    int caseULforced = 0; //3-state: 0 nothing, mUppChar, mLowChar
    wxString::const_iterator i = mask.begin();
    while ( i != mask.end() )
    {
        wxChar ch = *i;

        //Possibilities: command, upper/lower, expansion, literal
        if ( comchars.Find( ch ) != wxNOT_FOUND )
        {
            //let's see if it's a field's start
            if ( !fieldStarted )
            {
                fieldsBeg.push_back( pos ); //position in the expanded mask
                fieldStarted = true;
            }

            if ( ch == '#' )
            {
                maskIDTmp.push_back( DigitReq );
                if ( !IsHomogField(&firstFieldCmm, DigitReq, DigitOpt) )
                    return false;
            }
            else if ( ch == '9' )
            {
                maskIDTmp.push_back( DigitOpt );
                if ( !IsHomogField(&firstFieldCmm, DigitReq, DigitOpt) )
                    return false;
            }
            else if( ch == 'A' )
            {
                maskIDTmp.push_back( AlphaReq + caseULforced );
                if ( !IsHomogField(&firstFieldCmm, AlphaReq, AlphaOpt) )
                    return false;
            }
            else if( ch == 'a' )
            {
                maskIDTmp.push_back( AlphaOpt + caseULforced );
                if ( !IsHomogField(&firstFieldCmm, AlphaReq, AlphaOpt) )
                    return false;
            }
            else if( ch == 'N' )
            {
                maskIDTmp.push_back( AlpNumReq + caseULforced );
                if ( !IsHomogField(&firstFieldCmm, AlpNumReq, AlpNumOpt) )
                    return false;
            }
            else if( ch == 'n' )
            {
                maskIDTmp.push_back( AlpNumOpt + caseULforced );
                if ( !IsHomogField(&firstFieldCmm, AlpNumReq, AlpNumOpt) )
                    return false;
            }
            else if( ch == 'C' )
            {
                maskIDTmp.push_back( AnyCharReq + caseULforced );
                if ( !IsHomogField(&firstFieldCmm, AnyCharReq, AnyCharOpt) )
                    return false;
            }
            else if( ch == 'c' )
            {
                maskIDTmp.push_back( AnyCharOpt + caseULforced );
                if ( !IsHomogField(&firstFieldCmm, AnyCharReq, AnyCharOpt) )
                    return false;
            }
            else if( ch == 'X' )
            {
                maskIDTmp.push_back( HexReq + caseULforced );
                if ( !IsHomogField(&firstFieldCmm, HexReq, HexOpt) )
                    return false;
            }
            else if( ch == 'x' )
            {
                maskIDTmp.push_back( HexOpt + caseULforced );
                if ( !IsHomogField(&firstFieldCmm, HexReq, HexOpt) )
                    return false;
            }

            maskTmp << ch;
            lastIsComchars = true;
            if (ch == '#' || ch == '9')
                sForSize << '9' ;
            else
                sForSize << 'M' ;

            pos++;
            i++;
            continue;
        }

        if ( ch == '>' )
        {
            caseULforced = UppChar;
            //Don't 'close' the field. This char is not a literal
            i++;
            continue;
        }
        if ( ch == '<' )
        {
            caseULforced = LowChar;
            //Don't 'close' the field. This char is not a literal
            i++;
            continue;
        }
        if ( ch == '^' )
        {
            caseULforced = 0; //stop upper/lower forcing
            //Don't 'close' the field. This char is not a literal
            i++;
            continue;
        }

        isLiteral = true;
        //The expansion command '{' only acts on chars inside comchars wxString
        if( lastIsComchars && ch == '{' )
        {
            //If only digits are found until '}', this is an expansion command
            //Otherwise it's a literal
            wxString::const_iterator j = i;
            j++;
            wxChar ce;
            size_t nudigs = 0;
            wxString theNum;
            //Let's see the next characters
            while ( j != mask.end() )
            {
                ce = *j;
                if ( wxIsdigit( ce ) )
                {
                    nudigs++;
                    theNum << ce; //forming the number
                    j++;
                }
                else if ( ce == '}' && nudigs > 0 ) //expander finishes here
                {
                    //Convert theNum to a long. Perhaps it was "000", which
                    // is a valid but useless expander.
                    long numex = 0;
                    if ( theNum.ToCLong( &numex ) && numex > 1 )
                    {
                        //Expand previous command
                        numex--; //the first "copy" was already given
                        int lastID = maskIDTmp.back();
                        for (long nu = 0; nu < numex; nu++)
                        {
                            maskIDTmp.push_back(lastID);
                        }
                        maskTmp.Append( maskTmp.Last(), (size_t) numex );
                        if (sForSize.Last() == '9')
                            sForSize.Append('9', (size_t) numex );
                        else
                            sForSize.Append('M', (size_t) numex );

                        pos += numex;
                    }
                    i = j; //skip these already examined chars.
                    i++;
                    isLiteral = false;
                    //Don't allow consecutive expanders: {3}{6}
                    lastIsComchars = false;
                    break;
                }
                else
                {
                    break; //treat this '{' as a literal
                }
            }
        }

        //Literal treatment
        if ( isLiteral )
        {
            //When previous char was inside a field, finish it.
            if ( fieldStarted )
            {
                fieldsEnd.push_back( pos - 1 ); //Define a new field
                fieldStarted = false;
                firstFieldCmm = -1; //reset
            }

            //'|' means fields ends here, but don't show '|' as any literal
            if ( ch != '|' )
            {
                if ( ch == '\\' )
                {
                    i++; //the real literal is next char.
                    if ( i != mask.end() )
                    {
                        ch = *i;
                    }
                    else
                    {
                        wxFAIL_MSG("Incomplete escaped character");
                        return false; //'\' at end of mask
                    }
                }
                //Store in the expanded mask. Useful for 'painting' it
                if ( !wxIsprint(ch) )
                {
                    wxFAIL_MSG("Warning: Unprintable character in mask");
                }
                maskTmp << ch;
                maskIDTmp.push_back(Literal);
                sForSize << ch; //This literal has its own size. Don't use 9/M
                pos++;
            }
            i++;
            lastIsComchars = false;
        }
    }

    //If a field is still 'not closed', finish it
    if ( fieldStarted )
    {
        fieldsEnd.push_back( pos - 1 ); //Define a new field
    }

    if ( fieldsEnd.empty() )
    {
        wxFAIL_MSG("The mask must have at least one field");
        return false;
    }

    //Copy temporary objects to definitive ones
    m_ExpMask = maskTmp;
    m_maskIDs = maskIDTmp;
    m_arrPosS = fieldsBeg;
    m_arrPosF = fieldsEnd;

    //Update also at params object
    m_params.mask = mask; //User definition string, not the expanded

    //Also clear all field dependent parameters.
    //Changing the mask may change fields. We suppose that previous fields
    // parameters (choices, allowed/excluded chars, etc) are now invalid.
    m_params.ResetFields( fieldsEnd.size() );

    //Previous value in the control is lost. Let's show the new empty one
    m_filled.clear();
    m_filled.reserve( m_ExpMask.Len() );
    for (size_t j = 0; j < m_ExpMask.Len(); j++)
        m_filled.push_back(false);

    SetAllFieldsValue("");

    //Update the minimal size of the control
    sForSize << 'a'; //give it a tip, for margins

    int extraSize = 0;
#if wxUSE_COMBOBOX
    //If our control is a wxComboBox we need extra space for the drop arrow
    wxComboBox *combo = wxDynamicCast(msk_control, wxComboBox);
    if ( combo )
    {
        wxScrollBar *sb = new wxScrollBar(combo->GetParent(), wxID_ANY,
                                          wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL);
        extraSize = sb->GetSize().GetWidth();
        sb->Destroy();
    }
#endif // wxUSE_COMBOBOX

    int w, h;
    msk_control->GetTextExtent(sForSize, &w, &h);
    w += extraSize;
    //try to avoid ugliness
    if ( msk_control->GetFont().IsFixedWidth() )
        w += 3; //just a bit more
    else
        w = w * 8 / 10; //The rule of 80%/20% is worth everywhere

    msk_control->SetMinClientSize(wxSize(w, -1));

    return true;
}

//Set the colours for different cases.
void wxMaskedEdit::SetMaskedColours(const wxMaskedEditColours& colours)
{
    wxColour defFg, defBg;
    GetDefaultColours( &defFg, &defBg );

    m_params.colours = colours;
    //Use defaults for colours that have not been set yet
    if ( m_params.colours.colOKBackground == wxNullColour )
        m_params.colours.colOKBackground = defBg;
    if ( m_params.colours.colOKForeground == wxNullColour )
        m_params.colours.colOKForeground = defFg;
    if ( m_params.colours.colInvaldBackground == wxNullColour )
        m_params.colours.colInvaldBackground = defBg;
    if ( m_params.colours.colInvaldForeground == wxNullColour )
        m_params.colours.colInvaldForeground = defFg;
    if ( m_params.colours.colBlankBackground == wxNullColour )
        m_params.colours.colBlankBackground = defBg;
    if ( m_params.colours.colBlankForeground == wxNullColour )
        m_params.colours.colBlankForeground = defFg;
}

//Default colours. We need them if not all of the six colours have been set.
void wxMaskedEdit::GetDefaultColours(wxColour* defFgCol, wxColour* defBgCol) const
{
#if wxUSE_TEXTCTRL
    if ( wxTextCtrl *text = wxDynamicCast(msk_control, wxTextCtrl) )
    {
        *defBgCol = text->GetClassDefaultAttributes().colBg;
        *defFgCol = text->GetClassDefaultAttributes().colFg;
        return;
    }
#endif // wxUSE_TEXTCTRL

#if wxUSE_COMBOBOX
    if ( wxComboBox *combo = wxDynamicCast(msk_control, wxComboBox) )
    {
        *defBgCol = combo->GetClassDefaultAttributes().colBg;
        *defFgCol = combo->GetClassDefaultAttributes().colFg;
        return;
    }
#endif // wxUSE_COMBOBOX

    return;
}

//Set all fields with the same flags.
void wxMaskedEdit::SetAllFieldsFlags(const wxMaskedEditFieldFlags& flags)
{
    for (size_t i = 0; i < GetFieldsCount(); i++)
        SetFieldFlags(i, flags);
}

//Set the test function for a field.
void wxMaskedEdit::SetFieldFunction(size_t index,
                                    wxMaskedFieldFunc* fn,
                                    void* param)
{
    m_params.fieldsFuncs.at(index) = fn;
    m_params.fieldFuncsParams.at(index) = param;
}

//Set all fields with the same functions
void wxMaskedEdit::SetAllFieldsFunction(wxMaskedFieldFunc* fn, void* param)
{
    for (size_t i = 0; i < GetFieldsCount(); i++)
        SetFieldFunction(i, fn, param);
}

//Set the test function for the whole control.
void wxMaskedEdit::SetControlFunction(wxMaskedFunc* fn, void* param)
{
    m_params.controlFunc = fn;
    m_params.controlFuncParams = param;
}

//Set all parameters at once, replacing old ones
bool wxMaskedEdit::SetParams(const wxMaskedEditParams& params)
{
    //Try the mask
    if ( !SetMask(params.mask) )
        return false;

    //Set the new fields parameters
    if ( params.fieldsFlags.size() != m_params.fieldsFlags.size()
        || params.fieldsFuncs.size() != m_params.fieldsFuncs.size()
        || params.fieldFuncsParams.size() != m_params.fieldFuncsParams.size() )
    {
        wxFAIL_MSG("Wrong number of fields parameters");
        return false;
    }

    for (size_t i = 0; i < params.fieldsFlags.size(); i++)
    {
        SetFieldFlags(i, params.fieldsFlags[i]);
        SetFieldFunction(i, params.fieldsFuncs[i], params.fieldFuncsParams[i]);
    }

    SetControlFunction(params.controlFunc, params.controlFuncParams);

    //Set the colours
    SetMaskedColours(params.colours);

    return true;
}

//Set the control's value from an undecorated text, which is supposed to
// resemble user's input.
//Also annotates empty cells (characters not typed)
bool wxMaskedEdit::SetAllFieldsValue(const wxString& plainValue)
{
    //If the mask is not big enough, this plainValue does not fit in it
    //TODO: allow it with a flag?
    if ( plainValue.Len() > m_ExpMask.Len() )
        return false;

    size_t posMask;
    wxString forControl(wxChar('\0'), m_ExpMask.Len());
    wxChar ch;
    //If we return before all chars are processed, we'll restore the backup
    m_fillBck = m_filled;

    // Fill from left to right
    wxString::const_iterator iMask = m_ExpMask.begin();
    wxString::const_iterator iPlain = plainValue.begin();
    wxString::iterator iControl = forControl.begin();
    posMask = 0;
    while ( iMask != m_ExpMask.end() )
    {
        if ( m_maskIDs[posMask] == Literal )
        {
            ch = *iMask;
            m_filled[posMask] = false;
        }
        else
        {
            if ( iPlain != plainValue.end() )
            {
                ch = *iPlain;
                iPlain++;
                if ( !TestChar(&ch, posMask) ) //may force upper/lower case
                {
                    m_filled = m_fillBck; //restore
                    return false; //stop now because this char is not allowed
                }
                m_filled[posMask] = true; //mark it as 'typed'
            }
            else //use fillChar
            {
                ch = GetFieldFlags(FindField((long)posMask)).GetFillChar();
                m_filled[posMask] = false;
            }
        }
        *iControl = ch; //set the char
        iControl++;
        iMask++;
        posMask++;
    }

    //Make sure all fields are aligned
    for (size_t index = 0; index < GetFieldsCount(); index++)
        NormalizeField(index, &forControl);

    //Update the control
    GetTextEntry()->SetValue(forControl);

    return true;
}

//Like SetFieldValue(), but operates on inText, not in control
wxString wxMaskedEdit::SetFieldOnText(size_t index,
                                      const wxString& plainValue,
                                      const wxString& inText)
{
    wxString resString;

    //Does this field exist?
    if ( index >= GetFieldsCount() )
        return wxEmptyString;

    //If left aligned, fill from left, else fill from right
    int ctlPos = m_arrPosS[index];
    int endPos = m_arrPosF[index] + 1;
    int valuePos = 0;
    int valueEnd = plainValue.Len();
    int inc = 1;

    //If the field is not big enough, this plainValue does not fit in it
    //TODO allow it with a flag?
    if ( valueEnd > endPos - ctlPos )
        return wxEmptyString;

    if ( GetFieldFlags(index).GetAlignment() == wxALIGN_RIGHT )
    {
        endPos = m_arrPosS[index] - 1;
        ctlPos = m_arrPosF[index];
        inc = -1;
        valuePos = plainValue.Len() - 1;
        valueEnd = -1;
    }

    resString = inText;
    wxChar ch;
    while ( ctlPos != endPos )
    {
        if ( valuePos != valueEnd )
        {
            ch = plainValue.GetChar((size_t)valuePos);
            if ( !TestChar(&ch, ctlPos) ) //may force upper/lower case
                return wxEmptyString; //stop now because this char is not allowed

            m_filled[(size_t)ctlPos] = true; //mark it as 'typed'
            valuePos += inc;
        }
        else //plainValue is shorter than the field
        {
            ch = GetFieldFlags(index).GetFillChar();
            m_filled[(size_t)ctlPos] = false; //mark it as 'not typed'
        }

        resString[(size_t)ctlPos] = ch; //replace in the whole [control] string
        ctlPos += inc;
    }

    return resString;
}

//Set the value for a field.
bool wxMaskedEdit::SetFieldValue(size_t index, const wxString& plainValue)
{
    wxString controlValue = GetTextEntry()->GetValue();
    m_fillBck = m_filled;

    wxString cVal = SetFieldOnText(index, plainValue, controlValue);

    if ( cVal.IsEmpty() )
    {
        m_filled = m_fillBck;
        return false;
    }

    //Update the control
    GetTextEntry()->SetValue(cVal);

    return true;
}

//Like GetFieldValue(), but operates on inText, not in control
//Fill empty cells with 'chPad', if it is != '\0'
wxString wxMaskedEdit::GetFieldOnText(size_t index,
                                      const wxString& inText,
                                      wxChar chPad) const
{
    wxString resString;

    //Does this field exist?
    if ( index >= GetFieldsCount() )
        return resString;

    //Extract char by char. Fill with chPad if the cell is empty
    for (long i = m_arrPosS[index]; i <= m_arrPosF[index]; i++)
    {
        if ( !CellIsEmpty(i) )
            resString << inText[(size_t)i];
        else if ( chPad != '\0' )
            resString << chPad;
    }

    return resString;
}

//Get the current text in a field. Fill empty cells with padding-char flag
wxString wxMaskedEdit::GetFieldValue(size_t index) const
{
    return GetFieldOnText(index,
                          GetTextEntry()->GetValue(),
                          GetFieldFlags(index).GetPaddingChar() );
}

//Get control's value, without decorations.
wxString wxMaskedEdit::GetAllFieldsValue() const
{
    wxString pValue;
    wxString ctrlValue = GetTextEntry()->GetValue();
    wxString::const_iterator iT = ctrlValue.begin();
    size_t pos = 0;
    size_t field = 0;
    wxChar padCh = '\0';
    while ( iT != ctrlValue.end() )
    {
        if ( m_maskIDs[pos] != Literal )
        {
            if ( m_filled[pos] )
                pValue += *iT;
            else
            {
                //use the padding char
                field = FindField((long) pos);
                if ( field != (size_t)wxNOT_FOUND )
                {
                    padCh = GetFieldFlags(field).GetPaddingChar();
                    if ( padCh != '\0' )
                        pValue += padCh;
                }
            }
        }
        ++iT;
        ++pos;
    }

    return pValue;
}

// ---------------------------------------------------------------------------
// -  -  -  -  -  -  -  - Event handling -  -  -  -  -  -  -  -  -  -  -  -  -

//Handle the selection, returning the new possible control's value
wxString wxMaskedEdit::HandleSelection(long *pos, bool *changed)
{
    wxString newString = GetTextEntry()->GetValue();
    long selFrom, selTo;
    GetTextEntry()->GetSelection(&selFrom, &selTo);

    if (selFrom == selTo)
    {
        //No selection, nothing else to do
        *changed = false;
        return newString;
    }

    //To erase the selection we replace each cell with it's fillchar.
    //Also mark this cells as 'empty'
    size_t i = (size_t)selFrom;
    wxString::iterator it = newString.begin() + selFrom;
    while ( i < (size_t)selTo )
    {
        if ( m_maskIDs[i] != Literal && m_filled[i] )
        {
            *it = GetFieldFlags((size_t) FindField(i) ).GetFillChar();
            m_filled[i] = false;
            *changed = true;
        }
        i++;
        ++it;
    }

    if ( *changed )
        *pos = selFrom;

    return newString;
}

void wxMaskedEdit::OnKeyDown(wxKeyEvent& event)
{
    event.Skip();

    //Using shift+arrow means 'selecting'. We don't handle this.
    if ( event.HasAnyModifiers() )
    {
        if (  event.GetKeyCode() == WXK_BACK
            ||event.GetKeyCode() == WXK_DELETE
            ||event.GetKeyCode() == WXK_NUMPAD_DELETE )
        {
            //don't allow these sort of deleting
            event.Skip(false);
        }

        //Clipboard
        if ( event.GetModifiers()== wxMOD_CONTROL )
        {
            //Cutting with 'Ctrl-X'
            if (event.GetKeyCode() == 88)
            {
                event.Skip(false);
                DoCut();
            }
            //Pasting with 'Ctrl-V'
            else if(event.GetKeyCode() == 86)
            {
                event.Skip(false);
                DoPaste();
            }
        }

        //We don't handle other modifiers neither.
        return;
    }

    long carPos = GetTextEntry()->GetInsertionPoint();
    bool toNext = true;

    switch ( event.GetKeyCode() )
    {
        case WXK_LEFT:
        case WXK_NUMPAD_LEFT:
        {
            toNext = false;
            //fall through
        }
        case WXK_RIGHT:
        case WXK_NUMPAD_RIGHT:
        {
            MoveCaret(carPos, toNext);
            event.Skip(false);
            return;
        }

        case WXK_BACK:
            if ( carPos > m_arrPosS[0] )
                carPos--;
        case WXK_DELETE:
        case WXK_NUMPAD_DELETE:
        {
            long newPos = -1;
            bool shouldUpdate = false;
            //The whole control's value, changed after deleting the selection
            event.Skip(false); //no other processing

            wxString ctrString = HandleSelection(&newPos, &shouldUpdate);
            if ( newPos == -1 )
            {
                //No selection.
                size_t index = (size_t) wxNOT_FOUND;

                if ( carPos >= m_arrPosS[0] && carPos <= m_arrPosF.back() )
                {
                    index = (size_t) FindField(carPos);
                    if (index != (size_t) wxNOT_FOUND && !CellIsEmpty(carPos))
                    {
                        m_filled[carPos] = false;
                        ctrString[carPos] = GetFieldFlags((size_t)index).GetFillChar();
                        NormalizeField((size_t)index, &ctrString);
                        shouldUpdate = true;
                        if ( GetFieldFlags((size_t)index).GetAlignment() == wxALIGN_RIGHT)
                            carPos++;
                    }
                    else
                    {
                        if (event.GetKeyCode() == WXK_BACK )
                        {
                            //Backspace in an empty position, move to previous field
                            carPos = MoveCaret(++carPos, false);
                            index = (size_t)FindField(carPos);
                            if ( index != (size_t)wxNOT_FOUND
                                && GetFieldFlags((size_t)index).GetAlignment()
                                    == wxALIGN_LEFT
                                && !CellIsEmpty(carPos) )
                            {
                                carPos++;
                            }
                        }
                        else
                        {
                            //delete in an empty position, move towards right
                            carPos = MoveCaret(carPos, true);
                        }
                    }
                }
                else
                {
                    if (event.GetKeyCode() == WXK_BACK )
                        carPos = MoveCaret(++carPos, false);
                    else
                        carPos = MoveCaret(carPos, true);
                }
            }
            //with selection
            else
            {
                //done: delete the selection and set cursor at its beginning
                if (event.GetKeyCode() == WXK_BACK )
                    carPos++;
            }

            //Set value and cursor
            if ( newPos != -1 || shouldUpdate )
            {
                //Join characters and align fields
                for (size_t index = 0; index < GetFieldsCount(); index++)
                    NormalizeField(index, &ctrString);

                GetTextEntry()->SetValue(ctrString);
            }

            GetTextEntry()->SetInsertionPoint(carPos);
            break;
        }

        default:
            return;
    }
}

void wxMaskedEdit::OnChar(wxKeyEvent& event)
{
    event.Skip();

    //The char. Unicode or ASCII
    int kc = 0;
#if wxUSE_UNICODE
    kc = (int) event.GetUnicodeKey();
#else // !wxUSE_UNICODE
    kc = event.GetKeyCode();
    if ( kc >= WXK_START)
        return;
#endif // wxUSE_UNICODE/!wxUSE_UNICODE

    //Allow default 'ctrl-C' manager
    if ( kc == WXK_CONTROL_C)
        return;

    //These are non-printable characters
    if ( kc < WXK_SPACE )
        return;

    wxChar keyChar(kc);

    event.Skip(false); //This event will end here.

    //If an EVT_CHAR event was generated for these cases, don't process
    //them again. They have been already processed at OnKeyDown
    switch ( (int)keyChar )
    {
        case WXK_LEFT:
        case WXK_NUMPAD_LEFT:
        case WXK_RIGHT:
        case WXK_NUMPAD_RIGHT:
        case WXK_BACK:
        case WXK_DELETE:
        case WXK_NUMPAD_DELETE:
            return;

        default:
            break;
    }

    long caretPos = GetTextEntry()->GetInsertionPoint();

    m_fillBck = m_filled;
    //First handle the selection, if any
    long selPos = -1; //changes if a selection erases anything
    bool needsUpdate = false;
    //The whole control's value, changed after deleting the selection;
    // and the new caret position.
    wxString workString = HandleSelection(&selPos, &needsUpdate);
    if ( selPos != -1 )
        caretPos = selPos;

    bool isProccesed = ProcessChar(keyChar, &caretPos, &workString, &needsUpdate);

    //Update if necessary
    if ( isProccesed )
    {
        //If there was no selection and the typed char is a literal, we only
        // move the caret, without useless SetValue().
        //And avoid so the false wxEVT_COMMAND_TEXT_UPDATED event.
        if ( needsUpdate || selPos != -1)
            GetTextEntry()->SetValue(workString);

        //May jump to next field
        size_t inField = (size_t) FindField(caretPos);
        if ( inField != (size_t) wxNOT_FOUND && caretPos == m_arrPosF[inField] && !HasFieldRoom(inField) )
        {
            long defPos = PosForFieldNext(caretPos);
            //If we have inserted at last position, keep the caret at its end.
            if( defPos == wxNOT_FOUND )
            {
                defPos = m_arrPosF[inField] + 1;
            }
            GetTextEntry()->SetInsertionPoint(defPos);
        }
        else
        {
            MoveCaret(caretPos, true);
        }
    }
    else
    {
        //restore
        m_filled = m_fillBck;
#if wxUSE_ACCESSIBILITY
        //beep
        DoBell();
#endif
    }
}

void wxMaskedEdit::OnCut(wxCommandEvent& event)
{
    event.Skip(false);
    DoCut();
}

void wxMaskedEdit::OnPaste(wxCommandEvent& event)
{
    event.Skip(false);
    DoPaste();
}

#if wxUSE_COMBOBOX
void wxMaskedEdit::OnComboSelect(wxCommandEvent& event)
{
    //We must process this new value (test, tell empty cells, etc)
    SetMaskedValue(event.GetString());
    UpdateMaskedColours();
    GetTextEntry()->SetInsertionPoint(0);

    event.Skip(true);
}

void wxMaskedEdit::OnComboDrop(wxCommandEvent& event)
{
    //Neither OK nor Wr[ong] colours are good here. Use colours for an empty value
    UseColours(BlankColour);

    event.Skip(true);
}
#endif // wxUSE_COMBOBOX

// ---------------------------------------------------------------------------
// -  -  -  -  -  -  -  - More helpers -  -  -  -  -  -  -  -  -  -  -  -  -

//Our control derives from wxTextEntry, but not this class. Get a pointer to it
wxTextEntry* wxMaskedEdit::GetTextEntry() const
{
#if wxUSE_TEXTCTRL
    if ( wxTextCtrl *text = wxDynamicCast(msk_control, wxTextCtrl) )
        return text;
#endif // wxUSE_TEXTCTRL

#if wxUSE_COMBOBOX
    if ( wxComboBox *combo = wxDynamicCast(msk_control, wxComboBox) )
        return combo;
#endif // wxUSE_COMBOBOX

    wxFAIL_MSG("Can only be used with wxTextCtrl or wxComboBox");

    return NULL;
}

//Update colours, depending on empty/valid/invalid cases
void wxMaskedEdit::UpdateMaskedColours()
{
    //On an empty value, there are also defined colours
    bool isEmpty = true;
    for (size_t i = 0; i < m_filled.size(); i++)
    {
        if ( m_filled[i] )
        {
            isEmpty = false;
            break;
        }
    }

    if ( isEmpty )
        UseColours(BlankColour);
    else if ( GetInvalidFieldIndex() == -1 )
        UseColours(OKColour);
    else
        UseColours(InvalidColour);

}

//Join characters and align the field. inText is the whole control's value.
void wxMaskedEdit::NormalizeField(size_t index, wxString* inText)
{
    //Extract the field. Passing '\0' as filling char give us the typed value
    wxString sField = GetFieldOnText(index, *inText, '\0');

    //Do nothing on an empty field
    if ( sField.IsEmpty() )
        return;
    //If the field is full filled, do nothing
    size_t nToFill = m_arrPosF[index] - m_arrPosS[index] + 1;
    if (sField.Len() == nToFill)
        return;


    size_t pos = m_arrPosS[index];
    wxString::iterator iT = inText->begin() + pos;
    wxChar fillChar = GetFieldFlags(index).GetFillChar();
    wxString::const_iterator isF = sField.begin();

    //set empty cells at left
    if ( GetFieldFlags(index).GetAlignment() == wxALIGN_RIGHT )
    {
        while ( nToFill > sField.Len() )
        {
            *iT = fillChar;
            m_filled[pos] = false;
            ++iT;
            ++pos;
            nToFill--;
        }
    }
    //Fill the cells
    while ( nToFill > 0 )
    {
        if ( isF == sField.end() )
        {
            *iT = fillChar;
            m_filled[pos] = false;
        }
        else
        {
            //This moved char must agree with mask. The field is homogeneous,
            // so we know it is allowed. Set it to upper/lower case.
            *iT = ToUppLowCase(*isF, pos);
            m_filled[pos] = true;
            ++isF;
        }
        ++iT;
        ++pos;
        nToFill--;
    }
}

//If each cell is filled with a [typed] char or it is empty.
//This information is stored in 'm_filled'
/* For a field with 10 alpha chars (e.g. a 'Name' field) and being the fillchar
  the typical blank (space), the 'real' control is initially filled with
  10 spaces, so it can be shown in the screen.
  When we call wxTextEntry::GetValue() we can't tell if those spaces are typed
  or if they are there because of initial filling.
  Carefully handling 'm_filled', we are aware of typed/filled chars.
*/
bool wxMaskedEdit::HasFieldRoom(size_t index) const
{
    //m_arrPosS/m_arrPosF are where field starts/finishes
    for (long i = m_arrPosS[index]; i <= m_arrPosF[index]; i++)
    {
        //If we find an empty cell, the field has room
        if ( CellIsEmpty(i) )
            return true;
    }

    //No free cell found
    return false;
}

//Find the field in which the cursor is.
//For a position like "_23|-__" with mask "###-aa" it will return false, even
// this is a valid position for typing the next char in a right aligned field.
// GuessField() is more convenient for that case.
int wxMaskedEdit::FindField(long curpos) const
{
    if ( curpos >= 0 )
    {
        for (size_t i = 0; i < m_arrPosS.size(); i++)
        {
            //Is the cursor between start and finish of a field?
            if ( m_arrPosS[i] <= curpos && curpos <= m_arrPosF[i] )
            {
                return (int)i; //found
            }
        }
    }

    return wxNOT_FOUND; //not in a field
}

//Find the field to insert a char in the given position.
int wxMaskedEdit::GuessField(long pos) const
{
    int index = FindField(pos);

    //Perhaps we are just past the right most cell in a right aligned field
    //For an insane mask as "###|AAA" with the first field right aligned, if
    // the caret is just between both fields, which field do we get? The first
    // one if it has room; otherwise, the second.
    int prevIndex = FindField( pos - 1 );
    if ( prevIndex != wxNOT_FOUND
        && GetFieldFlags((size_t)prevIndex).GetAlignment() == wxALIGN_RIGHT
        && HasFieldRoom((size_t)prevIndex) )
    {
        index = prevIndex;
    }

    return index;
}

//A reasonable position for caret in the previous field to where the caret is.
long wxMaskedEdit::PosForFieldPrev(long curpos) const
{
    //Backwards search the first field that ends before the cursor position
    size_t iField = m_arrPosF.size();
    while ( iField > 0 )
    {
        if ( m_arrPosF[iField - 1] < curpos )
        {
            break;
        }
        iField--;
    }
    if ( iField == 0 )
    {
        //There is no field at left of the caret
        return wxNOT_FOUND;
    }

    iField--;
    long pos = m_arrPosF[iField];
    if ( GetFieldFlags(iField).GetAlignment() == wxALIGN_LEFT )
    {
        if ( CellIsEmpty(pos) )
        {
            while (pos >= m_arrPosS[iField] && CellIsEmpty(pos) )
                pos--;
            pos++; //we have gone 1 char beyond
        }
    }
    else
    {
        pos++; //e.g. "__123|"

        //For a mask like "###-###|AA" we can have the case "__1-___FG".
        //If the caret is at left of 'F' "__1-___|FG" and the user press
        // left arrow, we must skip the second field (because it's empty)
        // and move to the previous field.
        if ( pos == curpos )
        {
            if ( CellIsEmpty(pos - 1) ) //empty field
            {
                long ppos = PosForFieldPrev(pos - 1);
                if ( ppos != wxNOT_FOUND )
                    pos = ppos;
            }
            else
                pos--; //from "__123|" to "__12|3"
        }
    }

    return pos;
}

//A reasonable position for caret in the next field to where the caret is.
long wxMaskedEdit::PosForFieldNext(long curpos) const
{
    if ( curpos < 0 )
        return wxNOT_FOUND;

    //Search the first field that begins after the cursor position
    size_t iField = 0;
    while ( iField < m_arrPosS.size() )
    {
        if ( m_arrPosS[iField] > curpos )
        {
            break;
        }
        iField++;
    }
    if ( iField == m_arrPosS.size() )
    {
        //The cursor is at last field, or beyond it
        return wxNOT_FOUND;
    }

    long pos = m_arrPosS[iField];
    if ( GetFieldFlags(iField).GetAlignment() == wxALIGN_RIGHT )
    {
        while (pos <= m_arrPosF[iField] && CellIsEmpty(pos) )
            pos++;
    }

    return pos;
}

//Move caret from current position to the foreseeable one.
//In order this to work, alignment on fields values must be previously done.
long wxMaskedEdit::MoveCaret(long curPos, bool toRight)
{
    long defPos = toRight ? curPos + 1 : curPos - 1;

    //Check bounds
    //not before the first cell of the first field
    //not after the last cell of the last field
    if ( defPos < m_arrPosS.front() || defPos > m_arrPosF.back() )
    {
        if (defPos < m_arrPosS.front())
            defPos = m_arrPosS.front();
        else
            defPos = m_arrPosF.back() + 1; //last cell at left of caret

        //Move the caret
        GetTextEntry()->SetInsertionPoint(defPos);
        return defPos;
    }

    //current field
    int inField = FindField(curPos);

    //All possible position/action cases
    if ( inField != wxNOT_FOUND )
    {
        //defPos may be changed to another field.
        //Either define the new position or look for a new field
        if ( GetFieldFlags((size_t)inField).GetAlignment() == wxALIGN_LEFT )
        {
            if ( toRight )
            {
                if ( CellIsEmpty(curPos) )
                {
                    defPos = PosForFieldNext(curPos);
                }
            }
            else //towards left in a left aligned field
            {
                if ( curPos == m_arrPosS[(size_t)inField] )
                {
                    defPos = PosForFieldPrev(curPos);
                }
                else
                {
                    //case like "ab_|_"  move to "ab|__"
                    //case like "_|___"  move to "|____"
                    if ( CellIsEmpty(defPos) )
                    {
                        while (defPos >= m_arrPosS[(size_t)inField]
                               && CellIsEmpty(defPos) )
                        {
                            defPos--;
                        }
                        defPos++;
                    }
                }
            }
        }

        else //we are in a right aligned field
        {
            if ( !toRight )
            {
                if ( curPos == m_arrPosS[(size_t)inField] //at start
                    || CellIsEmpty(defPos) ) //at left (still in field) is empty
                {
                    defPos = PosForFieldPrev(curPos);
                }
                //else defPos already defined
            }
            else //towards right in a right aligned field
            {
                //case like "_|__45"  move to "___|45"
                //case like "_|____"  move to "_____|"
                while (defPos <= m_arrPosF[(size_t)inField]
                        && CellIsEmpty(defPos) )
                    defPos++;
            }
        }
    }

    else // not in a field
    {
        defPos = toRight ? PosForFieldNext(curPos) : PosForFieldPrev(curPos);
    }

    //We have not found where to go
    if ( defPos == wxNOT_FOUND )
    {
        return curPos; //don't move
    }

    //Move the caret
    GetTextEntry()->SetInsertionPoint(defPos);

    return defPos;
}

//Insert a char at an intended position. Returns false if insertion is not
// possible (invalid char or no room available)
//'pos' is replaced with the effective insertion point.
bool wxMaskedEdit::InsertAtPos(wxChar iChar, long* pos, size_t index,
                               wxString* inText)
{
    /* We are asked to insert 'iChar' character at '*pos' position in
       field 'index'.

       Insertion is made in the current position. It's field is properly
       re-aligned. If the field was full, the char is still inserted, which
       means that the rest of chars are right-shifted. If this shifting is not
       possible (i.e. some char doesn't agree with the mask or there is no
       room in the fields affected), then nothing is done.
    */

    if ( index == (size_t)wxNOT_FOUND )
        return false;

    long ePos = *pos;

    //Because of 'HandleSelection()' we may be trying to insert 'x' in a
    // current field value like "__|_ab__". The final result should be
    // "x|ab____". The tricky part is the new caret position. We deal with
    //it counting empty skipped cells.
    long emptyCount = 0;

    bool isLeftAlign = false;
    if ( GetFieldFlags(index).GetAlignment() == wxALIGN_LEFT )
        isLeftAlign = true;
    for (long gpos = m_arrPosS[index]; gpos <= m_arrPosF[index]; gpos++)
    {
        if (isLeftAlign && gpos < ePos && CellIsEmpty(gpos) )
            emptyCount++;
        else if (!isLeftAlign && gpos >= ePos && CellIsEmpty(gpos) )
            emptyCount++;
    }
    //The good position to insert the char in a field with all chars joined
    ePos += isLeftAlign ? -emptyCount : emptyCount;

    //The current filled cells
    wxString fieldText = GetFieldOnText(index, *inText, '\0');

    //find the place to insert at
    if ( isLeftAlign )
        ePos = ePos - m_arrPosS[index];
    else
        ePos = ePos - m_arrPosF[index] + fieldText.Len() - 1;

    //set the char
    fieldText.insert((size_t)ePos, 1, iChar);

    //If the field is full, we will "push" a char to the next field.
    wxChar pushedChar = '\0';

    //Is shifting needed?
    if ( (long)fieldText.Len() > m_arrPosF[index] - m_arrPosS[index] + 1 )
    {
        //Nowhere to push
        if (index == GetFieldsCount() - 1)
            return false;

        pushedChar = fieldText.Last();
        fieldText.RemoveLast();
    }

    //Setting the field also tests the char and does upper/lower conversion
    wxString str = SetFieldOnText(index, fieldText, *inText);
    if ( ! str.IsEmpty() )
    {
        //The real caret position where we have inserted
        //We can not presume the caret is at a "good" position, because
        // the user can click anywhere. But we know where we have inserted.
        if ( !isLeftAlign )
        {
            //MoveCaret() will move it towards right. We want to stay here
            *pos = ePos - fieldText.Len() + m_arrPosF[index] + 1;
        }
        else
            *pos = ePos + m_arrPosS[index];
    }

    else
        return false; //this char is not allowed here

    //Shifting
    if ( pushedChar == '\0' )
    {
        *inText = str;
        return true;
    }

    index++;
    while ( pushedChar != '\0' && index < GetFieldsCount() )
    {
        fieldText = GetFieldOnText(index, str, '\0');
        fieldText.insert(0, 1, pushedChar);
        if ( (long)fieldText.Len() > m_arrPosF[index] - m_arrPosS[index] + 1 )
        {
            //Nowhere to push
            if (index == GetFieldsCount() - 1)
                return false;

            pushedChar = fieldText.Last();
            fieldText.RemoveLast();
        }
        else
            pushedChar = '\0';

        str = SetFieldOnText(index, fieldText, wxString(str));
        if ( str.IsEmpty() )
            return false;

        index++;
    }

    *inText = str;
    return true;
}

//Process a char, coming from OnChar() or from DoPaste()
//It may return false because the char is not allowed or because there is not
// enough room for it.
//It may return true because it is inserted, or because it means 'move the caret'
bool wxMaskedEdit::ProcessChar(wxChar aChar, long* pos, wxString* inText, bool* changed)
{
    bool isProccesed = false;
    long nPos = *pos;

    //The field in which the char would be inserted.
    size_t index = (size_t) GuessField(nPos);
    if ( index == (size_t)wxNOT_FOUND && nPos < m_arrPosS.back() )
    {
        //We have one or more fields towards right.
        //Test this char for coincidence with a literal at this position
        if ( aChar == m_ExpMask[nPos] )
        {
            isProccesed = true;
            //a position in next field
            nPos = PosForFieldNext(nPos);
            nPos--;
        }
    }

    //The char must be valid before we insert it.
    //caret pos may be just at right of a right aligned field, beyond the
    // position where this field finishes. Instead, because the field is
    // homogeneous, we can test the char against any position in the field.
    if ( !isProccesed && index != (size_t)wxNOT_FOUND )
    {
        if ( TestChar(&aChar, m_arrPosF[index]) )
        {
            //Try to insert the char
            if ( InsertAtPos(aChar, &nPos, index, inText) )
            {
                *changed = true;
                isProccesed = true;
            }
        }

        else
        {
            //If the char is the same as first char of next literal, this means
            // that the user wants to move to next field
            size_t nextLitPos = (size_t) m_arrPosF[index] + 1;
            if ( nextLitPos < m_ExpMask.Len()
                && m_maskIDs[nextLitPos] == Literal
                && aChar == m_ExpMask[nextLitPos] )
            {
                nPos = nextLitPos;
                isProccesed = true;
            }
        }
    }

    //Try inserting in next field
    if ( !isProccesed
        && index == (size_t)wxNOT_FOUND
        && nPos < m_arrPosS.back() )
    {
        nPos = PosForFieldNext(nPos);
        index = GuessField(nPos);
        //Try to insert the char
        if ( InsertAtPos(aChar, &nPos, index, inText) )
        {
            *changed = true;
            isProccesed = true;
        }
    }

    if ( isProccesed )
        *pos = nPos;

    return isProccesed;
}

//Cutting
void wxMaskedEdit::DoCut()
{
    if ( ! GetTextEntry()->CanCut() )
        return;

    long selFrom, selTo;
    GetTextEntry()->GetSelection(&selFrom, &selTo);

    //Get the string to cut
    wxString cutStr = ( GetTextEntry()->GetValue() )
                   .substr((size_t)selFrom, (size_t)(selTo - selFrom));

    //Pass it to the clipboard
    if ( wxTheClipboard->Open() )
    {
        wxTheClipboard->SetData( new wxTextDataObject(cutStr) );
        wxTheClipboard->Close();
    }

    //Update in our control
    long pos = -1;
    bool changed = false;
    GetTextEntry()->SetValue( HandleSelection(&pos, &changed) );
    if ( changed && pos >=  0 )
        GetTextEntry()->SetInsertionPoint(pos);
}

//Pasting
void wxMaskedEdit::DoPaste()
{
    if ( ! GetTextEntry()->CanPaste() )
        return;

    wxString pasteStr;
    if (wxTheClipboard->Open())
    {
        if (wxTheClipboard->IsSupported( wxDF_TEXT ))
        {
            wxTextDataObject data;
            wxTheClipboard->GetData( data );
            pasteStr = data.GetText();
        }
        wxTheClipboard->Close();
    }

    if ( pasteStr.IsEmpty() )
        return;

    long pos = GetTextEntry()->GetInsertionPoint();
    bool somethingProcessed = false;
    //Backup copy. We'll restore it if this paste fails.
    m_fillBck = m_filled;
    //First handle the selection, if any
    long selPos = -1; //changes if a selection erases anything
    bool valueChanged = false;
    //The whole control's value, changed after deleting the selection;
    // and the new caret position.
    wxString workString = HandleSelection(&selPos, &valueChanged);
    if ( selPos != -1 )
        pos = selPos;

    //Insert char by char, as if they were typed
    wxString::const_iterator iT = pasteStr.begin();
    while ( iT != pasteStr.end() )
    {
        if ( ProcessChar(*iT, &pos, &workString, &valueChanged) )
        {
            somethingProcessed = true;
            pos = MoveCaret(pos, true);
            //new backup
            m_fillBck = m_filled;
        }

        iT++;
    }

    //Some chars have been inserted and/or the caret has been moved
    if ( somethingProcessed )
    {
        if ( valueChanged || selPos != -1)
        {
            GetTextEntry()->SetValue(workString);
            GetTextEntry()->SetInsertionPoint(pos);
        }
    }
    else //nothing was accepted. Nothing to change, not even the selection, if any
    {
        //restore
        m_filled = m_fillBck;
#if wxUSE_ACCESSIBILITY
        //beep
        DoBell();
#endif
    }
}

//Set the proper string for SetValue/ChangeValue
wxString wxMaskedEdit::SetMaskedValue(const wxString& value)
{
    //'value' is supposed to be well masked (i.e. with decorations)
    //We make sure of it, and also test each char against the mask,
    // and fill the 'typed cells' vector.

    size_t pos = 0;
    size_t maxLen = m_ExpMask.Len();
    wxString maskedValue('\0', maxLen);

    wxString::const_iterator iV = value.begin();
    wxString::const_iterator iM = m_ExpMask.begin() + pos;
    wxString::iterator iMV = maskedValue.begin() + pos;
    wxChar ch = '\0';

    while (pos < maxLen)
    {
        if ( m_maskIDs[pos] == Literal )
        {
            *iMV = *iM;
            m_filled[pos] = false;
        }

        else
        {
            ch = '\0';

            if ( iV != value.end() )
                ch = *iV;

            if ( !TestChar(&ch, (long)pos) )
            {
                //This char is not allowed. Use fillChar instead
                size_t index = FindField((long)pos);
                ch = GetFieldFlags(index).GetFillChar();
                m_filled[pos] = false;
            }
            else
                m_filled[pos] = true;

            *iMV = ch;
        }

        pos++;
        ++iMV;
        ++iM;
        if ( iV != value.end() )
            ++iV;
    }

    //Make sure all fields are aligned, and all chars in proper upper/lower case
    for (size_t index = 0; index < GetFieldsCount(); index++)
        NormalizeField(index, &maskedValue);

    return maskedValue;
}

//Show the colours
void wxMaskedEdit::UseColours(int whatColours)
{
    if (whatColours == BlankColour)
    {
        msk_control->SetBackgroundColour(m_params.colours.colBlankBackground);
        msk_control->SetForegroundColour(m_params.colours.colBlankForeground);
    }

    else if (whatColours == OKColour)
    {
        msk_control->SetBackgroundColour(m_params.colours.colOKBackground);
        msk_control->SetForegroundColour(m_params.colours.colOKForeground);
    }

    else
    {
        msk_control->SetBackgroundColour(m_params.colours.colInvaldBackground);
        msk_control->SetForegroundColour(m_params.colours.colInvaldForeground);
    }

    msk_control->Refresh();
}


// ---------------------------------------------------------------------------
// -  -  -  -  -  -  -  - General Test Functions -  -  -  -  -  -  -  -  -

//Character vs mask commands. 'pos' is the cursor position, which corresponds
// to expanded mask position.
//We don't do forced upper/lower conversion here. Should be done before this.
//Note these wxIsxxx functions work with current locale.
bool wxMaskedEdit::CharAgreesWithMask(wxChar testChar, long pos) const
{
    switch ( GetMaskedType( (size_t)pos ) )
    {
        case DigitReq :
        case DigitOpt :
            return wxIsdigit( testChar );
        case AlphaReq :
        case AlphaReq + UppChar :
        case AlphaReq + LowChar :
        case AlphaOpt :
        case AlphaOpt + UppChar :
        case AlphaOpt + LowChar :
            return wxIsalpha( testChar );
        case AlpNumReq :
        case AlpNumReq + UppChar :
        case AlpNumReq + LowChar :
        case AlpNumOpt :
        case AlpNumOpt + UppChar :
        case AlpNumOpt + LowChar :
            return wxIsalnum( testChar );
        case AnyCharReq :
        case AnyCharReq + UppChar :
        case AnyCharReq + LowChar :
        case AnyCharOpt :
        case AnyCharOpt + UppChar :
        case AnyCharOpt + LowChar :
            // iswprint() gets wrong with Unicode, at least in MSW
            return true;
        case HexReq :
        case HexReq + UppChar :
        case HexReq + LowChar :
        case HexOpt :
        case HexOpt + UppChar :
        case HexOpt + LowChar :
            return wxIsxdigit( testChar );

        default :
            return false;
    }
}

//Return the char converted to upper/lower case according with the mask
wxChar wxMaskedEdit::ToUppLowCase(wxChar ch, size_t pos)
{
    int mktype = GetMaskedType( pos );
    //Forced uppercase types are normal + mUppChar
    //Forced lowercase types are normal + mLowChar
    if ( mktype > UppChar && mktype < LowChar )
    {
        return wxToupper( ch );
    }
    else if ( mktype > LowChar )
    {
        return wxTolower( ch );
    }
    return ch;
}

//Test the character.
//'tryChar' is received from EVT_CHAR, SetAllFieldsValue, SetFieldValue, etc.
//We change it to upper/lower if mask tells it so and it is accepted.
bool wxMaskedEdit::TestChar(wxChar* tryChar, long pos)
{
    int index = FindField( pos );
    if ( index == wxNOT_FOUND )
    {
        //The cursor is not in a field.
        return false;
    }

    //Null char is never allowed
    if ( *tryChar == '\0' )
        return false;

    //If it is the char we use to fill empty cells, don't allow it
    //Not doing so will make m_filled[] really wrong.
    if ( *tryChar == GetFieldFlags((size_t)index).GetFillChar() )
        return false;

    //TODO: Other test
    //allowed / excluded chars

    //Convert it upper/lower case, if demanded
    wxChar candidateChar = ToUppLowCase(*tryChar, (size_t) pos );

    //Does this char match the mask?
    if ( CharAgreesWithMask(candidateChar, pos) )
    {
        *tryChar = candidateChar;
        return true;
    }

    return false;
}

//Tell if field passed all of its tests
long wxMaskedEdit::IsFieldValid(size_t index) const
{
    //TODO: choices validation

    //We have already done TestChar() for every char, so we know they agree
    // with the mask, and other tests.
    //We check here whether all required chars are in their cells.
    for (long i = m_arrPosS[index]; i <= m_arrPosF[index] ; i++)
    {
        int masktype = m_maskIDs[(size_t)i];
        //remove upper/lower forced
        if (masktype > UppChar)
            masktype -= UppChar;
        if (masktype > LowChar)
            masktype -= LowChar;
        //check
        if ( ( masktype == DigitReq || masktype == AlphaReq
            || masktype == AlpNumReq || masktype == AnyCharReq
            || masktype == HexReq ) && !m_filled[(size_t)i] )
        {
            return i - m_arrPosS[index]; //error position
        }
    }

    //Validation function for this field, if any
    wxMaskedFieldFunc *fn = m_params.fieldsFuncs[index];
    if ( fn )
        return fn(this, index, m_params.fieldFuncsParams[index]);

    return -1;
}

//Check fields and the whole control's value
long wxMaskedEdit::GetInvalidFieldIndex() const
{
    //Don't do validation if we have no fields at all
    size_t nuFields = GetFieldsCount();
    if ( nuFields < 1 )
        return 0;

    //Test all fields
    for (size_t i = 0; i < nuFields; i++)
    {
        long res = IsFieldValid(i);
        if ( res >= 0 )
            return res + m_arrPosS[i];
    }

    //Test the whole value
    if ( m_params.controlFunc )
        return m_params.controlFunc(this, m_params.controlFuncParams);

    return -1;
}


// ---------------------------------------------------------------------------
// -  -  -  -  -  -  -  - Predefined Test Functions -  -  -  -  -  -  -  -  -

//If we want to set the cursor in the position where an error is found, we
//return this position. Returning '-1' means that no error has been found.

//Range check. we handle two cases: allow only inside the range or outside it.
long wxMaskedRangeCheck(const wxMaskedEdit* caller, size_t index, void* params)
{
    if ( !params )
        return 0;

    wxRangeParams *rparms = (wxRangeParams*) params;
    long vmin = rparms->rmin;
    long vmax = rparms->rmax;
    int base = (rparms->base >= 0 && rparms->base < 37) ? rparms->base : 10;

    //Current field value
    wxString sval = caller->GetFieldValue(index);
    long val;
    if ( !sval.ToCLong(&val, base) )
        return 0; //beginning position in this field

    if (//Normal case: test vmin <= val <= vmax
        ( vmin <= vmax && vmin <= val && val <= vmax )
        || //Excluded range
        ( vmin > vmax && (val < vmin || val > vmax) ) )
    {
        return -1;
    }

    //value not in range
    return 0;
}

//Date check
long wxMaskedDateShort(const wxMaskedEdit* caller, void* params)
{
    wxDateParams *dparams = (wxDateParams*) params;
    //Get data
    wxString str = caller->GetFieldValue(dparams->dayField);
    long day, month, year;
    if ( !str.ToCLong(&day) )
        return 0;
    str = caller->GetFieldValue(dparams->monthField);
    if ( !str.ToCLong(&month) )
        return 0;
    str = caller->GetFieldValue(dparams->yearField);
    if ( !str.ToCLong(&year) )
        return 0;

    //Test day and month
    if ( (day > 31 || day < 1 || month > 12 || month < 1)
        || ((month == 4 || month == 6 || month == 9 || month == 11) && day > 31)
        || ( month == 2 && day > 29) )
    {
        return 0;
    }

    //Test leap year
    if ( day == 29 && month == 2
        && !(year % 400 == 0 || ((year % 4) == 0 && (year % 100) > 0) ) )
    {
        return 0;
    }

    //OK
    return -1;
}

void wxMaskedEditText::MaskPostInit()
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
}

#endif // wxUSE_MASKED_EDIT
