/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%              X   X  W   W  IIIII  N   N  DDDD    OOO   W   W                %
%               X X   W   W    I    NN  N  D   D  O   O  W   W                %
%                X    W   W    I    N N N  D   D  O   O  W   W                %
%               X X   W W W    I    N  NN  D   D  O   O  W W W                %
%              X   X   W W   IIIII  N   N  DDDD    OOO    W W                 %
%                                                                             %
%                                                                             %
%                     X11 Utility Methods for ImageMagick                     %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                  July 1992                                  %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("ImageMagick"),  %
%  to deal in ImageMagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of ImageMagick, and to permit persons to whom the       %
%  ImageMagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                         %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  In no event shall   %
%  ImageMagick Studio be liable for any claim, damages or other liability,    %
%  whether in an action of contract, tort or otherwise, arising from, out of  %
%  or in connection with ImageMagick or the use or other dealings in          %
%  ImageMagick.                                                               %
%                                                                             %
%  Except as contained in this notice, the name of the ImageMagick Studio     %
%  shall not be used in advertising or otherwise to promote the sale, use or  %
%  other dealings in ImageMagick without prior written authorization from the %
%  ImageMagick Studio.                                                        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "blob.h"
#include "log.h"
#include "utility.h"
#include "version.h"
#include "xwindow.h"
#if defined(HasX11)

/*
  X defines.
*/
#define XBlueGamma(color) ((Quantum) (blue_gamma == 1.0 ? (color) : \
  ((pow((double) (color)/MaxRGB,1.0/blue_gamma)*MaxRGB)+0.5)))
#define XGammaPixel(map,color)  (unsigned long) (map->base_pixel+ \
  ((ScaleQuantumToShort(XRedGamma((color)->red))*map->red_max/65535L)* \
    map->red_mult)+ \
  ((ScaleQuantumToShort(XGreenGamma((color)->green))*map->green_max/65535L)* \
    map->green_mult)+ \
  ((ScaleQuantumToShort(XBlueGamma((color)->blue))*map->blue_max/65535L)* \
    map->blue_mult))
#define XGreenGamma(color) ((Quantum) (green_gamma == 1.0 ? (color) : \
  ((pow((double) (color)/MaxRGB,1.0/green_gamma)*MaxRGB)+0.5)))
#define XRedGamma(color) ((Quantum) (red_gamma == 1.0 ? (color) : \
  ((pow((double) (color)/MaxRGB,1.0/red_gamma)*MaxRGB)+0.5)))
#define XStandardPixel(map,color)  (unsigned long) (map->base_pixel+ \
  (((color)->red*map->red_max/65535L)*map->red_mult)+ \
  (((color)->green*map->green_max/65535L)*map->green_mult)+ \
  (((color)->blue*map->blue_max/65535L)*map->blue_mult))

/*
  Constant declaractions.
*/
static unsigned int
  xerror_alert = False;

/*
  Method prototypes.
*/
static char
  *XVisualClassName(const int);

static double
  blue_gamma = 1.0,
  green_gamma = 1.0,
  red_gamma = 1.0;

static unsigned int
  XMakePixmap(Display *,const XResourceInfo *,XWindowInfo *);

static void
  XMakeImageLSBFirst(const XResourceInfo *,const XWindowInfo *,Image *,
    XImage *,XImage *),
  XMakeImageMSBFirst(const XResourceInfo *,const XWindowInfo *,Image *,
    XImage *,XImage *);

