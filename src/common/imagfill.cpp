/////////////////////////////////////////////////////////////////////////////
// Name:        imagfill.cpp
// Purpose:     FloodFill for wxImage
// Author:      
// RCS-ID:      $Id$
// Copyright:   
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_IMAGE

#include "wx/image.h"

#ifndef WX_PRECOMP
    #include "wx/brush.h"
#endif

// DoFloodFill
// Fills with the colour extracted from fillBrush, starting at x,y until either
// a color different from the start pixel is reached (wxFLOOD_SURFACE)
// or fill color is reached (wxFLOOD_BORDER)

bool wxImage::MatchPixel(int x, int y, int w, int h, const wxColour & c)
{
    if ((x<0)||(x>=w)||(y<0)||(y>=h)) return false;

    unsigned char r = GetRed(x,y);
    unsigned char g = GetGreen(x,y);
    unsigned char b = GetBlue(x,y);
    return c.Red() == r && c.Green() == g && c.Blue() == b ;
}

bool wxImage::MatchBoundaryPixel(int x, int y, int w, int h, const wxColour & fill, const wxColour & bound)
{
    if ((x<0)||(x>=w)||(y<0)||(y>=h)) return TRUE;

    unsigned char r = GetRed(x,y);
    unsigned char g = GetGreen(x,y);
    unsigned char b = GetBlue(x,y);
    if ( fill.Red() == r && fill.Green() == g && fill.Blue() == b ) return TRUE;
    if ( bound.Red() == r && bound.Green() == g && bound.Blue() == b ) return TRUE;
    return FALSE ;
}


void wxImage::DoFloodFill (wxCoord x, wxCoord y, const wxBrush & fillBrush,
        const wxColour& testColour, int style /*=wxFLOOD_SURFACE */,
        int LogicalFunction /*= wxCOPY, currently unused */)
{
    /* A diamond flood-fill using a circular queue system.
    Each pixel surrounding the current pixel is added to
    the queue if it meets the criteria, then is retrieved in
    its turn.  Code originally based on http://www.drawit.co.nz/Developers.htm */

    int width = GetWidth();
    int height = GetHeight();

    //Draw using a pen made from the current brush colour
    //Potentially allows us to use patterned flood fills in future code
    wxColour fillColour = fillBrush.GetColour();
    unsigned char r = fillColour.Red();
    unsigned char g = fillColour.Green();
    unsigned char b = fillColour.Blue();

    //initial test :
    if (style == wxFLOOD_SURFACE)
    {
       //if wxFLOOD_SURFACE, if fill colour is same as required, we don't do anything
       if (     GetRed(x,y)   != r
             || GetGreen(x,y) != g
             || GetBlue (x,y) != b   )
        {
        //prepare memory for queue
        //queue save, start, read
        size_t *qs, *qst, *qr;

        //queue size (physical)
        long qSz= height * width * 2;
        qst = new size_t [qSz];

        //temporary x and y locations
        int xt, yt;

        for (int i=0; i < qSz; i++)
            qst[i] = 0;

        // start queue
        qs=qr=qst;
        *qs=xt=x;
        qs++;
        *qs=yt=y;
        qs++;

        SetRGB(xt,yt,r,g,b);

        //Main queue loop
        while(qr!=qs)
        {
            //Add new members to queue
            //Above current pixel
            if(MatchPixel(xt,yt-1,width,height,testColour))
            {
                *qs=xt;
                qs++;
                *qs=yt-1;
                qs++;
                SetRGB(xt,yt-1,r,g,b);

                //Loop back to beginning of queue
                if(qs>=(qst+qSz)) qs=qst;
            }

            //Below current pixel
            if(MatchPixel(xt,yt+1,width,height,testColour))
            {
                *qs=xt;
                qs++;
                *qs=yt+1;
                qs++;
                SetRGB(xt,yt+1,r,g,b);
                if(qs>=(qst+qSz)) qs=qst;
            }

            //Left of current pixel
            if(MatchPixel(xt-1,yt,width,height,testColour))
            {
                *qs=xt-1;
                qs++;
                *qs=yt;
                qs++;
                SetRGB(xt-1,yt,r,g,b);
                if(qs>=(qst+qSz)) qs=qst;
            }

            //Right of current pixel
            if(MatchPixel(xt+1,yt,width,height,testColour))
            {
                *qs=xt+1;
                qs++;
                *qs=yt;
                qs++;
                SetRGB(xt+1,yt,r,g,b);
                if(qs>=(qst+qSz)) qs=qst;
            }

            //Retrieve current queue member
            qr+=2;

            //Loop back to the beginning
            if(qr>=(qst+qSz)) qr=qst;
            xt=*qr;
            yt=*(qr+1);

        //Go Back to beginning of loop
        }

        delete [] qst ;
        }
    }
    else
    {
    //style is wxFLOOD_BORDER
    // fill up to testColor border - if already testColour don't do anything
    if (  GetRed(x,y)   != testColour.Red()
       || GetGreen(x,y) != testColour.Green()
       || GetBlue(x,y)  != testColour.Blue()   )
        {
        //prepare memory for queue
        //queue save, start, read
        size_t *qs, *qst, *qr;

        //queue size (physical)
        long qSz= height * width * 2;
        qst = new size_t [qSz];

        //temporary x and y locations
        int xt, yt;

        for (int i=0; i < qSz; i++)
            qst[i] = 0;

        // start queue
        qs=qr=qst;
        *qs=xt=x;
        qs++;
        *qs=yt=y;
        qs++;

        SetRGB(xt,yt,r,g,b);

        //Main queue loop
        while(qr!=qs)
        {
            //Add new members to queue
            //Above current pixel
            if(!MatchBoundaryPixel(xt,yt-1,width,height,fillColour,testColour))
            {
                *qs=xt;
                qs++;
                *qs=yt-1;
                qs++;
                SetRGB(xt,yt-1,r,g,b);

                //Loop back to beginning of queue
                if(qs>=(qst+qSz)) qs=qst;
            }

            //Below current pixel
            if(!MatchBoundaryPixel(xt,yt+1,width,height,fillColour,testColour))
            {
                *qs=xt;
                qs++;
                *qs=yt+1;
                qs++;
                SetRGB(xt,yt+1,r,g,b);
                if(qs>=(qst+qSz)) qs=qst;
            }

            //Left of current pixel
            if(!MatchBoundaryPixel(xt-1,yt,width,height,fillColour,testColour))
            {
                *qs=xt-1;
                qs++;
                *qs=yt;
                qs++;
                SetRGB(xt-1,yt,r,g,b);
                if(qs>=(qst+qSz)) qs=qst;
            }

            //Right of current pixel
            if(!MatchBoundaryPixel(xt+1,yt,width,height,fillColour,testColour))
            {
                *qs=xt+1;
                qs++;
                *qs=yt;
                qs++;
                SetRGB(xt+1,yt,r,g,b);
                if(qs>=(qst+qSz)) qs=qst;
            }

            //Retrieve current queue member
            qr+=2;

            //Loop back to the beginning
            if(qr>=(qst+qSz)) qr=qst;
            xt=*qr;
            yt=*(qr+1);

        //Go Back to beginning of loop
        }

        delete [] qst ;
        }
    }
    //all done,
}

#endif // wxUSE_IMAGE

