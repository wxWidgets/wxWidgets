#define _GNU_SOURCE
#include "../gdk_imlib/gdk_imlib.h"
#include "../gdk_imlib/gdk_imlib_private.h"

GdkPixmap          *
gdk_imlib_pixmap_foreign_new(gint width, gint height,
			     gint depth, Pixmap pmap)
{
   GdkPixmap          *pixmap;
   GdkWindowPrivate   *private;
   GdkWindowPrivate   *window_private;

   private = g_new(GdkWindowPrivate, 1);
   pixmap = (GdkPixmap *) private;

   window_private = (GdkWindowPrivate *) id->x.gdk_win;

   private->xdisplay = window_private->xdisplay;
   private->window_type = GDK_WINDOW_PIXMAP;
   private->xwindow = pmap;
   private->colormap = id->x.gdk_cmap;
   private->children = NULL;
   private->parent = NULL;
   private->x = 0;
   private->y = 0;
   private->width = width;
   private->height = height;
   private->resize_count = 0;
   private->ref_count = 1;
   private->destroyed = 0;

   gdk_xid_table_insert(&private->xwindow, pixmap);

   return pixmap;
}

gint
gdk_imlib_best_color_match(gint * r, gint * g, gint * b)
{
   int                 i;
   int                 dif;
   int                 dr, dg, db;
   int                 col;
   int                 mindif = 0x7fffffff;
   XColor              xcl;

   col = 0;
   if (!id)
     {
	fprintf(stderr, "ImLib ERROR: No ImlibData initialised\n");
	return -1;
     }
   if ((id->render_type == RT_PLAIN_TRUECOL) ||
       (id->render_type == RT_DITHER_TRUECOL))
     {
	xcl.red = (unsigned short)((*r << 8) | (*r));
	xcl.green = (unsigned short)((*g << 8) | (*g));
	xcl.blue = (unsigned short)((*b << 8) | (*b));
	xcl.flags = DoRed | DoGreen | DoBlue;
	XAllocColor(id->x.disp, id->x.root_cmap, &xcl);
	*r = xcl.red >> 8;
	*g = xcl.green >> 8;
	*b = xcl.blue >> 8;
	return xcl.pixel;
     }
   for (i = 0; i < id->num_colors; i++)
     {
	dr = *r - id->palette[i].r;
	if (dr < 0)
	   dr = -dr;
	dg = *g - id->palette[i].g;
	if (dg < 0)
	   dg = -dg;
	db = *b - id->palette[i].b;
	if (db < 0)
	   db = -db;
	dif = dr + dg + db;
	if (dif < mindif)
	  {
	     mindif = dif;
	     col = i;
	  }
     }
   *r -= id->palette[col].r;
   *g -= id->palette[col].g;
   *b -= id->palette[col].b;
   col = id->palette[col].pixel;
   return col;
}

gint
gindex_best_color_match(gint * r, gint * g, gint * b)
{
   int                 i;
   int                 dif;
   int                 dr, dg, db;
   int                 col;
   int                 mindif = 0x7fffffff;
   XColor              xcl;

   col = 0;
   if (!id)
     {
	fprintf(stderr, "ImLib ERROR: No ImlibData initialised\n");
	return -1;
     }
   if ((id->render_type == RT_PLAIN_TRUECOL) ||
       (id->render_type == RT_DITHER_TRUECOL))
     {
	xcl.red = (unsigned short)((*r << 8) | (*r));
	xcl.green = (unsigned short)((*g << 8) | (*g));
	xcl.blue = (unsigned short)((*b << 8) | (*b));
	xcl.flags = DoRed | DoGreen | DoBlue;
	XAllocColor(id->x.disp, id->x.root_cmap, &xcl);
	*r = xcl.red >> 8;
	*g = xcl.green >> 8;
	*b = xcl.blue >> 8;
	return xcl.pixel;
     }
   for (i = 0; i < id->num_colors; i++)
     {
	dr = *r - id->palette[i].r;
	if (dr < 0)
	   dr = -dr;
	dg = *g - id->palette[i].g;
	if (dg < 0)
	   dg = -dg;
	db = *b - id->palette[i].b;
	if (db < 0)
	   db = -db;
	dif = dr + dg + db;
	if (dif < mindif)
	  {
	     mindif = dif;
	     col = i;
	  }
     }
   *r -= id->palette[col].r;
   *g -= id->palette[col].g;
   *b -= id->palette[col].b;
   return col;
}

void
grender_shaped_15_fast_dither(GdkImlibImage * im, int w, int h, XImage * xim,
			      XImage * sxim, int *er1, int *er2, int *xarray,
			      unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;
   unsigned short     *img;
   int                 jmp;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	ter = er1;
	er1 = er2;
	er2 = ter;
	for (ex = 0; ex < (w + 2) * 3; ex++)
	   er2[ex] = 0;
	ex = 3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
	       {
		  XPutPixel(sxim, x, y, 0);
		  img++;
		  ex += 3;
	       }
	     else
	       {
		  XPutPixel(sxim, x, y, 1);
		  er = r + er1[ex++];
		  eg = g + er1[ex++];
		  eb = b + er1[ex++];
		  if (er > 255)
		     er = 255;
		  if (eg > 255)
		     eg = 255;
		  if (eb > 255)
		     eb = 255;
		  val = ((er & 0xf8) << 7) | ((eg & 0xf8) << 2) | ((eb & 0xf8) >> 3);
		  er = er & 0x07;
		  eg = eg & 0x07;
		  eb = eb & 0x07;
		  DITHER_ERROR(er1, er2, ex, er, eg, eb);
		  *img++ = val;
	       }
	  }
	img += jmp;
     }
}

void
grender_shaped_15_fast_dither_ordered(GdkImlibImage * im, int w, int h, XImage * xim,
			       XImage * sxim, int *er1, int *er2, int *xarray,
				      unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;
   unsigned short     *img;
   int                 jmp;

   unsigned char       dither[4][4] =
   {
      {0, 4, 6, 5},
      {6, 2, 7, 3},
      {2, 6, 1, 5},
      {7, 4, 7, 3}
   };
   int                 dithy, dithx;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	dithy = y & 0x3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
	       {
		  XPutPixel(sxim, x, y, 0);
		  img++;
	       }
	     else
	       {
		  XPutPixel(sxim, x, y, 1);
		  er = r & 0x07;
		  eg = g & 0x07;
		  eb = b & 0x07;
		  dithx = x & 0x3;
		  if ((dither[dithy][dithx] < er) && (r < (256 - 8)))
		     r += 8;
		  if ((dither[dithy][dithx] < eg) && (g < (256 - 8)))
		     g += 8;
		  if ((dither[dithy][dithx] < eb) && (b < (256 - 8)))
		     b += 8;
		  val = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
		  *img++ = val;
	       }
	  }
	img += jmp;
     }
}

void
grender_15_fast_dither(GdkImlibImage * im, int w, int h, XImage * xim,
		       XImage * sxim, int *er1, int *er2, int *xarray,
		       unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;
   unsigned short     *img;
   int                 jmp;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	ter = er1;
	er1 = er2;
	er2 = ter;
	for (ex = 0; ex < (w + 2) * 3; ex++)
	   er2[ex] = 0;
	ex = 3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     er = r + er1[ex++];
	     eg = g + er1[ex++];
	     eb = b + er1[ex++];
	     if (er > 255)
		er = 255;
	     if (eg > 255)
		eg = 255;
	     if (eb > 255)
		eb = 255;
	     val = ((er & 0xf8) << 7) | ((eg & 0xf8) << 2) | ((eb & 0xf8) >> 3);
	     er = er & 0x07;
	     eg = eg & 0x07;
	     eb = eb & 0x07;
	     DITHER_ERROR(er1, er2, ex, er, eg, eb);
	     *img++ = val;
	  }
	img += jmp;
     }
}

void
grender_15_fast_dither_ordered(GdkImlibImage * im, int w, int h, XImage * xim,
			       XImage * sxim, int *er1, int *er2, int *xarray,
			       unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   unsigned short     *img;
   int                 jmp;

   unsigned char       dither[4][4] =
   {
      {0, 4, 6, 5},
      {6, 2, 7, 3},
      {2, 6, 1, 5},
      {7, 4, 7, 3}
   };
   int                 dithy, dithx;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	dithy = y & 0x3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     er = r & 0x07;
	     eg = g & 0x07;
	     eb = b & 0x07;
	     dithx = x & 0x3;
	     if ((dither[dithy][dithx] < er) && (r < (256 - 8)))
		r += 8;
	     if ((dither[dithy][dithx] < eg) && (g < (256 - 8)))
		g += 8;
	     if ((dither[dithy][dithx] < eb) && (b < (256 - 8)))
		b += 8;
	     val = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
	     *img++ = val;
	  }
	img += jmp;
     }
}

void
grender_shaped_16_fast_dither(GdkImlibImage * im, int w, int h, XImage * xim,
			      XImage * sxim, int *er1, int *er2, int *xarray,
			      unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;
   unsigned short     *img;
   int                 jmp;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	ter = er1;
	er1 = er2;
	er2 = ter;
	for (ex = 0; ex < (w + 2) * 3; ex++)
	   er2[ex] = 0;
	ex = 3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
	       {
		  XPutPixel(sxim, x, y, 0);
		  img++;
		  ex += 3;
	       }
	     else
	       {
		  XPutPixel(sxim, x, y, 1);
		  er = r + er1[ex++];
		  eg = g + er1[ex++];
		  eb = b + er1[ex++];
		  if (er > 255)
		     er = 255;
		  if (eg > 255)
		     eg = 255;
		  if (eb > 255)
		     eb = 255;
		  val = ((er & 0xf8) << 8) | ((eg & 0xfc) << 3) | ((eb & 0xf8) >> 3);
		  er = er & 0x07;
		  eg = eg & 0x03;
		  eb = eb & 0x07;
		  DITHER_ERROR(er1, er2, ex, er, eg, eb);
		  *img++ = val;
	       }
	  }
	img += jmp;
     }
}

void 
grender_shaped_16_fast_dither_ordered(GdkImlibImage * im, int w, int h, XImage * xim,
			       XImage * sxim, int *er1, int *er2, int *xarray,
				      unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;
   unsigned short     *img;
   int                 jmp;

   unsigned char       dither[4][4] =
   {
      {0, 4, 6, 5},
      {6, 2, 7, 3},
      {2, 6, 1, 5},
      {7, 4, 7, 3}
   };
   int                 dithy, dithx;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	dithy = y & 0x3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
	       {
		  XPutPixel(sxim, x, y, 0);
		  img++;
	       }
	     else
	       {
		  XPutPixel(sxim, x, y, 1);
		  er = r & 0x07;
		  eg = g & 0x03;
		  eb = b & 0x07;
		  dithx = x & 0x3;
		  if ((dither[dithy][dithx] < er) && (r < (256 - 8)))
		     r += 8;
		  if ((dither[dithy][dithx] < (eg << 1)) && (g < (256 - 4)))
		     g += 4;
		  if ((dither[dithy][dithx] < eb) && (b < (256 - 8)))
		     b += 8;
		  val = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
		  *img++ = val;
	       }
	  }
	img += jmp;
     }
}

void
grender_16_fast_dither(GdkImlibImage * im, int w, int h, XImage * xim,
		       XImage * sxim, int *er1, int *er2, int *xarray,
		       unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   unsigned short     *img;
   int                 jmp;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	ter = er1;
	er1 = er2;
	er2 = ter;
	for (ex = 0; ex < (w + 2) * 3; ex++)
	   er2[ex] = 0;
	ex = 3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     er = r + er1[ex++];
	     eg = g + er1[ex++];
	     eb = b + er1[ex++];
	     if (er > 255)
		er = 255;
	     if (eg > 255)
		eg = 255;
	     if (eb > 255)
		eb = 255;
	     val = ((er & 0xf8) << 8) | ((eg & 0xfc) << 3) | ((eb & 0xf8) >> 3);
	     er = er & 0x07;
	     eg = eg & 0x03;
	     eb = eb & 0x07;
	     DITHER_ERROR(er1, er2, ex, er, eg, eb);
	     *img++ = val;
	  }
	img += jmp;
     }
}

void 
grender_16_fast_dither_ordered(GdkImlibImage * im, int w, int h, XImage * xim,
			       XImage * sxim, int *er1, int *er2, int *xarray,
			       unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   unsigned short     *img;
   int                 jmp;

   unsigned char       dither[4][4] =
   {
      {0, 4, 6, 5},
      {6, 2, 7, 3},
      {2, 6, 1, 5},
      {7, 4, 7, 3}
   };
   int                 dithy, dithx;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	dithy = y & 0x3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     er = r & 0x07;
	     eg = g & 0x03;
	     eb = b & 0x07;
	     dithx = x & 0x3;
	     if ((dither[dithy][dithx] < er) && (r < (256 - 8)))
		r += 8;
	     if ((dither[dithy][dithx] < (eg << 1)) && (g < (256 - 4)))
		g += 4;
	     if ((dither[dithy][dithx] < eb) && (b < (256 - 8)))
		b += 8;
	     val = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
	     *img++ = val;
	  }
	img += jmp;
     }
}

