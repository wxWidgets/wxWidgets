
#define _GNU_SOURCE
#include "../gdk_imlib/config.h"

#include "../gdk_imlib/gdk_imlib.h"
#include "../gdk_imlib/gdk_imlib_private.h"

/*      Split the ID - damages input    */

static char        *
g_SplitID(char *file)
{
   char               *p = strrchr(file, ':');

   if (p == NULL)
      return "";
   else
     {
	*p++ = 0;
	return p;
     }
}

/*
 *     Doesn't damage the input
 */

char               *
g_GetExtension(char *file)
{
   char               *p = strrchr(file, '.');

   if (p == NULL)
      return "";
   else
      return p + 1;
}

#ifdef HAVE_LIBJPEG
unsigned char      *
g_LoadJPEG(FILE * f, int *w, int *h)
{
   struct jpeg_decompress_struct cinfo;
   struct jpeg_error_mgr jerr;
   unsigned char      *data, *line[16], *ptr;
   int                 x, y, i;

   cinfo.err = jpeg_std_error(&jerr);
   jpeg_create_decompress(&cinfo);
   jpeg_stdio_src(&cinfo, f);
   jpeg_read_header(&cinfo, TRUE);
   cinfo.do_fancy_upsampling = FALSE;
   cinfo.do_block_smoothing = FALSE;
   jpeg_start_decompress(&cinfo);
   *w = cinfo.output_width;
   *h = cinfo.output_height;
   data = malloc(*w ** h * 3);
   if (!data)
     {
	jpeg_destroy_decompress(&cinfo);
	return NULL;
     }
   ptr = data;

   if (cinfo.rec_outbuf_height > 16)
     {
	fprintf(stderr, "gdk_imlib ERROR: JPEG uses line buffers > 16. Cannot load.\n");
	return NULL;
     }
   if (cinfo.output_components == 3)
     {
	for (y = 0; y < *h; y += cinfo.rec_outbuf_height)
	  {
	     for (i = 0; i < cinfo.rec_outbuf_height; i++)
	       {
		  line[i] = ptr;
		  ptr += *w * 3;
	       }
	     jpeg_read_scanlines(&cinfo, line, cinfo.rec_outbuf_height);
	  }
     }
   else if (cinfo.output_components == 1)
     {
	for (i = 0; i < cinfo.rec_outbuf_height; i++)
	  {
	     if ((line[i] = malloc(*w)) == NULL)
	       {
		  int                 t = 0;

		  for (t = 0; t < i; t++)
		     free(line[t]);
		  jpeg_destroy_decompress(&cinfo);
		  return NULL;
	       }
	  }
	for (y = 0; y < *h; y += cinfo.rec_outbuf_height)
	  {
	     jpeg_read_scanlines(&cinfo, line, cinfo.rec_outbuf_height);
	     for (i = 0; i < cinfo.rec_outbuf_height; i++)
	       {
		  for (x = 0; x < *w; x++)
		    {
		       *ptr++ = line[i][x];
		       *ptr++ = line[i][x];
		       *ptr++ = line[i][x];
		    }
	       }
	  }
	for (i = 0; i < cinfo.rec_outbuf_height; i++)
	   free(line[i]);
     }
   jpeg_finish_decompress(&cinfo);
   jpeg_destroy_decompress(&cinfo);
   return data;
}
#endif /* HAVE_LIBJPEG */

