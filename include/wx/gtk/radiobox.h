/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/radiobox.h
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_RADIOBOX_H_
#define _WX_GTK_RADIOBOX_H_

#include "wx/bitmap.h"

#include <vector>

class wxGTKRadioButtonInfo;

//-----------------------------------------------------------------------------
// wxRadioBox
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxRadioBox : public wxControl,
                                    public wxRadioBoxBase
{
public:
    // ctors and dtor
    wxRadioBox();
    wxRadioBox(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int n = 0,
               const wxString choices[] = (const wxString *) nullptr,
               int majorDim = 0,
               long style = wxRA_SPECIFY_COLS,
               const wxValidator& val = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxRadioBoxNameStr));

    wxRadioBox(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos,
               const wxSize& size,
               const wxArrayString& choices,
               int majorDim = 0,
               long style = wxRA_SPECIFY_COLS,
               const wxValidator& val = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxRadioBoxNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0,
                const wxString choices[] = (const wxString *) nullptr,
                int majorDim = 0,
                long style = wxRA_SPECIFY_COLS,
                const wxValidator& val = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxRadioBoxNameStr));
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                int majorDim = 0,
                long style = wxRA_SPECIFY_COLS,
                const wxValidator& val = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxRadioBoxNameStr));

    virtual ~wxRadioBox();


    // implement wxItemContainerImmutable methods
    virtual unsigned int GetCount() const override;

    virtual wxString GetString(unsigned int n) const override;
    virtual void SetString(unsigned int n, const wxString& s) override;

    virtual void SetSelection(int n) override;
    virtual int GetSelection() const override;


    // implement wxRadioBoxBase methods
    virtual bool Show(unsigned int n, bool show = true) override;
    virtual bool Enable(unsigned int n, bool enable = true) override;

    virtual bool IsItemEnabled(unsigned int n) const override;
    virtual bool IsItemShown(unsigned int n) const override;


    // override some base class methods to operate on radiobox itself too
    virtual bool Show( bool show = true ) override;
    virtual bool Enable( bool enable = true ) override;

    virtual void SetLabel( const wxString& label ) override;

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    virtual int GetItemFromPoint( const wxPoint& pt ) const override;
#if wxUSE_HELP
    // override virtual wxWindow::GetHelpTextAtPoint to use common platform independent
    // wxRadioBoxBase::DoGetHelpTextAtPoint from the platform independent
    // base class-interface wxRadioBoxBase.
    virtual wxString GetHelpTextAtPoint(const wxPoint & pt, wxHelpEvent::Origin origin) const override
    {
        return wxRadioBoxBase::DoGetHelpTextAtPoint( this, pt, origin );
    }
#endif // wxUSE_HELP

    // implementation
    // --------------

    void GtkDisableEvents();
    void GtkEnableEvents();
#if wxUSE_TOOLTIPS
    virtual void GTKApplyToolTip(const char* tip) override;
#endif // wxUSE_TOOLTIPS

    std::vector<wxGTKRadioButtonInfo> m_buttonsInfo;

protected:
    virtual wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }

#if wxUSE_TOOLTIPS
    virtual void DoSetItemToolTip(unsigned int n, wxToolTip *tooltip) override;
#endif

    virtual void DoApplyWidgetStyle(GtkRcStyle *style) override;
    virtual GdkWindow *GTKGetWindow(wxArrayGdkWindows& windows) const override;

    virtual void DoEnable(bool enable) override;

    virtual bool GTKNeedsToFilterSameWindowFocus() const override { return true; }

    virtual bool GTKWidgetNeedsMnemonic() const override;
    virtual void GTKWidgetDoSetMnemonic(GtkWidget* w) override;

private:
    wxDECLARE_DYNAMIC_CLASS(wxRadioBox);
};

#endif // _WX_GTK_RADIOBOX_H_
