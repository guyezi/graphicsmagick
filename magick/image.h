/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
  Copyright 1991-1999 E. I. du Pont de Nemours and Company
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick Image Methods.
*/
#ifndef _MAGICK_IMAGE_H
#define _MAGICK_IMAGE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Include declarations.
*/
#include "magick/forward.h"
#include "magick/colorspace.h"
#include "magick/error.h"
#include "magick/timer.h"

/*
  Define declarations.
*/
#if !defined(QuantumDepth)
#  define QuantumDepth  16
#endif

/*
  Maximum unsigned RGB value which fits in the specified bits
*/
#define MaxValueGivenBits(bits) ((unsigned long) (0x01UL << (bits-1)) +((0x01UL << (bits-1))-1))

#if (QuantumDepth == 8)
#  define MaxColormapSize  256U
#  define MaxMap  255U
#  define MaxRGB  255U
#  define ScaleCharToMap(value)        ((unsigned char) (value))
#  define ScaleCharToQuantum(value)    ((Quantum) (value))
#  define ScaleLongToQuantum(value)    ((Quantum) ((value)/16843009UL))
#  define ScaleMapToChar(value)        ((unsigned int) (value))
#  define ScaleMapToQuantum(value)     ((Quantum) (value))
#  define ScaleQuantum(quantum)        ((unsigned long) (quantum))
#  define ScaleQuantumToChar(quantum)  ((unsigned char) (quantum))
#  define ScaleQuantumToLong(quantum)  ((unsigned long) (16843009UL*(quantum)))
#  define ScaleQuantumToMap(quantum)   ((unsigned char) (quantum))
#  define ScaleQuantumToShort(quantum) ((unsigned short) (257U*(quantum)))
#  define ScaleShortToQuantum(value)   ((Quantum) ((value)/257U))
#  define ScaleToQuantum(value)        ((unsigned long) (value))
#  define ScaleQuantumToIndex(value)   ((unsigned char) (value))
   typedef unsigned char Quantum;
#elif (QuantumDepth == 16)
#  define MaxColormapSize  65536U
#  define MaxMap 65535U
#  define MaxRGB  65535U
#  define ScaleCharToMap(value)        ((unsigned short) (257U*(value)))
#  define ScaleCharToQuantum(value)    ((Quantum) (257U*(value)))
#  define ScaleLongToQuantum(value)    ((Quantum) ((value)/65537UL))
#  define ScaleMapToChar(value)        ((unsigned int) ((value)/257U))
#  define ScaleMapToQuantum(value)     ((Quantum) (value))
#  define ScaleQuantum(quantum)        ((unsigned long) ((quantum)/257UL))
#  define ScaleQuantumToChar(quantum)  ((unsigned char) ((quantum)/257U))
#  define ScaleQuantumToLong(quantum)  ((unsigned long) (65537UL*(quantum)))
#  define ScaleQuantumToMap(quantum)   ((unsigned short) (quantum))
#  define ScaleQuantumToShort(quantum) ((unsigned short) (quantum))
#  define ScaleShortToQuantum(value)   ((Quantum) (value))
#  define ScaleToQuantum(value)        ((unsigned long) (257UL*(value)))
#  define ScaleQuantumToIndex(value)   ((unsigned short) (value))
   typedef unsigned short Quantum;
#elif (QuantumDepth == 32)
#  define MaxColormapSize  65536U
#  define MaxRGB  4294967295U
#  define ScaleCharToQuantum(value)    ((Quantum) (16843009U*(value)))
#  define ScaleLongToQuantum(value)    ((Quantum) ((value)))
#  define ScaleQuantum(quantum)        ((unsigned long) ((quantum)/16843009UL))
#  define ScaleQuantumToChar(quantum)  ((unsigned char) ((quantum)/16843009U))
#  define ScaleQuantumToLong(quantum)  ((unsigned long) (quantum))
#  define ScaleQuantumToShort(quantum) ((unsigned short) ((quantum)/65537U))
#  define ScaleShortToQuantum(value)   ((Quantum) (65537U*(value)))
#  define ScaleToQuantum(value)        ((unsigned long) (16843009UL*(value)))
#  define ScaleQuantumToIndex(value)   ((unsigned short) ((value)/65537U))