#ifdef HAVE_LIBPNG
unsigned char      *
g_LoadPNG(FILE * f, int *w, int *h, int *t)
{
   png_structp         png_ptr;
   png_infop           info_ptr;
   unsigned char      *data, *ptr, **lines, *ptr2, r, g, b, a;
   int                 i, x, y, transp, bit_depth, color_type, interlace_type;
   png_uint_32        *ww, *hh;

   /* Init PNG Reader */
   transp = 0;
   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (!png_ptr)
      return NULL;

   info_ptr = png_create_info_struct(png_ptr);
   if (!info_ptr)
     {
	png_destroy_read_struct(&png_ptr, NULL, NULL);
	return NULL;
     }

   if (setjmp(png_ptr->jmpbuf))
     {
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	return NULL;
     }

   if (info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
     {
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	return NULL;
     }
   png_init_io(png_ptr, f);
   /* Read Header */
   png_read_info(png_ptr, info_ptr);
   ww = (png_uint_32 *) w;
   hh = (png_uint_32 *) h;
   png_get_IHDR(png_ptr, info_ptr, ww, hh, &bit_depth, &color_type, &interlace_type,
		NULL, NULL);
   /* Setup Translators */
   if (color_type == PNG_COLOR_TYPE_PALETTE)
      png_set_expand(png_ptr);
   png_set_strip_16(png_ptr);
   png_set_packing(png_ptr);
   if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
      png_set_expand(png_ptr);
   png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
   *w = info_ptr->width;
   *h = info_ptr->height;
   data = malloc(*w ** h * 3);
   if (!data)
     {
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	return NULL;
     }
   lines = (unsigned char **)malloc(*h * sizeof(unsigned char *));

   if (lines == NULL)
     {
	free(data);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	return NULL;
     }
   for (i = 0; i < *h; i++)
     {
	if ((lines[i] = malloc(*w * (sizeof(unsigned char) * 4))) == NULL)
	  {
	     int                 n;

	     free(data);
	     for (n = 0; n < i; n++)
		free(lines[n]);
	     free(lines);
	     png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	     return NULL;
	  }
     }
   png_read_image(png_ptr, lines);
   png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
   ptr = data;
   if (color_type == PNG_COLOR_TYPE_GRAY)
     {
	for (y = 0; y < *h; y++)
	  {
	     ptr2 = lines[y];
	     for (x = 0; x < *w; x++)
	       {
		  r = *ptr2++;
		  *ptr++ = r;
		  *ptr++ = r;
		  *ptr++ = r;
	       }
	  }
     }
   else
     {
	for (y = 0; y < *h; y++)
	  {
	     ptr2 = lines[y];
	     for (x = 0; x < *w; x++)
	       {
		  r = *ptr2++;
		  g = *ptr2++;
		  b = *ptr2++;
		  a = *ptr2++;
		  if (a < 128)
		    {
		       *ptr++ = 255;
		       *ptr++ = 0;
		       *ptr++ = 255;
		       transp = 1;
		    }
		  else
		    {
		       if ((r == 255) && (g == 0) && (b == 255))
			  r = 254;
		       *ptr++ = r;
		       *ptr++ = g;
		       *ptr++ = b;
		    }
	       }
	  }
     }
   for (i = 0; i < *h; i++)
      free(lines[i]);
   free(lines);
   *t = transp;
   return data;
}
#endif /* HAVE_LIBPNG */

#ifdef HAVE_LIBTIFF
unsigned char      *
g_LoadTIFF(char *f, int *w, int *h, int *t)
{
   TIFF               *tif;
   unsigned char      *data, *ptr, r, g, b, a;
   int                 x, y;
   uint32              ww, hh, *rast, *tptr;
   size_t              npix;
   int                 istransp;

   istransp = 0;
   if (!f)
      return NULL;

   tif = TIFFOpen(f, "r");
   if (!tif)
      return NULL;

   TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &ww);
   TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &hh);
   npix = ww * hh;
   *w = (int)ww;
   *h = (int)hh;
   rast = (uint32 *) _TIFFmalloc(npix * sizeof(uint32));
   if (!rast)
     {
	TIFFClose(tif);
	return NULL;
     }
   data = NULL;
   if (TIFFReadRGBAImage(tif, ww, hh, rast, 0))
     {
	data = (unsigned char *)malloc(*w ** h * 3);
	if (!data)
	  {
	     _TIFFfree(rast);
	     TIFFClose(tif);
	     return NULL;
	  }
	ptr = data;
	for (y = 0; y < *h; y++)
	  {
	     tptr = rast;
	     tptr += ((*h - y - 1) ** w);
	     for (x = 0; x < *w; x++)
	       {
		  a = TIFFGetA(*tptr);
		  b = TIFFGetB(*tptr);
		  g = TIFFGetG(*tptr);
		  r = TIFFGetR(*tptr);
		  tptr++;
		  if (a < 128)
		    {
		       *ptr++ = 255;
		       *ptr++ = 0;
		       *ptr++ = 255;
		       istransp = 1;
		    }
		  else
		    {
		       if ((r == 255) && (g == 0) && (b == 255))
			  r = 254;
		       *ptr++ = r;
		       *ptr++ = g;
		       *ptr++ = b;
		    }
	       }
	  }
     }
   _TIFFfree(rast);
   TIFFClose(tif);
   *t = istransp;
   return data;
}

#endif /* HAVE_LIBTIFF */