void
grender_shaped_15_dither(GdkImlibImage * im, int w, int h, XImage * xim,
			 XImage * sxim, int *er1, int *er2, int *xarray,
			 unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   for (y = 0; y < h; y++)
     {
	ter = er1;
	er1 = er2;
	er2 = ter;
	for (ex = 0; ex < (w + 2) * 3; ex++)
	   er2[ex] = 0;
	ex = 3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
	       {
		  XPutPixel(sxim, x, y, 0);
		  ex += 3;
	       }
	     else
	       {
		  XPutPixel(sxim, x, y, 1);
		  er = r + er1[ex++];
		  eg = g + er1[ex++];
		  eb = b + er1[ex++];
		  if (er > 255)
		     er = 255;
		  if (eg > 255)
		     eg = 255;
		  if (eb > 255)
		     eb = 255;
		  val = ((er & 0xf8) << 7) | ((eg & 0xf8) << 2) | ((eb & 0xf8) >> 3);
		  er = er & 0x07;
		  eg = eg & 0x07;
		  eb = eb & 0x07;
		  if (er > 255)
		     er = 255;
		  else if (er < 0)
		     er = 0;
		  if (eg > 255)
		     eg = 255;
		  else if (eg < 0)
		     eg = 0;
		  if (eb > 255)
		     eb = 255;
		  else if (eb < 0)
		     eb = 0;
		  val = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
		  er = r & 0x07;
		  eg = g & 0x07;
		  eb = b & 0x07;
		  DITHER_ERROR(er1, er2, ex, er, eg, eb);
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
}

void 
grender_shaped_15_dither_ordered(GdkImlibImage * im, int w, int h, XImage * xim,
			       XImage * sxim, int *er1, int *er2, int *xarray,
				 unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   unsigned char       dither[4][4] =
   {
      {0, 4, 6, 5},
      {6, 2, 7, 3},
      {2, 6, 1, 5},
      {7, 4, 7, 3}
   };
   int                 dithy, dithx;

   for (y = 0; y < h; y++)
     {
	dithy = y & 0x3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
	       {
		  XPutPixel(sxim, x, y, 0);
	       }
	     else
	       {
		  XPutPixel(sxim, x, y, 1);
		  er = r & 0x07;
		  eg = g & 0x07;
		  eb = b & 0x07;
		  dithx = x & 0x3;
		  if ((dither[dithy][dithx] < er) && (r < (256 - 8)))
		     r += 8;
		  if ((dither[dithy][dithx] < eg) && (g < (256 - 8)))
		     g += 8;
		  if ((dither[dithy][dithx] < eb) && (b < (256 - 8)))
		     b += 8;
		  val = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
}

void
grender_15_dither(GdkImlibImage * im, int w, int h, XImage * xim,
		  XImage * sxim, int *er1, int *er2, int *xarray,
		  unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   for (y = 0; y < h; y++)
     {
	ter = er1;
	er1 = er2;
	er2 = ter;
	for (ex = 0; ex < (w + 2) * 3; ex++)
	   er2[ex] = 0;
	ex = 3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     er = r + er1[ex++];
	     eg = g + er1[ex++];
	     eb = b + er1[ex++];
	     if (er > 255)
		er = 255;
	     if (eg > 255)
		eg = 255;
	     if (eb > 255)
		eb = 255;
	     val = ((er & 0xf8) << 7) | ((eg & 0xf8) << 2) | ((eb & 0xf8) >> 3);
	     er = er & 0x07;
	     eg = eg & 0x07;
	     eb = eb & 0x07;
	     DITHER_ERROR(er1, er2, ex, er, eg, eb);
	     XPutPixel(xim, x, y, val);
	  }
     }
}

void
grender_15_dither_ordered(GdkImlibImage * im, int w, int h, XImage * xim,
			  XImage * sxim, int *er1, int *er2, int *xarray,
			  unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   unsigned char       dither[4][4] =
   {
      {0, 4, 6, 5},
      {6, 2, 7, 3},
      {2, 6, 1, 5},
      {7, 4, 7, 3}
   };
   int                 dithy, dithx;

   for (y = 0; y < h; y++)
     {
	dithy = y & 0x3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     er = r & 0x07;
	     eg = g & 0x07;
	     eb = b & 0x07;
	     dithx = x & 0x3;
	     if ((dither[dithy][dithx] < er) && (r < (256 - 8)))
		r += 8;
	     if ((dither[dithy][dithx] < eg) && (g < (256 - 8)))
		g += 8;
	     if ((dither[dithy][dithx] < eb) && (b < (256 - 8)))
		b += 8;
	     val = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
	     XPutPixel(xim, x, y, val);
	  }
     }
}

void
grender_shaped_16_dither(GdkImlibImage * im, int w, int h, XImage * xim,
			 XImage * sxim, int *er1, int *er2, int *xarray,
			 unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   for (y = 0; y < h; y++)
     {
	ter = er1;
	er1 = er2;
	er2 = ter;
	for (ex = 0; ex < (w + 2) * 3; ex++)
	   er2[ex] = 0;
	ex = 3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
	       {
		  XPutPixel(sxim, x, y, 0);
		  ex += 3;
	       }
	     else
	       {
		  XPutPixel(sxim, x, y, 1);
		  er = r + er1[ex++];
		  eg = g + er1[ex++];
		  eb = b + er1[ex++];
		  if (er > 255)
		     er = 255;
		  if (eg > 255)
		     eg = 255;
		  if (eb > 255)
		     eb = 255;
		  val = ((er & 0xf8) << 8) | ((eg & 0xfc) << 3) | ((eb & 0xf8) >> 3);
		  er = er & 0x07;
		  eg = eg & 0x03;
		  eb = eb & 0x07;
		  DITHER_ERROR(er1, er2, ex, er, eg, eb);
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
}

void
grender_shaped_16_dither_ordered(GdkImlibImage * im, int w, int h, XImage * xim,
			       XImage * sxim, int *er1, int *er2, int *xarray,
				 unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   unsigned char       dither[4][4] =
   {
      {0, 4, 6, 5},
      {6, 2, 7, 3},
      {2, 6, 1, 5},
      {7, 4, 7, 3}
   };
   int                 dithy, dithx;

   for (y = 0; y < h; y++)
     {
	dithy = y & 0x3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
	       {
		  XPutPixel(sxim, x, y, 0);
	       }
	     else
	       {
		  XPutPixel(sxim, x, y, 1);
		  er = r & 0x07;
		  eg = g & 0x03;
		  eb = b & 0x07;
		  dithx = x & 0x3;
		  if ((dither[dithy][dithx] < er) && (r < (256 - 8)))
		     r += 8;
		  if ((dither[dithy][dithx] < (eg << 1)) && (g < (256 - 4)))
		     g += 4;
		  if ((dither[dithy][dithx] < eb) && (b < (256 - 8)))
		     b += 8;
		  val = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
}

void
grender_16_dither(GdkImlibImage * im, int w, int h, XImage * xim,
		  XImage * sxim, int *er1, int *er2, int *xarray,
		  unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   for (y = 0; y < h; y++)
     {
	ter = er1;
	er1 = er2;
	er2 = ter;
	for (ex = 0; ex < (w + 2) * 3; ex++)
	   er2[ex] = 0;
	ex = 3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     er = r + er1[ex++];
	     eg = g + er1[ex++];
	     eb = b + er1[ex++];
	     if (er > 255)
		er = 255;
	     if (eg > 255)
		eg = 255;
	     if (eb > 255)
		eb = 255;
	     val = ((er & 0xf8) << 8) | ((eg & 0xfc) << 3) | ((eb & 0xf8) >> 3);
	     er = er & 0x07;
	     eg = eg & 0x03;
	     eb = eb & 0x07;
	     DITHER_ERROR(er1, er2, ex, er, eg, eb);
	     XPutPixel(xim, x, y, val);
	  }
     }
}

void
grender_16_dither_ordered(GdkImlibImage * im, int w, int h, XImage * xim,
			  XImage * sxim, int *er1, int *er2, int *xarray,
			  unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   unsigned char       dither[4][4] =
   {
      {0, 4, 6, 5},
      {6, 2, 7, 3},
      {2, 6, 1, 5},
      {7, 4, 7, 3}
   };
   int                 dithy, dithx;

   for (y = 0; y < h; y++)
     {
	dithy = y & 0x3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     er = r & 0x07;
	     eg = g & 0x03;
	     eb = b & 0x07;
	     dithx = x & 0x3;
	     if ((dither[dithy][dithx] < er) && (r < (256 - 8)))
		r += 8;
	     if ((dither[dithy][dithx] < (eg << 1)) && (g < (256 - 4)))
		g += 4;
	     if ((dither[dithy][dithx] < eb) && (b < (256 - 8)))
		b += 8;
	     val = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
	     XPutPixel(xim, x, y, val);
	  }
     }
}

void
grender_shaped_15_fast(GdkImlibImage * im, int w, int h, XImage * xim,
		       XImage * sxim, int *er1, int *er2, int *xarray,
		       unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;
   unsigned short     *img;
   int                 jmp;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
	       {
		  XPutPixel(sxim, x, y, 0);
		  img++;
	       }
	     else
	       {
		  XPutPixel(sxim, x, y, 1);
		  val = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
		  *img++ = val;
	       }
	  }
	img += jmp;
     }
}

void
grender_15_fast(GdkImlibImage * im, int w, int h, XImage * xim,
		XImage * sxim, int *er1, int *er2, int *xarray,
		unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;
   unsigned short     *img;
   int                 jmp;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     val = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
	     *img++ = val;
	  }
	img += jmp;
     }
}

void
grender_shaped_16_fast(GdkImlibImage * im, int w, int h, XImage * xim,
		       XImage * sxim, int *er1, int *er2, int *xarray,
		       unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;
   unsigned short     *img;
   int                 jmp;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
	       {
		  XPutPixel(sxim, x, y, 0);
		  img++;
	       }
	     else
	       {
		  XPutPixel(sxim, x, y, 1);
		  val = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
		  *img++ = val;
	       }
	  }
	img += jmp;
     }
}

void
grender_16_fast(GdkImlibImage * im, int w, int h, XImage * xim,
		XImage * sxim, int *er1, int *er2, int *xarray,
		unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;

   unsigned short     *img;
   int                 jmp;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     val = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
	     *img++ = val;
	  }
	img += jmp;
     }
}

void
grender_shaped_24_fast(GdkImlibImage * im, int w, int h, XImage * xim,
		       XImage * sxim, int *er1, int *er2, int *xarray,
		       unsigned char **yarray)
{
   int                 x, y, r, g, b;
   unsigned char      *ptr2;
   unsigned char      *img;
   int                 jmp;

   jmp = (xim->bytes_per_line) - w;
   img = (unsigned char *)xim->data;
   if (id->byte_order == BYTE_ORD_24_RGB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img += 3;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       *img++ = r;
		       *img++ = g;
		       *img++ = b;
		    }
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_RBG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img += 3;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       *img++ = r;
		       *img++ = b;
		       *img++ = g;
		    }
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BRG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img += 3;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       *img++ = b;
		       *img++ = r;
		       *img++ = g;
		    }
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BGR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img += 3;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       *img++ = b;
		       *img++ = g;
		       *img++ = r;
		    }
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GRB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img += 3;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       *img++ = g;
		       *img++ = r;
		       *img++ = b;
		    }
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GBR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img += 3;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       *img++ = g;
		       *img++ = b;
		       *img++ = r;
		    }
	       }
	     img += jmp;
	  }
     }
}

void
grender_24_fast(GdkImlibImage * im, int w, int h, XImage * xim,
		XImage * sxim, int *er1, int *er2, int *xarray,
		unsigned char **yarray)
{
   int                 x, y, r, g, b;
   unsigned char      *ptr2;
   unsigned char      *img;
   int                 jmp;

   jmp = (xim->bytes_per_line) - w;
   img = (unsigned char *)xim->data;
   if (id->byte_order == BYTE_ORD_24_RGB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  *img++ = r;
		  *img++ = g;
		  *img++ = b;
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_RBG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  *img++ = r;
		  *img++ = b;
		  *img++ = g;
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BRG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  *img++ = b;
		  *img++ = r;
		  *img++ = g;
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BGR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  *img++ = b;
		  *img++ = g;
		  *img++ = r;
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GRB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  *img++ = g;
		  *img++ = r;
		  *img++ = b;
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GBR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  *img++ = g;
		  *img++ = b;
		  *img++ = r;
	       }
	     img += jmp;
	  }
     }
}

void
grender_shaped_32_fast(GdkImlibImage * im, int w, int h, XImage * xim,
		       XImage * sxim, int *er1, int *er2, int *xarray,
		       unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;
   unsigned int       *img;
   int                 jmp;

   jmp = (xim->bytes_per_line >> 2) - w;
   img = (unsigned int *)xim->data;
   if (id->byte_order == BYTE_ORD_24_RGB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img++;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       val = (r << 16) | (g << 8) | b;
		       *img++ = val;
		    }
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_RBG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img++;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       val = (r << 16) | (b << 8) | g;
		       *img++ = val;
		    }
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BRG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img++;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       val = (b << 16) | (r << 8) | g;
		       *img++ = val;
		    }
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BGR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img++;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       val = (b << 16) | (g << 8) | r;
		       *img++ = val;
		    }
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GRB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img++;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       val = (g << 16) | (r << 8) | b;
		       *img++ = val;
		    }
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GBR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img++;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       val = (g << 16) | (b << 8) | r;
		       *img++ = val;
		    }
	       }
	     img += jmp;
	  }
     }
}

void
grender_32_fast(GdkImlibImage * im, int w, int h, XImage * xim,
		XImage * sxim, int *er1, int *er2, int *xarray,
		unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;
   unsigned int       *img;
   int                 jmp;

   jmp = (xim->bytes_per_line >> 2) - w;
   img = (unsigned int *)xim->data;
   if (id->byte_order == BYTE_ORD_24_RGB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  val = (r << 16) | (g << 8) | b;
		  *img++ = val;
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_RBG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  val = (r << 16) | (b << 8) | g;
		  *img++ = val;
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BRG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  val = (b << 16) | (r << 8) | g;
		  *img++ = val;
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BGR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  val = (b << 16) | (g << 8) | r;
		  *img++ = val;
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GRB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  val = (g << 16) | (r << 8) | b;
		  *img++ = val;
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GBR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  val = (g << 16) | (b << 8) | r;
		  *img++ = val;
	       }
	     img += jmp;
	  }
     }
}

void
grender_shaped_15(GdkImlibImage * im, int w, int h, XImage * xim,
		  XImage * sxim, int *er1, int *er2, int *xarray,
		  unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;

   for (y = 0; y < h; y++)
     {
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
		XPutPixel(sxim, x, y, 0);
	     else
	       {
		  XPutPixel(sxim, x, y, 1);
		  val = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
}

void
grender_15(GdkImlibImage * im, int w, int h, XImage * xim,
	   XImage * sxim, int *er1, int *er2, int *xarray,
	   unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;

   for (y = 0; y < h; y++)
     {
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     val = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
	     XPutPixel(xim, x, y, val);
	  }
     }
}

void
grender_shaped_16(GdkImlibImage * im, int w, int h, XImage * xim,
		  XImage * sxim, int *er1, int *er2, int *xarray,
		  unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;

   for (y = 0; y < h; y++)
     {
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
		XPutPixel(sxim, x, y, 0);
	     else
	       {
		  XPutPixel(sxim, x, y, 1);
		  val = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
}

void
grender_16(GdkImlibImage * im, int w, int h, XImage * xim,
	   XImage * sxim, int *er1, int *er2, int *xarray,
	   unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;

   for (y = 0; y < h; y++)
     {
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     val = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
	     XPutPixel(xim, x, y, val);
	  }
     }
}

void
grender_shaped_24(GdkImlibImage * im, int w, int h, XImage * xim,
		  XImage * sxim, int *er1, int *er2, int *xarray,
		  unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;

   if (id->byte_order == BYTE_ORD_24_RGB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		     XPutPixel(sxim, x, y, 0);
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       val = (r << 16) | (g << 8) | b;
		       XPutPixel(xim, x, y, val);
		    }
	       }
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_RBG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		     XPutPixel(sxim, x, y, 0);
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       val = (r << 16) | (b << 8) | g;
		       XPutPixel(xim, x, y, val);
		    }
	       }
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BRG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		     XPutPixel(sxim, x, y, 0);
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       val = (b << 16) | (r << 8) | g;
		       XPutPixel(xim, x, y, val);
		    }
	       }
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BGR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		     XPutPixel(sxim, x, y, 0);
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       val = (b << 16) | (g << 8) | r;
		       XPutPixel(xim, x, y, val);
		    }
	       }
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GRB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		     XPutPixel(sxim, x, y, 0);
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       val = (g << 16) | (r << 8) | b;
		       XPutPixel(xim, x, y, val);
		    }
	       }
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GBR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		     XPutPixel(sxim, x, y, 0);
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       val = (g << 16) | (b << 8) | r;
		       XPutPixel(xim, x, y, val);
		    }
	       }
	  }
     }
}

