// Scintilla source code edit control
// Platform.h - interface to platform facilities
// Also includes some basic utilities
// Implemented in PlatGTK.cxx for GTK+/Linux, PlatWin.cxx for Windows, and PlatWX.cxx for wxWindows
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef PLATFORM_H
#define PLATFORM_H

// PLAT_GTK = GTK+ on Linux, PLAT_WIN = Win32 API on Win32 OS
// PLAT_WX is wxWindows on any supported platform
// Could also have PLAT_GTKWIN = GTK+ on Win32 OS in future

#define PLAT_GTK 0
#define PLAT_WIN 0
#define PLAT_WX  0

#if defined(__WX__)
#undef PLAT_WX
#define PLAT_WX  1

#elif defined(GTK)
#undef PLAT_GTK
#define PLAT_GTK 1

#else
#undef PLAT_WIN
#define PLAT_WIN 1

#endif


// Include the main header for each platform

#if PLAT_GTK
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#endif

#if PLAT_WIN
#define _WIN32_WINNT  0x0400 // Otherwise some required stuff gets ifdef'd out
// Vassili Bourdo: shut up annoying Visual C++ warnings:
#ifdef _MSC_VER
#pragma warning(disable: 4800 4244 4309)
#endif
#include <windows.h>
#include <richedit.h>
#endif

#if PLAT_WX
#include <wx/wx.h>
#endif

// Underlying the implementation of the platform classes are platform specific types.
// Sometimes these need to be passed around by client code so they are defined here

#if PLAT_GTK
typedef GdkColor ColourID;
typedef GdkFont* FontID;
typedef GdkDrawable* SurfaceID;
typedef GtkWidget* WindowID;
typedef GtkItemFactory* MenuID;
#endif

#if PLAT_WIN
typedef COLORREF ColourID;
typedef HFONT FontID;
typedef HDC SurfaceID;
typedef HWND WindowID;
typedef HMENU MenuID;
#endif

#if PLAT_WX
typedef wxColour ColourID;
typedef wxFont* FontID;
typedef wxDC* SurfaceID;
typedef wxWindow* WindowID;
typedef wxMenu* MenuID;
#endif

#if PLAT_GTK || PLAT_WX
#define SHIFT_PRESSED 1
#define LEFT_CTRL_PRESSED 2
#define LEFT_ALT_PRESSED 4
#endif

// Point is exactly the same as the Win32 POINT and GTK+ GdkPoint so can be used interchangeably

class Point {
public:
	int x;
	int y;
	
	Point(int x_=0, int y_=0) : x(x_), y(y_) {
	}

	// Other automatically defined methods (assignment, copy constructor, destructor) are fine
	
	static Point FromLong(long lpoint);
};

// PRectangle is exactly the same as the Win32 RECT so can be used interchangeably
// PRectangles contain their top and left sides, but not their right and bottom sides
class PRectangle {
public:
	int left;
	int top;
	int right;
	int bottom;

	PRectangle(int left_=0, int top_=0, int right_=0, int bottom_ = 0) :
		left(left_), top(top_), right(right_), bottom(bottom_) {
	}

	// Other automatically defined methods (assignment, copy constructor, destructor) are fine

	bool Contains(Point pt) {
		return (pt.x >= left) && (pt.x <= right) &&
			(pt.y >= top) && (pt.y <= bottom);
	}
	bool Contains(PRectangle rc) {
		return (rc.left >= left) && (rc.right <= right) &&
			(rc.top >= top) && (rc.bottom <= bottom);
	}
	bool Intersects(PRectangle other) {
		return (right >= other.left) && (left <= other.right) &&
			(bottom >= other.top) && (top <= other.bottom);
	}
	int Width() { return right - left; }
	int Height() { return bottom - top; }
};

#if PLAT_WX
wxRect wxRectFromPRectangle(PRectangle prc);
PRectangle PRectangleFromwxRect(wxRect rc);
#endif

class Colour {
	ColourID co;
public:
	Colour(long lcol=0);
	Colour(unsigned int red, unsigned int green, unsigned int blue);
	bool operator==(const Colour &other) const;
	long AsLong() const;
	unsigned int GetRed();
	unsigned int GetGreen();
	unsigned int GetBlue();
	
	friend class Surface;
	friend class Palette;
};

// Colour pairs hold a desired colour and the colour that the graphics engine
// allocates to approximate the desired colour.
// To make palette management more automatic, ColourPairs could register at 
// construction time with a palette management object.
struct ColourPair {
	Colour desired;
	Colour allocated;

	ColourPair(Colour desired_=Colour(0,0,0)) {
		desired = desired_;
		allocated = desired;
	}
};

class Window;	// Forward declaration for Palette

class Palette {
	int used;
	enum {numEntries = 100};
	ColourPair entries[numEntries];
#if PLAT_GTK
	GdkColor *allocatedPalette;
	int allocatedLen;
#elif PLAT_WIN
	HPALETTE hpal;
#elif PLAT_WX
	// wxPalette* pal;  // **** Is this needed?
#endif
public:
	bool allowRealization;
	
	Palette();
	~Palette();

	void Release();
	
	// This method either adds a colour to the list of wanted colours (want==true)
	// or retrieves the allocated colour back to the ColourPair.
	// This is one method to make it easier to keep the code for wanting and retrieving in sync.
	void WantFind(ColourPair &cp, bool want);

	void Allocate(Window &w);
	
	friend class Surface;
};

class Font {
	FontID id;
#if PLAT_WX
	int ascent;
#endif
	// Private so Font objects can not be copied
	Font(const Font &) {}
	Font &operator=(const Font &) { id=0; return *this; }
public:
	Font();
	~Font();

