/////////////////////////////////////////////////////////////////////////////
// Name:        bmpbutton.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __BMPBUTTONH__
#define __BMPBUTTONH__

#ifdef __GNUG__
#pragma interface
#endif

// ----------------------------------------------------------------------------
// wxBitmapButton
// ----------------------------------------------------------------------------

class wxBitmapButton: public wxBitmapButtonBase
{
public:
    wxBitmapButton();
    wxBitmapButton(wxWindow *parent,
                   wxWindowID id,
                   const wxBitmap& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxBU_AUTODRAW,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxButtonNameStr)
    {
        Create(parent, id, bitmap, pos, size, style, validator, name);
    }
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmap& bitmap,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxBU_AUTODRAW,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxButtonNameStr);

    virtual void SetDefault();
    virtual bool Enable(bool enable = TRUE);
    
    void SetLabel( const wxString &label );
    wxString GetLabel() const;
    virtual void SetLabel( const wxBitmap& bitmap ) { SetBitmapLabel(bitmap); }
    
    // implementation
    // --------------
  
    void HasFocus();
    void NotFocus();
    void StartSelect();
    void EndSelect();
    void SetBitmap();
    void ApplyWidgetStyle();    
  
    bool         m_hasFocus;
    bool         m_isSelected;
  
protected:
    virtual void OnSetBitmap();

private:
    DECLARE_DYNAMIC_CLASS(wxBitmapButton)
};

#endif // __BMPBUTTONH__