void
grender_24(GdkImlibImage * im, int w, int h, XImage * xim,
	   XImage * sxim, int *er1, int *er2, int *xarray,
	   unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;

   if (id->byte_order == BYTE_ORD_24_RGB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  val = (r << 16) | (g << 8) | b;
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_RBG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  val = (r << 16) | (b << 8) | g;
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BRG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  val = (b << 16) | (r << 8) | g;
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BGR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  val = (b << 16) | (g << 8) | r;
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GRB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  val = (g << 16) | (r << 8) | b;
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GBR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  val = (g << 16) | (b << 8) | r;
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
}

void
grender_shaped(GdkImlibImage * im, int w, int h, XImage * xim,
	       XImage * sxim, int *er1, int *er2, int *xarray,
	       unsigned char **yarray, int bpp)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;
   unsigned char      *img;
   int                 jmp;

   jmp = (xim->bytes_per_line) - w;
   img = (unsigned char *)xim->data;
   switch (id->render_type)
     {
     case RT_PLAIN_PALETTE:
	if ((id->fastrend) && (xim->bits_per_pixel == 8))
	  {
	     for (y = 0; y < h; y++)
	       {
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       if ((r == im->shape_color.r) &&
			   (g == im->shape_color.g) &&
			   (b == im->shape_color.b))
			 {
			    XPutPixel(sxim, x, y, 0);
			    img++;
			 }
		       else
			 {
			    XPutPixel(sxim, x, y, 1);
			    val = gdk_imlib_best_color_match(&r, &g, &b);
			    *img++ = val;
			 }
		    }
		  img += jmp;
	       }
	  }
	else
	  {
	     for (y = 0; y < h; y++)
	       {
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       if ((r == im->shape_color.r) &&
			   (g == im->shape_color.g) &&
			   (b == im->shape_color.b))
			  XPutPixel(sxim, x, y, 0);
		       else
			 {
			    XPutPixel(sxim, x, y, 1);
			    val = gdk_imlib_best_color_match(&r, &g, &b);
			    XPutPixel(xim, x, y, val);
			 }
		    }
	       }
	  }
	break;
     case RT_PLAIN_PALETTE_FAST:
	if ((id->fastrend) && (xim->bits_per_pixel == 8))
	  {
	     for (y = 0; y < h; y++)
	       {
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       if ((r == im->shape_color.r) &&
			   (g == im->shape_color.g) &&
			   (b == im->shape_color.b))
			 {
			    XPutPixel(sxim, x, y, 0);
			    img++;
			 }
		       else
			 {
			    XPutPixel(sxim, x, y, 1);
			    val = COLOR_RGB(r >> 3, g >> 3, b >> 3);
			    *img++ = val;
			 }
		    }
		  img += jmp;
	       }
	  }
	else
	  {
	     for (y = 0; y < h; y++)
	       {
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       if ((r == im->shape_color.r) &&
			   (g == im->shape_color.g) &&
			   (b == im->shape_color.b))
			  XPutPixel(sxim, x, y, 0);
		       else
			 {
			    XPutPixel(sxim, x, y, 1);
			    val = COLOR_RGB(r >> 3, g >> 3, b >> 3);
			    XPutPixel(xim, x, y, val);
			 }
		    }
	       }
	  }
	break;
     case RT_DITHER_PALETTE:
	if ((id->fastrend) && (xim->bits_per_pixel == 8))
	  {
	     for (y = 0; y < h; y++)
	       {
		  ter = er1;
		  er1 = er2;
		  er2 = ter;
		  for (ex = 0; ex < (w + 2) * 3; ex++)
		     er2[ex] = 0;
		  ex = 3;
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       if ((r == im->shape_color.r) &&
			   (g == im->shape_color.g) &&
			   (b == im->shape_color.b))
			 {
			    {
			       XPutPixel(sxim, x, y, 0);
			       img++;
			    }
			    ex += 3;
			 }
		       else
			 {
			    XPutPixel(sxim, x, y, 1);
			    er = r + er1[ex++];
			    eg = g + er1[ex++];
			    eb = b + er1[ex++];
			    if (er > 255)
			       er = 255;
			    else if (er < 0)
			       er = 0;
			    if (eg > 255)
			       eg = 255;
			    else if (eg < 0)
			       eg = 0;
			    if (eb > 255)
			       eb = 255;
			    else if (eb < 0)
			       eb = 0;
			    val = gdk_imlib_best_color_match(&er, &eg, &eb);
			    DITHER_ERROR(er1, er2, ex, er, eg, eb);
			    *img++ = val;
			 }
		    }
		  img += jmp;
	       }
	  }
	else
	  {
	     for (y = 0; y < h; y++)
	       {
		  ter = er1;
		  er1 = er2;
		  er2 = ter;
		  for (ex = 0; ex < (w + 2) * 3; ex++)
		     er2[ex] = 0;
		  ex = 3;
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       if ((r == im->shape_color.r) &&
			   (g == im->shape_color.g) &&
			   (b == im->shape_color.b))
			 {
			    XPutPixel(sxim, x, y, 0);
			    ex += 3;
			 }
		       else
			 {
			    XPutPixel(sxim, x, y, 1);
			    er = r + er1[ex++];
			    eg = g + er1[ex++];
			    eb = b + er1[ex++];
			    if (er > 255)
			       er = 255;
			    else if (er < 0)
			       er = 0;
			    if (eg > 255)
			       eg = 255;
			    else if (eg < 0)
			       eg = 0;
			    if (eb > 255)
			       eb = 255;
			    else if (eb < 0)
			       eb = 0;
			    val = gdk_imlib_best_color_match(&er, &eg, &eb);
			    DITHER_ERROR(er1, er2, ex, er, eg, eb);
			    XPutPixel(xim, x, y, val);
			 }
		    }
	       }
	  }
	break;
     case RT_DITHER_PALETTE_FAST:
	if ((id->fastrend) && (xim->bits_per_pixel == 8))
	  {
	     for (y = 0; y < h; y++)
	       {
		  ter = er1;
		  er1 = er2;
		  er2 = ter;
		  for (ex = 0; ex < (w + 2) * 3; ex++)
		     er2[ex] = 0;
		  ex = 3;
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       if ((r == im->shape_color.r) &&
			   (g == im->shape_color.g) &&
			   (b == im->shape_color.b))
			 {
			    {
			       XPutPixel(sxim, x, y, 0);
			       img++;
			    }
			    ex += 3;
			 }
		       else
			 {
			    XPutPixel(sxim, x, y, 1);
			    er = r + er1[ex++];
			    eg = g + er1[ex++];
			    eb = b + er1[ex++];
			    if (er > 255)
			       er = 255;
			    else if (er < 0)
			       er = 0;
			    if (eg > 255)
			       eg = 255;
			    else if (eg < 0)
			       eg = 0;
			    if (eb > 255)
			       eb = 255;
			    else if (eb < 0)
			       eb = 0;
			    val = INDEX_RGB(er >> 3, eg >> 3, eb >> 3);
			    er = ERROR_RED(er, val);
			    eg = ERROR_GRN(eg, val);
			    eb = ERROR_BLU(eb, val);
			    DITHER_ERROR(er1, er2, ex, er, eg, eb);
			    *img++ = COLOR_INDEX(val);
			 }
		    }
		  img += jmp;
	       }
	  }
	else
	  {
	     for (y = 0; y < h; y++)
	       {
		  ter = er1;
		  er1 = er2;
		  er2 = ter;
		  for (ex = 0; ex < (w + 2) * 3; ex++)
		     er2[ex] = 0;
		  ex = 3;
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       if ((r == im->shape_color.r) &&
			   (g == im->shape_color.g) &&
			   (b == im->shape_color.b))
			 {
			    XPutPixel(sxim, x, y, 0);
			    ex += 3;
			 }
		       else
			 {
			    XPutPixel(sxim, x, y, 1);
			    er = r + er1[ex++];
			    eg = g + er1[ex++];
			    eb = b + er1[ex++];
			    if (er > 255)
			       er = 255;
			    else if (er < 0)
			       er = 0;
			    if (eg > 255)
			       eg = 255;
			    else if (eg < 0)
			       eg = 0;
			    if (eb > 255)
			       eb = 255;
			    else if (eb < 0)
			       eb = 0;
			    val = INDEX_RGB(er >> 3, eg >> 3, eb >> 3);
			    er = ERROR_RED(er, val);
			    eg = ERROR_GRN(eg, val);
			    eb = ERROR_BLU(eb, val);
			    DITHER_ERROR(er1, er2, ex, er, eg, eb);
			    XPutPixel(xim, x, y, COLOR_INDEX(val));
			 }
		    }
	       }
	  }
	break;
     default:
	if (id->fastrend)
	  {
	     switch (bpp)
	       {
	       case 8:
		  break;
	       case 15:
		  if (id->render_type == RT_DITHER_TRUECOL)
		    {
		       if (id->ordered_dither)
			  grender_shaped_15_fast_dither_ordered(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		       else
			  grender_shaped_15_fast_dither(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		    }
		  else
		     grender_shaped_15_fast(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       case 16:
		  if (id->render_type == RT_DITHER_TRUECOL)
		    {
		       if (id->ordered_dither)
			  grender_shaped_16_fast_dither_ordered(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		       else
			  grender_shaped_16_fast_dither(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		    }
		  else
		     grender_shaped_16_fast(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       case 24:
	       case 32:
		  if (xim->bits_per_pixel == 24)
		     grender_shaped_24_fast(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  else
		     grender_shaped_32_fast(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       default:
		  break;
	       }
	  }
	else
	  {
	     switch (bpp)
	       {
	       case 8:
		  break;
	       case 15:
		  if (id->render_type == RT_DITHER_TRUECOL)
		    {
		       if (id->ordered_dither)
			  grender_shaped_15_dither_ordered(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		       else
			  grender_shaped_15_dither(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		    }
		  else
		     grender_shaped_15(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       case 16:
		  if (id->render_type == RT_DITHER_TRUECOL)
		    {
		       if (id->ordered_dither)
			  grender_shaped_16_dither_ordered(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		       else
			  grender_shaped_16_dither(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		    }
		  else
		     grender_shaped_16(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       case 24:
		  grender_shaped_24(im, w, h, xim, sxim, er1, er2, xarray, yarray);
	       case 32:
		  grender_shaped_24(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       default:
		  break;
	       }
	  }
	break;
     }
}

void
grender(GdkImlibImage * im, int w, int h, XImage * xim,
	XImage * sxim, int *er1, int *er2, int *xarray,
	unsigned char **yarray, int bpp)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;
   unsigned char      *img;
   int                 jmp;

   jmp = (xim->bytes_per_line) - w;
   img = (unsigned char *)xim->data;
   switch (id->render_type)
     {
     case RT_PLAIN_PALETTE:
	if ((id->fastrend) && (xim->bits_per_pixel == 8))
	  {
	     for (y = 0; y < h; y++)
	       {
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       val = gdk_imlib_best_color_match(&r, &g, &b);
		       *img++ = val;
		    }
		  img += jmp;
	       }
	  }
	else
	  {
	     for (y = 0; y < h; y++)
	       {
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       val = gdk_imlib_best_color_match(&r, &g, &b);
		       XPutPixel(xim, x, y, val);
		    }
	       }
	  }
	break;
     case RT_PLAIN_PALETTE_FAST:
	if ((id->fastrend) && (xim->bits_per_pixel == 8))
	  {
	     for (y = 0; y < h; y++)
	       {
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       val = COLOR_RGB(r >> 3, g >> 3, b >> 3);
		       *img++ = val;
		    }
		  img += jmp;
	       }
	  }
	else
	  {
	     for (y = 0; y < h; y++)
	       {
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       val = COLOR_RGB(r >> 3, g >> 3, b >> 3);
		       XPutPixel(xim, x, y, val);
		    }
	       }
	  }
	break;
     case RT_DITHER_PALETTE:
	if ((id->fastrend) && (xim->bits_per_pixel == 8))
	  {
	     for (y = 0; y < h; y++)
	       {
		  ter = er1;
		  er1 = er2;
		  er2 = ter;
		  for (ex = 0; ex < (w + 2) * 3; ex++)
		     er2[ex] = 0;
		  ex = 3;
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       er = r + er1[ex++];
		       eg = g + er1[ex++];
		       eb = b + er1[ex++];
		       if (er > 255)
			  er = 255;
		       else if (er < 0)
			  er = 0;
		       if (eg > 255)
			  eg = 255;
		       else if (eg < 0)
			  eg = 0;
		       if (eb > 255)
			  eb = 255;
		       else if (eb < 0)
			  eb = 0;
		       val = gdk_imlib_best_color_match(&er, &eg, &eb);
		       DITHER_ERROR(er1, er2, ex, er, eg, eb);
		       *img++ = val;
		    }
		  img += jmp;
	       }
	  }
	else
	  {
	     for (y = 0; y < h; y++)
	       {
		  ter = er1;
		  er1 = er2;
		  er2 = ter;
		  for (ex = 0; ex < (w + 2) * 3; ex++)
		     er2[ex] = 0;
		  ex = 3;
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       er = r + er1[ex++];
		       eg = g + er1[ex++];
		       eb = b + er1[ex++];
		       if (er > 255)
			  er = 255;
		       else if (er < 0)
			  er = 0;
		       if (eg > 255)
			  eg = 255;
		       else if (eg < 0)
			  eg = 0;
		       if (eb > 255)
			  eb = 255;
		       else if (eb < 0)
			  eb = 0;
		       val = gdk_imlib_best_color_match(&er, &eg, &eb);
		       DITHER_ERROR(er1, er2, ex, er, eg, eb);
		       XPutPixel(xim, x, y, val);
		    }
	       }
	  }
	break;
     case RT_DITHER_PALETTE_FAST:
	if ((id->fastrend) && (xim->bits_per_pixel == 8))
	  {
	     for (y = 0; y < h; y++)
	       {
		  ter = er1;
		  er1 = er2;
		  er2 = ter;
		  for (ex = 0; ex < (w + 2) * 3; ex++)
		     er2[ex] = 0;
		  ex = 3;
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       er = r + er1[ex++];
		       eg = g + er1[ex++];
		       eb = b + er1[ex++];
		       if (er > 255)
			  er = 255;
		       else if (er < 0)
			  er = 0;
		       if (eg > 255)
			  eg = 255;
		       else if (eg < 0)
			  eg = 0;
		       if (eb > 255)
			  eb = 255;
		       else if (eb < 0)
			  eb = 0;
		       val = INDEX_RGB(er >> 3, eg >> 3, eb >> 3);
		       er = ERROR_RED(er, val);
		       eg = ERROR_GRN(eg, val);
		       eb = ERROR_BLU(eb, val);
		       DITHER_ERROR(er1, er2, ex, er, eg, eb);
		       *img++ = COLOR_INDEX(val);
		    }
		  img += jmp;
	       }
	  }
	else
	  {
	     for (y = 0; y < h; y++)
	       {
		  ter = er1;
		  er1 = er2;
		  er2 = ter;
		  for (ex = 0; ex < (w + 2) * 3; ex++)
		     er2[ex] = 0;
		  ex = 3;
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       er = r + er1[ex++];
		       eg = g + er1[ex++];
		       eb = b + er1[ex++];
		       if (er > 255)
			  er = 255;
		       else if (er < 0)
			  er = 0;
		       if (eg > 255)
			  eg = 255;
		       else if (eg < 0)
			  eg = 0;
		       if (eb > 255)
			  eb = 255;
		       else if (eb < 0)
			  eb = 0;
		       val = INDEX_RGB(er >> 3, eg >> 3, eb >> 3);
		       er = ERROR_RED(er, val);
		       eg = ERROR_GRN(eg, val);
		       eb = ERROR_BLU(eb, val);
		       DITHER_ERROR(er1, er2, ex, er, eg, eb);
		       XPutPixel(xim, x, y, COLOR_INDEX(val));
		    }
	       }
	  }
	break;
     default:
	if (id->fastrend)
	  {
	     switch (bpp)
	       {
	       case 8:
		  break;
	       case 15:
		  if (id->render_type == RT_DITHER_TRUECOL)
		    {
		       if (id->ordered_dither)
			  grender_15_fast_dither_ordered(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		       else
			  grender_15_fast_dither(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		    }
		  else
		     grender_15_fast(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       case 16:
		  if (id->render_type == RT_DITHER_TRUECOL)
		    {
		       if (id->ordered_dither)
			  grender_16_fast_dither_ordered(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		       else
			  grender_16_fast_dither(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		    }
		  else
		     grender_16_fast(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       case 24:
	       case 32:
		  if (xim->bits_per_pixel == 24)
		     grender_24_fast(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  else
		     grender_32_fast(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       default:
		  break;
	       }
	  }
	else
	  {
	     switch (bpp)
	       {
	       case 8:
		  break;
	       case 15:
		  if (id->render_type == RT_DITHER_TRUECOL)
		    {
		       if (id->ordered_dither)
			  grender_15_dither_ordered(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		       else
			  grender_15_dither(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		    }
		  else
		     grender_15(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       case 16:
		  if (id->render_type == RT_DITHER_TRUECOL)
		    {
		       if (id->ordered_dither)
			  grender_16_dither_ordered(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		       else
			  grender_16_dither(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		    }
		  else
		     grender_16(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       case 24:
		  grender_24(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       case 32:
		  grender_24(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       default:
		  break;
	       }
	     break;
	  }
     }
}

void
grender_shaped_15_fast_dither_mod(GdkImlibImage * im, int w, int h, XImage * xim,
			       XImage * sxim, int *er1, int *er2, int *xarray,
				  unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;
   unsigned short     *img;
   int                 jmp;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	ter = er1;
	er1 = er2;
	er2 = ter;
	for (ex = 0; ex < (w + 2) * 3; ex++)
	   er2[ex] = 0;
	ex = 3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
	       {
		  XPutPixel(sxim, x, y, 0);
		  img++;
		  ex += 3;
	       }
	     else
	       {
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  XPutPixel(sxim, x, y, 1);
		  er = r + er1[ex++];
		  eg = g + er1[ex++];
		  eb = b + er1[ex++];
		  if (er > 255)
		     er = 255;
		  if (eg > 255)
		     eg = 255;
		  if (eb > 255)
		     eb = 255;
		  val = ((er & 0xf8) << 7) | ((eg & 0xf8) << 2) | ((eb & 0xf8) >> 3);
		  er = er & 0x07;
		  eg = eg & 0x07;
		  eb = eb & 0x07;
		  DITHER_ERROR(er1, er2, ex, er, eg, eb);
		  *img++ = val;
	       }
	  }
	img += jmp;
     }
}

void 
grender_shaped_15_fast_dither_mod_ordered(GdkImlibImage * im, int w, int h, XImage * xim,
			       XImage * sxim, int *er1, int *er2, int *xarray,
					  unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;
   unsigned short     *img;
   int                 jmp;

   unsigned char       dither[4][4] =
   {
      {0, 4, 6, 5},
      {6, 2, 7, 3},
      {2, 6, 1, 5},
      {7, 4, 7, 3}
   };
   int                 dithy, dithx;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	dithy = y & 0x3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
	       {
		  XPutPixel(sxim, x, y, 0);
		  img++;
	       }
	     else
	       {
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  XPutPixel(sxim, x, y, 1);
		  er = r & 0x07;
		  eg = g & 0x07;
		  eb = b & 0x07;
		  dithx = x & 0x3;
		  if ((dither[dithy][dithx] < er) && (r < (256 - 8)))
		     r += 8;
		  if ((dither[dithy][dithx] < eg) && (g < (256 - 8)))
		     g += 8;
		  if ((dither[dithy][dithx] < eb) && (b < (256 - 8)))
		     b += 8;
		  val = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
		  *img++ = val;
	       }
	  }
	img += jmp;
     }
}

void
grender_15_fast_dither_mod(GdkImlibImage * im, int w, int h, XImage * xim,
			   XImage * sxim, int *er1, int *er2, int *xarray,
			   unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;
   unsigned short     *img;
   int                 jmp;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	ter = er1;
	er1 = er2;
	er2 = ter;
	for (ex = 0; ex < (w + 2) * 3; ex++)
	   er2[ex] = 0;
	ex = 3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     r = im->rmap[r];
	     g = im->gmap[g];
	     b = im->bmap[b];
	     er = r + er1[ex++];
	     eg = g + er1[ex++];
	     eb = b + er1[ex++];
	     if (er > 255)
		er = 255;
	     if (eg > 255)
		eg = 255;
	     if (eb > 255)
		eb = 255;
	     val = ((er & 0xf8) << 7) | ((eg & 0xf8) << 2) | ((eb & 0xf8) >> 3);
	     er = er & 0x07;
	     eg = eg & 0x07;
	     eb = eb & 0x07;
	     DITHER_ERROR(er1, er2, ex, er, eg, eb);
	     *img++ = val;
	  }
	img += jmp;
     }
}

void 
grender_15_fast_dither_mod_ordered(GdkImlibImage * im, int w, int h, XImage * xim,
			       XImage * sxim, int *er1, int *er2, int *xarray,
				   unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   unsigned short     *img;
   int                 jmp;

   unsigned char       dither[4][4] =
   {
      {0, 4, 6, 5},
      {6, 2, 7, 3},
      {2, 6, 1, 5},
      {7, 4, 7, 3}
   };
   int                 dithy, dithx;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	dithy = y & 0x3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     r = im->rmap[r];
	     g = im->gmap[g];
	     b = im->bmap[b];
	     er = r & 0x07;
	     eg = g & 0x07;
	     eb = b & 0x07;
	     dithx = x & 0x3;
	     if ((dither[dithy][dithx] < er) && (r < (256 - 8)))
		r += 8;
	     if ((dither[dithy][dithx] < eg) && (g < (256 - 8)))
		g += 8;
	     if ((dither[dithy][dithx] < eb) && (b < (256 - 8)))
		b += 8;
	     val = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
	     *img++ = val;
	  }
	img += jmp;
     }
}

void
grender_shaped_16_fast_dither_mod(GdkImlibImage * im, int w, int h, XImage * xim,
			       XImage * sxim, int *er1, int *er2, int *xarray,
				  unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;
   unsigned short     *img;
   int                 jmp;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	ter = er1;
	er1 = er2;
	er2 = ter;
	for (ex = 0; ex < (w + 2) * 3; ex++)
	   er2[ex] = 0;
	ex = 3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
	       {
		  XPutPixel(sxim, x, y, 0);
		  img++;
		  ex += 3;
	       }
	     else
	       {
		  XPutPixel(sxim, x, y, 1);
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  er = r + er1[ex++];
		  eg = g + er1[ex++];
		  eb = b + er1[ex++];
		  if (er > 255)
		     er = 255;
		  if (eg > 255)
		     eg = 255;
		  if (eb > 255)
		     eb = 255;
		  val = ((er & 0xf8) << 8) | ((eg & 0xfc) << 3) | ((eb & 0xf8) >> 3);
		  er = er & 0x07;
		  eg = eg & 0x03;
		  eb = eb & 0x07;
		  DITHER_ERROR(er1, er2, ex, er, eg, eb);
		  *img++ = val;
	       }
	  }
	img += jmp;
     }
}

void 
grender_shaped_16_fast_dither_mod_ordered(GdkImlibImage * im, int w, int h, XImage * xim,
			       XImage * sxim, int *er1, int *er2, int *xarray,
					  unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;
   unsigned short     *img;
   int                 jmp;

   unsigned char       dither[4][4] =
   {
      {0, 4, 6, 5},
      {6, 2, 7, 3},
      {2, 6, 1, 5},
      {7, 4, 7, 3}
   };
   int                 dithy, dithx;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	dithy = y & 0x3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
	       {
		  XPutPixel(sxim, x, y, 0);
		  img++;
	       }
	     else
	       {
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  XPutPixel(sxim, x, y, 1);
		  er = r & 0x07;
		  eg = g & 0x03;
		  eb = b & 0x07;
		  dithx = x & 0x3;
		  if ((dither[dithy][dithx] < er) && (r < (256 - 8)))
		     r += 8;
		  if ((dither[dithy][dithx] < (eg << 1)) && (g < (256 - 4)))
		     g += 4;
		  if ((dither[dithy][dithx] < eb) && (b < (256 - 8)))
		     b += 8;
		  val = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
		  *img++ = val;
	       }
	  }
	img += jmp;
     }
}

void
grender_16_fast_dither_mod(GdkImlibImage * im, int w, int h, XImage * xim,
			   XImage * sxim, int *er1, int *er2, int *xarray,
			   unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   unsigned short     *img;
   int                 jmp;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	ter = er1;
	er1 = er2;
	er2 = ter;
	for (ex = 0; ex < (w + 2) * 3; ex++)
	   er2[ex] = 0;
	ex = 3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     r = im->rmap[r];
	     g = im->gmap[g];
	     b = im->bmap[b];
	     er = r + er1[ex++];
	     eg = g + er1[ex++];
	     eb = b + er1[ex++];
	     if (er > 255)
		er = 255;
	     if (eg > 255)
		eg = 255;
	     if (eb > 255)
		eb = 255;
	     val = ((er & 0xf8) << 8) | ((eg & 0xfc) << 3) | ((eb & 0xf8) >> 3);
	     er = er & 0x07;
	     eg = eg & 0x03;
	     eb = eb & 0x07;
	     DITHER_ERROR(er1, er2, ex, er, eg, eb);
	     *img++ = val;
	  }
	img += jmp;
     }
}

void 
grender_16_fast_dither_mod_ordered(GdkImlibImage * im, int w, int h, XImage * xim,
			       XImage * sxim, int *er1, int *er2, int *xarray,
				   unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   unsigned short     *img;
   int                 jmp;

   unsigned char       dither[4][4] =
   {
      {0, 4, 6, 5},
      {6, 2, 7, 3},
      {2, 6, 1, 5},
      {7, 4, 7, 3}
   };
   int                 dithy, dithx;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	dithy = y & 0x3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     r = im->rmap[r];
	     g = im->gmap[g];
	     b = im->bmap[b];
	     er = r & 0x07;
	     eg = g & 0x03;
	     eb = b & 0x07;
	     dithx = x & 0x3;
	     if ((dither[dithy][dithx] < er) && (r < (256 - 8)))
		r += 8;
	     if ((dither[dithy][dithx] < (eg << 1)) && (g < (256 - 4)))
		g += 4;
	     if ((dither[dithy][dithx] < eb) && (b < (256 - 8)))
		b += 8;
	     val = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
	     *img++ = val;
	  }
	img += jmp;
     }
}

void 
grender_shaped_15_dither_mod_ordered(GdkImlibImage * im, int w, int h, XImage * xim,
			       XImage * sxim, int *er1, int *er2, int *xarray,
				     unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   unsigned char       dither[4][4] =
   {
      {0, 4, 6, 5},
      {6, 2, 7, 3},
      {2, 6, 1, 5},
      {7, 4, 7, 3}
   };
   int                 dithy, dithx;

   for (y = 0; y < h; y++)
     {
	dithy = y & 0x3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
	       {
		  XPutPixel(sxim, x, y, 0);
	       }
	     else
	       {
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  XPutPixel(sxim, x, y, 1);
		  er = r & 0x07;
		  eg = g & 0x07;
		  eb = b & 0x07;
		  dithx = x & 0x3;
		  if ((dither[dithy][dithx] < er) && (r < (256 - 8)))
		     r += 8;
		  if ((dither[dithy][dithx] < eg) && (g < (256 - 8)))
		     g += 8;
		  if ((dither[dithy][dithx] < eb) && (b < (256 - 8)))
		     b += 8;
		  val = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
}

void 
grender_15_dither_mod_ordered(GdkImlibImage * im, int w, int h, XImage * xim,
			      XImage * sxim, int *er1, int *er2, int *xarray,
			      unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   unsigned char       dither[4][4] =
   {
      {0, 4, 6, 5},
      {6, 2, 7, 3},
      {2, 6, 1, 5},
      {7, 4, 7, 3}
   };
   int                 dithy, dithx;

   for (y = 0; y < h; y++)
     {
	dithy = y & 0x3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     r = im->rmap[r];
	     g = im->gmap[g];
	     b = im->bmap[b];
	     er = r & 0x07;
	     eg = g & 0x07;
	     eb = b & 0x07;
	     dithx = x & 0x3;
	     if ((dither[dithy][dithx] < er) && (r < (256 - 8)))
		r += 8;
	     if ((dither[dithy][dithx] < eg) && (g < (256 - 8)))
		g += 8;
	     if ((dither[dithy][dithx] < eb) && (b < (256 - 8)))
		b += 8;
	     val = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
	     XPutPixel(xim, x, y, val);
	  }
     }
}

void 
grender_shaped_16_dither_mod_ordered(GdkImlibImage * im, int w, int h, XImage * xim,
			       XImage * sxim, int *er1, int *er2, int *xarray,
				     unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   unsigned char       dither[4][4] =
   {
      {0, 4, 6, 5},
      {6, 2, 7, 3},
      {2, 6, 1, 5},
      {7, 4, 7, 3}
   };
   int                 dithy, dithx;

   for (y = 0; y < h; y++)
     {
	dithy = y & 0x3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
	       {
		  XPutPixel(sxim, x, y, 0);
	       }
	     else
	       {
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  XPutPixel(sxim, x, y, 1);
		  er = r & 0x07;
		  eg = g & 0x03;
		  eb = b & 0x07;
		  dithx = x & 0x3;
		  if ((dither[dithy][dithx] < er) && (r < (256 - 8)))
		     r += 8;
		  if ((dither[dithy][dithx] < (eg << 1)) && (g < (256 - 4)))
		     g += 4;
		  if ((dither[dithy][dithx] < eb) && (b < (256 - 8)))
		     b += 8;
		  val = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
}

void 
grender_16_dither_mod_ordered(GdkImlibImage * im, int w, int h, XImage * xim,
			      XImage * sxim, int *er1, int *er2, int *xarray,
			      unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   unsigned char       dither[4][4] =
   {
      {0, 4, 6, 5},
      {6, 2, 7, 3},
      {2, 6, 1, 5},
      {7, 4, 7, 3}
   };
   int                 dithy, dithx;

   for (y = 0; y < h; y++)
     {
	dithy = y & 0x3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     r = im->rmap[r];
	     g = im->gmap[g];
	     b = im->bmap[b];
	     er = r & 0x07;
	     eg = g & 0x03;
	     eb = b & 0x07;
	     dithx = x & 0x3;
	     if ((dither[dithy][dithx] < er) && (r < (256 - 8)))
		r += 8;
	     if ((dither[dithy][dithx] < (eg << 1)) && (g < (256 - 4)))
		g += 4;
	     if ((dither[dithy][dithx] < eb) && (b < (256 - 8)))
		b += 8;
	     val = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
	     XPutPixel(xim, x, y, val);
	  }
     }
}

void
grender_shaped_15_dither_mod(GdkImlibImage * im, int w, int h, XImage * xim,
			     XImage * sxim, int *er1, int *er2, int *xarray,
			     unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   for (y = 0; y < h; y++)
     {
	ter = er1;
	er1 = er2;
	er2 = ter;
	for (ex = 0; ex < (w + 2) * 3; ex++)
	   er2[ex] = 0;
	ex = 3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
	       {
		  XPutPixel(sxim, x, y, 0);
		  ex += 3;
	       }
	     else
	       {
		  XPutPixel(sxim, x, y, 1);
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  er = r + er1[ex++];
		  eg = g + er1[ex++];
		  eb = b + er1[ex++];
		  if (er > 255)
		     er = 255;
		  if (eg > 255)
		     eg = 255;
		  if (eb > 255)
		     eb = 255;
		  val = ((er & 0xf8) << 7) | ((eg & 0xf8) << 2) | ((eb & 0xf8) >> 3);
		  er = er & 0x07;
		  eg = eg & 0x07;
		  eb = eb & 0x07;
		  if (er > 255)
		     er = 255;
		  else if (er < 0)
		     er = 0;
		  if (eg > 255)
		     eg = 255;
		  else if (eg < 0)
		     eg = 0;
		  if (eb > 255)
		     eb = 255;
		  else if (eb < 0)
		     eb = 0;
		  val = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
		  er = r & 0x07;
		  eg = g & 0x07;
		  eb = b & 0x07;
		  DITHER_ERROR(er1, er2, ex, er, eg, eb);
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
}

void
grender_15_dither_mod(GdkImlibImage * im, int w, int h, XImage * xim,
		      XImage * sxim, int *er1, int *er2, int *xarray,
		      unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   for (y = 0; y < h; y++)
     {
	ter = er1;
	er1 = er2;
	er2 = ter;
	for (ex = 0; ex < (w + 2) * 3; ex++)
	   er2[ex] = 0;
	ex = 3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     r = im->rmap[r];
	     g = im->gmap[g];
	     b = im->bmap[b];
	     er = r + er1[ex++];
	     eg = g + er1[ex++];
	     eb = b + er1[ex++];
	     if (er > 255)
		er = 255;
	     if (eg > 255)
		eg = 255;
	     if (eb > 255)
		eb = 255;
	     val = ((er & 0xf8) << 7) | ((eg & 0xf8) << 2) | ((eb & 0xf8) >> 3);
	     er = er & 0x07;
	     eg = eg & 0x07;
	     eb = eb & 0x07;
	     DITHER_ERROR(er1, er2, ex, er, eg, eb);
	     XPutPixel(xim, x, y, val);
	  }
     }
}

void
grender_shaped_16_dither_mod(GdkImlibImage * im, int w, int h, XImage * xim,
			     XImage * sxim, int *er1, int *er2, int *xarray,
			     unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   for (y = 0; y < h; y++)
     {
	ter = er1;
	er1 = er2;
	er2 = ter;
	for (ex = 0; ex < (w + 2) * 3; ex++)
	   er2[ex] = 0;
	ex = 3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
	       {
		  XPutPixel(sxim, x, y, 0);
		  ex += 3;
	       }
	     else
	       {
		  XPutPixel(sxim, x, y, 1);
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  er = r + er1[ex++];
		  eg = g + er1[ex++];
		  eb = b + er1[ex++];
		  if (er > 255)
		     er = 255;
		  if (eg > 255)
		     eg = 255;
		  if (eb > 255)
		     eb = 255;
		  val = ((er & 0xf8) << 8) | ((eg & 0xfc) << 3) | ((eb & 0xf8) >> 3);
		  er = er & 0x07;
		  eg = eg & 0x03;
		  eb = eb & 0x07;
		  DITHER_ERROR(er1, er2, ex, er, eg, eb);
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
}

void
grender_16_dither_mod(GdkImlibImage * im, int w, int h, XImage * xim,
		      XImage * sxim, int *er1, int *er2, int *xarray,
		      unsigned char **yarray)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;

   for (y = 0; y < h; y++)
     {
	ter = er1;
	er1 = er2;
	er2 = ter;
	for (ex = 0; ex < (w + 2) * 3; ex++)
	   er2[ex] = 0;
	ex = 3;
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     r = im->rmap[r];
	     g = im->gmap[g];
	     b = im->bmap[b];
	     er = r + er1[ex++];
	     eg = g + er1[ex++];
	     eb = b + er1[ex++];
	     if (er > 255)
		er = 255;
	     if (eg > 255)
		eg = 255;
	     if (eb > 255)
		eb = 255;
	     val = ((er & 0xf8) << 8) | ((eg & 0xfc) << 3) | ((eb & 0xf8) >> 3);
	     er = er & 0x07;
	     eg = eg & 0x03;
	     eb = eb & 0x07;
	     DITHER_ERROR(er1, er2, ex, er, eg, eb);
	     XPutPixel(xim, x, y, val);
	  }
     }
}

void
grender_shaped_15_fast_mod(GdkImlibImage * im, int w, int h, XImage * xim,
			   XImage * sxim, int *er1, int *er2, int *xarray,
			   unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;
   unsigned short     *img;
   int                 jmp;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
	       {
		  XPutPixel(sxim, x, y, 0);
		  img++;
	       }
	     else
	       {
		  XPutPixel(sxim, x, y, 1);
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  val = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
		  *img++ = val;
	       }
	  }
	img += jmp;
     }
}

void
grender_15_fast_mod(GdkImlibImage * im, int w, int h, XImage * xim,
		    XImage * sxim, int *er1, int *er2, int *xarray,
		    unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;
   unsigned short     *img;
   int                 jmp;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     r = im->rmap[r];
	     g = im->gmap[g];
	     b = im->bmap[b];
	     val = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
	     *img++ = val;
	  }
	img += jmp;
     }
}

void
grender_shaped_16_fast_mod(GdkImlibImage * im, int w, int h, XImage * xim,
			   XImage * sxim, int *er1, int *er2, int *xarray,
			   unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;
   unsigned short     *img;
   int                 jmp;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
	       {
		  XPutPixel(sxim, x, y, 0);
		  img++;
	       }
	     else
	       {
		  XPutPixel(sxim, x, y, 1);
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  val = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
		  *img++ = val;
	       }
	  }
	img += jmp;
     }
}

void
grender_16_fast_mod(GdkImlibImage * im, int w, int h, XImage * xim,
		    XImage * sxim, int *er1, int *er2, int *xarray,
		    unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;

   unsigned short     *img;
   int                 jmp;

   jmp = (xim->bytes_per_line >> 1) - w;
   img = (unsigned short *)xim->data;
   for (y = 0; y < h; y++)
     {
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     r = im->rmap[r];
	     g = im->gmap[g];
	     b = im->bmap[b];
	     val = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
	     *img++ = val;
	  }
	img += jmp;
     }
}