/*
  MaxMap defines the maximum index value for algorithms which depend
  on lookup tables (e.g. colorspace transformations and
  normalization). When MaxMap is less than MaxRGB it is necessary to
  downscale samples to fit the range of MaxMap. The number of bits
  which are effectively preserved depends on the size of MaxMap.
  MaxMap should be a multiple of 255 and no larger than MaxRGB.  Note
  that tables can become quite large and as the tables grow larger it
  may take more time to compute the table than to process the image.
*/
#define MaxMap 65535U
#if MaxMap == 65535U
#  define ScaleCharToMap(value)        ((unsigned short) (257U*(value)))
#  define ScaleMapToChar(value)        ((unsigned int) ((value)/257U))
#  define ScaleMapToQuantum(value)     ((Quantum) (65537U*(value)))
#  define ScaleQuantumToMap(quantum)   ((unsigned short) ((quantum)/65537U))
#else
#  define ScaleCharToMap(value)        ((unsigned short) ((MaxMap/255U)*(value)))
#  define ScaleMapToChar(value)        ((unsigned int) ((value)/(MaxMap/255U)))
#  define ScaleMapToQuantum(value)     ((Quantum) ((MaxRGB/MaxMap)*(value)))
#  define ScaleQuantumToMap(quantum)   ((unsigned short) ((quantum)/(MaxRGB/MaxMap)))
#endif
typedef unsigned int Quantum;
#else
#  ifndef _CH_
#    error "Specified value of QuantumDepth is not supported"
#  endif
#endif

#define OpaqueOpacity  0UL
#define TransparentOpacity  MaxRGB
#define RoundSignedToQuantum(value) ((Quantum) (value < 0 ? 0 : \
  (value > MaxRGB) ? MaxRGB : value + 0.5))
#define RoundToQuantum(value) ((Quantum) ((double) value > (double) MaxRGB ? \
   MaxRGB : (double) value + 0.5))
#define ConstrainToRange(min,max,value) (value < min ? min : \
  (value > max) ? max : value)
#define ConstrainToQuantum(value) ConstrainToRange(0,MaxRGB,value)
#define ScaleAnyToQuantum(x,max_value) \
  ((Quantum) (((double) MaxRGB*x)/max_value+0.5))
#define MagickBoolToString(value) (value != MagickFalse ? "True" : "False")

/*
  Deprecated defines.
*/
#define RunlengthEncodedCompression RLECompression

/*
  Enum declarations.
*/
typedef enum
{
  UnspecifiedAlpha,
  AssociatedAlpha,
  UnassociatedAlpha
} AlphaType;

typedef enum
{
  UndefinedChannel,
  RedChannel,
  CyanChannel,
  GreenChannel,
  MagentaChannel,
  BlueChannel,
  YellowChannel,
  OpacityChannel,
  BlackChannel,
  MatteChannel,
  AllChannels
} ChannelType;

typedef enum
{
  UndefinedClass,
  DirectClass,
  PseudoClass
} ClassType;

typedef enum
{
  UndefinedCompliance = 0x0000,
  NoCompliance = 0x0000,
  SVGCompliance = 0x0001,
  X11Compliance = 0x0002,
  XPMCompliance = 0x0004,
  AllCompliance = 0xffff
} ComplianceType;

