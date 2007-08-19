#ifndef _WX_DC_H_
#define _WX_DC_H_

class WXDLLEXPORT wxDC : public wxDCBase {
public:
    wxDC();
    virtual ~wxDC();

    virtual void DoSetClippingRegion(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    
private:
    DECLARE_ABSTRACT_CLASS(wxDC)
};

#endif // _WX_DC_H_
