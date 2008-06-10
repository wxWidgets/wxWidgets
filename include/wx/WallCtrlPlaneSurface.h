#pragma once
#include "wallctrlsurface.h"
#include "gl/glut.h"

#define BYTES_PER_PIXEL 4

class wxWallCtrlPlaneSurface :
	public wxWallCtrlSurface
{
public:
	// Precondition: dataSource must be valid
	wxWallCtrlPlaneSurface(wxWallCtrlDataSource * dataSource):wxWallCtrlSurface(dataSource)
	{
		m_init = false;
		m_scopeSize.SetHeight(1);
		m_scopeSize.SetWidth(1);
		m_firstItem = m_dataSource->GetFirstItem();
	}
	virtual ~wxWallCtrlPlaneSurface(void);


	void Render()
	{
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
	}

	// Loads a texture from the DC into the supplied array
	// Precondition: Texture must be at least Width * Height * BYTES_PER_PIXEL
	void CreateTextureFromDC(wxMemoryDC &dc, GLubyte * texture, const wxSize& dcSize)
	{
		// Temp color variable
		wxColor color;
		
		// Query each pixel
		for (int y=0; y < dcSize.GetHeight(); ++y)
		{
			for (int x=0; x < dcSize.GetWidth(); ++x)
			{
				// Sequential pixel position
				unsigned pixel = y*dcSize.GetWidth() + x;

				dc.GetPixel(x, y, &color);

				*(texture + pixel*BYTES_PER_PIXEL + 0) = color.Red();
				*(texture + pixel*BYTES_PER_PIXEL + 1) = color.Green();
				*(texture + pixel*BYTES_PER_PIXEL + 2) = color.Blue();
				*(texture + pixel*BYTES_PER_PIXEL + 3) = 255;
			}
		}
	}

	// Loads a bitmap into a texture array
	// Precondition: Texture must be at least Width * Height * BYTES_PER_PIXEL
	void CreateTextureFromBitmap(wxBitmap bitmap, GLubyte * texture)
	{
		// Create a memory DC to hold the bitmap
		wxMemoryDC tempDC;
		tempDC.SelectObject(bitmap);

		// Now load from the DC to the array
		CreateTextureFromDC(tempDC, texture, wxSize(bitmap.GetWidth(), bitmap.GetHeight()));
	}

	GLuint GetItemTexture(wxWallCtrlItemID itemID)
	{
		// If that texture was previously loaded
		if (texturesCache.find( itemID ) != texturesCache.end())
		{
			// Then just return its cached name without reloading it
			return texturesCache[itemID];
		}
		
		// We will use the itemID as the texture identifier since item names are guaranteed to be unique
		GLuint texName = itemID + 1;	// We add one to make it 1-based

		// Query the item to get information about it
		wxWallCtrlItem info;
		m_dataSource->GetItemInfo(itemID, info);

		// We must let the source render the texture for us, so we prepare a correctly sized DC for it
		wxMemoryDC tempDC;
		tempDC.SelectObject(wxBitmap (info.size.GetWidth(), info.size.GetHeight()));
		
		// Let the source draw the item
		m_dataSource->RenderItem(itemID, tempDC, info.size);
		
		// Start creating the texture
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		
		//glGenTextures(1, &texName);	// Already have a name so we don't need this

		// Select this texture
		glBindTexture(GL_TEXTURE_2D, texName);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// Allocate enough space for the texture
		GLubyte * tex = new GLubyte[info.size.GetWidth()*info.size.GetHeight()*BYTES_PER_PIXEL];
		
		//CreateTextureFromBitmap(m_dataSource->GetBitmap(itemID), tex);
		CreateTextureFromDC(tempDC, tex, info.size);	// This should be used instead


		bool useMipmaps = true;

		if (useMipmaps)
		{
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, info.size.GetWidth(), info.size.GetHeight(),	GL_RGBA, GL_UNSIGNED_BYTE, tex);
		}
		else
		{
			//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, info.size.GetWidth(), info.size.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);
		}

		// We don't need the texture array any more.
		delete [] tex;

		// Cache the texture name before returning it
		return texturesCache[itemID]=texName;
	}

	void InitGL()
	{
		//SetCurrent();	// This is done outside now

		/* set viewing projection */
		glMatrixMode(GL_PROJECTION);
		glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);
		//gluPerspective(45.0,(GLfloat)400/(GLfloat)400,0.01f,10000.0f);

		/* position viewer */
		glMatrixMode(GL_MODELVIEW);
		glTranslatef(0.0f, 0.0f, -2.0f);


		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_LIGHTING);
		//glEnable(GL_LIGHT0);
	}



	// This is a temp method that should be integrated with the surfaces, it renders all the items in the source
	void TempRenderItems()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

		// Loop over the grid-like scope window
		for (int y=0; y < m_scopeSize.GetHeight(); ++y)
		{
			for (int x=0; x < m_scopeSize.GetWidth(); ++x)
			{
				// Get the index of the item at this position
				int Index = m_firstItem + y * m_scopeSize.GetWidth() + x;


			}
		}
		for (unsigned i=0; i < m_dataSource->GetCount(); ++i)
		{
			//			m_dataSource->RenderItem(i, )
			//			GLuint texName;
			glBindTexture(GL_TEXTURE_2D, GetItemTexture(i));
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
	}

	// Sets the number of elements in a single frame (in both directions)
	void SetScopeSize(wxSize size)
	{
		if ((size.GetHeight() < 1) || (size.GetWidth() < 1))
		{
			// TODO: This is a serious issue, can't proceed
			assert(false);
			return;
		}
		m_scopeSize = size;
	}

	private:
		bool m_init;
		
		// The number of rows the surface will put the items in
		unsigned m_rowsCount;

		// The index of the first item of the current scope. NOTE: Items are not sequential inside the scope
		unsigned m_firstItem;

		// This is the 2D window in number of items in each dimension
		wxSize m_scopeSize;

		// A map to hold the texture names for items that were previously cached
		WX_DECLARE_HASH_MAP(int, int, wxIntegerHash, wxIntegerEqual, Int2IntMap);
		Int2IntMap texturesCache;
};