typedef enum
{
  UndefinedCompositeOp = 0,
  OverCompositeOp,
  InCompositeOp,
  OutCompositeOp,
  AtopCompositeOp,
  XorCompositeOp,
  PlusCompositeOp,
  MinusCompositeOp,
  AddCompositeOp,
  SubtractCompositeOp,
  DifferenceCompositeOp,
  MultiplyCompositeOp,
  BumpmapCompositeOp,
  CopyCompositeOp,
  CopyRedCompositeOp,
  CopyGreenCompositeOp,
  CopyBlueCompositeOp,
  CopyOpacityCompositeOp,
  ClearCompositeOp,
  DissolveCompositeOp,
  DisplaceCompositeOp,
  ModulateCompositeOp,
  ThresholdCompositeOp,
  NoCompositeOp,
  DarkenCompositeOp,
  LightenCompositeOp,
  HueCompositeOp,
  SaturateCompositeOp,
  ColorizeCompositeOp,
  LuminizeCompositeOp,
  ScreenCompositeOp,
  OverlayCompositeOp,
  CopyCyanCompositeOp,
  CopyMagentaCompositeOp,
  CopyYellowCompositeOp,
  CopyBlackCompositeOp
} CompositeOperator;

typedef enum
{
  UndefinedCompression,
  NoCompression,
  BZipCompression,
  FaxCompression,
  Group4Compression,
  JPEGCompression,
  LosslessJPEGCompression,
  LZWCompression,
  RLECompression,
  ZipCompression
} CompressionType;

typedef enum
{
  UndefinedDispose,
  NoneDispose,
  BackgroundDispose,
  PreviousDispose
} DisposeType;

typedef enum
{
  UndefinedEndian,
  LSBEndian,            /* "little" endian */
  MSBEndian,            /* "big" endian */
  NativeEndian          /* native endian */
} EndianType;

typedef enum
{
  UndefinedFilter,
  PointFilter,
  BoxFilter,
  TriangleFilter,
  HermiteFilter,
  HanningFilter,
  HammingFilter,
  BlackmanFilter,
  GaussianFilter,
  QuadraticFilter,
  CubicFilter,
  CatromFilter,
  MitchellFilter,
  LanczosFilter,
  BesselFilter,
  SincFilter
} FilterTypes;

typedef enum
{
#undef NoValue
  NoValue = 0x0000,
#undef XValue
  XValue = 0x0001,
#undef YValue
  YValue = 0x0002,
#undef WidthValue
  WidthValue = 0x0004,
#undef HeightValue
  HeightValue = 0x0008,
#undef AllValues
  AllValues = 0x000F,
#undef XNegative
  XNegative = 0x0010,
#undef YNegative
  YNegative = 0x0020,
  PercentValue = 0x1000,
  AspectValue = 0x2000,
  LessValue = 0x4000,
  GreaterValue = 0x8000,
  AreaValue = 0x10000
} GeometryFlags;

typedef enum
{
#undef ForgetGravity
  ForgetGravity,
#undef NorthWestGravity
  NorthWestGravity,
#undef NorthGravity
  NorthGravity,
#undef NorthEastGravity
  NorthEastGravity,
#undef WestGravity
  WestGravity,
#undef CenterGravity
  CenterGravity,
#undef EastGravity
  EastGravity,
#undef SouthWestGravity
  SouthWestGravity,
#undef SouthGravity
  SouthGravity,
#undef SouthEastGravity
  SouthEastGravity,
#undef StaticGravity
  StaticGravity
} GravityType;

typedef enum
{
  UndefinedType,
  BilevelType,
  GrayscaleType,
  GrayscaleMatteType,
  PaletteType,
  PaletteMatteType,
  TrueColorType,
  TrueColorMatteType,
  ColorSeparationType,
  ColorSeparationMatteType,
  OptimizeType
} ImageType;

typedef enum
{
  UndefinedInterlace,
  NoInterlace,
  LineInterlace,
  PlaneInterlace,
  PartitionInterlace
} InterlaceType;

typedef enum
{
  UndefinedMode,
  FrameMode,
  UnframeMode,
  ConcatenateMode
} MontageMode;

