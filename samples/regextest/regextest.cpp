/////////////////////////////////////////////////////////////////////////////
// Name:        regextest.cpp
// Purpose:     Application to test regular expression (wxRegEx)
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

//
//  This is the required header for wxRegEx
//
#include <wx/regex.h>

//===========================================================================
//                          IMPLEMENTATION
//===========================================================================

//---------------------------------------------------------------------------
//                          MyDialog
//---------------------------------------------------------------------------

class MyDialog : public wxFrame
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
        NotEolID
    };


    MyDialog() : wxFrame(  NULL, -1, _("regextest - wxRegEx Testing App"),
                            wxPoint(20,20), wxSize(300,300) )
    {
        //Set the background to something light gray-ish
        SetBackgroundColour(wxColour(150,150,150));

        //
        //  Create the menus (Exit & About)
        //
     #if wxUSE_MENUS
        wxMenu *FileMenu = new wxMenu;
                OptionsMenu = new wxMenu;
        wxMenu *HelpMenu = new wxMenu;

        FileMenu->Append(wxID_EXIT, _T("&Exit"), _("Quit this program"));

        OptionsMenu->AppendCheckItem(ExtendedID, _T("wxRE_EXTENDED"), _("Extended Regular Expressions?"));
        OptionsMenu->AppendCheckItem(ICaseID, _T("wxRE_ICASE"), _("Enable case-insensitive matching?"));
        OptionsMenu->AppendCheckItem(NewLineID, _T("wxRE_NEWLINE"), _("Treat \n as special?"));
        OptionsMenu->AppendSeparator();
        OptionsMenu->AppendCheckItem(NotBolID, _T("wxRE_NOTBOL"), _("Use functionality of ^ character?"));
        OptionsMenu->AppendCheckItem(NotEolID, _T("wxRE_NOTEOL"), _("Use functionality of $ character?"));
        OptionsMenu->Check(ExtendedID, true);

        HelpMenu->Append(wxID_ABOUT, _T("&About...\tF1"), _("Show about dialog"));

        wxMenuBar *MenuBar = new wxMenuBar();
        MenuBar->Append(FileMenu, _T("&File"));
        MenuBar->Append(OptionsMenu, _T("&Options"));
        MenuBar->Append(HelpMenu, _T("&Help"));

        SetMenuBar(MenuBar);
    #endif // wxUSE_MENUS

        // Text controls
        PatternText.Create(this, PatternTextID, _(""), wxPoint(5, 30));
        SearchText.Create(this, SearchTextID, _(""), wxPoint(5, 75));
        //reset size of text controls
        wxSize TextSize = PatternText.GetSize();
        TextSize.SetWidth(200);
        PatternText.SetSize(TextSize);
        SearchText.SetSize(TextSize);
        
        // StaticText
        PatternHeader.Create(this, -1, _("Regular Expression:"), wxPoint(5, 10));
        SearchHeader.Create(this, -1, _("String to Search:"), wxPoint(5, 55));
        ResultText.Create(this, -1, _(""), wxPoint(5, 150));        

        // Button
        OkButton.Create(this, OkButtonID, _("OK"), wxPoint(20, 120));
    }

    void OnAbout(wxCommandEvent& WXUNUSED(evt))
    {
        wxMessageBox(wxString::Format(
            _("%s%s%s"),
            _("Regular Expression Test Application\n"),
            _("(c) 2003 Ryan Norton <wxprojects@comcast.net>\n"),
            wxVERSION_STRING
                                     )
                    );
    }

    void OnMatch(wxCommandEvent& WXUNUSED(evt))
    {
        wxString szPattern = PatternText.GetValue();
        wxString szSearch = SearchText.GetValue();
        wxString szResult = _("Result:\n"); //Will be displayed in ResultText

        int nCompileFlags = 0;
        int nMatchFlags = 0;

        if (!(OptionsMenu->IsChecked(ExtendedID)))
            nCompileFlags |= wxRE_BASIC;

        if (OptionsMenu->IsChecked(ICaseID))
            nCompileFlags |= wxRE_ICASE;

        if (OptionsMenu->IsChecked(NewLineID))
            nCompileFlags |= wxRE_NEWLINE;

        if (OptionsMenu->IsChecked(NotBolID))
            nMatchFlags |= wxRE_NOTBOL;

        if (OptionsMenu->IsChecked(NotEolID))
            nMatchFlags |= wxRE_NOTEOL;

        //Our regular expression object
		wxRegEx Regex;

        //We're going to go ahead and time the match
        //for fun (clock() is a c library routine that
        //returns the current time since a certian point
        //in milliseconds
        time_t dwStartTime = clock();

        //Regular Expressions must be compiled before
        //you can search a string with them, or
        //at least most implementations do.
        //(Boost and Microsoft have templated
        //versions that don't require compilation)
        //Basically compilation breaks it down into
        //something that's easier to parse, due
        //to the syntax of regular expressions
        if (!Regex.Compile(szPattern, nCompileFlags))
            szResult += _("\nCompile Failed!\n"); 
        else
        {
            //Here's where we actually search our string
            if (!Regex.Matches(szSearch, nMatchFlags))
                szResult += _("\nExecution/Matching Failed!\n");
            else
            {
                //Success!  Here we'll display some 
                //information to the user
                size_t dwStartIndex, dwEndIndex;
                Regex.GetMatch(&dwStartIndex, &dwEndIndex);
                
                szResult += wxString::Format(
                                    _("Start Index:%i\nMatch Length:%i\nString Matched:%s\n"),
                                    dwStartIndex, dwEndIndex, 
                                    szSearch.Mid(dwStartIndex, dwEndIndex)
                                            );
            }
        }

        //This line gets the difference in time between now
        //and when we first initialized dwStartTime.
        time_t dwEndTime = clock() - dwStartTime;

        szResult += wxString::Format(_("Match Time:%u Milliseconds"), dwEndTime);

        ResultText.SetLabel(szResult);
    }

    void OnQuit(wxCommandEvent& WXUNUSED(evt))
    {
        Close(TRUE);
    }

    wxTextCtrl PatternText, SearchText;
    wxStaticText PatternHeader, SearchHeader, ResultText;
    wxButton OkButton;

    wxMenu *OptionsMenu;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MyDialog, wxFrame)

//menu
EVT_MENU(wxID_EXIT,  MyDialog::OnQuit)
EVT_MENU(wxID_ABOUT,  MyDialog::OnAbout)

//text
EVT_TEXT_ENTER(MyDialog::PatternTextID, MyDialog::OnMatch)
EVT_TEXT_ENTER(MyDialog::SearchTextID, MyDialog::OnMatch)

//button
EVT_BUTTON(MyDialog::OkButtonID, MyDialog::OnMatch)

END_EVENT_TABLE()

//---------------------------------------------------------------------------
//                          MyApp
//---------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
	bool OnInit()
    {
       MyDialog* dialog = new MyDialog();
       dialog->Show();
       return true;
    }
};

IMPLEMENT_APP(MyApp)