#ifdef HAVE_LIBGIF
unsigned char      *
g_LoadGIF(char *f, int *w, int *h, int *t)
{
   unsigned char      *data, *ptr;
   GifFileType        *gif;
   GifRowType         *rows;
   GifRecordType       rec;
   ColorMapObject     *cmap;
   int                 i, j, done, bg, csize, r, g, b;
   int                 intoffset[] =
   {0, 4, 2, 1};
   int                 intjump[] =
   {8, 8, 4, 2};
   int                 istransp, transp;

   done = 0;
   istransp = 0;
   gif = DGifOpenFileName(f);
   if (!gif)
      return NULL;

   do
     {
	DGifGetRecordType(gif, &rec);
	if ((rec == IMAGE_DESC_RECORD_TYPE) && (!done))
	  {
	     DGifGetImageDesc(gif);
	     *w = gif->Image.Width;
	     *h = gif->Image.Height;
	     rows = malloc(*h * sizeof(GifRowType *));
	     if (!rows)
	       {
		  DGifCloseFile(gif);
		  return NULL;
	       }
	     data = malloc(*w ** h * 3);
	     if (!data)
	       {
		  DGifCloseFile(gif);
		  free(rows);
		  return NULL;
	       }
	     for (i = 0; i < *h; i++)
		rows[i] = NULL;
	     for (i = 0; i < *h; i++)
	       {
		  rows[i] = malloc(*w * sizeof(GifPixelType));
		  if (!rows[i])
		    {
		       DGifCloseFile(gif);
		       for (i = 0; i < *h; i++)
			  if (rows[i])
			     free(rows[i]);
		       free(rows);
		       free(data);
		       return NULL;
		    }
	       }
	     if (gif->Image.Interlace)
	       {
		  for (i = 0; i < 4; i++)
		    {
		       for (j = intoffset[i]; j < *h; j += intjump[i])
			  DGifGetLine(gif, rows[j], *w);
		    }
	       }
	     else
	       {
		  for (i = 0; i < *h; i++)
		     DGifGetLine(gif, rows[i], *w);
	       }
	     done = 1;
	  }
	else if (rec == EXTENSION_RECORD_TYPE)
	  {
	     int                 ext_code;
	     GifByteType        *ext;

	     DGifGetExtension(gif, &ext_code, &ext);
	     if (ext)
	       {
		  if ((ext[1] & 1))
		    {
		       istransp = 1;
		       transp = (int)ext[4];
		    }
	       }
	     do
	       {
		  DGifGetExtensionNext(gif, &ext);
	       }
	     while (ext);
	  }
     }
   while (rec != TERMINATE_RECORD_TYPE);
   bg = gif->SBackGroundColor;
   cmap = (gif->Image.ColorMap ? gif->Image.ColorMap : gif->SColorMap);
   csize = cmap->ColorCount;
   ptr = data;
   if (!istransp)
     {
	for (i = 0; i < *h; i++)
	  {
	     for (j = 0; j < *w; j++)
	       {
		  r = cmap->Colors[rows[i][j]].Red;
		  g = cmap->Colors[rows[i][j]].Green;
		  b = cmap->Colors[rows[i][j]].Blue;
		  *ptr++ = r;
		  *ptr++ = g;
		  *ptr++ = b;
	       }
	  }
     }
   else
     {
	for (i = 0; i < *h; i++)
	  {
	     for (j = 0; j < *w; j++)
	       {
		  if (rows[i][j] == transp)
		    {
		       *ptr++ = 255;
		       *ptr++ = 0;
		       *ptr++ = 255;
		    }
		  else
		    {
		       r = cmap->Colors[rows[i][j]].Red;
		       g = cmap->Colors[rows[i][j]].Green;
		       b = cmap->Colors[rows[i][j]].Blue;
		       if (r == 255 && g == 0 && b == 255)
			  r = 254;
		       *ptr++ = r;
		       *ptr++ = g;
		       *ptr++ = b;
		    }
	       }
	  }
     }
   DGifCloseFile(gif);
   for (i = 0; i < *h; i++)
      free(rows[i]);
   free(rows);
   *t = istransp;
   return data;
}

#endif /* HAVE_LIBGIF */