typedef enum
{
  UniformNoise,
  GaussianNoise,
  MultiplicativeGaussianNoise,
  ImpulseNoise,
  LaplacianNoise,
  PoissonNoise
} NoiseType;

/*
  Image orientation.  Based on TIFF standard values.
*/
typedef enum               /* Line direction / Frame Direction */
{                          /* -------------- / --------------- */
  UndefinedOrientation,    /* Unknown        / Unknown         */
  TopLeftOrientation,      /* Left to right  / Top to bottom   */
  TopRightOrientation,     /* Right to left  / Top to bottom   */
  BottomRightOrientation,  /* Right to left  / Bottom to top   */
  BottomLeftOrientation,   /* Left to right  / Bottom to top   */
  LeftTopOrientation,      /* Top to bottom  / Left to right   */
  RightTopOrientation,     /* Top to bottom  / Right to left   */
  RightBottomOrientation,  /* Bottom to top  / Right to left   */
  LeftBottomOrientation    /* Bottom to top  / Left to right   */
} OrientationType;

typedef enum
{
  UndefinedPreview = 0,
  RotatePreview,
  ShearPreview,
  RollPreview,
  HuePreview,
  SaturationPreview,
  BrightnessPreview,
  GammaPreview,
  SpiffPreview,
  DullPreview,
  GrayscalePreview,
  QuantizePreview,
  DespecklePreview,
  ReduceNoisePreview,
  AddNoisePreview,
  SharpenPreview,
  BlurPreview,
  ThresholdPreview,
  EdgeDetectPreview,
  SpreadPreview,
  SolarizePreview,
  ShadePreview,
  RaisePreview,
  SegmentPreview,
  SwirlPreview,
  ImplodePreview,
  WavePreview,
  OilPaintPreview,
  CharcoalDrawingPreview,
  JPEGPreview
} PreviewType;

typedef enum
{
  UndefinedIntent,
  SaturationIntent,
  PerceptualIntent,
  AbsoluteIntent,
  RelativeIntent
} RenderingIntent;

typedef enum
{
  UndefinedResolution,
  PixelsPerInchResolution,
  PixelsPerCentimeterResolution
} ResolutionType;

/*
  Typedef declarations.
*/
typedef struct _AffineMatrix
{
  double
    sx,
    rx,
    ry,
    sy,
    tx,
    ty;
} AffineMatrix;

typedef struct _PrimaryInfo
{
  double
    x,
    y,
    z;
} PrimaryInfo;

typedef struct _ChromaticityInfo
{
  PrimaryInfo
    red_primary,
    green_primary,
    blue_primary,
    white_point;
} ChromaticityInfo;

#if defined(MAGICK_IMPLEMENTATION)
/*
  Useful macros for accessing PixelPacket members in a generic way.
*/
# define GetRedSample(p) (p->red)
# define GetGreenSample(p) (p->green)
# define GetBlueSample(p) (p->blue)
# define GetOpacitySample(p) (p->opacity)

# define SetRedSample(q,value) (q->red=(value))
# define SetGreenSample(q,value) (q->green=(value))
# define SetBlueSample(q,value) (q->blue=(value))
# define SetOpacitySample(q,value) (q->opacity=(value))

# define GetGraySample(p) (p->red)
# define SetGraySample(q,value) (q->red=q->green=q->blue=(value))

# define GetYSample(p) (p->red)
# define GetCbSample(p) (p->green)
# define GetCrSample(p) (p->blue)

# define SetYSample(q,value) (q->red=(value))
# define SetCbSample(q,value) (q->green=(value))
# define SetCrSample(q,value) (q->blue=(value))

# define GetCyanSample(p) (p->red)
# define GetMagentSample(p) (p->green)
# define GetYellowSample(p) (p->blue)
# define GetBlackSample(p) (p->opacity)

