/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.h
// Purpose:     documentation for wxStaticText class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxStaticText
    @wxheader{stattext.h}
    
    A static text control displays one or more lines of read-only text.
    
    @beginStyleTable
    @style{wxALIGN_LEFT}:
           Align the text to the left
    @style{wxALIGN_RIGHT}:
           Align the text to the right
    @style{wxALIGN_CENTRE}:
           Center the text (horizontally)
    @style{wxST_NO_AUTORESIZE}:
           By default, the control will adjust its size to exactly fit to the
           size of the text when  SetLabel is called. If this style flag is
           given, the control will not change its size (this style is
           especially useful with controls which also have wxALIGN_RIGHT or
           CENTER style because otherwise they won't make sense any longer
           after a call to SetLabel)
    @style{wxST_ELLIPSIZE_START}:
           If the text width exceeds the control width, replace the beginning
           of the text with an ellipsis
    @style{wxST_ELLIPSIZE_MIDDLE}:
           Same as above, but replace the text in the middle of the control
           with an ellipsis
    @style{wxST_ELLIPSIZE_END}:
           Same as above, but replace the end of the text with an ellipsis
    @style{wxST_MARKUP}:
           Support markup in the label; see SetLabel for more information
    @endStyleTable
    
    @library{wxcore}
    @category{ctrl}
    @appearance{statictext.png}
    
    @seealso
    wxStaticBitmap, wxStaticBox
*/
class wxStaticText : public wxControl
{
public:
    //@{
    /**
        Constructor, creating and showing a text control.
        
        @param parent 
        Parent window. Should not be @NULL.
        
        @param id 
        Control identifier. A value of -1 denotes a default value.
        
        @param label 
        Text label.
        
        @param pos 
        Window position.
        
        @param size 
        Window size.
        
        @param style 
        Window style. See wxStaticText.
        
        @param name 
        Window name.
        
        @sa Create()
    */
    wxStaticText();
        wxStaticText(wxWindow* parent, wxWindowID id,
                     const wxString& label,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = 0,
                     const wxString& name = "staticText");
    //@}

    /**
        Creation function, for two-step construction. For details see wxStaticText().
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = "staticText");

    /**
        Returns the contents of the control.
        
        Note that the returned string contains both the mnemonics (@c  characters),
        if any, and markup tags, if any.
        
        Use GetLabelText() if only the
        label text is needed.
    */
    wxString GetLabel();

    //@{
    /**
        The first form returns the control's label without the mnemonics characters (if
        any) 
        and without the markup (if the control has @c wxST_MARKUP style).
        
        The second (static) version returns the given @e label string without the
        mnemonics 
        characters (if any) and without the markup.
    */
    wxString GetLabelText();
        static wxString GetLabelText(const wxString& label);
    //@}

    /**
        Sets the static text label and updates the controls size to exactly fit the
        label unless the control has wxST_NO_AUTORESIZE flag.
        
        This function allows to set decorated static label text on platforms which
        support it (currently only GTK+ 2). For the other platforms, the markup is
        ignored.
        
        The supported tags are:
        
        
        b
        
        
        bold text
        
        big
        
        
        bigger text
        
        i
        
        
        italic text
        
        s
        
        
        strike-through text
        
        sub
        
        
        subscript text
        
        sup
        
        
        superscript text
        
        small
        
        
        smaller text
        
        tt
        
        
        monospaced text
        
        u
        
        
        underlined text
        
        span
        
        
        generic formatter tag; see Pango Markup for more information.
        
        Note that the string must be well-formed (e.g. all tags must be correctly
        closed)
        otherwise it can be not shown correctly or at all.
        
        Also note that you need to escape the following special characters:
        
        
        @b Special character
        
        
        @b Escape as
        
        @c 
        
        
        @c amp; or as @c 
        
        @c '
        
        
        @c apos;
        
        @c "
        
        
        @c quot;
        
        @c 
        
        
        @c lt;
        
        @c 
        
        
        @c gt;
        
        The non-escaped ampersand @c  characters are interpreted as
        mnemonics; see wxControl::SetLabel.
        Example:
        
        @param label 
        The new label to set. It may contain newline characters and the markup tags
        described above.
    */
    virtual void SetLabel(const wxString& label);

    /**
        This functions wraps the controls label so that each of its lines becomes at
        most @e width pixels wide if possible (the lines are broken at words
        boundaries so it might not be the case if words are too long). If @e width
        is negative, no wrapping is done.
        
        This function is new since wxWidgets version 2.6.2
    */
    void Wrap(int width);
};
