#ifndef _WX_POPUPWIN_H_
#define _WX_POPUPWIN_H_

class WXDLLEXPORT wxPopupWindow : public wxPopupWindowBase {
public:
    wxPopupWindow();
    wxPopupWindow(wxWindow *parent, int flags = wxBORDER_NONE);
    bool Create(wxWindow *parent, int flags = wxBORDER_NONE);

protected:    
    DECLARE_DYNAMIC_CLASS(wxPopupWindow)
};

#endif // _WX_POPUPWIN_H_