void
grender_shaped_24_fast_mod(GdkImlibImage * im, int w, int h, XImage * xim,
			   XImage * sxim, int *er1, int *er2, int *xarray,
			   unsigned char **yarray)
{
   int                 x, y, r, g, b;
   unsigned char      *ptr2;
   unsigned char      *img;
   int                 jmp;

   jmp = (xim->bytes_per_line) - w;
   img = (unsigned char *)xim->data;
   if (id->byte_order == BYTE_ORD_24_RGB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img += 3;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       *img++ = r;
		       *img++ = g;
		       *img++ = b;
		    }
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_RBG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img += 3;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       *img++ = r;
		       *img++ = b;
		       *img++ = g;
		    }
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BRG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img += 3;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       *img++ = b;
		       *img++ = r;
		       *img++ = g;
		    }
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BGR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img += 3;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       *img++ = b;
		       *img++ = g;
		       *img++ = r;
		    }
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GRB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img += 3;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       *img++ = g;
		       *img++ = r;
		       *img++ = b;
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		    }
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GBR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img += 3;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       *img++ = g;
		       *img++ = b;
		       *img++ = r;
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		    }
	       }
	     img += jmp;
	  }
     }
}

void
grender_24_fast_mod(GdkImlibImage * im, int w, int h, XImage * xim,
		    XImage * sxim, int *er1, int *er2, int *xarray,
		    unsigned char **yarray)
{
   int                 x, y, r, g, b;
   unsigned char      *ptr2;
   unsigned char      *img;
   int                 jmp;

   jmp = (xim->bytes_per_line) - w;
   img = (unsigned char *)xim->data;
   if (id->byte_order == BYTE_ORD_24_RGB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  *img++ = r;
		  *img++ = g;
		  *img++ = b;
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_RBG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  *img++ = r;
		  *img++ = b;
		  *img++ = g;
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BRG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  *img++ = b;
		  *img++ = r;
		  *img++ = g;
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BGR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  *img++ = b;
		  *img++ = g;
		  *img++ = r;
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GRB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  *img++ = g;
		  *img++ = r;
		  *img++ = b;
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GBR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  *img++ = g;
		  *img++ = b;
		  *img++ = r;
	       }
	     img += jmp;
	  }
     }
}

