#define _GNU_SOURCE
#include <config.h>

#include "gdk_imlib.h"
#include "gdk_imlib_private.h"

gint
gdk_imlib_save_image(GdkImlibImage * im, char *file, GdkImlibSaveInfo * info)
{
   char               *ext;
   char                cmd[10240];
   FILE               *f;
   GdkImlibSaveInfo    defaults;

   if (!im || !file)
      return 0;

   defaults.quality = 208;
   defaults.scaling = 1024;
   defaults.xjustification = 512;
   defaults.yjustification = 512;
   defaults.page_size = PAGE_SIZE_LETTER;
   defaults.color = 1;

   if (!info)
      info = &defaults;
   ext = g_GetExtension(file);

   if ((!strcasecmp(ext, "ppm")) || (!strcasecmp(ext, "pnm")))
     {
	f = fopen(file, "wb");
	if (f)
	  {
	     if (!fprintf(f, "P6\n# Created by Imlib\n%i %i\n255\n", im->rgb_width, im->rgb_height))
	       {
		  fclose(f);
		  return 0;
	       }
	     if (!fwrite(im->rgb_data, 1, (im->rgb_width * im->rgb_height * 3), f))
	       {
		  fclose(f);
		  return 0;
	       }
	     fclose(f);
	     return 1;
	  }
     }
   else if (!strcasecmp(ext, "pgm"))
     {
	int                 x, y;
	unsigned char      *ptr, val;
	int                 v;

	f = fopen(file, "wb");
	if (f)
	  {
	     if (!fprintf(f, "P5\n# Created by Imlib\n%i %i\n255\n", im->rgb_width, im->rgb_height))
	       {
		  fclose(f);
		  return 0;
	       }
	     ptr = im->rgb_data;
	     for (y = 0; y < im->rgb_height; y++)
	       {
		  for (x = 0; x < im->rgb_width; x++)
		    {
		       v = (int)(*ptr++);
		       v += (int)(*ptr++);
		       v += (int)(*ptr++);
		       val = (unsigned char)(v / 3);
		       if (!fwrite(&val, 1, 1, f))
			 {
			    fclose(f);
			    return 0;
			 }
		    }
	       }
	     fclose(f);
	     return 1;
	  }
     }
   else if (!strcasecmp(ext, "ps"))
     {
	int                 bx, by, bxx, byy;
	int                 w, h;
	int                 sx, sy;
	int                 tx = 35, ty = 35;
	int                 x, y;
	unsigned char      *ptr;
	int                 v;

	f = fopen(file, "wb");

	if (f == NULL)
	   return 0;

	w = im->rgb_width;
	h = im->rgb_height;

	switch (info->page_size)
	  {
	  case PAGE_SIZE_EXECUTIVE:
	     sx = 540;
	     sy = 720;
	     break;
	  case PAGE_SIZE_LETTER:
	     sx = 612;
	     sy = 792;
	     break;
	  case PAGE_SIZE_LEGAL:
	     sx = 612;
	     sy = 1008;
	     break;
	  case PAGE_SIZE_A4:
	     sx = 595;
	     sy = 842;
	     break;
	  case PAGE_SIZE_A3:
	     sx = 842;
	     sy = 1190;
	     break;
	  case PAGE_SIZE_A5:
	     sx = 420;
	     sy = 595;
	     break;
	  case PAGE_SIZE_FOLIO:
	     sx = 612;
	     sy = 936;
	     break;
	  }
	bxx = ((sx - (tx * 2)) * info->scaling) >> 10;
	byy = ((h * bxx / w) * info->scaling) >> 10;
	if ((((sy - (ty * 2)) * info->scaling) >> 10) < byy)
	  {
	     byy = ((sy - (ty * 2)) * info->scaling) >> 10;
	     bxx = ((w * byy / h) * info->scaling) >> 10;
	  }
	bx = tx + ((((sx - (tx * 2)) - bxx) * info->xjustification) >> 10);
	by = ty + ((((sy - (ty * 2)) - byy) * info->yjustification) >> 10);
	if (f)
	  {
	     fprintf(f, "%%!PS-Adobe-2.0 EPSF-2.0\n");
	     fprintf(f, "%%%%Title: %s\n", file);
	     fprintf(f, "%%%%Creator: Imlib by The Rasterman\n");
	     fprintf(f, "%%%%BoundingBox: %i %i %i %i\n", bx, by, bxx, byy);
	     fprintf(f, "%%%%Pages: 1\n");
	     fprintf(f, "%%%%DocumentFonts:\n");
	     fprintf(f, "%%%%EndComments\n");
	     fprintf(f, "%%%%EndProlog\n");
	     fprintf(f, "%%%%Page: 1 1\n");
	     fprintf(f, "/origstate save def\n");
	     fprintf(f, "20 dict begin\n");
	     if (info->color)
	       {
		  fprintf(f, "/pix %i string def\n", w * 3);
		  fprintf(f, "/grays %i string def\n", w);
		  fprintf(f, "/npixls 0 def\n");
		  fprintf(f, "/rgbindx 0 def\n");
		  fprintf(f, "%i %i translate\n", bx, by);
		  fprintf(f, "%i %i scale\n", bxx, byy);
		  fprintf(f,
			  "/colorimage where\n"
			  "{ pop }\n"
			  "{\n"
			  "/colortogray {\n"
			  "/rgbdata exch store\n"
			  "rgbdata length 3 idiv\n"
			  "/npixls exch store\n"
			  "/rgbindx 0 store\n"
			  "0 1 npixls 1 sub {\n"
			  "grays exch\n"
			  "rgbdata rgbindx       get 20 mul\n"
			  "rgbdata rgbindx 1 add get 32 mul\n"
			  "rgbdata rgbindx 2 add get 12 mul\n"
			  "add add 64 idiv\n"
			  "put\n"
			  "/rgbindx rgbindx 3 add store\n"
			  "} for\n"
			  "grays 0 npixls getinterval\n"
			  "} bind def\n"
			  "/mergeprocs {\n"
			  "dup length\n"
			  "3 -1 roll\n"
			  "dup\n"
			  "length\n"
			  "dup\n"
			  "5 1 roll\n"
			  "3 -1 roll\n"
			  "add\n"
			  "array cvx\n"
			  "dup\n"
			  "3 -1 roll\n"
			  "0 exch\n"
			  "putinterval\n"
			  "dup\n"
			  "4 2 roll\n"
			  "putinterval\n"
			  "} bind def\n"
			  "/colorimage {\n"
			  "pop pop\n"
			  "{colortogray} mergeprocs\n"
			  "image\n"
			  "} bind def\n"
			  "} ifelse\n");
		  fprintf(f, "%i %i 8\n", w, h);
		  fprintf(f, "[%i 0 0 -%i 0 %i]\n", w, h, h);
		  fprintf(f, "{currentfile pix readhexstring pop}\n");
		  fprintf(f, "false 3 colorimage\n");
		  fprintf(f, "\n");
		  ptr = im->rgb_data;
		  for (y = 0; y < h; y++)
		    {
		       for (x = 0; x < w; x++)
			 {
			    v = (int)(*ptr++);
			    if (v < 0x10)
			       fprintf(f, "0%x", v);
			    else
			       fprintf(f, "%x", v);
			    v = (int)(*ptr++);
			    if (v < 0x10)
			       fprintf(f, "0%x", v);
			    else
			       fprintf(f, "%x", v);
			    v = (int)(*ptr++);
			    if (v < 0x10)
			       fprintf(f, "0%x", v);
			    else
			       fprintf(f, "%x", v);
			 }
		       fprintf(f, "\n");
		    }
	       }
	     else
	       {
		  fprintf(f, "/pix %i string def\n", w);
		  fprintf(f, "/grays %i string def\n", w);
		  fprintf(f, "/npixls 0 def\n");
		  fprintf(f, "/rgbindx 0 def\n");
		  fprintf(f, "%i %i translate\n", bx, by);
		  fprintf(f, "%i %i scale\n", bxx, byy);
		  fprintf(f, "%i %i 8\n", w, h);
		  fprintf(f, "[%i 0 0 -%i 0 %i]\n", w, h, h);
		  fprintf(f, "{currentfile pix readhexstring pop}\n");
		  fprintf(f, "image\n");
		  fprintf(f, "\n");
		  ptr = im->rgb_data;
		  for (y = 0; y < h; y++)
		    {
		       for (x = 0; x < w; x++)
			 {
			    v = (int)(*ptr++);
			    v += (int)(*ptr++);
			    v += (int)(*ptr++);
			    v /= 3;
			    if (v < 0x10)
			       fprintf(f, "0%x", v);
			    else
			       fprintf(f, "%x", v);
			 }
		       fprintf(f, "\n");
		    }
	       }
	     fprintf(f, "\n");
	     fprintf(f, "showpage\n");
	     fprintf(f, "end\n");
	     fprintf(f, "origstate restore\n");
	     fprintf(f, "%%%%Trailer\n");
	     fclose(f);
	     return 1;
	  }
     }
   else if ((!strcasecmp(ext, "jpeg")) || (!strcasecmp(ext, "jpg")))
     {
#ifdef HAVE_LIBJPEG
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW            row_pointer[1];
	int                 row_stride;
	int                 y;

	f = fopen(file, "wb");
	if (f)
	  {
	     cinfo.err = jpeg_std_error(&jerr);
	     jpeg_create_compress(&cinfo);
	     jpeg_stdio_dest(&cinfo, f);
	     cinfo.image_width = im->rgb_width;
	     cinfo.image_height = im->rgb_height;
	     cinfo.input_components = 3;
	     cinfo.in_color_space = JCS_RGB;
	     jpeg_set_defaults(&cinfo);
	     jpeg_set_quality(&cinfo, (100 * info->quality) >> 8, TRUE);
	     jpeg_start_compress(&cinfo, TRUE);
	     row_stride = cinfo.image_width * 3;
	     while (cinfo.next_scanline < cinfo.image_height)
	       {
		  row_pointer[0] = im->rgb_data + (cinfo.next_scanline * row_stride);
		  jpeg_write_scanlines(&cinfo, row_pointer, 1);
	       }
	     jpeg_finish_compress(&cinfo);
	     fclose(f);
	     return 1;
	  }
#endif
     }
   else if (!strcasecmp(ext, "png"))
     {
#ifdef HAVE_LIBPNG
	png_structp         png_ptr;
	png_infop           info_ptr;
	unsigned char      *data, *ptr;
	int                 x, y;
	png_bytep           row_ptr;
	png_color_8         sig_bit;

	f = fopen(file, "wb");
	if (f)
	  {
	     png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
					       NULL, NULL, NULL);
	     if (!png_ptr)
	       {
		  fclose(f);
		  return 0;
	       }
	     info_ptr = png_create_info_struct(png_ptr);
	     if (info_ptr == NULL)
	       {
		  fclose(f);
		  png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
		  return 0;
	       }
	     if (setjmp(png_ptr->jmpbuf))
	       {
		  fclose(f);
		  png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
		  return 0;
	       }
	     png_init_io(png_ptr, f);
	     png_set_IHDR(png_ptr, info_ptr, im->rgb_width, im->rgb_height, 8,
			  PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
			  PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	     sig_bit.red = 8;
	     sig_bit.green = 8;
	     sig_bit.blue = 8;
	     sig_bit.alpha = 8;
	     png_set_sBIT(png_ptr, info_ptr, &sig_bit);
	     png_write_info(png_ptr, info_ptr);
	     png_set_shift(png_ptr, &sig_bit);
	     png_set_packing(png_ptr);
	     data = malloc(im->rgb_width * 4);
	     if (!data)
	       {
		  fclose(f);
		  png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
		  return 0;
	       }
	     for (y = 0; y < im->rgb_height; y++)
	       {
		  ptr = im->rgb_data + (y * im->rgb_width * 3);
		  for (x = 0; x < im->rgb_width; x++)
		    {
		       data[(x << 2) + 0] = *ptr++;
		       data[(x << 2) + 1] = *ptr++;
		       data[(x << 2) + 2] = *ptr++;
		       if ((data[(x << 2) + 0] == im->shape_color.r) &&
			   (data[(x << 2) + 1] == im->shape_color.g) &&
			   (data[(x << 2) + 2] == im->shape_color.b))
			  data[(x << 2) + 3] = 0;
		       else
			  data[(x << 2) + 3] = 255;
		    }
		  row_ptr = data;
		  png_write_rows(png_ptr, &row_ptr, 1);
	       }
	     free(data);
	     png_write_end(png_ptr, info_ptr);
	     png_destroy_write_struct(&png_ptr, (png_infopp) NULL);

	     fclose(f);
	     return 1;
	  }
#endif
     }
   else if ((!strcasecmp(ext, "tiff")) || (!strcasecmp(ext, "tif")))
     {
#ifdef HAVE_LIBTIFF
	TIFF               *tif;
	unsigned char      *ptr, *data;
	int                 x, y;
	int                 w;

	tif = TIFFOpen(file, "w");
	if (tif)
	  {
	     TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, im->rgb_width);
	     TIFFSetField(tif, TIFFTAG_IMAGELENGTH, im->rgb_height);
	     TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	     TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
	     TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	     TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
	     {
		TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
		TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
		w = TIFFScanlineSize(tif);
		TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,
			     TIFFDefaultStripSize(tif, -1));
		for (y = 0; y < im->rgb_height; y++)
		  {
		     data = im->rgb_data + (y * im->rgb_width * 3);
		     TIFFWriteScanline(tif, data, y, 0);
		  }
	     }
	     TIFFClose(tif);
	     return 1;
	  }
