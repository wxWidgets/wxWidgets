/////////////////////////////////////////////////////////////////////////////
// Name:        regextest.cpp
// Purpose:     Test regex libs and some gui components
// Author:      Ryan Norton
// Modified by: 
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Ryan Norton <wxprojects@comcast.net>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//                          HEADERS
//===========================================================================

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/file.h"   

#if !wxUSE_REGEX
#   error wxUSE_REGEX needs to be enabled in setup.h to use wxRegEx
#endif

//
//  This is the required header for wxRegEx
//
#include <wx/regex.h>

//===========================================================================
//                          IMPLEMENTATION
//===========================================================================

//
//  One of the best sources for regex info is IEEE document/standard 1003.2
//  From the open group.  
//  A current link - (http://www.opengroup.org/onlinepubs/007904975/basedefs/xbd_chap09.html).
//

//---------------------------------------------------------------------------
//                          MyFrame
//---------------------------------------------------------------------------

class MyFrame : public wxFrame
{
public:

    //IDs for our controls
    enum
    {
        PatternTextID,
        SearchTextID,
        OkButtonID
    };

    //Menu IDs
    enum
    {
        ExtendedID,
        ICaseID,
        NewLineID,
        NotBolID,
        NotEolID,
        CompID,
        MatchID
    };


    //
    //  Adds an item to a menu (long way, easier to do wxMenu::AppendCheckItem())
    //
    void AddMenuItem(wxMenu* pMenu, int nID = wxID_SEPARATOR, const wxChar* szTitle = _(""), 
                                    const wxChar* szToolTip = _(""))
    {
        wxMenuItem* pItem;

        if (nID == wxID_SEPARATOR)
            pItem = new wxMenuItem (NULL, wxID_SEPARATOR, szTitle, szToolTip, wxITEM_SEPARATOR);
        else
            pItem = new wxMenuItem (NULL, nID, szTitle, szToolTip, wxITEM_CHECK);

#ifdef __WXMSW__
        pItem->SetBackgroundColour(wxColour(115, 113, 115));
        pItem->SetTextColour(*wxBLACK);
#endif
        pMenu->Append(pItem);
    }

#if defined( __WXMSW__ ) || defined( __WXMAC__ )
    void OnContextMenu(wxContextMenuEvent& event)
        { PopupMenu(GetMenuBar()->GetMenu(1), ScreenToClient(event.GetPosition())); }
#else
    void OnRightUp(wxMouseEvent& event)
        { PopupMenu(GetMenuBar()->GetMenu(1), event.GetPosition()); }
#endif

