/////////////////////////////////////////////////////////////////////////////
// Name:        drawer.cpp
// Purpose:     Generic Drawer child window classes.
//              Drawer windows appear under their parent window and
//              behave like a drawer, opening and closing to reveal
//              content that does not need to be visible at all times.
// Author:      Ryan Norton 
// Modified by: 
// Created:     2004-30-01
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "drawerg.h"
#endif

#include "wx/defs.h"
#include "wx/generic/drawerg.h"
#include "wx/timer.h"

enum wxGenericDrawerConstants
{
    wxGENERICDRAWER_INCREMENT = 1,
    wxGENERICDRAWER_INTERVAL = 3
};

class wxGenericDrawerTimer : public wxTimer
{
    public:
    
    wxGenericDrawerTimer(wxGenericDrawerWindow* pWin, const int& nMult) :
            m_pWin(pWin), m_nMult(nMult) {}
    
    void Notify()
    {
        if ((m_nMult < 0 && !m_pWin->m_nOpenOffset) ||
        (
        m_nMult >= 0 &&
        (( (m_pWin->m_nCurrentEdge == wxLEFT ||
               m_pWin->m_nCurrentEdge == wxRIGHT) && m_pWin->m_nOpenOffset >= m_pWin->GetSize().GetWidth()) 
               
               ||

        ( (m_pWin->m_nCurrentEdge == wxTOP ||
               m_pWin->m_nCurrentEdge == wxBOTTOM) && m_pWin->m_nOpenOffset >= m_pWin->GetSize().GetHeight())
        ))
               )
        {
        /*
            wxFprintf(stderr, "shutdown - mult:%i off:%i tlr:%i ttb:%i\n", m_nMult,  m_pWin->m_nOpenOffset,
            
            (m_pWin->m_nCurrentEdge == wxLEFT ||
               m_pWin->m_nCurrentEdge == wxRIGHT) && m_pWin->m_nOpenOffset >= m_pWin->GetSize().GetWidth(),
               
               (m_pWin->m_nCurrentEdge == wxTOP ||
               m_pWin->m_nCurrentEdge == wxBOTTOM) && m_pWin->m_nOpenOffset >= m_pWin->GetSize().GetHeight()
            );
            */
        
            //clean up & shut down
            Stop();
            m_pWin->m_pTimer = NULL;
            
            //reset current edge to preferred edge
            if (m_nMult < 0)
            {
                m_pWin->m_nCurrentEdge = m_pWin->m_nPreferredEdge;
                m_pWin->DoDrawerPosition();
            }
            
            //commit seppuku
            delete this;
        }
        else
        {
//            wxFprintf(stderr, "continue\n");
            wxPoint pos = m_pWin->GetPosition();
            switch (m_pWin->m_nCurrentEdge)
            {
            case wxLEFT:
                pos.x -= wxGENERICDRAWER_INCREMENT * m_nMult;
                break;
            
            case wxRIGHT:
                pos.x += wxGENERICDRAWER_INCREMENT * m_nMult;
                break;
            
            case wxTOP:
                pos.y -= wxGENERICDRAWER_INCREMENT * m_nMult;
                break;
            
            case wxBOTTOM:
            default:
                pos.y += wxGENERICDRAWER_INCREMENT * m_nMult;
                break;
            }
            m_pWin->SetPosition(pos);
            //wxFprintf(stderr, "tpos:%i,%i\n", pos.x, pos.y);
            m_pWin->m_nOpenOffset += wxGENERICDRAWER_INCREMENT * m_nMult;
        }
    }
    
    wxGenericDrawerWindow* m_pWin;
    int m_nMult;
};

enum wxGenericDrawerOffsets
{
    wxGENERICDRAWER_TOPOFFSET = 20,
    wxGENERICDRAWER_BOTTOMOFFSET = 20
};

IMPLEMENT_DYNAMIC_CLASS(wxGenericDrawerWindow, wxWindow)

wxGenericDrawerWindow::wxGenericDrawerWindow() :
    m_bOpen(false), m_nCurrentEdge(wxLEFT), m_nPreferredEdge(wxLEFT), m_nOpenOffset(0),
        m_pTimer(NULL)
{
}

wxGenericDrawerWindow::~wxGenericDrawerWindow()
{ 
    m_isBeingDeleted = true;
    this->Show(false);
    if (m_pTimer)
    {
        m_pTimer->Stop();
        delete m_pTimer;
    }
}
    
