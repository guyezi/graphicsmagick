/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        DDDD   RRRR    AAA   W   W                           %
%                        D   D  R   R  A   A  W   W                           %
%                        D   D  RRRR   AAAAA  W   W                           %
%                        D   D  R R    A   A  W W W                           %
%                        DDDD   R  R   A   A   W W                            %
%                                                                             %
%                                                                             %
%                     ImageMagick Image Drawing Methods                       %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated %
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
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"

/*
  Define declarations.
*/
#define BezierQuantum  200
#define MatteMatch(color,target,delta) \
  (ColorMatch(color,target,delta) && ((color).opacity == (target).opacity))
#define MaxStacksize  (1 << 15)
#define Push(up,left,right,delta) \
  if ((s < (segment_stack+MaxStacksize)) && (((up)+(delta)) >= 0) && \
      (((up)+(delta)) < (int) image->rows)) \
    { \
      s->y1=(up); \
      s->x1=(left); \
      s->x2=(right); \
      s->y2=(delta); \
      s++; \
    }

/*
  Typedef declaractions.
*/
typedef struct _PrimitiveInfo
{
  PrimitiveType
    primitive;

  PointInfo
    point;

  unsigned int
    coordinates;

  PaintMethod
    method;

  char
    *text;
} PrimitiveInfo;

/*
  Forward declarations
*/
static double
  GenerateCircle(PrimitiveInfo *,const PointInfo,const PointInfo),
  IntersectPrimitive(PrimitiveInfo *,const DrawInfo *,const PointInfo *,
    const int,Image *);

static unsigned int
  GeneratePath(PrimitiveInfo *,const char *);

static void
  GenerateArc(PrimitiveInfo *,const PointInfo,const PointInfo,const PointInfo,
    const double,const unsigned int,const unsigned int),
  GenerateBezier(PrimitiveInfo *),
  GenerateEllipse(PrimitiveInfo *,const PointInfo,const PointInfo,
    const PointInfo),
  GenerateLine(PrimitiveInfo *,const PointInfo,const PointInfo),
  GeneratePoint(PrimitiveInfo *,const PointInfo),
  GenerateRectangle(PrimitiveInfo *,const PointInfo,const PointInfo),
  GenerateRoundRectangle(PrimitiveInfo *,const PointInfo,const PointInfo,
    PointInfo);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e D r a w I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CloneDrawInfo makes a duplicate of the given draw info, or if