unsigned char      *
g_LoadXPM(char *f, int *w, int *h, int *t)
{
   FILE               *file;
   unsigned char      *data, *ptr;
   int                 pc, c, i, j, k, ncolors, cpp, comment, transp, quote,
                       context, len, token, done;
   char                line[65536], s[65536], tok[65536], col[65536];
   XColor              xcol;
   struct _cmap
     {
	char                str[8];
	char                transp;
	int                 r, g, b;
     }
                      *cmap;
   int                 lookup[128][128];

   transp = 0;
   done = 0;

   file = fopen(f, "r");
   if (!file)
      return NULL;

   *w = 10;
   *h = 10;

   ptr = NULL;
   data = NULL;
   c = ' ';
   comment = 0;
   quote = 0;
   context = 0;

   while (!done)
     {
	pc = c;
	c = fgetc(file);
	if (c == EOF)
	   break;
	if (!quote)
	  {
	     if (pc == '/' && c == '*')
		comment = 1;
	     else if (pc == '*' && c == '/' && comment)
		comment = 0;
	  }
	if (!comment)
	  {
	     if (!quote && c == '"')
	       {
		  quote = 1;
		  i = 0;
	       }
	     else if (quote && c == '"')
	       {
		  line[i] = 0;
		  quote = 0;
		  if (context == 0)
		    {
		       /* Header */
		       sscanf(line, "%i %i %i %i", w, h, &ncolors, &cpp);
		       if (cpp > 7)
			 {
			    fprintf(stderr, "gdk_imlib ERROR: XPM files with characters per pixel > 7 not supported\n");
			    return NULL;
			 }
		       if (*w > 32767)
			 {
			    fprintf(stderr, "gdk_imlib ERROR: Image width > 32767 pixels for file\n");
			    return NULL;
			 }
		       if (*h > 32767)
			 {
			    fprintf(stderr, "gdk_imlib ERROR: Image height > 32767 pixels for file\n");
			    return NULL;
			 }
		       cmap = malloc(sizeof(struct _cmap) * ncolors);

		       if (!cmap)
			  return NULL;
		       data = malloc(*w ** h * 3);
		       if (!data)
			 {
			    free(cmap);
			    return NULL;
			 }
		       ptr = data;
		       j = 0;
		       context++;
		    }
		  else if (context == 1)
		    {
		       /* Color Table */
		       if (j < ncolors)
			 {
			    int                 colptr = 0;
			    int                 slen;

			    tok[0] = 0;
			    col[0] = 0;
			    s[0] = 0;
			    len = strlen(line);
			    strncpy(cmap[j].str, line, cpp);
			    cmap[j].str[cpp] = 0;
			    cmap[j].r = -1;
			    cmap[j].transp = 0;
			    for (k = cpp; k < len; k++)
			      {
				 if (line[k] != ' ')
				   {
				      s[0] = 0;
				      sscanf(&line[k], "%65535s", s);
				      slen = strlen(s);
				      k += slen;
				      if ((!strcmp(s, "m")) || (!strcmp(s, "s")) ||
					  (!strcmp(s, "g4")) || (!strcmp(s, "g")) ||
					  (!strcmp(s, "c")) || (k >= len))
					{
					   if (k >= len)
					     {
						if (col[0])
						   strcat(col, " ");
						strcat(col, s);
					     }
					   if (col[0])
					     {
						if (!strcasecmp(col, "none"))
						  {
						     transp = 1;
						     cmap[j].transp = 1;
						  }
						else
						  {
						     if ((cmap[j].r < 0) ||
							 (!strcmp(tok, "c")))
						       {
							  XParseColor(id->x.disp,
							      id->x.root_cmap,
								  col, &xcol);
							  cmap[j].r = xcol.red >> 8;
							  cmap[j].g = xcol.green >> 8;
							  cmap[j].b = xcol.blue >> 8;
							  if ((cmap[j].r == 255) &&
							   (cmap[j].g == 0) &&
							   (cmap[j].b == 255))
							     cmap[j].r = 254;
						       }
						  }
					     }
					   strcpy(tok, s);
					   col[0] = 0;
					}
				      else
					{
					   if (col[0])
					      strcat(col, " ");
					   strcat(col, s);
					}
				   }
			      }
			 }
		       j++;
		       if (j >= ncolors)
			 {
			    if (cpp == 1)
			       for (i = 0; i < ncolors; i++)
				  lookup[cmap[i].str[0]][cmap[i].str[1]] = i;
			    if (cpp == 2)
			       for (i = 0; i < ncolors; i++)
				  lookup[cmap[i].str[0]][cmap[i].str[1]] = i;
			    context++;
			 }
		    }
		  else
		    {
		       /* Image Data */
		       i = 0;
		       if (cpp == 0)
			 {
			    /* Chars per pixel = 0? well u never know */
			 }
		       if (cpp == 1)
			 {
			    if (transp)
			      {
				 for (i = 0; ((i < 65536) && (line[i])); i++)
				   {
				      col[0] = line[i];
				      if (cmap[lookup[col[0]][0]].transp)
					{
					   *ptr++ = 255;
					   *ptr++ = 0;
					   *ptr++ = 255;
					}
				      else
					{
					   *ptr++ = (unsigned char)cmap[lookup[col[0]][0]].r;
					   *ptr++ = (unsigned char)cmap[lookup[col[0]][0]].g;
					   *ptr++ = (unsigned char)cmap[lookup[col[0]][0]].b;
					}
				   }
			      }
			    else
			      {
				 for (i = 0; ((i < 65536) && (line[i])); i++)
				   {
				      col[0] = line[i];
				      *ptr++ = (unsigned char)cmap[lookup[col[0]][0]].r;
				      *ptr++ = (unsigned char)cmap[lookup[col[0]][0]].g;
				      *ptr++ = (unsigned char)cmap[lookup[col[0]][0]].b;
				   }
			      }
			 }
		       else if (cpp == 2)
			 {
			    if (transp)
			      {
				 for (i = 0; ((i < 65536) && (line[i])); i++)
				   {
				      col[0] = line[i++];
				      col[1] = line[i];
				      if (cmap[lookup[col[0]][col[1]]].transp)
					{
					   *ptr++ = 255;
					   *ptr++ = 0;
					   *ptr++ = 255;
					}
				      else
					{
					   *ptr++ = (unsigned char)cmap[lookup[col[0]][col[1]]].r;
					   *ptr++ = (unsigned char)cmap[lookup[col[0]][col[1]]].g;
					   *ptr++ = (unsigned char)cmap[lookup[col[0]][col[1]]].b;
					}
				   }
			      }
			    else
			      {
				 for (i = 0; ((i < 65536) && (line[i])); i++)
				   {
				      col[0] = line[i++];
				      col[1] = line[i];
				      *ptr++ = (unsigned char)cmap[lookup[col[0]][col[1]]].r;
				      *ptr++ = (unsigned char)cmap[lookup[col[0]][col[1]]].g;
				      *ptr++ = (unsigned char)cmap[lookup[col[0]][col[1]]].b;
				   }
			      }
			 }
		       else
			 {
			    if (transp)
			      {
				 for (i = 0; ((i < 65536) && (line[i])); i++)
				   {
				      for (j = 0; j < cpp; j++, i++)
					{
					   col[j] = line[i];
					}
				      col[j] = 0;
				      i--;
				      for (j = 0; j < ncolors; j++)
					{
					   if (!strcmp(col, cmap[j].str))
					     {
						if (cmap[j].transp)
						  {
						     *ptr++ = 255;
						     *ptr++ = 0;
						     *ptr++ = 255;
						  }
						else
						  {
						     *ptr++ = (unsigned char)cmap[j].r;
						     *ptr++ = (unsigned char)cmap[j].g;
						     *ptr++ = (unsigned char)cmap[j].b;
						  }
						j = ncolors;
					     }
					}
				   }
			      }
			    else
			      {
				 for (i = 0; ((i < 65536) && (line[i])); i++)
				   {
				      for (j = 0; j < cpp; j++, i++)
					{
					   col[j] = line[i];
					}
				      col[j] = 0;
				      i--;
				      for (j = 0; j < ncolors; j++)
					{
					   if (!strcmp(col, cmap[j].str))
					     {
						*ptr++ = (unsigned char)cmap[j].r;
						*ptr++ = (unsigned char)cmap[j].g;
						*ptr++ = (unsigned char)cmap[j].b;
						j = ncolors;
					     }
					}
				   }
			      }
			 }
		    }
	       }
	  }
	/* Scan in line from XPM file (limit line length 65k) */
	if (i < 65536)
	  {
	     if ((!comment) && (quote) && (c != '"'))
	       {
		  line[i++] = c;
	       }
	  }
	if ((ptr) && ((ptr - data) >= *w ** h * 3))
	   done = 1;
     }
   fclose(file);
   if (transp)
      *t = 1;
   else
      *t = 0;
   free(cmap);
   return data;
}