void
grender_shaped_32_fast_mod(GdkImlibImage * im, int w, int h, XImage * xim,
			   XImage * sxim, int *er1, int *er2, int *xarray,
			   unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;
   unsigned int       *img;
   int                 jmp;

   jmp = (xim->bytes_per_line >> 2) - w;
   img = (unsigned int *)xim->data;
   if (id->byte_order == BYTE_ORD_24_RGB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img++;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       val = (r << 16) | (g << 8) | b;
		       *img++ = val;
		    }
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_RBG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img++;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       val = (r << 16) | (b << 8) | g;
		       *img++ = val;
		    }
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BRG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img++;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       val = (b << 16) | (r << 8) | g;
		       *img++ = val;
		    }
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BGR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img++;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       val = (b << 16) | (g << 8) | r;
		       *img++ = val;
		    }
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GRB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img++;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       val = (g << 16) | (r << 8) | b;
		       *img++ = val;
		    }
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GBR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		    {
		       XPutPixel(sxim, x, y, 0);
		       img++;
		    }
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       val = (g << 16) | (b << 8) | r;
		       *img++ = val;
		    }
	       }
	     img += jmp;
	  }
     }
}

void
grender_32_fast_mod(GdkImlibImage * im, int w, int h, XImage * xim,
		    XImage * sxim, int *er1, int *er2, int *xarray,
		    unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;
   unsigned int       *img;
   int                 jmp;

   jmp = (xim->bytes_per_line >> 2) - w;
   img = (unsigned int *)xim->data;
   if (id->byte_order == BYTE_ORD_24_RGB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  val = (r << 16) | (g << 8) | b;
		  *img++ = val;
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_RBG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  val = (r << 16) | (b << 8) | g;
		  *img++ = val;
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BRG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  val = (b << 16) | (r << 8) | g;
		  *img++ = val;
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BGR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  val = (b << 16) | (g << 8) | r;
		  *img++ = val;
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GRB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  val = (g << 16) | (r << 8) | b;
		  *img++ = val;
	       }
	     img += jmp;
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GBR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  val = (g << 16) | (b << 8) | r;
		  *img++ = val;
	       }
	     img += jmp;
	  }
     }
}

void
grender_shaped_15_mod(GdkImlibImage * im, int w, int h, XImage * xim,
		      XImage * sxim, int *er1, int *er2, int *xarray,
		      unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;

   for (y = 0; y < h; y++)
     {
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
		XPutPixel(sxim, x, y, 0);
	     else
	       {
		  XPutPixel(sxim, x, y, 1);
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  val = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
}

void
grender_15_mod(GdkImlibImage * im, int w, int h, XImage * xim,
	       XImage * sxim, int *er1, int *er2, int *xarray,
	       unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;

   for (y = 0; y < h; y++)
     {
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     r = im->rmap[r];
	     g = im->gmap[g];
	     b = im->bmap[b];
	     val = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
	     XPutPixel(xim, x, y, val);
	  }
     }
}

void
grender_shaped_16_mod(GdkImlibImage * im, int w, int h, XImage * xim,
		      XImage * sxim, int *er1, int *er2, int *xarray,
		      unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;

   for (y = 0; y < h; y++)
     {
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     if ((r == im->shape_color.r) &&
		 (g == im->shape_color.g) &&
		 (b == im->shape_color.b))
		XPutPixel(sxim, x, y, 0);
	     else
	       {
		  XPutPixel(sxim, x, y, 1);
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  val = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
}

void
grender_16_mod(GdkImlibImage * im, int w, int h, XImage * xim,
	       XImage * sxim, int *er1, int *er2, int *xarray,
	       unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;

   for (y = 0; y < h; y++)
     {
	for (x = 0; x < w; x++)
	  {
	     ptr2 = yarray[y] + xarray[x];
	     r = (int)*ptr2++;
	     g = (int)*ptr2++;
	     b = (int)*ptr2;
	     r = im->rmap[r];
	     g = im->gmap[g];
	     b = im->bmap[b];
	     val = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
	     XPutPixel(xim, x, y, val);
	  }
     }
}

void
grender_shaped_24_mod(GdkImlibImage * im, int w, int h, XImage * xim,
		      XImage * sxim, int *er1, int *er2, int *xarray,
		      unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;

   if (id->byte_order == BYTE_ORD_24_RGB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		     XPutPixel(sxim, x, y, 0);
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       val = (r << 16) | (g << 8) | b;
		       XPutPixel(xim, x, y, val);
		    }
	       }
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_RBG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		     XPutPixel(sxim, x, y, 0);
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       val = (r << 16) | (b << 8) | g;
		       XPutPixel(xim, x, y, val);
		    }
	       }
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BRG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		     XPutPixel(sxim, x, y, 0);
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       val = (b << 16) | (r << 8) | g;
		       XPutPixel(xim, x, y, val);
		    }
	       }
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BGR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		     XPutPixel(sxim, x, y, 0);
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       val = (b << 16) | (g << 8) | r;
		       XPutPixel(xim, x, y, val);
		    }
	       }
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GRB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		     XPutPixel(sxim, x, y, 0);
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       val = (g << 16) | (r << 8) | b;
		       XPutPixel(xim, x, y, val);
		    }
	       }
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GBR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  if ((r == im->shape_color.r) &&
		      (g == im->shape_color.g) &&
		      (b == im->shape_color.b))
		     XPutPixel(sxim, x, y, 0);
		  else
		    {
		       XPutPixel(sxim, x, y, 1);
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       val = (g << 16) | (b << 8) | r;
		       XPutPixel(xim, x, y, val);
		    }
	       }
	  }
     }
}