	void Create(const char *faceName, int size, bool bold=false, bool italic=false);
	void Release();

	FontID GetID() { return id; }
	friend class Surface;
};

// A surface abstracts a place to draw
class Surface {
private:
#if PLAT_GTK
	GdkDrawable *drawable;
	GdkGC *gc;
	GdkPixmap *ppixmap;
	int x;
	int y;
	bool inited;
	bool createdGC;
#elif PLAT_WIN
	HDC hdc;
	bool hdcOwned;
	HPEN pen;
	HPEN penOld;
	HBRUSH brush;
	HBRUSH brushOld;
	HFONT font;
	HFONT fontOld;
	HBITMAP bitmap;
	HBITMAP bitmapOld;
	HPALETTE paletteOld;
#elif PLAT_WX
	wxDC* hdc;
	bool hdcOwned;
	wxBitmap* bitmap;
	int x;
	int y;
#endif

	// Private so Surface objects can not be copied
	Surface(const Surface &) {}
	Surface &operator=(const Surface &) { return *this; }
#if PLAT_WIN || PLAT_WX
	void BrushColor(Colour back);
	void SetFont(Font &font_);
#endif
public:
	Surface();
	~Surface();
	
	void Init();
	void Init(SurfaceID hdc_);
	void InitPixMap(int width, int height, Surface *surface_);

	void Release();
	bool Initialised();
	void PenColour(Colour fore);
	int LogPixelsY();
	void MoveTo(int x_, int y_);
	void LineTo(int x_, int y_);
	void Polygon(Point *pts, int npts, Colour fore, Colour back);
	void RectangleDraw(PRectangle rc, Colour fore, Colour back);
	void FillRectangle(PRectangle rc, Colour back);
	void FillRectangle(PRectangle rc, Surface &surfacePattern);
	void RoundedRectangle(PRectangle rc, Colour fore, Colour back);
	void Ellipse(PRectangle rc, Colour fore, Colour back);
	void Copy(PRectangle rc, Point from, Surface &surfaceSource);

	void DrawText(PRectangle rc, Font &font_, int ybase, const char *s, int len, Colour fore, Colour back);
	void DrawTextClipped(PRectangle rc, Font &font_, int ybase, const char *s, int len, Colour fore, Colour back);
	void MeasureWidths(Font &font_, const char *s, int len, int *positions);
	int WidthText(Font &font_, const char *s, int len);
	int WidthChar(Font &font_, char ch);
	int Ascent(Font &font_);
	int Descent(Font &font_);
	int InternalLeading(Font &font_);
	int ExternalLeading(Font &font_);
	int Height(Font &font_);
	int AverageCharWidth(Font &font_);
	
	int SetPalette(Palette *pal, bool inBackGround);
	void SetClip(PRectangle rc);
};

// Class to hide the details of window manipulation
// Does not own the window which will normally have a longer life than this object
class Window {
	friend class ListBox;
protected:
	WindowID id;
public:
	Window() : id(0) {}
	virtual ~Window();
	Window &operator=(WindowID id_) {
		id = id_;
		return *this;
	}
	WindowID GetID() { return id; }
	bool Created() { return id != 0; }
	void Destroy();
	bool HasFocus();
	PRectangle GetPosition();
	void SetPosition(PRectangle rc);
	void SetPositionRelative(PRectangle rc, Window relativeTo);
	PRectangle GetClientPosition();
	void Show(bool show=true);
	void InvalidateAll();
	void InvalidateRectangle(PRectangle rc);
	void SetFont(Font &font);
	enum Cursor { cursorText, cursorArrow, cursorUp, cursorWait, cursorHoriz, cursorVert, cursorReverseArrow };
	void SetCursor(Cursor curs);
	void SetTitle(const char *s);
#if PLAT_WIN
	LRESULT SendMessage(UINT msg, WPARAM wParam=0, LPARAM lParam=0);
	int GetDlgCtrlID();
	HINSTANCE GetInstance();
#endif
};

class ListBox : public Window {
#if PLAT_GTK
	WindowID list;
	WindowID scroller;
	int current;
#endif
public:
	ListBox();
	virtual ~ListBox();
	ListBox &operator=(WindowID id_) {
		id = id_;
		return *this;
	}
	void Create(Window &parent, int ctrlID);
	void Clear();
	void Append(char *s);
	int Length();
	void Select(int n);
	int GetSelection();
	int Find(const char *prefix);
	void GetValue(int n, char *value, int len);
	void Sort();
};

class Menu {
	MenuID id;
public:
	Menu();
	MenuID GetID() { return id; }
	void CreatePopUp();
	void Destroy();
	void Show(Point pt, Window &w);
};

// Platform class used to retrieve system wide parameters such as double click speed
// and chrome colour. Not a creatable object, more of a module with several functions.
class Platform {
	// Private so Platform objects can not be copied
	Platform(const Platform &) {}
	Platform &operator=(const Platform &) { return *this; }
public:
	// Should be private because no new Platforms are ever created
	// but gcc warns about this
	Platform() {}
	~Platform() {}
	static Colour Chrome();
	static Colour ChromeHighlight();
	static const char *DefaultFont();
	static int DefaultFontSize();
	static unsigned int DoubleClickTime();
	static void DebugDisplay(const char *s);
	static bool IsKeyDown(int key);
	static long SendScintilla(
		WindowID w, unsigned int msg, unsigned long wParam=0, long lParam=0);
	
	// These are utility functions not really tied to a platform
	static int Minimum(int a, int b);
	static int Maximum(int a, int b);
	static void DebugPrintf(const char *format, ...);
	static int Clamp(int val, int minVal, int maxVal);
};

#endif