unsigned char      *
g_LoadPPM(FILE * f, int *w, int *h)
{
   int                 done;
   unsigned char      *ptr;
   unsigned char       chr;
   char                s[256];
   int                 a, b;
   int                 color, scale;

   a = b = scale = 0;
   fgets(s, 256, f);
   s[strlen(s) - 1] = 0;

   if (!strcmp(s, "P6"))
      color = 1;
   else if (!strcmp(s, "P5"))
      color = 0;
   else
      return NULL;

   done = 1;
   ptr = NULL;
   while (done)
     {
	if (fgets(s, 256, f) == NULL)
	   break;

	s[strlen(s) - 1] = 0;
	if (s[0] != '#')
	  {
	     done = 0;
	     sscanf(s, "%i %i", w, h);
	     a = *w;
	     b = *h;
	     if (a > 32767)
	       {
		  fprintf(stderr, "gdk_imlib ERROR: Image width > 32767 pixels for file\n");
		  return NULL;
	       }
	     if (b > 32767)
	       {
		  fprintf(stderr, "gdk_imlib ERROR: Image height > 32767 pixels for file\n");
		  return NULL;
	       }
	     fgets(s, 256, f);
	     sscanf(s, "%i", &scale);
	     s[strlen(s) - 1] = 0;
	     ptr = (unsigned char *)malloc(a * b * 3);
	     if (!ptr)
	       {
		  fprintf(stderr, "gdk_imlib ERROR: Cannot allocate RAM for RGB data in file");
		  return ptr;
	       }
	     if (color)
	       {
		  if (!fread(ptr, a * b * 3, 1, f))
		    {
		       free(ptr);
		       return NULL;
		    }
	       }
	     else
	       {
		  b = (a * b * 3);
		  a = 0;
		  while ((fread(&chr, 1, 1, f)) && (a < b))
		    {
		       ptr[a++] = chr;
		       ptr[a++] = chr;
		       ptr[a++] = chr;
		    }
	       }
	  }
     }
   if (scale == 0)
     {
	free(ptr);
	return NULL;
     }
   if (scale < 255)
     {
	int                 rot;
	unsigned char      *po;

	if (scale <= 1)
	   rot = 7;
	else if (scale <= 3)
	   rot = 6;
	else if (scale <= 7)
	   rot = 5;
	else if (scale <= 15)
	   rot = 4;
	else if (scale <= 31)
	   rot = 3;
	else if (scale <= 63)
	   rot = 2;
	else
	   rot = 1;

	if (rot > 0)
	  {
	     po = ptr;
	     while (po < (ptr + (*w ** h * 3)))
	       {
		  *po++ <<= rot;
		  *po++ <<= rot;
		  *po++ <<= rot;
	       }
	  }
     }
   return ptr;
}

static int
gispnm(char *file)
{
   FILE               *f;
   char                buf[8];

   f = fopen(file, "rb");
   if (!f)
      return 0;
   fgets(buf, 8, f);
   fclose(f);
   if (!strcmp("P6\n", buf))
      return 1;
   if (!strcmp("P5\n", buf))
      return 1;
   return 0;
}

static int
gisjpeg(char *file)
{
   FILE               *f;
   unsigned char       buf[8];

   f = fopen(file, "rb");
   if (!f)
      return 0;
   fread(buf, 1, 2, f);
   fclose(f);
   if ((buf[0] == 0xff) && (buf[1] == 0xd8))
      return 1;
   return 0;
}

static int
gispng(char *file)
{
#ifdef HAVE_LIBPNG
   FILE               *f;
   unsigned char       buf[8];

   f = fopen(file, "rb");
   if (!f)
      return 0;
   fread(buf, 1, 8, f);
   fclose(f);
   return (int)png_check_sig(buf, 8);
#else
   return 0;
#endif
}