# define SetCyanSample(q,value) (q->red=(value))
# define SetMagentaSample(q,value) (q->green=(value))
# define SetYellowSample(q,value) (q->blue=(value))
# define SetBlackSample(q,value) (q->opacity=(value))

#endif /* defined(MAGICK_IMPLEMENTATION) */

typedef struct _PixelPacket
{
#if defined(WORDS_BIGENDIAN)
  /* RGBA */
#define MAGICK_PIXELS_RGBA 1
  Quantum
    red,
    green,
    blue,
    opacity;
#else
  /* BGRA (as used by Microsoft Windows DIB) */
#define MAGICK_PIXELS_BGRA 1
  Quantum
    blue,
    green,
    red,
    opacity;
#endif
} PixelPacket;

typedef struct _ColorInfo
{
  char
    *path,
    *name;

  ComplianceType
    compliance;

  PixelPacket
    color;

  unsigned int
    stealth;

  unsigned long
    signature;

  struct _ColorInfo
    *previous,
    *next;
} ColorInfo;

typedef struct _DoublePixelPacket
{
  double
    red,
    green,
    blue,
    opacity;
} DoublePixelPacket;

/*
  ErrorInfo is used to record statistical difference (error)
  information based on computed Euclidean distance in RGB space.
*/
typedef struct _ErrorInfo
{
  double
    mean_error_per_pixel,     /* Average error per pixel (absolute range) */
    normalized_mean_error,    /* Average error per pixel (normalized to 1.0) */
    normalized_maximum_error; /* Maximum error encountered (normalized to 1.0) */
} ErrorInfo;

typedef struct _FrameInfo
{
  unsigned long
    width,
    height;

  long
    x,
    y,
    inner_bevel,
    outer_bevel;
} FrameInfo;

typedef Quantum IndexPacket;

typedef struct _LongPixelPacket
{
  unsigned long
    red,
    green,
    blue,
    opacity;
} LongPixelPacket;

typedef struct _MontageInfo
{
  char
    *geometry,
    *tile,
    *title,
    *frame,
    *texture,
    *font;

  double
    pointsize;

  unsigned long
    border_width;

  unsigned int
    shadow;

  PixelPacket
    fill,
    stroke,
    background_color,
    border_color,
    matte_color;

  GravityType
    gravity;

  char
    filename[MaxTextExtent];

  unsigned long
    signature;
} MontageInfo;

typedef struct _ProfileInfo
{
  size_t
    length;

  char
    *name;

  unsigned char
    *info;
} ProfileInfo;

typedef struct _RectangleInfo
{
  unsigned long
    width,
    height;

  long
    x,
    y;
} RectangleInfo;

typedef struct _SegmentInfo
{
  double
    x1,
    y1,
    x2,
    y2;
} SegmentInfo;

typedef struct _ImageChannelStatistics
 {
   /* Minimum value observed */
   double maximum;
   /* Maximum value observed */
   double minimum;
   /* Average (mean) value observed */
   double mean;
   /* Standard deviation, sqrt(variance) */
   double standard_deviation;
   /* Variance */
   double variance;
 } ImageChannelStatistics;

typedef struct _ImageStatistics
 {
   ImageChannelStatistics red;
   ImageChannelStatistics green;
   ImageChannelStatistics blue;
   ImageChannelStatistics opacity;
 } ImageStatistics;

typedef struct _ImageCharacteristics
{
  MagickBool
    cmyk,               /* CMYK(A) image */
    grayscale,          /* Grayscale image */
    monochrome,         /* Black/white image */
    opaque,             /* Opaque image */
    palette;            /* Colormapped image */
} ImageCharacteristics;

