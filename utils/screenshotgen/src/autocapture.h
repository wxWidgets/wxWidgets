/////////////////////////////////////////////////////////////////////////////
// Name:        autocapture.h
// Purpose:     Defines the AutoCaptureMechanism class
// Author:      Utensil Candel (UtensilCandel@@gmail.com)
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef AUTOCAP_H
#define AUTOCAP_H

#include <wx/notebook.h>
#include <wx/settings.h>
#include <vector>
#include <ctime>

// Global helper functions. to take screenshot for a rect region
wxBitmap Capture(wxRect rect);
wxBitmap Capture(int x, int y, int width, int height);

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
    AutoCaptureMechanism(wxNotebook * notebook,
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

    void CaptureAll()
    {
        m_notebook->SetSelection(0);
        wxYield();

        for(ControlList::iterator it = m_controlList.begin();
            it != m_controlList.end();
            ++it)
        {
            Control & ctrl = *it;

            if(ctrl.flag == AJ_TurnPage) // Turn to next page
            {
                m_notebook->SetSelection(m_notebook->GetSelection() + 1);
                wxYield();
                continue;
            }

            wxBitmap screenshot = Capture(ctrl);

            if(ctrl.flag & AJ_Union)
            {
                screenshot = Union(screenshot, Capture(*(++it)));
            }

            Save(screenshot, ctrl.name);
        }
    }

protected:      // internal utils
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

    //if AJ_RegionAdjust is specified, the following line will use the label trick to adjust
    //the region position and size
    wxRect GetRect(wxWindow* ctrl, int flag);

    //put the control back after the label trick(Using reparent/resizer approach)
    void PutBack(wxWindow * ctrl);

    wxBitmap Union(wxBitmap pic1, wxBitmap pic2);

    void Save(wxBitmap screenshot, wxString fileName);

private:
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

#endif // AUTOCAP_H