#endif
     }
   if (id->fallback)
     {
	f = open_helper("%C/convert pnm:- %s", file, "wb");
	if (f)
	  {
	     if (!fprintf(f, "P6\n# Created by Imlib\n%i %i\n255\n", im->rgb_width, im->rgb_height))
	       {
		  close_helper(f);
		  return 0;
	       }
	     if (!fwrite(im->rgb_data, 1, (im->rgb_width * im->rgb_height * 3), f))
	       {
		  close_helper(f);
		  return 0;
	       }
	     if (close_helper(f))
		return 0;
	     return 1;
	  }

	if (!strcasecmp(ext, "jpeg"))
	   g_snprintf(cmd, sizeof(cmd), "%%H -quality %i -progressive -outfile %%s", 100 * info->quality / 256);
	else if (!strcasecmp(ext, "jpg"))
	   g_snprintf(cmd, sizeof(cmd), "%%H -quality %i -progressive -outfile %%s", 100 * info->quality / 256);
	else if (!strcasecmp(ext, "bmp"))
	   strcpy(cmd, "%Q %N/ppmtobmp >%s");
	else if (!strcasecmp(ext, "gif"))
	   strcpy(cmd, "%Q %N/ppmtogif -interlace >%s");
	else if (!strcasecmp(ext, "ilbm"))
	   strcpy(cmd, "%N/ppmtoilbm -24if -hires -lace -compress >%s");
	else if (!strcasecmp(ext, "ilb"))
	   strcpy(cmd, "%N/ppmtoilbm -24if -hires -lace -compress >%s");
	else if (!strcasecmp(ext, "iff"))
	   strcpy(cmd, "%N/ppmtoilbm -24if -hires -lace -compress >%s");
	else if (!strcasecmp(ext, "icr"))
	   strcpy(cmd, "%N/ppmtoicr >%s");
	else if (!strcasecmp(ext, "map"))
	   strcpy(cmd, "%N/ppmtomap >%s");
	else if (!strcasecmp(ext, "mit"))
	   strcpy(cmd, "%N/ppmtomitsu -sharpness 4 >%s");
	else if (!strcasecmp(ext, "mitsu"))
	   strcpy(cmd, "%N/ppmtomitsu -sharpness 4 >%s");
	else if (!strcasecmp(ext, "pcx"))
	   strcpy(cmd, "%N/ppmtopcx -24bit -packed >%s");
	else if (!strcasecmp(ext, "pgm"))
	   strcpy(cmd, "%N/ppmtopgm >%s");
	else if (!strcasecmp(ext, "pi1"))
	   strcpy(cmd, "%N/ppmtopi1 >%s");
	else if (!strcasecmp(ext, "pic"))
	   strcpy(cmd, "%Q %N/ppmtopict >%s");
	else if (!strcasecmp(ext, "pict"))
	   strcpy(cmd, "%Q %N/ppmtopict >%s");
	else if (!strcasecmp(ext, "pj"))
	   strcpy(cmd, "%N/ppmtopj >%s");
	else if (!strcasecmp(ext, "pjxl"))
	   strcpy(cmd, "%N/ppmtopjxl >%s");
	else if (!strcasecmp(ext, "puz"))
	   strcpy(cmd, "%N/ppmtopuzz >%s");
	else if (!strcasecmp(ext, "puzz"))
	   strcpy(cmd, "%N/ppmtopuzz >%s");
	else if (!strcasecmp(ext, "rgb3"))
	   strcpy(cmd, "%N/ppmtorgb3 >%s");
	else if (!strcasecmp(ext, "six"))
	   strcpy(cmd, "%N/ppmtosixel >%s");
	else if (!strcasecmp(ext, "sixel"))
	   strcpy(cmd, "%N/ppmtosizel >%s");
	else if (!strcasecmp(ext, "tga"))
	   strcpy(cmd, "%N/ppmtotga -rgb >%s");
	else if (!strcasecmp(ext, "targa"))
	   strcpy(cmd, "%N/ppmtotga -rgb >%s");
	else if (!strcasecmp(ext, "uil"))
	   strcpy(cmd, "%N/ppmtouil >%s");
	else if (!strcasecmp(ext, "xpm"))
	   strcpy(cmd, "%Q %N/ppmtoxpm >%s");
	else if (!strcasecmp(ext, "yuv"))
	   strcpy(cmd, "%N/ppmtoyuv >%s");
	else if (!strcasecmp(ext, "png"))
	   strcpy(cmd, "%N/pnmtopng >%s");
	else if (!strcasecmp(ext, "ps"))
	   strcpy(cmd, "%N/pnmtops -center -scale 100 >%s");
	else if (!strcasecmp(ext, "rast"))
	   strcpy(cmd, "%N/pnmtorast -rle >%s");
	else if (!strcasecmp(ext, "ras"))
	   strcpy(cmd, "%N/pnmtorast -rle >%s");
	else if (!strcasecmp(ext, "sgi"))
	   strcpy(cmd, "%N/pnmtosgi >%s");
	else if (!strcasecmp(ext, "sir"))
	   strcpy(cmd, "%N/pnmtosir >%s");
	else if (!strcasecmp(ext, "tif"))
	   strcpy(cmd, "%N/pnmtotiff -lzw >%s");
	else if (!strcasecmp(ext, "tiff"))
	   strcpy(cmd, "%N/pnmtotiff -lzw >%s");
	else if (!strcasecmp(ext, "xwd"))
	   strcpy(cmd, "%N/pnmtoxwd >%s");
	else
	   ext = "";
	if (ext[0])
	  {
	     f = open_helper(cmd, file, "wb");
	     if (f)
	       {
		  if (!fprintf(f, "P6\n# Created by Imlib\n%i %i\n255\n", im->rgb_width, im->rgb_height))
		    {
		       close_helper(f);
		       return 0;
		    }
		  if (!fwrite(im->rgb_data, 1, (im->rgb_width * im->rgb_height * 3), f))
		    {
		       close_helper(f);
		       return 0;
		    }
		  if (close_helper(f))
		     return 0;
		  return 1;
	       }
	  }
     }
   return 0;
}