void
grender_24_mod(GdkImlibImage * im, int w, int h, XImage * xim,
	       XImage * sxim, int *er1, int *er2, int *xarray,
	       unsigned char **yarray)
{
   int                 x, y, val, r, g, b;
   unsigned char      *ptr2;

   if (id->byte_order == BYTE_ORD_24_RGB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  val = (r << 16) | (g << 8) | b;
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_RBG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  val = (r << 16) | (b << 8) | g;
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BRG)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  val = (b << 16) | (r << 8) | g;
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_BGR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  val = (b << 16) | (g << 8) | r;
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GRB)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  val = (g << 16) | (r << 8) | b;
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
   else if (id->byte_order == BYTE_ORD_24_GBR)
     {
	for (y = 0; y < h; y++)
	  {
	     for (x = 0; x < w; x++)
	       {
		  ptr2 = yarray[y] + xarray[x];
		  r = (int)*ptr2++;
		  g = (int)*ptr2++;
		  b = (int)*ptr2;
		  r = im->rmap[r];
		  g = im->gmap[g];
		  b = im->bmap[b];
		  val = (g << 16) | (b << 8) | r;
		  XPutPixel(xim, x, y, val);
	       }
	  }
     }
}

void
grender_shaped_mod(GdkImlibImage * im, int w, int h, XImage * xim,
		   XImage * sxim, int *er1, int *er2, int *xarray,
		   unsigned char **yarray, int bpp)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;
   unsigned char      *img;
   int                 jmp;

   jmp = (xim->bytes_per_line) - w;
   img = (unsigned char *)xim->data;
   switch (id->render_type)
     {
     case RT_PLAIN_PALETTE:
	if ((id->fastrend) && (xim->bits_per_pixel == 8))
	  {
	     for (y = 0; y < h; y++)
	       {
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       if ((r == im->shape_color.r) &&
			   (g == im->shape_color.g) &&
			   (b == im->shape_color.b))
			 {
			    XPutPixel(sxim, x, y, 0);
			    img++;
			 }
		       else
			 {
			    XPutPixel(sxim, x, y, 1);
			    r = im->rmap[r];
			    g = im->gmap[g];
			    b = im->bmap[b];
			    val = gdk_imlib_best_color_match(&r, &g, &b);
			    *img++ = val;
			 }
		    }
		  img += jmp;
	       }
	  }
	else
	  {
	     for (y = 0; y < h; y++)
	       {
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       if ((r == im->shape_color.r) &&
			   (g == im->shape_color.g) &&
			   (b == im->shape_color.b))
			  XPutPixel(sxim, x, y, 0);
		       else
			 {
			    XPutPixel(sxim, x, y, 1);
			    r = im->rmap[r];
			    g = im->gmap[g];
			    b = im->bmap[b];
			    val = gdk_imlib_best_color_match(&r, &g, &b);
			    XPutPixel(xim, x, y, val);
			 }
		    }
	       }
	  }
	break;
     case RT_PLAIN_PALETTE_FAST:
	if ((id->fastrend) && (xim->bits_per_pixel == 8))
	  {
	     for (y = 0; y < h; y++)
	       {
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       if ((r == im->shape_color.r) &&
			   (g == im->shape_color.g) &&
			   (b == im->shape_color.b))
			 {
			    XPutPixel(sxim, x, y, 0);
			    img++;
			 }
		       else
			 {
			    XPutPixel(sxim, x, y, 1);
			    r = im->rmap[r];
			    g = im->gmap[g];
			    b = im->bmap[b];
			    val = COLOR_RGB(r >> 3, g >> 3, b >> 3);
			    *img++ = val;
			 }
		    }
		  img += jmp;
	       }
	  }
	else
	  {
	     for (y = 0; y < h; y++)
	       {
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       if ((r == im->shape_color.r) &&
			   (g == im->shape_color.g) &&
			   (b == im->shape_color.b))
			  XPutPixel(sxim, x, y, 0);
		       else
			 {
			    XPutPixel(sxim, x, y, 1);
			    r = im->rmap[r];
			    g = im->gmap[g];
			    b = im->bmap[b];
			    val = COLOR_RGB(r >> 3, g >> 3, b >> 3);
			    XPutPixel(xim, x, y, val);
			 }
		    }
	       }
	  }
	break;
     case RT_DITHER_PALETTE:
	if ((id->fastrend) && (xim->bits_per_pixel == 8))
	  {
	     for (y = 0; y < h; y++)
	       {
		  ter = er1;
		  er1 = er2;
		  er2 = ter;
		  for (ex = 0; ex < (w + 2) * 3; ex++)
		     er2[ex] = 0;
		  ex = 3;
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       if ((r == im->shape_color.r) &&
			   (g == im->shape_color.g) &&
			   (b == im->shape_color.b))
			 {
			    {
			       XPutPixel(sxim, x, y, 0);
			       img++;
			    }
			    ex += 3;
			 }
		       else
			 {
			    XPutPixel(sxim, x, y, 1);
			    r = im->rmap[r];
			    g = im->gmap[g];
			    b = im->bmap[b];
			    er = r + er1[ex++];
			    eg = g + er1[ex++];
			    eb = b + er1[ex++];
			    if (er > 255)
			       er = 255;
			    else if (er < 0)
			       er = 0;
			    if (eg > 255)
			       eg = 255;
			    else if (eg < 0)
			       eg = 0;
			    if (eb > 255)
			       eb = 255;
			    else if (eb < 0)
			       eb = 0;
			    val = gdk_imlib_best_color_match(&er, &eg, &eb);
			    DITHER_ERROR(er1, er2, ex, er, eg, eb);
			    *img++ = val;
			 }
		    }
		  img += jmp;
	       }
	  }
	else
	  {
	     for (y = 0; y < h; y++)
	       {
		  ter = er1;
		  er1 = er2;
		  er2 = ter;
		  for (ex = 0; ex < (w + 2) * 3; ex++)
		     er2[ex] = 0;
		  ex = 3;
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       if ((r == im->shape_color.r) &&
			   (g == im->shape_color.g) &&
			   (b == im->shape_color.b))
			 {
			    XPutPixel(sxim, x, y, 0);
			    ex += 3;
			 }
		       else
			 {
			    XPutPixel(sxim, x, y, 1);
			    r = im->rmap[r];
			    g = im->gmap[g];
			    b = im->bmap[b];
			    er = r + er1[ex++];
			    eg = g + er1[ex++];
			    eb = b + er1[ex++];
			    if (er > 255)
			       er = 255;
			    else if (er < 0)
			       er = 0;
			    if (eg > 255)
			       eg = 255;
			    else if (eg < 0)
			       eg = 0;
			    if (eb > 255)
			       eb = 255;
			    else if (eb < 0)
			       eb = 0;
			    val = gdk_imlib_best_color_match(&er, &eg, &eb);
			    DITHER_ERROR(er1, er2, ex, er, eg, eb);
			    XPutPixel(xim, x, y, val);
			 }
		    }
	       }
	  }
	break;
     case RT_DITHER_PALETTE_FAST:
	if ((id->fastrend) && (xim->bits_per_pixel == 8))
	  {
	     for (y = 0; y < h; y++)
	       {
		  ter = er1;
		  er1 = er2;
		  er2 = ter;
		  for (ex = 0; ex < (w + 2) * 3; ex++)
		     er2[ex] = 0;
		  ex = 3;
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       if ((r == im->shape_color.r) &&
			   (g == im->shape_color.g) &&
			   (b == im->shape_color.b))
			 {
			    {
			       XPutPixel(sxim, x, y, 0);
			       img++;
			    }
			    ex += 3;
			 }
		       else
			 {
			    XPutPixel(sxim, x, y, 1);
			    r = im->rmap[r];
			    g = im->gmap[g];
			    b = im->bmap[b];
			    er = r + er1[ex++];
			    eg = g + er1[ex++];
			    eb = b + er1[ex++];
			    if (er > 255)
			       er = 255;
			    else if (er < 0)
			       er = 0;
			    if (eg > 255)
			       eg = 255;
			    else if (eg < 0)
			       eg = 0;
			    if (eb > 255)
			       eb = 255;
			    else if (eb < 0)
			       eb = 0;
			    val = INDEX_RGB(er >> 3, eg >> 3, eb >> 3);
			    er = ERROR_RED(er, val);
			    eg = ERROR_GRN(eg, val);
			    eb = ERROR_BLU(eb, val);
			    DITHER_ERROR(er1, er2, ex, er, eg, eb);
			    *img++ = COLOR_INDEX(val);
			 }
		    }
		  img += jmp;
	       }
	  }
	else
	  {
	     for (y = 0; y < h; y++)
	       {
		  ter = er1;
		  er1 = er2;
		  er2 = ter;
		  for (ex = 0; ex < (w + 2) * 3; ex++)
		     er2[ex] = 0;
		  ex = 3;
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       if ((r == im->shape_color.r) &&
			   (g == im->shape_color.g) &&
			   (b == im->shape_color.b))
			 {
			    XPutPixel(sxim, x, y, 0);
			    ex += 3;
			 }
		       else
			 {
			    XPutPixel(sxim, x, y, 1);
			    r = im->rmap[r];
			    g = im->gmap[g];
			    b = im->bmap[b];
			    er = r + er1[ex++];
			    eg = g + er1[ex++];
			    eb = b + er1[ex++];
			    if (er > 255)
			       er = 255;
			    else if (er < 0)
			       er = 0;
			    if (eg > 255)
			       eg = 255;
			    else if (eg < 0)
			       eg = 0;
			    if (eb > 255)
			       eb = 255;
			    else if (eb < 0)
			       eb = 0;
			    val = INDEX_RGB(er >> 3, eg >> 3, eb >> 3);
			    er = ERROR_RED(er, val);
			    eg = ERROR_GRN(eg, val);
			    eb = ERROR_BLU(eb, val);
			    DITHER_ERROR(er1, er2, ex, er, eg, eb);
			    XPutPixel(xim, x, y, COLOR_INDEX(val));
			 }
		    }
	       }
	  }
	break;
     default:
	if (id->fastrend)
	  {
	     switch (bpp)
	       {
	       case 8:
		  break;
	       case 15:
		  if (id->render_type == RT_DITHER_TRUECOL)
		    {
		       if (id->ordered_dither)
			  grender_shaped_15_fast_dither_mod_ordered(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		       else
			  grender_shaped_15_fast_dither_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		    }
		  else
		     grender_shaped_15_fast_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       case 16:
		  if (id->render_type == RT_DITHER_TRUECOL)
		    {
		       if (id->ordered_dither)
			  grender_shaped_16_fast_dither_mod_ordered(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		       else
			  grender_shaped_16_fast_dither_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		    }
		  else
		     grender_shaped_16_fast_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       case 24:
	       case 32:
		  if (xim->bits_per_pixel == 24)
		     grender_shaped_24_fast_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  else
		     grender_shaped_32_fast_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       default:
		  break;
	       }
	  }
	else
	  {
	     switch (bpp)
	       {
	       case 8:
		  break;
	       case 15:
		  if (id->render_type == RT_DITHER_TRUECOL)
		    {
		       if (id->ordered_dither)
			  grender_shaped_15_dither_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		       grender_shaped_15_dither_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		    }
		  else
		     grender_shaped_15_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       case 16:
		  if (id->render_type == RT_DITHER_TRUECOL)
		    {
		       if (id->ordered_dither)
			  grender_shaped_16_dither_mod_ordered(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		       else
			  grender_shaped_16_dither_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		    }
		  else
		     grender_shaped_16_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       case 24:
		  grender_shaped_24_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
	       case 32:
		  grender_shaped_24_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       default:
		  break;
	       }
	  }
	break;
     }
}

void
grender_mod(GdkImlibImage * im, int w, int h, XImage * xim,
	    XImage * sxim, int *er1, int *er2, int *xarray,
	    unsigned char **yarray, int bpp)
{
   int                 x, y, val, r, g, b, *ter, ex, er, eg, eb;
   unsigned char      *ptr2;
   unsigned char      *img;
   int                 jmp;

   jmp = (xim->bytes_per_line) - w;
   img = (unsigned char *)xim->data;
   switch (id->render_type)
     {
     case RT_PLAIN_PALETTE:
	if ((id->fastrend) && (xim->bits_per_pixel == 8))
	  {
	     for (y = 0; y < h; y++)
	       {
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       val = gdk_imlib_best_color_match(&r, &g, &b);
		       *img++ = val;
		    }
		  img += jmp;
	       }
	  }
	else
	  {
	     for (y = 0; y < h; y++)
	       {
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       val = gdk_imlib_best_color_match(&r, &g, &b);
		       XPutPixel(xim, x, y, val);
		    }
	       }
	  }
	break;
     case RT_PLAIN_PALETTE_FAST:
	if ((id->fastrend) && (xim->bits_per_pixel == 8))
	  {
	     for (y = 0; y < h; y++)
	       {
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       val = COLOR_RGB(r >> 3, g >> 3, b >> 3);
		       *img++ = val;
		    }
		  img += jmp;
	       }
	  }
	else
	  {
	     for (y = 0; y < h; y++)
	       {
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       val = COLOR_RGB(r >> 3, g >> 3, b >> 3);
		       XPutPixel(xim, x, y, val);
		    }
	       }
	  }
	break;
     case RT_DITHER_PALETTE:
	if ((id->fastrend) && (xim->bits_per_pixel == 8))
	  {
	     for (y = 0; y < h; y++)
	       {
		  ter = er1;
		  er1 = er2;
		  er2 = ter;
		  for (ex = 0; ex < (w + 2) * 3; ex++)
		     er2[ex] = 0;
		  ex = 3;
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       er = r + er1[ex++];
		       eg = g + er1[ex++];
		       eb = b + er1[ex++];
		       if (er > 255)
			  er = 255;
		       else if (er < 0)
			  er = 0;
		       if (eg > 255)
			  eg = 255;
		       else if (eg < 0)
			  eg = 0;
		       if (eb > 255)
			  eb = 255;
		       else if (eb < 0)
			  eb = 0;
		       val = gdk_imlib_best_color_match(&er, &eg, &eb);
		       DITHER_ERROR(er1, er2, ex, er, eg, eb);
		       *img++ = val;
		    }
		  img += jmp;
	       }
	  }
	else
	  {
	     for (y = 0; y < h; y++)
	       {
		  ter = er1;
		  er1 = er2;
		  er2 = ter;
		  for (ex = 0; ex < (w + 2) * 3; ex++)
		     er2[ex] = 0;
		  ex = 3;
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       er = r + er1[ex++];
		       eg = g + er1[ex++];
		       eb = b + er1[ex++];
		       if (er > 255)
			  er = 255;
		       else if (er < 0)
			  er = 0;
		       if (eg > 255)
			  eg = 255;
		       else if (eg < 0)
			  eg = 0;
		       if (eb > 255)
			  eb = 255;
		       else if (eb < 0)
			  eb = 0;
		       val = gdk_imlib_best_color_match(&er, &eg, &eb);
		       DITHER_ERROR(er1, er2, ex, er, eg, eb);
		       XPutPixel(xim, x, y, val);
		    }
	       }
	  }
	break;
     case RT_DITHER_PALETTE_FAST:
	if ((id->fastrend) && (xim->bits_per_pixel == 8))
	  {
	     for (y = 0; y < h; y++)
	       {
		  ter = er1;
		  er1 = er2;
		  er2 = ter;
		  for (ex = 0; ex < (w + 2) * 3; ex++)
		     er2[ex] = 0;
		  ex = 3;
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       er = r + er1[ex++];
		       eg = g + er1[ex++];
		       eb = b + er1[ex++];
		       if (er > 255)
			  er = 255;
		       else if (er < 0)
			  er = 0;
		       if (eg > 255)
			  eg = 255;
		       else if (eg < 0)
			  eg = 0;
		       if (eb > 255)
			  eb = 255;
		       else if (eb < 0)
			  eb = 0;
		       val = INDEX_RGB(er >> 3, eg >> 3, eb >> 3);
		       er = ERROR_RED(er, val);
		       eg = ERROR_GRN(eg, val);
		       eb = ERROR_BLU(eb, val);
		       DITHER_ERROR(er1, er2, ex, er, eg, eb);
		       *img++ = COLOR_INDEX(val);
		    }
		  img += jmp;
	       }
	  }
	else
	  {
	     for (y = 0; y < h; y++)
	       {
		  ter = er1;
		  er1 = er2;
		  er2 = ter;
		  for (ex = 0; ex < (w + 2) * 3; ex++)
		     er2[ex] = 0;
		  ex = 3;
		  for (x = 0; x < w; x++)
		    {
		       ptr2 = yarray[y] + xarray[x];
		       r = (int)*ptr2++;
		       g = (int)*ptr2++;
		       b = (int)*ptr2;
		       r = im->rmap[r];
		       g = im->gmap[g];
		       b = im->bmap[b];
		       er = r + er1[ex++];
		       eg = g + er1[ex++];
		       eb = b + er1[ex++];
		       if (er > 255)
			  er = 255;
		       else if (er < 0)
			  er = 0;
		       if (eg > 255)
			  eg = 255;
		       else if (eg < 0)
			  eg = 0;
		       if (eb > 255)
			  eb = 255;
		       else if (eb < 0)
			  eb = 0;
		       val = INDEX_RGB(er >> 3, eg >> 3, eb >> 3);
		       er = ERROR_RED(er, val);
		       eg = ERROR_GRN(eg, val);
		       eb = ERROR_BLU(eb, val);
		       DITHER_ERROR(er1, er2, ex, er, eg, eb);
		       XPutPixel(xim, x, y, COLOR_INDEX(val));
		    }
	       }
	  }
	break;
     default:
	if (id->fastrend)
	  {
	     switch (bpp)
	       {
	       case 8:
		  break;
	       case 15:
		  if (id->render_type == RT_DITHER_TRUECOL)
		    {
		       if (id->ordered_dither)
			  grender_15_fast_dither_mod_ordered(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		       else
			  grender_15_fast_dither_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		    }
		  else
		     grender_15_fast_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       case 16:
		  if (id->render_type == RT_DITHER_TRUECOL)
		    {
		       if (id->ordered_dither)
			  grender_16_fast_dither_mod_ordered(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		       else
			  grender_16_fast_dither_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		    }
		  else
		     grender_16_fast_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       case 24:
	       case 32:
		  if (xim->bits_per_pixel == 24)
		     grender_24_fast_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  else
		     grender_32_fast_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       default:
		  break;
	       }
	  }
	else
	  {
	     switch (bpp)
	       {
	       case 8:
		  break;
	       case 15:
		  if (id->render_type == RT_DITHER_TRUECOL)
		    {
		       if (id->ordered_dither)
			  grender_15_dither_mod_ordered(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		       else
			  grender_15_dither_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		    }
		  else
		     grender_15_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       case 16:
		  if (id->render_type == RT_DITHER_TRUECOL)
		    {
		       if (id->ordered_dither)
			  grender_16_dither_mod_ordered(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		       else
			  grender_16_dither_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		    }
		  else
		     grender_16_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       case 24:
		  grender_24_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       case 32:
		  grender_24_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray);
		  break;
	       default:
		  break;
	       }
	     break;
	  }
     }
}

gint
gdk_imlib_render(GdkImlibImage * im, gint w, gint h)
{
   XImage             *xim, *sxim;
   GC                  tgc, stgc;
   XGCValues           gcv;
   unsigned char      *tmp, *stmp, **yarray, *ptr22;
   int                 w3, x, inc, pos, *error, *er1, *er2, *xarray, ex,
                       bpp, huge;
   Pixmap              pmap, mask;
   GdkPixmap          *pm, *mm;
   int                 shared_pixmap, shared_image, ok;

   sxim = NULL;
   xim = NULL;
   tmp = NULL;
   stmp = NULL;
   pmap = 0;
   mask = 0;
   tgc = 0;
   stgc = 0;
   inc = 0;
   if (!im)
      return 0;
   if (w <= 0)
      return 0;
   if (h <= 0)
      return 0;
   gcv.graphics_exposures = False;

/* look for the pixmap in cache first */
   if (id->cache.on_pixmap)
     {
	pmap = 0;
	gfind_pixmap(im, w, h, &pm, &mm);
	if (pm)
	  {
	     im->width = w;
	     im->height = h;
	     im->pixmap = pm;
	     if (mm)
		im->shape_mask = mm;
	     else
		im->shape_mask = NULL;
	     return 1;
	  }
     }

   if (im->pixmap)
      gfree_pixmappmap(im->pixmap);
   im->pixmap = NULL;
   im->shape_mask = NULL;
/* setup stuff */
   huge = 0;
   if (id->x.depth <= 8)
      bpp = 1;
   else if (id->x.depth <= 16)
      bpp = 2;
   else
      bpp = 4;
   if ((id->max_shm) && ((bpp * w * h) > id->max_shm))
      huge = 1;
   im->width = w;
   im->height = h;

/* dithering array */
   error = (int *)malloc(sizeof(int) * (w + 2) * 2 * 3);

   if (!error)
     {
	fprintf(stderr, "ERROR: Cannot allocate RAM for image dither buffer\n");
	return 0;
     }

/* setup pointers to point right */
   er1 = error;
   er2 = error + ((w + 2) * 3);
   w3 = im->rgb_width * 3;
   ptr22 = im->rgb_data;

/* setup coord-mapping array (specially for border scaling) */
   xarray = malloc(sizeof(int) * w);

   if (!xarray)
     {
	fprintf(stderr, "ERROR: Cannot allocate X co-ord buffer\n");
	free(error);
	return 0;
     }
   yarray = malloc(sizeof(unsigned char *) * h);

   if (!yarray)
     {
	fprintf(stderr, "ERROR: Cannot allocate Y co-ord buffer\n");
	free(xarray);
	free(error);
	return 0;
     }
   for (ex = 0; ex < ((w + 2) * 3 * 2); ex++)
      error[ex] = 0;
   {
      int                 l, r, m;

      if (w < im->border.left + im->border.right)
	{
	   l = w >> 1;
	   r = w - l;
	   m = 0;
	}
      else
	{
	   l = im->border.left;
	   r = im->border.right;
	   m = w - l - r;
	}
      if (m > 0)
	 inc = ((im->rgb_width - im->border.left - im->border.right) << 16) / m;
      pos = 0;
      if (l)
	 for (x = 0; x < l; x++)
	   {
	      xarray[x] = (pos >> 16) + (pos >> 16) + (pos >> 16);
	      pos += 0x10000;
	   }
      if (m)
	{
	   for (x = l; x < l + m; x++)
	     {
		xarray[x] = (pos >> 16) + (pos >> 16) + (pos >> 16);
		pos += inc;
	     }
	}
      pos = (im->rgb_width - r) << 16;
      for (x = w - r; x < w; x++)
	{
	   xarray[x] = (pos >> 16) + (pos >> 16) + (pos >> 16);
	   pos += 0x10000;
	}

      if (h < im->border.top + im->border.bottom)
	{
	   l = h >> 1;
	   r = h - l;
	   m = 0;
	}
      else
	{
	   l = im->border.top;
	   r = im->border.bottom;
	   m = h - l - r;
	}
      if (m > 0)
	 inc = ((im->rgb_height - im->border.top - im->border.bottom) << 16) / m;
      pos = 0;
      for (x = 0; x < l; x++)
	{
	   yarray[x] = ptr22 + ((pos >> 16) * w3);
	   pos += 0x10000;
	}
      if (m)
	{
	   for (x = l; x < l + m; x++)
	     {
		yarray[x] = ptr22 + ((pos >> 16) * w3);
		pos += inc;
	     }
	}
      pos = (im->rgb_height - r) << 16;
      for (x = h - r; x < h; x++)
	{
	   yarray[x] = ptr22 + ((pos >> 16) * w3);
	   pos += 0x10000;
	}
   }

/* work out if we should use shared pixmap. images etc */
   shared_pixmap = 0;
   shared_image = 0;
   if ((id->x.shmp) && (id->x.shm) && (!huge))
     {
	shared_pixmap = 1;
	shared_image = 0;
     }
   else if ((id->x.shm) && (!huge))
     {
	shared_pixmap = 0;
	shared_image = 1;
     }
   else
      shared_pixmap = 0;
   shared_image = 0;

/* init images and pixmaps */
   ok = 1;
   if (shared_pixmap)
     {
	xim = XShmCreateImage(id->x.disp, id->x.visual, id->x.depth, ZPixmap, NULL, &id->x.last_shminfo, w, h);
	if (!xim)
	  {
	     fprintf(stderr, "IMLIB ERROR: Mit-SHM can't create XImage for Shared Pixmap Wrapper\n");
	     fprintf(stderr, "             Falling back on Shared XImages\n");
	     shared_pixmap = 0;
	     shared_image = 1;
	     ok = 0;
	  }
	if (ok)
	  {
	     id->x.last_shminfo.shmid = shmget(IPC_PRIVATE, xim->bytes_per_line * xim->height, IPC_CREAT | 0777);
	     if (id->x.last_shminfo.shmid == -1)
	       {
		  fprintf(stderr, "IMLIB ERROR: SHM can't get SHM Identifier for Shared Pixmap Wrapper\n");
		  fprintf(stderr, "             Falling back on Shared XImages\n");
		  XDestroyImage(xim);
		  shared_pixmap = 0;
		  shared_image = 1;
		  ok = 1;
	       }
	     if (ok)
	       {
		  id->x.last_shminfo.shmaddr = xim->data = shmat(id->x.last_shminfo.shmid, 0, 0);
		  id->x.last_shminfo.readOnly = False;
		  XShmAttach(id->x.disp, &id->x.last_shminfo);
		  tmp = (unsigned char *)xim->data;
		  id->x.last_xim = xim;
		  pmap = XShmCreatePixmap(id->x.disp, id->x.base_window,
					  id->x.last_shminfo.shmaddr,
				      &id->x.last_shminfo, w, h, id->x.depth);
		  tgc = XCreateGC(id->x.disp, pmap, GCGraphicsExposures, &gcv);
		  if ((im->shape_color.r >= 0) && (im->shape_color.g >= 0) && (im->shape_color.b >= 0))
		    {
		       sxim = XShmCreateImage(id->x.disp, id->x.visual, 1, ZPixmap, NULL, &id->x.last_sshminfo, w, h);
		       if (!sxim)
			 {
			    fprintf(stderr, "IMLIB ERROR: Mit-SHM can't create XImage for Shared Pixmap mask Wrapper\n");
			    fprintf(stderr, "             Falling back on Shared XImages\n");
			    XShmDetach(id->x.disp, &id->x.last_shminfo);
			    XDestroyImage(xim);
			    shmdt(id->x.last_shminfo.shmaddr);
			    shmctl(id->x.last_shminfo.shmid, IPC_RMID, 0);
			    shared_pixmap = 0;
			    shared_image = 1;
			    ok = 0;
			 }
		       if (ok)
			 {
			    id->x.last_sshminfo.shmid = shmget(IPC_PRIVATE, sxim->bytes_per_line * sxim->height, IPC_CREAT | 0777);
			    if (id->x.last_sshminfo.shmid == -1)
			      {
				 fprintf(stderr, "IMLIB ERROR: SHM can't get SHM Identifier for Shared Pixmap mask Wrapper\n");
				 fprintf(stderr, "             Falling back on Shared XImages\n");
				 XShmDetach(id->x.disp, &id->x.last_shminfo);
				 XDestroyImage(xim);
				 shmdt(id->x.last_shminfo.shmaddr);
				 shmctl(id->x.last_shminfo.shmid, IPC_RMID, 0);
				 XDestroyImage(sxim);
				 shared_pixmap = 0;
				 shared_image = 1;
				 ok = 0;
			      }
			    id->x.last_sshminfo.shmaddr = sxim->data = shmat(id->x.last_sshminfo.shmid, 0, 0);
			    id->x.last_sshminfo.readOnly = False;
			    XShmAttach(id->x.disp, &id->x.last_sshminfo);
			    stmp = (unsigned char *)sxim->data;
			    id->x.last_sxim = sxim;
			    mask = XShmCreatePixmap(id->x.disp, id->x.base_window,
						  id->x.last_sshminfo.shmaddr,
					       &id->x.last_sshminfo, w, h, 1);
			    stgc = XCreateGC(id->x.disp, mask, GCGraphicsExposures, &gcv);
			 }
		    }
	       }
	  }
     }
   ok = 1;
   if (shared_image)
     {
	xim = XShmCreateImage(id->x.disp, id->x.visual, id->x.depth, ZPixmap, NULL, &id->x.last_shminfo, w, h);
	if (!xim)
	  {
	     fprintf(stderr, "IMLIB ERROR: Mit-SHM can't create Shared XImage\n");
	     fprintf(stderr, "             Falling back on XImages\n");
	     shared_pixmap = 0;
	     shared_image = 0;
	     ok = 0;
	  }
	if (ok)
	  {
	     id->x.last_shminfo.shmid = shmget(IPC_PRIVATE, xim->bytes_per_line * xim->height, IPC_CREAT | 0777);
	     if (id->x.last_shminfo.shmid == -1)
	       {
		  fprintf(stderr, "IMLIB ERROR: SHM can't get SHM Identifier for Shared XImage\n");
		  fprintf(stderr, "             Falling back on XImages\n");
		  XDestroyImage(xim);
		  shared_pixmap = 0;
		  shared_image = 0;
		  ok = 0;
	       }
	     if (ok)
	       {
		  id->x.last_shminfo.shmaddr = xim->data = shmat(id->x.last_shminfo.shmid, 0, 0);
		  id->x.last_shminfo.readOnly = False;
		  XShmAttach(id->x.disp, &id->x.last_shminfo);
		  tmp = (unsigned char *)xim->data;
		  id->x.last_xim = xim;
		  pmap = XCreatePixmap(id->x.disp, id->x.base_window, w, h, id->x.depth);
		  tgc = XCreateGC(id->x.disp, pmap, GCGraphicsExposures, &gcv);
		  if ((im->shape_color.r >= 0) && (im->shape_color.g >= 0) && (im->shape_color.b >= 0))
		    {
		       sxim = XShmCreateImage(id->x.disp, id->x.visual, 1, ZPixmap, NULL, &id->x.last_sshminfo, w, h);
		       if (!sxim)
			 {
			    fprintf(stderr, "IMLIB ERROR: Mit-SHM can't create Shared XImage mask\n");
			    fprintf(stderr, "             Falling back on XImages\n");
			    XShmDetach(id->x.disp, &id->x.last_shminfo);
			    XDestroyImage(xim);
			    shmdt(id->x.last_shminfo.shmaddr);
			    shmctl(id->x.last_shminfo.shmid, IPC_RMID, 0);
			    shared_pixmap = 0;
			    shared_image = 0;
			    ok = 0;
			 }
		       if (ok)
			 {
			    id->x.last_sshminfo.shmid = shmget(IPC_PRIVATE, sxim->bytes_per_line * sxim->height, IPC_CREAT | 0777);
			    if (id->x.last_sshminfo.shmid == -1)
			      {
				 fprintf(stderr, "Imlib ERROR: SHM can't get SHM Identifierfor Shared XImage mask\n");
				 fprintf(stderr, "             Falling back on XImages\n");
				 XShmDetach(id->x.disp, &id->x.last_shminfo);
				 XDestroyImage(xim);
				 shmdt(id->x.last_shminfo.shmaddr);
				 shmctl(id->x.last_shminfo.shmid, IPC_RMID, 0);
				 XDestroyImage(sxim);
				 shared_pixmap = 0;
				 shared_image = 0;
				 ok = 0;
			      }
			    if (ok)
			      {
				 id->x.last_sshminfo.shmaddr = sxim->data = shmat(id->x.last_sshminfo.shmid, 0, 0);
				 id->x.last_sshminfo.readOnly = False;
				 XShmAttach(id->x.disp, &id->x.last_sshminfo);
				 stmp = (unsigned char *)sxim->data;
				 id->x.last_sxim = sxim;
				 mask = XCreatePixmap(id->x.disp, id->x.base_window, w, h, 1);
				 stgc = XCreateGC(id->x.disp, mask, GCGraphicsExposures, &gcv);
			      }
			 }
		    }
	       }
	  }
     }
   ok = 1;
   if ((!shared_pixmap) && (!shared_image))
     {
	tmp = (unsigned char *)malloc(w * h * bpp);
	if (!tmp)
	  {
	     fprintf(stderr, "IMLIB ERROR: Cannot allocate RAM for XImage data\n");
	     free(xarray);
	     free(yarray);
	     free(error);
	     return 0;
	  }
	xim = XCreateImage(id->x.disp, id->x.visual, id->x.depth, ZPixmap, 0, (char *)tmp, w, h, 8, 0);
	if (!xim)
	  {
	     fprintf(stderr, "IMLIB ERROR: Cannot allocate XImage buffer\n");
	     free(xarray);
	     free(yarray);
	     free(error);
	     free(tmp);
	     return 0;
	  }
	pmap = XCreatePixmap(id->x.disp, id->x.base_window, w, h, id->x.depth);
	if (!pmap)
	  {
	     fprintf(stderr, "IMLIB ERROR: Cannot create pixmap\n");
	     free(xarray);
	     free(yarray);
	     free(error);
	     XDestroyImage(xim);
	     return 0;
	  }
	tgc = XCreateGC(id->x.disp, pmap, GCGraphicsExposures, &gcv);
	if ((im->shape_color.r >= 0) && (im->shape_color.g >= 0) && (im->shape_color.b >= 0))
	  {
	     stmp = (unsigned char *)malloc(((w >> 3) + 8) * h);
	     if (!stmp)
	       {
		  fprintf(stderr, "IMLIB ERROR: Cannot allocate RAM for shape XImage data\n");
		  free(xarray);
		  free(yarray);
		  free(error);
		  XDestroyImage(xim);
		  return 0;
	       }
	     sxim = XCreateImage(id->x.disp, id->x.visual, 1, ZPixmap, 0, (char *)stmp, w, h, 8, 0);
	     if (!sxim)
	       {
		  fprintf(stderr, "IMLIB ERROR: Cannot allocate XImage shape buffer\n");
		  free(xarray);
		  free(yarray);
		  free(error);
		  free(stmp);
		  XDestroyImage(xim);
		  return 0;
	       }
	     mask = XCreatePixmap(id->x.disp, id->x.base_window, w, h, 1);
	     if (!mask)
	       {
		  fprintf(stderr, "IMLIB ERROR: Cannot create shape pixmap\n");
		  free(xarray);
		  free(yarray);
		  free(error);
		  XDestroyImage(sxim);
		  XDestroyImage(xim);
		  return 0;
	       }
	     stgc = XCreateGC(id->x.disp, mask, GCGraphicsExposures, &gcv);
	  }
     }

/* copy XImage to the pixmap, if not a shared pixmap */
   if ((im->shape_color.r >= 0) && (im->shape_color.g >= 0) && (im->shape_color.b >= 0))
     {
	if ((im->mod.gamma == 256) && (im->mod.brightness == 256) && (im->mod.contrast == 256) &&
	    (im->rmod.gamma == 256) && (im->rmod.brightness == 256) && (im->rmod.contrast == 256) &&
	    (im->gmod.gamma == 256) && (im->gmod.brightness == 256) && (im->gmod.contrast == 256) &&
	    (im->bmod.gamma == 256) && (im->bmod.brightness == 256) && (im->bmod.contrast == 256))
	  {
	     if (id->x.depth <= 8)
		grender_shaped(im, w, h, xim, sxim, er1, er2, xarray, yarray, 8);
	     else
		grender_shaped(im, w, h, xim, sxim, er1, er2, xarray, yarray, id->x.render_depth);
	  }
	else
	  {
	     if (id->x.depth <= 8)
		grender_shaped_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray, 8);
	     else
		grender_shaped_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray, id->x.render_depth);
	  }
	if (shared_image)
	  {
	     XShmPutImage(id->x.disp, pmap, tgc, xim, 0, 0, 0, 0, w, h, False);
	     XShmPutImage(id->x.disp, mask, stgc, sxim, 0, 0, 0, 0, w, h, False);
	     XSync(id->x.disp, False);
	     im->pixmap = gdk_imlib_pixmap_foreign_new(w, h, id->x.depth, pmap);
	     im->shape_mask = gdk_imlib_pixmap_foreign_new(w, h, 1, mask);
	     XShmDetach(id->x.disp, &id->x.last_shminfo);
	     XDestroyImage(xim);
	     shmdt(id->x.last_shminfo.shmaddr);
	     shmctl(id->x.last_shminfo.shmid, IPC_RMID, 0);
	     XShmDetach(id->x.disp, &id->x.last_sshminfo);
	     XDestroyImage(sxim);
	     shmdt(id->x.last_sshminfo.shmaddr);
	     shmctl(id->x.last_sshminfo.shmid, IPC_RMID, 0);
	     id->x.last_xim = NULL;
	     id->x.last_sxim = NULL;
	     xim = NULL;
	     sxim = NULL;
	     XFreeGC(id->x.disp, tgc);
	     XFreeGC(id->x.disp, stgc);
	  }
	else if (shared_pixmap)
	  {
	     Pixmap              p2, m2;

	     p2 = XCreatePixmap(id->x.disp, id->x.base_window, w, h, id->x.depth);
	     m2 = XCreatePixmap(id->x.disp, id->x.base_window, w, h, 1);
	     XCopyArea(id->x.disp, pmap, p2, tgc, 0, 0, w, h, 0, 0);
	     XCopyArea(id->x.disp, mask, m2, stgc, 0, 0, w, h, 0, 0);
	     im->pixmap = gdk_imlib_pixmap_foreign_new(w, h, id->x.depth, p2);
	     im->shape_mask = gdk_imlib_pixmap_foreign_new(w, h, 1, m2);
	     XFreeGC(id->x.disp, tgc);
	     XFreeGC(id->x.disp, stgc);
	     XFreePixmap(id->x.disp, pmap);
	     XFreePixmap(id->x.disp, mask);
	     XSync(id->x.disp, False);
	     XShmDetach(id->x.disp, &id->x.last_shminfo);
	     XDestroyImage(xim);
	     shmdt(id->x.last_shminfo.shmaddr);
	     shmctl(id->x.last_shminfo.shmid, IPC_RMID, 0);
	     XShmDetach(id->x.disp, &id->x.last_sshminfo);
	     XDestroyImage(sxim);
	     shmdt(id->x.last_sshminfo.shmaddr);
	     shmctl(id->x.last_sshminfo.shmid, IPC_RMID, 0);
	     id->x.last_xim = NULL;
	     id->x.last_sxim = NULL;
	     xim = NULL;
	     sxim = NULL;
	  }
	else
	  {
	     XPutImage(id->x.disp, pmap, tgc, xim, 0, 0, 0, 0, w, h);
	     XPutImage(id->x.disp, mask, stgc, sxim, 0, 0, 0, 0, w, h);
	     im->pixmap = gdk_imlib_pixmap_foreign_new(w, h, id->x.depth, pmap);
	     im->shape_mask = gdk_imlib_pixmap_foreign_new(w, h, 1, mask);
	     XDestroyImage(xim);
	     XDestroyImage(sxim);
	     xim = NULL;
	     sxim = NULL;
	     XFreeGC(id->x.disp, tgc);
	     XFreeGC(id->x.disp, stgc);
	  }
     }
   else
     {
	if ((im->mod.gamma == 256) && (im->mod.brightness == 256) && (im->mod.contrast == 256) &&
	    (im->rmod.gamma == 256) && (im->rmod.brightness == 256) && (im->rmod.contrast == 256) &&
	    (im->gmod.gamma == 256) && (im->gmod.brightness == 256) && (im->gmod.contrast == 256) &&
	    (im->bmod.gamma == 256) && (im->bmod.brightness == 256) && (im->bmod.contrast == 256))
	  {
	     if (id->x.depth <= 8)
		grender(im, w, h, xim, sxim, er1, er2, xarray, yarray, 8);
	     else
		grender(im, w, h, xim, sxim, er1, er2, xarray, yarray, id->x.render_depth);
	  }
	else
	  {
	     if (id->x.depth <= 8)
		grender_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray, 8);
	     else
		grender_mod(im, w, h, xim, sxim, er1, er2, xarray, yarray, id->x.render_depth);
	  }
	if (shared_image)
	  {
	     XShmPutImage(id->x.disp, pmap, tgc, xim, 0, 0, 0, 0, w, h, False);
	     im->pixmap = gdk_imlib_pixmap_foreign_new(w, h, id->x.depth, pmap);
	     im->shape_mask = NULL;
	     XSync(id->x.disp, False);
	     XShmDetach(id->x.disp, &id->x.last_shminfo);
	     XDestroyImage(xim);
	     shmdt(id->x.last_shminfo.shmaddr);
	     shmctl(id->x.last_shminfo.shmid, IPC_RMID, 0);
	     id->x.last_xim = NULL;
	     xim = NULL;
	     sxim = NULL;
	     XFreeGC(id->x.disp, tgc);
	  }
	else if (shared_pixmap)
	  {
	     Pixmap              p2;

	     p2 = XCreatePixmap(id->x.disp, id->x.base_window, w, h, id->x.depth);
	     XCopyArea(id->x.disp, pmap, p2, tgc, 0, 0, w, h, 0, 0);
	     im->pixmap = gdk_imlib_pixmap_foreign_new(w, h, id->x.depth, p2);
	     im->shape_mask = NULL;
	     XFreeGC(id->x.disp, tgc);
	     XFreePixmap(id->x.disp, pmap);
	     XSync(id->x.disp, False);
	     XShmDetach(id->x.disp, &id->x.last_shminfo);
	     XDestroyImage(xim);
	     shmdt(id->x.last_shminfo.shmaddr);
	     shmctl(id->x.last_shminfo.shmid, IPC_RMID, 0);
	     id->x.last_xim = NULL;
	     xim = NULL;
	     sxim = NULL;
	  }
	else
	  {
	     XPutImage(id->x.disp, pmap, tgc, xim, 0, 0, 0, 0, w, h);
	     im->pixmap = gdk_imlib_pixmap_foreign_new(w, h, id->x.depth, pmap);
	     im->shape_mask = NULL;
	     XDestroyImage(xim);
	     xim = NULL;
	     sxim = NULL;
	     XFreeGC(id->x.disp, tgc);
	  }
     }

/* cleanup */
   XSync(id->x.disp, False);
   free(error);
   free(xarray);
   free(yarray);

/* add this pixmap to the cache */
   gadd_pixmap(im, w, h, xim, sxim);
   return 1;
}
