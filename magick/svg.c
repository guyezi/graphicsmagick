/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            SSSSS  V   V   GGGG                              %
%                            SS     V   V  G                                  %
%                             SSS   V   V  G GG                               %
%                               SS   V V   G   G                              %
%                            SSSSS    V     GGG                               %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
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
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"


/*
  Forward declarations.
*/
static unsigned int
  WriteSVGImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s S V G                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsSVG returns True if the image format type, identified by the
%  magick string, is SVG.
%
%  The format of the IsSVG method is:
%
%      unsigned int IsSVG(const unsigned char *magick,
%        const unsigned int length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsSVG returns True if the image format type is SVG.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsSVG(const unsigned char *magick,
  const unsigned int length)
{
  if (length < 5)
    return(False);
  if (LocaleNCompare((char *) magick,"<?xml",5) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d S V G I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadSVGImage reads a Scalable Vector Gaphics file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadSVGImage method is:
%
%      Image *ReadSVGImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadSVGImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static unsigned int GetToken(Image *image,char **token,int *c,
  ExceptionInfo *exception)
{
  register char
    *p;

  unsigned int
    length,
    quote;

  length=1;
  p=(*token);
  for (*p='\0'; *c != EOF; )
  {
    while (isspace((int) *c))
      *c=ReadByte(image);
    quote=(*c) == '"';
    for ( ; ; )
    {
      if ((*c == '\n') || (*c == '\r'))
        *c=' ';
      if (Extent(*token) >= (length-MaxTextExtent-1))
        {
          length<<=1;
          *token=(char *) ReallocateMemory(*token,length);
          if (*token == (char *) NULL)
            ThrowBinaryException(ResourceLimitWarning,"Unable to get token",
              "Memory allocation failed");
          p=(*token)+Extent(*token);
        }
      if (*c != '"')
        *p++=(*c);
      *p='\0';
      *c=ReadByte(image);
      if (!quote)
        break;
      if ((*c == '"') || (*c == EOF))
        break;
    }
    if (quote)
      {
        *c=ReadByte(image);
        break;
      }
    p--;
    if (!isalnum((int) *p) && (*p != '-') && (*p != '>'))
      break;
    p++;
    if (!isalnum(*c) && (*c != '-') && (*c != '>'))
      break;
  }
  if (*c == EOF)
    return(False);
  return(True);
}

Export char **StringToTokens(const char *text,int *number_tokens)
{
  char
    **tokens;

  register char
    *p,
    *q;

  register int
    i;

  *number_tokens=0;
  if (text == (char *) NULL)
    return((char **) NULL);
  /*
    Determine the number of arguments.
  */
  for (p=(char *) text; *p != '\0'; )
  {
    while (isspace((int) (*p)))
      p++;
    (*number_tokens)++;
    if (*p == '"')
      for (p++; (*p != '"') && (*p != '\0'); p++);
    if (*p == '\'')
      for (p++; (*p != '\'') && (*p != '\0'); p++);
    if (*p == '(')
      for (p++; (*p != ')') && (*p != '\0'); p++);
    while (!isspace((int) (*p)) && (*p != '(') && (*p != '\0'))
    {
      p++;
      if (!isspace((int) *p) && ((*(p-1) == ':') || (*(p-1) == ';')))
        (*number_tokens)++;
    }
  }
  tokens=(char **) AllocateMemory((*number_tokens+1)*sizeof(char *));
  if (tokens == (char **) NULL)
    MagickError(ResourceLimitError,"Unable to convert string to tokens",
      "Memory allocation failed");
  /*
    Convert string to an ASCII list.
  */
  p=(char *) text;
  for (i=0; i < *number_tokens; i++)
  {
    while (isspace((int) (*p)))
      p++;
    q=p;
    if (*q == '"')
      {
        p++;
        for (q++; (*q != '"') && (*q != '\0'); q++);
      }
    else
      if (*q == '\'')
        {
          for (q++; (*q != '\'') && (*q != '\0'); q++);
          q++;
        }
      else
        if (*q == '(')
          {
            for (q++; (*q != ')') && (*q != '\0'); q++);
            q++;
          }
        else
          while (!isspace((int) (*q)) && (*q != '(') && (*q != '\0'))
          {
            q++;
            if (!isspace((int) *q) && ((*(q-1) == ':') || (*(q-1) == ';')))
              break;
          }
    tokens[i]=(char *) AllocateMemory(q-p+1);
    if (tokens[i] == (char *) NULL)
      MagickError(ResourceLimitError,"Unable to convert string to tokens",
        "Memory allocation failed");
    (void) strncpy(tokens[i],p,q-p);
    tokens[i][q-p]='\0';
    p=q;
    if ((*(q-1) == ':') || (*(q-1) == ';') || (*q == '('))
      continue;
    while (!isspace((int) (*p)) && (*p != '\0'))
      p++;
  }
  tokens[i]=(char *) NULL;
  return(tokens);
}

#define BezierCoordinates  4
      
static void ReflectPoint(PointInfo *input, PointInfo *output)
{
  double
    temp;
      
  /* reflect the x control point */
  temp=input[3].x - input[0].x;
  if (!temp)
    temp=1.0;
  temp=(input[3].x - input[2].x)/temp;
  temp=temp*(output[3].x - input[3].x);
  output[1].x=temp+input[3].x;

  /* reflect the y control point */
  temp=input[3].y - input[0].y;
  if (!temp)
    temp=1.0;
  temp=(input[3].y - input[2].y)/temp;
  temp=temp*(output[3].y - input[3].y);
  output[1].y=temp+input[3].y;
}

static void BezierSmoothPoints(PointInfo *input, PointInfo *output, int outlen)
{
#ifdef SMOOTHING_STUFF
  double
    alpha,
    weight,
    coefficients[BezierCoordinates];

  PointInfo
    pixel;

  register int
    i,
    j;

  for (i=0; i < BezierCoordinates; i++)
    coefficients[i]=Permutate(BezierCoordinates-1,i);
  weight=0.0;
  for (i=0; i < (BezierCoordinates*outlen); i++)
  {
    pixel.x=0;
    pixel.y=0;
    alpha=pow(1.0-weight,BezierCoordinates-1);
    for (j=0; j < BezierCoordinates; j++)
    {
      pixel.x+=alpha*coefficients[j]*input[j].x;
      pixel.y+=alpha*coefficients[j]*input[j].y;
      alpha*=weight/(1.0-weight);
    }
    output[i]=pixel;
    weight+=1.0/outlen/BezierCoordinates;
  }
#else
  register int
    i;

  for (i=0; i < BezierCoordinates; i++)
    output[i]=input[i];
#endif
}

#define BezierQuantum  1

static char *TraversePath(const char *data)
{
  char
    *path,
    points[MaxTextExtent];

  double
    x,
    y;

  int
    attribute,
    n;

  PointInfo
    point,
    start,
    lastp,
    pixels[BezierCoordinates];

  register const char
    *p;

  path=AllocateString("");
  p=data;
  while (*p != '\0')
  {
    while (isspace(*p))
      p++;
    attribute=(*p);
    switch (attribute)
    {
      case 'c':
      case 'C':
      {
        PointInfo
          newpixels[BezierCoordinates];
      
        register int
          i;
      
        /*
          Compute bezier points.
        */
        p++;
        pixels[0]=point;
        for (i=1; i < BezierCoordinates; i++)
        {
          n=0;
          if ((*p == ',') || isspace(*p))
            p++;
          (void) sscanf(p,"%lf%lf%n",&x,&y,&n);
          if (n == 0)
            (void) sscanf(p,"%lf,%lf%n",&x,&y,&n);
          if (n == 0)
            break;
          lastp.x=attribute == 'C' ? x : point.x+x;
          lastp.y=attribute == 'C' ? y : point.y+y;
          pixels[i]=lastp;
          p+=n;
        }

        BezierSmoothPoints(pixels, newpixels, BezierQuantum);
        for (i=0; i < BezierCoordinates; i++)
        {
          (void) FormatString(points,"%g,%g ",newpixels[i].x,newpixels[i].y);
          if (!ConcatenateString(&path,points))
            return((char *) NULL);
        }
        point=lastp;
        p--;
        break;
      }
      case 's':
      case 'S':
      {
        PointInfo
          newpixels[BezierCoordinates];
      
        register int
          i;

        /*
          Compute bezier points.
        */
        p++;
        newpixels[0]=pixels[3];
        for (i=2; i < BezierCoordinates; i++)
        {
          n=0;
          if ((*p == ',') || isspace(*p))
            p++;
          (void) sscanf(p,"%lf%lf%n",&x,&y,&n);
          if (n == 0)
            (void) sscanf(p,"%lf,%lf%n",&x,&y,&n);
          if (n == 0)
            break;
          lastp.x=attribute == 'S' ? x : point.x+x;
          lastp.y=attribute == 'S' ? y : point.y+y;
          newpixels[i]=lastp;
          p+=n;
        }

        ReflectPoint(pixels, newpixels);
        for (i=0; i < BezierCoordinates; i++)
          pixels[i]=newpixels[i];

        BezierSmoothPoints(pixels, newpixels, BezierQuantum);
        for (i=0; i < BezierCoordinates; i++)
        {
          (void) FormatString(points,"%g,%g ",pixels[i].x,pixels[i].y);
          if (!ConcatenateString(&path,points))
            return((char *) NULL);
        }

        point=lastp;
        p--;
        break;
      }
      case 'h':
      case 'H':
      {
        (void) sscanf(p+1,"%lf%n",&x,&n);
        point.x=attribute == 'H' ? x: point.x+x;
        (void) FormatString(points,"%g,%g ",point.x,point.y);
        if (!ConcatenateString(&path,points))
          return((char *) NULL);
        p+=n;
        break;
      }
      case 'l':
      case 'L':
      {
        for (n=0; ; n=0)
        {
          (void) sscanf(p+1,"%lf%lf%n",&x,&y,&n);
          if (n == 0)
            (void) sscanf(p+1,"%lf,%lf%n",&x,&y,&n);
          if (n == 0)
            break;
          point.x=attribute == 'L' ? x : point.x+x;
          point.y=attribute == 'L' ? y : point.y+y;
          (void) FormatString(points,"%g,%g  ",point.x,point.y);
          if (!ConcatenateString(&path,points))
            return((char *) NULL);
          p+=n;
        }
        break;
      }
      case 'M':
      {
        n=0;
        (void) sscanf(p+1,"%lf%lf%n",&point.x,&point.y,&n);
        if (n == 0)
          (void) sscanf(p+1,"%lf,%lf%n",&point.x,&point.y,&n);
        (void) FormatString(points,"%g,%g ",point.x,point.y);
        if (!ConcatenateString(&path,points))
          return((char *) NULL);
        start.x=point.x;
        start.y=point.y;
        p+=n;
        break;
      }
      case 'v':
      case 'V':
      {
        (void) sscanf(p+1,"%lf%n",&y,&n);
        point.y=attribute == 'V' ? y : point.y+y;
        (void) FormatString(points,"%g,%g ",point.x,point.y);
        if (!ConcatenateString(&path,points))
          return((char *) NULL);
        p+=n;
        break;
      }
      case 'z':
      case 'Z':
      {
        point.x=start.x;
        point.y=start.y;
        (void) FormatString(points,"%g,%g ",start.x,start.y);
        if (!ConcatenateString(&path,points))
          return((char *) NULL);
        break;
      }
      default:
      {
        if (isalpha((int) *p))
          (void) fprintf(stderr,"attribute not implemented: %c\n",*p);
        break;
      }
    }
    p++;
  }
  return(path);
}

static double UnitOfMeasure(const char *value)
{
  assert(value != (const char *) NULL);
  if (Extent(value) < 3)
    return(1.0);
  if (LocaleCompare(value+strlen(value)-2,"cm") == 0)
    return(72.0/2.54);
  if (LocaleCompare(value+strlen(value)-2,"in") == 0)
    return(72.0);
  if (LocaleCompare(value+strlen(value)-2,"pt") == 0)
    return(1.0);
  return(1.0);
}

static Image *ReadSVGImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define MaxContexts  256

  typedef struct _ElementInfo
  {
    double
      cx,
      cy,
      major,
      minor,
      angle;
  } ElementInfo;

  typedef struct _GraphicContext
  {
    char
      *fill,
      *stroke;

    unsigned int
      antialias;

    double
      linewidth,
      pointsize,
      opacity;
  } GraphicContext;

  char
    filename[MaxTextExtent],
    geometry[MaxTextExtent],
    *keyword,
    *primitive,
    *text,
    *token,
    **tokens,
    *url,
    *value,
    *vertices;

  DrawInfo
    *draw_info;

  ElementInfo
    element;

  FILE
    *file;

  GraphicContext
    graphic_context[MaxContexts];

  Image
    *image;

  int
    c,
    n,
    length,
    number_tokens;

  PointInfo
    translate;

  RectangleInfo
    page;

  SegmentInfo
    segment;

  register int
    i;

  unsigned int
    height,
    quote,
    status,
    width;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  image->columns=1;
  image->rows=1;
  SetImage(image,Opaque);
  /*
    Open draw file.
  */
  (void) strcpy(filename,"@");
  TemporaryFilename(filename+1);
  file=fopen(filename+1,"w");
  if (file == (FILE *) NULL)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Interpret SVG language.
  */
  element.cx=0.0;
  element.cy=0.0;
  element.minor=0.0;
  element.major=0.0;
  element.angle=0.0;
  height=image->rows;
  for (i=0; i < MaxContexts; i++)
  {
    graphic_context[i].fill=AllocateString("none");
    graphic_context[i].stroke=AllocateString("none");
    graphic_context[i].opacity=100.0;
    graphic_context[i].linewidth=1.0;
    graphic_context[i].pointsize=12.0;
    graphic_context[i].antialias=True;
  }
  n=0;
  keyword=AllocateString("none");
  quote=False;
  GetPageInfo(&page);
  primitive=AllocateString("none");
  text=AllocateString("none");
  token=AllocateString("none");
  url=AllocateString("logo:");
  value=AllocateString("none");
  vertices=AllocateString("");
  width=image->columns;
  /*
    Parse SVG drawing primitives.
  */
  length=MaxTextExtent;
  c=ReadByte(image);
  while (GetToken(image,&token,&c,exception))
  {
    /*
      Interpret the SVG tokens.
    */
    if (*token == '>')
      {
        quote=True;
        CloneString(&text,"");
      }
    else
      if (*token == '<')
        quote=False;
    if (quote)
      {
        if (isalnum(*token))
          (void) ConcatenateString(&text," ");
        (void) ConcatenateString(&text,token);
        (void) CloneString(&keyword,token);
        continue;
      }
    if (LocaleCompare(token,"=") == 0)
      (void) GetToken(image,&value,&c,exception);
    if (((LocaleCompare(keyword,">") == 0) ||
         (LocaleCompare(keyword,"text>") == 0)) &&
        (LocaleCompare(primitive,"none") != 0))
      {
        char
          *command,
          points[MaxTextExtent];

        unsigned int
          length;

        /*
          Render graphic primitive.
        */
        if ((LocaleCompare(primitive,"rectangle") == 0) &&
            (element.major != 0.0))
          CloneString(&primitive,"roundRectangle");
        length=strlen(primitive)+MaxTextExtent;
        if (vertices != (char *) NULL)
          length+=strlen(vertices);
        command=(char *) AllocateMemory(length);
        if (command == (char *) NULL)
          ThrowReaderException(ResourceLimitWarning,"Unable to allocate memory",
            image);
        (void) strcpy(command,primitive);
        (void) strcat(command," ");
        if (LocaleCompare(primitive,"circle") == 0)
          {
            FormatString(points,"%g,%g %g,%g",element.cx,element.cy,
              element.cx,element.cy+element.minor);
            (void) strcat(command,points);
          }
        if (LocaleCompare(primitive,"ellipse") == 0)
          {
            FormatString(points,"%g,%g %g,%g 0,360",element.cx,element.cy,
              element.major,element.minor);
            (void) strcat(command,points);
          }
        if (LocaleCompare(primitive,"line") == 0)
          {
            FormatString(points,"%g,%g %g,%g",segment.x1,segment.y1,
              segment.x2,segment.y2);
            (void) strcat(command,points);
          }
        if (LocaleCompare(primitive,"polyline") == 0)
          (void) strcat(command,vertices);
        if (LocaleCompare(primitive,"polygon") == 0)
          (void) strcat(command,vertices);
        if (LocaleCompare(primitive,"rectangle") == 0)
          {
            FormatString(points,"%d,%d %d,%d",page.x,page.y,
              page.x+page.width,page.y+page.height);
            (void) strcat(command,points);
          }
        if (LocaleCompare(primitive,"roundRectangle") == 0)
          {
            FormatString(points,"%g,%g %d,%d %g,%g",page.x+page.width/2.0,
              page.y+page.height/2.0,page.width,page.height,
              element.major/2.0,element.minor/2.0);
            (void) strcat(command,points);
          }
        if (LocaleCompare(primitive,"image") == 0)
          {
            FormatString(points,"%d,%d",page.x,page.y);
            (void) strcat(command,points);
            (void) strcat(command," ");
            (void) strcat(command,url);
          }
        if (LocaleCompare(primitive,"text") == 0)
          {
            FormatString(points,"%d,%g",page.x,page.y-
              graphic_context[n].pointsize/2.0);
            (void) strcat(command,points);
            (void) strcat(command," '");
            (void) strcat(command,text+1);
            (void) strcat(command,"'");
          }
        (void) fprintf(file,"antialias %d\n",
          graphic_context[n].antialias ? 1 : 0);
        (void) fprintf(file,"linewidth %g\n",graphic_context[n].linewidth);
        (void) fprintf(file,"pointsize %g\n",graphic_context[n].pointsize);
        (void) fprintf(file,"translate %g,%g\n",translate.x,translate.y);
        (void) fprintf(file,"rotate %g\n",element.angle);
        (void) fprintf(file,"opacity %g\n",graphic_context[n].opacity);
        if (LocaleCompare(graphic_context[n].fill,"none") != 0)
          {
            (void) fprintf(file,"pen %s\n",graphic_context[n].fill);
            (void) fprintf(file,"fill 1\n");
            (void) fprintf(file,"%s\n",command);
          }
        if (LocaleCompare(graphic_context[n].stroke,"none") != 0)
          (void) fprintf(file,"pen %s\n",graphic_context[n].stroke);
        (void) fprintf(file,"fill 0\n");
        (void) fprintf(file,"%s\n",command);
        FreeMemory((void **) &command);
        (void) CloneString(&primitive,"none");
        (void) CloneString(&vertices," ");
        (void) CloneString(&graphic_context[0].fill,"none");
        (void) CloneString(&graphic_context[0].stroke,"none");
        graphic_context[0].opacity=100.0;
        graphic_context[0].linewidth=1;
        graphic_context[0].pointsize=12;
        graphic_context[0].antialias=True;
        element.cx=0.0;
        element.cy=0.0;
        element.minor=0.0;
        element.major=0.0;
        element.angle=0.0;
        translate.x=0.0;
        translate.y=0.0;
        (void) CloneString(&keyword,token);
        continue;
      }
    if (LocaleCompare(keyword,"d") == 0)
      {
        char
          *path;

        path=TraversePath(value);
        if (path == (char *) NULL)
          ThrowReaderException(ResourceLimitWarning,"Unable to allocate memory",
            image);
        (void) fprintf(file,"antialias %d\n",
          graphic_context[n].antialias ? 1 : 0);
        (void) fprintf(file,"linewidth %g\n",graphic_context[n].linewidth);
        (void) fprintf(file,"pointsize %g\n",graphic_context[n].pointsize);
        (void) fprintf(file,"translate %g,%g\n",translate.x,translate.y);
        (void) fprintf(file,"rotate %g\n",element.angle);
        (void) fprintf(file,"opacity %g\n",graphic_context[n].opacity);
        if (LocaleCompare(graphic_context[n].fill,"none") != 0)
          {
            (void) fprintf(file,"pen %s\n",graphic_context[n].fill);
            (void) fprintf(file,"fill 1\n");
            (void) fprintf(file,"polyline %s\n",path);
          }
        if (LocaleCompare(graphic_context[n].stroke,"none") != 0)
          (void) fprintf(file,"pen %s\n",graphic_context[n].stroke);
        (void) fprintf(file,"fill 0\n");
        (void) fprintf(file,"polyline %s\n",path);
        (void) FreeMemory((void **) &path);
        (void) CloneString(&keyword,token);
        continue;
      }
    if (LocaleCompare(keyword,">") == 0)
      CloneString(&primitive,"none");
    if (LocaleCompare(keyword,"angle") == 0)
      (void) sscanf(value,"%lf",&element.angle);
    if (LocaleCompare(keyword,"circle") == 0)
      CloneString(&primitive,"circle");
    if (LocaleCompare(keyword,"cx") == 0)
      {
        (void) sscanf(value,"%lf",&element.cx);
        element.cx*=UnitOfMeasure(value);
      }
    if (LocaleCompare(keyword,"cy") == 0)
      {
        (void) sscanf(value,"%lf",&element.cy);
        element.cy*=UnitOfMeasure(value);
      }
    if (LocaleCompare(keyword,"ellipse") == 0)
      CloneString(&primitive,"ellipse");
    if (LocaleCompare(keyword,"g") == 0)
      {
        if (*token == '>')
          n=Max(n-1,0);
        else
          n=Min(n+1,MaxContexts-1);
      }
    if (LocaleCompare(keyword,"height") == 0)
      {
        (void) sscanf(value,"%u",&page.height);
        page.height*=UnitOfMeasure(value);
        if (height < page.height)
          height=page.height;
      }
    if (LocaleCompare(keyword,"href") == 0)
      (void) CloneString(&url,value);
    if (LocaleCompare(keyword,"line") == 0)
      CloneString(&primitive,"line");
    if (LocaleCompare(keyword,"image") == 0)
      CloneString(&primitive,"image");
    if (LocaleCompare(keyword,"major") == 0)
      (void) sscanf(value,"%lf",&element.major);
    if (LocaleCompare(keyword,"minor") == 0)
      (void) sscanf(value,"%lf",&element.minor);
    if (LocaleCompare(keyword,"polygon") == 0)
      CloneString(&primitive,"polygon");
    if (LocaleCompare(keyword,"polyline") == 0)
      CloneString(&primitive,"polyline");
    if (LocaleCompare(keyword,"points") == 0)
      (void) CloneString(&vertices,value);
    if (LocaleCompare(keyword,"r") == 0)
      {
        (void) sscanf(value,"%lf",&element.major);
        element.major*=UnitOfMeasure(value);
        element.minor=element.major;
      }
    if (LocaleCompare(keyword,"rect") == 0)
      CloneString(&primitive,"rectangle");
    if (LocaleCompare(keyword,"rx") == 0)
      {
        (void) sscanf(value,"%lf",&element.major);
        element.major*=2.0;
        element.minor=element.major;
      }
    if (LocaleCompare(keyword,"ry") == 0)
      {
        (void) sscanf(value,"%lf",&element.minor);
        element.minor*=2.0;
      }
    if (LocaleCompare(keyword,"style") == 0)
      {
        tokens=StringToTokens(value,&number_tokens);
        for (i=0; i < (number_tokens-1); i++)
        {
          if (LocaleCompare(tokens[i],"fill:") == 0)
            {
              (void) CloneString(&value,tokens[++i]);
              if (LocaleCompare(value+Extent(value)-1,";") == 0)
                value[Extent(value)-1]='\0';
              (void) CloneString(&graphic_context[n].fill,value);
            }
          if (LocaleCompare(tokens[i],"fill-opacity:") == 0)
            {
              (void) sscanf(tokens[++i],"%lf",&graphic_context[n].opacity);
              if (strchr(tokens[i],'%') == (char *) NULL)
                graphic_context[n].opacity*=100.0;
            }
          if (LocaleCompare(tokens[i],"font-size:") == 0)
            {
              (void) sscanf(tokens[++i],"%lf",&graphic_context[n].pointsize);
              graphic_context[n].linewidth*=UnitOfMeasure(tokens[i]);
            }
          if (LocaleCompare(tokens[i],"stroke:") == 0)
            {
              (void) CloneString(&value,tokens[++i]);
              if (LocaleCompare(value+Extent(value)-1,";") == 0)
                value[Extent(value)-1]='\0';
              (void) CloneString(&graphic_context[n].stroke,value);
            }
          if (LocaleCompare(tokens[i],"stroke-antialiasing:") == 0)
            graphic_context[n].antialias=LocaleCompare(tokens[++i],"true");
          if (LocaleCompare(tokens[i],"stroke-opacity:") == 0)
            (void) sscanf(tokens[++i],"%lf",&graphic_context[n].opacity);
          if (LocaleCompare(tokens[i],"stroke-width:") == 0)
            {
              (void) sscanf(tokens[++i],"%lf",&graphic_context[n].linewidth);
              graphic_context[n].linewidth*=UnitOfMeasure(tokens[i]);
            }
          if (LocaleCompare(tokens[i],"text-antialiasing:") == 0)
            graphic_context[n].antialias=LocaleCompare(tokens[++i],"true");
          FreeMemory((void **) &tokens[i]);
        }
        FreeMemory((void **) &tokens);
      }
    if (LocaleCompare(keyword,"text") == 0)
      CloneString(&primitive,"text");
    if (LocaleCompare(keyword,"transform") == 0)
      {
        tokens=StringToTokens(value,&number_tokens);
        for (i=0; i < (number_tokens-1); i++)
        {
          if (LocaleCompare(tokens[i],"translate") == 0)
            (void) sscanf(tokens[++i]+1,"%lf %lf",&translate.x,&translate.y);
          if (LocaleCompare(tokens[i],"rotate") == 0)
            (void) sscanf(tokens[++i]+1,"%lf",&element.angle);
          FreeMemory((void **) &tokens[i]);
        }
        FreeMemory((void **) &tokens);
      }
    if (LocaleCompare(keyword,"verts") == 0)
      (void) CloneString(&vertices,value);
    if (LocaleCompare(keyword,"viewBox") == 0)
      {
        (void) sscanf(value,"%d %d %u %u",&page.x,&page.y,
          &page.width,&page.height);
        if (height < page.height)
          height=page.height;
        if (width < page.width)
          width=page.width;
      }
    if (LocaleCompare(keyword,"width") == 0)
      {
        (void) sscanf(value,"%u",&page.width);
        page.width*=UnitOfMeasure(value);
        if (width < page.width)
          width=page.width;
      }
    if (LocaleCompare(keyword,"x") == 0)
      {
        (void) sscanf(value,"%d",&page.x);
        page.x*=UnitOfMeasure(value);
      }
    if (LocaleCompare(keyword,"x1") == 0)
      (void) sscanf(value,"%lf",&segment.x1);
    if (LocaleCompare(keyword,"x2") == 0)
      (void) sscanf(value,"%lf",&segment.x2);
    if (LocaleCompare(keyword,"y") == 0)
      {
        (void) sscanf(value,"%d",&page.y);
        page.y*=UnitOfMeasure(value);
      }
    if (LocaleCompare(keyword,"y1") == 0)
      (void) sscanf(value,"%lf",&segment.y1);
    if (LocaleCompare(keyword,"y2") == 0)
      (void) sscanf(value,"%lf",&segment.y2);
    (void) CloneString(&keyword,token);
  }
  /*
    Free resources.
  */
  for (i=0; i < MaxContexts; i++)
  {
    FreeMemory((void **) &graphic_context[i].fill);
    FreeMemory((void **) &graphic_context[i].stroke);
  }
  FreeMemory((void **) &vertices);
  FreeMemory((void **) &value);
  FreeMemory((void **) &url);
  FreeMemory((void **) &token);
  FreeMemory((void **) &text);
  FreeMemory((void **) &primitive);
  FreeMemory((void **) &keyword);
  (void) fclose(file);
  CloseBlob(image);
  /*
    Draw image.
  */
  FormatString(geometry,"%ux%u!",width,height);
  TransformImage(&image,(char *) NULL,geometry);
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  (void) CloneString(&draw_info->primitive,filename);
  status=DrawImage(image,draw_info);
puts(filename); if (0)
  (void) remove(filename+1);
  if (status == False)
    ThrowReaderException(CorruptImageWarning,"Unable to read SVG image",image);
  DestroyDrawInfo(draw_info);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r S V G I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterSVGImage adds attributes for the SVG image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterSVGImage method is:
%
%      RegisterSVGImage(void)
%
*/
Export void RegisterSVGImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("SVG");
  entry->magick=IsSVG;
  entry->decoder=ReadSVGImage;
  entry->description=AllocateString("Scalable Vector Gaphics");
  entry->module=AllocateString("SVG");
  RegisterMagickInfo(entry);
  entry=SetMagickInfo("XML");
  entry->magick=IsSVG;
  entry->decoder=ReadSVGImage;
  entry->description=AllocateString("Scalable Vector Gaphics");
  entry->module=AllocateString("SVG");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r S V G I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterSVGImage removes format registrations made by the
%  SVG module from the list of supported formats.
%
%  The format of the UnregisterSVGImage method is:
%
%      UnregisterSVGImage(void)
%
*/
Export void UnregisterSVGImage(void)
{
  UnregisterMagickInfo("SVG");
}