%  draw info is NULL, a new one.
%
%  The format of the CloneDrawInfo method is:
%
%      DrawInfo *CloneDrawInfo(const ImageInfo *image_info,
%        const DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o cloned_info: Method CloneDrawInfo returns a duplicate of the given
%      annotate info, or if annotate info is NULL a new one.
%
%    o image_info: a structure of type ImageInfo.
%
%    o draw_info: a structure of type DrawInfo.
%
%
*/
MagickExport DrawInfo *CloneDrawInfo(const ImageInfo *image_info,
  const DrawInfo *draw_info)
{
  DrawInfo
    *cloned_info;

  cloned_info=(DrawInfo *) AcquireMemory(sizeof(DrawInfo));
  if (cloned_info == (DrawInfo *) NULL)
    MagickError(ResourceLimitError,"Unable to clone draw info",
      "Memory allocation failed");
  if (draw_info == (DrawInfo *) NULL)
    {
      GetDrawInfo(image_info,cloned_info);
      return(cloned_info);
    }
  *cloned_info=(*draw_info);
  if (draw_info->primitive != (char *) NULL)
    cloned_info->primitive=AllocateString(draw_info->primitive);
  if (draw_info->font != (char *) NULL)
    cloned_info->font=AllocateString(draw_info->font);
  if (draw_info->tile != (Image *) NULL)
    cloned_info->tile=CloneImage(draw_info->tile,draw_info->tile->columns,
      draw_info->tile->rows,True,&draw_info->tile->exception);
  return(cloned_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o l o r F l o o d f i l l I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ColorFloodfillImage floodfills the designated area with a color.
%  The floodfill algorithm is strongly based on a similar algorithm in
%  "Graphics Gems" by Paul Heckbert.
%
%  The format of the ColorFloodfillImage method is:
%
%      unsigned int ColorFloodfillImage(Image *image,const DrawInfo *draw_info,
%        const PixelPacket target,const int x_offset,const int y_offset,
%        const PaintMethod method)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o draw_info: a structure of type DrawInfo.
%
%    o target: A PixelPacket structure.  This is the RGB value of the target
%      color.
%
%    o x,y: Unsigned integers representing the current location of the pen.
%
%    o method: drawing method of type PrimitiveType: floodfill or fill to
%      border.
%
%
*/
MagickExport unsigned int ColorFloodfillImage(Image *image,
  const DrawInfo *draw_info,const PixelPacket target,const int x_offset,
  const int y_offset,const PaintMethod method)
{
  int
    offset,
    skip,
    start,
    x1,
    x2,
    y;

  PixelPacket
    color;

  register IndexPacket
    *indexes;

  register int
    i,
    x;

  register PixelPacket
    *p,
    *q;

  register SegmentInfo
    *s;

  SegmentInfo
    *segment_stack;

  /*
    Check boundary conditions.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  if ((x_offset < 0) || (x_offset >= (int) image->columns))
    return(False);
  if ((y_offset < 0) || (y_offset >= (int) image->rows))
    return(False);
  /*
    Set floodfill color.
  */
  if (ColorMatch(draw_info->fill,target,image->fuzz))
    return(False);
  segment_stack=(SegmentInfo *)
    AcquireMemory(MaxStacksize*sizeof(SegmentInfo));
  if (segment_stack == (SegmentInfo *) NULL)
    ThrowBinaryException(ResourceLimitWarning,"Unable to floodfill image",
      image->filename);
  /*
    Track "hot" pixels with the image index packets.
  */
  image->storage_class=PseudoClass;
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    if (p == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (int) image->columns; x++)
      indexes[x]=False;
    if (!SyncImagePixels(image))
      break;
  }
  /*
    Push initial segment on stack.
  */
  x=x_offset;
  y=y_offset;
  start=0;
  s=segment_stack;
  Push(y,x,x,1);
  Push(y+1,x,x,-1);
  while (s > segment_stack)
  {
    /*
      Pop segment off stack.
    */
    s--;
    x1=(int) s->x1;
    x2=(int) s->x2;
    offset=(int) s->y2;
    y=(int) s->y1+offset;
    /*
      Recolor neighboring pixels.
    */
    q=GetImagePixels(image,0,y,x1+1,1);
    if (q == (PixelPacket *) NULL)
      break;
    q+=x1;
    indexes=GetIndexes(image);
    for (x=x1; x >= 0 ; x--)
    {
      if (method == FloodfillMethod)
        {
          if (!ColorMatch(*q,target,image->fuzz))
            break;
        }
      else
        if (ColorMatch(*q,target,image->fuzz) ||
            ColorMatch(*q,draw_info->fill,image->fuzz))
          break;
      indexes[x]=True;
      *q=draw_info->fill;
      q--;
    }
    if (!SyncImagePixels(image))
      break;
    skip=x >= x1;
    if (!skip)
      {
        start=x+1;
        if (start < x1)
          Push(y,start,x1-1,-offset);
        x=x1+1;
      }
    do
    {
      if (!skip)
        {
          if (x < image->columns)
            {
              q=GetImagePixels(image,x,y,image->columns-x,1);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=GetIndexes(image);
              for (i=0; x < (int) image->columns; x++)
              {
                if (method == FloodfillMethod)
                  {
                    if (!ColorMatch(*q,target,image->fuzz))
                      break;
                  }
                else
                  if (ColorMatch(*q,target,image->fuzz) ||
                      ColorMatch(*q,draw_info->fill,image->fuzz))
                    break;
                indexes[i++]=True;
                *q=draw_info->fill;
                q++;
              }
              if (!SyncImagePixels(image))
                break;
            }
          Push(y,start,x-1,offset);
          if (x > (x2+1))
            Push(y,x2+1,x-1,-offset);
        }
      skip=False;
      x++;
      if (x <= x2)
        {
          q=GetImagePixels(image,x,y,x2-x+1,1);
          if (q == (PixelPacket *) NULL)
            break;
          for ( ; x <= x2; x++)
          {
            if (method == FloodfillMethod)
              {
                if (ColorMatch(*q,target,image->fuzz))
                  break;
              }
            else
              if (!ColorMatch(*q,target,image->fuzz) &&
                  !ColorMatch(*q,draw_info->fill,image->fuzz))
                break;
            q++;
          }
        }
      start=x;
    } while (x <= x2);
  }
  if (draw_info->tile == (Image *) NULL)
    for (y=0; y < (int) image->rows; y++)
    {
      /*
        Tile fill color onto floodplane.
      */
      q=GetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      indexes=GetIndexes(image);
      for (x=0; x < (int) image->columns; x++)
      {
        if (indexes[x])
          *q=draw_info->fill;
        q++;
      }
      if (!SyncImagePixels(image))
        break;
    }
  else
    {
      double
        alpha;

      /*
        Tile image onto floodplane.
      */
      alpha=1.0/MaxRGB;
      for (y=0; y < (int) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (int) image->columns; x++)
        {
          if (indexes[x])
            {
              color=GetOnePixel(draw_info->tile,x % draw_info->tile->columns,
                y % draw_info->tile->rows);
              if (!draw_info->tile->matte)
                *q=color;
              else
                {
                  q->red=(Quantum) (alpha*(color.red*(MaxRGB-color.opacity)+
                    q->red*color.opacity));
                  q->green=(Quantum) (alpha*(color.green*(MaxRGB-color.opacity)+
                    q->green*color.opacity));
                  q->blue=(Quantum) (alpha*(color.blue*(MaxRGB-color.opacity)+
                    q->blue*color.opacity));
                  q->opacity=(Quantum) (alpha*(color.opacity*
                    (MaxRGB-color.opacity)+q->opacity*color.opacity));
                }
            }
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
    }
  image->storage_class=DirectClass;
  LiberateMemory((void **) &segment_stack);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y D r a w I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyDrawInfo deallocates memory associated with an
%  DrawInfo structure.
%
%  The format of the DestroyDrawInfo method is:
%
%      void DestroyDrawInfo(DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o draw_info: Specifies a pointer to an DrawInfo structure.
%
%
*/
MagickExport void DestroyDrawInfo(DrawInfo *draw_info)
{
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  if (draw_info->primitive != (char *) NULL)
    LiberateMemory((void **) &draw_info->primitive);
  if (draw_info->font != (char *) NULL)
    LiberateMemory((void **) &draw_info->font);
  if (draw_info->tile != (Image *) NULL)
    DestroyImage(draw_info->tile);
  LiberateMemory((void **) &draw_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DrawImage draws a primitive (line, rectangle, ellipse) on the
%  image.
%
%  The format of the DrawImage method is:
%
%      unsigned int DrawImage(Image *image,const DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o draw_info: The address of a DrawInfo structure.
%
%
*/
MagickExport unsigned int DrawImage(Image *image,const DrawInfo *draw_info)
{
#define DrawImageText  "  Drawing on image...  "

  char
    geometry[MaxTextExtent],
    keyword[MaxTextExtent],
    *p,
    *primitive;

  double
    alpha,
    mid,
    opacity,
    radius;

  DrawInfo
    *clone_info;

  int
    j,
    n,
    y;

  PixelPacket
    color;

  PointInfo
    point,
    target;

  PrimitiveInfo
    *primitive_info;

  PrimitiveType
    primitive_type;

  register int
    i,
    x;

  register PixelPacket
    *q;

  SegmentInfo
    bounds;

  unsigned int
    indirection,
    length,
    number_points;

  /*
    Ensure the annotation info is valid.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  assert(draw_info->primitive != (char *) NULL);
  if (*draw_info->primitive == '\0')
    return(False);
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  primitive=clone_info->primitive;
  indirection=(*primitive == '@');
  if (indirection)
    {
      FILE
        *file;

      int
        c;

      register char
        *q;

      /*
        Read text from a file.
      */
      file=(FILE *) fopen(primitive+1,"r");
      if (file == (FILE *) NULL)
        {
          DestroyDrawInfo(clone_info);
          ThrowBinaryException(FileOpenWarning,"Unable to read primitive file",
            primitive+1);
        }
      length=MaxTextExtent;
      primitive=(char *) AcquireMemory(length);
      q=primitive;
      while (primitive != (char *) NULL)
      {
        c=fgetc(file);
        if (c == EOF)
          break;
        if ((q-primitive+1) >= (int) length)
          {
            *q='\0';
            length<<=1;
            ReacquireMemory((void **) &primitive,length);
            if (primitive == (char *) NULL)
              break;
            q=primitive+Extent(primitive);
          }
        *q++=c;
      }
      (void) fclose(file);
      if (primitive == (char *) NULL)
        {
          DestroyDrawInfo(clone_info);
          ThrowBinaryException(ResourceLimitWarning,"Unable to draw image",
            "Memory allocation failed");
        }
      *q='\0';
    }
  /*
    Allocate primitive info memory.
  */
  number_points=2048;
  primitive_info=(PrimitiveInfo *)
    AcquireMemory(number_points*sizeof(PrimitiveInfo));
  if (primitive_info == (PrimitiveInfo *) NULL)
    {
      if (indirection)
        LiberateMemory((void **) &primitive);
      DestroyDrawInfo(clone_info);
      ThrowBinaryException(ResourceLimitWarning,"Unable to draw image",
        "Memory allocation failed");
    }
  for (p=primitive; *p != '\0'; )
  {
    /*
      Define primitive.
    */
    while (isspace((int) (*p)) && (*p != '\0'))
      p++;
    for (x=0; !isspace((int) (*p)) && (*p != '\0'); x++)
      keyword[x]=(*p++);
    keyword[x]='\0';
    if (*keyword == '\0')
      break;
    while (isspace((int) (*p)) && (*p != '\0'))
      p++;
    if (LocaleCompare("affine",keyword) == 0)
      {
        for (n=0; n < 6; n++)
        {
          clone_info->affine[n]=strtod(p,&p);
          if (*p == ',')
            break;
        }
        continue;
      }
    if (LocaleCompare("angle",keyword) == 0)
      {
        clone_info->angle=strtod(p,&p);
        continue;
      }
    if (LocaleCompare("antialias",keyword) == 0)
      {
        clone_info->antialias=(unsigned int) strtod(p,&p);
        continue;
      }
    if (LocaleCompare("decorate",keyword) == 0)
      {
        for (x=0; !isspace((int) (*p)) && (*p != '\0'); x++)
          keyword[x]=(*p++);
        keyword[x]='\0';
        if (LocaleCompare("none",keyword) == 0)
          clone_info->decorate=NoDecoration;
        if (LocaleCompare("underline",keyword) == 0)
          clone_info->decorate=UnderlineDecoration;
        if (LocaleCompare("overline",keyword) == 0)
          clone_info->decorate=OverlineDecoration;
        if (LocaleCompare("line-through",keyword) == 0)
          clone_info->decorate=LineThroughDecoration;
        continue;
      }
    if (LocaleCompare("fill",keyword) == 0)
      {
        if ((LocaleNCompare(p,"rgb(",4) == 0))
          for (x=0; (*(p-1) != ')') && (*p != '\0'); x++)
            keyword[x]=(*p++);
        else
          for (x=0; !isspace((int) (*p)) && (*p != '\0'); x++)
            keyword[x]=(*p++);
        keyword[x]='\0';
        (void) QueryColorDatabase(keyword,&clone_info->fill);
        continue;
      }
    if (LocaleCompare("font",keyword) == 0)
      {
        for (x=0; !isspace((int) (*p)) && (*p != '\0'); x++)
          keyword[x]=(*p++);
        keyword[x]='\0';
        CloneString(&clone_info->font,keyword);
        continue;
      }
    if (LocaleCompare("gravity",keyword) == 0)
      {
        for (x=0; !isspace((int) (*p)) && (*p != '\0'); x++)
          keyword[x]=(*p++);
        keyword[x]='\0';
        if (LocaleCompare("NorthWest",keyword) == 0)
          clone_info->gravity=NorthWestGravity;
        if (LocaleCompare("North",keyword) == 0)
          clone_info->gravity=NorthGravity;
        if (LocaleCompare("NorthEast",keyword) == 0)
          clone_info->gravity=NorthEastGravity;
        if (LocaleCompare("West",keyword) == 0)
          clone_info->gravity=WestGravity;
        if (LocaleCompare("Center",keyword) == 0)
          clone_info->gravity=CenterGravity;
        if (LocaleCompare("East",keyword) == 0)
          clone_info->gravity=EastGravity;
        if (LocaleCompare("SouthWest",keyword) == 0)
          clone_info->gravity=SouthWestGravity;
        if (LocaleCompare("South",keyword) == 0)
          clone_info->gravity=SouthGravity;
        if (LocaleCompare("SouthEast",keyword) == 0)
          clone_info->gravity=SouthEastGravity;
        continue;
      }
    if (LocaleCompare("linewidth",keyword) == 0)
      {
        clone_info->linewidth=strtod(p,&p);
        continue;
      }
    if (LocaleCompare("opacity",keyword) == 0)
      {
        clone_info->opacity=strtod(p,&p);
        continue;
      }
    if (LocaleCompare("pointsize",keyword) == 0)
      {
        clone_info->pointsize=strtod(p,&p);
        continue;
      }
    if (LocaleCompare("stroke",keyword) == 0)
      {
        if ((LocaleNCompare(p,"rgb(",4) == 0))
          for (x=0; (*(p-1) != ')') && (*p != '\0'); x++)
            keyword[x]=(*p++);
        else
          for (x=0; !isspace((int) (*p)) && (*p != '\0'); x++)
            keyword[x]=(*p++);
        keyword[x]='\0';
        (void) QueryColorDatabase(keyword,&clone_info->stroke);
        continue;
      }
    primitive_type=UndefinedPrimitive;
    if (LocaleNCompare("fill",keyword,4) == 0)
      (void) strcpy(keyword,keyword+4);
    if (LocaleCompare("Point",keyword) == 0)
      primitive_type=PointPrimitive;
    if (LocaleCompare("Line",keyword) == 0)
      primitive_type=LinePrimitive;
    if (LocaleNCompare("Rect",keyword,4) == 0)
      primitive_type=RectanglePrimitive;
    if (LocaleCompare("RoundRectangle",keyword) == 0)
      primitive_type=RoundRectanglePrimitive;
    if (LocaleCompare("Arc",keyword) == 0)
      primitive_type=ArcPrimitive;
    if (LocaleCompare("Ellipse",keyword) == 0)
      primitive_type=EllipsePrimitive;
    if (LocaleCompare("Circle",keyword) == 0)
      primitive_type=CirclePrimitive;
    if (LocaleCompare("Polyline",keyword) == 0)
      primitive_type=PolylinePrimitive;
    if (LocaleCompare("Polygon",keyword) == 0)
      primitive_type=PolygonPrimitive;
    if (LocaleCompare("Bezier",keyword) == 0)
      primitive_type=BezierPrimitive;
    if (LocaleCompare("Path",keyword) == 0)
      primitive_type=PathPrimitive;
    if (LocaleCompare("Color",keyword) == 0)
      primitive_type=ColorPrimitive;
    if (LocaleCompare("Matte",keyword) == 0)
      primitive_type=MattePrimitive;
    if (LocaleCompare("Text",keyword) == 0)
      primitive_type=TextPrimitive;
    if (LocaleCompare("Image",keyword) == 0)
      primitive_type=ImagePrimitive;
    if (primitive_type == UndefinedPrimitive)
      break;
    /*
      Parse the primitive attributes.
    */
    bounds.x1=image->columns-1.0;
    bounds.y1=image->rows-1.0;
    bounds.x2=0.0;
    bounds.y2=0.0;
    radius=0.0;
    i=0;
    j=0;
    for (x=0; *p != '\0'; x++)
    {
      /*
        Define points.
      */
      while (isspace((int) (*p)) && (*p != '\0'))
        p++;
      if (!IsGeometry(p))
        break;
      point.x=strtod(p,&p);
      if (*p == ',')
        p++;
      point.y=strtod(p,&p);
      if (*p == ',')
        p++;
      primitive_info[i].primitive=primitive_type;
      primitive_info[i].point=point;
      primitive_info[i].coordinates=0;
      primitive_info[i].method=FloodfillMethod;
      while ((isspace((int) (*p)) || (*p == ',')) && (*p != '\0'))
        p++;
      i++;
      if (i < (int) (number_points-Max(4*BezierQuantum,360)-1))
        continue;
      number_points<<=1;
      ReacquireMemory((void **) &primitive_info,
        number_points*sizeof(PrimitiveInfo));
      if (primitive_info != (PrimitiveInfo *) NULL)
        continue;
      if (indirection)
        LiberateMemory((void **) &primitive);
      DestroyDrawInfo(clone_info);
      ThrowBinaryException(ResourceLimitWarning,"Unable to draw image",
        "Memory allocation failed");
    }
    while (isspace((int) (*p)) && (*p != '\0'))
      p++;
    primitive_info[j].primitive=primitive_type;
    primitive_info[j].coordinates=x;
    primitive_info[j].method=FloodfillMethod;
    primitive_info[j].text=(char *) NULL;
    switch (primitive_type)
    {
      case PointPrimitive:
      default:
      {
        if (primitive_info[j].coordinates != 1)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        GeneratePoint(primitive_info+j,primitive_info[j].point);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case LinePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        GenerateLine(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case RectanglePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        GenerateRectangle(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case RoundRectanglePrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        GenerateRoundRectangle(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point,primitive_info[j+2].point);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case ArcPrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        GenerateArc(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point,primitive_info[j+2].point,0,False,False);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case EllipsePrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        GenerateEllipse(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point,primitive_info[j+2].point);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case CirclePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        radius=GenerateCircle(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case PolylinePrimitive:
      {
        if (primitive_info[j].coordinates < 2)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        break;
      }
      case PolygonPrimitive:
      {
        if (primitive_info[j].coordinates < 3)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        primitive_info[i]=primitive_info[j];
        primitive_info[i].coordinates=0;
        primitive_info[j].coordinates++;
        i++;
        break;
      }
      case BezierPrimitive:
      {
        if (primitive_info[j].coordinates < 3)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        GenerateBezier(primitive_info+j);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case PathPrimitive:
      {
        char
          *path;

        unsigned int
          number_attributes;

        number_attributes=0;
        path=p;
        if (*p == '"')
          {
            for (p++; *p != '\0'; p++)
            {
              if (isalpha(*p))
                number_attributes++;
              if ((*p == '"') && (*(p-1) != '\\'))
                break;
            }
          }
        else
          if (*p == '\'')
            {
              for (p++; *p != '\0'; p++)
              {
                if (isalpha(*p))
                  number_attributes++;
                if ((*p == '\'') && (*(p-1) != '\\'))
                  break;
              }
            }
          else
            for (p++;  *p != '\0'; p++)
            {
              if (isalpha(*p))
                number_attributes++;
              if (isspace((int) *p) && (*(p-1) != '\\') && (*p != '\0'))
                break;
            }
        if (*p != '\0')
          p++;
        *p++='\0';
        if (i < (number_points-4*BezierQuantum*number_attributes-1))
          {
            number_points+=4*BezierQuantum*number_attributes;
            ReacquireMemory((void **) &primitive_info,
              number_points*sizeof(PrimitiveInfo));
            if (primitive_info == (PrimitiveInfo *) NULL)
              {
                if (indirection)
                  LiberateMemory((void **) &primitive);
                DestroyDrawInfo(clone_info);
                ThrowBinaryException(ResourceLimitWarning,
                  "Unable to draw image","Memory allocation failed");
              }
          }
        i=j+GeneratePath(primitive_info+j,path);
        break;
      }
      case ColorPrimitive:
      case MattePrimitive:
      {
        if (primitive_info[j].coordinates != 1)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        /*
          Define method.
        */
        for (x=0; !isspace((int) (*p)) && (*p != '\0'); x++)
          keyword[x]=(*p++);
        keyword[x]='\0';
        if (*keyword == '\0')
          break;
        if (LocaleCompare("point",keyword) == 0)
          primitive_info[j].method=PointMethod;
        else
          if (LocaleCompare("replace",keyword) == 0)
            primitive_info[j].method=ReplaceMethod;
          else
            if (LocaleCompare("floodfill",keyword) == 0)
              primitive_info[j].method=FloodfillMethod;
            else
              if (LocaleCompare("filltoborder",keyword) == 0)
                primitive_info[j].method=FillToBorderMethod;
              else
                if (LocaleCompare("reset",keyword) == 0)
                  primitive_info[j].method=ResetMethod;
                else
                  primitive_type=UndefinedPrimitive;
        if ((primitive_info[j].method == ReplaceMethod) ||
            (primitive_info[j].method == ResetMethod))
          {
            bounds.x1=0.0;
            bounds.y1=0.0;
            bounds.x2=image->columns-1.0;
            bounds.y2=image->rows-1.0;
          }
        while (isspace((int) (*p)) && (*p != '\0'))
          p++;
        break;
      }
      case TextPrimitive:
      {
        if (primitive_info[j].coordinates != 1)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        if (*p == '\0')
          break;
        primitive_info[j].text=p;
        if (*p == '"')
          {
            for (p++; *p != '\0'; p++)
              if ((*p == '"') && (*(p-1) != '\\'))
                break;
          }
        else
          if (*p == '\'')
            {
              for (p++; *p != '\0'; p++)
                if ((*p == '\'') && (*(p-1) != '\\'))
                  break;
            }
          else
            for (p++;  *p != '\0'; p++)
              if (isspace((int) *p) && (*(p-1) != '\\') && (*p != '\0'))
                break;
        if (*p != '\0')
          p++;
        break;
      }
      case ImagePrimitive:
      {
        Image
          *composite_image;

        ImageInfo
          *composite_info;

        register char
          *q;

        if (primitive_info[j].coordinates != 2)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        composite_info=CloneImageInfo((ImageInfo *) NULL);
        q=composite_info->filename;
        if (*p != '\0')
          {
            primitive_info[j].text=p;
            if (*p == '"')
              {
                for (p++; *p != '\0'; p++)
                {
                  if ((*p == '"') && (*(p-1) != '\\'))
                    break;
                  *q++=(*p);
                }
              }
            else
              if (*p == '\'')
                {
                  for (p++; *p != '\0'; p++)
                  {
                    if ((*p == '\'') && (*(p-1) != '\\'))
                      break;
                    *q++=(*p);
                  }
                }
              else
                for ( ;  *p != '\0'; p++)
                {
                  if (isspace((int) *p) && (*(p-1) != '\\') && (*p != '\0'))
                    break;
                  *q++=(*p);
                }
            if (*p != '\0')
              p++;
          }
        *q='\0';
        composite_image=ReadImage(composite_info,&image->exception);
        if (composite_image == (Image *) NULL)
          break;
        if ((primitive_info[j+1].point.x != 0) &&
            (primitive_info[j+1].point.y != 0))
          {
            /*
              Resize image.
            */
            FormatString(geometry,"%gx%g",primitive_info[j+1].point.x,
              primitive_info[j+1].point.y);
            TransformImage(&composite_image,(char *) NULL,geometry);
          }
        if ((clone_info->affine[1] == 0.0) && (clone_info->affine[2] == 0.0))
          {
            if ((clone_info->affine[0] != 1.0) ||
                (clone_info->affine[0] != 1.0))
              {
                Image
                  *scale_image;

                unsigned int
                  height,
                  width;

                width=(unsigned int)
                  (clone_info->affine[0]*composite_image->columns);
                height=(unsigned int)
                  (clone_info->affine[3]*composite_image->rows);
                scale_image=
                  ZoomImage(composite_image,width,height,&image->exception);
                if (scale_image != (Image *) NULL)
                  {
                    DestroyImage(composite_image);
                    composite_image=scale_image;
                  }
              }
          }
        else
          {
            if (((clone_info->affine[0]-clone_info->affine[3]) == 0.0) &&
                ((clone_info->affine[1]+clone_info->affine[2]) == 0.0))
              {
                double
                  theta;

                Image
                  *rotate_image;

                theta=(180.0/M_PI)*
                  atan2(clone_info->affine[1],clone_info->affine[0]);
                rotate_image=
                  RotateImage(composite_image,theta,&image->exception);
                if (rotate_image != (Image *) NULL)
                  {
                    DestroyImage(composite_image);
                    composite_image=rotate_image;
                  }
              }
          }
        CompositeImage(image,image->matte ? OverCompositeOp :
          ReplaceCompositeOp,composite_image,(int) primitive_info[j].point.x,
          (int) primitive_info[j].point.y);
        DestroyImage(composite_image);
        DestroyImageInfo(composite_info);
        break;
      }
    }
    if (primitive_type == UndefinedPrimitive)
      break;
    primitive_info[i].primitive=UndefinedPrimitive;
    /*
      Transform points.
    */
    for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++)
    {
      point=primitive_info[i].point;
      primitive_info[i].point.x=clone_info->affine[0]*point.x+
        clone_info->affine[2]*point.y+clone_info->affine[4];
      primitive_info[i].point.y=clone_info->affine[1]*point.x+
        clone_info->affine[3]*point.y+clone_info->affine[5];
    }
    /*
      Compute bounding box.
    */
    for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++)
    {
      point=primitive_info[i].point;
      if (point.x < bounds.x1)
        bounds.x1=point.x;
      if (point.y < bounds.y1)
        bounds.y1=point.y;
      if (point.x > bounds.x2)
        bounds.x2=point.x;
      if (point.y > bounds.y2)
        bounds.y2=point.y;
    }
    mid=ceil(radius+clone_info->linewidth/2.0);
    bounds.x1-=mid;
    if (bounds.x1 < 0.0)
      bounds.x1=0.0;
    bounds.y1-=mid;
    if (bounds.y1 < 0.0)
      bounds.y1=0.0;
    bounds.x2+=mid;
    if (bounds.x2 >= image->columns)
      bounds.x2=image->columns-1.0;
    bounds.y2+=mid;
    if (bounds.y2 >= image->rows)
      bounds.y2=image->rows-1.0;
    alpha=1.0/MaxRGB;
    image->storage_class=DirectClass;
    for (y=(int) bounds.y1; y <= (int) bounds.y2; y++)
    {
      /*
        Fill the primitive on the image.
      */
      n=(int) (bounds.x2-bounds.x1+1.0);
      target.y=y;
      q=GetImagePixels(image,(int) bounds.x1,y,n,1);
      if (q == (PixelPacket *) NULL)
        break;
      for (x=(int) bounds.x1; x <= (int) bounds.x2; x++)
      {
        target.x=x;
        switch (primitive_info->primitive)
        {
          case ArcPrimitive:
          case BezierPrimitive:
          case CirclePrimitive:
          case EllipsePrimitive:
          case PathPrimitive:
          case PolylinePrimitive:
          case PolygonPrimitive:
          case RectanglePrimitive:
          case RoundRectanglePrimitive:
          {
            color=clone_info->fill;
            if (clone_info->tile != (Image *) NULL)
              color=GetOnePixel(clone_info->tile,x % clone_info->tile->columns,
                y % clone_info->tile->rows);
            opacity=0.01*(MaxRGB-color.opacity)*clone_info->opacity*
              IntersectPrimitive(primitive_info,clone_info,&target,True,image);
            opacity=MaxRGB-opacity;
            if ((int) opacity == TransparentOpacity)
              break;
            if (!clone_info->antialias)
              opacity=(Quantum) (OpaqueOpacity*clone_info->opacity/100.0);
            q->red=(Quantum)
              (alpha*(color.red*(MaxRGB-opacity)+q->red*opacity));
            q->green=(Quantum)
              (alpha*(color.green*(MaxRGB-opacity)+q->green*opacity));
            q->blue=(Quantum)
              (alpha*(color.blue*(MaxRGB-opacity)+q->blue*opacity));
            q->opacity=(Quantum)
              (alpha*(opacity*(MaxRGB-opacity)+q->opacity*opacity));
            break;
          }
          default:
            break;
        }
        color=clone_info->stroke;
        opacity=0.01*(MaxRGB-color.opacity)*clone_info->opacity*
          IntersectPrimitive(primitive_info,clone_info,&target,False,image);
        opacity=MaxRGB-opacity;
        if ((int) opacity == TransparentOpacity)
          {
            q++;
            continue;
          }
        if (!clone_info->antialias)
          opacity=(Quantum) (OpaqueOpacity*clone_info->opacity/100.0);
        q->red=(Quantum) (alpha*(color.red*(MaxRGB-opacity)+q->red*opacity));
        q->green=(Quantum)
          (alpha*(color.green*(MaxRGB-opacity)+q->green*opacity));
        q->blue=(Quantum) (alpha*(color.blue*(MaxRGB-opacity)+q->blue*opacity));
        q->opacity=(Quantum)
          (alpha*(opacity*(MaxRGB-opacity)+q->opacity*opacity));
        q++;
      }
      if (!SyncImagePixels(image))
        break;
      if (QuantumTick(y,image->rows))
        ProgressMonitor(DrawImageText,y,image->rows);
    }
  }
  /*
    Free resources.
  */
  LiberateMemory((void **) &primitive_info);
  if (indirection)
    LiberateMemory((void **) &primitive);
  DestroyDrawInfo(clone_info);
  if (primitive_type == UndefinedPrimitive)
    ThrowBinaryException(OptionWarning,
      "Non-conforming drawing primitive definition",keyword);
  (void) IsMatteImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e n e r a t e P r i m i t i v e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GeneratePrimitive is a collection of methods for generating graphic
%  primitives such as arcs, ellipses, paths, etc.
%
%
*/

static void GenerateArc(PrimitiveInfo *primitive_info,const PointInfo start,
  const PointInfo end,const PointInfo arc,const double angle,
  const unsigned int large_arc,const unsigned int sweep)
{
  double
    alpha,
    arc_angle,
    beta,
    cosine,
    factor,
    gamma,
    sine;

  int
    number_segments;

  PointInfo
    center,
    points[3];

  register int
    i;

  register PrimitiveInfo
    *p;

  cosine=cos(DegreesToRadians(angle));
  sine=sin(DegreesToRadians(angle));
  points[0].x=cosine*start.x/arc.x+sine*start.y/arc.x;
  points[0].y=cosine*start.y/arc.y-sine*start.x/arc.y;
  points[1].x=cosine*end.x/arc.x+sine*end.y/arc.x;
  points[1].y=cosine*end.y/arc.y-sine*end.x/arc.y;
  alpha=points[1].x-points[0].x;
  beta=points[1].y-points[0].y;
  factor=1.0/(alpha*alpha+beta*beta)-0.25;
  if (factor < 0.0)
    factor=0.0;
  factor=sqrt(factor);
  if (sweep == large_arc)
    factor=(-factor);
  center.x=0.5*(points[0].x+points[1].x)-factor*beta;
  center.y=0.5*(points[0].y+points[1].y)+factor*alpha;
  alpha=atan2(points[0].y-center.y,points[0].x-center.x);
  arc_angle=atan2(points[1].y-center.y,points[1].x-center.x)-alpha;
  if ((arc_angle < 0.0) && sweep)
    arc_angle+=2.0*M_PI;
  else
    if ((arc_angle > 0.0) && !sweep)
      arc_angle-=2.0*M_PI;
  number_segments=ceil(fabs(arc_angle/(0.5*M_PI+0.001)));
  p=primitive_info;
  for (i=0; i < number_segments; i++)
  {
    beta=0.5*((alpha+(i+1)*arc_angle/number_segments)-
      (alpha+i*arc_angle/number_segments));
    gamma=(8.0/3.0)*sin(0.5*beta)*sin(0.5*beta)/sin(beta);
    points[0].x=center.x+cos((alpha+i*arc_angle/number_segments))-
      gamma*sin((alpha+i*arc_angle/number_segments));
    points[0].y=center.y+sin((alpha+i*arc_angle/number_segments))+
      gamma*cos((alpha+i*arc_angle/number_segments));
    points[2].x=center.x+cos((alpha+(i+1)*arc_angle/number_segments));
    points[2].y=center.y+sin((alpha+(i+1)*arc_angle/number_segments));
    points[1].x=points[2].x+gamma*sin((alpha+(i+1)*arc_angle/number_segments));
    points[1].y=points[2].y-gamma*cos((alpha+(i+1)*arc_angle/number_segments));
    p->point=p == primitive_info ? start : (p-1)->point;
    (p+1)->point.x=cosine*arc.x*points[0].x-sine*arc.y*points[0].y;
    (p+1)->point.y=sine*arc.x*points[0].x+cosine*arc.y*points[0].y;
    (p+2)->point.x=cosine*arc.x*points[1].x-sine*arc.y*points[1].y;
    (p+2)->point.y=sine*arc.x*points[1].x+cosine*arc.y*points[1].y;
    (p+3)->point.x=cosine*arc.x*points[2].x-sine*arc.y*points[2].y;
    (p+3)->point.y=sine*arc.x*points[2].x+cosine*arc.y*points[2].y;
    if (i == (number_segments-1))
      (p+3)->point=end;
    p->coordinates=4;
    GenerateBezier(p);
    p+=p->coordinates;
  }
  primitive_info->coordinates=p-primitive_info;
  for (i=0; i < primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
}

static void GenerateBezier(PrimitiveInfo *primitive_info)
{
  double
    alpha,
    *coefficients,
    weight;

  PointInfo
    last,
    point,
    *points;

  register int
    i,
    j;

  register PrimitiveInfo
    *p;

  unsigned int
    control_points,
    quantum;

  /*
    Allocate coeficients.
  */
  quantum=primitive_info->coordinates;
  for (i=0; i < primitive_info->coordinates; i++)
  {
    for (j=i+1; j < primitive_info->coordinates; j++)
    {
      alpha=fabs(primitive_info[j].point.x-primitive_info[i].point.x);
      if (alpha > quantum)
        quantum=alpha;
      alpha=fabs(primitive_info[j].point.y-primitive_info[i].point.y);
      if (alpha > quantum)
        quantum=alpha;
    }
  }
  quantum=Min(quantum/primitive_info->coordinates,BezierQuantum);
  control_points=quantum*primitive_info->coordinates;
  coefficients=(double *)
    AcquireMemory(primitive_info->coordinates*sizeof(double));
  points=(PointInfo *) AcquireMemory(control_points*sizeof(PointInfo));
  if ((coefficients == (double *) NULL) || (points == (PointInfo *) NULL))
    MagickError(ResourceLimitWarning,"Unable to draw image",
      "Memory allocation failed");
  /*
    Compute bezier points.
  */
  last=primitive_info[primitive_info->coordinates-1].point;
  weight=0.0;
  for (i=0; i < primitive_info->coordinates; i++)
    coefficients[i]=Permutate(primitive_info->coordinates-1,i);
  for (i=0; i < (int) control_points; i++)
  {
    p=primitive_info;
    point.x=0;
    point.y=0;
    alpha=pow(1.0-weight,primitive_info->coordinates-1);
    for (j=0; j < primitive_info->coordinates; j++)
    {
      point.x+=alpha*coefficients[j]*p->point.x;
      point.y+=alpha*coefficients[j]*p->point.y;
      alpha*=weight/(1.0-weight);
      p++;
    }
    points[i]=point;
    weight+=1.0/quantum/primitive_info->coordinates;
  }
  /*
    Bezier curves are just short segmented polys.
  */
  p=primitive_info;
  for (i=0; i < (int) control_points; i++)
  {
    point=points[i];
    p->point=point;
    p++;
  }
  p->point=last;
  p++;
  primitive_info->coordinates=p-primitive_info;
  for (i=0; i < primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
  LiberateMemory((void **) &points);
  LiberateMemory((void **) &coefficients);
}

static double GenerateCircle(PrimitiveInfo *primitive_info,
  const PointInfo start,const PointInfo end)
{
  double
    alpha,
    beta;

  register PrimitiveInfo
    *p,
    *q;

  primitive_info->coordinates=2;
  p=primitive_info;
  p->point=start;
  q=p+1;
  q->primitive=primitive_info->primitive;
  q->point=end;
  alpha=q->point.x-p->point.x;
  beta=q->point.y-p->point.y;
  return(sqrt(alpha*alpha+beta*beta));
}

static void GenerateEllipse(PrimitiveInfo *primitive_info,const PointInfo start,
  const PointInfo end,const PointInfo degrees)
{
  double
    angle;

  PointInfo
    point;

  register int
    i;

  register PrimitiveInfo
    *p;

  /*
    Ellipses are just short segmented polys.
  */
  p=primitive_info;
  for (angle=(degrees.x+1.0); angle <= degrees.y; angle+=1.0)
  {
    point.x=cos(DegreesToRadians(fmod(angle,360.0)))*end.x+start.x;
    point.y=sin(DegreesToRadians(fmod(angle,360.0)))*end.y+start.y;
    p->point=point;
    p++;
  }
  primitive_info->coordinates=p-primitive_info;
  for (i=0; i < primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
}

static void GenerateLine(PrimitiveInfo *primitive_info,const PointInfo start,
  const PointInfo end)
{
  register PrimitiveInfo
    *p,
    *q;

  p=primitive_info;
  p->point=start;
  q=p+1;
  q->primitive=primitive_info->primitive;
  q->point=end;
  primitive_info->coordinates=2;
}

static unsigned int GeneratePath(PrimitiveInfo *primitive_info,const char *path)
{
  char
    *p;

  double
    x,
    y;

  int
    attribute;

  PointInfo
    last,
    points[4],
    point,
    start;

  PrimitiveType
    primitive_type;

  register int
    i;

  register PrimitiveInfo
    *q;

  unsigned int
    number_coordinates,
    z_count;

  point.x=0;
  point.y=0;
  number_coordinates=0;
  z_count=0;
  primitive_type=primitive_info->primitive;
  q=primitive_info;
  p=(char *) path;
  while (*p != '\0')
  {
    while (isspace((int) *p))
      p++;
    attribute=(*p++);
    switch (attribute)
    {
      case 'a':
      case 'A':
      {
        double
          angle;

        PointInfo
          arc;

        unsigned int
          large_arc,
          sweep;

        /*
          Compute arc points.
        */
        arc.x=strtod(p,&p);
        if (*p == ',')
          p++;
        arc.y=strtod(p,&p);
        if (*p == ',')
          p++;
        angle=strtod(p,&p);
        if (*p == ',')
          p++;
        large_arc=strtod(p,&p);
        if (*p == ',')
          p++;
        sweep=strtod(p,&p);
        if (*p == ',')
          p++;
        x=strtod(p,&p);
        if (*p == ',')
          p++;
        y=strtod(p,&p);
        last.x=attribute == 'A' ? x : point.x+x;
        last.y=attribute == 'A' ? y : point.y+y;
        GenerateArc(q,point,last,arc,angle,large_arc,sweep);
        q+=q->coordinates;
        break;
      }
      case 'c':
      case 'C':
      {
        /*
          Compute bezier points.
        */
        do
        {
          points[0]=point;
          for (i=1; i <= 3; i++)
          {
            x=strtod(p,&p);
            if (*p == ',')
              p++;
            y=strtod(p,&p);
            if (*p == ',')
              p++;
            last.x=attribute == 'C' ? x : point.x+x;
            last.y=attribute == 'C' ? y : point.y+y;
            points[i]=last;
          }
          for (i=0; i <= 3; i++)
            (q+i)->point=points[i];
          q->coordinates=4;
          GenerateBezier(q);
          q+=q->coordinates;
        } while (IsGeometry(p));
        break;
      }
      case 'H':
      case 'h':
      {
        x=strtod(p,&p);
        point.x=attribute == 'H' ? x: point.x+x;
        GeneratePoint(q,point);
        q+=q->coordinates;
        break;
      }
      case 'l':
      case 'L':
      {
        do
        {
          x=strtod(p,&p);
          if (*p == ',')
            p++;
          y=strtod(p,&p);
          if (*p == ',')
            p++;
          point.x=attribute == 'L' ? x : point.x+x;
          point.y=attribute == 'L' ? y : point.y+y;
          GeneratePoint(q,point);
          q+=q->coordinates;
        } while (IsGeometry(p));
        break;
      }
      case 'm':
      {
        primitive_info->coordinates=q-primitive_info;
        number_coordinates+=primitive_info->coordinates;
        primitive_info=q;
      }
      case 'M':
      {
        do
        {
          x=strtod(p,&p);
          if (*p == ',')
            p++;
          y=strtod(p,&p);
          point.x=attribute == 'M' ? x : point.x+x;
          point.y=attribute == 'M' ? y : point.y+y;
          GeneratePoint(q,point);
          q+=q->coordinates;
        } while (IsGeometry(p));
        start=point;
        break;
      }
      case 'q':
      case 'Q':
      {
        /*
          Compute bezier points.
        */
        do
        {
          points[0]=point;
          for (i=1; i < 3; i++)
          {
            x=strtod(p,&p);
            if (*p == ',')
              p++;
            y=strtod(p,&p);
            if (*p == ',')
              p++;
            last.x=attribute == 'Q' ? x : point.x+x;
            last.y=attribute == 'Q' ? y : point.y+y;
            points[i]=last;
          }
          for (i=0; i < 3; i++)
            (q+i)->point=points[i];
          q->coordinates=3;
          GenerateBezier(q);
          q+=q->coordinates;
        } while (IsGeometry(p));
        break;
      }
      case 's':
      case 'S':
      {
        /*
          Compute bezier points.
        */
        do
        {
          points[0]=points[3];
          points[1].x=2.0*points[3].x-points[2].x;
          points[1].y=2.0*points[3].y-points[2].y;
          for (i=2; i <= 3; i++)
          {
            x=strtod(p,&p);
            if (*p == ',')
              p++;
            y=strtod(p,&p);
            if (*p == ',')
              p++;
            last.x=attribute == 'S' ? x : point.x+x;
            last.y=attribute == 'S' ? y : point.y+y;
            points[i]=last;
          }
          for (i=0; i <= 4; i++)
            (q+i)->point=points[i];
          q->coordinates=4;
          GenerateBezier(q);
          q+=q->coordinates;
        } while (IsGeometry(p));
        break;
      }
      case 't':
      case 'T':
      {
        /*
          Compute bezier points.
        */
        do
        {
          points[0]=points[2];
          points[1].x=2.0*points[2].x-points[1].x;
          points[1].y=2.0*points[2].y-points[1].y;
          for (i=2; i < 3; i++)
          {
            x=strtod(p,&p);
            if (*p == ',')
              p++;
            y=strtod(p,&p);
            if (*p == ',')
              p++;
            last.x=attribute == 'T' ? x : point.x+x;
            last.y=attribute == 'T' ? y : point.y+y;
            points[i]=last;
          }
          for (i=0; i < 3; i++)
            (q+i)->point=points[i];
          q->coordinates=3;
          GenerateBezier(q);
          q+=q->coordinates;
        } while (IsGeometry(p));
        break;
      }
      case 'v':
      case 'V':
      {
        y=strtod(p,&p);
        point.y=attribute == 'V' ? y : point.y+y;
        GeneratePoint(q,point);
        q+=q->coordinates;
        break;
      }
      case 'z':
      case 'Z':
      {
        point=start;
        GeneratePoint(q,point);
        q+=q->coordinates;
        primitive_info->coordinates=q-primitive_info;
        number_coordinates+=primitive_info->coordinates;
        primitive_info=q;
        z_count++;
        break;
      }
      default:
      {
        if (isalpha((int) attribute))
          (void) fprintf(stderr,"attribute not recognized: %c\n",attribute);
        break;
      }
    }
    if (q != primitive_info)
      point=(q-1)->point;
  }
  if (z_count == 0)
    {
      GeneratePoint(q,point);
      q+=q->coordinates;
    }
  primitive_info->coordinates=q-primitive_info;
  number_coordinates+=primitive_info->coordinates;
  for (i=0; i < number_coordinates; i++)
  {
    q--;
    q->primitive=primitive_type;
    if (z_count > 1)
      q->method=FillToBorderMethod;
  }
  return(number_coordinates);
}

static void GeneratePoint(PrimitiveInfo *primitive_info,const PointInfo start)
{
  primitive_info->coordinates=1;
  primitive_info->point=start;
}

static void GenerateRectangle(PrimitiveInfo *primitive_info,
  const PointInfo start,const PointInfo end)
{
  PointInfo
    point;

  register int
    i;

  register PrimitiveInfo
    *p;

  p=primitive_info;
  GeneratePoint(p,start);
  p+=p->coordinates;
  point.x=start.x;
  point.y=end.y;
  GeneratePoint(p,point);
  p+=p->coordinates;
  GeneratePoint(p,end);
  p+=p->coordinates;
  point.x=end.x;
  point.y=start.y;
  GeneratePoint(p,point);
  p+=p->coordinates;
  GeneratePoint(p,start);
  p+=p->coordinates;
  primitive_info->coordinates=p-primitive_info;
  for (i=0; i < primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
}

static void GenerateRoundRectangle(PrimitiveInfo *primitive_info,
  const PointInfo start,const PointInfo end,PointInfo arc)
{
  PointInfo
    degrees,
    point;

  register int
    i;

  register PrimitiveInfo
    *p;

  p=primitive_info;
  if (arc.x > (0.5*end.x))
    arc.x=0.5*end.x;
  if (arc.y > (0.5*end.y))
    arc.y=0.5*end.y;
  point.x=start.x+end.x-arc.x;
  point.y=start.y+arc.y;
  degrees.x=270.0;
  degrees.y=360.0;
  GenerateEllipse(p,point,arc,degrees);
  p+=p->coordinates;
  point.x=start.x+end.x-arc.x;
  point.y=start.y+end.y-arc.y;
  degrees.x=0.0;
  degrees.y=90.0;
  GenerateEllipse(p,point,arc,degrees);
  p+=p->coordinates;
  point.x=start.x+arc.x;
  point.y=start.y+end.y-arc.y;
  degrees.x=90.0;
  degrees.y=180.0;
  GenerateEllipse(p,point,arc,degrees);
  p+=p->coordinates;
  point.x=start.x+arc.x;
  point.y=start.y+arc.y;
  degrees.x=180.0;
  degrees.y=270.0;
  GenerateEllipse(p,point,arc,degrees);
  p+=p->coordinates;
  GeneratePoint(p,primitive_info->point);
  p+=p->coordinates;
  primitive_info->coordinates=p-primitive_info;
  for (i=0; i < primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t D r a w I n f o                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetDrawInfo initializes the DrawInfo structure.
%
%  The format of the GetDrawInfo method is:
%
%      void GetDrawInfo(const ImageInfo *image_info,DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o draw_info: Specifies a pointer to a DrawInfo structure.
%
%
*/
MagickExport void GetDrawInfo(const ImageInfo *image_info,DrawInfo *draw_info)
{
  register int
    i;

  /*
    Initialize draw attributes.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(draw_info != (DrawInfo *) NULL);
  draw_info->primitive=(char *) NULL;
  draw_info->font=AllocateString(image_info->font);
  draw_info->antialias=image_info->antialias;
  draw_info->gravity=NorthWestGravity;
  draw_info->opacity=100.0;
  draw_info->linewidth=1.0;
  draw_info->pointsize=image_info->pointsize;
  draw_info->angle=0.0;
  for (i=0; i < 6; i++)
    draw_info->affine[i]=image_info->affine[i];
  draw_info->fill=image_info->fill;
  draw_info->stroke=image_info->stroke;
  (void) QueryColorDatabase("none",&draw_info->box);
  draw_info->border_color=image_info->border_color;
  draw_info->decorate=NoDecoration;
  draw_info->tile=(Image *) NULL;
  draw_info->signature=MagickSignature;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   I n t e r s e c t P r i m i t i v e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IntersectPrimitive returns the value from [0..1] for the (x,y)
%  position of the image.  The opacity is 1.0 if the (x,y) position intersects
%  within the bounds of the primitive as defined in primitive_info.  A value
%  less than 1.0 and greater than 0.0 is returned for a primitive edge point
%  to allow for anti-aliasing.  Otherwise 0.0 is returned.
%
%  Rick Mabry provided the algorithms for anti-aliased primitives.
%
%  The format of the IntersectPrimitive method is:
%
%      double IntersectPrimitive(PrimitiveInfo *primitive_info,
%        const DrawInfo *draw_info,const PointInfo *point,const int fill,
%        Image *image)
%
%  A description of each parameter follows:
%
%    o opacity:  Method IntersectPrimitive returns a opacity from [0..1] as
%      determined by intesecting the (x,y) position of the image with the
%      specified primitive list.
%
%    o primitive_info: Specifies a pointer to a PrimitiveInfo structure.
%
%    o draw_info: Specifies a pointer to a DrawInfo structure.
%
%    o target: PointInfo representing the (x,y) location in the image.
%
%    o fill: only return True if the point is in the interior of the
%      specified graphics primitive.
%
%    o image: The address of a structure of type Image.
%
%
*/

static double DistanceToLine(const PointInfo *point,const PointInfo *p,
  const PointInfo *q)
{
  double
    dot_product,
    gamma,
    phi;

  register double
    alpha,
    beta;

  alpha=point->x-p->x;
  beta=point->y-p->y;
  dot_product=alpha*(q->x-p->x)+beta*(q->y-p->y);
  if (dot_product <= 0)
    return(alpha*alpha+beta*beta);
  phi=(q->x-p->x)*(q->x-p->x)+(q->y-p->y)*(q->y-p->y);
  gamma=dot_product*dot_product/phi;
  if (gamma <= phi)
    return(alpha*alpha+beta*beta-gamma+MagickEpsilon);
  alpha=point->x-q->x;
  beta=point->y-q->y;
  return(alpha*alpha+beta*beta);
}

static double PixelOnLine(const PointInfo *point,const PointInfo *p,
  const PointInfo *q,const double mid,const double opacity)
{
  register double
    alpha,
    distance;

  if ((mid == 0) || (opacity == 1.0))
    return(opacity);
  if ((p->x == q->x) && (p->y == q->y))
    return((point->x == p->x) && (point->y == p->y) ? 1.0 : opacity);
  distance=DistanceToLine(point,p,q);
  alpha=mid-0.5;
  if (distance <= (alpha*alpha))
    return(1.0);
  alpha=mid+0.5;
  if (distance <= (alpha*alpha))
    {
      alpha=sqrt(distance)-mid-0.5;
      return(Max(opacity,alpha*alpha));
    }
  return(opacity);
}

static double IntersectPrimitive(PrimitiveInfo *primitive_info,
  const DrawInfo *draw_info,const PointInfo *point,const int fill,Image *image)
{
  PixelPacket
    border_color;

  double
    alpha,
    beta,
    distance,
    mid,
    opacity,
    radius;

  register int
    i;

  register PrimitiveInfo
    *p,
    *q;

  PixelPacket
    target;

  assert(primitive_info != (PrimitiveInfo *) NULL);
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  mid=draw_info->linewidth/2.0;
  opacity=0.0;
  p=primitive_info;
  while (p->primitive != UndefinedPrimitive)
  {
    q=p+p->coordinates-1;
    switch (p->primitive)
    {
      case PointPrimitive:
      default:
      {
        if ((point->x == (int) (p->point.x+0.5)) &&
            (point->y == (int) (p->point.y+0.5)))
          opacity=1.0;
        break;
      }
      case LinePrimitive:
      {
        opacity=PixelOnLine(point,&p->point,&q->point,mid,opacity);
        break;
      }
      case CirclePrimitive:
      {
        alpha=p->point.x-point->x;
        beta=p->point.y-point->y;
        distance=sqrt(alpha*alpha+beta*beta);
        alpha=p->point.x-q->point.x;
        beta=p->point.y-q->point.y;
        radius=sqrt(alpha*alpha+beta*beta);
        if (fill)
          {
            if (distance <= (radius-1.0))
              opacity=1.0;
            else
              if (distance < (radius+1.0))
                {
                  alpha=(radius-distance+1.0)/2.0;
                  opacity=Max(opacity,alpha*alpha);
                }
            break;
          }
        if (fabs(distance-radius) < (mid+0.5))
          {
            if (fabs(distance-radius) <= (mid-0.5))
              opacity=1.0;
            else
              {
                alpha=mid-fabs(distance-radius)+0.5;
                opacity=Max(opacity,alpha*alpha);
              }
          }
        break;
      }
      case ArcPrimitive:
      case BezierPrimitive:
      case EllipsePrimitive:
      case PathPrimitive:
      case PolylinePrimitive:
      case PolygonPrimitive:
      case RectanglePrimitive:
      case RoundRectanglePrimitive:
      {
        double
          minimum_distance,
          poly_opacity;

        int
          crossing,
          crossings;

        PrimitiveInfo
          *primitive_info;

        if (!fill)
          {
            poly_opacity=0.0;
            for ( ; (p < q) && (poly_opacity != 1.0); p++)
              poly_opacity=PixelOnLine(point,&p->point,&(p+1)->point,mid,
                Max(opacity,poly_opacity));
            opacity=Max(opacity,poly_opacity);
            break;
          }
        primitive_info=p;
        crossings=0;
        if ((point->y < q->point.y) != (point->y < p->point.y))
          {
            crossing=point->x < q->point.x;
            if (crossing != (point->x < p->point.x))
              crossings+=point->x < (q->point.x-(q->point.y-point->y)*
                (p->point.x-q->point.x)/(p->point.y-q->point.y));
            else
              if (crossing)
                crossings++;
          }
        for (p++; p <= q; p++)
        {
          if (point->y < (p-1)->point.y)
            {
              while ((p <= q) && (point->y < p->point.y))
                p++;
              if (p > q)
                break;
              crossing=point->x < (p-1)->point.x;
              if (crossing != (point->x < p->point.x))
                crossings+=point->x < ((p-1)->point.x-((p-1)->point.y-point->y)*
                  (p->point.x-(p-1)->point.x)/(p->point.y-(p-1)->point.y));
              else
                if (crossing)
                  crossings++;
              continue;
            }
          while ((p <= q) && (point->y >= p->point.y))
            p++;
          if (p > q)
            break;
          crossing=point->x < (p-1)->point.x;
          if (crossing != (point->x < p->point.x))
            crossings+=point->x < ((p-1)->point.x-((p-1)->point.y-point->y)*
              (p->point.x-(p-1)->point.x)/(p->point.y-(p-1)->point.y));
          else
            if (crossing)
              crossings++;
        }
        poly_opacity=crossings & 0x01 ? 1.0 : 0.0;
        minimum_distance=DistanceToLine(point,&q->point,&primitive_info->point);
        for (p=primitive_info ; p < q; p++)
        {
          distance=DistanceToLine(point,&p->point,&(p+1)->point);
          if (distance < minimum_distance)
            minimum_distance=distance;
        }
        if (minimum_distance <= (0.5*0.5))
          {
            if (crossings & 0x01)
              alpha=0.5+sqrt(minimum_distance);
            else
              alpha=0.5-sqrt(minimum_distance);
            poly_opacity=alpha*alpha;
          }
        if (primitive_info->method == FillToBorderMethod)
          if ((opacity != 0.0) && (poly_opacity != 0.0))
            {
              opacity=0.0;
              for (p=primitive_info ; (p < q) && (opacity != 1.0); p++)
                opacity=PixelOnLine(point,&p->point,&(p+1)->point,1.0,opacity);
              break;
            }
        opacity=Max(opacity,poly_opacity);
        break;
      }
      case ColorPrimitive:
      {
        switch (p->method)
        {
          case PointMethod:
          default:
          {
            if ((point->x != (int) (p->point.x+0.5)) ||
                (point->y != (int) (p->point.y+0.5)))
              break;
            opacity=1.0;
            break;
          }
          case ReplaceMethod:
          {
            PixelPacket
              target;

            if ((point->x != (int) (p->point.x+0.5)) ||
                (point->y != (int) (p->point.y+0.5)))
              break;
            target=GetOnePixel(image,(int) p->point.x,(int) p->point.y);
            (void) OpaqueImage(image,target,draw_info->fill);
            opacity=0.0;
            break;
          }
          case FloodfillMethod:
          case FillToBorderMethod:
          {
            if ((point->x != (int) (p->point.x+0.5)) ||
                (point->y != (int) (p->point.y+0.5)))
              break;
            target=GetOnePixel(image,(int) p->point.x,(int) p->point.y);
            if (p->method == FillToBorderMethod)
              {
                border_color=draw_info->border_color;
                target=border_color;
              }
            (void) ColorFloodfillImage(image,draw_info,target,
              (int) point->x,(int) point->y,p->method);
            break;
          }
          case ResetMethod:
          {
            opacity=1.0;
            break;
          }
        }
        break;
      }
      case MattePrimitive:
      {
        register PixelPacket
          *q;

        if (!image->matte)
          MatteImage(image,OpaqueOpacity);
        switch (p->method)
        {
          case PointMethod:
          default:
          {
            if ((point->x != (int) (p->point.x+0.5)) ||
                (point->y != (int) (p->point.y+0.5)))
              break;
            q=GetImagePixels(image,(int) point->x,(int) point->y,1,1);
            if (q != (PixelPacket *) NULL)
              {
                q->opacity=TransparentOpacity;
                (void) SyncImagePixels(image);
              }
            break;
          }
          case ReplaceMethod:
          {
            PixelPacket
              target;

            if ((point->x != (int) (p->point.x+0.5)) ||
                (point->y != (int) (p->point.y+0.5)))
              break;
            target=GetOnePixel(image,(int) p->point.x,(int) p->point.y);
            (void) TransparentImage(image,target);
            opacity=0.0;
            break;
          }
          case FloodfillMethod:
          case FillToBorderMethod:
          {
            if ((point->x != (int) (p->point.x+0.5)) ||
                (point->y != (int) (p->point.y+0.5)))
              break;
            target=GetOnePixel(image,(int) p->point.x,(int) p->point.y);
            if (p->method == FillToBorderMethod)
              {
                border_color=draw_info->border_color;
                target=border_color;
              }
            (void) MatteFloodfillImage(image,target,TransparentOpacity,
              (int) point->x,(int) point->y,p->method);
            break;
          }
          case ResetMethod:
          {
            q=GetImagePixels(image,(int) point->x,(int) point->y,1,1);
            if (q != (PixelPacket *) NULL)
              {
                q->opacity=OpaqueOpacity;
                (void) SyncImagePixels(image);
              }
            break;
          }
        }
        break;
      }
      case TextPrimitive:
      {
        AnnotateInfo
          *annotate;

        ImageInfo
          *clone_info;

        register char
          *r;

        if ((point->x != (int) (p->point.x+0.5)) ||
            (point->y != (int) (p->point.y+0.5)))
          break;
        if (p->text == (char *) NULL)
          break;
        r=p->text;
        if (*r == '"')
          {
            p->text++;
            for (r++; *r != '\0'; r++)
              if ((*r == '"') && (*(r-1) != '\\'))
                break;
          }
        else
          if (*r == '\'')
            {
              p->text++;
              for (r++; *r != '\0'; r++)
                if ((*r == '\'') && (*(r-1) != '\\'))
                  break;
            }
          else
            for (r++;  *r != '\0'; r++)
              if (isspace((int) *r) && (*(r-1) != '\\') && (*r != '\0'))
                break;
        clone_info=CloneImageInfo((ImageInfo *) NULL);
        clone_info->font=AllocateString(draw_info->font);
        clone_info->antialias=draw_info->antialias;
        clone_info->pointsize=draw_info->pointsize;
        for (i=0; i < 6; i++)
          clone_info->affine[i]=draw_info->affine[i];
        annotate=CloneAnnotateInfo(clone_info,(AnnotateInfo *) NULL);
        DestroyImageInfo(clone_info);
        annotate->degrees=draw_info->angle;
        annotate->gravity=draw_info->gravity;
        annotate->decorate=draw_info->decorate;
        annotate->geometry=AllocateString("");
        annotate->fill=draw_info->fill;
        annotate->stroke=draw_info->stroke;
        annotate->box=draw_info->box;
        annotate->text=(char *) AcquireMemory(r-p->text+1);
        if (annotate->text == (char *) NULL)
          MagickError(ResourceLimitError,"Unable to annotate image",
            "Memory allocation failed");
        (void) strncpy(annotate->text,p->text,r-p->text);
        annotate->text[r-p->text]='\0';
        FormatString(annotate->geometry,"%+f%+f",p->point.x,p->point.y);
        AnnotateImage(image,annotate);
        DestroyAnnotateInfo(annotate);
        break;
      }
      case ImagePrimitive:
        break;
    }
    p=q+1;
  }
  return(opacity);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a t t e F l o o d f i l l I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MatteFloodfillImage floodfills the designated area with a matte
%  value.  The floodfill algorithm is strongly based on a similar algorithm in
%  "Graphics Gems" by Paul Heckbert.
%
%  The format of the MatteFloodfillImage method is:
%
%      unsigned int MatteFloodfillImage(Image *image,const PixelPacket target,
%        const unsigned int matte,const int x_offset,const int y_offset,
%        const PaintMethod method)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o target: A PixelPacket structure.  This is the RGB value of the target
%      color.
%
%    o matte: A integer value representing the amount of transparency.
%
%    o x,y: Unsigned integers representing the current location of the pen.
%
%    o method: drawing method of type PrimitiveType: floodfill or fill to
%      border.
%
%
*/
MagickExport unsigned int MatteFloodfillImage(Image *image,
  const PixelPacket target,const unsigned int matte,const int x_offset,
  const int y_offset,const PaintMethod method)
{
  int
    offset,
    skip,
    start,
    x1,
    x2,
    y;

  register int
    x;

  register PixelPacket
    *q;

  register SegmentInfo
    *s;

  SegmentInfo
    *segment_stack;

  /*
    Check boundary conditions.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if ((x_offset < 0) || (x_offset >= (int) image->columns))
    return(False);
  if ((y_offset < 0) || (y_offset >= (int) image->rows))
    return(False);
  if (target.opacity == matte)
    return(False);
  q=GetImagePixels(image,x_offset,y_offset,1,1);
  if (q == (PixelPacket *) NULL)
    return(False);
  if (q->opacity == matte)
    return(False);
  /*
    Allocate segment stack.
  */
  segment_stack=(SegmentInfo *)
    AcquireMemory(MaxStacksize*sizeof(SegmentInfo));
  if (segment_stack == (SegmentInfo *) NULL)
    ThrowBinaryException(ResourceLimitWarning,"Unable to floodfill image",
      image->filename);
  /*
    Push initial segment on stack.
  */
  image->storage_class=DirectClass;
  if (!image->matte)
    MatteImage(image,OpaqueOpacity);
  x=x_offset;
  y=y_offset;
  start=0;
  s=segment_stack;
  Push(y,x,x,1);
  Push(y+1,x,x,-1);
  while (s > segment_stack)
  {
    /*
      Pop segment off stack.
    */
    s--;
    x1=(int) s->x1;
    x2=(int) s->x2;
    offset=(int) s->y2;
    y=(int) s->y1+offset;
    /*
      Recolor neighboring points.
    */
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    q+=x1;
    for (x=x1; x >= 0 ; x--)
    {
      if (method == FloodfillMethod)
        {
          if (!MatteMatch(*q,target,image->fuzz))
            break;
        }
      else
        if (MatteMatch(*q,target,image->fuzz) || (q->opacity == matte))
          break;
      q->opacity=matte;
      q--;
    }
    if (!SyncImagePixels(image))
      break;
    skip=x >= x1;
    if (!skip)
      {
        start=x+1;
        if (start < x1)
          Push(y,start,x1-1,-offset);
        x=x1+1;
      }
    do
    {
      if (!skip)
        {
          q=GetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          q+=x;
          for ( ; x < (int) image->columns; x++)
          {
            if (method == FloodfillMethod)
              {
                if (!MatteMatch(*q,target,image->fuzz))
                  break;
              }
            else
              if (MatteMatch(*q,target,image->fuzz) || (q->opacity == matte))
                break;
            q->opacity=matte;
            q++;
          }
          if (!SyncImagePixels(image))
            break;
          Push(y,start,x-1,offset);
          if (x > (x2+1))
            Push(y,x2+1,x-1,-offset);
        }
      skip=False;
      q=GetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      q+=x;
      for (x++; x <= x2 ; x++)
      {
        q++;
        if (method == FloodfillMethod)
          {
            if (MatteMatch(*q,target,image->fuzz))
              break;
          }
        else
          if (!MatteMatch(*q,target,image->fuzz) && (q->opacity != matte))
            break;
      }
      start=x;
    } while (x <= x2);
  }
  LiberateMemory((void **) &segment_stack);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     O p a q u e I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method OpaqueImage changes the color of an opaque point to the pen color.
%
%  The format of the OpaqueImage method is:
%
%      unsigned int OpaqueImage(Image *image,const PixelPacket target,
%        const PixelPacket pen_color)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o target: the color to search for in the image.
%
%    o pen_color: the color to replace it with.
%
%
*/
MagickExport unsigned int OpaqueImage(Image *image,const PixelPacket target,
  const PixelPacket pen_color)
{
#define OpaqueImageText  "  Setting opaque color in the image...  "

  int
    y;

  register int
    i,
    x;

  register PixelPacket
    *q;

  /*
    Make image color opaque.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      /*
        Make DirectClass image opaque.
      */
      for (y=0; y < (int) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) image->columns; x++)
        {
          if (ColorMatch(*q,target,image->fuzz))
            *q=pen_color;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          ProgressMonitor(OpaqueImageText,y,image->rows);
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Make PseudoClass image opaque.
      */
      for (i=0; i < (int) image->colors; i++)
      {
        if (ColorMatch(image->colormap[i],target,image->fuzz))
          image->colormap[i]=pen_color;
        if (QuantumTick(i,image->colors))
          ProgressMonitor(OpaqueImageText,i,image->colors);
      }
      SyncImage(image);
      break;
    }
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     T r a n s p a r e n t I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TransparentImage creates a matte image associated with the
%  image.  All pixel locations are initially set to opaque.  Any pixel
%  that matches the specified color are set to transparent.
%
%  The format of the TransparentImage method is:
%
%      unsigned int TransparentImage(Image *image,const PixelPacket target)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o target: The color to search for in the image.
%
%
*/
MagickExport unsigned int TransparentImage(Image *image,
  const PixelPacket target)
{
#define TransparentImageText  "  Setting transparent color in the image...  "

  int
    y;

  register int
    x;

  register PixelPacket
    *q;

  /*
    Make image color transparent.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (!image->matte)
    MatteImage(image,OpaqueOpacity);
  for (y=0; y < (int) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) image->columns; x++)
    {
      if (ColorMatch(*q,target,image->fuzz))
        q->opacity=TransparentOpacity;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      ProgressMonitor(TransparentImageText,y,image->rows);
  }
  return(True);
}