    MyFrame() : wxFrame(  NULL, -1, _("regextest - wxRegEx Testing App"),
                            wxPoint(20,20), wxSize(300,400), wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL )
    {
        //Set the background to something light gray-ish
        SetBackgroundColour(wxColour(150,150,150));

        //
        //  Create the menus 
        //
#if wxUSE_MENUS

        wxMenu *FileMenu = new wxMenu;
        wxMenu *OptionsMenu = new wxMenu;
        wxMenu *HelpMenu = new wxMenu;

        AddMenuItem(FileMenu, wxID_EXIT, _("&Exit"), _("Quit this program"));

        AddMenuItem(OptionsMenu, ExtendedID, _T("wxRE_EXTENDED"), _("Extended Regular Expressions?"));
        AddMenuItem(OptionsMenu, ICaseID, _T("wxRE_ICASE"), _("Enable case-insensitive matching?"));
        AddMenuItem(OptionsMenu,  NewLineID, _T("wxRE_NEWLINE"), _("Treat \n as special?"));
        AddMenuItem(OptionsMenu);
        AddMenuItem(OptionsMenu,  NotBolID, _T("wxRE_NOTBOL"), _("Use functionality of ^ character?"));
        AddMenuItem(OptionsMenu,  NotEolID, _T("wxRE_NOTEOL"), _("Use functionality of $ character?"));
        AddMenuItem(OptionsMenu);
        AddMenuItem(OptionsMenu);
        AddMenuItem(OptionsMenu,  CompID, _("Test Compile"), _("Added Compiling to Match Time?"));
        AddMenuItem(OptionsMenu,  MatchID, _("Test Match"), _("Added Matching to Match Time?"));

        AddMenuItem(HelpMenu, wxID_ABOUT, _T("&About...\tF1"), _("Show about dialog"));

        OptionsMenu->Check(ExtendedID, true);

        OptionsMenu->Check(CompID, true);
        OptionsMenu->Check(MatchID, true);

        wxMenuBar *MenuBar = new wxMenuBar();
        MenuBar->Append(FileMenu, _T("&File"));
        MenuBar->Append(OptionsMenu, _T("&Options"));
        MenuBar->Append(HelpMenu, _T("&Help"));

        SetMenuBar(MenuBar);
#endif // wxUSE_MENUS

        // Text controls
        PatternText.Create(this, PatternTextID, _(""), wxPoint(5, 30));
        SearchText.Create(this, SearchTextID, _(""), wxPoint(5, 75), PatternText.GetSize(), wxTE_MULTILINE);
        NumIters.Create(this, -1, _("5000"), wxPoint(100, 190));
        //reset size of the 2 main text controls
        wxSize TextSize = PatternText.GetSize();
        TextSize.SetWidth(200);
        PatternText.SetSize(TextSize);
        TextSize.SetHeight(TextSize.GetHeight() * 4);
        SearchText.SetSize(TextSize);
        
        // StaticText
        PatternHeader.Create(this, -1, _("Regular Expression:"), wxPoint(5, 10));
        SearchHeader.Create(this, -1, _("String to Search:"), wxPoint(5, 55));
        IterHeader.Create(this, -1, _("Iterations (Match Time):"), wxPoint(100, 170));

        ResultText.Create(this, -1, _(""), wxPoint(5, 220), wxSize(100,110), wxST_NO_AUTORESIZE);

        // Button
        OkButton.Create(this, OkButtonID, _("OK"), wxPoint(20, 190));


#if wxUSE_STATUSBAR && !defined(__WXWINCE__)
        // create a status bar just for fun (by default with 1 pane only)
        CreateStatusBar(1);
        SetStatusText(_("Enter Some Values and Press the OK Button or Enter"));
#endif // wxUSE_STATUSBAR
    }

    void OnAbout(wxCommandEvent& WXUNUSED(evt))
    {
        wxChar* szBuffer;
        wxFile f;

        f.Open(_("README.txt"), wxFile::read);
        szBuffer = new wxChar[f.Length() + 1];
        f.Read(szBuffer, f.Length());
        f.Close();

        wxMessageBox(wxString::Format(
            _("%s%s%s%s"),
            _("-----------Regular Expression Test Application-----------\n"),
            szBuffer,
            _("\n\n\n(c) 2003 Ryan Norton <wxprojects@comcast.net>\n"),
            wxVERSION_STRING
                                     )
                    );

        delete szBuffer;
    }