typedef struct _Image
{
  ClassType
    storage_class;      /* DirectClass (TrueColor) or PseudoClass (colormapped) */

  ColorspaceType
    colorspace;         /* Current image colorspace/model */

  CompressionType
    compression;        /* Compression algorithm to use when encoding image */

  MagickBool
    dither,             /* True if image is to be dithered */
    matte;              /* True if image has an opacity (alpha) channel */ 

  unsigned long
    columns,            /* Number of image columns */
    rows;               /* Number of image rows */

  unsigned int
    colors,             /* Current number of colors in PseudoClass colormap */
    depth;              /* Bits of precision to preserve in color quantum */

  PixelPacket
    *colormap;          /* Pseudoclass colormap array */

  PixelPacket
    background_color,   /* Background color */
    border_color,       /* Border color */
    matte_color;        /* Matte (transparent) color */

  double
    gamma;              /* Image gamma (e.g. 0.45) */

  ChromaticityInfo
    chromaticity;       /* Red, green, blue, and white chromaticity values */

  OrientationType
    orientation;        /* Image orientation */

  RenderingIntent
    rendering_intent;   /* Rendering intent */

  ResolutionType
    units;              /* Units of image resolution (density) */

  char
    *montage,           /* Tile size and offset within an image montage */
    *directory,         /* Tile names from within an image montage */
    *geometry;          /* Composite/Crop options */

  long
    offset;             /* Offset to start of image data */

  double
    x_resolution,       /* Horizontal resolution (also see units) */
    y_resolution;       /* Vertical resolution (also see units) */

  RectangleInfo
    page,               /* Offset to apply when placing image */
    tile_info;          /* Subregion tile dimensions and offset */

  double
    blur,               /* Amount of blur to apply when zooming image */
    fuzz;               /* Colors within this distance match target color */

  FilterTypes
    filter;             /* Filter to use when zooming image */

  InterlaceType
    interlace;          /* Interlace pattern to use when writing image */

  EndianType
    endian;             /* Byte order to use when writing image */

  GravityType
    gravity;            /* Image placement gravity */

  CompositeOperator
    compose;            /* Image placement composition */

  DisposeType
    dispose;            /* GIF disposal option */

  unsigned long
    scene,              /* Animation frame scene number */
    delay,              /* Animation frame scene delay */
    iterations,         /* Animation iterations */
    total_colors;       /* Number of unique colors. See GetNumberColors() */

  long
    start_loop;         /* Animation frame number to start looping at */

  ErrorInfo
    error;              /* Computed image comparison or quantization error */

  TimerInfo
    timer;              /* Operation micro-timer */

  void
    *client_data;       /* User specified opaque data pointer */

  /*
    Output file name.

    A colon delimited format identifier may be prepended to the file
    name in order to force a particular output format. Otherwise the
    file extension is used. If no format prefix or file extension is
    present, then the output format is determined by the 'magick'
    field.
  */
  char
    filename[MaxTextExtent];

  /*
    Original file name (name of input image file)
  */
  char
    magick_filename[MaxTextExtent];

  /*
    File format of the input file, and the default output format.

    The precedence when selecting the output format is:
      1) magick prefix to file name (e.g. "jpeg:foo).
      2) file name extension. (e.g. "foo.jpg")
      3) content of this magick field.

  */
  char
    magick[MaxTextExtent];

  /*
    Original image width (before transformations)
  */
  unsigned long
    magick_columns;

  /*
    Original image height (before transformations)
  */
  unsigned long
    magick_rows;

  ExceptionInfo
    exception;          /* Any error associated with this image frame */

  struct _Image
    *previous,          /* Pointer to previous frame */
    *next;              /* Pointer to next frame */

  /*
    Only private members appear past this point
  */

  void                  /* Private, Embedded profiles */
    *profiles;

  unsigned int
    is_monochrome,      /* Private, True if image is known to be monochrome */
    is_grayscale,       /* Private, True if image is known to be grayscale */
    taint;              /* Private, True if image has not been modifed */

  struct _Image
    *clip_mask;         /* Private, Clipping mask to apply when updating pixels */

  _CacheInfoPtr_
    cache;              /* Private, image pixel cache */

  _ImageAttributePtr_
    attributes;         /* Private, Image attribute list */

  _Ascii85InfoPtr_
    ascii85;            /* Private, supports huffman encoding */

  _BlobInfoPtr_
    blob;               /* Private, file I/O object */

  long
    reference_count;    /* Private, Image reference count */

  _SemaphoreInfoPtr_
    semaphore;          /* Private, Per image lock (for reference count) */

  unsigned int
    logging;            /* Private, True if logging is enabled */

  struct _Image
    *list;              /* Private, used only by display */

  unsigned long
    signature;          /* Private, Unique code to validate structure */
} Image;

