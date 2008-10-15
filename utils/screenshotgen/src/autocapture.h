/////////////////////////////////////////////////////////////////////////////
// Name:        autocapture.h
// Purpose:     Defines the AutoCaptureMechanism class
// Author:      Utensil Candel (UtensilCandel@@gmail.com)
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _AUTOCAPTURE_H_
#define _AUTOCAPTURE_H_

#include <vector>
#include <ctime>

#include <wx/notebook.h>


// TODO: document what these flags mean
enum AdjustFlags
{
    AJ_Normal = 0,
    AJ_RegionAdjust = 1 << 0,
    AJ_Dropdown = 1 << 1,
    AJ_TurnPage = 1 << 2,
    AJ_Union = 1 << 3,
    AJ_UnionEnd = 1 << 4
};


// ----------------------------------------------------------------------------
// class AutoCaptureMechanism
// ----------------------------------------------------------------------------

class AutoCaptureMechanism
{
public:
    AutoCaptureMechanism(wxNotebook *notebook,
                         wxString directory = wxT("screenshots"),
                         int border = 5)
        : m_notebook(notebook), m_dir(directory), m_border(border) {}

    ~AutoCaptureMechanism(){}

    /*
        If wxRTTI can't get the name correctly, specify name;
        If wxWindow::GetScreenRect doesn't get the rect correctly, set flag to AJ_RegionAdjust
    */
    void RegisterControl(wxWindow * ctrl, wxString name = wxT(""),
                         int flag = AJ_Normal)
    {
        m_controlList.push_back(Control(ctrl, name, flag));
    }

    void RegisterControl(wxWindow * ctrl, int flag)
    {
        RegisterControl(ctrl, wxT(""), flag);
    }

    void RegisterPageTurn()
    {
        m_controlList.push_back(Control(0, wxT(""), AJ_TurnPage));
    }

    // capture all controls of the associated notebook
    void CaptureAll();

    // take a screenshot only of the given rect
    // delay is only useful for Mac, for fixing a delay bug
    static wxBitmap Capture(wxRect rect, int delay = 0);
    static wxBitmap Capture(int x, int y, int width, int height, int delay = 0);

    static void Delay(int seconds);


private:      // internal utils
    struct Control
    {
        Control() {}

        Control(wxWindow * _ctrl, wxString _name, int _flag)
            : ctrl(_ctrl), name(_name), flag(_flag) {}

        wxWindow * ctrl;
        wxString name;
        int flag;
    };

    wxBitmap Capture(Control & ctrl);

    // if AJ_RegionAdjust is specified, the following line will use the label
    // trick to adjust the region position and size
    wxRect GetRect(wxWindow* ctrl, int flag);

    // put the control back after the label trick(Using reparent/resizer approach)
    void PutBack(wxWindow * ctrl);

    wxBitmap Union(wxBitmap pic1, wxBitmap pic2);

    void Save(wxBitmap screenshot, wxString fileName);

    typedef std::vector<Control> ControlList;
    ControlList m_controlList;

    // here we introduce the dependency on wxNotebook.
    // The assumption of this whole class is that the gui has the following top-down structure
    //  wxNotebook wxPanel wxSizer wxControls
    wxNotebook* m_notebook;

    wxFlexGridSizer* m_grid;

    wxString m_dir;
    int m_border;
};

#endif // _AUTOCAPTURE_H_