static Window
  XSelectWindow(Display *,RectangleInfo *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s T r u e                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsTrue returns True if the reason is "true", "on", "yes" or "1".
%
%  The format of the IsTrue method is:
%
%      unsigned int IsTrue(const char *reason)
%
%  A description of each parameter follows:
%
%    o option: either True or False depending on the reason parameter.
%
%    o reason: Specifies a pointer to a character array.
%
%
*/
MagickExport unsigned int IsTrue(const char *reason)
{
  if (reason == (char *) NULL)
    return(False);
  if (LocaleCompare(reason,"true") == 0)
    return(True);
  if (LocaleCompare(reason,"on") == 0)
    return(True);
  if (LocaleCompare(reason,"yes") == 0)
    return(True);
  if (LocaleCompare(reason,"1") == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X A n n o t a t e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XAnnotateImage annotates the image with text.
%
%  The format of the XAnnotateImage method is:
%
%      unsigned int XAnnotateImage(Display *display,
%        const XPixelInfo *pixel,XAnnotateInfo *annotate_info,Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method XAnnotateImage returns True if the image is
%      successfully annotated with text.  False is returned is there is a
%      memory shortage.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o pixel: Specifies a pointer to a XPixelInfo structure.
%
%    o annotate_info: Specifies a pointer to a XAnnotateInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%
*/
MagickExport unsigned int XAnnotateImage(Display *display,
  const XPixelInfo *pixel,XAnnotateInfo *annotate_info,Image *image)
{
  GC
    annotate_context;

  Image
    *annotate_image;

  int
    x,
    y;

  Pixmap
    annotate_pixmap;

  register PixelPacket
    *q;

  unsigned int
    depth,
    height,
    matte,
    width;

  Window
    root_window;

  XGCValues
    context_values;

  XImage
    *annotate_ximage;

  /*
    Initialize annotated image.
  */
  assert(display != (Display *) NULL);
  assert(pixel != (XPixelInfo *) NULL);
  assert(annotate_info != (XAnnotateInfo *) NULL);
  assert(image != (Image *) NULL);
  /*
    Initialize annotated pixmap.
  */
  root_window=XRootWindow(display,XDefaultScreen(display));
  depth=XDefaultDepth(display,XDefaultScreen(display));
  annotate_pixmap=XCreatePixmap(display,root_window,annotate_info->width,
    annotate_info->height,(int) depth);
  if (annotate_pixmap == (Pixmap) NULL)
    return(False);
  /*
    Initialize graphics info.
  */
  context_values.background=0;
  context_values.foreground=(unsigned long) (~0);
  context_values.font=annotate_info->font_info->fid;
  annotate_context=XCreateGC(display,root_window,GCBackground | GCFont |
    GCForeground,&context_values);
  if (annotate_context == (GC) NULL)
    return(False);
  /*
    Draw text to pixmap.
  */
  (void) XDrawImageString(display,annotate_pixmap,annotate_context,0,
    (int) annotate_info->font_info->ascent,annotate_info->text,
    (int) strlen(annotate_info->text));
  (void) XFreeGC(display,annotate_context);
  /*
    Initialize annotated X image.
  */
  annotate_ximage=XGetImage(display,annotate_pixmap,0,0,annotate_info->width,
    annotate_info->height,AllPlanes,ZPixmap);
  if (annotate_ximage == (XImage *) NULL)
    return(False);
  (void) XFreePixmap(display,annotate_pixmap);
  /*
    Initialize annotated image.
  */
  annotate_image=AllocateImage((ImageInfo *) NULL);
  if (annotate_image == (Image *) NULL)
    return(False);
  annotate_image->columns=annotate_info->width;
  annotate_image->rows=annotate_info->height;
  /*
    Transfer annotated X image to image.
  */
  width=(unsigned int) image->columns;
  height=(unsigned int) image->rows;
  x=0;
  y=0;
  (void) XParseGeometry(annotate_info->geometry,&x,&y,&width,&height);
  annotate_image->background_color=GetOnePixel(image,x,y);
  annotate_image->matte=annotate_info->stencil == ForegroundStencil;
  for (y=0; y < (long) annotate_image->rows; y++)
  {
    q=GetImagePixels(annotate_image,0,y,annotate_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) annotate_image->columns; x++)
    {
      q->opacity=OpaqueOpacity;
      if (XGetPixel(annotate_ximage,x,y) == 0)
        {
          /*
            Set this pixel to the background color.
          */
          q->red=ScaleShortToQuantum(pixel->box_color.red);
          q->green=ScaleShortToQuantum(pixel->box_color.green);
          q->blue=ScaleShortToQuantum(pixel->box_color.blue);
          if ((annotate_info->stencil == ForegroundStencil) ||
              (annotate_info->stencil == OpaqueStencil))
            q->opacity=TransparentOpacity;
        }
      else
        {
          /*
            Set this pixel to the pen color.
          */
          q->red=ScaleShortToQuantum(pixel->pen_color.red);
          q->green=ScaleShortToQuantum(pixel->pen_color.green);
          q->blue=ScaleShortToQuantum(pixel->pen_color.blue);
          if (annotate_info->stencil == BackgroundStencil)
            q->opacity=TransparentOpacity;
        }
      q++;
    }
    if (!SyncImagePixels(annotate_image))
      break;
  }
  XDestroyImage(annotate_ximage);
  /*
    Determine annotate geometry.
  */
  (void) XParseGeometry(annotate_info->geometry,&x,&y,&width,&height);
  if ((width != (unsigned int) annotate_image->columns) ||
      (height != (unsigned int) annotate_image->rows))
    {
      char
        image_geometry[MaxTextExtent];

      /*
        Scale image.
      */
      FormatString(image_geometry,"%ux%u",width,height);
      TransformImage(&annotate_image,(char *) NULL,image_geometry);
    }
  if (annotate_info->degrees != 0.0)
    {
      double
        normalized_degrees;

      Image
        *rotate_image;

      int
        rotations;

      /*
        Rotate image.
      */
      rotate_image=
        RotateImage(annotate_image,annotate_info->degrees,&image->exception);
      if (rotate_image == (Image *) NULL)
        return(False);
      DestroyImage(annotate_image);
      annotate_image=rotate_image;
      /*
        Annotation is relative to the degree of rotation.
      */
      normalized_degrees=annotate_info->degrees;
      while (normalized_degrees < -45.0)
        normalized_degrees+=360.0;
      for (rotations=0; normalized_degrees > 45.0; rotations++)
        normalized_degrees-=90.0;
      switch (rotations % 4)
      {
        default:
        case 0:
          break;
        case 1:
        {
          /*
            Rotate 90 degrees.
          */
          x-=(int) annotate_image->columns/2;
          y+=(int) annotate_image->columns/2;
          break;
        }
        case 2:
        {
          /*
            Rotate 180 degrees.
          */
          x=x-(int) annotate_image->columns;
          break;
        }
        case 3:
        {
          /*
            Rotate 270 degrees.
          */
          x=x-(int) annotate_image->columns/2;
          y=y-(int) (annotate_image->rows-(annotate_image->columns/2));
          break;
        }
      }
    }
  /*
    Composite text onto the image.
  */
  (void) XParseGeometry(annotate_info->geometry,&x,&y,&width,&height);
  matte=image->matte;
  (void) CompositeImage(image,annotate_image->matte ? OverCompositeOp :
    CopyCompositeOp,annotate_image,x,y);
  image->matte=matte;
  DestroyImage(annotate_image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X B e s t F o n t                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XBestFont returns the "best" font.  "Best" is defined as a font
%  specified in the X resource database or a font such that the text width
%  displayed with the font does not exceed the specified maximum width.
%
%  The format of the XBestFont method is:
%
%      XFontStruct *XBestFont(Display *display,
%        const XResourceInfo *resource_info,const unsigned int text_font)
%
%  A description of each parameter follows:
%
%    o font: XBestFont returns a pointer to a XFontStruct structure.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o text_font:  True is font should be mono-spaced (typewriter style).
%
%
*/

static char **FontToList(char *font)
{
  char
    **fontlist;

  register char
    *p,
    *q;

  register int
    i;

  unsigned int
    fonts;

  if (font == (char *) NULL)
    return((char **) NULL);
  /*
    Convert string to an ASCII list.
  */
  fonts=1;
  for (p=font; *p != '\0'; p++)
    if ((*p == ':') || (*p == ';') || (*p == ','))
      fonts++;
  fontlist=(char **) AcquireMemory((fonts+1)*sizeof(char *));
  if (fontlist == (char **) NULL)
    {
      MagickError(ResourceLimitError,"MemoryAllocationFailed",
        "unable to convert font");
      return((char **) NULL);
    }
  p=font;
  for (i=0; i < (int) fonts; i++)
  {
    for (q=p; *q != '\0'; q++)
      if ((*q == ':') || (*q == ';') || (*q == ','))
        break;
    fontlist[i]=(char *) AcquireMemory(q-p+1);
    if (fontlist[i] == (char *) NULL)
      {
        MagickError(ResourceLimitError,"MemoryAllocationFailed",
          "unable to convert font");
        return((char **) NULL);
      }
    (void) strncpy(fontlist[i],p,q-p);
    fontlist[i][q-p]='\0';
    p=q+1;
  }
  fontlist[i]=(char *) NULL;
  return(fontlist);
}

MagickExport XFontStruct *XBestFont(Display *display,
  const XResourceInfo *resource_info,const unsigned int text_font)
{
  static const char
    *Fonts[]=
    {
      "-*-helvetica-medium-r-normal--12-*-*-*-*-*-iso8859-1",
      "-*-arial-medium-r-normal--12-*-*-*-*-*-iso8859-1",
      "-*-helvetica-medium-r-normal--12-*-*-*-*-*-iso8859-15",
      "-*-arial-medium-r-normal--12-*-*-*-*-*-iso8859-15",
      "variable",
      (char *) NULL
    },
    *TextFonts[]=
    {
      "-*-fixed-medium-r-normal-*-12-*-*-*-*-*-iso8859-1",
      "-*-fixed-medium-r-normal-*-12-*-*-*-*-*-iso8859-15",
      "fixed",
      (char *) NULL
    };

  char
    *font_name;

  register const char
    **p;

  XFontStruct
    *font_info;

  font_info=(XFontStruct *) NULL;
  font_name=resource_info->font;
  if (text_font)
    font_name=resource_info->text_font;
  if (font_name != (char *) NULL)
    {
      char
        **fontlist;

      register int
        i;

      /*
        Load preferred font specified in the X resource database.
      */
      fontlist=FontToList(font_name);
      if (fontlist != (char **) NULL)
        {
          for (i=0; fontlist[i] != (char *) NULL; i++)
          {
            if (font_info == (XFontStruct *) NULL)
              font_info=XLoadQueryFont(display,fontlist[i]);
            LiberateMemory((void **) &fontlist[i]);
          }
          LiberateMemory((void **) &fontlist);
        }
      if (font_info == (XFontStruct *) NULL)
        MagickError(XServerError,"UnableToLoadFont",font_name);
    }
  /*
    Load fonts from list of fonts until one is found.
  */
  p=Fonts;
  if (text_font)
    p=TextFonts;
  if (XDisplayHeight(display,XDefaultScreen(display)) >= 748)
    p++;
  while (*p != (char *) NULL)
  {
    if (font_info != (XFontStruct *) NULL)
      break;
    font_info=XLoadQueryFont(display,(char *) *p);
    p++;
  }
  return(font_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X B e s t I c o n S i z e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XBestIconSize returns the "best" icon size.  "Best" is defined as
%  an icon size that maintains the aspect ratio of the image.  If the window
%  manager has preferred icon sizes, one of the preferred sizes is used.
%
%  The format of the XBestIconSize method is:
%
%      void XBestIconSize(Display *display,XWindowInfo *window,Image *image)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%
*/
MagickExport void XBestIconSize(Display *display,XWindowInfo *window,
  Image *image)
{
  double
    scale_factor;

  int
    i,
    number_sizes;

  unsigned int
    height,
    icon_height,
    icon_width,
    width;

  Window
    root_window;

  XIconSize
    *icon_size,
    *size_list;

  /*
    Determine if the window manager has specified preferred icon sizes.
  */
  assert(display != (Display *) NULL);
  assert(window != (XWindowInfo *) NULL);
  assert(image != (Image *) NULL);
  window->width=MaxIconSize;
  window->height=MaxIconSize;
  icon_size=(XIconSize *) NULL;
  number_sizes=0;
  root_window=XRootWindow(display,window->screen);
  if (XGetIconSizes(display,root_window,&size_list,&number_sizes) != 0)
    if ((number_sizes > 0) && (size_list != (XIconSize *) NULL))
      icon_size=size_list;
  if (icon_size == (XIconSize *) NULL)
    {
      /*
        Window manager does not restrict icon size.
      */
      icon_size=XAllocIconSize();
      if (icon_size == (XIconSize *) NULL)
        {
          MagickError(ResourceLimitError,"MemoryAllocationFailed",
            "unable to get best icon size");
          return;
        }
      icon_size->min_width=1;
      icon_size->max_width=MaxIconSize;
      icon_size->min_height=1;
      icon_size->max_height=MaxIconSize;
      icon_size->width_inc=1;
      icon_size->height_inc=1;
    }
  /*
    Determine aspect ratio of image.
  */
  width=(unsigned int) image->columns;
  height=(unsigned int) image->rows;
  i=0;
  if (window->crop_geometry)
    (void) XParseGeometry(window->crop_geometry,&i,&i,&width,&height);
  /*
    Look for an icon size that maintains the aspect ratio of image.
  */
  scale_factor=(double) icon_size->max_width/width;
  if (scale_factor > ((double) icon_size->max_height/height))
    scale_factor=(double) icon_size->max_height/height;
  icon_width=icon_size->min_width;
  while ((int) icon_width < icon_size->max_width)
  {
    if (icon_width >= (scale_factor*width))
      break;
    icon_width+=icon_size->width_inc;
  }
  icon_height=icon_size->min_height;
  while ((int) icon_height < icon_size->max_height)
  {
    if (icon_height >= (scale_factor*height))
      break;
    icon_height+=icon_size->height_inc;
  }
  (void) XFree((void *) icon_size);
  window->width=icon_width;
  window->height=icon_height;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X B e s t P i x e l                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XBestPixel returns a pixel from an array of pixels that is closest
%  to the requested color.  If the color array is NULL, the colors are obtained
%  from the X server.
%
%  The format of the XBestPixel method is:
%
%      void XBestPixel(Display *display,const Colormap colormap,XColor *colors,
%        unsigned int number_colors,XColor *color)
%
%  A description of each parameter follows:
%
%    o pixel: XBestPixel returns the pixel value closest to the requested
%      color.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o colormap: Specifies the ID of the X server colormap.
%
%    o colors: Specifies an array of XColor structures.
%
%    o number_colors: Specifies the number of XColor structures in the
%      color definition array.
%
%    o color: Specifies the desired RGB value to find in the colors array.
%
%
*/
MagickExport void XBestPixel(Display *display,const Colormap colormap,
  XColor *colors,unsigned int number_colors,XColor *color)
{
  double
    min_distance;

  DoublePixelPacket
    pixel;

  int
    query_server,
    status;

  register double
    distance;

  register int
    i,
    j;

  /*
    Find closest representation for the requested RGB color.
  */
  assert(display != (Display *) NULL);
  assert(color != (XColor *) NULL);
  status=XAllocColor(display,colormap,color);
  if (status != 0)
    return;
  query_server=colors == (XColor *) NULL;
  if (query_server)
    {
      /*
        Read X server colormap.
      */
      colors=(XColor *) AcquireMemory(number_colors*sizeof(XColor));
      if (colors == (XColor *) NULL)
        {
          MagickError(ResourceLimitError,"MemoryAllocationFailed",
            "unable to read X server colormap");
          return;
        }
      for (i=0; i < (int) number_colors; i++)
        colors[i].pixel=(unsigned long) i;
      if (number_colors > 256)
        number_colors=256;
      (void) XQueryColors(display,colormap,colors,number_colors);
    }
  min_distance=3.0*((double) MaxRGB+1.0)*((double) MaxRGB+1.0);
  j=0;
  for (i=0; i < (int) number_colors; i++)
  {
    pixel.red=colors[i].red-(double) color->red;
    distance=pixel.red*pixel.red;
    if (distance > min_distance)
      continue;
    pixel.green=colors[i].green-(double) color->green;
    distance+=pixel.green*pixel.green;
    if (distance > min_distance)
      continue;
    pixel.blue=colors[i].blue-(double) color->blue;
    distance+=pixel.blue*pixel.blue;
    if (distance > min_distance)
      continue;
    min_distance=distance;
    color->pixel=colors[i].pixel;
    j=i;
  }
  (void) XAllocColor(display,colormap,&colors[j]);
  if (query_server)
    LiberateMemory((void **) &colors);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X B e s t V i s u a l I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XBestVisualInfo returns visual information for a visual that is
%  the "best" the server supports.  "Best" is defined as:
%
%    1. Restrict the visual list to those supported by the default screen.
%
%    2. If a visual type is specified, restrict the visual list to those of
%       that type.
%
%    3. If a map type is specified, choose the visual that matches the id
%       specified by the Standard Colormap.
%
%    4  From the list of visuals, choose one that can display the most
%       simultaneous colors.  If more than one visual can display the same
%       number of simultaneous colors, one is chosen based on a rank.
%
%  The format of the XBestVisualInfo method is:
%
%      XVisualInfo *XBestVisualInfo(Display *display,
%        XStandardColormap *map_info,XResourceInfo *resource_info)
%
%  A description of each parameter follows:
%
%    o visual_info: XBestVisualInfo returns a pointer to a X11 XVisualInfo
%      structure.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o map_info: If map_type is specified, this structure is initialized
%      with info from the Standard Colormap.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%
*/
MagickExport XVisualInfo *XBestVisualInfo(Display *display,
  XStandardColormap *map_info,XResourceInfo *resource_info)
{
#define MaxStandardColormaps  7
#define XVisualColormapSize(visual_info) Min( (int) (\
  (visual_info->storage_class == TrueColor) || (visual_info->storage_class == DirectColor) ? \
   visual_info->red_mask | visual_info->green_mask | visual_info->blue_mask : \
   visual_info->colormap_size),1 << visual_info->depth)

  char
    *map_type,
    *visual_type;

  long
    visual_mask;

  register int
    i;

  static int
    number_visuals;

  static XVisualInfo
    visual_template;

  XVisualInfo
    *visual_info,
    *visual_list;

  /*
    Restrict visual search by screen number.
  */
  assert(display != (Display *) NULL);
  assert(map_info != (XStandardColormap *) NULL);
  assert(resource_info != (XResourceInfo *) NULL);
  map_type=resource_info->map_type;
  visual_type=resource_info->visual_type;
  visual_mask=VisualScreenMask;
  visual_template.screen=XDefaultScreen(display);
  visual_template.depth=XDefaultDepth(display,XDefaultScreen(display));
  if (resource_info->immutable && (resource_info->colors != 0))
    if (resource_info->colors <= (1UL << visual_template.depth))
      visual_mask|=VisualDepthMask;
  if (visual_type != (char *) NULL)
    {
      /*
        Restrict visual search by class or visual id.
      */
      if (LocaleCompare("staticgray",visual_type) == 0)
        {
          visual_mask|=VisualClassMask;
          visual_template.storage_class=StaticGray;
        }
      else
        if (LocaleCompare("grayscale",visual_type) == 0)
          {
            visual_mask|=VisualClassMask;
            visual_template.storage_class=GrayScale;
          }
        else
          if (LocaleCompare("staticcolor",visual_type) == 0)
            {
              visual_mask|=VisualClassMask;
              visual_template.storage_class=StaticColor;
            }
          else
            if (LocaleCompare("pseudocolor",visual_type) == 0)
              {
                visual_mask|=VisualClassMask;
                visual_template.storage_class=PseudoColor;
              }
            else
              if (LocaleCompare("truecolor",visual_type) == 0)
                {
                  visual_mask|=VisualClassMask;
                  visual_template.storage_class=TrueColor;
                }
              else
                if (LocaleCompare("directcolor",visual_type) == 0)
                  {
                    visual_mask|=VisualClassMask;
                    visual_template.storage_class=DirectColor;
                  }
                else
                  if (LocaleCompare("default",visual_type) == 0)
                    {
                      visual_mask|=VisualIDMask;
                      visual_template.visualid=XVisualIDFromVisual(
                        XDefaultVisual(display,XDefaultScreen(display)));
                    }
                  else
                    if (isdigit((int) (*visual_type)))
                      {
                        visual_mask|=VisualIDMask;
                        visual_template.visualid=
                          strtol(visual_type,(char **) NULL,0);
                      }
                    else
                      MagickError(XServerError,"InvalidVisualSpecifier",
                        visual_type);
    }
  /*
    Get all visuals that meet our criteria so far.
  */
  number_visuals=0;
  visual_list=XGetVisualInfo(display,visual_mask,&visual_template,
    &number_visuals);
  visual_mask=VisualScreenMask | VisualIDMask;
  if ((number_visuals == 0) || (visual_list == (XVisualInfo *) NULL))
    {
      /*
        Failed to get visual;  try using the default visual.
      */
      MagickWarning(XServerWarning,"UnableToGetVisual",visual_type);
      visual_template.visualid=
        XVisualIDFromVisual(XDefaultVisual(display,XDefaultScreen(display)));
      visual_list=XGetVisualInfo(display,visual_mask,&visual_template,
        &number_visuals);
      if ((number_visuals == 0) || (visual_list == (XVisualInfo *) NULL))
        return((XVisualInfo *) NULL);
      MagickWarning(XServerWarning,"UsingDefaultVisual",
        XVisualClassName(visual_list->storage_class));
    }
  resource_info->color_recovery=False;
  if ((map_info != (XStandardColormap *) NULL) && (map_type != (char *) NULL))
    {
      Atom
        map_property;

      char
        map_name[MaxTextExtent];

      int
        j,
        number_maps,
        status;

      Window
        root_window;

      XStandardColormap
        *map_list;

      /*
        Choose a visual associated with a standard colormap.
      */
      root_window=XRootWindow(display,XDefaultScreen(display));
      status=0;
      if (LocaleCompare(map_type,"list") != 0)
        {
          /*
            User specified Standard Colormap.
          */
          FormatString((char *) map_name,"RGB_%.1024s_MAP",map_type);
          LocaleUpper(map_name);
          map_property=XInternAtom(display,(char *) map_name,True);
          if (map_property != (Atom) NULL)
            status=XGetRGBColormaps(display,root_window,&map_list,&number_maps,
              map_property);
        }
      else
        {
          static const char
            *colormap[MaxStandardColormaps]=
            {
              "_HP_RGB_SMOOTH_MAP_LIST",
              "RGB_BEST_MAP",
              "RGB_DEFAULT_MAP",
              "RGB_GRAY_MAP",
              "RGB_RED_MAP",
              "RGB_GREEN_MAP",
              "RGB_BLUE_MAP",
            };

          /*
            Choose a standard colormap from a list.
          */
          for (i=0; i < MaxStandardColormaps; i++)
          {
            map_property=XInternAtom(display,(char *) colormap[i],True);
            if (map_property == (Atom) NULL)
              continue;
            status=XGetRGBColormaps(display,root_window,&map_list,&number_maps,
              map_property);
            if (status != 0)
              break;
          }
          resource_info->color_recovery=(i == 0);  /* _HP_RGB_SMOOTH_MAP_LIST */
        }
      if (status == 0)
        {
          MagickError(XServerError,"UnableToGetStandardColormap",map_type);
          return((XVisualInfo *) NULL);
        }
      /*
        Search all Standard Colormaps and visuals for ids that match.
      */
      *map_info=map_list[0];
#if !defined(PRE_R4_ICCCM)
      visual_template.visualid=XVisualIDFromVisual(visual_list[0].visual);
      for (i=0; i < number_maps; i++)
        for (j=0; j < number_visuals; j++)
          if (map_list[i].visualid ==
              XVisualIDFromVisual(visual_list[j].visual))
            {
              *map_info=map_list[i];
              visual_template.visualid=
                XVisualIDFromVisual(visual_list[j].visual);
              break;
            }
      if (map_info->visualid != visual_template.visualid)
        {
          MagickError(XServerError,"UnableToMatchVisualToStandardColormap",
            map_type);
          return((XVisualInfo *) NULL);
        }
#endif
      if (map_info->colormap == (Colormap) NULL)
        {
          MagickError(XServerError,"StandardColormapIsNotInitialized",map_type);
          return((XVisualInfo *) NULL);
        }
      (void) XFree((void *) map_list);
    }
  else
    {
      static const unsigned int
        rank[]=
          {
            StaticGray,
            GrayScale,
            StaticColor,
            DirectColor,
            TrueColor,
            PseudoColor
          };

      XVisualInfo
        *p;

      /*
        Pick one visual that displays the most simultaneous colors.
      */
      visual_info=visual_list;
      p=visual_list;
      for (i=1; i < number_visuals; i++)
      {
        p++;
        if (XVisualColormapSize(p) > XVisualColormapSize(visual_info))
          visual_info=p;
        else
          if (XVisualColormapSize(p) == XVisualColormapSize(visual_info))
            if (rank[p->storage_class] > rank[visual_info->storage_class])
              visual_info=p;
      }
      visual_template.visualid=XVisualIDFromVisual(visual_info->visual);
    }
  (void) XFree((void *) visual_list);
  /*
    Retrieve only one visual by its screen & id number.
  */
  visual_info=XGetVisualInfo(display,visual_mask,&visual_template,
    &number_visuals);
  if ((number_visuals == 0) || (visual_info == (XVisualInfo *) NULL))
    return((XVisualInfo *) NULL);
  return(visual_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X C h e c k R e f r e s h W i n d o w s                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XCheckRefreshWindows checks the X server for exposure events for
%  a particular window and updates the area associated withe exposure event.
%
%  The format of the XCheckRefreshWindows method is:
%
%      void XCheckRefreshWindows(Display *display,XWindows *windows)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o windows: Specifies a pointer to a XWindows structure.
%
%
*/
MagickExport void XCheckRefreshWindows(Display *display,XWindows *windows)
{
  XEvent
    event;

  assert(display != (Display *) NULL);
  assert(windows != (XWindows *) NULL);
  XDelay(display,SuspendTime);
  while (XCheckTypedWindowEvent(display,windows->command.id,Expose,&event))
    (void) XCommandWidget(display,windows,(char const **) NULL,&event);
  while (XCheckTypedWindowEvent(display,windows->image.id,Expose,&event))
    XRefreshWindow(display,&windows->image,&event);
  XDelay(display,SuspendTime << 1);
  while (XCheckTypedWindowEvent(display,windows->command.id,Expose,&event))
    (void) XCommandWidget(display,windows,(char const **) NULL,&event);
  while (XCheckTypedWindowEvent(display,windows->image.id,Expose,&event))
    XRefreshWindow(display,&windows->image,&event);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X C l i e n t M e s s a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XClientMessage sends a reason to a window with XSendEvent.  The
%  reason is initialized with a particular protocol type and atom.
%
%  The format of the XClientMessage function is:
%
%      XClientMessage(display,window,protocol,reason,timestamp)
%
%  A description of each parameter follows:
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a Window structure.
%
%    o protocol: Specifies an atom value.
%
%    o reason: Specifies an atom value which is the reason to send.
%
%    o timestamp: Specifies a value of type Time.
%
%
*/
MagickExport void XClientMessage(Display *display,const Window window,
  const Atom protocol,const Atom reason,const Time timestamp)
{
  XClientMessageEvent
    client_event;

  assert(display != (Display *) NULL);
  client_event.type=ClientMessage;
  client_event.window=window;
  client_event.message_type=protocol;
  client_event.format=32;
  client_event.data.l[0]=(long) reason;
  client_event.data.l[1]=(long) timestamp;
  (void) XSendEvent(display,window,False,NoEventMask,(XEvent *) &client_event);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   X C l i e n t W i n d o w                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XClientWindow finds a window, at or below the specified window,
%  which has a WM_STATE property.  If such a window is found, it is returned,
%  otherwise the argument window is returned.
%
%  The format of the XClientWindow function is:
%
%      client_window=XClientWindow(display,target_window)
%
%  A description of each parameter follows:
%
%    o client_window: XClientWindow returns a window, at or below the specified
%      window, which has a WM_STATE property otherwise the argument
%      target_window is returned.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o target_window: Specifies the window to find a WM_STATE property.
%
%
*/
static Window XClientWindow(Display *display,Window target_window)
{
  Atom
    state,
    type;

  int
    format,
    status;

  unsigned char
    *data;

  unsigned long
    after,
    number_items;

  Window
    client_window;

  assert(display != (Display *) NULL);
  state=XInternAtom(display,"WM_STATE",True);
  if (state == (Atom) NULL)
    return(target_window);
  type=(Atom) NULL;
  status=XGetWindowProperty(display,target_window,state,0L,0L,False,
    (Atom) AnyPropertyType,&type,&format,&number_items,&after,&data);
  if ((status == Success) && (type != (Atom) NULL))
    return(target_window);
  client_window=XWindowByProperty(display,target_window,state);
  if (client_window == (Window) NULL)
    return(target_window);
  return(client_window);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X C o n f i g u r e I m a g e C o l o r m a p                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XConfigureImageColormap creates a new X colormap.
%
%  The format of the XConfigureImageColormap method is:
%
%      void XConfigureImageColormap(Display *display,
%        XResourceInfo *resource_info,XWindows *windows,Image *image)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o windows: Specifies a pointer to a XWindows structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%
*/
MagickExport void XConfigureImageColormap(Display *display,
  XResourceInfo *resource_info,XWindows *windows,Image *image)
{
  Colormap
    colormap;

  /*
    Make standard colormap.
  */
  XSetCursorState(display,windows,True);
  XCheckRefreshWindows(display,windows);
  XMakeStandardColormap(display,windows->visual_info,resource_info,image,
    windows->map_info,windows->pixel_info);
  colormap=windows->map_info->colormap;
  (void) XSetWindowColormap(display,windows->image.id,colormap);
  (void) XSetWindowColormap(display,windows->command.id,colormap);
  (void) XSetWindowColormap(display,windows->widget.id,colormap);
  if (windows->magnify.mapped)
    (void) XSetWindowColormap(display,windows->magnify.id,colormap);
  if (windows->pan.mapped)
    (void) XSetWindowColormap(display,windows->pan.id,colormap);
  XSetCursorState(display,windows,False);
  XClientMessage(display,windows->image.id,windows->im_protocols,
    windows->im_update_colormap,CurrentTime);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X C o n s t r a i n W i n d o w P o s i t i o n                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XConstrainWindowPosition assures a window is positioned witin the
%  X server boundaries.
%
%  The format of the XConstrainWindowPosition method is:
%
%      void XConstrainWindowPosition(Display *display,XWindowInfo *window_info)
%
%  A description of each parameter follows:
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o window_info: Specifies a pointer to a XWindowInfo structure.
%
%
*/
MagickExport void XConstrainWindowPosition(Display *display,
  XWindowInfo *window_info)
{
  unsigned int
    limit;

  assert(display != (Display *) NULL);
  assert(window_info != (XWindowInfo *) NULL);
  limit=XDisplayWidth(display,window_info->screen)-window_info->width;
  if (window_info->x < 0)
    window_info->x=0;
  else
    if (window_info->x > (int) limit)
      window_info->x=limit;
  limit=XDisplayHeight(display,window_info->screen)-window_info->height;
  if (window_info->y < 0)
    window_info->y=0;
  else
    if (window_info->y > (int) limit)
      window_info->y=limit;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X D e l a y                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XDelay suspends program execution for the number of milliseconds
%  specified.
%
%  The format of the Delay method is:
%
%      void XDelay(Display *display,const unsigned long milliseconds)
%
%  A description of each parameter follows:
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o milliseconds: Specifies the number of milliseconds to delay before
%      returning.
%
%
*/
MagickExport void XDelay(Display *display,const unsigned long milliseconds)
{
  assert(display != (Display *) NULL);
  (void) XFlush(display);
  if (milliseconds == 0)
    return;
#if defined(WIN32)
  Sleep(milliseconds);
#elif defined(vms)
  {
    float
      timer;

    timer=milliseconds/1000.0;
    lib$wait(&timer);
  }
#elif defined(HAVE_POLL)
  (void) poll((struct pollfd *) NULL,0,(int) milliseconds);
#elif defined(HAVE_SELECT)
  {
    struct timeval
      timer;

    timer.tv_sec=(long) milliseconds/1000;
    timer.tv_usec=(long) (milliseconds % 1000)*1000;
    (void) select(0,(XFD_SET *) NULL,(XFD_SET *) NULL,(XFD_SET *) NULL,&timer);
  }
#else
# error "Time delay method not defined."
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X D e s t r o y W i n d o w C o l o r s                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XDestroyWindowColors frees X11 color resources previously saved on
%  a window by XRetainWindowColors or programs like xsetroot.
%
%  The format of the XDestroyWindowColors method is:
%
%      void XDestroyWindowColors(Display *display,Window window)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a Window structure.
%
%
*/
MagickExport void XDestroyWindowColors(Display *display,Window window)
{
  Atom
    property,
    type;

  int
    format,
    status;

  unsigned char
    *data;

  unsigned long
    after,
    length;

  /*
    If there are previous resources on the root window, destroy them.
  */
  assert(display != (Display *) NULL);
  property=XInternAtom(display,"_XSETROOT_ID",False);
  if (property == (Atom) NULL)
    {
      MagickError(XServerError,"UnableToCreateXProperty","_XSETROOT_ID");
      return;
    }
  status=XGetWindowProperty(display,window,property,0L,1L,True,
    (Atom) AnyPropertyType,&type,&format,&length,&after,&data);
  if (status != Success)
    return;
  if ((type == XA_PIXMAP) && (format == 32) && (length == 1) && (after == 0))
    {
      (void) XKillClient(display,(XID) (*((Pixmap *) data)));
      (void) XDeleteProperty(display,window,property);
    }
  if (type != None)
    (void) XFree((void *) data);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X D i s p l a y I m a g e I n f o                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XDisplayImageInfo displays information about an X image.
%
%  The format of the XDisplayImageInfo method is:
%
%      void XDisplayImageInfo(Display *display,
%        const XResourceInfo *resource_info,XWindows *windows,Image *undo_image,
%        Image *image)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o windows: Specifies a pointer to a XWindows structure.
%
%    o undo_image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%
*/
MagickExport void XDisplayImageInfo(Display *display,
  const XResourceInfo *resource_info,XWindows *windows,Image *undo_image,
  Image *image)
{
  char
    filename[MaxTextExtent],
    *text,
    **textlist;

  FILE
    *file;

  long
    bytes;

  register long
    i;

  size_t
    length;

  unsigned int
    levels;

  unsigned long
    number_pixels;

  /*
    Write info about the X server to a file.
  */
  assert(display != (Display *) NULL);
  assert(resource_info != (XResourceInfo *) NULL);
  assert(windows != (XWindows *) NULL);
  assert(image != (Image *) NULL);
  TemporaryFilename(filename);
  file=fopen(filename,"w");
  if (file == (FILE *) NULL)
    {
      XNoticeWidget(display,windows,"Unable to display image info",filename);
      return;
    }
  if (resource_info->gamma_correct)
    if (resource_info->display_gamma != (char *) NULL)
      (void) fprintf(file,"Display\n  gamma: %.1024s\n\n",
        resource_info->display_gamma);
  /*
    Write info about the X image to a file.
  */
  (void) fprintf(file,"X\n  visual: %.1024s\n",
    XVisualClassName(windows->image.storage_class));
  (void) fprintf(file,"  depth: %d\n",windows->image.ximage->depth);
  if (windows->visual_info->colormap_size != 0)
    (void) fprintf(file,"  colormap size: %d\n",
      windows->visual_info->colormap_size);
  if (resource_info->colormap== SharedColormap)
    (void) fprintf(file,"  colormap type: Shared\n");
  else
    (void) fprintf(file,"  colormap type: Private\n");
  (void) fprintf(file,"  geometry: %dx%d\n",windows->image.ximage->width,
    windows->image.ximage->height);
  if (windows->image.crop_geometry != (char *) NULL)
    (void) fprintf(file,"  crop geometry: %.1024s\n",
      windows->image.crop_geometry);
  if (windows->image.pixmap == (Pixmap) NULL)
    (void) fprintf(file,"  type: X Image\n");
  else
    (void) fprintf(file,"  type: Pixmap\n");
  if (windows->image.shape)
    (void) fprintf(file,"  non-rectangular shape: True\n");
  else
    (void) fprintf(file,"  non-rectangular shape: False\n");
  if (windows->image.shared_memory)
    (void) fprintf(file,"  shared memory: True\n");
  else
    (void) fprintf(file,"  shared memory: False\n");
  (void) fprintf(file,"\n");
  if (resource_info->font != (char *) NULL)
    (void) fprintf(file,"Font: %.1024s\n\n",resource_info->font);
  if (resource_info->text_font != (char *) NULL)
    (void) fprintf(file,"Text font: %.1024s\n\n",resource_info->text_font);
  /*
    Write info about the undo cache to a file.
  */
  bytes=0;
  for (levels=0; undo_image != (Image *) NULL; levels++)
  {
    number_pixels=undo_image->list->columns*undo_image->list->rows;
    bytes+=number_pixels*sizeof(PixelPacket);
    undo_image=undo_image->previous;
  }
  (void) fprintf(file,"Undo Edit Cache\n  levels: %u\n",levels);
  (void) fprintf(file,"  bytes: %lumb\n",(bytes+(1 << 19)) >> 20);
  (void) fprintf(file,"  limit: %lumb\n\n",resource_info->undo_cache);
  /*
    Write info about the image to a file.
  */
  DescribeImage(image,file,True);
  (void) fclose(file);
  text=(char *) FileToBlob(filename,&length,&image->exception);
  (void) remove(filename);
  if (text == (char *) NULL)
    {
      XNoticeWidget(display,windows,"Unable to display image info",
        "MemoryAllocationFailed");
      return;
    }
  textlist=StringToList(text);
  if (textlist != (char **) NULL)
    {
      char
        title[MaxTextExtent];

      /*
        Display information about the image in the Text View widget.
      */
      (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
      FormatString(title,"Image Info: %.1024s",image->filename);
      XTextViewWidget(display,resource_info,windows,True,title,
        (char const **) textlist);
      for (i=0; textlist[i] != (char *) NULL; i++)
        LiberateMemory((void **) &textlist[i]);
      LiberateMemory((void **) &textlist);
    }
  LiberateMemory((void **) &text);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+     X D i t h e r I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XDitherImage dithers the reference image as required by the HP
%  Color Recovery algorithm.  The color values are quantized to 3 bits of red
%  and green, and 2 bits of blue (3/3/2) and can be used as indices into a
%  8-bit X standard colormap.
%
%  The format of the XDitherImage method is:
%
%      void XDitherImage(Image *image,XImage *ximage)
%
%  A description of each parameter follows:
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o ximage: Specifies a pointer to a XImage structure;  returned from
%      XCreateImage.
%
%
*/
static void XDitherImage(Image *image,XImage *ximage)
{
  static const short int
    dither_red[2][16]=
    {
      {-16,  4, -1, 11,-14,  6, -3,  9,-15,  5, -2, 10,-13,  7, -4,  8},
      { 15, -5,  0,-12, 13, -7,  2,-10, 14, -6,  1,-11, 12, -8,  3, -9}
    },
    dither_green[2][16]=
    {
      { 11,-15,  7, -3,  8,-14,  4, -2, 10,-16,  6, -4,  9,-13,  5, -1},
      {-12, 14, -8,  2, -9, 13, -5,  1,-11, 15, -7,  3,-10, 12, -6,  0}
    },
    dither_blue[2][16]=
    {
      { -3,  9,-13,  7, -1, 11,-15,  5, -4,  8,-14,  6, -2, 10,-16,  4},
      {  2,-10, 12, -8,  0,-12, 14, -6,  3, -9, 13, -7,  1,-11, 15, -5}
    };

  PixelPacket
    color;

  int
    y;

  long
    value;

  register char
    *q;

  register const PixelPacket
    *p;

  register int
    i,
    j,
    x;

  unsigned int
    scanline_pad;

  register unsigned long
    pixel;

  unsigned char
    *blue_map[2][16],
    *green_map[2][16],
    *red_map[2][16];

  /*
    Allocate and initialize dither maps.
  */
  for (i=0; i < 2; i++)
    for (j=0; j < 16; j++)
    {
      red_map[i][j]=(unsigned char *) AcquireMemory(256*sizeof(unsigned char));
      green_map[i][j]=(unsigned char *)
        AcquireMemory(256*sizeof(unsigned char));
      blue_map[i][j]=(unsigned char *) AcquireMemory(256*sizeof(unsigned char));
      if ((red_map[i][j] == (unsigned char *) NULL) ||
          (green_map[i][j] == (unsigned char *) NULL) ||
          (blue_map[i][j] == (unsigned char *) NULL))
        {
          MagickError(ResourceLimitError,"MemoryAllocationFailed",
            "unable to dither image");
          return;
        }
    }
  /*
    Initialize dither tables.
  */
  for (i=0; i < 2; i++)
    for (j=0; j < 16; j++)
      for (x=0; x < 256; x++)
      {
        value=x-16;
        if (x < 48)
          value=x/2+8;
        value+=dither_red[i][j];
        red_map[i][j][x]=(unsigned char)
          ((value < 0) ? 0 : (value > 255) ? 255 : value);
        value=x-16;
        if (x < 48)
          value=x/2+8;
        value+=dither_green[i][j];
        green_map[i][j][x]=(unsigned char)
          ((value < 0) ? 0 : (value > 255) ? 255 : value);
        value=x-32;
        if (x < 112)
          value=x/2+24;
        value+=(dither_blue[i][j] << 1);
        blue_map[i][j][x]=(unsigned char)
          ((value < 0) ? 0 : (value > 255) ? 255 : value);
      }
  /*
    Dither image.
  */
  scanline_pad=ximage->bytes_per_line-
    ((ximage->width*ximage->bits_per_pixel) >> 3);
  i=0;
  j=0;
  q=ximage->data;
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      color.red=red_map[i][j][ScaleQuantumToChar(p->red)] << 8;
      color.green=green_map[i][j][ScaleQuantumToChar(p->green)] << 8;
      color.blue=blue_map[i][j][ScaleQuantumToChar(p->blue)] << 8;
      pixel=(unsigned long) ((color.red & 0xe0) |
        ((unsigned long) (color.green & 0xe0) >> 3) |
        ((unsigned long) (color.blue & 0xc0) >> 6));
      *q++=(unsigned char) pixel;
      p++;
      j++;
      if (j == 16)
        j=0;
    }
    q+=scanline_pad;
    i++;
    if (i == 2)
      i=0;
  }
  /*
    Free allocated memory.
  */
  for (i=0; i < 2; i++)
    for (j=0; j < 16; j++)
    {
      LiberateMemory((void **) &green_map[i][j]);
      LiberateMemory((void **) &blue_map[i][j]);
      LiberateMemory((void **) &red_map[i][j]);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X D r a w I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XDrawImage draws a line on the image.
%
%  The format of the XDrawImage method is:
%
%    status=XDrawImage(display,pixel,draw_info,image)
%
%  A description of each parameter follows:
%
%    o status: Method XDrawImage returns True if the image is
%      successfully drawd with text.  False is returned is there is a
%      memory shortage.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o pixel: Specifies a pointer to a XPixelInfo structure.
%
%    o draw_info: Specifies a pointer to a XDrawInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%
*/
MagickExport unsigned int XDrawImage(Display *display,const XPixelInfo *pixel,
  XDrawInfo *draw_info,Image *image)
{
  GC
    draw_context;

  Image
    *draw_image;

  int
    x,
    y;

  Pixmap
    draw_pixmap;

  register PixelPacket
    *q;

  unsigned int
    depth,
    height,
    matte,
    width;

  Window
    root_window;

  XGCValues
    context_values;

  XImage
    *draw_ximage;

  /*
    Initialize drawd image.
  */
  assert(display != (Display *) NULL);
  assert(pixel != (XPixelInfo *) NULL);
  assert(draw_info != (XDrawInfo *) NULL);
  assert(image != (Image *) NULL);
  /*
    Initialize drawd pixmap.
  */
  root_window=XRootWindow(display,XDefaultScreen(display));
  depth=XDefaultDepth(display,XDefaultScreen(display));
  draw_pixmap=XCreatePixmap(display,root_window,draw_info->width,
    draw_info->height,(int) depth);
  if (draw_pixmap == (Pixmap) NULL)
    return(False);
  /*
    Initialize graphics info.
  */
  context_values.background=(unsigned long) (~0);
  context_values.foreground=0;
  context_values.line_width=draw_info->line_width;
  draw_context=XCreateGC(display,root_window,GCBackground | GCForeground |
    GCLineWidth,&context_values);
  if (draw_context == (GC) NULL)
    return(False);
  /*
    Clear pixmap.
  */
  (void) XFillRectangle(display,draw_pixmap,draw_context,0,0,draw_info->width,
    draw_info->height);
  /*
    Draw line to pixmap.
  */
  (void) XSetBackground(display,draw_context,0);
  (void) XSetForeground(display,draw_context,(unsigned long) (~0));
  (void) XSetFillStyle(display,draw_context,FillOpaqueStippled);
  (void) XSetStipple(display,draw_context,draw_info->stipple);
  switch (draw_info->element)
  {
    case PointElement:
    default:
    {
      (void) XDrawLines(display,draw_pixmap,draw_context,
        draw_info->coordinate_info,draw_info->number_coordinates,
        CoordModeOrigin);
      break;
    }
    case LineElement:
    {
      (void) XDrawLine(display,draw_pixmap,draw_context,draw_info->line_info.x1,
        draw_info->line_info.y1,draw_info->line_info.x2,
        draw_info->line_info.y2);
      break;
    }
    case RectangleElement:
    {
      (void) XDrawRectangle(display,draw_pixmap,draw_context,
        (int) draw_info->rectangle_info.x,(int) draw_info->rectangle_info.y,
        (unsigned int) draw_info->rectangle_info.width,
        (unsigned int) draw_info->rectangle_info.height);
      break;
    }
    case FillRectangleElement:
    {
      (void) XFillRectangle(display,draw_pixmap,draw_context,
        (int) draw_info->rectangle_info.x,(int) draw_info->rectangle_info.y,
        (unsigned int) draw_info->rectangle_info.width,
        (unsigned int) draw_info->rectangle_info.height);
      break;
    }
    case CircleElement:
    case EllipseElement:
    {
      (void) XDrawArc(display,draw_pixmap,draw_context,
        (int) draw_info->rectangle_info.x,(int) draw_info->rectangle_info.y,
        (unsigned int) draw_info->rectangle_info.width,
        (unsigned int) draw_info->rectangle_info.height,0,360*64);
      break;
    }
    case FillCircleElement:
    case FillEllipseElement:
    {
      (void) XFillArc(display,draw_pixmap,draw_context,
        (int) draw_info->rectangle_info.x,(int) draw_info->rectangle_info.y,
        (unsigned int) draw_info->rectangle_info.width,
        (unsigned int) draw_info->rectangle_info.height,0,360*64);
      break;
    }
    case PolygonElement:
    {
      XPoint
        *coordinate_info;

      coordinate_info=draw_info->coordinate_info;
      (void) XDrawLines(display,draw_pixmap,draw_context,coordinate_info,
        draw_info->number_coordinates,CoordModeOrigin);
      (void) XDrawLine(display,draw_pixmap,draw_context,
        coordinate_info[draw_info->number_coordinates-1].x,
        coordinate_info[draw_info->number_coordinates-1].y,
        coordinate_info[0].x,coordinate_info[0].y);
      break;
    }
    case FillPolygonElement:
    {
      (void) XFillPolygon(display,draw_pixmap,draw_context,
        draw_info->coordinate_info,draw_info->number_coordinates,Complex,
        CoordModeOrigin);
      break;
    }
  }
  (void) XFreeGC(display,draw_context);
  /*
    Initialize X image.
  */
  draw_ximage=XGetImage(display,draw_pixmap,0,0,draw_info->width,
    draw_info->height,AllPlanes,ZPixmap);
  if (draw_ximage == (XImage *) NULL)
    return(False);
  (void) XFreePixmap(display,draw_pixmap);
  /*
    Initialize draw image.
  */
  draw_image=AllocateImage((ImageInfo *) NULL);
  if (draw_image == (Image *) NULL)
    return(False);
  draw_image->columns=draw_info->width;
  draw_image->rows=draw_info->height;
  /*
    Transfer drawn X image to image.
  */
  width=(unsigned int) image->columns;
  height=(unsigned int) image->rows;
  x=0;
  y=0;
  (void) XParseGeometry(draw_info->geometry,&x,&y,&width,&height);
  draw_image->background_color=GetOnePixel(image,x,y);
  SetImageType(draw_image,TrueColorMatteType);
  for (y=0; y < (long) draw_image->rows; y++)
  {
    q=SetImagePixels(draw_image,0,y,draw_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) draw_image->columns; x++)
    {
      if (XGetPixel(draw_ximage,x,y) == 0)
        {
          /*
            Set this pixel to the background color.
          */
          *q=draw_image->background_color;
          q->opacity=(Quantum) (draw_info->stencil == OpaqueStencil ?
            TransparentOpacity : OpaqueOpacity);
        }
      else
        {
          /*
            Set this pixel to the pen color.
          */
          q->red=ScaleShortToQuantum(pixel->pen_color.red);
          q->green=ScaleShortToQuantum(pixel->pen_color.green);
          q->blue=ScaleShortToQuantum(pixel->pen_color.blue);
          q->opacity=(Quantum) (draw_info->stencil == OpaqueStencil ?
            OpaqueOpacity : TransparentOpacity);
        }
      q++;
    }
    if (!SyncImagePixels(draw_image))
      break;
  }
  XDestroyImage(draw_ximage);
  /*
    Determine draw geometry.
  */
  (void) XParseGeometry(draw_info->geometry,&x,&y,&width,&height);
  if ((width != (unsigned int) draw_image->columns) ||
      (height != (unsigned int) draw_image->rows))
    {
      char
        image_geometry[MaxTextExtent];

      /*
        Scale image.
      */
      FormatString(image_geometry,"%ux%u",width,height);
      TransformImage(&draw_image,(char *) NULL,image_geometry);
    }
  if (draw_info->degrees != 0.0)
    {
      double
        normalized_degrees;

      Image
        *rotate_image;

      int
        rotations;

      /*
        Rotate image.
      */
      rotate_image=RotateImage(draw_image,draw_info->degrees,&image->exception);
      if (rotate_image == (Image *) NULL)
        return(False);
      DestroyImage(draw_image);
      draw_image=rotate_image;
      /*
        Annotation is relative to the degree of rotation.
      */
      normalized_degrees=draw_info->degrees;
      while (normalized_degrees < -45.0)
        normalized_degrees+=360.0;
      for (rotations=0; normalized_degrees > 45.0; rotations++)
        normalized_degrees-=90.0;
      switch (rotations % 4)
      {
        default:
        case 0:
          break;
        case 1:
        {
          /*
            Rotate 90 degrees.
          */
          x=x-(int) draw_image->columns/2;
          y=y+(int) draw_image->columns/2;
          break;
        }
        case 2:
        {
          /*
            Rotate 180 degrees.
          */
          x=x-(int) draw_image->columns;
          break;
        }
        case 3:
        {
          /*
            Rotate 270 degrees.
          */
          x=x-(int) draw_image->columns/2;
          y=y-(int) (draw_image->rows-(draw_image->columns/2));
          break;
        }
      }
    }
  /*
    Composite text onto the image.
  */
  for (y=0; y < (long) draw_image->rows; y++)
  {
    q=GetImagePixels(draw_image,0,y,draw_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) draw_image->columns; x++)
    {
      if (q->opacity != TransparentOpacity)
        q->opacity=OpaqueOpacity;
      q++;
    }
    if (!SyncImagePixels(draw_image))
      break;
  }
  (void) XParseGeometry(draw_info->geometry,&x,&y,&width,&height);
  if (draw_info->stencil == TransparentStencil)
    (void) CompositeImage(image,CopyOpacityCompositeOp,draw_image,x,y);
  else
    {
      matte=image->matte;
      (void) CompositeImage(image,OverCompositeOp,draw_image,x,y);
      image->matte=matte;
    }
  DestroyImage(draw_image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X E r r o r                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XError ignores BadWindow errors for XQueryTree and
%  XGetWindowAttributes, and ignores BadDrawable errors for XGetGeometry, and
%  ignores BadValue errors for XQueryColor.  It returns False in those cases.
%  Otherwise it returns True.
%
%  The format of the XError function is:
%
%      XError(display,error)
%
%  A description of each parameter follows:
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o error: Specifies the error event.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

MagickExport int XError(Display *display,XErrorEvent *error)
{
  assert(display != (Display *) NULL);
  assert(error != (XErrorEvent *) NULL);
  xerror_alert=True;
  switch (error->request_code)
  {
    case X_GetGeometry:
    {
      if (error->error_code == BadDrawable)
        return(False);
      break;
    }
    case X_GetWindowAttributes:
    case X_QueryTree:
    {
      if (error->error_code == BadWindow)
        return(False);
      break;
    }
    case X_QueryColors:
    {
      if (error->error_code == BadValue)
        return(False);
      break;
    }
  }
  return(True);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X F r e e R e s o u r c e s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XFreeResources frees X11 resources.
%
%  The format of the XFreeResources method is:
%
%      void XFreeResources(Display *display,XVisualInfo *visual_info,
%        XStandardColormap *map_info,XPixelInfo *pixel,XFontStruct *font_info,
%        XResourceInfo *resource_info,XWindowInfo *window_info)
%        resource_info,window_info)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o visual_info: Specifies a pointer to a X11 XVisualInfo structure;
%      returned from XGetVisualInfo.
%
%    o map_info: If map_type is specified, this structure is initialized
%      with info from the Standard Colormap.
%
%    o pixel: Specifies a pointer to a XPixelInfo structure.
%
%    o font_info: Specifies a pointer to a XFontStruct structure.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o window_info: Specifies a pointer to a X11 XWindowInfo structure.
%
%
*/
MagickExport void XFreeResources(Display *display,XVisualInfo *visual_info,
  XStandardColormap *map_info,XPixelInfo *pixel,XFontStruct *font_info,
  XResourceInfo *resource_info,XWindowInfo *window_info)
{
  assert(display != (Display *) NULL);
  assert(resource_info != (XResourceInfo *) NULL);
  if (window_info != (XWindowInfo *) NULL)
    {
      /*
        Free X image.
      */
      if (window_info->ximage != (XImage *) NULL)
        XDestroyImage(window_info->ximage);
      if (window_info->id != (Window) NULL)
        {
          /*
            Free destroy window and free cursors.
          */
          if (window_info->id != XRootWindow(display,visual_info->screen))
            (void) XDestroyWindow(display,window_info->id);
          if (window_info->annotate_context != (GC) NULL)
            (void) XFreeGC(display,window_info->annotate_context);
          if (window_info->highlight_context != (GC) NULL)
            (void) XFreeGC(display,window_info->highlight_context);
          if (window_info->widget_context != (GC) NULL)
            (void) XFreeGC(display,window_info->widget_context);
          (void) XFreeCursor(display,window_info->cursor);
          (void) XFreeCursor(display,window_info->busy_cursor);
        }
    }
  /*
    Free font.
  */
  if (font_info != (XFontStruct *) NULL)
    (void) XFreeFont(display,font_info);
  if (map_info != (XStandardColormap *) NULL)
    {
      /*
        Free X Standard Colormap.
      */
      if (resource_info->map_type == (char *) NULL)
        (void) XFreeStandardColormap(display,visual_info,map_info,pixel);
      (void) XFree((void *) map_info);
    }
  /*
    Free X visual info.
  */
  if (visual_info != (XVisualInfo *) NULL)
    (void) XFree((void *) visual_info);
  if (resource_info->close_server)
    (void) XCloseDisplay(display);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X F r e e S t a n d a r d C o l o r m a p                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XFreeStandardColormap frees an X11 colormap.
%
%  The format of the XFreeStandardColormap method is:
%
%      void XFreeStandardColormap(Display *display,
%        const XVisualInfo *visual_info,XStandardColormap *map_info,
%        XPixelInfo *pixel)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o visual_info: Specifies a pointer to a X11 XVisualInfo structure;
%      returned from XGetVisualInfo.
%
%    o map_info: If map_type is specified, this structure is initialized
%      with info from the Standard Colormap.
%
%    o pixel: Specifies a pointer to a XPixelInfo structure.
%
%
*/
MagickExport void XFreeStandardColormap(Display *display,
  const XVisualInfo *visual_info,XStandardColormap *map_info,XPixelInfo *pixel)
{
  /*
    Free colormap.
  */
  assert(display != (Display *) NULL);
  assert(visual_info != (XVisualInfo *) NULL);
  assert(map_info != (XStandardColormap *) NULL);
  (void) XFlush(display);
  if (map_info->colormap != (Colormap) NULL)
    {
      if (map_info->colormap != XDefaultColormap(display,visual_info->screen))
        (void) XFreeColormap(display,map_info->colormap);
      else
        if (pixel != (XPixelInfo *) NULL)
          if ((visual_info->storage_class != TrueColor) &&
              (visual_info->storage_class != DirectColor))
            (void) XFreeColors(display,map_info->colormap,pixel->pixels,
              (int) pixel->colors,0);
    }
  map_info->colormap=(Colormap) NULL;
  if (pixel != (XPixelInfo *) NULL)
    {
      if (pixel->pixels != (unsigned long *) NULL)
        LiberateMemory((void **) &pixel->pixels);
      pixel->pixels=(unsigned long *) NULL;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t A n n o t a t e I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XGetAnnotateInfo initializes the AnnotateInfo structure.
%
%  The format of the XGetAnnotateInfo method is:
%
%      void XGetAnnotateInfo(XAnnotateInfo *annotate_info)
%
%  A description of each parameter follows:
%
%    o annotate_info: Specifies a pointer to a XAnnotateInfo structure.
%
%
*/
MagickExport void XGetAnnotateInfo(XAnnotateInfo *annotate_info)
{
  /*
    Initialize annotate structure.
  */
  assert(annotate_info != (XAnnotateInfo *) NULL);
  annotate_info->x=0;
  annotate_info->y=0;
  annotate_info->width=0;
  annotate_info->height=0;
  annotate_info->stencil=ForegroundStencil;
  annotate_info->degrees=0.0;
  annotate_info->font_info=(XFontStruct *) NULL;
  annotate_info->text=(char *) NULL;
  *annotate_info->geometry='\0';
  annotate_info->previous=(XAnnotateInfo *) NULL;
  annotate_info->next=(XAnnotateInfo *) NULL;
  (void) XSupportsLocale();
  (void) XSetLocaleModifiers("");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t M a p I n f o                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XGetMapInfo initializes the XStandardColormap structure.
%
%  The format of the XStandardColormap method is:
%
%      void XGetMapInfo(const XVisualInfo *visual_info,const Colormap colormap,
%        XStandardColormap *map_info)
%
%  A description of each parameter follows:
%
%    o colormap: Specifies the ID of the X server colormap.
%
%    o visual_info: Specifies a pointer to a X11 XVisualInfo structure;
%      returned from XGetVisualInfo.
%
%    o map_info: Specifies a pointer to a X11 XStandardColormap structure.
%
%
*/
MagickExport void XGetMapInfo(const XVisualInfo *visual_info,
  const Colormap colormap,XStandardColormap *map_info)
{
  /*
    Initialize map info.
  */
  assert(visual_info != (XVisualInfo *) NULL);
  assert(map_info != (XStandardColormap *) NULL);
  map_info->colormap=colormap;
  map_info->red_max=visual_info->red_mask;
  map_info->red_mult=map_info->red_max != 0 ? 1 : 0;
  if (map_info->red_max != 0)
    while ((map_info->red_max & 0x01) == 0)
    {
      map_info->red_max>>=1;
      map_info->red_mult<<=1;
    }
  map_info->green_max=visual_info->green_mask;
  map_info->green_mult=map_info->green_max != 0 ? 1 : 0;
  if (map_info->green_max != 0)
    while ((map_info->green_max & 0x01) == 0)
    {
      map_info->green_max>>=1;
      map_info->green_mult<<=1;
    }
  map_info->blue_max=visual_info->blue_mask;
  map_info->blue_mult=map_info->blue_max != 0 ? 1 : 0;
  if (map_info->blue_max != 0)
    while ((map_info->blue_max & 0x01) == 0)
    {
      map_info->blue_max>>=1;
      map_info->blue_mult<<=1;
    }
  map_info->base_pixel=0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t I m a g e I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XGetImportInfo initializes the XImportInfo structure.
%
%  The format of the XGetImportInfo method is:
%
%      void XGetImportInfo(XImportInfo *ximage_info)
%
%  A description of each parameter follows:
%
%    o ximage_info: Specifies a pointer to a ImageInfo structure.
%
%
*/
MagickExport void XGetImportInfo(XImportInfo *ximage_info)
{
  assert(ximage_info != (XImportInfo *) NULL);
  ximage_info->frame=False;
  ximage_info->borders=False;
  ximage_info->screen=False;
  ximage_info->descend=True;
  ximage_info->silent=False;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t P i x e l I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XGetPixelPacket initializes the PixelPacket structure.
%
%  The format of the XGetPixelPacket method is:
%
%      void XGetPixelPacket(Display *display,const XVisualInfo *visual_info,
%        const XStandardColormap *map_info,const XResourceInfo *resource_info,
%        Image *image,XPixelInfo *pixel)
%        pixel)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o visual_info: Specifies a pointer to a X11 XVisualInfo structure;
%      returned from XGetVisualInfo.
%
%    o map_info: If map_type is specified, this structure is initialized
%      with info from the Standard Colormap.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o pixel: Specifies a pointer to a XPixelInfo structure.
%
%
*/
MagickExport void XGetPixelPacket(Display *display,
  const XVisualInfo *visual_info,const XStandardColormap *map_info,
  const XResourceInfo *resource_info,Image *image,XPixelInfo *pixel)
{
  static const char
    *PenColors[MaxNumberPens]=
    {
      "#000000000000",  /* black */
      "#00000000ffff",  /* blue */
      "#0000ffffffff",  /* cyan */
      "#0000ffff0000",  /* green */
      "#bdbdbdbdbdbd",  /* gray */
      "#ffff00000000",  /* red */
      "#ffff0000ffff",  /* magenta */
      "#ffffffff0000",  /* yellow */
      "#ffffffffffff",  /* white */
      "#bdbdbdbdbdbd",  /* gray */
      "#bdbdbdbdbdbd"   /* gray */
    };

  Colormap
    colormap;

  int
    status;

  register long
    i;

  unsigned int
    packets;

  /*
    Initialize pixel info.
  */
  assert(display != (Display *) NULL);
  assert(visual_info != (XVisualInfo *) NULL);
  assert(map_info != (XStandardColormap *) NULL);
  assert(resource_info != (XResourceInfo *) NULL);
  assert(pixel != (XPixelInfo *) NULL);
  pixel->colors=0;
  if (image != (Image *) NULL)
    if (image->storage_class == PseudoClass)
      pixel->colors=image->colors;
  packets=Max((int) pixel->colors,visual_info->colormap_size)+MaxNumberPens;
  if (pixel->pixels != (unsigned long *) NULL)
    LiberateMemory((void **) &pixel->pixels);
  pixel->pixels=(unsigned long *) AcquireMemory(packets*sizeof(unsigned long));
  if (pixel->pixels == (unsigned long *) NULL)
    MagickFatalError(ResourceLimitFatalError,"Unable to get pixel info",
      "MemoryAllocationFailed");
  /*
    Set foreground color.
  */
  colormap=map_info->colormap;
  (void) XParseColor(display,colormap,(char *) ForegroundColor,
    &pixel->foreground_color);
  status=XParseColor(display,colormap,resource_info->foreground_color,
    &pixel->foreground_color);
  if (status == 0)
    MagickError(XServerError,"ColorIsNotKnownToServer",
      resource_info->foreground_color);
  pixel->foreground_color.pixel=
    XStandardPixel(map_info,&pixel->foreground_color);
  pixel->foreground_color.flags=DoRed | DoGreen | DoBlue;
  /*
    Set background color.
  */
  (void) XParseColor(display,colormap,"#d6d6d6d6d6d6",&pixel->background_color);
  status=XParseColor(display,colormap,resource_info->background_color,
    &pixel->background_color);
  if (status == 0)
    MagickError(XServerError,"ColorIsNotKnownToServer",
      resource_info->background_color);
  pixel->background_color.pixel=
    XStandardPixel(map_info,&pixel->background_color);
  pixel->background_color.flags=DoRed | DoGreen | DoBlue;
  /*
    Set border color.
  */
  (void) XParseColor(display,colormap,(char *) BorderColor,
    &pixel->border_color);
  status=XParseColor(display,colormap,resource_info->border_color,
    &pixel->border_color);
  if (status == 0)
    MagickError(XServerError,"ColorIsNotKnownToServer",
      resource_info->border_color);
  pixel->border_color.pixel=XStandardPixel(map_info,&pixel->border_color);
  pixel->border_color.flags=DoRed | DoGreen | DoBlue;
  /*
    Set matte color.
  */
  pixel->matte_color=pixel->background_color;
  if (resource_info->matte_color != (char *) NULL)
    {
      /*
        Matte color is specified as a X resource or command line argument.
      */
      status=XParseColor(display,colormap,resource_info->matte_color,
        &pixel->matte_color);
      if (status == 0)
        MagickError(XServerError,"ColorIsNotKnownToServer",
          resource_info->matte_color);
      pixel->matte_color.pixel=XStandardPixel(map_info,&pixel->matte_color);
      pixel->matte_color.flags=DoRed | DoGreen | DoBlue;
    }
  /*
    Set highlight color.
  */
  pixel->highlight_color.red=(unsigned short) (((double)
     pixel->matte_color.red*ScaleQuantumToShort(HighlightModulate))/65535L+
     (ScaleQuantumToShort((double) MaxRGB-HighlightModulate)));
  pixel->highlight_color.green=(unsigned short) (((double)
     pixel->matte_color.green*ScaleQuantumToShort(HighlightModulate))/65535L+
     (ScaleQuantumToShort((double) MaxRGB-HighlightModulate)));
  pixel->highlight_color.blue=(unsigned short) (((double)
     pixel->matte_color.blue*ScaleQuantumToShort(HighlightModulate))/65535L+
     (ScaleQuantumToShort((double) MaxRGB-HighlightModulate)));
  pixel->highlight_color.pixel=XStandardPixel(map_info,&pixel->highlight_color);
  pixel->highlight_color.flags=DoRed | DoGreen | DoBlue;
  /*
    Set shadow color.
  */
  pixel->shadow_color.red=(unsigned short) (((double)
    pixel->matte_color.red*ScaleQuantumToShort(ShadowModulate))/65535L);
  pixel->shadow_color.green=(unsigned short) (((double)
    pixel->matte_color.green*ScaleQuantumToShort(ShadowModulate))/65535L);
  pixel->shadow_color.blue=(unsigned short) (((double)
    pixel->matte_color.blue*ScaleQuantumToShort(ShadowModulate))/65535L);
  pixel->shadow_color.pixel=XStandardPixel(map_info,&pixel->shadow_color);
  pixel->shadow_color.flags=DoRed | DoGreen | DoBlue;
  /*
    Set depth color.
  */
  pixel->depth_color.red=(unsigned short) (((double)
    pixel->matte_color.red*ScaleQuantumToShort(DepthModulate))/65535L);
  pixel->depth_color.green=(unsigned short) (((double)
    pixel->matte_color.green*ScaleQuantumToShort(DepthModulate))/65535L);
  pixel->depth_color.blue=(unsigned short) (((double)
    pixel->matte_color.blue*ScaleQuantumToShort(DepthModulate))/65535L);
  pixel->depth_color.pixel=XStandardPixel(map_info,&pixel->depth_color);
  pixel->depth_color.flags=DoRed | DoGreen | DoBlue;
  /*
    Set trough color.
  */
  pixel->trough_color.red=(unsigned short) (((double)
    pixel->matte_color.red*ScaleQuantumToShort(TroughModulate))/65535L);
  pixel->trough_color.green=(unsigned short) (((double)
    pixel->matte_color.green*ScaleQuantumToShort(TroughModulate))/65535L);
  pixel->trough_color.blue=(unsigned short) (((double)
    pixel->matte_color.blue*ScaleQuantumToShort(TroughModulate))/65535L);
  pixel->trough_color.pixel=XStandardPixel(map_info,&pixel->trough_color);
  pixel->trough_color.flags=DoRed | DoGreen | DoBlue;
  /*
    Set pen color.
  */
  for (i=0; i < MaxNumberPens; i++)
  {
    (void) XParseColor(display,colormap,(char *) PenColors[i],
      &pixel->pen_colors[i]);
    status=XParseColor(display,colormap,resource_info->pen_colors[i],
      &pixel->pen_colors[i]);
    if (status == 0)
      MagickError(XServerError,"ColorIsNotKnownToServer",
        resource_info->pen_colors[i]);
    pixel->pen_colors[i].pixel=XStandardPixel(map_info,&pixel->pen_colors[i]);
    pixel->pen_colors[i].flags=DoRed | DoGreen | DoBlue;
  }
  pixel->box_color=pixel->background_color;
  pixel->pen_color=pixel->foreground_color;
  pixel->box_index=0;
  pixel->pen_index=1;
  if (image != (Image *) NULL)
    {
      if (resource_info->gamma_correct && (image->gamma != 0.0))
        {
          int
            count;

          /*
            Initialize map relative to display and image gamma.
          */
          count=sscanf(resource_info->display_gamma,"%lf%*[,/]%lf%*[,/]%lf",
            &red_gamma,&green_gamma,&blue_gamma);
          if (count == 1)
            {
              green_gamma=red_gamma;
              blue_gamma=red_gamma;
            }
          red_gamma*=image->gamma;
          green_gamma*=image->gamma;
          blue_gamma*=image->gamma;
        }
      if (image->storage_class == PseudoClass)
        {
          /*
            Initialize pixel array for images of type PseudoClass.
          */
          for (i=0; i < (long) image->colors; i++)
            pixel->pixels[i]=
              XGammaPixel(map_info,image->colormap+i);
          for (i=0; i < MaxNumberPens; i++)
            pixel->pixels[image->colors+i]=pixel->pen_colors[i].pixel;
          pixel->colors+=MaxNumberPens;
        }
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t R e s o u r c e C l a s s                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XGetResourceClass queries the X server for the specified resource
%  name or class.  If the resource name or class is not defined in the
%  database, the supplied default value is returned.
%
%  The format of the XGetResourceClass method is:
%
%      char *XGetResourceClass(XrmDatabase database,const char *client_name,
%        const char *keyword,char *resource_default)
%
%  A description of each parameter follows:
%
%    o value: Method XGetResourceClass returns the resource value associated
%      with the name or class.  If none is found, the supplied default value is
%      returned.
%
%    o database: Specifies a resource database; returned from
%      XrmGetStringDatabase.
%
%    o client_name:  Specifies the application name used to retrieve resource
%      info from the X server database.
%
%    o keyword: Specifies the keyword of the value being retrieved.
%
%    o resource_default: Specifies the default value to return if the query
%      fails to find the specified keyword/class.
%
%
*/
MagickExport char *XGetResourceClass(XrmDatabase database,
  const char *client_name,const char *keyword,char *resource_default)
{
  char
    resource_class[MaxTextExtent],
    resource_name[MaxTextExtent];

  int
    status;

  static char
    *resource_type;

  XrmValue
    resource_value;

  if (database == (XrmDatabase) NULL)
    return(resource_default);
  *resource_name='\0';
  *resource_class='\0';
  if (keyword != (char *) NULL)
    {
      unsigned char
        c,
        k;

      /*
        Initialize resource keyword and class.
      */
      FormatString(resource_name,"%.1024s.%.1024s",client_name,keyword);
      c=(*client_name);
      if ((c >= XK_a) && (c <= XK_z))
        c-=(XK_a-XK_A);
      else
        if ((c >= XK_agrave) && (c <= XK_odiaeresis))
          c-=(XK_agrave-XK_Agrave);
        else
          if ((c >= XK_oslash) && (c <= XK_thorn))
            c-=(XK_oslash-XK_Ooblique);
      k=(*keyword);
      if ((k >= XK_a) && (k <= XK_z))
        k-=(XK_a-XK_A);
      else
        if ((k >= XK_agrave) && (k <= XK_odiaeresis))
          k-=(XK_agrave-XK_Agrave);
        else
          if ((k >= XK_oslash) && (k <= XK_thorn))
            k-=(XK_oslash-XK_Ooblique);
      FormatString(resource_class,"%c%.1024s.%c%.1024s",c,client_name+1,k,
        keyword+1);
    }
  status=XrmGetResource(database,resource_name,resource_class,&resource_type,
    &resource_value);
  if (status == False)
    return(resource_default);
  return(resource_value.addr);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t R e s o u r c e D a t a b a s e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XGetResourceDatabase creates a new resource database and
%  initializes it.
%
%  The format of the XGetResourceDatabase method is:
%
%      XrmDatabase XGetResourceDatabase(Display *display,
%        const char *client_name)
%
%  A description of each parameter follows:
%
%    o database: Method XGetResourceDatabase returns the database after
%      it is initialized.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o client_name:  Specifies the application name used to retrieve resource
%      info from the X server database.
%
%
*/
MagickExport XrmDatabase XGetResourceDatabase(Display *display,
  const char *client_name)
{
  char
    filename[MaxTextExtent];

  register const char
    *p;

  unsigned char
    c;

  XrmDatabase
    resource_database,
    server_database;

  if (display == (Display *) NULL)
    return((XrmDatabase) NULL);
  assert(client_name != (char *) NULL);
  /*
    Initialize resource database.
  */
  XrmInitialize();
  (void) XGetDefault(display,(char *) client_name,(char *) "dummy");
  resource_database=XrmGetDatabase(display);
  /*
    Combine application database.
  */
  if (client_name != (char *) NULL)
    {
      /*
        Get basename of client.
      */
      p=client_name+(strlen(client_name)-1);
      while ((p > client_name) && (*p != '/'))
        p--;
      if (*p == '/')
        client_name=p+1;
    }
  c=(*client_name);
  if ((c >= XK_a) && (c <= XK_z))
    c-=(XK_a-XK_A);
  else
    if ((c >= XK_agrave) && (c <= XK_odiaeresis))
      c-=(XK_agrave-XK_Agrave);
    else
      if ((c >= XK_oslash) && (c <= XK_thorn))
        c-=(XK_oslash-XK_Ooblique);
#if defined(ApplicationDefaults)
  FormatString(filename,"%.1024s%c%.1024s",ApplicationDefaults,c,client_name+1);
  (void) XrmCombineFileDatabase(filename,&resource_database,False);
#endif
  if (XResourceManagerString(display) != (char *) NULL)
    {
      /*
        Combine server database.
      */
      server_database=XrmGetStringDatabase(XResourceManagerString(display));
      XrmCombineDatabase(server_database,&resource_database,False);
    }
  /*
    Merge user preferences database.
  */
#if defined(PreferencesDefaults)
  FormatString(filename,"%.1024s%.1024src",PreferencesDefaults,client_name);
  ExpandFilename(filename);
  (void) XrmCombineFileDatabase(filename,&resource_database,False);
#endif
  return(resource_database);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t R e s o u r c e I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XGetResourceInfo initializes the ResourceInfo structure.
%
%  The format of the XGetResourceInfo method is:
%
%      void XGetResourceInfo(XrmDatabase database,char *client_name,
%        XResourceInfo *resource_info)
%
%  A description of each parameter follows:
%
%    o database: Specifies a resource database; returned from
%      XrmGetStringDatabase.
%
%    o client_name:  Specifies the application name used to retrieve
%      resource info from the X server database.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%
*/
MagickExport void XGetResourceInfo(XrmDatabase database,char *client_name,
  XResourceInfo *resource_info)
{
  char
    *resource_value;

  /*
    Initialize resource info fields.
  */
  assert(resource_info != (XResourceInfo *) NULL);
  (void) memset(resource_info,0,sizeof(XResourceInfo));
  resource_info->resource_database=database;
  resource_info->image_info=CloneImageInfo((ImageInfo *) NULL);
  resource_info->quantize_info=CloneQuantizeInfo((QuantizeInfo *) NULL);
  resource_info->close_server=True;
  resource_info->client_name=client_name;
  resource_value=XGetResourceClass(database,client_name,(char *) "backdrop",
    (char *) "False");
  resource_info->backdrop=IsTrue(resource_value);
  resource_info->background_color=XGetResourceInstance(database,client_name,
    (char *) "background","#d6d6d6d6d6d6");
  resource_info->border_color=XGetResourceInstance(database,client_name,
    (char *) "borderColor",BorderColor);
  resource_value=XGetResourceClass(database,client_name,(char *) "borderWidth",
    (char *) "2");
  resource_info->border_width=atoi(resource_value);
  resource_value=XGetResourceClass(database,client_name,(char *) "colormap",
    (char *) "shared");
  resource_info->colormap=UndefinedColormap;
  if (LocaleCompare("private",resource_value) == 0)
    resource_info->colormap=PrivateColormap;
  if (LocaleCompare("shared",resource_value) == 0)
    resource_info->colormap=SharedColormap;
  if (resource_info->colormap == UndefinedColormap)
    MagickError(OptionError,"UnrecognizedColormapType",resource_value);
  resource_value=XGetResourceClass(database,client_name,
    (char *) "colorRecovery",(char *) "False");
  resource_info->color_recovery=IsTrue(resource_value);
  resource_value=XGetResourceClass(database,client_name,(char *) "confirmExit",
    (char *) "False");
  resource_info->confirm_exit=IsTrue(resource_value);
  resource_value=XGetResourceClass(database,client_name,(char *) "delay",
    (char *) "1");
  resource_info->delay=atoi(resource_value);
  resource_info->display_gamma=XGetResourceClass(database,client_name,
    (char *) "displayGamma",(char *) "2.2");
  resource_value=XGetResourceClass(database,client_name,
    (char *) "displayWarnings",(char *) "True");
  resource_info->display_warnings=IsTrue(resource_value);
  resource_info->font=XGetResourceClass(database,client_name,(char *) "font",
    (char *) "-*-helvetica-medium-r-normal--12-*-*-*-*-*-iso8859-1");
  resource_info->font=XGetResourceClass(database,client_name,
    (char *) "fontList",resource_info->font);
  resource_info->font_name[0]=XGetResourceClass(database,client_name,
    (char *) "font1",(char *) "fixed");
  resource_info->font_name[1]=XGetResourceClass(database,client_name,
    (char *) "font2",(char *) "variable");
  resource_info->font_name[2]=XGetResourceClass(database,client_name,
    (char *) "font3",(char *) "5x8");
  resource_info->font_name[3]=XGetResourceClass(database,client_name,
    (char *) "font4",(char *) "6x10");
  resource_info->font_name[4]=XGetResourceClass(database,client_name,
    (char *) "font5",(char *) "7x13bold");
  resource_info->font_name[5]=XGetResourceClass(database,client_name,
    (char *) "font6",(char *) "8x13bold");
  resource_info->font_name[6]=XGetResourceClass(database,client_name,
    (char *) "font7",(char *) "9x15bold");
  resource_info->font_name[7]=XGetResourceClass(database,client_name,
    (char *) "font8",(char *) "10x20");
  resource_info->font_name[8]=XGetResourceClass(database,client_name,
    (char *) "font9",(char *) "12x24");
  resource_info->font_name[9]=XGetResourceClass(database,client_name,
    (char *) "font0",(char *) "fixed");
  resource_info->font_name[10]=XGetResourceClass(database,client_name,
    (char *) "font0",(char *) "fixed");
  resource_info->foreground_color=XGetResourceInstance(database,client_name,
    "foreground",ForegroundColor);
  resource_value=XGetResourceClass(database,client_name,(char *) "gammaCorrect",
    (char *) "True");
  resource_info->gamma_correct=IsTrue(resource_value);
  resource_info->image_geometry=XGetResourceClass(database,client_name,
    (char *) "geometry",(char *) NULL);
  resource_value=XGetResourceClass(database,client_name,(char *) "gravity",
    (char *) "Center");
  resource_info->gravity=(-1);
  if (LocaleCompare("Forget",resource_value) == 0)
    resource_info->gravity=ForgetGravity;
  if (LocaleCompare("NorthWest",resource_value) == 0)
    resource_info->gravity=NorthWestGravity;
  if (LocaleCompare("North",resource_value) == 0)
    resource_info->gravity=NorthGravity;
  if (LocaleCompare("NorthEast",resource_value) == 0)
    resource_info->gravity=NorthEastGravity;
  if (LocaleCompare("West",resource_value) == 0)
    resource_info->gravity=WestGravity;
  if (LocaleCompare("Center",resource_value) == 0)
    resource_info->gravity=CenterGravity;
  if (LocaleCompare("East",resource_value) == 0)
    resource_info->gravity=EastGravity;
  if (LocaleCompare("SouthWest",resource_value) == 0)
    resource_info->gravity=SouthWestGravity;
  if (LocaleCompare("South",resource_value) == 0)
    resource_info->gravity=SouthGravity;
  if (LocaleCompare("SouthEast",resource_value) == 0)
    resource_info->gravity=SouthEastGravity;
  if (LocaleCompare("Static",resource_value) == 0)
    resource_info->gravity=StaticGravity;
  if (resource_info->gravity == (-1))
    {
      MagickError(OptionError,"UnrecognizedGravityType",resource_value);
      resource_info->gravity=CenterGravity;
    }
  (void) getcwd(resource_info->home_directory,MaxTextExtent-1);
  resource_info->icon_geometry=XGetResourceClass(database,client_name,
    (char *) "iconGeometry",(char *) NULL);
  resource_value=XGetResourceClass(database,client_name,(char *) "iconic",
    (char *) "False");
  resource_info->iconic=IsTrue(resource_value);
  resource_value=XGetResourceClass(database,client_name,(char *) "immutable",
    LocaleCompare(client_name,(char *) "PerlMagick") == 0 ? (char *) "True" :
    (char *) "False");
  resource_info->immutable=IsTrue(resource_value);
  resource_value=XGetResourceClass(database,client_name,(char *) "magnify",
    (char *) "3");
  resource_info->magnify=atoi(resource_value);
  resource_info->map_type=XGetResourceClass(database,client_name,(char *) "map",
    (char *) NULL);
  resource_info->matte_color=XGetResourceInstance(database,client_name,
    (char *) "mattecolor",(char *) NULL);
  resource_info->name=XGetResourceClass(database,client_name,(char *) "name",
    (char *) NULL);
  resource_info->pen_colors[0]=XGetResourceClass(database,client_name,
    (char *) "pen1",(char *) "black");
  resource_info->pen_colors[1]=XGetResourceClass(database,client_name,
    (char *) "pen2",(char *) "blue");
  resource_info->pen_colors[2]=XGetResourceClass(database,client_name,
    (char *) "pen3",(char *) "cyan");
  resource_info->pen_colors[3]=XGetResourceClass(database,client_name,
    (char *) "pen4",(char *) "green");
  resource_info->pen_colors[4]=XGetResourceClass(database,client_name,
    (char *) "pen5",(char *) "gray");
  resource_info->pen_colors[5]=XGetResourceClass(database,client_name,
    (char *) "pen6",(char *) "red");
  resource_info->pen_colors[6]=XGetResourceClass(database,client_name,
    (char *) "pen7",(char *) "magenta");
  resource_info->pen_colors[7]=XGetResourceClass(database,client_name,
    (char *) "pen8",(char *) "yellow");
  resource_info->pen_colors[8]=XGetResourceClass(database,client_name,
    (char *) "pen9",(char *) "white");
  resource_info->pen_colors[9]=XGetResourceClass(database,client_name,
    (char *) "pen0",(char *) "gray");
  resource_info->pen_colors[10]=XGetResourceClass(database,client_name,
    (char *) "pen0",(char *) "gray");
  resource_value=XGetResourceClass(database,client_name,(char *) "pause",
    (char *) "0");
  resource_info->pause=atoi(resource_value);
  resource_value=XGetResourceClass(database,client_name,(char *) "quantum",
    (char *) "1");
  resource_info->quantum=atoi(resource_value);
  resource_info->text_font=XGetResourceClass(database,client_name,(char *)
    "font",(char *) "-*-fixed-medium-r-normal-*-12-*-*-*-*-*-iso8859-1");
  resource_info->text_font=XGetResourceClass(database,client_name,
    (char *) "textFontList",resource_info->text_font);
  resource_info->title=XGetResourceClass(database,client_name,(char *) "title",
    (char *) NULL);
  resource_value=XGetResourceClass(database,client_name,(char *) "undoCache",
    (char *) "16");
  resource_info->undo_cache=atol(resource_value);
  resource_value=XGetResourceClass(database,client_name,(char *) "update",
    (char *) "False");
  resource_info->update=IsTrue(resource_value);
  resource_value=XGetResourceClass(database,client_name,(char *) "usePixmap",
    (char *) "False");
  resource_info->use_pixmap=IsTrue(resource_value);
  resource_value=XGetResourceClass(database,client_name,(char *) "sharedMemory",
    (char *) "True");
  resource_info->use_shared_memory=IsTrue(resource_value);
  resource_info->visual_type=XGetResourceClass(database,client_name,
    (char *) "visual",(char *) NULL);
  resource_info->window_group=XGetResourceClass(database,client_name,
    (char *) "windowGroup",(char *) NULL);
  resource_info->window_id=XGetResourceClass(database,client_name,
    (char *) "window",(char *) NULL);
  resource_info->write_filename=XGetResourceClass(database,client_name,
    (char *) "writeFilename",(char *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t R e s o u r c e I n s t a n c e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XGetResourceInstance queries the X server for the specified
%  resource name.  If the resource name is not defined in the database, the
%  supplied default value is returned.
%
%  The format of the XGetResourceInstance method is:
%
%      char *XGetResourceInstance(XrmDatabase database,const char *client_name,
%        const char *keyword,const char *resource_default)
%
%  A description of each parameter follows:
%
%    o value: Method XGetResourceInstance returns the resource value
%      associated with the name or class.  If none is found, the supplied
%      default value is returned.
%
%    o database: Specifies a resource database; returned from
%      XrmGetStringDatabase.
%
%    o client_name:  Specifies the application name used to retrieve
%      resource info from the X server database.
%
%    o keyword: Specifies the keyword of the value being retrieved.
%
%    o resource_default: Specifies the default value to return if the query
%      fails to find the specified keyword/class.
%
%
*/
MagickExport char *XGetResourceInstance(XrmDatabase database,
  const char *client_name,const char *keyword,const char *resource_default)
{
  char
    *resource_type,
    resource_name[MaxTextExtent];

  int
    status;

  XrmValue
    resource_value;

  if (database == (XrmDatabase) NULL)
    return((char *) resource_default);
  *resource_name='\0';
  if (keyword != (char *) NULL)
    FormatString(resource_name,"%.1024s.%.1024s",client_name,keyword);
  status=XrmGetResource(database,resource_name,"ImageMagick",&resource_type,
    &resource_value);
  if (status == False)
    return((char *) resource_default);
  return(resource_value.addr);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t S c r e e n D e n s i t y                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XGetScreenDensity returns the density of the X server screen in
%  dots-per-inch.
%
%  The format of the XGetScreenDensity method is:
%
%      char *XGetScreenDensity(Display *display)
%
%  A description of each parameter follows:
%
%    o density: Method XGetScreenDensity returns the density of the X screen
%      in dots-per-inch.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%
*/
MagickExport char *XGetScreenDensity(Display *display)
{
  char
    density[MaxTextExtent];

  double
    x_density,
    y_density;

  /*
    Set density as determined by screen size.
  */
  x_density=((((double) DisplayWidth(display,XDefaultScreen(display)))*25.4)/
    ((double) DisplayWidthMM(display,XDefaultScreen(display))));
  y_density=((((double) DisplayHeight(display,XDefaultScreen(display)))*25.4)/
    ((double) DisplayHeightMM(display,XDefaultScreen(display))));
  FormatString(density,"%gx%g",x_density,y_density);
  return(GetPageGeometry(density));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   X G e t S u b w i n d o w                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XGetSubwindow returns the subwindow of a window chosen the
%  user with the pointer and a button press.
%
%  The format of the XGetSubwindow method is:
%
%      Window XGetSubwindow(Display *display,Window window,int x,int y)
%
%  A description of each parameter follows:
%
%    o subwindow: Method XGetSubwindow returns NULL if no subwindow is found
%      otherwise the subwindow is returned.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a Window.
%
%    o x: the x coordinate of the pointer relative to the origin of the
%      window.
%
%    o y: the y coordinate of the pointer relative to the origin of the
%      window.
%
%
*/
static Window XGetSubwindow(Display *display,Window window,int x,int y)
{
  Window
    source_window,
    target_window;

  int
    status,
    x_offset,
    y_offset;

  assert(display != (Display *) NULL);
  source_window=XRootWindow(display,XDefaultScreen(display));
  if (window == (Window) NULL)
    return(source_window);
  target_window=window;
  for ( ; ; )
  {
    status=XTranslateCoordinates(display,source_window,window,x,y,
      &x_offset,&y_offset,&target_window);
    if (status != True)
      break;
    if (target_window == (Window) NULL)
      break;
    source_window=window;
    window=target_window;
    x=x_offset;
    y=y_offset;
  }
  if (target_window == (Window) NULL)
    target_window=window;
  return(target_window);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t W i n d o w C o l o r                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XGetWindowColor returns the color of a pixel interactively chosen
%  from the X server.
%
%  The format of the XGetWindowColor method is:
%
%      unsigned int XGetWindowColor(Display *display,XWindows *windows,
%        char *name)
%
%  A description of each parameter follows:
%
%    o status: Method XGetWindowColor returns True if the color is obtained
%      from the X server.  False is returned if any errors occurs.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o windows: Specifies a pointer to a XWindows structure.
%
%    o name: The name of of the color if found in the X Color Database is
%      returned in this character string.
%
%
*/
MagickExport unsigned int XGetWindowColor(Display *display,XWindows *windows,
  char *name)
{
  int
    x,
    y;

  PixelPacket
    pixel;

  RectangleInfo
    crop_info;

  unsigned int
    status;

  Window
    child,
    client_window,
    root_window,
    target_window;

  XColor
    color;

  XImage
    *ximage;

  XWindowAttributes
    window_attributes;

  /*
    Choose a pixel from the X server.
  */
  assert(display != (Display *) NULL);
  assert(name != (char *) NULL);
  *name='\0';
  target_window=XSelectWindow(display,&crop_info);
  root_window=XRootWindow(display,XDefaultScreen(display));
  client_window=target_window;
  if (target_window != root_window)
    {
      unsigned int
        d;

      /*
        Get client window.
      */
      status=XGetGeometry(display,target_window,&root_window,&x,&x,&d,&d,&d,&d);
      if (status != 0)
        {
          client_window=XClientWindow(display,target_window);
          target_window=client_window;
        }
    }
  /*
    Verify window is viewable.
  */
  status=XGetWindowAttributes(display,target_window,&window_attributes);
  if ((status == False) || (window_attributes.map_state != IsViewable))
    return(False);
  /*
    Get window X image.
  */
  (void) XTranslateCoordinates(display,root_window,target_window,
    (int) crop_info.x,(int) crop_info.y,&x,&y,&child);
  ximage=XGetImage(display,target_window,x,y,1,1,AllPlanes,ZPixmap);
  if (ximage == (XImage *) NULL)
    return(False);
  color.pixel=XGetPixel(ximage,0,0);
  XDestroyImage(ximage);
  /*
    Match color against the color database.
  */
  (void) XQueryColor(display,window_attributes.colormap,&color);
  pixel.red=ScaleShortToQuantum(color.red);
  pixel.green=ScaleShortToQuantum(color.green);
  pixel.blue=ScaleShortToQuantum(color.blue);
  pixel.opacity=OpaqueOpacity;
  (void) QueryColorname(windows->image.image,&pixel,X11Compliance,name,
    &windows->image.image->exception);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   X G e t W i n d o w I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XGetWindowImage reads an image from the target X window and returns
%  it.  XGetWindowImage optionally descends the window hierarchy and overlays
%  the target image with each child image in an optimized fashion.  Any child
%  window that have the same visual, colormap, and are contained by its
%  parent are exempted.
%
%  The format of the XGetWindowImage method is:
%
%      Image *XGetWindowImage(Display *display,const Window window,
%        const unsigned int borders,const unsigned int level)
%
%  A description of each parameter follows:
%
%    o image: Method XGetWindowImage returns a MIFF image if it can be
%      successfully read from the X window.  A null image is returned if
%      any errors occurs.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o window: Specifies the window to obtain the image from.
%
%    o borders: Specifies whether borders pixels are to be saved with
%      the image.
%
%    o level: Specifies an unsigned integer representing the level of
%      decent in the window hierarchy.  This value must be zero or one on
%      the initial call to XGetWindowImage.  A value of zero returns after
%      one call.  A value of one causes the function to descend the window
%      hierarchy and overlay the target image with each subwindow image.
%
%
*/
static Image *XGetWindowImage(Display *display,const Window window,
  const unsigned int borders,const unsigned int level)
{
  typedef struct _ColormapInfo
  {
    Colormap
      colormap;

    XColor
      *colors;

    struct _ColormapInfo
      *next;
  } ColormapInfo;

  typedef struct _WindowInfo
  {
    Window
      window,
      parent;

    Visual
      *visual;

    Colormap
      colormap;

    XSegment
      bounds;

    RectangleInfo
      crop_info;
  } WindowInfo;

  IndexPacket
    index;

  int
    display_height,
    display_width,
    id,
    status,
    x_offset,
    y_offset;

  RectangleInfo
    crop_info;

  register IndexPacket
    *indexes;

  register int
    i;

  static ColormapInfo
    *colormap_info = (ColormapInfo *) NULL;

  static int
    max_windows = 0,
    number_windows = 0;

  static WindowInfo
    *window_info;

  Window
    child,
    root_window;

  XWindowAttributes
    window_attributes;

  /*
    Verify window is viewable.
  */
  assert(display != (Display *) NULL);
  status=XGetWindowAttributes(display,window,&window_attributes);
  if ((status == False) || (window_attributes.map_state != IsViewable))
    return((Image *) NULL);
  /*
    Cropping rectangle is relative to root window.
  */
  root_window=XRootWindow(display,XDefaultScreen(display));
  (void) XTranslateCoordinates(display,window,root_window,0,0,&x_offset,
    &y_offset,&child);
  crop_info.x=x_offset;
  crop_info.y=y_offset;
  crop_info.width=window_attributes.width;
  crop_info.height=window_attributes.height;
  if (borders)
    {
      /*
        Include border in image.
      */
      crop_info.x-=window_attributes.border_width;
      crop_info.y-=window_attributes.border_width;
      crop_info.width+=window_attributes.border_width << 1;
      crop_info.height+=window_attributes.border_width << 1;
    }
  /*
    Crop to root window.
  */
  if (crop_info.x < 0)
    {
      crop_info.width+=crop_info.x;
      crop_info.x=0;
    }
  if (crop_info.y < 0)
    {
      crop_info.height+=crop_info.y;
      crop_info.y=0;
    }
  display_width=XDisplayWidth(display,XDefaultScreen(display));
  if ((int) (crop_info.x+crop_info.width) > display_width)
    crop_info.width=display_width-crop_info.x;
  display_height=XDisplayHeight(display,XDefaultScreen(display));
  if ((int) (crop_info.y+crop_info.height) > display_height)
    crop_info.height=display_height-crop_info.y;
  /*
    Initialize window info attributes.
  */
  if (number_windows >= max_windows)
    {
      /*
        Allocate or resize window info buffer.
      */
      max_windows+=1024;
      if (window_info == (WindowInfo *) NULL)
        window_info=(WindowInfo *)
          AcquireMemory(max_windows*sizeof(WindowInfo));
      else
        ReacquireMemory((void **) &window_info,max_windows*sizeof(WindowInfo));
    }
  if (window_info == (WindowInfo *) NULL)
    {
      MagickError(ResourceLimitError,"MemoryAllocationFailed",
        "unable to read X image");
      return((Image *) NULL);
    }
  id=number_windows++;
  window_info[id].window=window;
  window_info[id].visual=window_attributes.visual;
  window_info[id].colormap=window_attributes.colormap;
  window_info[id].bounds.x1=(short) crop_info.x;
  window_info[id].bounds.y1=(short) crop_info.y;
  window_info[id].bounds.x2=(short) (crop_info.x+(int) crop_info.width-1);
  window_info[id].bounds.y2=(short) (crop_info.y+(int) crop_info.height-1);
  crop_info.x-=x_offset;
  crop_info.y-=y_offset;
  window_info[id].crop_info=crop_info;
  if (level != 0)
    {
      unsigned int
        number_children;

      Window
        *children;

      /*
        Descend the window hierarchy.
      */
      status=XQueryTree(display,window,&root_window,&window_info[id].parent,
        &children,&number_children);
      for (i=0; i < id; i++)
        if ((window_info[i].window == window_info[id].parent) &&
            (window_info[i].visual == window_info[id].visual) &&
            (window_info[i].colormap == window_info[id].colormap))
          if ((window_info[id].bounds.x1 <= window_info[i].bounds.x1) ||
              (window_info[id].bounds.x1 >= window_info[i].bounds.x2) ||
              (window_info[id].bounds.y1 <= window_info[i].bounds.y1) ||
              (window_info[id].bounds.y1 >= window_info[i].bounds.y2))
            {
              /*
                Eliminate windows not circumscribed by their parent.
              */
              number_windows--;
              break;
            }
      if ((status == True) && (number_children != 0))
        {
          for (i=0; i < (int) number_children; i++)
            (void) XGetWindowImage(display,children[i],False,level+1);
          (void) XFree((void *) children);
        }
    }
  if (level <= 1)
    {
      ColormapInfo
        *next;

      Image
        *composite_image,
        *image;

      int
        y;

      register int
        j,
        x;

      register PixelPacket
        *q;

      register unsigned long
        pixel;

      unsigned int
        import,
        number_colors;

      XColor
        *colors;

      XImage
        *ximage;

      /*
        Get X image for each window in the list.
      */
      image=(Image *) NULL;
      for (id=0; id < number_windows; id++)
      {
        /*
          Does target window intersect top level window?
        */
        import=(window_info[id].bounds.x2 >= window_info[0].bounds.x1) &&
          (window_info[id].bounds.x1 <= window_info[0].bounds.x2) &&
          (window_info[id].bounds.y2 >= window_info[0].bounds.y1) &&
          (window_info[id].bounds.y1 <= window_info[0].bounds.y2);
        /*
          Is target window contained by another window with the same colormap?
        */
        for (j=0; j < id; j++)
          if ((window_info[id].visual == window_info[j].visual) &&
              (window_info[id].colormap == window_info[j].colormap) &&
              (window_info[id].bounds.x1 >= window_info[j].bounds.x1) &&
              (window_info[id].bounds.y1 >= window_info[j].bounds.y1) &&
              (window_info[id].bounds.x2 <= window_info[j].bounds.x2) &&
              (window_info[id].bounds.y2 <= window_info[j].bounds.y2))
            import=False;
          else
            if ((window_info[id].visual != window_info[j].visual) ||
                (window_info[id].colormap != window_info[j].colormap))
              if ((window_info[id].bounds.x2 > window_info[j].bounds.x1) &&
                  (window_info[id].bounds.x1 < window_info[j].bounds.x2) &&
                  (window_info[id].bounds.y2 > window_info[j].bounds.y1) &&
                  (window_info[id].bounds.y1 < window_info[j].bounds.y2))
                import=True;
        if (!import)
          continue;
        /*
          Get X image.
        */
        ximage=XGetImage(display,window_info[id].window,
          (int) window_info[id].crop_info.x,(int) window_info[id].crop_info.y,
          (unsigned int) window_info[id].crop_info.width,
          (unsigned int) window_info[id].crop_info.height,AllPlanes,ZPixmap);
        if (ximage == (XImage *) NULL)
          continue;
        /*
          Initialize window colormap.
        */
        number_colors=0;
        colors=(XColor *) NULL;
        if (window_info[id].colormap != (Colormap) NULL)
          {
            ColormapInfo
              *p;

            /*
              Search colormap list for window colormap.
            */
            number_colors=window_info[id].visual->map_entries;
            for (p=colormap_info; p != (ColormapInfo *) NULL; p=p->next)
              if (p->colormap == window_info[id].colormap)
                break;
            if (p == (ColormapInfo *) NULL)
              {
                /*
                  Get the window colormap.
                */
                colors=(XColor *) AcquireMemory(number_colors*sizeof(XColor));
                if (colors == (XColor *) NULL)
                  {
                    XDestroyImage(ximage);
                    return((Image *) NULL);
                  }
                if ((window_info[id].visual->storage_class != DirectColor) &&
                    (window_info[id].visual->storage_class != TrueColor))
                  for (i=0; i < (int) number_colors; i++)
                  {
                    colors[i].pixel=i;
                    colors[i].pad=0;
                  }
                else
                  {
                    unsigned long
                      blue,
                      blue_bit,
                      green,
                      green_bit,
                      red,
                      red_bit;

                    /*
                      DirectColor or TrueColor visual.
                    */
                    red=0;
                    green=0;
                    blue=0;
                    red_bit=window_info[id].visual->red_mask &
                      (~(window_info[id].visual->red_mask)+1);
                    green_bit=window_info[id].visual->green_mask &
                      (~(window_info[id].visual->green_mask)+1);
                    blue_bit=window_info[id].visual->blue_mask &
                      (~(window_info[id].visual->blue_mask)+1);
                    for (i=0; i < (int) number_colors; i++)
                    {
                      colors[i].pixel=red | green | blue;
                      colors[i].pad=0;
                      red+=red_bit;
                      if (red > window_info[id].visual->red_mask)
                        red=0;
                      green+=green_bit;
                      if (green > window_info[id].visual->green_mask)
                        green=0;
                      blue+=blue_bit;
                      if (blue > window_info[id].visual->blue_mask)
                        blue=0;
                    }
                  }
                (void) XQueryColors(display,window_info[id].colormap,colors,
                 (int) number_colors);
                /*
                  Append colormap to colormap list.
                */
                p=(ColormapInfo *) AcquireMemory(sizeof(ColormapInfo));
                if (p == (ColormapInfo *) NULL)
                  return((Image *) NULL);
                p->colormap=window_info[id].colormap;
                p->colors=colors;
                p->next=colormap_info;
                colormap_info=p;
              }
            colors=p->colors;
          }
        /*
          Allocate image structure.
        */
        composite_image=AllocateImage((ImageInfo *) NULL);
        if (composite_image == (Image *) NULL)
          {
            XDestroyImage(ximage);
            return((Image *) NULL);
          }
        /*
          Convert X image to MIFF format.
        */
        if ((window_info[id].visual->storage_class != TrueColor) &&
            (window_info[id].visual->storage_class != DirectColor))
          composite_image->storage_class=PseudoClass;
        composite_image->columns=ximage->width;
        composite_image->rows=ximage->height;
        switch (composite_image->storage_class)
        {
          case DirectClass:
          default:
          {
            register unsigned long
              color,
              index;

            unsigned long
              blue_mask,
              blue_shift,
              green_mask,
              green_shift,
              red_mask,
              red_shift;

            /*
              Determine shift and mask for red, green, and blue.
            */
            red_mask=window_info[id].visual->red_mask;
            red_shift=0;
            while ((red_mask & 0x01) == 0)
            {
              red_mask>>=1;
              red_shift++;
            }
            green_mask=window_info[id].visual->green_mask;
            green_shift=0;
            while ((green_mask & 0x01) == 0)
            {
              green_mask>>=1;
              green_shift++;
            }
            blue_mask=window_info[id].visual->blue_mask;
            blue_shift=0;
            while ((blue_mask & 0x01) == 0)
            {
              blue_mask>>=1;
              blue_shift++;
            }
            /*
              Convert X image to DirectClass packets.
            */
            if ((number_colors != 0) &&
                (window_info[id].visual->storage_class == DirectColor))
              for (y=0; y < (long) composite_image->rows; y++)
              {
                q=SetImagePixels(composite_image,0,y,
                  composite_image->columns,1);
                if (q == (PixelPacket *) NULL)
                  break;
                for (x=0; x < (long) composite_image->columns; x++)
                {
                  pixel=XGetPixel(ximage,x,y);
                  index=(pixel >> red_shift) & red_mask;
                  q->red=ScaleShortToQuantum(colors[index].red);
                  index=(pixel >> green_shift) & green_mask;
                  q->green=ScaleShortToQuantum(colors[index].green);
                  index=(pixel >> blue_shift) & blue_mask;
                  q->blue=ScaleShortToQuantum(colors[index].blue);
                  q++;
                }
                if (!SyncImagePixels(composite_image))
                  break;
              }
            else
              for (y=0; y < (long) composite_image->rows; y++)
              {
                q=SetImagePixels(composite_image,0,y,
                  composite_image->columns,1);
                if (q == (PixelPacket *) NULL)
                  break;
                for (x=0; x < (long) composite_image->columns; x++)
                {
                  pixel=XGetPixel(ximage,x,y);
                  color=(pixel >> red_shift) & red_mask;
                  q->red=ScaleShortToQuantum((65535L*color)/red_mask);
                  color=(pixel >> green_shift) & green_mask;
                  q->green=ScaleShortToQuantum((65535L*color)/green_mask);
                  color=(pixel >> blue_shift) & blue_mask;
                  q->blue=ScaleShortToQuantum((65535L*color)/blue_mask);
                  q++;
                }
                if (!SyncImagePixels(composite_image))
                  break;
              }
            break;
          }
          case PseudoClass:
          {
            /*
              Create colormap.
            */
            if (!AllocateImageColormap(composite_image,number_colors))
              {
                XDestroyImage(ximage);
                DestroyImage(composite_image);
                return((Image *) NULL);
              }
            for (i=0; i < (int) composite_image->colors; i++)
            {
              composite_image->colormap[colors[i].pixel].red=
                ScaleShortToQuantum(colors[i].red);
              composite_image->colormap[colors[i].pixel].green=
                ScaleShortToQuantum(colors[i].green);
              composite_image->colormap[colors[i].pixel].blue=
                ScaleShortToQuantum(colors[i].blue);
            }
            /*
              Convert X image to PseudoClass packets.
            */
            for (y=0; y < (long) composite_image->rows; y++)
            {
              q=SetImagePixels(composite_image,0,y,composite_image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=GetIndexes(composite_image);
              for (x=0; x < (long) composite_image->columns; x++)
              {
                index=(IndexPacket) XGetPixel(ximage,x,y);
                indexes[x]=index;
                *q++=composite_image->colormap[index];
              }
              if (!SyncImagePixels(composite_image))
                break;
            }
            break;
          }
        }
        XDestroyImage(ximage);
        if (image == (Image *) NULL)
          {
            image=composite_image;
            continue;
          }
        /*
          Composite any children in back-to-front order.
        */
        (void) XTranslateCoordinates(display,window_info[id].window,window,0,0,
          &x_offset,&y_offset,&child);
        x_offset-=(int) crop_info.x;
        if (x_offset < 0)
          x_offset=0;
        y_offset-=(int) crop_info.y;
        if (y_offset < 0)
          y_offset=0;
        (void) CompositeImage(image,CopyCompositeOp,composite_image,
          x_offset,y_offset);
      }
      /*
        Free resources.
      */
      while (colormap_info != (ColormapInfo *) NULL)
      {
        next=colormap_info->next;
        LiberateMemory((void **) &colormap_info->colors);
        LiberateMemory((void **) &colormap_info);
        colormap_info=next;
      }
      /*
        Free resources and restore initial state.
      */
      LiberateMemory((void **) &window_info);
      window_info=(WindowInfo *) NULL;
      max_windows=0;
      number_windows=0;
      colormap_info=(ColormapInfo *) NULL;
      return(image);
    }
  return((Image *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t W i n d o w I n f o                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XGetWindowInfo initializes the XWindowInfo structure.
%
%  The format of the XGetWindowInfo method is:
%
%      void XGetWindowInfo(Display *display,XVisualInfo *visual_info,
%        XStandardColormap *map_info,XPixelInfo *pixel,XFontStruct *font_info,
%        XResourceInfo *resource_info,XWindowInfo *window)
%        resource_info,window)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o visual_info: Specifies a pointer to a X11 XVisualInfo structure;
%      returned from XGetVisualInfo.
%
%    o map_info: If map_type is specified, this structure is initialized
%      with info from the Standard Colormap.
%
%    o pixel: Specifies a pointer to a XPixelInfo structure.
%
%    o font_info: Specifies a pointer to a XFontStruct structure.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%
*/
MagickExport void XGetWindowInfo(Display *display,XVisualInfo *visual_info,
  XStandardColormap *map_info,XPixelInfo *pixel,XFontStruct *font_info,
  XResourceInfo *resource_info,XWindowInfo *window)
{
  /*
    Initialize window info.
  */
  assert(display != (Display *) NULL);
  assert(visual_info != (XVisualInfo *) NULL);
  assert(map_info != (XStandardColormap *) NULL);
  assert(pixel != (XPixelInfo *) NULL);
  assert(resource_info != (XResourceInfo *) NULL);
  assert(window != (XWindowInfo *) NULL);
  if (window->id != (Window) NULL)
    {
      (void) XFreeCursor(display,window->cursor);
      (void) XFreeCursor(display,window->busy_cursor);
      if (window->highlight_stipple != (Pixmap) NULL)
        (void) XFreePixmap(display,window->highlight_stipple);
      if (window->shadow_stipple != (Pixmap) NULL)
        (void) XFreePixmap(display,window->shadow_stipple);
    }
  else
    {
      /*
        Initialize these attributes just once.
      */
      window->id=(Window) NULL;
      window->x=XDisplayWidth(display,visual_info->screen) >> 1;
      window->y=XDisplayWidth(display,visual_info->screen) >> 1;
      window->ximage=(XImage *) NULL;
      window->matte_image=(XImage *) NULL;
      window->pixmap=(Pixmap) NULL;
      window->matte_pixmap=(Pixmap) NULL;
      window->mapped=False;
      window->stasis=False;
      window->shared_memory=True;
#if defined(HasSharedMemory)
      window->segment_info[0].shmid=(-1);
      window->segment_info[1].shmid=(-1);
#endif
    }
  /*
    Initialize these attributes every time function is called.
  */
  window->screen=visual_info->screen;
  window->root=XRootWindow(display,visual_info->screen);
  window->visual=visual_info->visual;
  window->storage_class=visual_info->storage_class;
  window->depth=visual_info->depth;
  window->visual_info=visual_info;
  window->map_info=map_info;
  window->pixel_info=pixel;
  window->font_info=font_info;
  window->cursor=XCreateFontCursor(display,XC_left_ptr);
  window->busy_cursor=XCreateFontCursor(display,XC_watch);
  window->name=(char *) "\0";
  window->geometry=(char *) NULL;
  window->icon_name=(char *) "\0";
  window->icon_geometry=resource_info->icon_geometry;
  window->crop_geometry=(char *) NULL;
  window->flags=PSize;
  window->width=1;
  window->height=1;
  window->min_width=1;
  window->min_height=1;
  window->width_inc=1;
  window->height_inc=1;
  window->border_width=resource_info->border_width;
  window->annotate_context=pixel->annotate_context;
  window->highlight_context=pixel->highlight_context;
  window->widget_context=pixel->widget_context;
  window->shadow_stipple=(Pixmap) NULL;
  window->highlight_stipple=(Pixmap) NULL;
  window->use_pixmap=True;
  window->immutable=False;
  window->shape=False;
  window->data=0;
  window->mask=CWBackingStore | CWBackPixel | CWBackPixmap | CWBitGravity |
    CWBorderPixel | CWColormap | CWCursor | CWDontPropagate | CWEventMask |
    CWOverrideRedirect | CWSaveUnder | CWWinGravity;
  window->attributes.background_pixel=pixel->background_color.pixel;
  window->attributes.background_pixmap=(Pixmap) NULL;
  window->attributes.bit_gravity=ForgetGravity;
  window->attributes.backing_store=NotUseful;
  window->attributes.save_under=False;
  window->attributes.border_pixel=pixel->border_color.pixel;
  window->attributes.colormap=map_info->colormap;
  window->attributes.cursor=window->cursor;
  window->attributes.do_not_propagate_mask=NoEventMask;
  window->attributes.event_mask=NoEventMask;
  window->attributes.override_redirect=False;
  window->attributes.win_gravity=NorthWestGravity;
  window->orphan=False;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X H i g h l i g h t E l l i p s e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XHighlightEllipse puts a border on the X server around a region
%  defined by highlight_info.
%
%  The format of the XHighlightEllipse method is:
%
%      void XHighlightEllipse(Display *display,Window window,
%        GC annotate_context,const RectangleInfo *highlight_info)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a Window structure.
%
%    o annotate_context: Specifies a pointer to a GC structure.
%
%    o highlight_info: Specifies a pointer to a RectangleInfo structure.  It
%      contains the extents of any highlighting rectangle.
%
%
*/
MagickExport void XHighlightEllipse(Display *display,Window window,
  GC annotate_context,const RectangleInfo *highlight_info)
{
  assert(display != (Display *) NULL);
  assert(window != (Window) NULL);
  assert(annotate_context != (GC) NULL);
  assert(highlight_info != (RectangleInfo *) NULL);
  if ((highlight_info->width < 4) || (highlight_info->height < 4))
    return;
  (void) XDrawArc(display,window,annotate_context,(int) highlight_info->x,
    (int) highlight_info->y,(unsigned int) highlight_info->width-1,
    (unsigned int) highlight_info->height-1,0,360*64);
  (void) XDrawArc(display,window,annotate_context,(int) highlight_info->x+1,
    (int) highlight_info->y+1,(unsigned int) highlight_info->width-3,
    (unsigned int) highlight_info->height-3,0,360*64);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X H i g h l i g h t L i n e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XHighlightLine puts a border on the X server around a region
%  defined by highlight_info.
%
%  The format of the XHighlightLine method is:
%
%      void XHighlightLine(Display *display,Window window,GC annotate_context,
%        const XSegment *highlight_info)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a Window structure.
%
%    o annotate_context: Specifies a pointer to a GC structure.
%
%    o highlight_info: Specifies a pointer to a RectangleInfo structure.  It
%      contains the extents of any highlighting rectangle.
%
%
*/
MagickExport void XHighlightLine(Display *display,Window window,
  GC annotate_context,const XSegment *highlight_info)
{
  assert(display != (Display *) NULL);
  assert(window != (Window) NULL);
  assert(annotate_context != (GC) NULL);
  assert(highlight_info != (XSegment *) NULL);
  (void) XDrawLine(display,window,annotate_context,highlight_info->x1,
    highlight_info->y1,highlight_info->x2,highlight_info->y2);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X H i g h l i g h t R e c t a n g l e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XHighlightRectangle puts a border on the X server around a region
%  defined by highlight_info.
%
%  The format of the XHighlightRectangle method is:
%
%      void XHighlightRectangle(Display *display,Window window,
%        GC annotate_context,const RectangleInfo *highlight_info)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a Window structure.
%
%    o annotate_context: Specifies a pointer to a GC structure.
%
%    o highlight_info: Specifies a pointer to a RectangleInfo structure.  It
%      contains the extents of any highlighting rectangle.
%
%
*/
MagickExport void XHighlightRectangle(Display *display,Window window,
  GC annotate_context,const RectangleInfo *highlight_info)
{
  assert(display != (Display *) NULL);
  assert(window != (Window) NULL);
  assert(annotate_context != (GC) NULL);
  assert(highlight_info != (RectangleInfo *) NULL);
  if ((highlight_info->width < 4) || (highlight_info->height < 4))
    return;
  (void) XDrawRectangle(display,window,annotate_context,(int) highlight_info->x,
    (int) highlight_info->y,(unsigned int) highlight_info->width-1,
    (unsigned int) highlight_info->height-1);
  (void) XDrawRectangle(display,window,annotate_context,(int) highlight_info->x+
    1,(int) highlight_info->y+1,(unsigned int) highlight_info->width-3,
    (unsigned int) highlight_info->height-3);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X I m p o r t I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure XImportImage reads an image from an X window.
%
%  The format of the XImportImage method is:
%
%      Image *XImportImage(const ImageInfo *image_info,XImportInfo *ximage_info)
%
%  A description of each parameter follows:
%
%    o image_info: The image info..
%
%    o ximage_info: Specifies a pointer to an XImportInfo structure.
%
%
*/
MagickExport Image *XImportImage(const ImageInfo *image_info,
  XImportInfo *ximage_info)
{
  Colormap
    *colormaps;

  Display
    *display;

  Image
    *image;

  int
    number_colormaps,
    number_windows,
    status,
    x;

  RectangleInfo
    crop_info;

  Window
    *children,
    client,
    prior_target,
    root,
    target;

  XTextProperty
    window_name;

  /*
    Open X server connection.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(ximage_info != (XImportInfo *) NULL);
  display=XOpenDisplay(image_info->server_name);
  if (display == (Display *) NULL)
    {
      MagickError(XServerError,"UnableToOpenXServer",
        XDisplayName(image_info->server_name));
      return((Image *) NULL);
    }
  /*
    Set our forgiving error handler.
  */
  (void) XSetErrorHandler(XError);
  /*
    Select target window.
  */
  crop_info.x=0;
  crop_info.y=0;
  crop_info.width=0;
  crop_info.height=0;
  root=XRootWindow(display,XDefaultScreen(display));
  target=(Window) NULL;
  if ((image_info->filename != (char *) NULL) &&
      (*image_info->filename != '\0'))
    {
      if (LocaleCompare(image_info->filename,"root") == 0)
        target=root;
      else
        {
          /*
            Select window by ID or name.
          */
          if (isdigit((int) (*image_info->filename)))
            target=XWindowByID(display,root,(Window)
              strtol(image_info->filename,(char **) NULL,0));
          if (target == (Window) NULL)
            target=XWindowByName(display,root,image_info->filename);
          if (target == (Window) NULL)
            MagickError(XServerError,"NoWindowWithSpecifiedIDExists",
              image_info->filename);
        }
    }
  /*
    If target window is not defined, interactively select one.
  */
  prior_target=target;
  if (target == (Window) NULL)
    target=XSelectWindow(display,&crop_info);
  client=target;   /* obsolete */
  if (target != root)
    {
      unsigned int
        d;

      status=XGetGeometry(display,target,&root,&x,&x,&d,&d,&d,&d);
      if (status != 0)
        {
          for ( ; ; )
          {
            Window
              parent;

            /*
              Find window manager frame.
            */
            status=XQueryTree(display,target,&root,&parent,&children,&d);
            if (status && (children != (Window *) NULL))
              (void) XFree((char *) children);
            if (!status || (parent == (Window) NULL) || (parent == root))
              break;
            target=parent;
          }
          /*
            Get client window.
          */
          client=XClientWindow(display,target);
          if (!ximage_info->frame)
            target=client;
          if (!ximage_info->frame && prior_target)
            target=prior_target;
          (void) XRaiseWindow(display,target);
          XDelay(display,SuspendTime << 4);
        }
    }
  if (ximage_info->screen)
    {
      int
        y;

      Window
        child;

      XWindowAttributes
        window_attributes;

      /*
        Obtain window image directly from screen.
      */
      status=XGetWindowAttributes(display,target,&window_attributes);
      if (status == False)
        {
          MagickError(XServerError,"UnableToReadxWindowAttributes",
            image_info->filename);
          (void) XCloseDisplay(display);
          return((Image *) NULL);
        }
      (void) XTranslateCoordinates(display,target,root,0,0,&x,&y,&child);
      crop_info.x=x;
      crop_info.y=y;
      crop_info.width=window_attributes.width;
      crop_info.height=window_attributes.height;
      if (ximage_info->borders)
        {
          /*
            Include border in image.
          */
          crop_info.x-=window_attributes.border_width;
          crop_info.y-=window_attributes.border_width;
          crop_info.width+=window_attributes.border_width << 1;
          crop_info.height+=window_attributes.border_width << 1;
        }
      target=root;
    }
  /*
    If WM_COLORMAP_WINDOWS property is set or multiple colormaps, descend.
  */
  number_windows=0;
  status=XGetWMColormapWindows(display,target,&children,&number_windows);
  if ((status == True) && (number_windows > 0))
    {
      ximage_info->descend=True;
      (void) XFree ((char *) children);
    }
  colormaps=XListInstalledColormaps(display,target,&number_colormaps);
  if (number_colormaps > 0)
    {
      if (number_colormaps > 1)
        ximage_info->descend=True;
      (void) XFree((char *) colormaps);
    }
  /*
    Alert the user not to alter the screen.
  */
  if (!ximage_info->silent)
    (void) XBell(display,0);
  /*
    Get image by window id.
  */
  (void) XGrabServer(display);
  image=XGetWindowImage(display,target,ximage_info->borders,
    ximage_info->descend ? 1 : 0);
  (void) XUngrabServer(display);
  if (image == (Image *) NULL)
    MagickError(XServerError,"UnableToReadXWindowImage",image_info->filename);
  else
    {
      (void) strncpy(image->filename,image_info->filename,MaxTextExtent-1);
      if ((crop_info.width != 0) && (crop_info.height != 0))
        {
          Image
            *clone_image,
            *crop_image;

          /*
            Crop image as defined by the cropping rectangle.
          */
          clone_image=CloneImage(image,0,0,True,&image->exception);
          if (clone_image != (Image *) NULL)
            {
              crop_image=CropImage(clone_image,&crop_info,&image->exception);
              if (crop_image != (Image *) NULL)
                {
                  DestroyImage(image);
                  image=crop_image;
                }
            }
        }
      status=XGetWMName(display,target,&window_name);
      if (status == True)
        {
          if ((image_info->filename != (char *) NULL) &&
              (*image_info->filename == '\0'))
            {
              /*
                Initialize image filename.
              */
              (void) strncpy(image->filename,(char *) window_name.value,
                (int) window_name.nitems);
              image->filename[window_name.nitems]='\0';
            }
          (void) XFree((void *) window_name.value);
        }
    }
  if (!ximage_info->silent)
    {
      /*
        Alert the user we're done.
      */
      (void) XBell(display,0);
      (void) XBell(display,0);
    }
  (void) XCloseDisplay(display);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X I n i t i a l i z e W i n d o w s                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XInitializeWindows initializes the XWindows structure.
%
%  The format of the XInitializeWindows method is:
%
%      XWindows *XInitializeWindows(Display *display,
%        XResourceInfo *resource_info)
%
%  A description of each parameter follows:
%
%    o windows: XInitializeWindows returns a pointer to a XWindows structure.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
*/
MagickExport XWindows *XInitializeWindows(Display *display,
  XResourceInfo *resource_info)
{
  Window
    root_window;

  XWindows
    *windows;

  /*
    Allocate windows structure.
  */
  windows=(XWindows *) AcquireMemory(sizeof(XWindows));
  if (windows == (XWindows *) NULL)
    {
      MagickError(ResourceLimitError,"MemoryAllocationFailed",
        "unable to create X windows");
      return((XWindows *) NULL);
    }
  (void) memset(windows,0,sizeof(XWindows));
  windows->pixel_info=(XPixelInfo *) AcquireMemory(sizeof(XPixelInfo));
  windows->icon_pixel=(XPixelInfo *) AcquireMemory(sizeof(XPixelInfo));
  windows->icon_resources=(XResourceInfo *)
    AcquireMemory(sizeof(XResourceInfo));
  if ((windows->pixel_info == (XPixelInfo *) NULL) ||
      (windows->icon_pixel == (XPixelInfo *) NULL) ||
      (windows->icon_resources == (XResourceInfo *) NULL))
    {
      MagickError(ResourceLimitError,"MemoryAllocationFailed",
        "unable to create X windows");
      return((XWindows *) NULL);
    }
  /*
    Initialize windows structure.
  */
  windows->display=display;
  windows->wm_protocols=XInternAtom(display,"WM_PROTOCOLS",False);
  windows->wm_delete_window=XInternAtom(display,"WM_DELETE_WINDOW",False);
  windows->wm_take_focus=XInternAtom(display,"WM_TAKE_FOCUS",False);
  windows->im_protocols=XInternAtom(display,"IM_PROTOCOLS",False);
  windows->im_remote_command=XInternAtom(display,"IM_REMOTE_COMMAND",False);
  windows->im_update_widget=XInternAtom(display,"IM_UPDATE_WIDGET",False);
  windows->im_update_colormap=
    XInternAtom(display,"IM_UPDATE_COLORMAP",False);
  windows->im_former_image=XInternAtom(display,"IM_FORMER_IMAGE",False);
  windows->im_next_image=XInternAtom(display,"IM_NEXT_IMAGE",False);
  windows->im_retain_colors=XInternAtom(display,"IM_RETAIN_COLORS",False);
  windows->im_exit=XInternAtom(display,"IM_EXIT",False);
  windows->dnd_protocols=XInternAtom(display,"DndProtocol",False);
#if defined(WIN32)
  (void) XSynchronize(display,IsWindows95());
#endif
  if (IsEventLogging())
    {
      (void) XSynchronize(display,True);
      LogMagickEvent(X11Event,"Version: %.1024s",
        GetMagickVersion((unsigned long *) NULL));
      LogMagickEvent(X11Event,"  timestamp: %ld",time((time_t *) NULL));
      LogMagickEvent(X11Event,"Protocols:");
      LogMagickEvent(X11Event,"  Window Manager: 0x%lx",windows->wm_protocols);
      LogMagickEvent(X11Event,"    delete window: 0x%lx",
        windows->wm_delete_window);
      LogMagickEvent(X11Event,"    take focus: 0x%lx",windows->wm_take_focus);
      LogMagickEvent(X11Event,"  ImageMagick: 0x%lx",windows->im_protocols);
      LogMagickEvent(X11Event,"    remote command: 0x%lx",
        windows->im_remote_command);
      LogMagickEvent(X11Event,"    update widget: 0x%lx",
        windows->im_update_widget);
      LogMagickEvent(X11Event,"    update colormap: 0x%lx",
        windows->im_update_colormap);
      LogMagickEvent(X11Event,"    former image: 0x%lx",
        windows->im_former_image);
      LogMagickEvent(X11Event,"    next image: 0x%lx",windows->im_next_image);
      LogMagickEvent(X11Event,"    retain colors: 0x%lx",
        windows->im_retain_colors);
      LogMagickEvent(X11Event,"    exit: 0x%lx",windows->im_exit);
      LogMagickEvent(X11Event,"  Drag and Drop: 0x%lx",windows->dnd_protocols);
      LogMagickEvent(X11Event,"  timestamp: %ld",time((time_t *) NULL));
    }
  /*
    Allocate standard colormap.
  */
  windows->map_info=XAllocStandardColormap();
  windows->icon_map=XAllocStandardColormap();
  if ((windows->map_info == (XStandardColormap *) NULL) ||
      (windows->icon_map == (XStandardColormap *) NULL))
    MagickFatalError(ResourceLimitFatalError,
      "Unable to create standard colormap","MemoryAllocationFailed");
  windows->map_info->colormap=(Colormap) NULL;
  windows->icon_map->colormap=(Colormap) NULL;
  windows->pixel_info->pixels=(unsigned long *) NULL;
  windows->pixel_info->annotate_context=(GC) NULL;
  windows->pixel_info->highlight_context=(GC) NULL;
  windows->pixel_info->widget_context=(GC) NULL;
  windows->font_info=(XFontStruct *) NULL;
  windows->icon_pixel->annotate_context=(GC) NULL;
  windows->icon_pixel->pixels=(unsigned long *) NULL;
  /*
    Allocate visual.
  */
  *windows->icon_resources=(*resource_info);
  windows->icon_resources->map_type=(char *) NULL;
  windows->icon_resources->visual_type=(char *) "default";
  windows->icon_resources->colormap=SharedColormap;
  windows->visual_info=
    XBestVisualInfo(display,windows->map_info,resource_info);
  windows->icon_visual=
    XBestVisualInfo(display,windows->icon_map,windows->icon_resources);
  if ((windows->visual_info == (XVisualInfo *) NULL) ||
      (windows->icon_visual == (XVisualInfo *) NULL))
    MagickFatalError(XServerFatalError,"Unable to get visual",
      resource_info->visual_type);
  if (IsEventLogging())
    {
      LogMagickEvent(X11Event,"Visual:");
      LogMagickEvent(X11Event,"  visual id: 0x%lx",
        windows->visual_info->visualid);
      LogMagickEvent(X11Event,"  class: %.1024s",
        XVisualClassName(windows->visual_info->storage_class));
      LogMagickEvent(X11Event,"  depth: %d planes",
        windows->visual_info->depth);
      LogMagickEvent(X11Event,"  size of colormap: %d entries",
        windows->visual_info->colormap_size);
      LogMagickEvent(X11Event,"  red, green, blue masks: 0x%lx 0x%lx 0x%lx",
        windows->visual_info->red_mask,windows->visual_info->green_mask,
        windows->visual_info->blue_mask);
      LogMagickEvent(X11Event,"  significant bits in color: %d bits",
        windows->visual_info->bits_per_rgb);
      LogMagickEvent(X11Event,"  timestamp: %ld",time((time_t *) NULL));
    }
  /*
    Allocate class and manager hints.
  */
  windows->class_hints=XAllocClassHint();
  windows->manager_hints=XAllocWMHints();
  if ((windows->class_hints == (XClassHint *) NULL) ||
      (windows->manager_hints == (XWMHints *) NULL))
    MagickFatalError(ResourceLimitFatalError,"Unable to allocate X hints",
      (char *) NULL);
  /*
    Determine group leader if we have one.
  */
  root_window=XRootWindow(display,windows->visual_info->screen);
  windows->group_leader.id=(Window) NULL;
  if (resource_info->window_group != (char *) NULL)
    {
      if (isdigit((int) (*resource_info->window_group)))
        windows->group_leader.id=XWindowByID(display,root_window,(Window)
          strtol((char *) resource_info->window_group,(char **) NULL,0));
      if (windows->group_leader.id == (Window) NULL)
        windows->group_leader.id=
          XWindowByName(display,root_window,resource_info->window_group);
    }
  return(windows);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e C u r s o r                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XMakeCursor creates a crosshairs X11 cursor.
%
%  The format of the XMakeCursor method is:
%
%      Cursor XMakeCursor(Display *display,Window window,Colormap colormap,
%        char *background_color,char *foreground_color)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o window: Specifies the ID of the window for which the cursor is
%      assigned.
%
%    o colormap: Specifies the ID of the colormap from which the background
%      and foreground color will be retrieved.
%
%    o background_color: Specifies the color to use for the cursor background.
%
%    o foreground_color: Specifies the color to use for the cursor foreground.
%
%
*/
MagickExport Cursor XMakeCursor(Display *display,Window window,
  Colormap colormap,char *background_color,char *foreground_color)
{
#define scope_height 17
#define scope_x_hot 8
#define scope_y_hot 8
#define scope_width 17

  static const unsigned char
    scope_bits[] =
    {
      0x80, 0x03, 0x00, 0x80, 0x02, 0x00, 0x80, 0x02, 0x00, 0x80, 0x02,
      0x00, 0x80, 0x02, 0x00, 0x80, 0x02, 0x00, 0x80, 0x02, 0x00, 0x7f,
      0xfc, 0x01, 0x01, 0x00, 0x01, 0x7f, 0xfc, 0x01, 0x80, 0x02, 0x00,
      0x80, 0x02, 0x00, 0x80, 0x02, 0x00, 0x80, 0x02, 0x00, 0x80, 0x02,
      0x00, 0x80, 0x02, 0x00, 0x80, 0x03, 0x00
    },
    scope_mask_bits[] =
    {
      0xc0, 0x07, 0x00, 0xc0, 0x07, 0x00, 0xc0, 0x06, 0x00, 0xc0, 0x06,
      0x00, 0xc0, 0x06, 0x00, 0xc0, 0x06, 0x00, 0xff, 0xfe, 0x01, 0x7f,
      0xfc, 0x01, 0x03, 0x80, 0x01, 0x7f, 0xfc, 0x01, 0xff, 0xfe, 0x01,
      0xc0, 0x06, 0x00, 0xc0, 0x06, 0x00, 0xc0, 0x06, 0x00, 0xc0, 0x06,
      0x00, 0xc0, 0x07, 0x00, 0xc0, 0x07, 0x00
    };

  Cursor
    cursor;

  Pixmap
    mask,
    source;

  XColor
    background,
    foreground;

  assert(display != (Display *) NULL);
  assert(window != (Window) NULL);
  assert(colormap != (Colormap) NULL);
  assert(background_color != (char *) NULL);
  assert(foreground_color != (char *) NULL);
  source=XCreateBitmapFromData(display,window,(char *) scope_bits,scope_width,
    scope_height);
  mask=XCreateBitmapFromData(display,window,(char *) scope_mask_bits,
    scope_width,scope_height);
  if ((source == (Pixmap) NULL) || (mask == (Pixmap) NULL))
    {
      MagickError(XServerError,"UnableToCreatePixmap",(char *) NULL);
      return((Cursor) NULL);
    }
  (void) XParseColor(display,colormap,background_color,&background);
  (void) XParseColor(display,colormap,foreground_color,&foreground);
  cursor=XCreatePixmapCursor(display,source,mask,&foreground,&background,
    scope_x_hot,scope_y_hot);
  (void) XFreePixmap(display,source);
  (void) XFreePixmap(display,mask);
  return(cursor);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XMakeImage creates an X11 image.  If the image size differs from
%  the X11 image size, the image is first resized.
%
%  The format of the XMakeImage method is:
%
%      unsigned int XMakeImage(Display *display,
%        const XResourceInfo *resource_info,XWindowInfo *window,Image *image,
%        unsigned int width,unsigned int height)
%
%  A description of each parameter follows:
%
%    o status: Method XMakeImage returns True if the X image is
%      successfully created.  False is returned is there is a memory shortage.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o window: Specifies a pointer to a XWindowInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o width: Specifies the width in pixels of the rectangular area to
%      display.
%
%    o height: Specifies the height in pixels of the rectangular area to
%      display.
%
%
*/
MagickExport unsigned int XMakeImage(Display *display,
  const XResourceInfo *resource_info,XWindowInfo *window,Image *image,
  unsigned int width,unsigned int height)
{
  int
    depth,
    format;

  XImage
    *matte_image,
    *ximage;

  assert(display != (Display *) NULL);
  assert(resource_info != (XResourceInfo *) NULL);
  assert(window != (XWindowInfo *) NULL);
  assert(width != 0);
  assert(height != 0);
  if ((window->width == 0) || (window->height == 0))
    return(False);
  /*
    Apply user transforms to the image.
  */
  (void) XDefineCursor(display,window->id,window->busy_cursor);
  (void) XFlush(display);
  depth=window->depth;
  if (window->destroy)
    DestroyImage(window->image);
  window->image=image;
  window->destroy=False;
  if (window->image != (Image *) NULL)
    {
      if (window->crop_geometry)
        {
          Image
            *crop_image;

          RectangleInfo
            crop_info;

          /*
            Crop image.
          */
          SetGeometry(window->image,&crop_info);
          (void) GetGeometry(window->crop_geometry,&crop_info.x,&crop_info.y,
            &crop_info.width,&crop_info.height);
          crop_image=CropImage(window->image,&crop_info,&image->exception);
          if (crop_image != (Image *) NULL)
            {
              if (window->image != image)
                DestroyImage(window->image);
              window->image=crop_image;
              window->destroy=True;
            }
        }
      if ((width != (unsigned int) window->image->columns) ||
          (height != (unsigned int) window->image->rows))
        {
          Image
            *resize_image;

          /*
            Resize image.
          */
          resize_image=(Image *) NULL;
          if (window->pixel_info->colors != 0)
            resize_image=SampleImage(window->image,width,height,
              &image->exception);
          else
            {
              double
                scale_factor;
              
              scale_factor=((double) width*height)/
                ((double) window->image->columns*window->image->rows);
              if (scale_factor >= 0.01)
                resize_image=ZoomImage(window->image,width,height,
                  &image->exception);
              else
                {
                  Image
                    *sample_image;

                  sample_image=SampleImage(window->image,5*width,5*height,
                    &image->exception);
                  if (sample_image != (Image *) NULL)
                    resize_image=ScaleImage(sample_image,width,height,
                      &image->exception);
                  DestroyImage(sample_image);
                }
            }
          if (resize_image != (Image *) NULL)
            {
              if (window->image != image)
                DestroyImage(window->image);
              window->image=resize_image;
              window->destroy=True;
            }
        }
      width=(unsigned int) window->image->columns;
      height=(unsigned int) window->image->rows;
    }
  /*
    Create X image.
  */
  ximage=(XImage *) NULL;
  format=(depth == 1) ? XYBitmap : ZPixmap;
#if defined(HasSharedMemory)
  if (window->shared_memory)
    {
      ximage=XShmCreateImage(display,window->visual,depth,format,(char *) NULL,
        &window->segment_info[1],width,height);
      window->segment_info[1].shmid=shmget(IPC_PRIVATE,(int)
        (ximage->bytes_per_line*ximage->height),IPC_CREAT | 0777);
      window->shared_memory&=window->segment_info[1].shmid >= 0;
      if (window->shared_memory)
        window->segment_info[1].shmaddr=(char *)
          shmat(window->segment_info[1].shmid,0,0);
    }
#endif
  /*
    Allocate X image pixel data.
  */
#if defined(HasSharedMemory)
  if (window->shared_memory)
    {
      (void) XSync(display,False);
      xerror_alert=False;
      ximage->data=window->segment_info[1].shmaddr;
      window->segment_info[1].readOnly=False;
      (void) XShmAttach(display,&window->segment_info[1]);
      (void) XSync(display,False);
      if (xerror_alert)
        {
          window->shared_memory=False;
          (void) shmdt(window->segment_info[1].shmaddr);
          (void) shmctl(window->segment_info[1].shmid,IPC_RMID,0);
          window->segment_info[1].shmid=(-1);
        }
    }
#endif
  if (!window->shared_memory)
    ximage=XCreateImage(display,window->visual,depth,format,0,(char *) NULL,
      width,height,XBitmapPad(display),0);
  if (ximage == (XImage *) NULL)
    {
      /*
        Unable to create X image.
      */
      (void) XDefineCursor(display,window->id,window->cursor);
      return(False);
    }
  if (IsEventLogging())
    {
      LogMagickEvent(X11Event,"XImage:");
      LogMagickEvent(X11Event,"  width, height: %dx%d",ximage->width,
        ximage->height);
      LogMagickEvent(X11Event,"  format: %d",ximage->format);
      LogMagickEvent(X11Event,"  byte order: %d",ximage->byte_order);
      LogMagickEvent(X11Event,"  bitmap unit, bit order, pad: %d %d %d",
        ximage->bitmap_unit,ximage->bitmap_bit_order,ximage->bitmap_pad);
      LogMagickEvent(X11Event,"  depth: %d",ximage->depth);
      LogMagickEvent(X11Event,"  bytes per line: %d",ximage->bytes_per_line);
      LogMagickEvent(X11Event,"  bits per pixel: %d",ximage->bits_per_pixel);
      LogMagickEvent(X11Event,"  red, green, blue masks: 0x%lx 0x%lx 0x%lx",
        ximage->red_mask,ximage->green_mask,ximage->blue_mask);
      LogMagickEvent(X11Event,"  timestamp: %ld",time((time_t *) NULL));
    }
  if (!window->shared_memory)
    {
      if (ximage->format == XYBitmap)
        ximage->data=(char *)
          AcquireMemory(ximage->bytes_per_line*ximage->height*ximage->depth);
      else
        ximage->data=(char *)
          AcquireMemory(ximage->bytes_per_line*ximage->height);
    }
  if (ximage->data == (char *) NULL)
    {
      /*
        Unable to allocate pixel data.
      */
      XDestroyImage(ximage);
      (void) XDefineCursor(display,window->id,window->cursor);
      return(False);
    }
  if (window->ximage != (XImage *) NULL)
    {
      /*
        Destroy previous X image.
      */
#if defined(HasSharedMemory)
      if (window->segment_info[0].shmid >= 0)
        {
          (void) XSync(display,False);
          (void) XShmDetach(display,&window->segment_info[0]);
          (void) XSync(display,False);
          (void) shmdt(window->segment_info[0].shmaddr);
          (void) shmctl(window->segment_info[0].shmid,IPC_RMID,0);
          window->segment_info[0].shmid=(-1);
          window->ximage->data=(char *) NULL;
        }
#endif
      if (window->ximage->data != (char *) NULL)
        LiberateMemory((void **) &window->ximage->data);
      window->ximage->data=(char *) NULL;
      XDestroyImage(window->ximage);
    }
#if defined(HasSharedMemory)
  window->segment_info[0]=window->segment_info[1];
#endif
  window->ximage=ximage;
  matte_image=(XImage *) NULL;
  if (window->image != (Image *) NULL)
    if (window->image->matte &&
        (width <= (unsigned int) XDisplayWidth(display,window->screen)) &&
        (height <= (unsigned int) XDisplayHeight(display,window->screen)))
      {
        /*
          Create matte image.
        */
        matte_image=XCreateImage(display,window->visual,1,XYBitmap,0,
          (char *) NULL,width,height,XBitmapPad(display),0);
        if (IsEventLogging())
          {
            LogMagickEvent(X11Event,"Matte Image:");
            LogMagickEvent(X11Event,"  width, height: %dx%d",matte_image->width,
              matte_image->height);
            LogMagickEvent(X11Event,"  timestamp: %ld",time((time_t *) NULL));
          }
        if (matte_image != (XImage *) NULL)
          {
            /*
              Allocate matte image pixel data.
            */
            matte_image->data=(char *)
              AcquireMemory(matte_image->bytes_per_line*
              matte_image->height*matte_image->depth);
            if (matte_image->data == (char *) NULL)
              {
                XDestroyImage(matte_image);
                matte_image=(XImage *) NULL;
              }
          }
      }
  if (window->matte_image != (XImage *) NULL)
    {
      /*
        Free matte image.
      */
      if (window->matte_image->data != (char *) NULL)
        (void) LiberateMemory((void **) &window->matte_image->data);
      window->matte_image->data=(char *) NULL;
      XDestroyImage(window->matte_image);
    }
  window->matte_image=matte_image;
  if (window->matte_pixmap != (Pixmap) NULL)
    {
      (void) XFreePixmap(display,window->matte_pixmap);
      window->matte_pixmap=(Pixmap) NULL;
#if defined(HasShape)
      if (window->shape)
        XShapeCombineMask(display,window->id,ShapeBounding,0,0,None,ShapeSet);
#endif
    }
  window->stasis=False;
  /*
    Convert pixels to X image data.
  */
  if (window->image != (Image *) NULL)
    {
      if ((ximage->byte_order == LSBFirst) || ((ximage->format == XYBitmap) &&
          (ximage->bitmap_bit_order == LSBFirst)))
        XMakeImageLSBFirst(resource_info,window,window->image,ximage,
          matte_image);
      else
        XMakeImageMSBFirst(resource_info,window,window->image,ximage,
          matte_image);
    }
  if (window->matte_image != (XImage *) NULL)
    {
      /*
        Create matte pixmap.
      */
      window->matte_pixmap=XCreatePixmap(display,window->id,width,height,1);
      if (window->matte_pixmap != (Pixmap) NULL)
        {
          GC
            graphics_context;

          XGCValues
            context_values;

          /*
            Copy matte image to matte pixmap.
          */
          context_values.background=1;
          context_values.foreground=0;
          graphics_context=XCreateGC(display,window->matte_pixmap,
            GCBackground | GCForeground,&context_values);
          (void) XPutImage(display,window->matte_pixmap,graphics_context,
            window->matte_image,0,0,0,0,width,height);
          (void) XFreeGC(display,graphics_context);
#if defined(HasShape)
          if (window->shape)
            XShapeCombineMask(display,window->id,ShapeBounding,0,0,
              window->matte_pixmap,ShapeSet);
#endif
        }
      }
  (void) XMakePixmap(display,resource_info,window);
  /*
    Restore cursor.
  */
  (void) XDefineCursor(display,window->id,window->cursor);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   X M a k e I m a g e L S B F i r s t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XMakeImageLSBFirst initializes the pixel data of an X11 Image.
%  The X image pixels are copied in least-significant bit and byte first
%  order.  The server's scanline pad is respected.  Rather than using one or
%  two general cases, many special cases are found here to help speed up the
%  image conversion.
%
%  The format of the XMakeImageLSBFirst method is:
%
%      void XMakeMagnifyImage(Display *display,XWindows *windows)
%
%  A description of each parameter follows:
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o window: Specifies a pointer to a XWindowInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o ximage: Specifies a pointer to a XImage structure;  returned from
%      XCreateImage.
%
%    o matte_image: Specifies a pointer to a XImage structure;  returned from
%      XCreateImage.
%
*/
static void XMakeImageLSBFirst(const XResourceInfo *resource_info,
  const XWindowInfo *window,Image *image,XImage *ximage,XImage *matte_image)
{
  int
    y;

  register IndexPacket
    *indexes;

  register int
    x;

  register const PixelPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    bit,
    byte;

  unsigned int
    scanline_pad;

  unsigned long
    pixel,
    *pixels;

  XStandardColormap
    *map_info;

  assert(resource_info != (XResourceInfo *) NULL);
  assert(window != (XWindowInfo *) NULL);
  assert(image != (Image *) NULL);
  scanline_pad=ximage->bytes_per_line-
    ((ximage->width*ximage->bits_per_pixel) >> 3);
  map_info=window->map_info;
  pixels=window->pixel_info->pixels;
  q=(unsigned char *) ximage->data;
  x=0;
  if (ximage->format == XYBitmap)
    {
      register unsigned short
        polarity;

      unsigned char
        background,
        foreground;

      /*
        Convert image to big-endian bitmap.
      */
      background=(PixelIntensity(&window->pixel_info->foreground_color) <
        PixelIntensity(&window->pixel_info->background_color) ?  0x80 : 0x00);
      foreground=(PixelIntensity(&window->pixel_info->background_color) <
        PixelIntensity(&window->pixel_info->foreground_color) ?  0x80 : 0x00);
      polarity=PixelIntensityToQuantum(&image->colormap[0]) < (MaxRGB/2);
      if (image->colors == 2)
        polarity=PixelIntensity(&image->colormap[0]) <
          PixelIntensity(&image->colormap[1]);
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
        if (p == (const PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        bit=0;
        byte=0;
        for (x=0; x < (long) image->columns; x++)
        {
          byte>>=1;
          if (indexes[x] == polarity)
            byte|=foreground;
          else
            byte|=background;
          bit++;
          if (bit == 8)
            {
              *q++=byte;
              bit=0;
              byte=0;
            }
        }
        if (bit != 0)
          *q=byte >> (8-bit);
        q+=scanline_pad;
      }
    }
  else
    if (window->pixel_info->colors != 0)
      switch (ximage->bits_per_pixel)
      {
        case 2:
        {
          register unsigned int
            nibble;

          /*
            Convert to 2 bit color-mapped X image.
          */
          for (y=0; y < (long) image->rows; y++)
          {
            p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            indexes=GetIndexes(image);
            nibble=0;
            for (x=0; x < (long) image->columns; x++)
            {
              pixel=pixels[indexes[x]] & 0x0f;
              switch (nibble)
              {
                case 0:
                {
                  *q=(unsigned char) pixel;
                  nibble++;
                  break;
                }
                case 1:
                {
                  *q|=(unsigned char) (pixel << 2);
                  nibble++;
                  break;
                }
                case 2:
                {
                  *q|=(unsigned char) (pixel << 4);
                  nibble++;
                  break;
                }
                case 3:
                {
                  *q|=(unsigned char) (pixel << 6);
                  q++;
                  nibble=0;
                  break;
                }
              }
            }
            q+=scanline_pad;
          }
          break;
        }
        case 4:
        {
          register unsigned int
            nibble;

          /*
            Convert to 4 bit color-mapped X image.
          */
          for (y=0; y < (long) image->rows; y++)
          {
            p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            indexes=GetIndexes(image);
            nibble=0;
            for (x=0; x < (long) image->columns; x++)
            {
              pixel=pixels[indexes[x]] & 0xf;
              switch (nibble)
              {
                case 0:
                {
                  *q=(unsigned char) pixel;
                  nibble++;
                  break;
                }
                case 1:
                {
                  *q|=(unsigned char) (pixel << 4);
                  q++;
                  nibble=0;
                  break;
                }
              }
            }
            q+=scanline_pad;
          }
          break;
        }
        case 6:
        case 8:
        {
          /*
            Convert to 8 bit color-mapped X image.
          */
          if (resource_info->color_recovery &&
              resource_info->quantize_info->dither)
            {
              XDitherImage(image,ximage);
              break;
            }
          for (y=0; y < (long) image->rows; y++)
          {
            p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            indexes=GetIndexes(image);
            for (x=0; x < (long) image->columns; x++)
            {
              pixel=pixels[indexes[x]];
              *q++=(unsigned char) pixel;
            }
            q+=scanline_pad;
          }
          break;
        }
        default:
        {
          register int
            k;

          register unsigned int
            bytes_per_pixel;

          unsigned char
            channel[sizeof(unsigned long)];

          /*
            Convert to multi-byte color-mapped X image.
          */
          bytes_per_pixel=ximage->bits_per_pixel >> 3;
          for (y=0; y < (long) image->rows; y++)
          {
            p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            indexes=GetIndexes(image);
            for (x=0; x < (long) image->columns; x++)
            {
              pixel=pixels[indexes[x]];
              for (k=0; k < (int) bytes_per_pixel; k++)
              {
                channel[k]=(unsigned char) pixel;
                pixel>>=8;
              }
              for (k=0; k < (int) bytes_per_pixel; k++)
                *q++=channel[k];
            }
            q+=scanline_pad;
          }
          break;
        }
      }
    else
      switch (ximage->bits_per_pixel)
      {
        case 2:
        {
          register unsigned int
            nibble;

          /*
            Convert to contiguous 2 bit continuous-tone X image.
          */
          for (y=0; y < (long) image->rows; y++)
          {
            nibble=0;
            p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            for (x=0; x < (long) image->columns; x++)
            {
              pixel=XGammaPixel(map_info,p);
              pixel&=0xf;
              switch (nibble)
              {
                case 0:
                {
                  *q=(unsigned char) pixel;
                  nibble++;
                  break;
                }
                case 1:
                {
                  *q|=(unsigned char) (pixel << 2);
                  nibble++;
                  break;
                }
                case 2:
                {
                  *q|=(unsigned char) (pixel << 4);
                  nibble++;
                  break;
                }
                case 3:
                {
                  *q|=(unsigned char) (pixel << 6);
                  q++;
                  nibble=0;
                  break;
                }
              }
              p++;
            }
            q+=scanline_pad;
          }
          break;
        }
        case 4:
        {
          register unsigned int
            nibble;

          /*
            Convert to contiguous 4 bit continuous-tone X image.
          */
          for (y=0; y < (long) image->rows; y++)
          {
            p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            nibble=0;
            for (x=0; x < (long) image->columns; x++)
            {
              pixel=XGammaPixel(map_info,p);
              pixel&=0xf;
              switch (nibble)
              {
                case 0:
                {
                  *q=(unsigned char) pixel;
                  nibble++;
                  break;
                }
                case 1:
                {
                  *q|=(unsigned char) (pixel << 4);
                  q++;
                  nibble=0;
                  break;
                }
              }
              p++;
            }
            q+=scanline_pad;
          }
          break;
        }
        case 6:
        case 8:
        {
          /*
            Convert to contiguous 8 bit continuous-tone X image.
          */
          if (resource_info->color_recovery &&
              resource_info->quantize_info->dither)
            {
              XDitherImage(image,ximage);
              break;
            }
          for (y=0; y < (long) image->rows; y++)
          {
            p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            for (x=0; x < (long) image->columns; x++)
            {
              pixel=XGammaPixel(map_info,p);
              *q++=(unsigned char) pixel;
              p++;
            }
            q+=scanline_pad;
          }
          break;
        }
        default:
        {
          if ((ximage->bits_per_pixel == 32) && (map_info->red_max == 255) &&
              (map_info->green_max == 255) && (map_info->blue_max == 255) &&
              (map_info->red_mult == 65536L) && (map_info->green_mult == 256) &&
              (map_info->blue_mult == 1))
            {
              /*
                Convert to 32 bit continuous-tone X image.
              */
              for (y=0; y < (long) image->rows; y++)
              {
                p=AcquireImagePixels(image,0,y,image->columns,1,
                  &image->exception);
                if (p == (const PixelPacket *) NULL)
                  break;
                for (x=0; x < (long) image->columns; x++)
                {
                  *q++=ScaleQuantumToChar(XBlueGamma(p->blue));
                  *q++=ScaleQuantumToChar(XGreenGamma(p->green));
                  *q++=ScaleQuantumToChar(XRedGamma(p->red));
                  *q++=0;
                  p++;
                }
              }
            }
          else
            if ((ximage->bits_per_pixel == 32) && (map_info->red_max == 255) &&
                (map_info->green_max == 255) && (map_info->blue_max == 255) &&
                (map_info->red_mult == 1) && (map_info->green_mult == 256) &&
                (map_info->blue_mult == 65536L))
              {
                /*
                  Convert to 32 bit continuous-tone X image.
                */
                for (y=0; y < (long) image->rows; y++)
                {
                  p=AcquireImagePixels(image,0,y,image->columns,1,
                    &image->exception);
                  if (p == (const PixelPacket *) NULL)
                    break;
                  for (x=0; x < (long) image->columns; x++)
                  {
                    *q++=ScaleQuantumToChar(XRedGamma(p->red));
                    *q++=ScaleQuantumToChar(XGreenGamma(p->green));
                    *q++=ScaleQuantumToChar(XBlueGamma(p->blue));
                    *q++=0;
                    p++;
                  }
                }
              }
            else
              {
                register int
                  k;

                register unsigned int
                  bytes_per_pixel;

                unsigned char
                  channel[sizeof(unsigned long)];

                /*
                  Convert to multi-byte continuous-tone X image.
                */
                bytes_per_pixel=ximage->bits_per_pixel >> 3;
                for (y=0; y < (long) image->rows; y++)
                {
                  p=AcquireImagePixels(image,0,y,image->columns,1,
                    &image->exception);
                  if (p == (PixelPacket *) NULL)
                    break;
                  for (x=0; x < (long) image->columns; x++)
                  {
                    pixel=XGammaPixel(map_info,p);
                    for (k=0; k < (int) bytes_per_pixel; k++)
                    {
                      channel[k]=(unsigned char) pixel;
                      pixel>>=8;
                    }
                    for (k=0; k < (int) bytes_per_pixel; k++)
                      *q++=channel[k];
                    p++;
                  }
                  q+=scanline_pad;
                }
              }
          break;
        }
      }
  if (matte_image != (XImage *) NULL)
    {
      /*
        Initialize matte image.
      */
      scanline_pad=matte_image->bytes_per_line-
        ((matte_image->width*matte_image->bits_per_pixel) >> 3);
      q=(unsigned char *) matte_image->data;
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
        if (p == (const PixelPacket *) NULL)
          break;
        bit=0;
        byte=0;
        for (x=0; x < (long) image->columns; x++)
        {
          byte>>=1;
          if (p->opacity == TransparentOpacity)
            byte|=0x80;
          bit++;
          if (bit == 8)
            {
              *q++=byte;
              bit=0;
              byte=0;
            }
          p++;
        }
        if (bit != 0)
          *q=byte >> (8-bit);
        q+=scanline_pad;
      }
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   X M a k e I m a g e M S B F i r s t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XMakeImageMSBFirst initializes the pixel data of an X11 Image.
%  The X image pixels are copied in most-significant bit and byte first order.
%  The server's scanline pad is also respected. Rather than using one or two
%  general cases, many special cases are found here to help speed up the image
%  conversion.
%
%  The format of the XMakeImageMSBFirst method is:
%
%      XMakeImageMSBFirst(resource_info,window,image,ximage,matte_image)
%
%  A description of each parameter follows:
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o window: Specifies a pointer to a XWindowInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o ximage: Specifies a pointer to a XImage structure;  returned from
%      XCreateImage.
%
%    o matte_image: Specifies a pointer to a XImage structure;  returned from
%      XCreateImage.
%
%
*/
static void XMakeImageMSBFirst(const XResourceInfo *resource_info,
  const XWindowInfo *window,Image *image,XImage *ximage,XImage *matte_image)
{
  int
    y;

  register IndexPacket
    *indexes;

  register int
    x;

  register const PixelPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    bit,
    byte;

  unsigned int
    scanline_pad;

  unsigned long
    pixel,
    *pixels;

  XStandardColormap
    *map_info;

  assert(resource_info != (XResourceInfo *) NULL);
  assert(window != (XWindowInfo *) NULL);
  assert(image != (Image *) NULL);
  scanline_pad=ximage->bytes_per_line-
    ((ximage->width*ximage->bits_per_pixel) >> 3);
  map_info=window->map_info;
  pixels=window->pixel_info->pixels;
  q=(unsigned char *) ximage->data;
  x=0;
  if (ximage->format == XYBitmap)
    {
      register unsigned short
        polarity;

      unsigned char
        background,
        foreground;

      /*
        Convert image to big-endian bitmap.
      */
      background=(PixelIntensity(&window->pixel_info->foreground_color) <
        PixelIntensity(&window->pixel_info->background_color) ?  0x01 : 0x00);
      foreground=(PixelIntensity(&window->pixel_info->background_color) <
        PixelIntensity(&window->pixel_info->foreground_color) ?  0x01 : 0x00);
      polarity=PixelIntensityToQuantum(&image->colormap[0]) < (MaxRGB/2);
      if (image->colors == 2)
        polarity=PixelIntensity(&image->colormap[0]) <
          PixelIntensity(&image->colormap[1]);
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
        if (p == (const PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        bit=0;
        byte=0;
        for (x=0; x < (long) image->columns; x++)
        {
          byte<<=1;
          if (indexes[x] == polarity)
            byte|=foreground;
          else
            byte|=background;
          bit++;
          if (bit == 8)
            {
              *q++=byte;
              bit=0;
              byte=0;
            }
        }
        if (bit != 0)
          *q=byte << (8-bit);
        q+=scanline_pad;
      }
    }
  else
    if (window->pixel_info->colors != 0)
      switch (ximage->bits_per_pixel)
      {
        case 2:
        {
          register unsigned int
            nibble;

          /*
            Convert to 2 bit color-mapped X image.
          */
          for (y=0; y < (long) image->rows; y++)
          {
            p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            indexes=GetIndexes(image);
            nibble=0;
            for (x=0; x < (long) image->columns; x++)
            {
              pixel=pixels[indexes[x]] & 0xf;
              switch (nibble)
              {
                case 0:
                {
                  *q=(unsigned char) (pixel << 6);
                  nibble++;
                  break;
                }
                case 1:
                {
                  *q|=(unsigned char) (pixel << 4);
                  nibble++;
                  break;
                }
                case 2:
                {
                  *q|=(unsigned char) (pixel << 2);
                  nibble++;
                  break;
                }
                case 3:
                {
                  *q|=(unsigned char) pixel;
                  q++;
                  nibble=0;
                  break;
                }
              }
            }
            q+=scanline_pad;
          }
          break;
        }
        case 4:
        {
          register unsigned int
            nibble;

          /*
            Convert to 4 bit color-mapped X image.
          */
          for (y=0; y < (long) image->rows; y++)
          {
            p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            indexes=GetIndexes(image);
            nibble=0;
            for (x=0; x < (long) image->columns; x++)
            {
              pixel=pixels[indexes[x]] & 0xf;
              switch (nibble)
              {
                case 0:
                {
                  *q=(unsigned char) (pixel << 4);
                  nibble++;
                  break;
                }
                case 1:
                {
                  *q|=(unsigned char) pixel;
                  q++;
                  nibble=0;
                  break;
                }
              }
            }
            q+=scanline_pad;
          }
          break;
        }
        case 6:
        case 8:
        {
          /*
            Convert to 8 bit color-mapped X image.
          */
          if (resource_info->color_recovery &&
              resource_info->quantize_info->dither)
            {
              XDitherImage(image,ximage);
              break;
            }
          for (y=0; y < (long) image->rows; y++)
          {
            p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            indexes=GetIndexes(image);
            for (x=0; x < (long) image->columns; x++)
            {
              pixel=pixels[indexes[x]];
              *q++=(unsigned char) pixel;
            }
            q+=scanline_pad;
          }
          break;
        }
        default:
        {
          register int
            k;

          register unsigned int
            bytes_per_pixel;

          unsigned char
            channel[sizeof(unsigned long)];

          /*
            Convert to 8 bit color-mapped X image.
          */
          bytes_per_pixel=ximage->bits_per_pixel >> 3;
          for (y=0; y < (long) image->rows; y++)
          {
            p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            indexes=GetIndexes(image);
            for (x=0; x < (long) image->columns; x++)
            {
              pixel=pixels[indexes[x]];
              for (k=bytes_per_pixel-1; k >= 0; k--)
              {
                channel[k]=(unsigned char) pixel;
                pixel>>=8;
              }
              for (k=0; k < (int) bytes_per_pixel; k++)
                *q++=channel[k];
            }
            q+=scanline_pad;
          }
          break;
        }
      }
    else
      switch (ximage->bits_per_pixel)
      {
        case 2:
        {
          register unsigned int
            nibble;

          /*
            Convert to 4 bit continuous-tone X image.
          */
          for (y=0; y < (long) image->rows; y++)
          {
            p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            nibble=0;
            for (x=0; x < (long) image->columns; x++)
            {
              pixel=XGammaPixel(map_info,p);
              pixel&=0xf;
              switch (nibble)
              {
                case 0:
                {
                  *q=(unsigned char) (pixel << 6);
                  nibble++;
                  break;
                }
                case 1:
                {
                  *q|=(unsigned char) (pixel << 4);
                  nibble++;
                  break;
                }
                case 2:
                {
                  *q|=(unsigned char) (pixel << 2);
                  nibble++;
                  break;
                }
                case 3:
                {
                  *q|=(unsigned char) pixel;
                  q++;
                  nibble=0;
                  break;
                }
              }
              p++;
            }
            q+=scanline_pad;
          }
          break;
        }
        case 4:
        {
          register unsigned int
            nibble;

          /*
            Convert to 4 bit continuous-tone X image.
          */
          for (y=0; y < (long) image->rows; y++)
          {
            p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            nibble=0;
            for (x=0; x < (long) image->columns; x++)
            {
              pixel=XGammaPixel(map_info,p);
              pixel&=0xf;
              switch (nibble)
              {
                case 0:
                {
                  *q=(unsigned char) (pixel << 4);
                  nibble++;
                  break;
                }
                case 1:
                {
                  *q|=(unsigned char) pixel;
                  q++;
                  nibble=0;
                  break;
                }
              }
              p++;
            }
            q+=scanline_pad;
          }
          break;
        }
        case 6:
        case 8:
        {
          /*
            Convert to 8 bit continuous-tone X image.
          */
          if (resource_info->color_recovery &&
              resource_info->quantize_info->dither)
            {
              XDitherImage(image,ximage);
              break;
            }
          for (y=0; y < (long) image->rows; y++)
          {
            p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            for (x=0; x < (long) image->columns; x++)
            {
              pixel=XGammaPixel(map_info,p);
              *q++=(unsigned char) pixel;
              p++;
            }
            q+=scanline_pad;
          }
          break;
        }
        default:
        {
          if ((ximage->bits_per_pixel == 32) && (map_info->red_max == 255) &&
              (map_info->green_max == 255) && (map_info->blue_max == 255) &&
              (map_info->red_mult == 65536L) && (map_info->green_mult == 256) &&
              (map_info->blue_mult == 1))
            {
              /*
                Convert to 32 bit continuous-tone X image.
              */
              for (y=0; y < (long) image->rows; y++)
              {
                p=AcquireImagePixels(image,0,y,image->columns,1,
                  &image->exception);
                if (p == (const PixelPacket *) NULL)
                  break;
                for (x=0; x < (long) image->columns; x++)
                {
                  *q++=0;
                  *q++=ScaleQuantumToChar(XRedGamma(p->red));
                  *q++=ScaleQuantumToChar(XGreenGamma(p->green));
                  *q++=ScaleQuantumToChar(XBlueGamma(p->blue));
                  p++;
                }
              }
            }
          else
            if ((ximage->bits_per_pixel == 32) && (map_info->red_max == 255) &&
                (map_info->green_max == 255) && (map_info->blue_max == 255) &&
                (map_info->red_mult == 1) && (map_info->green_mult == 256) &&
                (map_info->blue_mult == 65536L))
              {
                /*
                  Convert to 32 bit continuous-tone X image.
                */
                for (y=0; y < (long) image->rows; y++)
                {
                  p=AcquireImagePixels(image,0,y,image->columns,1,
                    &image->exception);
                  if (p == (const PixelPacket *) NULL)
                    break;
                  for (x=0; x < (long) image->columns; x++)
                  {
                    *q++=0;
                    *q++=ScaleQuantumToChar(XBlueGamma(p->blue));
                    *q++=ScaleQuantumToChar(XGreenGamma(p->green));
                    *q++=ScaleQuantumToChar(XRedGamma(p->red));
                    p++;
                  }
                }
              }
            else
              {
                register int
                  k;

                register unsigned int
                  bytes_per_pixel;

                unsigned char
                  channel[sizeof(unsigned long)];

                /*
                  Convert to multi-byte continuous-tone X image.
                */
                bytes_per_pixel=ximage->bits_per_pixel >> 3;
                for (y=0; y < (long) image->rows; y++)
                {
                  p=AcquireImagePixels(image,0,y,image->columns,1,
                    &image->exception);
                  if (p == (const PixelPacket *) NULL)
                    break;
                  for (x=0; x < (long) image->columns; x++)
                  {
                    pixel=XGammaPixel(map_info,p);
                    for (k=bytes_per_pixel-1; k >= 0; k--)
                    {
                      channel[k]=(unsigned char) pixel;
                      pixel>>=8;
                    }
                    for (k=0; k < (int) bytes_per_pixel; k++)
                      *q++=channel[k];
                    p++;
                  }
                  q+=scanline_pad;
                }
              }
          break;
        }
      }
  if (matte_image != (XImage *) NULL)
    {
      /*
        Initialize matte image.
      */
      scanline_pad=matte_image->bytes_per_line-
        ((matte_image->width*matte_image->bits_per_pixel) >> 3);
      q=(unsigned char *) matte_image->data;
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
        if (p == (const PixelPacket *) NULL)
          break;
        bit=0;
        byte=0;
        for (x=0; x < (long) image->columns; x++)
        {
          byte<<=1;
          if (p->opacity == TransparentOpacity)
            byte|=0x01;
          bit++;
          if (bit == 8)
            {
              *q++=byte;
              bit=0;
              byte=0;
            }
          p++;
        }
        if (bit != 0)
          *q=byte << (8-bit);
        q+=scanline_pad;
      }
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e M a g n i f y I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XMakeMagnifyImage magnifies a region of an X image and displays it.
%
%  The format of the XMakeMagnifyImage method is:
%
%      XMakeMagnifyImage(display,windows)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o windows: Specifies a pointer to a XWindows structure.
%
%
*/
MagickExport void XMakeMagnifyImage(Display *display,XWindows *windows)
{
  char
    tuple[MaxTextExtent];

  int
    y;

  long
    n;

  register int
    x;

  register long
    i;

  register unsigned char
    *p,
    *q;

  PixelPacket
    color;

  static char
    text[MaxTextExtent];

  static unsigned int
    previous_magnify = 0;

  static XWindowInfo
    magnify_window;

  unsigned int
    height,
    j,
    k,
    l,
    magnify,
    scanline_pad,
    width;

  XImage
    *ximage;

  /*
    Check boundary conditions.
  */
  assert(display != (Display *) NULL);
  assert(windows != (XWindows *) NULL);
  magnify=1;
  for (n=1; n < (long) windows->magnify.data; n++)
    magnify<<=1;
  while ((magnify*windows->image.ximage->width) < windows->magnify.width)
    magnify<<=1;
  while ((magnify*windows->image.ximage->height) < windows->magnify.height)
    magnify<<=1;
  while (magnify > windows->magnify.width)
    magnify>>=1;
  while (magnify > windows->magnify.height)
    magnify>>=1;
  if (magnify != previous_magnify)
    {
      unsigned int
        status;

      XTextProperty
        window_name;

      /*
        New magnify factor:  update magnify window name.
      */
      i=0;
      while ((1 << i) <= (int) magnify)
        i++;
      FormatString(windows->magnify.name,"Magnify %luX",i);
      status=XStringListToTextProperty(&windows->magnify.name,1,&window_name);
      if (status != 0)
        {
          XSetWMName(display,windows->magnify.id,&window_name);
          XSetWMIconName(display,windows->magnify.id,&window_name);
          (void) XFree((void *) window_name.value);
        }
    }
  previous_magnify=magnify;
  ximage=windows->image.ximage;
  width=windows->magnify.ximage->width;
  height=windows->magnify.ximage->height;
  if ((windows->magnify.x < 0) ||
      (windows->magnify.x >= windows->image.ximage->width))
    windows->magnify.x=windows->image.ximage->width >> 1;
  x=windows->magnify.x-((width/magnify) >> 1);
  if (x < 0)
    x=0;
  else
    if (x > (int) (ximage->width-(width/magnify)))
      x=ximage->width-width/magnify;
  if ((windows->magnify.y < 0) ||
      (windows->magnify.y >= windows->image.ximage->height))
    windows->magnify.y=windows->image.ximage->height >> 1;
  y=windows->magnify.y-((height/magnify) >> 1);
  if (y < 0)
    y=0;
  else
    if (y > (int) (ximage->height-(height/magnify)))
      y=ximage->height-height/magnify;
  q=(unsigned char *) windows->magnify.ximage->data;
  scanline_pad=windows->magnify.ximage->bytes_per_line-
    ((width*windows->magnify.ximage->bits_per_pixel) >> 3);
  if (ximage->bits_per_pixel < 8)
    {
      register unsigned char
        background,
        byte,
        foreground,
        p_bit,
        q_bit;

      register unsigned int
        plane;

      XPixelInfo
        *pixel_info;

      pixel_info=windows->magnify.pixel_info;
      switch (ximage->bitmap_bit_order)
      {
        case LSBFirst:
        {
          /*
            Magnify little-endian bitmap.
          */
          background=0x00;
          foreground=0x80;
          if (ximage->format == XYBitmap)
            {
              background=(PixelIntensity(&pixel_info->foreground_color) <
                PixelIntensity(&pixel_info->background_color) ?  0x80 : 0x00);
              foreground=(PixelIntensity(&pixel_info->background_color) <
                PixelIntensity(&pixel_info->foreground_color) ?  0x80 : 0x00);
              if (windows->magnify.depth > 1)
                Swap(background,foreground);
            }
          for (i=0; i < (long) height; i+=magnify)
          {
            /*
              Propogate pixel magnify rows.
            */
            for (j=0; j < magnify; j++)
            {
              p=(unsigned char *) ximage->data+y*ximage->bytes_per_line+
                ((x*ximage->bits_per_pixel) >> 3);
              p_bit=(x*ximage->bits_per_pixel) & 0x07;
              q_bit=0;
              byte=0;
              for (k=0; k < width; k+=magnify)
              {
                /*
                  Propogate pixel magnify columns.
                */
                for (l=0; l < magnify; l++)
                {
                  /*
                    Propogate each bit plane.
                  */
                  for (plane=0; (int) plane < ximage->bits_per_pixel; plane++)
                  {
                    byte>>=1;
                    if (*p & (0x01 << (p_bit+plane)))
                      byte|=foreground;
                    else
                      byte|=background;
                    q_bit++;
                    if (q_bit == 8)
                      {
                        *q++=byte;
                        q_bit=0;
                        byte=0;
                      }
                  }
                }
                p_bit+=ximage->bits_per_pixel;
                if (p_bit == 8)
                  {
                    p++;
                    p_bit=0;
                  }
                if (q_bit != 0)
                  *q=byte >> (8-q_bit);
                q+=scanline_pad;
              }
            }
            y++;
          }
          break;
        }
        case MSBFirst:
        default:
        {
          /*
            Magnify big-endian bitmap.
          */
          background=0x00;
          foreground=0x01;
          if (ximage->format == XYBitmap)
            {
              background=(PixelIntensity(&pixel_info->foreground_color) <
                PixelIntensity(&pixel_info->background_color) ?  0x01 : 0x00);
              foreground=(PixelIntensity(&pixel_info->background_color) <
                PixelIntensity(&pixel_info->foreground_color) ?  0x01 : 0x00);
              if (windows->magnify.depth > 1)
                Swap(background,foreground);
            }
          for (i=0; i < (long) height; i+=magnify)
          {
            /*
              Propogate pixel magnify rows.
            */
            for (j=0; j < magnify; j++)
            {
              p=(unsigned char *) ximage->data+y*ximage->bytes_per_line+
                ((x*ximage->bits_per_pixel) >> 3);
              p_bit=(x*ximage->bits_per_pixel) & 0x07;
              q_bit=0;
              byte=0;
              for (k=0; k < width; k+=magnify)
              {
                /*
                  Propogate pixel magnify columns.
                */
                for (l=0; l < magnify; l++)
                {
                  /*
                    Propogate each bit plane.
                  */
                  for (plane=0; (int) plane < ximage->bits_per_pixel; plane++)
                  {
                    byte<<=1;
                    if (*p & (0x80 >> (p_bit+plane)))
                      byte|=foreground;
                    else
                      byte|=background;
                    q_bit++;
                    if (q_bit == 8)
                      {
                        *q++=byte;
                        q_bit=0;
                        byte=0;
                      }
                  }
                }
                p_bit+=ximage->bits_per_pixel;
                if (p_bit == 8)
                  {
                    p++;
                    p_bit=0;
                  }
                if (q_bit != 0)
                  *q=byte << (8-q_bit);
                q+=scanline_pad;
              }
            }
            y++;
          }
          break;
        }
      }
    }
  else
    switch (ximage->bits_per_pixel)
    {
      case 6:
      case 8:
      {
        /*
          Magnify 8 bit X image.
        */
        for (i=0; i < (long) height; i+=magnify)
        {
          /*
            Propogate pixel magnify rows.
          */
          for (j=0; j < magnify; j++)
          {
            p=(unsigned char *) ximage->data+y*ximage->bytes_per_line+
              ((x*ximage->bits_per_pixel) >> 3);
            for (k=0; k < width; k+=magnify)
            {
              /*
                Propogate pixel magnify columns.
              */
              for (l=0; l < magnify; l++)
                *q++=(*p);
              p++;
            }
            q+=scanline_pad;
          }
          y++;
        }
        break;
      }
      default:
      {
        register unsigned int
          bytes_per_pixel,
          m;

        /*
          Magnify multi-byte X image.
        */
        bytes_per_pixel=ximage->bits_per_pixel >> 3;
        for (i=0; i < (long) height; i+=magnify)
        {
          /*
            Propogate pixel magnify rows.
          */
          for (j=0; j < magnify; j++)
          {
            p=(unsigned char *) ximage->data+y*ximage->bytes_per_line+
              ((x*ximage->bits_per_pixel) >> 3);
            for (k=0; k < width; k+=magnify)
            {
              /*
                Propogate pixel magnify columns.
              */
              for (l=0; l < magnify; l++)
                for (m=0; m < bytes_per_pixel; m++)
                  *q++=(*(p+m));
              p+=bytes_per_pixel;
            }
            q+=scanline_pad;
          }
          y++;
        }
        break;
      }
    }
  /*
    Copy X image to magnify pixmap.
  */
  x=windows->magnify.x-((width/magnify) >> 1);
  if (x < 0)
    x=(width >> 1)-windows->magnify.x*magnify;
  else
    if (x > (int) (ximage->width-(width/magnify)))
      x=(ximage->width-windows->magnify.x)*magnify-(width >> 1);
    else
      x=0;
  y=windows->magnify.y-((height/magnify) >> 1);
  if (y < 0)
    y=(height >> 1)-windows->magnify.y*magnify;
  else
    if (y > (int) (ximage->height-(height/magnify)))
      y=(ximage->height-windows->magnify.y)*magnify-(height >> 1);
    else
      y=0;
  if ((x != 0) || (y != 0))
    (void) XFillRectangle(display,windows->magnify.pixmap,
      windows->magnify.annotate_context,0,0,width,height);
  (void) XPutImage(display,windows->magnify.pixmap,
    windows->magnify.annotate_context,windows->magnify.ximage,0,0,x,y,width-x,
    height-y);
  if ((magnify > 1) && ((magnify <= (width >> 1)) &&
      (magnify <= (height >> 1))))
    {
      RectangleInfo
        highlight_info;

      /*
        Highlight center pixel.
      */
      highlight_info.x=(long) windows->magnify.width >> 1;
      highlight_info.y=(long) windows->magnify.height >> 1;
      highlight_info.width=magnify;
      highlight_info.height=magnify;
      (void) XDrawRectangle(display,windows->magnify.pixmap,
        windows->magnify.highlight_context,(int) highlight_info.x,
        (int) highlight_info.y,(unsigned int) highlight_info.width-1,
        (unsigned int) highlight_info.height-1);
      if (magnify > 2)
        (void) XDrawRectangle(display,windows->magnify.pixmap,
          windows->magnify.annotate_context,(int) highlight_info.x+1,
          (int) highlight_info.y+1,(unsigned int) highlight_info.width-3,
          (unsigned int) highlight_info.height-3);
    }
  /*
    Show center pixel color.
  */
  color=GetOnePixel(windows->image.image,windows->magnify.x,windows->magnify.y);
  GetColorTuple(&color,windows->image.image->depth,windows->image.image->matte,
    False,tuple);
  FormatString(text," %+d%+d  %.1024s ",windows->magnify.x,windows->magnify.y,
    tuple);
  height=windows->magnify.font_info->ascent+windows->magnify.font_info->descent;
  x=windows->magnify.font_info->max_bounds.width >> 1;
  y=windows->magnify.font_info->ascent+(height >> 2);
  (void) XDrawImageString(display,windows->magnify.pixmap,
    windows->magnify.annotate_context,x,y,text,(int) strlen(text));
  y+=height;
  (void) QueryColorname(windows->image.image,&color,X11Compliance,text,
    &windows->image.image->exception);
  (void) XDrawImageString(display,windows->magnify.pixmap,
    windows->magnify.annotate_context,x,y,text,(int) strlen(text));
  /*
    Refresh magnify window.
  */
  magnify_window=windows->magnify;
  magnify_window.x=0;
  magnify_window.y=0;
  XRefreshWindow(display,&magnify_window,(XEvent *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e P i x m a p                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XMakePixmap creates an X11 pixmap.
%
%  The format of the XMakePixmap method is:
%
%      void XMakeStandardColormap(Display *display,XVisualInfo *visual_info,
%        XResourceInfo *resource_info,Image *image,XStandardColormap *map_info,
%        XPixelInfo *pixel)
%
%  A description of each parameter follows:
%
%    o status: Method XMakePixmap returns True if the X pixmap is
%      successfully created.  False is returned is there is a memory shortage.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a XWindowInfo structure.
%
%
*/
static unsigned int XMakePixmap(Display *display,
  const XResourceInfo *resource_info,XWindowInfo *window)
{
  unsigned int
    height,
    width;

  assert(display != (Display *) NULL);
  assert(resource_info != (XResourceInfo *) NULL);
  assert(window != (XWindowInfo  *) NULL);
  if (window->pixmap != (Pixmap) NULL)
    {
      /*
        Destroy previous X pixmap.
      */
      (void) XFreePixmap(display,window->pixmap);
      window->pixmap=(Pixmap) NULL;
    }
  if (!window->use_pixmap)
    return(False);
  if (window->ximage == (XImage *) NULL)
    return(False);
  /*
    Display busy cursor.
  */
  (void) XDefineCursor(display,window->id,window->busy_cursor);
  (void) XFlush(display);
  /*
    Create pixmap.
  */
  width=window->ximage->width;
  height=window->ximage->height;
  window->pixmap=XCreatePixmap(display,window->id,width,height,window->depth);
  if (window->pixmap == (Pixmap) NULL)
    {
      /*
        Unable to allocate pixmap.
      */
      (void) XDefineCursor(display,window->id,window->cursor);
      return(False);
    }
  /*
    Copy X image to pixmap.
  */
#if defined(HasSharedMemory)
  if (window->shared_memory)
    (void) XShmPutImage(display,window->pixmap,window->annotate_context,window->ximage,
      0,0,0,0,width,height,True);
#endif
  if (!window->shared_memory)
    (void) XPutImage(display,window->pixmap,window->annotate_context,
      window->ximage,0,0,0,0,width,height);
  if (IsEventLogging())
    {
      LogMagickEvent(X11Event,"Pixmap:");
      LogMagickEvent(X11Event,"  width, height: %ux%u",width,height);
      LogMagickEvent(X11Event,"  timestamp: %ld",time((time_t *) NULL));
    }
  /*
    Restore cursor.
  */
  (void) XDefineCursor(display,window->id,window->cursor);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e S t a n d a r d C o l o r m a p                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XMakeStandardColormap creates an X11 Standard Colormap.
%
%  The format of the XMakeStandardColormap method is:
%
%      XMakeStandardColormap(display,visual_info,resource_info,image,
%        map_info,pixel)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o visual_info: Specifies a pointer to a X11 XVisualInfo structure;
%      returned from XGetVisualInfo.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o map_info: If a Standard Colormap type is specified, this structure is
%      initialized with info from the Standard Colormap.
%
%    o pixel: Specifies a pointer to a XPixelInfo structure.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static int IntensityCompare(const void *x,const void *y)
{
  DiversityPacket
    *color_1,
    *color_2;

  color_1=(DiversityPacket *) x;
  color_2=(DiversityPacket *) y;
  return((int) (PixelIntensity(color_2)-PixelIntensity(color_1)));
}

static int PopularityCompare(const void *x,const void *y)
{
  DiversityPacket
    *color_1,
    *color_2;

  color_1=(DiversityPacket *) x;
  color_2=(DiversityPacket *) y;
  return((int) color_2->count-(int) color_1->count);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

MagickExport void XMakeStandardColormap(Display *display,
  XVisualInfo *visual_info,XResourceInfo *resource_info,Image *image,
  XStandardColormap *map_info,XPixelInfo *pixel)
{
  Colormap
    colormap;

  int
    status;

  register IndexPacket
    *indexes;

  register PixelPacket
    *q;

  register long
    i;

  unsigned long
    number_colors,
    retain_colors;

  unsigned short
    gray_value;

  XColor
    color,
    *colors,
    *p;

  assert(display != (Display *) NULL);
  assert(visual_info != (XVisualInfo *) NULL);
  assert(map_info != (XStandardColormap *) NULL);
  assert(resource_info != (XResourceInfo *) NULL);
  assert(pixel != (XPixelInfo *) NULL);
  if (resource_info->map_type != (char *) NULL)
    {
      /*
        Standard Colormap is already defined (i.e. xstdcmap).
      */
      XGetPixelPacket(display,visual_info,map_info,resource_info,image,
        pixel);
      number_colors=(unsigned int) (map_info->base_pixel+
        (map_info->red_max+1)*(map_info->green_max+1)*(map_info->blue_max+1));
      if ((map_info->red_max*map_info->green_max*map_info->blue_max) != 0)
        if (!image->matte && !resource_info->color_recovery &&
            resource_info->quantize_info->dither &&
            ((int) number_colors < MaxColormapSize))
          {
            Image
              *map_image;

            /*
              Improve image appearance with error diffusion.
            */
            map_image=AllocateImage((ImageInfo *) NULL);
            if (map_image == (Image *) NULL)
              MagickFatalError(ResourceLimitFatalError,"Unable to dither image",
                "MemoryAllocationFailed");
            map_image->columns=number_colors;
            map_image->rows=1;
            /*
              Initialize colormap image.
            */
            q=SetImagePixels(map_image,0,0,map_image->columns,1);
            if (q != (PixelPacket *) NULL)
              {
                for (i=0; i < (long) number_colors; i++)
                {
                  q->red=0;
                  if (map_info->red_max != 0)
                    q->red=(Quantum) (((double) MaxRGB*
                      (i/map_info->red_mult))/map_info->red_max+0.5);
                  q->green=0;
                  if (map_info->green_max != 0)
                    q->green=(Quantum) (((double) MaxRGB*
                      ((i/map_info->green_mult) % (map_info->green_max+1)))/
                      map_info->green_max+0.5);
                  q->blue=0;
                  if (map_info->blue_max != 0)
                    q->blue=(Quantum) (((double) MaxRGB*
                      (i % map_info->green_mult))/map_info->blue_max+0.5);
                  q->opacity=TransparentOpacity;
                  q++;
                }
                (void) SyncImagePixels(map_image);
                (void) MapImage(image,map_image,True);
              }
            XGetPixelPacket(display,visual_info,map_info,resource_info,image,
              pixel);
            SetImageType(image,TrueColorType);
            DestroyImage(map_image);
          }
      if (IsEventLogging())
        {
          LogMagickEvent(X11Event,"Standard Colormap:");
          LogMagickEvent(X11Event,"  colormap id: 0x%lx",map_info->colormap);
          LogMagickEvent(X11Event,"  red, green, blue max: %lu %lu %lu",
            map_info->red_max,map_info->green_max,map_info->blue_max);
          LogMagickEvent(X11Event,"  red, green, blue mult: %lu %lu %lu",
            map_info->red_mult,map_info->green_mult,map_info->blue_mult);
          LogMagickEvent(X11Event,"  timestamp: %ld",time((time_t *) NULL));
        }
      return;
    }
  if ((visual_info->storage_class != DirectColor) &&
      (visual_info->storage_class != TrueColor))
    if ((image->storage_class == DirectClass) ||
        ((int) image->colors > visual_info->colormap_size))
      {
        QuantizeInfo
          quantize_info;

        /*
          Image has more colors than the visual supports.
        */
        quantize_info=(*resource_info->quantize_info);
        quantize_info.number_colors=visual_info->colormap_size;
        (void) QuantizeImage(&quantize_info,image);
      }
  /*
    Free previous and create new colormap.
  */
  (void) XFreeStandardColormap(display,visual_info,map_info,pixel);
  colormap=XDefaultColormap(display,visual_info->screen);
  if (visual_info->visual != XDefaultVisual(display,visual_info->screen))
    colormap=XCreateColormap(display,XRootWindow(display,visual_info->screen),
      visual_info->visual,visual_info->storage_class == DirectColor ?
      AllocAll : AllocNone);
  if (colormap == (Colormap) NULL)
    MagickFatalError(XServerFatalError,"Unable to create colormap",
      (char *) NULL);
  /*
    Initialize the map and pixel info structures.
  */
  XGetMapInfo(visual_info,colormap,map_info);
  XGetPixelPacket(display,visual_info,map_info,resource_info,image,pixel);
  /*
    Allocating colors in server colormap is based on visual class.
  */
  switch (visual_info->storage_class)
  {
    case StaticGray:
    case StaticColor:
    {
      /*
        Define Standard Colormap for StaticGray or StaticColor visual.
      */
      number_colors=image->colors;
      colors=(XColor *)
        AcquireMemory(visual_info->colormap_size*sizeof(XColor));
      if (colors == (XColor *) NULL)
        MagickFatalError(XServerFatalError,"Unable to create colormap",
          "MemoryAllocationFailed");
      p=colors;
      color.flags=DoRed | DoGreen | DoBlue;
      for (i=0; i < (long) image->colors; i++)
      {
        color.red=ScaleQuantumToShort(XRedGamma(image->colormap[i].red));
        color.green=ScaleQuantumToShort(XGreenGamma(image->colormap[i].green));
        color.blue=ScaleQuantumToShort(XBlueGamma(image->colormap[i].blue));
        if (visual_info->storage_class != StaticColor)
          {
            gray_value=(unsigned short) PixelIntensity(&color);
            color.red=gray_value;
            color.green=gray_value;
            color.blue=gray_value;
          }
        status=XAllocColor(display,colormap,&color);
        if (status == 0)
          {
            colormap=XCopyColormapAndFree(display,colormap);
            (void) XAllocColor(display,colormap,&color);
          }
        pixel->pixels[i]=color.pixel;
        *p++=color;
      }
      break;
    }
    case GrayScale:
    case PseudoColor:
    {
      unsigned int
        colormap_type;

      /*
        Define Standard Colormap for GrayScale or PseudoColor visual.
      */
      number_colors=image->colors;
      colors=(XColor *)
        AcquireMemory(visual_info->colormap_size*sizeof(XColor));
      if (colors == (XColor *) NULL)
        MagickFatalError(ResourceLimitFatalError,"Unable to create colormap",
          "MemoryAllocationFailed");
      /*
        Preallocate our GUI colors.
      */
      (void) XAllocColor(display,colormap,&pixel->foreground_color);
      (void) XAllocColor(display,colormap,&pixel->background_color);
      (void) XAllocColor(display,colormap,&pixel->border_color);
      (void) XAllocColor(display,colormap,&pixel->matte_color);
      (void) XAllocColor(display,colormap,&pixel->highlight_color);
      (void) XAllocColor(display,colormap,&pixel->shadow_color);
      (void) XAllocColor(display,colormap,&pixel->depth_color);
      (void) XAllocColor(display,colormap,&pixel->trough_color);
      for (i=0; i < MaxNumberPens; i++)
        (void) XAllocColor(display,colormap,&pixel->pen_colors[i]);
      /*
        Determine if image colors will "fit" into X server colormap.
      */
      colormap_type=resource_info->colormap;
      status=XAllocColorCells(display,colormap,False,(unsigned long *) NULL,0,
        pixel->pixels,(int) image->colors);
      if (status != 0)
        colormap_type=PrivateColormap;
      if (colormap_type == SharedColormap)
        {
          DiversityPacket
            *diversity;

          int
            y;

          register int
            x;

          unsigned short
            index;

          XColor
            *server_colors;

          /*
            Define Standard colormap for shared GrayScale or PseudoColor visual.
          */
          diversity=(DiversityPacket *)
            AcquireMemory(image->colors*sizeof(DiversityPacket));
          if (diversity == (DiversityPacket *) NULL)
            MagickFatalError(ResourceLimitFatalError,
              "Unable to create colormap","MemoryAllocationFailed");
          for (i=0; i < (long) image->colors; i++)
          {
            diversity[i].red=image->colormap[i].red;
            diversity[i].green=image->colormap[i].green;
            diversity[i].blue=image->colormap[i].blue;
            diversity[i].index=(unsigned short) i;
            diversity[i].count=0;
          }
          for (y=0; y < (long) image->rows; y++)
          {
            q=GetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            indexes=GetIndexes(image);
            for (x=0; x < (long) image->columns; x++)
              diversity[indexes[x]].count++;
          }
          /*
            Sort colors by decreasing intensity.
          */
          qsort((void *) diversity,image->colors,sizeof(DiversityPacket),
            IntensityCompare);
          for (i=0; i < (long) image->colors; i+=Max(image->colors >> 4,2))
            diversity[i].count<<=4;  /* increase this colors popularity */
          diversity[image->colors-1].count<<=4;
          qsort((void *) diversity,image->colors,sizeof(DiversityPacket),
            PopularityCompare);
          /*
            Allocate colors.
          */
          p=colors;
          color.flags=DoRed | DoGreen | DoBlue;
          for (i=0; i < (long) image->colors; i++)
          {
            index=diversity[i].index;
            color.red=
              ScaleQuantumToShort(XRedGamma(image->colormap[index].red));
            color.green=
              ScaleQuantumToShort(XGreenGamma(image->colormap[index].green));
            color.blue=
              ScaleQuantumToShort(XBlueGamma(image->colormap[index].blue));
            if (visual_info->storage_class != PseudoColor)
              {
                gray_value=(unsigned short) PixelIntensity(&color);
                color.red=gray_value;
                color.green=gray_value;
                color.blue=gray_value;
              }
            status=XAllocColor(display,colormap,&color);
            if (status == 0)
              break;
            pixel->pixels[index]=color.pixel;
            *p++=color;
          }
          /*
            Read X server colormap.
          */
          server_colors=(XColor *)
            AcquireMemory(visual_info->colormap_size*sizeof(XColor));
          if (server_colors == (XColor *) NULL)
            MagickFatalError(ResourceLimitFatalError,
              "Unable to create colormap","MemoryAllocationFailed");
          for (x=0; x < visual_info->colormap_size; x++)
            server_colors[x].pixel=(unsigned long) x;
          (void) XQueryColors(display,colormap,server_colors,
            (int) Min(visual_info->colormap_size,256));
          /*
            Select remaining colors from X server colormap.
          */
          for (; i < (long) image->colors; i++)
          {
            index=diversity[i].index;
            color.red=
              ScaleQuantumToShort(XRedGamma(image->colormap[index].red));
            color.green=
              ScaleQuantumToShort(XGreenGamma(image->colormap[index].green));
            color.blue=
              ScaleQuantumToShort(XBlueGamma(image->colormap[index].blue));
            if (visual_info->storage_class != PseudoColor)
              {
                gray_value=(unsigned short) PixelIntensity(&color);
                color.red=gray_value;
                color.green=gray_value;
                color.blue=gray_value;
              }
            XBestPixel(display,colormap,server_colors,(unsigned int)
              visual_info->colormap_size,&color);
            pixel->pixels[index]=color.pixel;
            *p++=color;
          }
          if ((int) image->colors < visual_info->colormap_size)
            {
              /*
                Fill up colors array-- more choices for pen colors.
              */
              retain_colors=Min(visual_info->colormap_size-image->colors,256);
              for (i=0; i < (long) retain_colors; i++)
                *p++=server_colors[i];
              number_colors+=retain_colors;
            }
          LiberateMemory((void **) &server_colors);
          LiberateMemory((void **) &diversity);
          break;
        }
      /*
        Define Standard colormap for private GrayScale or PseudoColor visual.
      */
      if (status == 0)
        {
          /*
            Not enough colormap entries in the colormap-- Create a new colormap.
          */
          colormap=XCreateColormap(display,
            XRootWindow(display,visual_info->screen),visual_info->visual,
            AllocNone);
          if (colormap == (Colormap) NULL)
            MagickFatalError(XServerFatalError,"Unable to create colormap",
              (char *) NULL);
          map_info->colormap=colormap;
          if ((int) image->colors < visual_info->colormap_size)
            {
              /*
                Retain colors from the default colormap to help lessens the
                effects of colormap flashing.
              */
              retain_colors=Min(visual_info->colormap_size-image->colors,256);
              p=colors+image->colors;
              for (i=0; i < (long) retain_colors; i++)
              {
                p->pixel=(unsigned long) i;
                p++;
              }
              (void) XQueryColors(display,
                XDefaultColormap(display,visual_info->screen),
                colors+image->colors,(int) retain_colors);
              /*
                Transfer colors from default to private colormap.
              */
              (void) XAllocColorCells(display,colormap,False,(unsigned long *)
                NULL,0,pixel->pixels,(int) retain_colors);
              p=colors+image->colors;
              for (i=0; i < (long) retain_colors; i++)
              {
                p->pixel=pixel->pixels[i];
                p++;
              }
              (void) XStoreColors(display,colormap,colors+image->colors,
                (int) retain_colors);
              number_colors+=retain_colors;
            }
          (void) XAllocColorCells(display,colormap,False,(unsigned long *) NULL,
            0,pixel->pixels,(int) image->colors);
        }
      /*
        Store the image colormap.
      */
      p=colors;
      color.flags=DoRed | DoGreen | DoBlue;
      for (i=0; i < (long) image->colors; i++)
      {
        color.red=ScaleQuantumToShort(XRedGamma(image->colormap[i].red));
        color.green=ScaleQuantumToShort(XGreenGamma(image->colormap[i].green));
        color.blue=ScaleQuantumToShort(XBlueGamma(image->colormap[i].blue));
        if (visual_info->storage_class != PseudoColor)
          {
            gray_value=(unsigned short) PixelIntensity(&color);
            color.red=gray_value;
            color.green=gray_value;
            color.blue=gray_value;
          }
        color.pixel=pixel->pixels[i];
        *p++=color;
      }
      (void) XStoreColors(display,colormap,colors,(int) image->colors);
      break;
    }
    case TrueColor:
    case DirectColor:
    default:
    {
      unsigned int
        linear_colormap;

      /*
        Define Standard Colormap for TrueColor or DirectColor visual.
      */
      number_colors=(unsigned int) ((map_info->red_max*map_info->red_mult)+
        (map_info->green_max*map_info->green_mult)+
        (map_info->blue_max*map_info->blue_mult)+1);
      linear_colormap=(number_colors > 4096) ||
        (((int) (map_info->red_max+1) == visual_info->colormap_size) &&
         ((int) (map_info->green_max+1) == visual_info->colormap_size) &&
         ((int) (map_info->blue_max+1) == visual_info->colormap_size));
      if (linear_colormap)
        number_colors=visual_info->colormap_size;
      /*
        Allocate color array.
      */
      colors=(XColor *) AcquireMemory(number_colors*sizeof(XColor));
      if (colors == (XColor *) NULL)
        MagickFatalError(ResourceLimitFatalError,"Unable to create colormap",
          "MemoryAllocationFailed");
      /*
        Initialize linear color ramp.
      */
      p=colors;
      color.flags=DoRed | DoGreen | DoBlue;
      if (linear_colormap)
        for (i=0; i < (long) number_colors; i++)
        {
          color.blue=(unsigned short) 0;
          if (map_info->blue_max != 0)
            color.blue=(unsigned short) ((unsigned long)
              ((65535L*(i % map_info->green_mult))/map_info->blue_max));
          color.green=color.blue;
          color.red=color.blue;
          color.pixel=XStandardPixel(map_info,&color);
          *p++=color;
        }
      else
        for (i=0; i < (long) number_colors; i++)
        {
          color.red=(unsigned short) 0;
          if (map_info->red_max != 0)
            color.red=(unsigned short) ((unsigned long)
              ((65535L*(i/map_info->red_mult))/map_info->red_max));
          color.green=(unsigned int) 0;
          if (map_info->green_max != 0)
            color.green=(unsigned short) ((unsigned long)
              ((65535L*((i/map_info->green_mult) % (map_info->green_max+1)))/
                map_info->green_max));
          color.blue=(unsigned short) 0;
          if (map_info->blue_max != 0)
            color.blue=(unsigned short) ((unsigned long)
              ((65535L*(i % map_info->green_mult))/map_info->blue_max));
          color.pixel=XStandardPixel(map_info,&color);
          *p++=color;
        }
      if ((visual_info->storage_class == DirectColor) &&
          (colormap != XDefaultColormap(display,visual_info->screen)))
        (void) XStoreColors(display,colormap,colors,(int) number_colors);
      else
        for (i=0; i < (long) number_colors; i++)
          (void) XAllocColor(display,colormap,&colors[i]);
      break;
    }
  }
  if ((visual_info->storage_class != DirectColor) &&
      (visual_info->storage_class != TrueColor))
    {
      /*
        Set foreground, background, border, etc. pixels.
      */
      XBestPixel(display,colormap,colors,(int) number_colors,
        &pixel->foreground_color);
      XBestPixel(display,colormap,colors,(int) number_colors,
        &pixel->background_color);
      if (pixel->background_color.pixel == pixel->foreground_color.pixel)
        {
          /*
            Foreground and background colors must differ.
          */
          pixel->background_color.red=(~pixel->foreground_color.red);
          pixel->background_color.green=
            (~pixel->foreground_color.green);
          pixel->background_color.blue=
            (~pixel->foreground_color.blue);
          XBestPixel(display,colormap,colors,(int) number_colors,
            &pixel->background_color);
        }
      XBestPixel(display,colormap,colors,(int) number_colors,
        &pixel->border_color);
      XBestPixel(display,colormap,colors,(int) number_colors,
        &pixel->matte_color);
      XBestPixel(display,colormap,colors,(int) number_colors,
        &pixel->highlight_color);
      XBestPixel(display,colormap,colors,(int) number_colors,
        &pixel->shadow_color);
      XBestPixel(display,colormap,colors,(int) number_colors,
        &pixel->depth_color);
      XBestPixel(display,colormap,colors,(int) number_colors,
        &pixel->trough_color);
      for (i=0; i < MaxNumberPens; i++)
      {
        XBestPixel(display,colormap,colors,(int) number_colors,
          &pixel->pen_colors[i]);
        pixel->pixels[image->colors+i]=pixel->pen_colors[i].pixel;
      }
      pixel->colors=image->colors+MaxNumberPens;
    }
  LiberateMemory((void **) &colors);
  if (IsEventLogging())
    {
      LogMagickEvent(X11Event,"Standard Colormap:");
      LogMagickEvent(X11Event,"  colormap id: 0x%lx",map_info->colormap);
      LogMagickEvent(X11Event,"  red, green, blue max: %lu %lu %lu",
        map_info->red_max,map_info->green_max,map_info->blue_max);
      LogMagickEvent(X11Event,"  red, green, blue mult: %lu %lu %lu",
        map_info->red_mult,map_info->green_mult,map_info->blue_mult);
      LogMagickEvent(X11Event,"  timestamp: %ld",time((time_t *) NULL));
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e W i n d o w                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XMakeWindow creates an X11 window.
%
%  The format of the XMakeWindow method is:
%
%      void XMakeWindow(Display *display,Window parent,char **argv,int argc,
%        XClassHint *class_hint,XWMHints *manager_hints,
%        XWindowInfo *window_info)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o parent: Specifies the parent window_info.
%
%    o argv: Specifies the application's argument list.
%
%    o argc: Specifies the number of arguments.
%
%    o class_hint: Specifies a pointer to a X11 XClassHint structure.
%
%    o manager_hints: Specifies a pointer to a X11 XWMHints structure.
%
%    o window_info: Specifies a pointer to a X11 XWindowInfo structure.
%
%
*/
MagickExport void XMakeWindow(Display *display,Window parent,char **argv,
  int argc,XClassHint *class_hint,XWMHints *manager_hints,
  XWindowInfo *window_info)
{
#define MinWindowSize  64

  Atom
    atom_list[2];

  int
    gravity,
    status;

  static XTextProperty
    icon_name,
    window_name;

  XSizeHints
    *size_hints;

  /*
    Set window info hints.
  */
  assert(display != (Display *) NULL);
  assert(window_info != (XWindowInfo *) NULL);
  size_hints=XAllocSizeHints();
  if (size_hints == (XSizeHints *) NULL)
    MagickFatalError(ResourceLimitFatalError,"Unable to make X window",
      "MemoryAllocationFailed");
  size_hints->flags=(long) window_info->flags;
  size_hints->x=window_info->x;
  size_hints->y=window_info->y;
  size_hints->width=window_info->width;
  size_hints->height=window_info->height;
  if (window_info->immutable)
    {
      /*
        Window size cannot be changed.
      */
      size_hints->min_width=size_hints->width;
      size_hints->min_height=size_hints->height;
      size_hints->max_width=size_hints->width;
      size_hints->max_height=size_hints->height;
      size_hints->flags|=PMinSize;
      size_hints->flags|=PMaxSize;
    }
  else
    {
      /*
        Window size can be changed.
      */
      size_hints->min_width=window_info->min_width;
      size_hints->min_height=window_info->min_height;
      size_hints->flags|=PResizeInc;
      size_hints->width_inc=window_info->width_inc;
      size_hints->height_inc=window_info->height_inc;
#if !defined(PRE_R4_ICCCM)
      size_hints->flags|=PBaseSize;
      size_hints->base_width=size_hints->width_inc;
      size_hints->base_height=size_hints->height_inc;
#endif
    }
  gravity=NorthWestGravity;
  if (window_info->geometry != (char *) NULL)
    {
      char
        default_geometry[MaxTextExtent],
        geometry[MaxTextExtent];

      int
        flags;

      register char
        *p;

      /*
        User specified geometry.
      */
      FormatString(default_geometry,"%dx%d",size_hints->width,
        size_hints->height);
      (void) strncpy(geometry,window_info->geometry,MaxTextExtent-1);
      p=geometry;
      while (strlen(p) != 0)
      {
        if (!isspace((int) (*p)) && (*p != '%'))
          p++;
        else
          (void) strcpy(p,p+1);
      }
      flags=XWMGeometry(display,window_info->screen,geometry,default_geometry,
        window_info->border_width,size_hints,&size_hints->x,&size_hints->y,
        &size_hints->width,&size_hints->height,&gravity);
      if ((flags & WidthValue) && (flags & HeightValue))
        size_hints->flags|=USSize;
      if ((flags & XValue) && (flags & YValue))
        {
          size_hints->flags|=USPosition;
          window_info->x=size_hints->x;
          window_info->y=size_hints->y;
        }
    }
#if !defined(PRE_R4_ICCCM)
  size_hints->win_gravity=gravity;
  size_hints->flags|=PWinGravity;
#endif
  if (window_info->id == (Window) NULL)
    window_info->id=XCreateWindow(display,parent,window_info->x,window_info->y,
      window_info->width,window_info->height,window_info->border_width,
      window_info->depth,InputOutput,window_info->visual,window_info->mask,
      &window_info->attributes);
  else
    {
      unsigned int
        mask;

      XEvent
        sans_event;

      XWindowChanges
        window_changes;

      /*
        Window already exists;  change relevant attributes.
      */
      (void) XChangeWindowAttributes(display,window_info->id,window_info->mask,
        &window_info->attributes);
      mask=ConfigureNotify;
      while (XCheckTypedWindowEvent(display,window_info->id,mask,&sans_event));
      window_changes.x=window_info->x;
      window_changes.y=window_info->y;
      window_changes.width=window_info->width;
      window_changes.height=window_info->height;
      mask=CWWidth | CWHeight;
      if (window_info->flags & USPosition)
        mask|=CWX | CWY;
      (void) XReconfigureWMWindow(display,window_info->id,window_info->screen,
        mask,&window_changes);
    }
  if (window_info->id == (Window) NULL)
    MagickFatalError(XServerFatalError,"Unable to create window",
      window_info->name);
  status=XStringListToTextProperty(&window_info->name,1,&window_name);
  if (status == 0)
    MagickFatalError(XServerFatalError,"Unable to create text property",
      window_info->name);
  status=XStringListToTextProperty(&window_info->icon_name,1,&icon_name);
  if (status == 0)
    MagickFatalError(XServerFatalError,"Unable to create text property",
      window_info->icon_name);
  if (window_info->icon_geometry != (char *) NULL)
    {
      int
        flags,
        height,
        width;

      /*
        User specified icon geometry.
      */
      size_hints->flags|=USPosition;
      flags=XWMGeometry(display,window_info->screen,window_info->icon_geometry,
        (char *) NULL,0,size_hints,&manager_hints->icon_x,
        &manager_hints->icon_y,&width,&height,&gravity);
      if ((flags & XValue) && (flags & YValue))
        manager_hints->flags|=IconPositionHint;
    }
  XSetWMProperties(display,window_info->id,&window_name,&icon_name,argv,argc,
    size_hints,manager_hints,class_hint);
  if (window_name.nitems != 0)
    (void) XFree((void *) window_name.value);
  if (icon_name.nitems != 0)
    (void) XFree((void *) icon_name.value);
  atom_list[0]=XInternAtom(display,"WM_DELETE_WINDOW",False);
  atom_list[1]=XInternAtom(display,"WM_TAKE_FOCUS",False);
  (void) XSetWMProtocols(display,window_info->id,atom_list,2);
  (void) XFree((void *) size_hints);
  if (window_info->shape)
    {
#if defined(HasShape)
      int
        error_base,
        event_base;

      /*
        Can we apply a non-rectangular shaping mask?
      */
      window_info->shape&=XShapeQueryExtension(display,&error_base,&event_base);
#else
      window_info->shape=False;
#endif
    }
  if (window_info->shared_memory)
    {
#if defined(HasSharedMemory)
      /*
        Can we we use shared memory with this window?
      */
      window_info->shared_memory&=XShmQueryExtension(display);
#else
      window_info->shared_memory=False;
#endif
    }
  window_info->image=(Image *) NULL;
  window_info->destroy=False;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X P r o g r e s s M o n i t o r                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XMagickMonitor displays the progress a task is making in
%  completing a task.
%
%  The format of the XMagickMonitor method is:
%
%      void XMagickMonitor(const char *task,
%        const ExtendedSignedIntegralType quantum,const size_t span,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o task: Identifies the task in progress.
%
%    o quantum: Specifies the quantum position within the span which represents
%      how much progress has been made in completing a task.
%
%    o span: Specifies the span relative to completing a task.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
MagickExport unsigned int XMagickMonitor(const char *task,
  const ExtendedSignedIntegralType quantum,const size_t span,
  ExceptionInfo *exception)
{
  XWindows
    *windows;

  windows=XSetWindows((XWindows *) ~0);
  if (windows == (XWindows *) NULL)
    return(True);
  XMonitorWidget(windows->display,windows,task,quantum,span);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X Q u e r y C o l o r D a t a b a s e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XQueryColorDatabase looks up a RGB values for a color given in the
%  target string.
%
%  The format of the XQueryColorDatabase method is:
%
%      unsigned int XQueryColorDatabase(const char *target,XColor *color)
%
%  A description of each parameter follows:
%
%    o status:  Method XQueryColorDatabase returns True if the RGB values
%      of the target color is defined, otherwise False is returned.
%
%    o target: Specifies the color to lookup in the X color database.
%
%    o color: A pointer to an PixelPacket structure.  The RGB value of the target
%      color is returned as this value.
%
%
*/
MagickExport unsigned int XQueryColorDatabase(const char *target,XColor *color)
{
  Colormap
    colormap;

  int
    status;

  static Display
    *display = (Display *) NULL;

  XColor
    xcolor;

  /*
    Initialize color return value.
  */
  assert(color != (XColor *) NULL);
  color->red=0;
  color->green=0;
  color->blue=0;
  color->flags=DoRed | DoGreen | DoBlue;
  if ((target == (char *) NULL) || (*target == '\0'))
    target="#ffffffffffff";
  /*
    Let the X server define the color for us.
  */
  if (display == (Display *) NULL)
    display=XOpenDisplay((char *) NULL);
  if (display == (Display *) NULL)
    {
      MagickError(XServerError,"ColorIsNotKnownToServer",target);
      return(False);
    }
  colormap=XDefaultColormap(display,XDefaultScreen(display));
  status=XParseColor(display,colormap,(char *) target,&xcolor);
  if (status == False)
    MagickError(XServerError,"ColorIsNotKnownToServer",target);
  else
    {
      color->red=xcolor.red;
      color->green=xcolor.green;
      color->blue=xcolor.blue;
      color->flags=xcolor.flags;
    }
  return(status != 0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X Q u e r y P o s i t i o n                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XQueryPosition gets the pointer coordinates relative to a window.
%
%  The format of the XQueryPosition method is:
%
%      void XQueryPosition(Display *display,const Window window,int *x,int *y)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a Window.
%
%    o x: Return the x coordinate of the pointer relative to the origin of the
%      window.
%
%    o y: Return the y coordinate of the pointer relative to the origin of the
%      window.
%
%
*/
MagickExport void XQueryPosition(Display *display,const Window window,int *x,int *y)
{
  int
    x_root,
    y_root;

  unsigned int
    mask;

  Window
    root_window;

  assert(display != (Display *) NULL);
  assert(window != (Window) NULL);
  assert(x != (int *) NULL);
  assert(y != (int *) NULL);
  (void) XQueryPointer(display,window,&root_window,&root_window,&x_root,&y_root,
    x,y,&mask);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X R e f r e s h W i n d o w                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XRefreshWindow refreshes an image in a X window.
%
%  The format of the XRefreshWindow method is:
%
%      void XRefreshWindow(Display *display,const XWindowInfo *window,
%        const XEvent *event)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a XWindowInfo structure.
%
%    o event: Specifies a pointer to a XEvent structure.  If it is NULL,
%      the entire image is refreshed.
%
%
*/
MagickExport void XRefreshWindow(Display *display,const XWindowInfo *window,
  const XEvent *event)
{
  int
    x,
    y;

  unsigned int
    height,
    width;

  assert(display != (Display *) NULL);
  assert(window != (XWindowInfo *) NULL);
  if (window->ximage == (XImage *) NULL)
    return;
  if (event != (XEvent *) NULL)
    {
      /*
        Determine geometry from expose event.
      */
      x=event->xexpose.x;
      y=event->xexpose.y;
      width=event->xexpose.width;
      height=event->xexpose.height;
    }
  else
    {
      XEvent
        sans_event;

      /*
        Refresh entire window; discard outstanding expose events.
      */
      x=0;
      y=0;
      width=window->width;
      height=window->height;
      while (XCheckTypedWindowEvent(display,window->id,Expose,&sans_event));
    }
  /*
    Check boundary conditions.
  */
  if ((window->ximage->width-(x+window->x)) < (int) width)
    width=window->ximage->width-(x+window->x);
  if ((window->ximage->height-(y+window->y)) < (int) height)
    height=window->ximage->height-(y+window->y);
  /*
    Refresh image.
  */
  (void) XSetClipMask(display,window->annotate_context,window->matte_pixmap);
  if (window->pixmap != (Pixmap) NULL)
    {
      if (window->depth > 1)
        (void) XCopyArea(display,window->pixmap,window->id,
          window->annotate_context,x+window->x,y+window->y,width,height,x,y);
      else
        (void) XCopyPlane(display,window->pixmap,window->id,
          window->highlight_context,x+window->x,y+window->y,width,height,
          x,y,1L);
    }
  else
    {
#if defined(HasSharedMemory)
      if (window->shared_memory)
        (void) XShmPutImage(display,window->id,window->annotate_context,
          window->ximage,x+window->x,y+window->y,x,y,width,height,True);
#endif
      if (!window->shared_memory)
        (void) XPutImage(display,window->id,window->annotate_context,
          window->ximage,x+window->x,y+window->y,x,y,width,height);
    }
  (void) XSetClipMask(display,window->annotate_context,None);
  (void) XFlush(display);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X R e m o t e C o m m a n d                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  XRemoteCommand() forces a remote display(1) to display the specified
%  image filename.
%
%  The format of the XRemoteCommand method is:
%
%      unsigned int XRemoteCommand(Display *display,const char *window,
%        const char *filename)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o window: Specifies the name or id of an X window.
%
%    o filename: The name of the image filename to display.
%
%
*/
MagickExport unsigned int XRemoteCommand(Display *display,const char *window,
  const char *filename)
{
  Atom
    remote_atom;

  Window
    remote_window,
    root_window;

  assert(filename != (char *) NULL);
  if (display == (Display *) NULL)
    display=XOpenDisplay((char *) NULL);
  if (display == (Display *) NULL)
    {
      MagickError(XServerError,"UnableToOpenXServer",(char *) NULL);
      return(False);
    }
  remote_atom=XInternAtom(display,"IM_PROTOCOLS",False);
  remote_window=(Window) NULL;
  root_window=XRootWindow(display,XDefaultScreen(display));
  if (window != (char *) NULL)
    {
      /*
        Search window hierarchy and identify any clients by name or ID.
      */
      if (isdigit((int) (*window)))
        remote_window=XWindowByID(display,root_window,(Window)
          strtol((char *) window,(char **) NULL,0));
      if (remote_window == (Window) NULL)
        remote_window=XWindowByName(display,root_window,window);
    }
  if (remote_window == (Window) NULL)
    remote_window=XWindowByProperty(display,root_window,remote_atom);
  if (remote_window == (Window) NULL)
    {
      MagickError(XServerError,"UnableToConnectToRemoteDisplay",(char *) NULL);
      return(False);
    }
  /*
    Send remote command.
  */
  remote_atom=XInternAtom(display,"IM_REMOTE_COMMAND",False);
  (void) XChangeProperty(display,remote_window,remote_atom,XA_STRING,8,
    PropModeReplace,(unsigned char *) filename,(int) strlen(filename));
  (void) XSync(display,False);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X R e t a i n W i n d o w C o l o r s                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XRetainWindowColors sets X11 color resources on a window.  This
%  preserves the colors associated with an image displayed on the window.
%
%  The format of the XRetainWindowColors method is:
%
%      void XRetainWindowColors(Display *display,const Window window)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a XWindowInfo structure.
%
%
*/
MagickExport void XRetainWindowColors(Display *display,const Window window)
{
  Atom
    property;

  Pixmap
    pixmap;

  /*
    Put property on the window.
  */
  assert(display != (Display *) NULL);
  assert(window != (Window) NULL);
  property=XInternAtom(display,"_XSETROOT_ID",False);
  if (property == (Atom) NULL)
    {
      MagickError(XServerError,"UnableToCreateProperty","_XSETROOT_ID");
      return;
    }
  pixmap=XCreatePixmap(display,window,1,1,1);
  if (pixmap == (Pixmap) NULL)
    {
      MagickError(XServerError,"UnabletoCreateBitmap",(char *) NULL);
      return;
    }
  (void) XChangeProperty(display,window,property,XA_PIXMAP,32,PropModeReplace,
    (unsigned char *) &pixmap,1);
  (void) XSetCloseDownMode(display,RetainPermanent);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X S e l e c t W i n d o w                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XSelectWindow allows a user to select a window using the mouse.  If
%  the mouse moves, a cropping rectangle is drawn and the extents of the
%  rectangle is returned in the crop_info structure.
%
%  The format of the XSelectWindow function is:
%
%      target_window=XSelectWindow(display,crop_info)
%
%  A description of each parameter follows:
%
%    o window: XSelectWindow returns the window id.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o crop_info: Specifies a pointer to a RectangleInfo structure.  It
%      contains the extents of any cropping rectangle.
%
%
*/
static Window XSelectWindow(Display *display,RectangleInfo *crop_info)
{
#define MinimumCropArea  (unsigned int) 9

  Cursor
    target_cursor;

  GC
    annotate_context;

  int
    presses,
    status,
    x_offset,
    y_offset;

  Window
    root_window,
    target_window;

  XEvent
    event;

  XGCValues
    context_values;

  /*
    Initialize graphic context.
  */
  assert(display != (Display *) NULL);
  assert(crop_info != (RectangleInfo *) NULL);
  root_window=XRootWindow(display,XDefaultScreen(display));
  context_values.background=XBlackPixel(display,XDefaultScreen(display));
  context_values.foreground=XWhitePixel(display,XDefaultScreen(display));
  context_values.function=GXinvert;
  context_values.plane_mask=
    context_values.background ^ context_values.foreground;
  context_values.subwindow_mode=IncludeInferiors;
  annotate_context=XCreateGC(display,root_window,GCBackground | GCForeground |
    GCFunction | GCSubwindowMode,&context_values);
  if (annotate_context == (GC) NULL)
    return(False);
  /*
    Grab the pointer using target cursor.
  */
  target_cursor=XMakeCursor(display,root_window,XDefaultColormap(display,
    XDefaultScreen(display)),(char *) "white",(char *) "black");
  status=XGrabPointer(display,root_window,False,(unsigned int)
    (ButtonPressMask | ButtonReleaseMask | ButtonMotionMask),GrabModeSync,
    GrabModeAsync,root_window,target_cursor,CurrentTime);
  if (status != GrabSuccess)
    {
      MagickError(XServerError,"UnableToGrabMouse",(char *) NULL);
      return(False);
    }
  /*
    Select a window.
  */
  crop_info->width=0;
  crop_info->height=0;
  presses=0;
  target_window=(Window) NULL;
  x_offset=0;
  y_offset=0;
  do
  {
    if ((crop_info->width*crop_info->height) >= MinimumCropArea)
      (void) XDrawRectangle(display,root_window,annotate_context,
        (int) crop_info->x,(int) crop_info->y,(unsigned int) crop_info->width-1,
        (unsigned int) crop_info->height-1);
    /*
      Allow another event.
    */
    (void) XAllowEvents(display,SyncPointer,CurrentTime);
    (void) XWindowEvent(display,root_window,ButtonPressMask |
      ButtonReleaseMask | ButtonMotionMask,&event);
    if ((crop_info->width*crop_info->height) >= MinimumCropArea)
      (void) XDrawRectangle(display,root_window,annotate_context,
        (int) crop_info->x,(int) crop_info->y,(unsigned int) crop_info->width-1,
        (unsigned int) crop_info->height-1);
    switch (event.type)
    {
      case ButtonPress:
      {
        target_window=XGetSubwindow(display,event.xbutton.subwindow,
          event.xbutton.x,event.xbutton.y);
        if (target_window == (Window) NULL)
          target_window=root_window;
        x_offset=event.xbutton.x_root;
        y_offset=event.xbutton.y_root;
        crop_info->x=x_offset;
        crop_info->y=y_offset;
        crop_info->width=0;
        crop_info->height=0;
        presses++;
        break;
      }
      case ButtonRelease:
      {
        presses--;
        break;
      }
      case MotionNotify:
      {
        /*
          Discard pending button motion events.
        */
        while (XCheckMaskEvent(display,ButtonMotionMask,&event));
        crop_info->x=event.xmotion.x;
        crop_info->y=event.xmotion.y;
        /*
          Check boundary conditions.
        */
        if ((int) crop_info->x < x_offset)
          crop_info->width=(unsigned int) (x_offset-crop_info->x);
        else
          {
            crop_info->width=(unsigned int) (crop_info->x-x_offset);
            crop_info->x=x_offset;
          }
        if ((int) crop_info->y < y_offset)
          crop_info->height=(unsigned int) (y_offset-crop_info->y);
        else
          {
            crop_info->height=(unsigned int) (crop_info->y-y_offset);
            crop_info->y=y_offset;
          }
      }
      default:
        break;
    }
  }
  while ((target_window == (Window) NULL) || (presses > 0));
  (void) XUngrabPointer(display,CurrentTime);
  (void) XFreeCursor(display,target_cursor);
  (void) XFreeGC(display,annotate_context);
  if ((crop_info->width*crop_info->height) < MinimumCropArea)
    {
      crop_info->width=0;
      crop_info->height=0;
    }
  if ((crop_info->width != 0) && (crop_info->height != 0))
    target_window=root_window;
  return(target_window);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X S i g n a l H a n d l e r                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XSignalHandler is called if the program execution is interrupted.
%
%  The format of the XSignalHandler method is:
%
%      void XSetCursorState(Display *display,XWindows *windows,
%        const unsigned int state)
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

MagickExport void XSignalHandler(int status)
{
  XWindows
    *windows;

  windows=XSetWindows((XWindows *) ~0);
  if (windows == (XWindows *) NULL)
    return;
#if defined(HasSharedMemory)
  if (windows->image.segment_info[0].shmid >= 0)
    {
      (void) shmdt(windows->image.segment_info[0].shmaddr);
      (void) shmctl(windows->image.segment_info[0].shmid,IPC_RMID,0);
    }
  if (windows->magnify.segment_info[0].shmid >= 0)
    {
      (void) shmdt(windows->magnify.segment_info[0].shmaddr);
      (void) shmctl(windows->magnify.segment_info[0].shmid,IPC_RMID,0);
    }
#endif
  DestroyMagick();
  Exit(status);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X S e t C u r s o r S t a t e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XSetCursorState sets the cursor state to busy, otherwise the
%  cursor are reset to their default.
%
%  The format of the XXSetCursorState method is:
%
%      XSetCursorState(display,windows,state)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o windows: Specifies a pointer to a XWindows structure.
%
%    o state: An unsigned integer greater than 0 sets the cursor state
%      to busy, otherwise the cursor are reset to their default.
%
%
*/
MagickExport void XSetCursorState(Display *display,XWindows *windows,
  const unsigned int state)
{
  assert(display != (Display *) NULL);
  assert(windows != (XWindows *) NULL);
  if (state)
    {
      (void) XDefineCursor(display,windows->image.id,
        windows->image.busy_cursor);
      (void) XDefineCursor(display,windows->pan.id,windows->pan.busy_cursor);
      (void) XDefineCursor(display,windows->magnify.id,
        windows->magnify.busy_cursor);
      (void) XDefineCursor(display,windows->command.id,
        windows->command.busy_cursor);
    }
  else
    {
      (void) XDefineCursor(display,windows->image.id,windows->image.cursor);
      (void) XDefineCursor(display,windows->pan.id,windows->pan.cursor);
      (void) XDefineCursor(display,windows->magnify.id,windows->magnify.cursor);
      (void) XDefineCursor(display,windows->command.id,windows->command.cursor);
      (void) XDefineCursor(display,windows->command.id,windows->widget.cursor);
      (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
    }
  windows->info.mapped=False;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X S e t W i n d o w s                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XSetWindows sets the X windows structure if the windows info
%  is specified.  Otherwise the current windows structure is returned.
%
%  The format of the XSetWindows method is:
%
%      XWindows *XSetWindows(XWindows *windows_info)
%
%  A description of each parameter follows:
%
%    o windows: Method XSetWindows returns a pointer to the XWindows
%      structure.
%
%    o windows_info: Initialize the Windows structure with this information.
%
*/
MagickExport XWindows *XSetWindows(XWindows *windows_info)
{
  static XWindows
    *windows = (XWindows *) NULL;

  if (windows_info != (XWindows *) ~0)
    windows=windows_info;
  return(windows);
}
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X U s e r P r e f e r e n c e s                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XUserPreferences saves the preferences in a configuration file in
%  the users' home directory.
%
%  The format of the XUserPreferences method is:
%
%      void XUserPreferences(XResourceInfo *resource_info)
%
%  A description of each parameter follows:
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%
*/
MagickExport void XUserPreferences(XResourceInfo *resource_info)
{
#if defined(PreferencesDefaults)
  char
    cache[MaxTextExtent],
    *client_name,
    filename[MaxTextExtent],
    specifier[MaxTextExtent];

  const char
    *value;

  XrmDatabase
    preferences_database;

  /*
    Save user preferences to the client configuration file.
  */
  assert(resource_info != (XResourceInfo *) NULL);
  client_name=SetClientName((char *) NULL);
  preferences_database=XrmGetStringDatabase("");
  FormatString(specifier,"%.1024s.backdrop",client_name);
  value=resource_info->backdrop ? "True" : "False";
  XrmPutStringResource(&preferences_database,specifier,(char *) value);
  FormatString(specifier,"%.1024s.colormap",client_name);
  value=resource_info->colormap == SharedColormap ? "Shared" : "Private";
  XrmPutStringResource(&preferences_database,specifier,(char *) value);
  FormatString(specifier,"%.1024s.confirmExit",client_name);
  value=resource_info->confirm_exit ? "True" : "False";
  XrmPutStringResource(&preferences_database,specifier,(char *) value);
  FormatString(specifier,"%.1024s.displayWarnings",client_name);
  value=resource_info->display_warnings ? "True" : "False";
  XrmPutStringResource(&preferences_database,specifier,(char *) value);
  FormatString(specifier,"%.1024s.dither",client_name);
  value=resource_info->quantize_info->dither ? "True" : "False";
  XrmPutStringResource(&preferences_database,specifier,(char *) value);
  FormatString(specifier,"%.1024s.gammaCorrect",client_name);
  value=resource_info->gamma_correct ? "True" : "False";
  XrmPutStringResource(&preferences_database,specifier,(char *) value);
  FormatString(specifier,"%.1024s.undoCache",client_name);
  FormatString(cache,"%lu",resource_info->undo_cache);
  XrmPutStringResource(&preferences_database,specifier,cache);
  FormatString(specifier,"%.1024s.usePixmap",client_name);
  value=resource_info->use_pixmap ? "True" : "False";
  XrmPutStringResource(&preferences_database,specifier,(char *) value);
  FormatString(filename,"%.1024s%.1024src",PreferencesDefaults,client_name);
  ExpandFilename(filename);
  XrmPutFileDatabase(preferences_database,filename);
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X V i s u a l C l a s s N a m e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XVisualClassName returns the visual class name as a character
%  string.
%
%  The format of the XVisualClassName method is:
%
%      char *XVisualClassName(const int visual_class)
%
%  A description of each parameter follows:
%
%    o visual_type: XVisualClassName returns the visual class as a character
%      string.
%
%    o class: Specifies the visual class.
%
%
*/
static char *XVisualClassName(const int visual_class)
{
  switch (visual_class)
  {
    case StaticGray: return((char *) "StaticGray");
    case GrayScale: return((char *) "GrayScale");
    case StaticColor: return((char *) "StaticColor");
    case PseudoColor: return((char *) "PseudoColor");
    case TrueColor: return((char *) "TrueColor");
    case DirectColor: return((char *) "DirectColor");
  }
  return((char *) "unknown visual class");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X W a r n i n g                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XWarning displays a warning reason in a Notice widget.
%
%  The format of the XWarning method is:
%
%      void XWarning(const unsigned int warning,const char *reason,
%        const char *description)
%
%  A description of each parameter follows:
%
%    o warning: Specifies the numeric warning category.
%
%    o reason: Specifies the reason to display before terminating the
%      program.
%
%    o description: Specifies any description to the reason.
%
%
*/
MagickExport void XWarning(const ExceptionType warning,const char *reason,
  const char *description)
{
  char
    text[MaxTextExtent];

  XWindows
    *windows;

  if (reason == (char *) NULL)
    return;
  (void) strncpy(text,reason,MaxTextExtent-1);
  (void) strcat(text,":");
  windows=XSetWindows((XWindows *) ~0);
  XNoticeWidget(windows->display,windows,text,(char *) description);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X W i n d o w B y I D                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XWindowByID locates a child window with a given ID.  If not window
%  with the given name is found, 0 is returned.   Only the window specified
%  and its subwindows are searched.
%
%  The format of the XWindowByID function is:
%
%      child=XWindowByID(display,window,id)
%
%  A description of each parameter follows:
%
%    o child: XWindowByID returns the window with the specified
%      id.  If no windows are found, XWindowByID returns 0.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o id: Specifies the id of the window to locate.
%
%
*/
MagickExport Window XWindowByID(Display *display,const Window root_window,
  const unsigned long id)
{
  RectangleInfo
    rectangle_info;

  register int
    i;

  unsigned int
    number_children;

  Window
    child,
    *children,
    window;

  assert(display != (Display *) NULL);
  assert(root_window != (Window) NULL);
  if (id == 0)
    return(XSelectWindow(display,&rectangle_info));
  if (root_window == id)
    return(id);
  if (!XQueryTree(display,root_window,&child,&child,&children,&number_children))
    return((Window) NULL);
  window=(Window) NULL;
  for (i=0; i < (int) number_children; i++)
  {
    /*
      Search each child and their children.
    */
    window=XWindowByID(display,children[i],id);
    if (window != (Window) NULL)
      break;
  }
  if (children != (Window *) NULL)
    (void) XFree((void *) children);
  return(window);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X W i n d o w B y N a m e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XWindowByName locates a window with a given name on a display.
%  If no window with the given name is found, 0 is returned. If more than
%  one window has the given name, the first one is returned.  Only root and
%  its children are searched.
%
%  The format of the XWindowByName function is:
%
%      window=XWindowByName(display,root_window,name)
%
%  A description of each parameter follows:
%
%    o window: XWindowByName returns the window id.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o root_window: Specifies the id of the root window.
%
%    o name: Specifies the name of the window to locate.
%
%
*/
MagickExport Window XWindowByName(Display *display,const Window root_window,
  const char *name)
{
  register int
    i;

  unsigned int
    number_children;

  Window
    *children,
    child,
    window;

  XTextProperty
    window_name;

  assert(display != (Display *) NULL);
  assert(root_window != (Window) NULL);
  assert(name != (char *) NULL);
  if (XGetWMName(display,root_window,&window_name) != 0)
    if (LocaleCompare((char *) window_name.value,name) == 0)
      return(root_window);
  if (!XQueryTree(display,root_window,&child,&child,&children,&number_children))
    return((Window) NULL);
  window=(Window) NULL;
  for (i=0; i < (int) number_children; i++)
  {
    /*
      Search each child and their children.
    */
    window=XWindowByName(display,children[i],name);
    if (window != (Window) NULL)
      break;
  }
  if (children != (Window *) NULL)
    (void) XFree((void *) children);
  return(window);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X W i n d o w B y P r o p e r y                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method XWindowByProperty locates a child window with a given property.
%  If no window with the given name is found, 0 is returned.  If more than
%  one window has the given property, the first one is returned.  Only the
%  window specified and its subwindows are searched.
%
%  The format of the XWindowByProperty function is:
%
%      child=XWindowByProperty(display,window,property)
%
%  A description of each parameter follows:
%
%    o child: XWindowByProperty returns the window id with the specified
%      property.  If no windows are found, XWindowByProperty returns 0.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o property: Specifies the property of the window to locate.
%
%
*/
MagickExport Window XWindowByProperty(Display *display,const Window window,
  const Atom property)
{
  Atom
    type;

  int
    format,
    status;

  unsigned char
    *data;

  unsigned int
    i,
    number_children;

  unsigned long
    after,
    number_items;

  Window
    child,
    *children,
    parent,
    root;

  assert(display != (Display *) NULL);
  assert(window != (Window) NULL);
  assert(property != (Atom) NULL);
  status=XQueryTree(display,window,&root,&parent,&children,&number_children);
  if (status == 0)
    return((Window) NULL);
  type=(Atom) NULL;
  child=(Window) NULL;
  for (i=0; (i < number_children) && (child == (Window) NULL); i++)
  {
    status=XGetWindowProperty(display,children[i],property,0L,0L,False,
      (Atom) AnyPropertyType,&type,&format,&number_items,&after,&data);
    if (data != NULL)
      (void) XFree((void *) data);
    if ((status == Success) && (type != (Atom) NULL))
      child=children[i];
  }
  for (i=0; (i < number_children) && (child == (Window) NULL); i++)
    child=XWindowByProperty(display,children[i],property);
  if (children != (Window *) NULL)
    (void) XFree((void *) children);
  return(child);
}
#endif
