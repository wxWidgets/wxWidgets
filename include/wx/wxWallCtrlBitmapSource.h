#pragma once
#include "wxwallctrldatasource.h"

class wxWallCtrlBitmapSource :
	public wxWallCtrlDataSource
{
public:
	wxWallCtrlBitmapSource(void);
	virtual ~wxWallCtrlBitmapSource(void);

	virtual wxWallCtrlRenderType GetRenderType(wxWallCtrlItemID& itemId, const wxSize& availableResolution, wxSize& desiredResolution)
	{
		return wxWallCtrlRenderType::wxWallCtrlRenderBitmap;
	}

	virtual bool RenderItem(wxWallCtrlItemID& itemId, wxDC& dc, const wxSize& dcSize)
	{
		// TODO: Do error checking for itemID existence if needed
		// Create a temp memory DC to hold the bitmap
		wxMemoryDC tempDC;
		tempDC.SelectObject(GetBitmap(itemId));

		// TODO: Check what width & height should be supplied
		// Blit to the supplied DC
		dc.Blit(0, 0, dcSize.GetWidth(), dcSize.GetHeight(), &tempDC, 0, 0);
		return true;
	}
	virtual wxBitmap wxWallCtrlDataSource::GetBitmap(wxWallCtrlItemID& itemId)
	{
		return bitmaps[itemId];
	}
	virtual bool GetItemInfo(wxWallCtrlItemID& itemId, wxWallCtrlItem& info)
	{
		return 0;
	}

	virtual unsigned GetCount()
	{
		return bitmaps.size();
	}

	// Appends a bitmap at the end of the list and give it an ordered id
	void AppendBitmap(wxBitmap bitmap)
	{
		bitmaps[bitmaps.size()] = bitmap;
	}

	void PrepareTextures()
	{
		// Render just one item for now

		bool b = tempTexture.LoadFile("S:\\Wall.bmp", wxBITMAP_TYPE_BMP);
		int t = tempTexture.GetHeight();


		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &texName);
		glBindTexture(GL_TEXTURE_2D, texName);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		GLubyte * tex = new GLubyte[tempTexture.GetWidth()*tempTexture.GetHeight()*BYTES_PER_PIXEL];
		CreateTexture(tempTexture, tex);

		bool useMipmaps = false;

		if (useMipmaps)
		{
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tempTexture.GetWidth(), tempTexture.GetHeight(),	GL_RGBA, GL_UNSIGNED_BYTE, tex);
		}
		else
		{
			//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, tempTexture.GetWidth(), tempTexture.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);
		}

	}


private:
	//WX_DEFINE_ARRAY(wxBitmap, ArrayOfBitmaps);
	WX_DECLARE_HASH_MAP(int, wxBitmap, wxIntegerHash, wxIntegerEqual, BitmapMap);

	BitmapMap bitmaps;
};