/*
  Callback used by ReadStream() and WriteStream().
*/
typedef unsigned int
  (*StreamHandler)(const Image *,const void *,const size_t);

typedef struct _ImageInfo
{
  CompressionType
    compression;             /* Image compression to use while decoding */

  MagickBool
    temporary,               /* Remove file "filename" once it has been read. */
    adjoin,                  /* If True, join multiple frames into one file */
    antialias;               /* If True, antialias while rendering */

  unsigned long
    subimage,                /* Starting image scene ID to select */
    subrange,                /* Span of image scene IDs (from starting scene) to select */
    depth;                   /* Number of quantum bits to preserve while encoding */

  char
    *size,                   /* Desired/known dimensions to use when decoding image */
    *tile,                   /* Deprecated, name of image to tile on background */
    *page;                   /* Output page size & offset */

  InterlaceType
    interlace;               /* Interlace scheme to use when decoding image */

  EndianType
    endian;                  /* Select MSB/LSB endian output for TIFF format */

  ResolutionType
    units;                   /* Units to apply when evaluating the density option */

  unsigned long
    quality;                 /* Compression quality factor (format specific) */

  char
    *sampling_factor,        /* JPEG, MPEG, and YUV chroma downsample factor */
    *server_name,            /* X11 server display specification */
    *font,                   /* Font name to use for text annotations */
    *texture,                /* Name of texture image to use for background fills */
    *density;                /* Image resolution (also see units) */

  double
    pointsize;               /* Font pointsize */

  double
    fuzz;                    /* Colors within this distance are a match */

  PixelPacket
    pen,                     /* Stroke or fill color while drawing */
    background_color,        /* Background color */
    border_color,            /* Border color (color surrounding frame) */
    matte_color;             /* Matte color (frame color) */

  MagickBool
    dither,                  /* If true, dither image while writing */
    monochrome,              /* If true, use monochrome format */
    progress;                /* If true, show progress indication */

  ColorspaceType
    colorspace;              /* Colorspace representations of image pixels */

  ImageType
    type;                    /* Desired image type (used while reading or writing) */

  long
    group;                   /* X11 window group ID */

  unsigned int
    verbose;                 /* If non-zero, display high-level processing */

  char
    *view,                   /* FlashPIX view specification */
    *authenticate;           /* Password used to decrypt file */

  void
    *client_data;            /* User-specified data to pass to coder */

  StreamHandler
    stream;                  /* Pass in open blob stream handler for read/write */

  FILE
    *file;                   /* If not null, stdio FILE to read image from */

  char
    magick[MaxTextExtent],   /* File format to read. Overrides file extension */
    filename[MaxTextExtent]; /* File name to read */

  /*
    Only private members appear past this point
  */

  _CacheInfoPtr_
     cache;                  /* Private. Used to pass image via open cache */

  void
    *definitions;            /* Private. Map of coder specific options passed by user.
                                Use AddDefinitions, RemoveDefinitions, & AccessDefinition
                                to access and manipulate this data. */

  Image
    *attributes;             /* Private. Image attribute list */

  MagickBool
    ping;                    /* Private, if true, read file header only */

  PreviewType
    preview_type;            /* Private, used by PreviewImage */

  MagickBool
    affirm;                  /* Private, when true do not intuit image format */

  _BlobInfoPtr_
    blob;                    /* Private, used to pass in open blob */

  size_t
    length;                  /* Private, used to pass in open blob length */

  char
    unique[MaxTextExtent],   /* Private, passes temporary filename to TranslateText */
    zero[MaxTextExtent];     /* Private, passes temporary filename to TranslateText */

  unsigned long
    signature;               /* Private, used to validate structure */
} ImageInfo;