static int
gistiff(char *file)
{
   FILE               *f;
   char                buf[8];

   f = fopen(file, "rb");
   if (!f)
      return 0;
   fgets(buf, 5, f);
   fclose(f);
   if ((buf[0] == 'M') && (buf[1] == 'M') && (buf[2] == 0x00) && (buf[3] == 0x2a))
      return 1;
   if ((buf[0] == 'I') && (buf[1] == 'I') && (buf[2] == 0x2a) && (buf[3] == 0x00))
      return 1;
   return 0;
}

static int
giseim(char *file)
{
   FILE               *f;
   char                buf[8];

   f = fopen(file, "rb");
   if (!f)
      return 0;
   fread(buf, 1, 4, f);
   fclose(f);
   if (!strncmp("EIM ", buf, 4))
      return 1;
   return 0;
}

static int
gisgif(char *file)
{
   FILE               *f;
   char                buf[8];

   f = fopen(file, "rb");
   if (!f)
      return 0;
   fread(buf, 1, 4, f);
   fclose(f);
   buf[4] = 0;
   if (!strcmp("GIF8", buf))
      return 1;
   return 0;
}

static int
gisxpm(char *file)
{
   FILE               *f;
   char                buf[11];

   f = fopen(file, "rb");
   if (!f)
      return 0;
   fread(buf, 1, 9, f);
   fclose(f);
   buf[9] = 0;
   if (!strcmp("/* XPM */", buf))
      return 1;
   return 0;
}

