/*
 * File:	gui.h
 * Purpose:	wxWindows plugin with a few GUI elements
 * Author:	Julian Smart
 * Created:	1997
 * Updated:	
 * Copyright:   (c) Julian Smart
 */

#ifndef __GUIH__
#define __GUIH__

// Define a new application type
class MyApp: public wxPluginApp
{ public:
    virtual wxFrame *OnInit(void);
    virtual wxPluginFrame* OnNewInstance(const wxPluginData& data);
};

class MyApp;
class MyFrame;
class MyCanvas;

class MyFrame: public wxPluginFrame
{
public:
	MyFrame(const wxPluginData& data);
	virtual ~MyFrame();

	void OldOnMenuCommand(int id);

private:
	wxMenu*		fileMenu;
	wxMenuBar*	menuBar;
	MyCanvas*	leftCanvas;
	MyCanvas*	rightCanvas;
    wxSplitterWindow* splitter;
};

class MyCanvas: public wxScrolledWindow
{
public:
	MyCanvas(wxWindow* parent, int x, int y, int w, int h);
	virtual ~MyCanvas();

	void OnPaint(wxPaintEvent& event);

DECLARE_EVENT_TABLE()
};

// ID for the menu quit command
#define SPLIT_QUIT          1
#define SPLIT_HORIZONTAL    2
#define SPLIT_VERTICAL      3
#define SPLIT_UNSPLIT       4


#endif

