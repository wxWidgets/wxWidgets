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

private:
    struct Control
    {
        Control() {}

        Control(wxWindow * _ctrl, wxString _name, int _flag)
            : ctrl(_ctrl), name(_name), flag(_flag) {}

        wxWindow * ctrl;
        wxString name;
        int flag;
    };

    typedef std::vector<Control> ControlList;
    ControlList m_controlList;

    // here we introduce the dependency on wxNotebook.
    // The assumption of this whole class is that the gui has the following top-down structure
    //  wxNotebook wxPanel wxSizer wxControls
    wxNotebook* m_notebook;

    wxFlexGridSizer* m_grid;

    wxString m_dir;
    int m_border;



    wxBitmap Capture(Control & ctrl)
    {
        if(ctrl.name == wxT("")) //no mannual specification for the control name
        {
            //Get name from wxRTTI
            ctrl.name = ctrl.ctrl->GetClassInfo()->GetClassName();
        }

        int choice = wxNO;

        if(ctrl.flag & AJ_Dropdown)
        {
            wxString caption = _("Do you wish to capture the dropdown list of ") + ctrl.name + _("?");
            wxString notice = _("Click YES to capture it.\nAnd you MUST drop down the ") + ctrl.name + _(" in 3 seconds after close me.\n");
            notice += _("Click NO otherwise.");

            choice = wxMessageBox(notice, caption, wxYES_NO, m_notebook);

            if(choice == wxYES)
            {
                //Wait for 3 seconds
                using std::clock;
                using std::clock_t;

                clock_t start = clock();
                while(clock() - start < CLOCKS_PER_SEC * 3)
                {
                    wxYieldIfNeeded();
                }
            }
        }

        wxRect rect = GetRect(ctrl.ctrl, ctrl.flag);

        //Do some rect adjust so it can include the dropdown list
        //Currently it only works well under MSW, not adjusted for Linux and Mac OS
        if(ctrl.flag & AJ_Dropdown && choice == wxYES)
        {
//                #ifdef __WXMSW__
            int h = rect.GetHeight();
            rect.SetHeight(h * 4);
//                #endif
        }

        //cut off "wx" and change them into lowercase.
        // e.g. wxButton will have a name of "button" at the end
        ctrl.name.StartsWith(_T("wx"), &(ctrl.name));
        ctrl.name.MakeLower();

        wxBitmap screenshot = ::Capture(rect);

        if(ctrl.flag & AJ_RegionAdjust)
        {
            PutBack(ctrl.ctrl);
        }

        return screenshot;
    }

    //if AJ_RegionAdjust is specified, the following line will use the label trick to adjust
    //the region position and size
    wxRect GetRect(wxWindow* ctrl, int flag);
    //put the control back after the label trick(Using reparent/resizer approach)
    void PutBack(wxWindow * ctrl);

    wxBitmap Union(wxBitmap pic1, wxBitmap pic2)
    {
        int w1, w2, h1, h2, w, h;
        w1 = pic1.GetWidth();
        w2 = pic2.GetWidth();
        h1 = pic1.GetHeight();
        h2 = pic2.GetHeight();

        const int gap_between = 20;

        w = (w1 >= w2) ? w1 : w2;
        h = h1 + h2 + gap_between;

        wxBitmap result(w, h, -1);

        wxMemoryDC dstDC;
        dstDC.SelectObject(result);

        dstDC.DrawBitmap(pic1, 0, 0, false);
        dstDC.DrawBitmap(pic2, 0, h1 + gap_between, false);

        dstDC.SelectObject(wxNullBitmap);

        wxMemoryDC maskDC;
        wxBitmap mask(w, h, 1);
        maskDC.SelectObject(mask);

        maskDC.SetPen(*wxTRANSPARENT_PEN);
        maskDC.SetBrush(*wxBLACK_BRUSH);
        maskDC.DrawRectangle(0, 0, w + 1, h + 1);

        maskDC.SetBrush(*wxWHITE_BRUSH);
        maskDC.DrawRectangle(0, 0, w1, h1);
        maskDC.DrawRectangle(0, h1 + gap_between, w2, h2);
        maskDC.SelectObject(wxNullBitmap);

        result.SetMask(new wxMask(mask));

        return result;
    }

    void Save(wxBitmap screenshot, wxString fileName);
};

#endif // AUTOCAP_H


