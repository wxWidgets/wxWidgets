// ----------------------------------------------------------------------------
// wxWallCtrl: the Wall Control
// Mokhtar M. Khorshid
// ----------------------------------------------------------------------------

#include "wx/glcanvas.h"

#include "wx/string.h"
#include "wx/Wallctrl/WallCtrlDataSource.h"

#include "wx/Wallctrl/WallCtrlSurface.h"


// For some reason wxVector< > does not compile, so for the meanwhile we'll use this vector
template <class T>
struct TempVector
{
	unsigned size()
	{
		return m_size;
	}
	int m_size;
};

class WXDLLEXPORT wxWallCtrl : public wxGLCanvas
{
public:
	// Constructors
	wxWallCtrl(wxWindow *parent, wxWallCtrlSurface *surface,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0, const wxString & name = _T("WallCtrl") )
		:wxGLCanvas(parent, (wxGLCanvas*) NULL, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name ), m_surface(surface)
	{
		m_init= false;
	}

	wxWallCtrl(wxWindow *parent, wxWallCtrlSurface *surface, const wxWallCtrl *other,
		wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
		const wxString& name )
		: wxGLCanvas(parent, other->GetContext(), id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name),m_surface(surface)
	{
		m_init=false;
	}

	// Render the control (and all its items)
	void Render()
	{
		SetCurrent();
		m_surface->Render();
		SwapBuffers();
	}

	void OnEnterWindow( wxMouseEvent& WXUNUSED(event) )
	{
		SetFocus();
	}

	void OnPaint( wxPaintEvent& WXUNUSED(event) )
	{
		Render();
	}

	void OnSize(wxSizeEvent& event)
	{
		// this is also necessary to update the context on some platforms
		wxGLCanvas::OnSize(event);
	}

	void OnEraseBackground(wxEraseEvent& WXUNUSED(event))
	{
		// Do nothing, to avoid flashing.
	}


	


	DECLARE_EVENT_TABLE()

	private:
		bool m_init;
		
		// TODO: Used for testing only
		wxBitmap tempTexture;


		// This is the data source that will supply the bitmaps
		wxWallCtrlDataSource * m_dataSource;

		// This will point to a single surface that is currently selected
		wxWallCtrlSurface * m_surface;

//		wxVectorBase <int> m_ImageFilenames;	// TODO: This is only a temp test variable, remove ASAP
};