GdkImlibImage      *
gdk_imlib_load_image(char *file)
{
   int                 w, h;
   int                 needs_conv = 1;
   unsigned char      *data;
   GdkImlibImage      *im;
   char                s[4096];
   char                fil[4096];
   char               *iden;
   char               *e;
   char                cmd[4096];
   FILE               *p;
   int                 eim;
   int                 fmt;
   int                 trans;

   eim = 0;
   fmt = 0;
   p = NULL;
   data = NULL;

   if (!file)
      return NULL;
   if (id->cache.on_image)
      if ((im = gfind_image(file)))
	 return im;

   strncpy(fil, file, sizeof(fil));
   iden = g_SplitID(fil);
   e = g_GetExtension(fil);

   if (gispnm(fil))
     {
	needs_conv = 0;
	fmt = 0;
     }
   else if (gisjpeg(fil))
     {
#ifdef HAVE_LIBJPEG
	needs_conv = 0;
	fmt = 2;
#else
	needs_conv = 1;
	fmt = 0;
#endif
     }
   else if (gistiff(fil))
     {
#ifdef HAVE_LIBTIFF
	needs_conv = 0;
	fmt = 3;
#else
	needs_conv = 1;
	fmt = 0;
#endif
     }
   else if (giseim(fil))
     {
	needs_conv = 0;
	eim = 1;
	fmt = 9999;
     }
   else if (gisxpm(fil))
     {
	needs_conv = 0;
	fmt = 5;
     }
   else if (gispng(fil))
     {
#ifdef HAVE_LIBPNG
	needs_conv = 0;
	fmt = 1;
#else
	needs_conv = 1;
	fmt = 0;
#endif
     }
   else if (gisgif(fil))
     {
#ifdef HAVE_LIBGIF
	needs_conv = 0;
	fmt = 4;
#else
	needs_conv = 1;
	fmt = 0;
#endif
     }
   if (needs_conv && id->fallback)
     {
	p = open_helper("%C/convert %s pnm:-", fil, "rb");
     }
   else if ((fmt == 2) || (fmt == 1) || (fmt == 0))
      p = fopen(fil, "rb");

   trans = 0;
   if (!eim && !data)
     {
	switch (fmt)
	  {
	  case 5:
	     data = g_LoadXPM(fil, &w, &h, &trans);
	     break;
#ifdef HAVE_LIBGIF
	  case 4:
	     data = g_LoadGIF(fil, &w, &h, &trans);
	     break;
#endif
#ifdef HAVE_LIBTIFF
	  case 3:
	     data = g_LoadTIFF(fil, &w, &h, &trans);
	     break;
#endif
#ifdef HAVE_LIBJPEG
	  case 2:
	     if (p)
		data = g_LoadJPEG(p, &w, &h);
	     break;
#endif
#ifdef HAVE_LIBPNG
	  case 1:
	     if (p)
		data = g_LoadPNG(p, &w, &h, &trans);
	     break;
#endif
	  default:
	     if (p)
		data = g_LoadPPM(p, &w, &h);
	     break;
	  }
     }

   if (p && !needs_conv)
      fclose(p);
   else if (p)
      close_helper(p);

   if ((!data) && (id->fallback))
     {
	p = open_helper("%C/convert %s pnm:-", fil, "rb");
	if (p)
	  {
	     data = g_LoadPPM(p, &w, &h);
	     close_helper(p);
	  }
     }
   if ((!eim) && (!data) && (id->fallback))
     {
	if (!strcasecmp(s, "jpeg"))
	   strcpy(cmd, "%J %s");
	else if (!strcasecmp(s, "jpg"))
	   strcpy(cmd, "%J %s");
	else if (!strcasecmp(s, "bmp"))
	   strcpy(cmd, "%P/bmptoppm %s");
	else if (!strcasecmp(s, "ilbm"))
	   strcpy(cmd, "%P/ilbmtoppm %s");
	else if (!strcasecmp(s, "ilb"))
	   strcpy(cmd, "%P/ilbmtoppm %s");
	else if (!strcasecmp(s, "iff"))
	   strcpy(cmd, "%P/ilbmtoppm %s");
	else if (!strcasecmp(s, "img"))
	   strcpy(cmd, "%P/imgtoppm %s");
	else if (!strcasecmp(s, "mtv"))
	   strcpy(cmd, "%P/mtvtoppm %s");
	else if (!strcasecmp(s, "pcx"))
	   strcpy(cmd, "%P/pcxtoppm %s");
	else if (!strcasecmp(s, "pgm"))
	   strcpy(cmd, "%P/pgmtoppm rgb:ffff/ffff/ffff %s");
	else if (!strcasecmp(s, "pi1"))
	   strcpy(cmd, "%P/pi1toppm %s");
	else if (!strcasecmp(s, "pict"))
	   strcpy(cmd, "%P/picttoppm %s");
	else if (!strcasecmp(s, "pic"))
	   strcpy(cmd, "%P/picttoppm %s");
	else if (!strcasecmp(s, "pj"))
	   strcpy(cmd, "%P/pjtoppm %s");
	else if (!strcasecmp(s, "qrt"))
	   strcpy(cmd, "%P/qrttoppm %s");
	else if (!strcasecmp(s, "sld"))
	   strcpy(cmd, "%P/sldtoppm %s");
	else if (!strcasecmp(s, "spc"))
	   strcpy(cmd, "%P/spctoppm %s");
	else if (!strcasecmp(s, "spu"))
	   strcpy(cmd, "%P/sputoppm %s");
	else if (!strcasecmp(s, "tga"))
	   strcpy(cmd, "%P/tgatoppm %s");
	else if (!strcasecmp(s, "xim"))
	   strcpy(cmd, "%P/ximtoppm %s");
	else if (!strcasecmp(s, "xpm"))
	   strcpy(cmd, "%P/xpmtoppm %s");
	else if (!strcasecmp(s, "gif"))
	   strcpy(cmd, "%P/giftopnm %s");
	else if (!strcasecmp(s, "rast"))
	   strcpy(cmd, "%P/rasttopnm %s");
	else if (!strcasecmp(s, "ras"))
	   strcpy(cmd, "%P/rasttopnm %s");
	else if (!strcasecmp(s, "sgi"))
	   strcpy(cmd, "%P/sgitopnm %s");
	else if (!strcasecmp(s, "sir"))
	   strcpy(cmd, "%P/sirtopnm %s");
	else if (!strcasecmp(s, "tiff"))
	   strcpy(cmd, "%P/tifftopnm %s");
	else if (!strcasecmp(s, "tif"))
	   strcpy(cmd, "%P/tifftopnm %s");
	else if (!strcasecmp(s, "wxd"))
	   strcpy(cmd, "%P/wxdtopnm %s");
	else if (!strcasecmp(s, "zeiss"))
	   strcpy(cmd, "%P/zeisstopnm -ppm %s");
	else if (!strcasecmp(s, "zei"))
	   strcpy(cmd, "%P/zeisstopnm -ppm %s");
	else if (!strcasecmp(s, "zis"))
	   strcpy(cmd, "%P/zeisstopnm -ppm %s");
	else
	   strcpy(cmd, "%P/anytopnm %s");
	p = open_helper(cmd, fil, "rb");
	if (p)
	  {
	     data = g_LoadPPM(p, &w, &h);
	     close_helper(p);
	  }
     }

   if (!eim && !data)
     {
	fprintf(stderr, "gdk_imlib ERROR: Cannot load image: %s\nAll fallbacks failed.\n", fil);
	return NULL;
     }

   im = (GdkImlibImage *) malloc(sizeof(GdkImlibImage));
   if (!im)
     {
	fprintf(stderr, "gdk_imlib ERROR: Cannot allocate RAM for image data\n");
	if (data)
	   free(data);
	return NULL;
     }
   im->alpha_data = NULL;
   if (trans)
     {
	im->shape_color.r = 255;
	im->shape_color.g = 0;
	im->shape_color.b = 255;
     }
   else
     {
	im->shape_color.r = -1;
	im->shape_color.g = -1;
	im->shape_color.b = -1;
     }
   im->border.left = 0;
   im->border.right = 0;
   im->border.top = 0;
   im->border.bottom = 0;
   im->cache = 1;
   im->rgb_data = data;
   im->rgb_width = w;
   im->rgb_height = h;
   im->pixmap = NULL;
   im->shape_mask = NULL;
   if (eim)
     {
	char                s1[256], s2[256];
	int                 num, size;
	int                 r, g, b;
	int                 br, bl, bt, bb;

	/* Load Native-as-can-be EIM format (Enlightenment IMlib format) */
	p = fopen(fil, "r");
	if (!p)
	  {
	     free(im);
	     return NULL;
	  }
	fgets(s, 4096, p);
	if ((s[0] != 'E') && (s[1] != 'I') && (s[2] != 'M') && (s[3] != ' '))
	  {
	     fclose(p);
	     free(im);
	     return NULL;
	  }
	sscanf(s, "%256s %i", s1, &num);
	if (num <= 0)
	  {
	     fclose(p);
	     free(im);
	     return NULL;
	  }
	while (fgets(s, 4096, p))
	  {
	     sscanf(s, "%256s", s1);
	     if (!strcmp("IMAGE", s1))
	       {
		  sscanf(s, "%256s %i %256s %i %i %i %i %i %i %i %i %i", s1, &size, s2, &w, &h, &r, &g, &b, &bl, &br, &bt, &bb);
		  if (!iden[0])
		     break;
		  else if (!strcmp(iden, s2))
		     break;
		  if (size > 0)
		     fseek(p, size, SEEK_CUR);
	       }
	  }
	im->rgb_data = malloc(w * h * 3);
	if (!im->rgb_data)
	  {
	     fclose(p);
	     free(im);
	     return NULL;
	  }
	im->shape_color.r = r;
	im->shape_color.g = g;
	im->shape_color.b = b;
	im->rgb_width = w;
	im->rgb_height = h;
	im->border.left = bl;
	im->border.right = br;
	im->border.top = bt;
	im->border.bottom = bb;
	fread(im->rgb_data, 1, w * h * 3, p);
	fclose(p);
	if (iden[0])
	  {
	     strncat(fil, ":", sizeof(fil) - strlen(fil));
	     strncat(fil, iden, sizeof(fil) - strlen(fil));
	  }
     }
   im->mod.gamma = id->mod.gamma;
   im->mod.brightness = id->mod.brightness;
   im->mod.contrast = id->mod.contrast;
   im->rmod.gamma = id->rmod.gamma;
   im->rmod.brightness = id->rmod.brightness;
   im->rmod.contrast = id->rmod.contrast;
   im->gmod.gamma = id->gmod.gamma;
   im->gmod.brightness = id->gmod.brightness;
   im->gmod.contrast = id->gmod.contrast;
   im->bmod.gamma = id->bmod.gamma;
   im->bmod.brightness = id->bmod.brightness;
   im->bmod.contrast = id->bmod.contrast;
   im->filename = malloc(strlen(file) + 1);
   if (im->filename)
      strcpy(im->filename, file);
   if ((id->cache.on_image && im))
      gadd_image(im, fil);
   gcalc_map_tables(im);
   return im;
}

