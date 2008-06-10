// ----------------------------------------------------------------------------
// wxWallCtrl: the Wall Control
// Mokhtar M. Khorshid
// ----------------------------------------------------------------------------

#include "wx/glcanvas.h"
#include "gl/glut.h"
#include "wx/string.h"
#include "wx/wxWallCtrlDataSource.h"

#define BYTES_PER_PIXEL 4

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
	wxWallCtrl(wxWindow *parent, wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0, const wxString & name = _T("WallCtrl") )
		:wxGLCanvas(parent, (wxGLCanvas*) NULL, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name )
	{
		m_init= false;
	}

	wxWallCtrl(wxWindow *parent, const wxWallCtrl *other,
		wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
		const wxString& name )
		: wxGLCanvas(parent, other->GetContext(), id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name)
	{
		m_init=false;
	}

	// Initialize OpenGL for first use

	void InitGL()
	{
		SetCurrent();

		/* set viewing projection */
		glMatrixMode(GL_PROJECTION);
		glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);

		/* position viewer */
		glMatrixMode(GL_MODELVIEW);
		glTranslatef(0.0f, 0.0f, -2.0f);


		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_LIGHTING);
		//glEnable(GL_LIGHT0);

		PrepareTextures();
	}

	// Render the control (and all its items)
	void Render()
	{
		SetCurrent();
		// Init OpenGL once, but after SetCurrent
		if (!m_init)
		{
			InitGL();
			m_init = true;
		}

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 5.0f);
		glMatrixMode(GL_MODELVIEW);


		/* clear color and depth buffers */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// This will be replaced soon
		TempRenderItems();

		glFlush();
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


	// This is a temp method that should be integrated with the surfaces, it renders all the items in the source
	void TempRenderItems()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

		for (int i=0; i < m_dataSource->GetCount(); ++i)
		{
//			m_dataSource->RenderItem(i, )
//			GLuint texName;
			glBindTexture(GL_TEXTURE_2D, texName);
			glBegin(GL_QUADS);							// Start drawing the face
			// Face 3
			glTexCoord2f(0.0, 0.0);
			glVertex3f( 1.0f, 1.0f, 1.0f);			// Top Right
			glTexCoord2f(0.0, 1.0);
			glVertex3f(-1.0f, 1.0f, 1.0f);			// Top Left
			glTexCoord2f(1.0, 1.0);
			glVertex3f(-1.0f,-1.0f, 1.0f);			// Bottom Left
			glTexCoord2f(1.0, 0.0);
			glVertex3f( 1.0f,-1.0f, 1.0f);			// Bottom Right
			glEnd();

		}

		glDisable(GL_TEXTURE_2D);
		// We are assuming the data source holds ordered items in succession
		//for (int i=0; i < m_dataSource->GetCount(); ++i)
		{

		}
	}

	// Loads the bitmap into the texture
	// Precondition: Texture must be at least Width * Height * BYTES_PER_PIXEL
	void CreateTexture(wxBitmap bitmap, GLubyte * texture)
	{
		// TODO: Check for more robust implementation

		// Create a memory DC
		wxMemoryDC tempDC;
		tempDC.SelectObject(bitmap);
		tempDC.

		// Temp color variable
		wxColor color;
		// Query each pixel
		for (int y=0; y < bitmap.GetHeight(); ++y)
		{
			for (int x=0; x < bitmap.GetWidth(); ++x)
			{
				// Sequential pixel position
				unsigned pixel = y*bitmap.GetWidth() + x;

				tempDC.GetPixel(x, y, &color);

				*(texture + pixel*BYTES_PER_PIXEL + 0) = color.Red();
				*(texture + pixel*BYTES_PER_PIXEL + 1) = color.Green();
				*(texture + pixel*BYTES_PER_PIXEL + 2) = color.Blue();
				*(texture + pixel*BYTES_PER_PIXEL + 3) = 255;
			}
		}
	}
	DECLARE_EVENT_TABLE()

	private:
		bool m_init;
		
		// TODO: Used for testing only
		wxBitmap tempTexture;


		// This is the data source that will supply the bitmaps
		wxWallCtrlDataSource * m_dataSource;
//		wxVectorBase <int> m_ImageFilenames;	// TODO: This is only a temp test variable, remove ASAP
};