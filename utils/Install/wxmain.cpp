#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/resource.h"
#include "install.h"
#include "instsup.h"

#include <ctype.h>

#if !defined(__EMX__) && !defined(__OS2__) && !defined(WIN32) && !defined(WINNT)
#define stricmp strcasecmp
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern char installdir[];

#ifdef __cplusplus
}
#endif

typedef struct _wxCommands {
	int id;
	char file[50];
} wxCommands;

#define COMMANDMAX 20

wxCommands wxcommands[COMMANDMAX];
char finishedscript[256] = "";
char percentname[256] = "";

class MyApp: public wxApp
{
public:
	MyApp();

	virtual bool OnInit();

	virtual ~MyApp();
};

class MyDialog : public wxDialog
{
public:
	void handle_button(wxCommandEvent& event);
};

class MyThread : public wxThread
{
public:
	virtual void *Entry();
};

void *MyThread::Entry()
{
	install_thread(NULL);
	return NULL;
}

MyDialog *dialog = NULL;

IMPLEMENT_APP(MyApp)

MyApp::MyApp()
{
}

wxCondition *InstCond = NULL;

void updatepercent(void)
{
	wxGauge *tmp = (wxGauge *)wxFindWindowByName(percentname, dialog);

	if(tmp)
	{
		unsigned long sliderpos;
		int pixels = 100;
		extern int current_file, files;

		sliderpos = (unsigned long)(((float)(current_file)/(float)files)*pixels);
		tmp->SetValue(sliderpos);
	}
}

int parse_ini(char *filename)
{
	FILE *f;
	int b;

	for(b=0;b<COMMANDMAX;b++)
	{
		wxcommands[b].id = 0;
		wxcommands[b].file[0] = 0;
	}

	grabfile(filename);
	if((f=fopen(filename, "r")) != NULL)
	{
		int bytesread = 1;
		char raw[256], entry[256], entrydata[256], entrydata2[256];

		/* figure out why we are not getting an EOF on windows...
		 bytesread is a hack to make sure it doesn't loop forever. */
		while(!feof(f) && bytesread)
		{
			bytesread = getparseline(f, '#', ',', '\"', raw, entry, entrydata, entrydata2);
			if(stricmp(entry, "loadwxr") == 0)
			{
				grabfile(entrydata);
				if(!wxResourceParseFile(entrydata))
				{
					error("Error loading resources!");
					remove(entrydata);
					remove(filename);
					return FALSE;
				}
				remove(entrydata);

				dialog = new MyDialog;

				dialog->LoadFromResource(NULL, "dialog1");
				dialog->Show(TRUE);

				/*MyApp::SetTopWindow(dialog);*/
			}
			else if(stricmp(entry, "closeold") == 0)
			{
				if(dialog)
					dialog->Destroy();
				dialog = NULL;
			}
			else if(stricmp(entry, "getcheck") == 0)
			{
				wxCheckBox *tmp = (wxCheckBox *)wxFindWindowByName(entrydata, dialog);

				if(tmp)
				{
					if(!tmp->GetValue())
					{
						parse_ini(entrydata2);
						bytesread = 0;
					}
				}
			}
			else if(stricmp(entry, "gettext") == 0)
			{
				wxTextCtrl *tmp = (wxTextCtrl *)wxFindWindowByName(entrydata, dialog);
                wxString bleah;

				if(tmp)
				{
					if((bleah = tmp->GetValue()))
                        strcpy(installdir, bleah);
				}
			}
			else if(stricmp(entry, "settext") == 0)
			{
				wxTextCtrl *tmp = (wxTextCtrl *)wxFindWindowByName(entrydata, dialog);

				if(tmp)
					tmp->SetValue(installdir);
			}
			else if(stricmp(entry, "message") == 0)
				mesg(entrydata);
			else if(stricmp(entry, "disable") == 0)
			{
				wxButton *tmp = (wxButton *)wxFindWindowByName(entrydata, dialog);
				if(tmp)
					tmp->Disable();
			}
			else if(stricmp(entry, "grabfile") == 0)
				grabfile(entrydata);
			else if(stricmp(entry, "remove") == 0)
				remove(entrydata);
			else if(stricmp(entry, "system") == 0)
				wxExecute(entrydata);
			else if(stricmp(entry, "startinst") == 0)
			{
			strcpy(percentname, entrydata);
			strcpy(finishedscript, entrydata2);

			/* I couldn't get it working with threads, even when
			 using conditions it blocked the message loop making
			 it unreadable, so I am going with wxYield for the
			 moment. */
#if 0
			MyThread *thread = new MyThread();

			if ( thread->Create() != wxTHREAD_NO_ERROR )
			{
				error("Can't create thread!");
			}
			strcpy(finishedscript, entrydata);

			InstCond = new wxCondition();

			thread->Run();

			wxMutex *test = new wxMutex();

			InstCond->Wait(*test);
#endif
			install_thread(NULL);
			parse_ini(finishedscript);
			}
			else if(stricmp(entry, "mleview") == 0)
			{
				FILE *f;
				unsigned long bytes;
				char buffer[1025];
				wxTextCtrl *tmp = (wxTextCtrl *)wxFindWindowByName(entrydata, dialog);

				if(tmp)
				{
					grabfile(entrydata2);

					if((f = fopen(entrydata2, "rb"))!= NULL)
					{
						while(!feof(f))
						{
							memset(buffer, 0, 1025);
							bytes = fread(buffer, 1, 1024, f);
							*tmp << buffer;
						}
						fclose(f);
					}
					remove(entrydata2);
                    tmp->ShowPosition(0);
				}

			}
			else if(stricmp(entry, "setbutton") == 0)
			{
				for(b=0;b<COMMANDMAX;b++)
				{
					if(!wxcommands[b].id)
					{
						wxcommands[b].id = atoi(entrydata);
						strcpy(wxcommands[b].file, entrydata2);
						b = COMMANDMAX;
					}
				}
				dialog->Connect( atol(entrydata),  -1, wxEVT_COMMAND_BUTTON_CLICKED,
								 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
								 &MyDialog::handle_button);

			}
		}
		fclose(f);
		remove(filename);
		return TRUE;
	}
	remove(filename);
	return FALSE;
}

void MyDialog::handle_button(wxCommandEvent& event )
{
	int b;

	for(b=0;b<COMMANDMAX;b++)
	{
		if(wxcommands[b].id &&
		   wxcommands[b].id == event.GetId())
		{
			char tempbuf[50];

			strcpy(tempbuf, wxcommands[b].file);
			if(stricmp(tempbuf, "exit") == 0)
				exit(0);
			grabfile(tempbuf);
			if(!parse_ini(tempbuf))
			{
				error("Error loading script \"%s\"", tempbuf);
				exit(1);
			}
			remove(tempbuf);
			return;
		}
	}

}

bool MyApp::OnInit(void)
{
#if 1
	install_init(argv[0]);
#else
	install_init("install.exe");
#endif

	if(!parse_ini("page1.ini"))
	{
		error("Could not load startup script!");
		return FALSE;
	}

	return TRUE;
}

MyApp::~MyApp()
{
}