gint
gdk_imlib_save_image_to_eim(GdkImlibImage * im, char *file)
{
   char                fil[4096];
   char               *iden;
   FILE               *f;
   int                 size;

   if ((!id) || (!im) || (!file))
      return 0;
   strncpy(fil, file, sizeof(fil));
   iden = g_SplitID(fil);
   if (!iden[0])
      iden = "default";
   f = fopen(fil, "w");
   if (!f)
      return 0;

   size = im->rgb_width * im->rgb_height * 3;
   fprintf(f, "EIM 1\n");
   fprintf(f, "IMAGE %i %s %i %i %i %i %i %i %i %i %i\n",
	   size,
	   iden,
	   im->rgb_width,
	   im->rgb_height,
	   im->shape_color.r,
	   im->shape_color.g,
	   im->shape_color.b,
	   im->border.left,
	   im->border.right,
	   im->border.top,
	   im->border.bottom);
   if (fwrite(im->rgb_data, size, 1, f) != 1)
     {
	fclose(f);
	return 0;
     }
   fclose(f);
   return 1;
}

gint
gdk_imlib_add_image_to_eim(GdkImlibImage * im, char *file)
{
   char                fil[4096];
   char               *iden;
   FILE               *f;
   int                 size;

   if ((!id) || (!im) || (!file))
      return 0;
   strncpy(fil, file, sizeof(fil));

   iden = g_SplitID(file);
   if (!iden[0])
      strcpy(iden, "default");

   f = fopen(fil, "a");
   if (!f)
      return 0;

   size = im->rgb_width * im->rgb_height * 3;
   fprintf(f, "IMAGE %i %s %i %i %i %i %i %i %i %i %i\n",
	   size,
	   iden,
	   im->rgb_width,
	   im->rgb_height,
	   im->shape_color.r,
	   im->shape_color.g,
	   im->shape_color.b,
	   im->border.left,
	   im->border.right,
	   im->border.top,
	   im->border.bottom);

   if (fwrite(im->rgb_data, size, 1, f) != 1)
     {
	fclose(f);
	return 0;
     }
   fclose(f);
   return 1;
}

gint
gdk_imlib_save_image_to_ppm(GdkImlibImage * im, char *file)
{
   FILE               *f;

   if ((!id) || (!im) || (!file))
      return 0;
   f = fopen(file, "w");
   if (!f)
      return 0;

   fprintf(f, "P6\n");
   fprintf(f, "%i %i\n255\n",
	   im->rgb_width,
	   im->rgb_height);
   if (fwrite(im->rgb_data, im->rgb_width * im->rgb_height * 3, 1, f) != 1)
     {
	fclose(f);
	return 0;
     }
   fclose(f);
   return 1;
}