    void OnMatch(wxCommandEvent& WXUNUSED(evt))
    {
        wxString szPattern = PatternText.GetValue();
        wxString szSearch = SearchText.GetValue();
        wxString szResult; //Will be displayed in ResultText
        wxString szStatus;

        int nCompileFlags = 0;
        int nMatchFlags = 0;

        wxMenu* OptionsMenu = GetMenuBar()->GetMenu(1);

        if (!(OptionsMenu->IsChecked(ExtendedID)))
        {
            nCompileFlags |= wxRE_BASIC;
        }

        if (OptionsMenu->IsChecked(ICaseID))
        {
            nCompileFlags |= wxRE_ICASE;
        }

        if (OptionsMenu->IsChecked(NotBolID))
        {
            nMatchFlags |= wxRE_NOTBOL;
        }

        if (OptionsMenu->IsChecked(NotEolID))
        {
            nMatchFlags |= wxRE_NOTEOL;
        }


        //Success!  Here we'll display some 
        //information to the user
        size_t dwStartIndex = 0, dwEndIndex = 0;

        time_t dwStartTime = 0, dwEndTime = 0; 

        int i = 0;
        long n;
        if (!NumIters.GetValue().ToLong(&n))
            n = 0;


        SetStatusText("Testing wxRegEx...");

        //Our regular expression object
        wxRegEx Regex;

        //Regular Expressions must be compiled before
        //you can search a string with them, or
        //at least most implementations do.
        //(Boost and Microsoft have templated
        //versions that don't require compilation)
        //Basically compilation breaks it down into
        //something that's easier to parse, due
        //to the syntax of regular expressions
        //
        //Note that you can use a constructor of wxRegEx
        //that compiles it automatically, i.e.
        //wxRegEx Regex(szPattern, nCompileFlags)
        if (!Regex.Compile(szPattern, nCompileFlags))
            szStatus += _("\nCompile Failed!\n"); 
        else
        {
            //Here's where we actually search our string
            if (!Regex.Matches(szSearch, nMatchFlags))
                szStatus += _("\nExecution/Matching Failed!\n");
            else
            {
                Regex.GetMatch(&dwStartIndex, &dwEndIndex);

                szStatus = _("Success");

                //We're going to go ahead and time the match
                //for fun (clock() is a c library routine that
                //returns the current time since a certian point
                //in milliseconds
                dwStartTime = clock();
        
                if (OptionsMenu->IsChecked(CompID))
                {
                    for(i = 0; i < n; ++i)
                    {
                        SetStatusText(wxString::Format(_("wxRegEx Compile #%i"), i));
                        Regex.Compile(szPattern, nCompileFlags);
                    }
                }
                if (OptionsMenu->IsChecked(MatchID))
                {
                    for(i = 0; i < n; ++i)
                    {
                        SetStatusText(wxString::Format(_("wxRegEx Match #%i"), i));
                        Regex.Matches(szSearch, nMatchFlags);
                    }
                }
        
                //This line gets the difference in time between now
                //and when we first initialized dwStartTime.
                dwEndTime = clock() - dwStartTime;
            }
        }


        szResult = wxString::Format(
                            _("--wxRegEx--\nIndex:[%i]-[%i]\nString:%s\nMatch Time:%ums\nStatus:%s"),
                            dwStartIndex, dwEndIndex+dwStartIndex, 
                            szSearch.Mid(dwStartIndex, dwEndIndex),
                            dwEndTime, 
                            szStatus
                                    );

        SetStatusText("Regex Run Complete");

        ResultText.SetLabel(szResult);
    }

    void OnQuit(wxCommandEvent& WXUNUSED(evt))
    {
        Close(TRUE);
    }

    wxTextCtrl PatternText, SearchText, NumIters;
    wxStaticText PatternHeader, SearchHeader, IterHeader, ResultText;
    wxButton OkButton;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)

    //menu
    EVT_MENU(wxID_EXIT,  MyFrame::OnQuit)
    EVT_MENU(wxID_ABOUT,  MyFrame::OnAbout)

    //text
    EVT_TEXT_ENTER(MyFrame::PatternTextID, MyFrame::OnMatch)
    EVT_TEXT_ENTER(MyFrame::SearchTextID, MyFrame::OnMatch)

    //button
    EVT_BUTTON(MyFrame::OkButtonID, MyFrame::OnMatch)

#if defined( __WXMSW__ ) || defined( __WXMAC__ )
    EVT_CONTEXT_MENU(MyFrame::OnContextMenu)
#else
    EVT_RIGHT_UP(MyFrame::OnRightUp)
#endif
    
END_EVENT_TABLE()

//---------------------------------------------------------------------------
//                          MyApp
//---------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
	bool OnInit()
    {
       MyFrame* dialog = new MyFrame();
       dialog->Show();
       return true;
    }
};

IMPLEMENT_APP(MyApp)