/*
  Image utilities methods.
*/

extern MagickExport ExceptionType
  CatchImageException(Image *);

extern MagickExport Image
  *AllocateImage(const ImageInfo *),
  *AppendImages(const Image *,const unsigned int,ExceptionInfo *),
  *AverageImages(const Image *,ExceptionInfo *),
  *CloneImage(const Image *,const unsigned long,const unsigned long,
   const unsigned int,ExceptionInfo *),
  *GetImageClipMask(const Image *,ExceptionInfo *),
  *ReferenceImage(Image *);

extern MagickExport ImageInfo
  *CloneImageInfo(const ImageInfo *);

extern MagickExport ImageType
  GetImageType(const Image *,ExceptionInfo *);

extern MagickExport const char
  *AccessDefinition(const ImageInfo *image_info,const char *magick,
     const char *key),
  *ClassTypeToString(const ClassType class_type),
  *ImageTypeToString(const ImageType image_type);

extern MagickExport int
  GetImageGeometry(const Image *,const char *,const unsigned int,
  RectangleInfo *);

extern MagickExport RectangleInfo
  GetImageBoundingBox(const Image *,ExceptionInfo *exception);

/* Functions which return unsigned int as a True/False boolean value */
extern MagickExport MagickBool
  IsImagesEqual(Image *,const Image *),
  IsTaintImage(const Image *),
  IsSubimage(const char *,const unsigned int);

/* Functions which return unsigned int to indicate operation pass/fail */
extern MagickExport MagickPassFail
  AddDefinitions(ImageInfo *image_info,const char *options,
    ExceptionInfo *exception),
  AllocateImageColormap(Image *,const unsigned long),
  AnimateImages(const ImageInfo *image_info,Image *image),
  ClipImage(Image *),
  ClipPathImage(Image *,const char *,const unsigned int),
  CycleColormapImage(Image *image,const int amount),
  DescribeImage(Image *image,FILE *file,const unsigned int verbose),
  DisplayImages(const ImageInfo *image_info,Image *image),
  GetImageCharacteristics(const Image *image,ImageCharacteristics *characteristics,
    const MagickBool optimize,ExceptionInfo *exception),
  GetImageStatistics(const Image *image,ImageStatistics *statistics,
    ExceptionInfo *exception),
  GradientImage(Image *,const PixelPacket *,const PixelPacket *),
  PlasmaImage(Image *,const SegmentInfo *,unsigned long,unsigned long),
  RemoveDefinitions(const ImageInfo *image_info,const char *options),
  ReplaceImageColormap(Image *image,const PixelPacket *colormap,
    const unsigned int colors),
  SetImageClipMask(Image *image,const Image *clip_mask),
  SetImageDepth(Image *,const unsigned long),
  SetImageInfo(ImageInfo *,const unsigned int,ExceptionInfo *),
  SetImageType(Image *,const ImageType),
  SortColormapByIntensity(Image *),
  SyncImage(Image *),
  TextureImage(Image *,const Image *);

extern MagickExport unsigned long
  GetImageDepth(const Image *,ExceptionInfo *);

extern MagickExport void
  AllocateNextImage(const ImageInfo *,Image *),
  DestroyImage(Image *),
  DestroyImageInfo(ImageInfo *),
  GetImageException(Image *,ExceptionInfo *),
  GetImageInfo(ImageInfo *),
  ModifyImage(Image **,ExceptionInfo *),
  SetImage(Image *,const Quantum),
  SetImageOpacity(Image *,const unsigned int);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* _MAGICK_IMAGE_H */