bool wxGenericDrawerWindow::Create(wxWindow *parent,
 wxWindowID id, const wxString& title,
 wxSize size, wxDirection edge, const wxString& name)
{
    wxASSERT_MSG(NULL != parent, wxT("wxDrawerWindows must be attached to a parent window."));
    
    wxASSERT_MSG(m_pTimer == NULL, wxT("Currently opening!!"));
           
    wxASSERT_MSG(edge == wxLEFT ||
             edge == wxRIGHT ||
             edge == wxTOP ||
             edge == wxBOTTOM, wxT("Invalid edge") );

    // Create the drawer window. 
    const wxPoint pos(0, 0);
    const long style = wxNO_BORDER || wxVSCROLL | wxHSCROLL;//wxFRAME_DRAWER;
    
    bool success  = wxTopLevelWindow::Create(parent, id, wxT(""), pos, size, style, name);
        
    if (success)
    {    
        // Set the drawers parent.
        

        if (parent->IsTopLevel())
        { 
            wxTopLevelWindow* tlwParent = (wxTopLevelWindow*) parent;
            
            //connect to parent's events
            tlwParent->Connect(tlwParent->GetId(), wxEVT_MOVE,
        (wxObjectEventFunction) (wxEventFunction) (wxMoveEventFunction) &wxGenericDrawerWindow::OnDrawerMove,
                                NULL, //user data
                                this);

            tlwParent->Connect(tlwParent->GetId(), wxEVT_SIZE,
        (wxObjectEventFunction) (wxEventFunction) (wxSizeEventFunction) &wxGenericDrawerWindow::OnDrawerSize,
                                NULL, //user data
                                this);
                    
            tlwParent->Connect(tlwParent->GetId(), wxEVT_KILL_FOCUS,
        (wxObjectEventFunction) (wxEventFunction) (wxFocusEventFunction) &wxGenericDrawerWindow::OnDrawerFocus,
                                NULL, //user data
                                this);

            m_LastParentSize = parent->GetSize();
            DoDrawerPosition();
            DoDrawerSize();
        }
        else
            success = false;
    }
    
    m_nCurrentEdge = m_nPreferredEdge = edge;
    Show(success);
    if (success && parent->IsShown()) //bring parent on top
    {
        parent->Show(false);
        parent->Show(true);
    }
   //wxFprintf(stderr,wxT("success==%i\n"), success);
    return success;
}

wxDirection wxGenericDrawerWindow::GetCurrentEdge() const
{
    return m_nCurrentEdge;
}

wxDirection wxGenericDrawerWindow::GetPreferredEdge() const
{
    return m_nPreferredEdge;
}

bool wxGenericDrawerWindow::IsOpen() const
{
    return m_bOpen;
}

bool wxGenericDrawerWindow::Open(const bool& show)
{
    if(show)
    {
        if (m_pTimer)
            delete m_pTimer;
        
        m_pTimer = new wxGenericDrawerTimer(this, 1);
        m_pTimer->Start(wxGENERICDRAWER_INTERVAL);
    }
    else
    {
        if (m_pTimer)
            delete m_pTimer;
            
        m_pTimer = new wxGenericDrawerTimer(this, -1);
        m_pTimer->Start(wxGENERICDRAWER_INTERVAL);            
    }
    
    return true;
}

bool wxGenericDrawerWindow::SetPreferredEdge(const wxDirection& edge)
{
    wxASSERT(edge == wxLEFT ||
             edge == wxRIGHT ||
             edge == wxTOP ||
             edge == wxBOTTOM );
             
    if (!m_nOpenOffset)
    {
        m_nCurrentEdge = edge;
        DoDrawerPosition();
    }
    
    m_nPreferredEdge = edge;
    return true;
}

void wxGenericDrawerWindow::DoDrawerPosition()
{
    const wxPoint parentPosition(GetParent()->GetPosition());
    SetPosition(wxPoint(
                parentPosition.x + 
                        (m_nCurrentEdge == wxLEFT ?
                            -m_nOpenOffset : m_nCurrentEdge == wxRIGHT ? GetParent()->GetSize().GetWidth() + m_nOpenOffset - GetSize().GetWidth(): wxGENERICDRAWER_TOPOFFSET),
                parentPosition.y + 
                        (m_nCurrentEdge == wxTOP ?
                            -m_nOpenOffset : m_nCurrentEdge == wxBOTTOM ? GetParent()->GetSize().GetHeight() + m_nOpenOffset - GetSize().GetHeight() : wxGENERICDRAWER_TOPOFFSET)                
    ));
    //wxFprintf(stderr,wxT("parentposition:%i,%i\n"), parentPosition.x, parentPosition.y);
    //wxFprintf(stderr,wxT("offset:%i\nposition:%i,%i\n"), m_nOpenOffset, GetPosition().x, GetPosition().y);
}

void wxGenericDrawerWindow::DoDrawerSize()
{
    // Constrain the drawer size to the parent window.
    const wxSize parentSize(GetParent()->GetClientSize());
    wxSize size = GetSize();
    
    if (wxLEFT == m_nCurrentEdge || wxRIGHT == m_nCurrentEdge)
    {
        if (size.GetHeight() > parentSize.GetHeight())
            size.SetHeight(parentSize.GetHeight() - (wxGENERICDRAWER_TOPOFFSET + wxGENERICDRAWER_BOTTOMOFFSET));
            
            size.SetHeight(size.GetHeight() + (parentSize.GetHeight() - m_LastParentSize.GetHeight()));
    }
    else
    {
        if (size.GetWidth() > parentSize.GetWidth())
            size.SetWidth(parentSize.GetWidth() - (wxGENERICDRAWER_TOPOFFSET + wxGENERICDRAWER_BOTTOMOFFSET));

            size.SetWidth(size.GetWidth() + (parentSize.GetWidth() - m_LastParentSize.GetWidth()));
    }
    
    SetSize(size);
    m_LastParentSize = parentSize;
    
    //wxFprintf(stderr,wxT("size:%i,%i\n"), size.GetWidth(), size.GetHeight());
}

void wxGenericDrawerWindow::OnDrawerFocus(wxFocusEvent& evt)
{
//    wxFprintf(stderr, wxT("focus\n"));
    
    if (wxWindow::FindFocus() == this)
        GetParent()->SetFocus();
}

void wxGenericDrawerWindow::OnDrawerMove(wxMoveEvent& evt)
{
    DoDrawerPosition();
}

void wxGenericDrawerWindow::OnDrawerSize(wxSizeEvent& evt)
{
    DoDrawerSize();
}

