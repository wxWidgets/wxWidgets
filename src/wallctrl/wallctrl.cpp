#include "wx/wx.h"


#if !wxUSE_GLCANVAS
#error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif


#include "wx/Wallctrl/WallCtrl.h"

BEGIN_EVENT_TABLE(wxWallCtrl, wxGLCanvas)
//	EVT_SIZE(wxWallCtrl::OnSize)
	EVT_PAINT(wxWallCtrl::OnPaint)
	EVT_ERASE_BACKGROUND(wxWallCtrl::OnEraseBackground)
//	EVT_KEY_DOWN( wxWallCtrl::OnKeyDown )
//	EVT_KEY_UP( wxWallCtrl::OnKeyUp )
//	EVT_ENTER_WINDOW( wxWallCtrl::OnEnterWindow )
END_EVENT_TABLE()

#if 0
// This code is from NeHe
AUX_RGBImageRec *LoadBMP(const char *Filename)				// Loads A Bitmap Image
{
	FILE *File=NULL;									// File Handle

	if (!Filename)										// Make Sure A Filename Was Given
	{
		return NULL;									// If Not Return NULL
	}

	File=fopen(Filename,"r");							// Check To See If The File Exists

	if (File)											// Does The File Exist?
	{
		fclose(File);									// Close The Handle
		return auxDIBImageLoad(Filename);				// Load The Bitmap And Return A Pointer
	}

	return NULL;										// If Load Failed Return NULL
}

// Load Bitmaps And Convert To Textures
int LoadGLTexture(GLuint * p_rTextureName, const char * p_FileName, bool p_bMIPMap /*= false*/)
{
	int Status=FALSE;									// Status Indicator

	AUX_RGBImageRec *TextureImage[1];					// Create Storage Space For The Texture

	memset(TextureImage,0,sizeof(void *)*1);           	// Set The Pointer To NULL

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if (TextureImage[0]=LoadBMP(p_FileName))
	{
		Status=TRUE;									// Set The Status To TRUE

		glGenTextures(1, p_rTextureName);					// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, p_rTextureName[0]);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

		if (p_bMIPMap)
		{
			gluBuild2DMipmaps( GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY,	GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
		}
		else
		{
			//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
		}



	}

	if (TextureImage[0])									// If Texture Exists
	{
		if (TextureImage[0]->data)							// If Texture Image Exists
		{
			free(TextureImage[0]->data);					// Free The Texture Image Memory
		}

		free(TextureImage[0]);								// Free The Image Structure
	}

	return Status;										// Return The Status
}
#endif
