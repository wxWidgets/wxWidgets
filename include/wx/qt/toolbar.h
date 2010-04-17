/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/toolbar.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_TOOLBAR_H_
#define _WX_QT_TOOLBAR_H_

class WXDLLIMPEXP_CORE wxToolBar : public wxToolBarBase
{
public:
    wxToolBar();
    wxToolBar(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxNO_BORDER | wxTB_HORIZONTAL,
                const wxString& name = wxToolBarNameStr);

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxNO_BORDER | wxTB_HORIZONTAL,
                const wxString& name = wxToolBarNameStr);

    virtual wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y) const;

protected:
    virtual bool DoInsertTool(size_t pos, wxToolBarToolBase *tool);
    virtual bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool);
    virtual void DoEnableTool(wxToolBarToolBase *tool, bool enable);
    virtual void DoToggleTool(wxToolBarToolBase *tool, bool toggle);
    virtual void DoSetToggle(wxToolBarToolBase *tool, bool toggle);

    virtual wxToolBarToolBase *CreateTool(int toolid,
                                          const wxString& label,
                                          const wxBitmap& bmpNormal,
                                          const wxBitmap& bmpDisabled,
                                          wxItemKind kind,
                                          wxObject *clientData,
                                          const wxString& shortHelp,
                                          const wxString& longHelp);
    
    virtual wxToolBarToolBase *CreateTool(wxControl *control,
                                          const wxString& label);

private:
};

#endif // _WX_QT_TOOLBAR_H_
