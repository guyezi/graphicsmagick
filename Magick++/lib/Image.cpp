// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2001, 2002
//
// Implementation of Image
//

#define MAGICK_IMPLEMENTATION

#include <string>
#include <string.h>
#include <errno.h>
#include <math.h>

using namespace std;

#include "Magick++/Image.h"
#include "Magick++/Functions.h"
#include "Magick++/Pixels.h"
#include "Magick++/Options.h"
#include "Magick++/ImageRef.h"

#define AbsoluteValue(x)  ((x) < 0 ? -(x) : (x))

extern MagickDLLDecl const std::string Magick::borderGeometryDefault = "6x6+0+0";
extern MagickDLLDecl const std::string Magick::frameGeometryDefault  = "25x25+6+6";
extern MagickDLLDecl const std::string Magick::raiseGeometryDefault  = "6x6+0+0";

//
// Explicit template instantiations
//

//
// Friend functions to compare Image objects
//

MagickDLLDecl int Magick::operator == ( const Magick::Image& left_,
                                        const Magick::Image& right_ )
{
  // If image pixels and signature are the same, then the image is identical
  return ( ( left_.rows() == right_.rows() ) &&
	   ( left_.columns() == right_.columns() ) &&
	   ( left_.signature() == right_.signature() )
	   );
}
MagickDLLDecl int Magick::operator != ( const Magick::Image& left_,
                                        const Magick::Image& right_ )
{
  return ( ! (left_ == right_) );
}
MagickDLLDecl int Magick::operator >  ( const Magick::Image& left_,
                                        const Magick::Image& right_ )
{
  return ( !( left_ < right_ ) && ( left_ != right_ ) );
}
MagickDLLDecl int Magick::operator <  ( const Magick::Image& left_,
                                        const Magick::Image& right_ )
{
  // If image pixels are less, then image is smaller
  return ( ( left_.rows() * left_.columns() ) <
	   ( right_.rows() * right_.columns() )
	   );
}
MagickDLLDecl int Magick::operator >= ( const Magick::Image& left_,
                                        const Magick::Image& right_ )
{
  return ( ( left_ > right_ ) || ( left_ == right_ ) );
}
MagickDLLDecl int Magick::operator <= ( const Magick::Image& left_,
                                        const Magick::Image& right_ )
{
  return ( ( left_ < right_ ) || ( left_ == right_ ) );
}

//
// Image object implementation
//

// Construct from image file or image specification
Magick::Image::Image( const std::string &imageSpec_ )
  : _imgRef(new ImageRef)
{
  try
    {
      // Initialize, Allocate and Read images
      read( imageSpec_ );
    }
  catch ( const Warning & /*warning_*/ )
    {
      // FIXME: need a way to report warnings in constructor
    }
}

// Construct a blank image canvas of specified size and color
Magick::Image::Image( const Geometry &size_,
		      const Color &color_ )
  : _imgRef(new ImageRef)
{
  // xc: prefix specifies an X11 color string
  std::string imageSpec("xc:");
  imageSpec += color_;

  try
    {
      // Set image size
      size( size_ );

      // Initialize, Allocate and Read images
      read( imageSpec );
    }
  catch ( const Warning & /*warning_*/ )
    {
      // FIXME: need a way to report warnings in constructor
    }
}

// Construct Image from in-memory BLOB
Magick::Image::Image ( const Blob &blob_ )
  : _imgRef(new ImageRef)
{
  try
    {
      // Initialize, Allocate and Read images
      read( blob_ );
    }
  catch ( const Warning & /*warning_*/ )
    {
      // FIXME: need a way to report warnings in constructor
    }
}

// Construct Image of specified size from in-memory BLOB
Magick::Image::Image ( const Blob &blob_,
		       const Geometry &size_ )
  : _imgRef(new ImageRef)
{
  try
    {
      // Read from Blob
      read( blob_, size_ );
    }
  catch ( const Warning & /*warning_*/ )
    {
      // FIXME: need a way to report warnings in constructor
    }
}

// Construct Image of specified size and depth from in-memory BLOB
Magick::Image::Image ( const Blob &blob_,
		       const Geometry &size_,
		       const unsigned int depth_ )
  : _imgRef(new ImageRef)
{
  try
    {
      // Read from Blob
      read( blob_, size_, depth_ );
    }
  catch ( const Warning & /*warning_*/ )
    {
      // FIXME: need a way to report warnings in constructor
    }
}

// Construct Image of specified size, depth, and format from in-memory BLOB
Magick::Image::Image ( const Blob &blob_,
		       const Geometry &size_,
		       const unsigned int depth_,
		       const std::string &magick_ )
  : _imgRef(new ImageRef)
{
  try
    {
      // Read from Blob
      read( blob_, size_, depth_, magick_ );
    }
  catch ( const Warning & /*warning_*/ )
    {
      // FIXME: need a way to report warnings in constructor
    }
}

// Construct Image of specified size, and format from in-memory BLOB
Magick::Image::Image ( const Blob &blob_,
		       const Geometry &size_,
		       const std::string &magick_ )
  : _imgRef(new ImageRef)
{
  try
    {
      // Read from Blob
      read( blob_, size_, magick_ );
    }
  catch ( const Warning & /*warning_*/ )
    {
      // FIXME: need a way to report warnings in constructor
    }
}

// Construct an image based on an array of raw pixels, of specified
// type and mapping, in memory
Magick::Image::Image ( const unsigned int width_,
                       const unsigned int height_,
                       const std::string &map_,
                       const StorageType type_,
                       const void *pixels_ )
  : _imgRef(new ImageRef)
{
  try
    {
      read( width_, height_, map_.c_str(), type_, pixels_ );
    }
  catch ( const Warning & /*warning_*/ )
    {
      // FIXME: need a way to report warnings in constructor
    }
}

// Default constructor
Magick::Image::Image( void )
  : _imgRef(new ImageRef)
{
}

// Destructor
/* virtual */
Magick::Image::~Image()
{
  bool doDelete = false;
  {
    Lock( &_imgRef->_mutexLock );
    if ( --_imgRef->_refCount == 0 )
      doDelete = true;
  }

  if ( doDelete )
    {
      delete _imgRef;
    }
  _imgRef = 0;
}

// Add noise to image
void Magick::Image::addNoise( const NoiseType noiseType_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    AddNoiseImage ( image(),
		    noiseType_,
		    &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Annotate using specified text, and placement location
void Magick::Image::annotate ( const std::string &text_,
			       const Geometry &location_ )
{
  annotate ( text_, location_,  NorthWestGravity, 0.0 );
}
// Annotate using specified text, bounding area, and placement gravity
void Magick::Image::annotate ( const std::string &text_,
			       const Geometry &boundingArea_,
			       const GravityType gravity_ )
{
  annotate ( text_, boundingArea_, gravity_, 0.0 );
}
// Annotate with text using specified text, bounding area, placement
// gravity, and rotation.
void Magick::Image::annotate ( const std::string &text_,
			       const Geometry &boundingArea_,
			       const GravityType gravity_,
			       const double degrees_ )
{
  modifyImage();

  DrawInfo *drawInfo
    = options()->drawInfo();
  
  drawInfo->text = const_cast<char *>(text_.c_str());

  char boundingArea[MaxTextExtent];

  drawInfo->geometry = 0;
  if ( boundingArea_.isValid() ){
    if ( boundingArea_.width() == 0 || boundingArea_.height() == 0 )
      {
        FormatString( boundingArea, "+%u+%u",
                      boundingArea_.xOff(), boundingArea_.yOff() );
      }
    else
      {
        strcpy( boundingArea, string(boundingArea_).c_str());
      }
    drawInfo->geometry = boundingArea;
  }

  drawInfo->gravity = gravity_;

  AffineMatrix oaffine = drawInfo->affine;
  if ( degrees_ != 0.0)
    {
        AffineMatrix affine;
        affine.sx=1.0;
        affine.rx=0.0;
        affine.ry=0.0;
        affine.sy=1.0;
        affine.tx=0.0;
        affine.ty=0.0;

        AffineMatrix current = drawInfo->affine;
#define DegreesToRadians(x) ((x)*3.14159265358979323846/180.0)
        affine.sx=cos(DegreesToRadians(fmod(degrees_,360.0)));
        affine.rx=sin(DegreesToRadians(fmod(degrees_,360.0)));
        affine.ry=(-sin(DegreesToRadians(fmod(degrees_,360.0))));
        affine.sy=cos(DegreesToRadians(fmod(degrees_,360.0)));

        drawInfo->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
        drawInfo->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
        drawInfo->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
        drawInfo->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
        drawInfo->affine.tx=current.sx*affine.tx+current.ry*affine.ty
          +current.tx;
    }

  AnnotateImage( image(), drawInfo );

  // Restore original values
  drawInfo->affine = oaffine;
  drawInfo->text = 0;
  drawInfo->geometry = 0;

  throwImageException();
}
// Annotate with text (bounding area is entire image) and placement gravity.
void Magick::Image::annotate ( const std::string &text_,
			       const GravityType gravity_ )
{
  modifyImage();

  DrawInfo *drawInfo
    = options()->drawInfo();

  drawInfo->text = const_cast<char *>(text_.c_str());

  drawInfo->gravity = gravity_;

  AnnotateImage( image(), drawInfo );

  drawInfo->gravity = NorthWestGravity;
  drawInfo->text = 0;

  throwImageException();
}

// Blur image
void Magick::Image::blur( const double radius_, const double sigma_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    BlurImage( image(), radius_, sigma_, &exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Add border to image
// Only uses width & height
void Magick::Image::border( const Geometry &geometry_ )
{
  RectangleInfo borderInfo = geometry_;
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    BorderImage( image(), &borderInfo, &exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Extract channel from image
void Magick::Image::channel ( const ChannelType channel_ )
{
  modifyImage();
  ChannelImage ( image(), channel_ );
  throwImageException();
}

// Charcoal-effect image
void Magick::Image::charcoal( const double radius_, const double sigma_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    CharcoalImage( image(), radius_, sigma_, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Chop image
void Magick::Image::chop( const Geometry &geometry_ )
{
  RectangleInfo chopInfo = geometry_;
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    ChopImage( image(), &chopInfo, &exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Colorize
void Magick::Image::colorize ( const unsigned int opacityRed_,
                               const unsigned int opacityGreen_,
                               const unsigned int opacityBlue_,
			       const Color &penColor_ )
{
  if ( !penColor_.isValid() )
  {
    throwExceptionExplicit( OptionError,
			    "Pen color argument is invalid");
  }

  char opacity[MaxTextExtent];
  FormatString(opacity,"%u/%u/%u",opacityRed_,opacityGreen_,opacityBlue_);

  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
  ColorizeImage ( image(), opacity,
		  penColor_, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}
void Magick::Image::colorize ( const unsigned int opacity_,
			       const Color &penColor_ )
{
  colorize( opacity_, opacity_, opacity_, penColor_ );
}

// Composite two images
void Magick::Image::composite ( const Image &compositeImage_,
				const int xOffset_,
				const int yOffset_,
				const CompositeOperator compose_ )
{
  // Image supplied as compositeImage is composited with current image and
  // results in updating current image.
  modifyImage();

  CompositeImage( image(),
		  compose_,
		  compositeImage_.constImage(),
		  xOffset_,
                  yOffset_ );
  throwImageException();
}
void Magick::Image::composite ( const Image &compositeImage_,
				const Geometry &offset_,
				const CompositeOperator compose_ )
{
  modifyImage();

  long x = offset_.xOff();
  long y = offset_.yOff();
  unsigned long width = columns();
  unsigned long height = rows();

  GetMagickGeometry (static_cast<std::string>(offset_).c_str(),
		      &x, &y,
		      &width, &height );

  CompositeImage( image(),
		  compose_,
		  compositeImage_.constImage(),
		  x, y );
  throwImageException();
}
void Magick::Image::composite ( const Image &compositeImage_,
				const GravityType gravity_,
				const CompositeOperator compose_ )
{
  modifyImage();

  long x = 0;
  long y = 0;

  switch (gravity_)
    {
    case NorthWestGravity:
      {
	x = 0;
	y = 0;
	break;
      }
    case NorthGravity:
      {
	x = (columns() - compositeImage_.columns()) >> 1;
	y = 0;
	break;
      }
    case NorthEastGravity:
      {
	x = static_cast<long>(columns() - compositeImage_.columns());
	y = 0;
	break;
      }
    case WestGravity:
      {
	x = 0;
	y = (rows() - compositeImage_.rows()) >> 1;
	break;
      }
    case ForgetGravity:
    case StaticGravity:
    case CenterGravity:
    default:
      {
	x = (columns() - compositeImage_.columns()) >> 1;
	y = (rows() - compositeImage_.rows()) >> 1;
	break;
      }
    case EastGravity:
      {
	x = static_cast<long>(columns() - compositeImage_.columns());
	y = (rows() - compositeImage_.rows()) >> 1;
	break;
      }
    case SouthWestGravity:
      {
	x = 0;
	y = static_cast<long>(rows() - compositeImage_.rows());
	break;
      }
    case SouthGravity:
      {
	x =  (columns() - compositeImage_.columns()) >> 1;
	y = static_cast<long>(rows() - compositeImage_.rows());
	break;
      }
    case SouthEastGravity:
      {
	x = static_cast<long>(columns() - compositeImage_.columns());
	y = static_cast<long>(rows() - compositeImage_.rows());
	break;
      }
    }

  CompositeImage( image(),
		  compose_,
		  compositeImage_.constImage(),
		  x, y );
  throwImageException();
}

// Contrast image
void Magick::Image::contrast ( const unsigned int sharpen_ )
{
  modifyImage();
  ContrastImage ( image(), sharpen_ );
  throwImageException();
}

// Convolve image.  Applies a general image convolution kernel to the image.
//  order_ represents the number of columns and rows in the filter kernel.
//  kernel_ is an array of doubles representing the convolution kernel.
void Magick::Image::convolve ( const unsigned int order_,
                               const double *kernel_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
  ConvolveImage ( image(), order_,
		  kernel_, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Crop image
void Magick::Image::crop ( const Geometry &geometry_ )
{
  RectangleInfo cropInfo = geometry_;
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    CropImage( image(),
	       &cropInfo,
	       &exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Cycle Color Map
void Magick::Image::cycleColormap ( const int amount_ )
{
  modifyImage();
  CycleColormapImage( image(), amount_ );
  throwImageException();
}

// Despeckle
void Magick::Image::despeckle ( void )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    DespeckleImage( image(), &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Display image
void Magick::Image::display( void )
{
  DisplayImages( imageInfo(), image() );
}

// Draw on image using single drawable
void Magick::Image::draw ( const Magick::Drawable &drawable_ )
{
  modifyImage();

  DrawContext context = DrawAllocateContext( options()->drawInfo(), image());

  if(context)
    {
      drawable_.operator()(context);

      if( constImage()->exception.severity == UndefinedException)
        DrawRender(context);

      DrawDestroyContext(context);
    }

  throwImageException();
}

// Draw on image using a drawable list
void Magick::Image::draw ( const std::list<Magick::Drawable> &drawable_ )
{
  modifyImage();

  DrawContext context = DrawAllocateContext( options()->drawInfo(), image());

  if(context)
    {
      for( std::list<Magick::Drawable>::const_iterator p = drawable_.begin();
           p != drawable_.end(); p++ )
        {
          p->operator()(context);
          if( constImage()->exception.severity != UndefinedException)
            break;
        }

      if( constImage()->exception.severity == UndefinedException)
        DrawRender(context);

      DrawDestroyContext(context);
    }

  throwImageException();
}

// Hilight edges in image
void Magick::Image::edge ( const double radius_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    EdgeImage( image(), radius_, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Emboss image (hilight edges)
void Magick::Image::emboss ( const double radius_, const double sigma_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    EmbossImage( image(), radius_, sigma_, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Enhance image (minimize noise)
void Magick::Image::enhance ( void )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    EnhanceImage( image(), &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Equalize image (histogram equalization)
void Magick::Image::equalize ( void )
{
  modifyImage();
  EqualizeImage( image() );
  throwImageException();
}

// Erase image to current "background color"
void Magick::Image::erase ( void )
{
  modifyImage();
  SetImage( image(), OpaqueOpacity );
  throwImageException();
}

// Flip image (reflect each scanline in the vertical direction)
void Magick::Image::flip ( void )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    FlipImage( image(), &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Flood-fill color across pixels that match the color of the
// target pixel and are neighbors of the target pixel.
// Uses current fuzz setting when determining color match.
void Magick::Image::floodFillColor( const unsigned int x_,
                                    const unsigned int y_,
				    const Magick::Color &fillColor_ )
{
  floodFillTexture( x_, y_, Image( Geometry( 1, 1), fillColor_ ) );
}
void Magick::Image::floodFillColor( const Geometry &point_,
				    const Magick::Color &fillColor_ )
{
  floodFillTexture( point_, Image( Geometry( 1, 1), fillColor_) );
}

// Flood-fill color across pixels starting at target-pixel and
// stopping at pixels matching specified border color.
// Uses current fuzz setting when determining color match.
void Magick::Image::floodFillColor( const unsigned int x_,
                                    const unsigned int y_,
				    const Magick::Color &fillColor_,
				    const Magick::Color &borderColor_ )
{
  floodFillTexture( x_, y_, Image( Geometry( 1, 1), fillColor_),
                    borderColor_ );
}
void Magick::Image::floodFillColor( const Geometry &point_,
				    const Magick::Color &fillColor_,
				    const Magick::Color &borderColor_ )
{
  floodFillTexture( point_, Image( Geometry( 1, 1), fillColor_),
                    borderColor_ );
}

// Floodfill pixels matching color (within fuzz factor) of target
// pixel(x,y) with replacement opacity value using method.
void Magick::Image::floodFillOpacity( const unsigned int x_,
                                      const unsigned int y_,
                                      const unsigned int opacity_,
                                      const PaintMethod method_ )
{
  modifyImage();
  MatteFloodfillImage ( image(),
                        static_cast<PixelPacket>(pixelColor(x_,y_)),
                        opacity_,
			static_cast<long>(x_), static_cast<long>(y_),
                        method_ );
  throwImageException();
}

// Flood-fill texture across pixels that match the color of the
// target pixel and are neighbors of the target pixel.
// Uses current fuzz setting when determining color match.
void Magick::Image::floodFillTexture( const unsigned int x_,
                                      const unsigned int y_,
				      const Magick::Image &texture_ )
{
  modifyImage();

  // Set drawing pattern
  options()->fillPattern(texture_.constImage());

  // Get pixel view
  Pixels pixels(*this);
  // Fill image
  PixelPacket *target = pixels.get(x_, y_, 1, 1 );
  if (target)
    ColorFloodfillImage ( image(), // Image *image
                          options()->drawInfo(), // const DrawInfo *draw_info
                          *target, // const PixelPacket target
                          static_cast<long>(x_), // const long x_offset
                          static_cast<long>(y_), // const long y_offset
                          FloodfillMethod // const PaintMethod method
      );

  throwImageException();
}
void Magick::Image::floodFillTexture( const Magick::Geometry &point_,
				      const Magick::Image &texture_ )
{
  floodFillTexture( point_.xOff(), point_.yOff(), texture_ );
}

// Flood-fill texture across pixels starting at target-pixel and
// stopping at pixels matching specified border color.
// Uses current fuzz setting when determining color match.
void Magick::Image::floodFillTexture( const unsigned int x_,
                                      const unsigned int y_,
				      const Magick::Image &texture_,
				      const Magick::Color &borderColor_ )
{
  modifyImage();

  // Set drawing fill pattern
  options()->fillPattern(texture_.constImage());

  ColorFloodfillImage ( image(),
                        options()->drawInfo(),
                        static_cast<PixelPacket>(borderColor_),
                        static_cast<long>(x_),
                        static_cast<long>(y_),
                        FillToBorderMethod
                        );

  throwImageException();
}
void  Magick::Image::floodFillTexture( const Magick::Geometry &point_,
				       const Magick::Image &texture_,
				       const Magick::Color &borderColor_ )
{
  floodFillTexture( point_.xOff(), point_.yOff(), texture_, borderColor_ );
}

// Flop image (reflect each scanline in the horizontal direction)
void Magick::Image::flop ( void )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    FlopImage( image(), &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Frame image
void Magick::Image::frame ( const Geometry &geometry_ )
{
  FrameInfo info;

  info.x           = static_cast<long>(geometry_.width());
  info.y           = static_cast<long>(geometry_.height());
  info.width       = columns() + ( static_cast<unsigned long>(info.x) << 1 );
  info.height      = rows() + ( static_cast<unsigned long>(info.y) << 1 );
  info.outer_bevel = geometry_.xOff();
  info.inner_bevel = geometry_.yOff();

  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    FrameImage( image(), &info, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}
void Magick::Image::frame ( const unsigned int width_,
                            const unsigned int height_,
			    const int innerBevel_, const int outerBevel_ )
{
  FrameInfo info;
  info.x           = static_cast<long>(width_);
  info.y           = static_cast<long>(height_);
  info.width       = columns() + ( static_cast<unsigned long>(info.x) << 1 );
  info.height      = rows() + ( static_cast<unsigned long>(info.y) << 1 );
  info.outer_bevel = static_cast<long>(outerBevel_);
  info.inner_bevel = static_cast<long>(innerBevel_);

  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    FrameImage( image(), &info, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Gamma correct image
void Magick::Image::gamma ( const double gamma_ )
{
  char gamma[MaxTextExtent + 1];
  FormatString( gamma, "%3.6f", gamma_);

  modifyImage();
  GammaImage ( image(), gamma );
}
void Magick::Image::gamma ( const double gammaRed_,
                            const double gammaGreen_,
			    const double gammaBlue_ )
{
  char gamma[MaxTextExtent + 1];
  FormatString( gamma, "%3.6f/%3.6f/%3.6f/",
		gammaRed_, gammaGreen_, gammaBlue_);

  modifyImage();
  GammaImage ( image(), gamma );
  throwImageException();
}

// Gaussian blur image
// The number of neighbor pixels to be included in the convolution
// mask is specified by 'width_'. The standard deviation of the
// gaussian bell curve is specified by 'sigma_'.
void Magick::Image::gaussianBlur ( const double width_, const double sigma_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    GaussianBlurImage( image(), width_, sigma_, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Implode image
void Magick::Image::implode ( const double factor_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    ImplodeImage( image(), factor_, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Magnify image by integral size
void Magick::Image::magnify ( void )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    MagnifyImage( image(), &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Remap image colors with closest color from reference image
void Magick::Image::map ( const Image &mapImage_ , const bool dither_ )
{
  modifyImage();
  MapImage ( image(),
             mapImage_.constImage(),
             dither_ == true ? 1 : 0 );
  throwImageException();
}
// Floodfill designated area with replacement opacity value
void Magick::Image::matteFloodfill ( const Color &target_ ,
				     const unsigned int opacity_,
				     const int x_, const int y_,
				     const Magick::PaintMethod method_ )
{
  modifyImage();
  MatteFloodfillImage ( image(), static_cast<PixelPacket>(target_),
                        opacity_, x_, y_, method_ );
  throwImageException();
}

// Filter image by replacing each pixel component with the median
// color in a circular neighborhood
void Magick::Image::medianFilter ( const double radius_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    MedianFilterImage ( image(), radius_, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Reduce image by integral size
void Magick::Image::minify ( void )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    MinifyImage( image(), &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Modulate percent hue, saturation, and brightness of an image
void Magick::Image::modulate ( const double brightness_,
			       const double saturation_,
			       const double hue_ )
{
  char modulate[MaxTextExtent + 1];
  FormatString( modulate, "%3.6f/%3.6f/%3.6f",
		brightness_, saturation_, hue_);

  modifyImage();
  ModulateImage( image(), modulate );
  throwImageException();
}

// Negate image.  Set grayscale_ to true to effect grayscale values
// only
void Magick::Image::negate ( const bool grayscale_ )
{
  modifyImage();
  NegateImage ( image(), grayscale_ == true ? 1 : 0 );
  throwImageException();
}

// Normalize image
void Magick::Image::normalize ( void )
{
  modifyImage();
  NormalizeImage ( image() );
  throwImageException();
}

// Oilpaint image
void Magick::Image::oilPaint ( const double radius_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    OilPaintImage( image(), radius_, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Set or attenuate the opacity channel. If the image pixels are
// opaque then they are set to the specified opacity value, otherwise
// they are blended with the supplied opacity value.  The value of
// opacity_ ranges from 0 (completely opaque) to MaxRGB. The defines
// OpaqueOpacity and TransparentOpacity are available to specify
// completely opaque or completely transparent, respectively.
void Magick::Image::opacity ( const unsigned int opacity_ )
{
  modifyImage();
  SetImageOpacity( image(), opacity_ );
}

// Change the color of an opaque pixel to the pen color.
void Magick::Image::opaque ( const Color &opaqueColor_,
			     const Color &penColor_ )
{
  if ( !opaqueColor_.isValid() )
  {
    throwExceptionExplicit( OptionError,
			    "Opaque color argument is invalid" );
  }
  if ( !penColor_.isValid() )
  {
    throwExceptionExplicit( OptionError,
			    "Pen color argument is invalid" );
  }

  modifyImage();
  OpaqueImage ( image(), opaqueColor_, penColor_ );
  throwImageException();
}

// Ping is similar to read except only enough of the image is read to
// determine the image columns, rows, and filesize.  Access the
// columns(), rows(), and fileSize() attributes after invoking ping.
// The image data is not valid after calling ping.
void Magick::Image::ping ( const std::string &imageSpec_ )
{
  options()->fileName( imageSpec_ );
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* image =
    PingImage( imageInfo(), &exceptionInfo );
  replaceImage( image );
  throwException( exceptionInfo );
}

// Ping is similar to read except only enough of the image is read
// to determine the image columns, rows, and filesize.  Access the
// columns(), rows(), and fileSize() attributes after invoking
// ping.  The image data is not valid after calling ping.
void Magick::Image::ping ( const Blob& blob_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* image =
    PingBlob( imageInfo(), blob_.data(), blob_.length(), &exceptionInfo );
  replaceImage( image );
  throwException( exceptionInfo );
}

// Quantize colors in image using current quantization settings
// Set measureError_ to true in order to measure quantization error
void Magick::Image::quantize ( const bool measureError_  )
{
  modifyImage();

  QuantizeImage( options()->quantizeInfo(),
                 image() );

  if ( measureError_ )
    GetImageQuantizationError( image() );

  // Udate DirectClass representation of pixels
  SyncImage( image() );
  throwImageException();
}

// Raise image (lighten or darken the edges of an image to give a 3-D
// raised or lowered effect)
void Magick::Image::raise ( const Geometry &geometry_ ,
			    const bool raisedFlag_ )
{
  RectangleInfo raiseInfo = geometry_;
  modifyImage();
  RaiseImage ( image(), &raiseInfo, raisedFlag_ == true ? 1 : 0 );
  throwImageException();
}

// Read image into current object
void Magick::Image::read ( const std::string &imageSpec_ )
{
  options()->fileName( imageSpec_ );

  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* image =
    ReadImage( imageInfo(), &exceptionInfo );

  // Ensure that multiple image frames were not read.
  if ( image && image->next )
    {
      // Destroy any extra image frames
      MagickLib::Image* next = image->next;
      image->next = 0;
      image->orphan = true;
      next->previous = 0;
      DestroyImageList( next );
 
    }
  replaceImage( image );
  throwException( exceptionInfo );
  if ( image )
    throwException( image->exception );
}

// Read image of specified size into current object
void Magick::Image::read ( const Geometry &size_,
			   const std::string &imageSpec_ )
{
  size( size_ );
  read( imageSpec_ );
}

// Read image from in-memory BLOB
void Magick::Image::read ( const Blob &blob_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* image =
    BlobToImage( imageInfo(),
		 static_cast<const void *>(blob_.data()),
		 blob_.length(), &exceptionInfo );
  replaceImage( image );
  throwException( exceptionInfo );
  if ( image )
    throwException( image->exception );
}

// Read image of specified size from in-memory BLOB
void  Magick::Image::read ( const Blob &blob_,
			    const Geometry &size_ )
{
  // Set image size
  size( size_ );
  // Read from Blob
  read( blob_ );
}

// Read image of specified size and depth from in-memory BLOB
void Magick::Image::read ( const Blob &blob_,
			   const Geometry &size_,
			   const unsigned int depth_ )
{
  // Set image size
  size( size_ );
  // Set image depth
  depth( depth_ );
  // Read from Blob
  read( blob_ );
}

// Read image of specified size, depth, and format from in-memory BLOB
void Magick::Image::read ( const Blob &blob_,
			   const Geometry &size_,
			   const unsigned int depth_,
			   const std::string &magick_ )
{
  // Set image size
  size( size_ );
  // Set image depth
  depth( depth_ );
  // Set image magick
  magick( magick_ );
  // Read from Blob
  read( blob_ );
}

// Read image of specified size, and format from in-memory BLOB
void Magick::Image::read ( const Blob &blob_,
			   const Geometry &size_,
			   const std::string &magick_ )
{
  // Set image size
  size( size_ );
  // Set image magick
  magick( magick_ );
  // Read from Blob
  read( blob_ );
}

// Read image based on raw pixels in memory (ConstituteImage)
void Magick::Image::read ( const unsigned int width_,
                           const unsigned int height_,
                           const std::string &map_,
                           const StorageType type_,
                           const void *pixels_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* image =
    ConstituteImage( width_, height_, map_.c_str(), type_, pixels_,
                     &exceptionInfo );
  replaceImage( image );
  throwException( exceptionInfo );
  if ( image )
    throwException( image->exception );
}

// Reduce noise in image
void Magick::Image::reduceNoise ( const double order_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    ReduceNoiseImage( image(), order_, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Roll image
void Magick::Image::roll ( const Geometry &roll_ )
{
  long xOff = roll_.xOff();
  if ( roll_.xNegative() )
    xOff = 0 - xOff;
  long yOff = roll_.yOff();
  if ( roll_.yNegative() )
    yOff = 0 - yOff;

  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    RollImage( image(), xOff, yOff, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}
void Magick::Image::roll ( const unsigned int columns_,
                           const unsigned int rows_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    RollImage( image(),
               static_cast<long>(columns_),
               static_cast<long>(rows_), &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Rotate image
void Magick::Image::rotate ( const double degrees_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    RotateImage( image(), degrees_, &exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Sample image
void Magick::Image::sample ( const Geometry &geometry_ )
{
  long x = 0;
  long y = 0;
  unsigned long width = columns();
  unsigned long height = rows();

  GetMagickGeometry (static_cast<std::string>(geometry_).c_str(),
		      &x, &y,
		      &width, &height );

  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    SampleImage( image(), width, height, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Scale image
void Magick::Image::scale ( const Geometry &geometry_ )
{
  long x = 0;
  long y = 0;
  unsigned long width = columns();
  unsigned long height = rows();

  GetMagickGeometry (static_cast<std::string>(geometry_).c_str(),
		      &x, &y,
		      &width, &height );

  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    ScaleImage( image(), width, height, &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Segment (coalesce similar image components) by analyzing the
// histograms of the color components and identifying units that are
// homogeneous with the fuzzy c-means technique.
void Magick::Image::segment ( const double clusterThreshold_, 
			      const double smoothingThreshold_ )
{
  modifyImage();
  SegmentImage ( image(),
		 options()->quantizeColorSpace(),
		 options()->verbose(),
		 clusterThreshold_,
		 smoothingThreshold_ );
  throwImageException();
  SyncImage( image() );
  throwImageException();
}

// Shade image using distant light source
void Magick::Image::shade ( const double azimuth_,
			    const double elevation_,
			    const bool   colorShading_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    ShadeImage( image(),
		colorShading_ == true ? 1 : 0,
		azimuth_,
		elevation_,
		&exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Sharpen pixels in image
void Magick::Image::sharpen ( const double radius_, const double sigma_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    SharpenImage( image(),
                  radius_,
                  sigma_,
                  &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Shave pixels from image edges.
void Magick::Image::shave ( const Geometry &geometry_ )
{
  RectangleInfo shaveInfo = geometry_;
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    ShaveImage( image(),
	       &shaveInfo,
	       &exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Shear image
void Magick::Image::shear ( const double xShearAngle_,
			    const double yShearAngle_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    ShearImage( image(),
		xShearAngle_,
		yShearAngle_,
		&exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Solarize image (similar to effect seen when exposing a photographic
// film to light during the development process)
void Magick::Image::solarize ( const double factor_ )
{
  modifyImage();
  SolarizeImage ( image(), factor_ );
  throwImageException();
}

// Spread pixels randomly within image by specified ammount
void Magick::Image::spread ( const unsigned int amount_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    SpreadImage( image(),
		 amount_,
		 &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Add a digital watermark to the image (based on second image)
void Magick::Image::stegano ( const Image &watermark_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    SteganoImage( image(),
		  watermark_.constImage(),
		  &exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Stereo image (left image is current image)
void Magick::Image::stereo ( const Image &rightImage_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    StereoImage( image(),
		 rightImage_.constImage(),
		 &exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Swirl image
void Magick::Image::swirl ( const double degrees_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    SwirlImage( image(), degrees_,
		&exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Texture image
void Magick::Image::texture ( const Image &texture_ )
{
  modifyImage();
  TextureImage( image(), texture_.constImage() );
  throwImageException();
}

// Threshold image
void Magick::Image::threshold ( const double threshold_ )
{
  modifyImage();
  ThresholdImage( image(), threshold_ );
  throwImageException();
}

// Transform image based on image geometry only
void Magick::Image::transform ( const Geometry &imageGeometry_ )
{
  modifyImage();
  TransformImage ( &(image()), 0,
		   std::string(imageGeometry_).c_str() );
  throwImageException();
}
// Transform image based on image and crop geometries
void Magick::Image::transform ( const Geometry &imageGeometry_,
				const Geometry &cropGeometry_ )
{
  modifyImage();
  TransformImage ( &(image()), std::string(cropGeometry_).c_str(),
		   std::string(imageGeometry_).c_str() );
  throwImageException();
}

// Add matte image to image, setting pixels matching color to transparent
void Magick::Image::transparent ( const Color &color_ )
{
  if ( !color_.isValid() )
  {
    throwExceptionExplicit( OptionError,
			    "Color argument is invalid" );
  }

  std::string color = color_;

  modifyImage();
  TransparentImage ( image(), color_, TransparentOpacity );
  throwImageException();
}

// Trim edges that are the background color from the image
void Magick::Image::trim ( void )
{
  // width=0, height=0 trims edges
  Geometry cropInfo(0,0);
  crop ( cropInfo );
}

// Replace image with a sharpened version of the original image
// using the unsharp mask algorithm.
//  radius_
//    the radius of the Gaussian, in pixels, not counting the
//    center pixel.
//  sigma_
//    the standard deviation of the Gaussian, in pixels.
//  amount_
//    the percentage of the difference between the original and
//    the blur image that is added back into the original.
// threshold_
//   the threshold in pixels needed to apply the diffence amount.
void Magick::Image::unsharpmask ( const double radius_,
                                  const double sigma_,
                                  const double amount_,
                                  const double threshold_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    UnsharpMaskImage( image(),
                      radius_,
                      sigma_,
                      amount_,
                      threshold_,
                      &exceptionInfo );
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Map image pixels to a sine wave
void Magick::Image::wave ( const double amplitude_, const double wavelength_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    WaveImage( image(),
	       amplitude_,
	       wavelength_,
	       &exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

// Write image to file
void Magick::Image::write( const std::string &imageSpec_ )
{
  modifyImage();
  fileName( imageSpec_ );
  WriteImage( imageInfo(), image() );
  throwImageException();
}

// Write image to in-memory BLOB
void Magick::Image::write ( Blob *blob_ )
{
  modifyImage();
  size_t length = 2048; // Efficient size for small images
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  void* data = ImageToBlob( imageInfo(),
			    image(),
			    &length,
			    &exceptionInfo);
  throwException( exceptionInfo );
  blob_->updateNoCopy( data, length, Blob::MallocAllocator );
  throwImageException();
}
void Magick::Image::write ( Blob *blob_,
			    const std::string &magick_ )
{
  modifyImage();
  magick(magick_);
  size_t length = 2048; // Efficient size for small images
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  void* data = ImageToBlob( imageInfo(),
			    image(),
			    &length,
			    &exceptionInfo);
  throwException( exceptionInfo );
  blob_->updateNoCopy( data, length, Blob::MallocAllocator );
  throwImageException();
}
void Magick::Image::write ( Blob *blob_,
			    const std::string &magick_,
			    const unsigned int depth_ )
{
  modifyImage();
  magick(magick_);
  depth(depth_);
  size_t length = 2048; // Efficient size for small images
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  void* data = ImageToBlob( imageInfo(),
			    image(),
			    &length,
			    &exceptionInfo);
  throwException( exceptionInfo );
  blob_->updateNoCopy( data, length, Blob::MallocAllocator );
  throwImageException();
}

// Write image to an array of pixels with storage type specified
// by user (DispatchImage), e.g.
// image.write( 0, 0, 640, 1, "RGB", 0, pixels );
void Magick::Image::write ( const int x_,
                            const int y_,
                            const unsigned int columns_,
                            const unsigned int rows_,
                            const std::string &map_,
                            const StorageType type_,
                            void *pixels_ )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  DispatchImage( image(), x_, y_, columns_, rows_, map_.c_str(), type_,
                 pixels_,
    &exceptionInfo);
  throwException( exceptionInfo );
}

// Zoom image
void Magick::Image::zoom( const Geometry &geometry_ )
{
  // Calculate new size.  This code should be supported using binary arguments
  // in the ImageMagick library.
  long x = 0;
  long y = 0;
  unsigned long width = columns();
  unsigned long height = rows();

  GetMagickGeometry (static_cast<std::string>(geometry_).c_str(),
                     &x, &y,
                     &width, &height );

  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  MagickLib::Image* newImage =
    ZoomImage( image(),
               width,
               height,
               &exceptionInfo);
  replaceImage( newImage );
  throwException( exceptionInfo );
}

/*
 * Methods for setting image attributes
 *
 */

// Join images into a single multi-image file
void Magick::Image::adjoin ( const bool flag_ )
{
  modifyImage();
  options()->adjoin( flag_ );
}
bool Magick::Image::adjoin ( void ) const
{
  return constOptions()->adjoin();
}

// Remove pixel aliasing
void Magick::Image::antiAlias( const bool flag_ )
{
  modifyImage();
  options()->antiAlias( static_cast<unsigned int>(flag_) );
}
bool Magick::Image::antiAlias( void )
{
  return static_cast<bool>( options()->antiAlias( ) );
}

// Animation inter-frame delay
void Magick::Image::animationDelay ( const unsigned int delay_ )
{
  modifyImage();
  image()->delay = delay_;
}
unsigned int Magick::Image::animationDelay ( void ) const
{
  return constImage()->delay;
}

// Number of iterations to play animation
void Magick::Image::animationIterations ( const unsigned int iterations_ )
{
  modifyImage();
  image()->iterations = iterations_;
}
unsigned int Magick::Image::animationIterations ( void ) const
{
  return constImage()->iterations;
}

// Background color
void Magick::Image::backgroundColor ( const Color &color_ )
{
  modifyImage();

  if ( color_.isValid() )
    {
      image()->background_color.red   = color_.redQuantum();
      image()->background_color.green = color_.greenQuantum();
      image()->background_color.blue  = color_.blueQuantum();
    }
  else
    {
      image()->background_color.red   = 0;
      image()->background_color.green = 0;
      image()->background_color.blue  = 0;
    }

  options()->backgroundColor( color_ );
}
Magick::Color Magick::Image::backgroundColor ( void ) const
{
  return constOptions()->backgroundColor( );
}

// Background fill texture
void Magick::Image::backgroundTexture ( const std::string &backgroundTexture_ )
{
  modifyImage();
  options()->backgroundTexture( backgroundTexture_ );
}
std::string Magick::Image::backgroundTexture ( void ) const
{
  return constOptions()->backgroundTexture( );
}

// Original image columns
unsigned int Magick::Image::baseColumns ( void ) const
{
  return constImage()->magick_columns;
}

// Original image name
std::string Magick::Image::baseFilename ( void ) const
{
  return std::string(constImage()->magick_filename);
}

// Original image rows
unsigned int Magick::Image::baseRows ( void ) const
{
  return constImage()->magick_rows;
}

// Border color
void Magick::Image::borderColor ( const Color &color_ )
{
  modifyImage();

  if ( color_.isValid() )
    {
      image()->border_color.red   = color_.redQuantum();
      image()->border_color.green = color_.greenQuantum();
      image()->border_color.blue  = color_.blueQuantum();
    }
  else
    {
      image()->border_color.red   = 0;
      image()->border_color.green = 0;
      image()->border_color.blue  = 0;
    }

  options()->borderColor( color_ );
}
Magick::Color Magick::Image::borderColor ( void ) const
{
  return constOptions()->borderColor( );
}

// Return smallest bounding box enclosing non-border pixels. The
// current fuzz value is used when discriminating between pixels.
// This is the crop bounding box used by crop(Geometry(0,0));
Magick::Geometry Magick::Image::boundingBox ( void ) const
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  RectangleInfo bbox = GetImageBoundingBox( constImage(), &exceptionInfo);
  throwException( exceptionInfo );
  return Geometry( bbox );
}

// Text bounding-box base color
void Magick::Image::boxColor ( const Color &boxColor_ )
{
  modifyImage();
  options()->boxColor( boxColor_ );
}
Magick::Color Magick::Image::boxColor ( void ) const
{
  return constOptions()->boxColor( );
}

// Pixel cache threshold.  Once this threshold is exceeded, all
// subsequent pixels cache operations are to/from disk.
// This setting is shared by all Image objects.
/* static */
void Magick::Image::cacheThreshold ( const unsigned int threshold_ )
{
  SetCacheThreshold( threshold_ );
}

void Magick::Image::chromaBluePrimary ( const double x_, const double y_ )
{
  modifyImage();
  image()->chromaticity.blue_primary.x = x_;
  image()->chromaticity.blue_primary.y = y_;
}
void Magick::Image::chromaBluePrimary ( double *x_, double *y_ ) const
{
  *x_ = constImage()->chromaticity.blue_primary.x;
  *y_ = constImage()->chromaticity.blue_primary.y;
}

void Magick::Image::chromaGreenPrimary ( const double x_, const double y_ )
{
  modifyImage();
  image()->chromaticity.green_primary.x = x_;
  image()->chromaticity.green_primary.y = y_;
}
void Magick::Image::chromaGreenPrimary ( double *x_, double *y_ ) const
{
  *x_ = constImage()->chromaticity.green_primary.x;
  *y_ = constImage()->chromaticity.green_primary.y;
}

void Magick::Image::chromaRedPrimary ( const double x_, const double y_ )
{
  modifyImage();
  image()->chromaticity.red_primary.x = x_;
  image()->chromaticity.red_primary.y = y_;
}
void Magick::Image::chromaRedPrimary ( double *x_, double *y_ ) const
{
  *x_ = constImage()->chromaticity.red_primary.x;
  *y_ = constImage()->chromaticity.red_primary.y;
}

void Magick::Image::chromaWhitePoint ( const double x_, const double y_ )
{
  modifyImage();
  image()->chromaticity.white_point.x = x_;
  image()->chromaticity.white_point.y = y_;
}
void Magick::Image::chromaWhitePoint ( double *x_, double *y_ ) const
{
  *x_ = constImage()->chromaticity.white_point.x;
  *y_ = constImage()->chromaticity.white_point.y;
}

// Set image storage class
void Magick::Image::classType ( const ClassType class_ )
{
  if ( classType() == PseudoClass && class_ == DirectClass )
    {
      // Use SyncImage to synchronize the DirectClass pixels with the
      // color map and then set to DirectClass type.
      modifyImage();
      SyncImage( image() );
      LiberateMemory( reinterpret_cast<void**>(&(image()->colormap)) );
      image()->storage_class = static_cast<MagickLib::ClassType>(DirectClass);
      return;
    }

  if ( classType() == DirectClass && class_ == PseudoClass )
    {
      // Quantize to create PseudoClass color map
      modifyImage();
      quantizeColors(MaxRGB + 1);
      quantize();
      image()->storage_class = static_cast<MagickLib::ClassType>(PseudoClass);
    }
}

// Associate a clip mask with the image. The clip mask must be the
// same dimensions as the image. Pass an invalid image to unset an
// existing clip mask.
void Magick::Image::clipMask ( const Magick::Image & clipMask_ )
{
  modifyImage();

  if( clipMask_.isValid() )
    {
      // Set clip mask
      ExceptionInfo exceptionInfo;
      GetExceptionInfo( &exceptionInfo );
      MagickLib::Image* clip_mask =
	CloneImage( clipMask_.constImage(),
                    0, // columns
                    0, // rows
                    1, // orphan
                    &exceptionInfo);
      throwException( exceptionInfo );
      SetImageClipMask( image(), clip_mask );
    }
  else
    {
      // Unset existing clip mask
      SetImageClipMask( image(), 0 );
    }
}
Magick::Image Magick::Image::clipMask ( void  ) const
{
      ExceptionInfo exceptionInfo;
      GetExceptionInfo( &exceptionInfo );
      MagickLib::Image* image =
	CloneImage( constImage()->clip_mask,
                    0, // columns
                    0, // rows
                    1, // orphan
                    &exceptionInfo);

      throwException( exceptionInfo );
      return Magick::Image( image );
}

void Magick::Image::colorFuzz ( const double fuzz_ )
{
  modifyImage();
  image()->fuzz = fuzz_;
  options()->colorFuzz( fuzz_ );
}
double Magick::Image::colorFuzz ( void ) const
{
  return constOptions()->colorFuzz( );
}

// Set color in colormap at index
void Magick::Image::colorMap ( const unsigned int index_,
			       const Color &color_ )
{
  MagickLib::Image* imageptr = image();

  if (index_ > MaxRGB )
    throwExceptionExplicit( OptionError,
                            "Index greater than MaxRGB" );
  
  if ( !color_.isValid() )
    throwExceptionExplicit( OptionError,
			    "Color argument is invalid");

  modifyImage();

  if ( !imageptr->colormap || index_ > imageptr->colors-1 )
    {

      if( !imageptr->colormap )
        {
          // Allocate colormap
          imageptr->colormap =
            static_cast<PixelPacket*>(AcquireMemory((index_+1)*
                                                    sizeof(PixelPacket)));
          imageptr->colors = 0;
        }
      else
        {
          // Re-allocate colormap
          ReacquireMemory(reinterpret_cast<void **>(&(imageptr->colormap)),
                          (index_+1)*sizeof(PixelPacket));
        }

      // Initialize new colormap entries as all black
      Color black(0,0,0);
      for( unsigned int i=imageptr->colors; i<index_; i++ )
        (imageptr->colormap)[i] = black;

      // Change number of colors
      imageptr->colors = index_+1;
    }

  // Finally, set color at index in colormap
  (imageptr->colormap)[index_] = color_;
}
// Return color in colormap at index
Magick::Color Magick::Image::colorMap ( const unsigned int index_ ) const
{
  const MagickLib::Image* imageptr = constImage();

  if ( !imageptr->colormap )
    throwExceptionExplicit( OptionError,
			    "Image does not support a colormap");

  if ( index_ > imageptr->colors-1 )
    throwExceptionExplicit( OptionError,
			    "Index out of range");

  return Magick::Color( (imageptr->colormap)[index_] );
}

// Image colorspace
void Magick::Image::colorSpace( const ColorspaceType colorSpace_ )
{
  // Nothing to do?
  if ( image()->colorspace == colorSpace_ )
    return;

  modifyImage();

  if ( image()->colorspace == RGBColorspace ||
       image()->colorspace == TransparentColorspace ||
       image()->colorspace == GRAYColorspace )
    {
      // Convert the image to an alternate colorspace representation
      // In:			Out:
      // RGBColorspace		RGBColorspace (no conversion)
      // TransparentColorspace	TransparentColorspace (no conversion)
      // GRAYColorspace		GRAYColorspace (no conversion if really Gray)
      // RGBColorspace		CMYKColorspace
      // RGBColorspace		GRAYColorspace
      // RGBColorspace		OHTAColorspace
      // RGBColorspace		sRGBColorspace
      // RGBColorspace		XYZColorspace
      // RGBColorspace		YCbCrColorspace
      // RGBColorspace		YCCColorspace
      // RGBColorspace		YIQColorspace
      // RGBColorspace		YPbPrColorspace
      // RGBColorspace		YUVColorspace
      RGBTransformImage( image(), colorSpace_ );
      throwImageException();
      return;
    }

  if ( colorSpace_ == RGBColorspace ||
       colorSpace_ == TransparentColorspace ||
       colorSpace_ == GRAYColorspace )
    {
      // Convert the image from an alternate colorspace representation
      // In:			Out:
      // CMYKColorspace		RGBColorspace
      // RGBColorspace          RGBColorspace (no conversion)
      // GRAYColorspace         GRAYColorspace (no conversion)
      // TransparentColorspace	TransparentColorspace (no conversion)
      // OHTAColorspace		RGBColorspace
      // sRGBColorspace		RGBColorspace
      // XYZColorspace		RGBColorspace
      // YCbCrColorspace	RGBColorspace
      // YCCColorspace		RGBColorspace
      // YIQColorspace		RGBColorspace
      // YPbPrColorspace        RGBColorspace
      // YUVColorspace		RGBColorspace
      TransformRGBImage( image(), colorSpace_ );
      throwImageException();
      return;
    }
}
Magick::ColorspaceType Magick::Image::colorSpace ( void ) const
{
  return constImage()->colorspace;
}

// Comment string
void Magick::Image::comment ( const std::string &comment_ )
{
  modifyImage();
  SetImageAttribute( image(), "Comment", NULL );
  if ( comment_.length() > 0 )
    SetImageAttribute( image(), "Comment", comment_.c_str() );
  throwImageException();
}
std::string Magick::Image::comment ( void ) const
{
  const ImageAttribute * attribute =
    GetImageAttribute( constImage(), "Comment" );

  if ( attribute )
    return std::string( attribute->value );

  return std::string(); // Intentionally no exception
}

// Compression algorithm
void Magick::Image::compressType ( const CompressionType compressType_ )
{
  modifyImage();
  image()->compression = compressType_;
  options()->compressType( compressType_ );
}
Magick::CompressionType Magick::Image::compressType ( void ) const
{
  return constOptions()->compressType( );
}

// Enable printing of debug messages from ImageMagick
void Magick::Image::debug ( const bool flag_ )
{
  modifyImage();
  options()->debug( flag_ );
}
bool Magick::Image::debug ( void ) const
{
  return constOptions()->debug();
}

// Pixel resolution
void Magick::Image::density ( const Geometry &density_ )
{
  modifyImage();
  options()->density( density_ );
  if ( density_.isValid() )
    {
      image()->x_resolution = density_.width();
      if ( density_.height() != 0 )
        {
          image()->y_resolution = density_.height();
        }
      else
        {
          image()->y_resolution = density_.width();
        }
    }
  else
    {
      // Reset to default
      image()->x_resolution = 0;
      image()->y_resolution = 0;
    }
}
Magick::Geometry Magick::Image::density ( void ) const
{
  return constOptions()->density( );
}

// Image depth (8 or 16)
void Magick::Image::depth ( const unsigned int depth_ )
{
  modifyImage();
  SetImageDepth( image(), depth_ );
  options()->depth( depth_ );
}
unsigned int Magick::Image::depth ( void ) const
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  unsigned int depth=GetImageDepth( constImage(), &exceptionInfo );
  throwException( exceptionInfo );
  return depth;
}

std::string Magick::Image::directory ( void ) const
{
  if ( constImage()->directory )
    return std::string( constImage()->directory );

  throwExceptionExplicit( CorruptImageWarning,
			  "Image does not contain a directory");

  return std::string();
}

// Endianness (little like Intel or big like SPARC) for image
// formats which support endian-specific options.
void Magick::Image::endian ( const Magick::EndianType endian_ )
{
  modifyImage();
  options()->endian( endian_ );
  image()->endian = endian_;
}
Magick::EndianType Magick::Image::endian ( void ) const
{
  return constImage()->endian;
}

// Image file name
void Magick::Image::fileName ( const std::string &fileName_ )
{
  modifyImage();

  fileName_.copy( image()->filename,
		  sizeof(image()->filename) - 1 );
  image()->filename[ fileName_.length() ] = 0; // Null terminate
  
  options()->fileName( fileName_ );
  
}
std::string Magick::Image::fileName ( void ) const
{
  return constOptions()->fileName( );
}

// Image file size
off_t Magick::Image::fileSize ( void ) const
{
  return SizeBlob( constImage() );
}

// Color to use when drawing inside an object
void Magick::Image::fillColor ( const Magick::Color &fillColor_ )
{
  modifyImage();
  options()->fillColor(fillColor_);
}
Magick::Color Magick::Image::fillColor ( void ) const
{
  return constOptions()->fillColor();
}

// Rule to use when filling drawn objects
void Magick::Image::fillRule ( const Magick::FillRule &fillRule_ )
{
  modifyImage();
  options()->fillRule(fillRule_);
}
Magick::FillRule Magick::Image::fillRule ( void ) const
{
  return constOptions()->fillRule();
}

// Pattern to use while filling drawn objects.
void Magick::Image::fillPattern ( const Image &fillPattern_ )
{
  modifyImage();
  if(fillPattern_.isValid())
    options()->fillPattern( fillPattern_.constImage() );
  else
    options()->fillPattern( static_cast<MagickLib::Image*>(NULL) );
}
Magick::Image  Magick::Image::fillPattern ( void  ) const
{
  // FIXME: This is inordinately innefficient
  Image texture;
  
  const MagickLib::Image* tmpTexture = constOptions()->fillPattern( );

  if ( tmpTexture )
    {
      ExceptionInfo exceptionInfo;
      GetExceptionInfo( &exceptionInfo );
      MagickLib::Image* image =
	CloneImage( tmpTexture,
                    0, // columns
                    0, // rows
                    1, // orphan
                    &exceptionInfo);
      texture.replaceImage( image );
      throwException( exceptionInfo );
    }
  return texture;
}

// Filter used by zoom
void Magick::Image::filterType ( const Magick::FilterTypes filterType_ )
{
  modifyImage();
  image()->filter = filterType_;
}
Magick::FilterTypes Magick::Image::filterType ( void ) const
{
  return constImage()->filter;
}

// Font name
void Magick::Image::font ( const std::string &font_ )
{
  modifyImage();
  options()->font( font_ );
}
std::string Magick::Image::font ( void ) const
{
  return constOptions()->font( );
}

// Font point size
void Magick::Image::fontPointsize ( const double pointSize_ )
{
  modifyImage();
  options()->fontPointsize( pointSize_ );
}
double Magick::Image::fontPointsize ( void ) const
{
  return constOptions()->fontPointsize( );
}

// Font type metrics
void Magick::Image::fontTypeMetrics( const std::string &text_,
                                     TypeMetric *metrics )
{
  DrawInfo *drawInfo = options()->drawInfo();
  drawInfo->text = const_cast<char *>(text_.c_str());
  GetTypeMetrics( image(), drawInfo, &(metrics->_typeMetric) );
  drawInfo->text = 0;
}

// Image format string
std::string Magick::Image::format ( void ) const
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  const MagickInfo * magick_info
    = GetMagickInfo( constImage()->magick, &exceptionInfo);
  throwException( exceptionInfo );

  if (( magick_info != 0 ) && 
      ( *magick_info->description != '\0' ))
    return std::string(magick_info->description);

  throwExceptionExplicit( CorruptImageWarning,
			  "Unrecognized image magick type" );
  return std::string();
}

// Gamma adjustment
double Magick::Image::gamma ( void ) const
{
  return constImage()->gamma;
}

Magick::Geometry Magick::Image::geometry ( void ) const
{
  if ( constImage()->geometry )
  {
    return Geometry(constImage()->geometry);
  }

  throwExceptionExplicit( OptionWarning,
			  "Image does not contain a geometry");

  return Geometry();
}

void Magick::Image::gifDisposeMethod ( const unsigned int disposeMethod_ )
{
  modifyImage();
  image()->dispose = disposeMethod_;
}
unsigned int Magick::Image::gifDisposeMethod ( void ) const
{
  // FIXME: It would be better to return an enumeration
  return constImage()->dispose;
}

// ICC color profile (BLOB)
void Magick::Image::iccColorProfile( const Magick::Blob &colorProfile_ )
{
  ProfileInfo * color_profile = &(image()->color_profile);
  LiberateMemory( reinterpret_cast<void**>(&color_profile->info) );
  color_profile->length = 0;

  if ( colorProfile_.data() != 0 )
    {
      color_profile->info = new unsigned char[colorProfile_.length()];
      memcpy( color_profile->info, colorProfile_.data(),
              colorProfile_.length());
      color_profile->length = colorProfile_.length();
    }
}
Magick::Blob Magick::Image::iccColorProfile( void ) const
{
  const ProfileInfo * color_profile = &(constImage()->color_profile);
  return Blob( color_profile->info, color_profile->length );
}

void Magick::Image::interlaceType ( const Magick::InterlaceType interlace_ )
{
  modifyImage();
  image()->interlace = interlace_;
  options()->interlaceType ( interlace_ );
}
Magick::InterlaceType Magick::Image::interlaceType ( void ) const
{
  return constOptions()->interlaceType ( );
}

// IPTC profile (BLOB)
void Magick::Image::iptcProfile( const Magick::Blob &iptcProfile_ )
{
  ProfileInfo * iptc_profile = &(image()->iptc_profile);
  LiberateMemory( reinterpret_cast<void**>(&iptc_profile->info) );
  iptc_profile->length = 0;

  if ( iptcProfile_.data() != 0 )
    {
      iptc_profile->info = new unsigned char[iptcProfile_.length()];
      memcpy( iptc_profile->info, iptcProfile_.data(),
              iptcProfile_.length());

      iptc_profile->length = iptcProfile_.length();
    }
}
Magick::Blob Magick::Image::iptcProfile( void ) const
{
  const ProfileInfo * iptc_profile = &(constImage()->iptc_profile);
  return Blob( iptc_profile->info, iptc_profile->length );
}

// Does object contain valid image?
void Magick::Image::isValid ( const bool isValid_ )
{
  if ( !isValid_ )
    {
      delete _imgRef;
      _imgRef = new ImageRef;
    }
  else if ( !isValid() )
    {
      // Construct with single-pixel black image to make
      // image valid.  This is an obvious hack.
      size( Geometry(1,1) );
      read( "xc:#000000" );
    }
}

bool Magick::Image::isValid ( void ) const
{
  if ( rows() && columns() )
    return true;

  return false;
}

// Label image
void Magick::Image::label ( const std::string &label_ )
{
  modifyImage();
  SetImageAttribute ( image(), "Label", NULL );
  if ( label_.length() > 0 )
    SetImageAttribute ( image(), "Label", label_.c_str() );
  throwImageException();
}
std::string Magick::Image::label ( void ) const
{
  const ImageAttribute * attribute =
    GetImageAttribute( constImage(), "Label" );

  if ( attribute )
    return std::string( attribute->value );

  return std::string();
}

void Magick::Image::magick ( const std::string &magick_ )
{
  modifyImage();

  magick_.copy( image()->magick,
		sizeof(image()->magick) - 1 );
  image()->magick[ magick_.length() ] = 0;
  
  options()->magick( magick_ );
}
std::string Magick::Image::magick ( void ) const
{
  if ( *(constImage()->magick) != '\0' )
    return std::string(constImage()->magick);

  return constOptions()->magick( );
}

void Magick::Image::matte ( bool matteFlag_ )
{
  modifyImage();
  image()->matte = matteFlag_;
}
bool Magick::Image::matte ( void ) const
{
  if ( constImage()->matte )
    return true;
  else
    return false;
}

void Magick::Image::matteColor ( const Color &matteColor_ )
{
  modifyImage();
  
  if ( matteColor_.isValid() )
    {
      image()->matte_color.red   = matteColor_.redQuantum();
      image()->matte_color.green = matteColor_.greenQuantum();
      image()->matte_color.blue  = matteColor_.blueQuantum();

      options()->matteColor( matteColor_ ); 
    }
  else
    {
      // Set to default matte color
      Color tmpColor( "#BDBDBD" );
      image()->matte_color.red   = tmpColor.redQuantum();
      image()->matte_color.green = tmpColor.greenQuantum();
      image()->matte_color.blue  = tmpColor.blueQuantum();

      options()->matteColor( tmpColor );
    }
}
Magick::Color Magick::Image::matteColor ( void ) const
{
  return Color( constImage()->matte_color.red,
		constImage()->matte_color.green,
		constImage()->matte_color.blue );
}

double Magick::Image::meanErrorPerPixel ( void ) const
{
  return(constImage()->mean_error_per_pixel);
}

void Magick::Image::monochrome ( const bool monochromeFlag_ )
{
  modifyImage();
  options()->monochrome( monochromeFlag_ );
}
bool Magick::Image::monochrome ( void ) const
{
  return constOptions()->monochrome( );
}

Magick::Geometry Magick::Image::montageGeometry ( void ) const
{
  if ( constImage()->montage )
    return Magick::Geometry(constImage()->montage);

  throwExceptionExplicit( CorruptImageWarning,
			  "Image does not contain a montage" );

  return Magick::Geometry();
}

double Magick::Image::normalizedMaxError ( void ) const
{
  return(constImage()->normalized_maximum_error);
}

double Magick::Image::normalizedMeanError ( void ) const
{
  return constImage()->normalized_mean_error;
}

void Magick::Image::penColor ( const Color &penColor_ )
{
  modifyImage();
  options()->fillColor(penColor_);
  options()->strokeColor(penColor_);
}
Magick::Color Magick::Image::penColor ( void  ) const
{
  return constOptions()->fillColor();
}

void Magick::Image::penTexture ( const Image &penTexture_ )
{
  modifyImage();
  if(penTexture_.isValid())
    options()->fillPattern( penTexture_.constImage() );
  else
    options()->fillPattern( static_cast<MagickLib::Image*>(NULL) );
}

Magick::Image  Magick::Image::penTexture ( void  ) const
{
  // FIXME: This is inordinately innefficient
  Image texture;
  
  const MagickLib::Image* tmpTexture = constOptions()->fillPattern( );

  if ( tmpTexture )
    {
      ExceptionInfo exceptionInfo;
      GetExceptionInfo( &exceptionInfo );
      MagickLib::Image* image =
	CloneImage( tmpTexture,
                    0, // columns
                    0, // rows
                    1, // orphan
                    &exceptionInfo);
      texture.replaceImage( image );
      throwException( exceptionInfo );
    }
  return texture;
}

// Set the color of a pixel.
void Magick::Image::pixelColor ( const unsigned int x_, const unsigned int y_,
				 const Color &color_ )
{
  if ( color_.isValid() )
  {
    // Test arguments to ensure they are within the image.
    if ( y_ > rows() || x_ > columns() )
      throwExceptionExplicit( OptionError,
			      "Access outside of image boundary" );
      
    modifyImage();

    // Set image to DirectClass
    classType( DirectClass );

    // Get pixel view
    Pixels pixels(*this);
    // Set pixel value
    *(pixels.get(x_, y_, 1, 1 )) = color_;
    // Tell ImageMagick that pixels have been updated
    pixels.sync();

    return;
  }

  throwExceptionExplicit( OptionError,
			  "Color argument is invalid" );
}
// Get the color of a pixel
Magick::Color Magick::Image::pixelColor ( const unsigned int x_,
					  const unsigned int y_ ) const
{
  ClassType storage_class;
  storage_class = classType();
  // DirectClass
  const PixelPacket* pixel = getConstPixels( x_, y_, 1, 1 );
  if ( storage_class == DirectClass )
    {
      if ( pixel )
        return Color( *pixel );
    }

  // PseudoClass
  if ( storage_class == PseudoClass )
    {
      const IndexPacket* indexes = getConstIndexes();
      if ( indexes )
        return colorMap( *indexes );
    }

  return Color(); // invalid
}

// Preferred size and location of an image canvas.
void Magick::Image::page ( const Magick::Geometry &pageSize_ )
{
  modifyImage();
  options()->page( pageSize_ );
  image()->page = pageSize_;
}
Magick::Geometry Magick::Image::page ( void ) const
{
  return Geometry( constImage()->page.width,
		   constImage()->page.height,
		   AbsoluteValue(constImage()->page.x),
		   AbsoluteValue(constImage()->page.y),
                   constImage()->page.x < 0 ? true : false,
                   constImage()->page.y < 0 ? true : false);
}

// Add a named profile to an an image or remove a named profile by
// passing an empty Blob (use default Blob constructor).
// Valid names are:
// "*", "8BIM", "ICM", "IPTC", or a generic profile name.
void Magick::Image::profile( const std::string name_,
                             const Magick::Blob &profile_ )
{
  modifyImage();
  int result = ProfileImage( image(), name_.c_str(),
                             (unsigned char *)profile_.data(),
                             profile_.length(), true);

  if( !result )
    throwImageException();
}

// Retrieve a named profile from the image.
// Valid names are:
// "8BIM", "8BIMTEXT", "APP1", "APP1JPEG", "ICC", "ICM", & "IPTC" or
// an existing generic profile name.
Magick::Blob Magick::Image::profile( const std::string name_ ) const
{
  const MagickLib::Image* image = constImage();

  for (long i=0; i < (long) image->generic_profiles; i++)
    {
      if (!GlobExpression(image->generic_profile[i].name,name_.c_str()))
        continue;      
      
      return Blob( (void*)image->generic_profile[i].info,
                   image->generic_profile[i].length );
    }
  
  Blob blob;
  Image temp_image = *this;
  temp_image.write( &blob, name_ );
  return blob;
}

void Magick::Image::quality ( const unsigned int quality_ )
{
  modifyImage();
  options()->quality( quality_ );
}
unsigned int Magick::Image::quality ( void ) const
{
  return constOptions()->quality( );
}

void Magick::Image::quantizeColors ( const unsigned int colors_ )
{
  modifyImage();
  options()->quantizeColors( colors_ );
}
unsigned int Magick::Image::quantizeColors ( void ) const
{
  return constOptions()->quantizeColors( );
}

void Magick::Image::quantizeColorSpace
  ( const Magick::ColorspaceType colorSpace_ )
{
  modifyImage();
  options()->quantizeColorSpace( colorSpace_ );
}
Magick::ColorspaceType Magick::Image::quantizeColorSpace ( void ) const
{
  return constOptions()->quantizeColorSpace( );
}

void Magick::Image::quantizeDither ( const bool ditherFlag_ )
{
  modifyImage();
  options()->quantizeDither( ditherFlag_ );
}
bool Magick::Image::quantizeDither ( void ) const
{
  return constOptions()->quantizeDither( );
}

void Magick::Image::quantizeTreeDepth ( const unsigned int treeDepth_ )
{
  modifyImage();
  options()->quantizeTreeDepth( treeDepth_ );
}
unsigned int Magick::Image::quantizeTreeDepth ( void ) const
{
  return constOptions()->quantizeTreeDepth( );
}

void Magick::Image::renderingIntent
  ( const Magick::RenderingIntent renderingIntent_ )
{
  modifyImage();
  image()->rendering_intent = renderingIntent_;
}
Magick::RenderingIntent Magick::Image::renderingIntent ( void ) const
{
  return static_cast<Magick::RenderingIntent>(constImage()->rendering_intent);
}

void Magick::Image::resolutionUnits
  ( const Magick::ResolutionType resolutionUnits_ )
{
  modifyImage();
  image()->units = resolutionUnits_;
  options()->resolutionUnits( resolutionUnits_ );
}
Magick::ResolutionType Magick::Image::resolutionUnits ( void ) const
{
  return constOptions()->resolutionUnits( );
}

void Magick::Image::scene ( const unsigned int scene_ )
{
  modifyImage();
  image()->scene = scene_;
}
unsigned int Magick::Image::scene ( void ) const
{
  return constImage()->scene;
}

std::string Magick::Image::signature ( const bool force_ ) const
{
  Lock( &_imgRef->_mutexLock );

  // Re-calculate image signature if necessary
  if ( force_ ||
       !GetImageAttribute(constImage(), "Signature") ||
       constImage()->taint )
    {
      SignatureImage( const_cast<MagickLib::Image *>(constImage()) );
    }

  const ImageAttribute * attribute =
    GetImageAttribute(constImage(), "Signature");

  return std::string( attribute->value );
}

void Magick::Image::size ( const Geometry &geometry_ )
{
  modifyImage();
  options()->size( geometry_ );
  image()->rows = geometry_.height();
  image()->columns = geometry_.width();
}
Magick::Geometry Magick::Image::size ( void ) const
{
  return Magick::Geometry( constImage()->columns, constImage()->rows );
}

// enabled/disable stroke anti-aliasing
void Magick::Image::strokeAntiAlias ( const bool flag_ )
{
  modifyImage();
  options()->strokeAntiAlias(flag_);
}
bool Magick::Image::strokeAntiAlias ( void ) const
{
  return constOptions()->strokeAntiAlias();
}

// Color to use when drawing object outlines
void Magick::Image::strokeColor ( const Magick::Color &strokeColor_ )
{
  modifyImage();
  options()->strokeColor(strokeColor_);
}
Magick::Color Magick::Image::strokeColor ( void ) const
{
  return constOptions()->strokeColor();
}

// dash pattern for drawing vector objects (default one)
void Magick::Image::strokeDashArray ( const double* strokeDashArray_ )
{
  modifyImage();
  options()->strokeDashArray( strokeDashArray_ );
}

const double* Magick::Image::strokeDashArray ( void ) const
{
  return constOptions()->strokeDashArray( );
}

// dash offset for drawing vector objects (default one)
void Magick::Image::strokeDashOffset ( const double strokeDashOffset_ )
{
  modifyImage();
  options()->strokeDashOffset( strokeDashOffset_ );
}

double Magick::Image::strokeDashOffset ( void ) const
{
  return constOptions()->strokeDashOffset( );
}

// Specify the shape to be used at the end of open subpaths when they
// are stroked. Values of LineCap are UndefinedCap, ButtCap, RoundCap,
// and SquareCap.
void Magick::Image::strokeLineCap ( const Magick::LineCap lineCap_ )
{
  modifyImage();
  options()->strokeLineCap( lineCap_ );
}
Magick::LineCap Magick::Image::strokeLineCap ( void ) const
{
  return constOptions()->strokeLineCap( );
}

// Specify the shape to be used at the corners of paths (or other
// vector shapes) when they are stroked. Values of LineJoin are
// UndefinedJoin, MiterJoin, RoundJoin, and BevelJoin.
void Magick::Image::strokeLineJoin ( const Magick::LineJoin lineJoin_ )
{
  modifyImage();
  options()->strokeLineJoin( lineJoin_ );
}
Magick::LineJoin Magick::Image::strokeLineJoin ( void ) const
{
  return constOptions()->strokeLineJoin( );
}

// Specify miter limit. When two line segments meet at a sharp angle
// and miter joins have been specified for 'lineJoin', it is possible
// for the miter to extend far beyond the thickness of the line
// stroking the path. The miterLimit' imposes a limit on the ratio of
// the miter length to the 'lineWidth'. The default value of this
// parameter is 4.
void Magick::Image::strokeMiterLimit ( const unsigned int strokeMiterLimit_ )
{
  modifyImage();
  options()->strokeMiterLimit( strokeMiterLimit_ );
}
unsigned int Magick::Image::strokeMiterLimit ( void ) const
{
  return constOptions()->strokeMiterLimit( );
}

// Pattern to use while stroking drawn objects.
void Magick::Image::strokePattern ( const Image &strokePattern_ )
{
  modifyImage();
  if(strokePattern_.isValid())
    options()->strokePattern( strokePattern_.constImage() );
  else
    options()->strokePattern( static_cast<MagickLib::Image*>(NULL) );
}
Magick::Image  Magick::Image::strokePattern ( void  ) const
{
  // FIXME: This is inordinately innefficient
  Image texture;
  
  const MagickLib::Image* tmpTexture = constOptions()->strokePattern( );

  if ( tmpTexture )
    {
      ExceptionInfo exceptionInfo;
      GetExceptionInfo( &exceptionInfo );
      MagickLib::Image* image =
	CloneImage( tmpTexture,
                    0, // columns
                    0, // rows
                    1, // orphan
                    &exceptionInfo);
      throwException( exceptionInfo );
      texture.replaceImage( image );
    }
  return texture;
}

// Stroke width for drawing lines, circles, ellipses, etc.
void Magick::Image::strokeWidth ( const double strokeWidth_ )
{
  modifyImage();
  options()->strokeWidth( strokeWidth_ );
}
double Magick::Image::strokeWidth ( void ) const
{
  return constOptions()->strokeWidth( );
}

void Magick::Image::subImage ( const unsigned int subImage_ )
{
  modifyImage();
  options()->subImage( subImage_ );
}
unsigned int Magick::Image::subImage ( void ) const
{
  return constOptions()->subImage( );
}

void Magick::Image::subRange ( const unsigned int subRange_ )
{
  modifyImage();
  options()->subRange( subRange_ );
}
unsigned int Magick::Image::subRange ( void ) const
{
  return constOptions()->subRange( );
}

// Annotation text encoding (e.g. "UTF-16")
void Magick::Image::textEncoding ( const std::string &encoding_ )
{
  modifyImage();
  options()->textEncoding( encoding_ );
}
std::string Magick::Image::textEncoding ( void ) const
{
  return constOptions()->textEncoding( );
}

void Magick::Image::tileName ( const std::string &tileName_ )
{
  modifyImage();
  options()->tileName( tileName_ );
}
std::string Magick::Image::tileName ( void ) const
{
  return constOptions()->tileName( );
}

unsigned long Magick::Image::totalColors ( void )
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  unsigned long colors = GetNumberColors( image(), 0, &exceptionInfo);
  throwException( exceptionInfo );
  return colors;
}

// Origin of coordinate system to use when annotating with text or drawing
void Magick::Image::transformOrigin ( const double x_, const double y_ )
{
  modifyImage();
  options()->transformOrigin( x_, y_ );
}

// Rotation to use when annotating with text or drawing
void Magick::Image::transformRotation ( const double angle_ )
{
  modifyImage();
  options()->transformRotation( angle_ );
}

// Reset transformation parameters to default
void Magick::Image::transformReset ( void )
{
  modifyImage();
  options()->transformReset();
}

// Scale to use when annotating with text or drawing
void Magick::Image::transformScale ( const double sx_, const double sy_ )
{
  modifyImage();
  options()->transformScale( sx_, sy_ );
}

// Skew to use in X axis when annotating with text or drawing
void Magick::Image::transformSkewX ( const double skewx_ )
{
  modifyImage();
  options()->transformSkewX( skewx_ );
}

// Skew to use in Y axis when annotating with text or drawing
void Magick::Image::transformSkewY ( const double skewy_ )
{
  modifyImage();
  options()->transformSkewY( skewy_ );
}

// Image representation type
Magick::ImageType Magick::Image::type ( void ) const
{

  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  ImageType image_type = constOptions()->type();
  if ( image_type == UndefinedType )
    image_type= GetImageType(constImage(), &exceptionInfo);
  throwException( exceptionInfo );
  return image_type;
}
void Magick::Image::type ( const Magick::ImageType type_)
{
  modifyImage();
  options()->type( type_ );
  SetImageType( image(), type_ );
}

void Magick::Image::verbose ( const bool verboseFlag_ )
{
  modifyImage();
  options()->verbose( verboseFlag_ );
}
bool Magick::Image::verbose ( void ) const
{
  return constOptions()->verbose( );
}

void Magick::Image::view ( const std::string &view_ )
{
  modifyImage();
  options()->view( view_ );
}
std::string Magick::Image::view ( void ) const
{
  return constOptions()->view( );
}

void Magick::Image::x11Display ( const std::string &display_ )
{
  modifyImage();
  options()->x11Display( display_ );
}
std::string Magick::Image::x11Display ( void ) const
{
  return constOptions()->x11Display( );
}

double Magick::Image::xResolution ( void ) const
{
  return constImage()->x_resolution;
}
double Magick::Image::yResolution ( void ) const
{
  return constImage()->y_resolution;
}

// Copy Constructor
Magick::Image::Image( const Image & image_ )
  : _imgRef(image_._imgRef)
{
  Lock( &_imgRef->_mutexLock );

  // Increase reference count
  ++_imgRef->_refCount;
}

// Assignment operator
Magick::Image& Magick::Image::operator=( const Magick::Image &image_ )
{
  if( this != &image_ )
    {
      {
        Lock( &image_._imgRef->_mutexLock );
        ++image_._imgRef->_refCount;
      }

      bool doDelete = false;
      {
        Lock( &_imgRef->_mutexLock );
        if ( --_imgRef->_refCount == 0 )
          doDelete = true;
      }

      if ( doDelete )
        {
          // Delete old image reference with associated image and options.
          delete _imgRef;
          _imgRef = 0;
        }
      // Use new image reference
      _imgRef = image_._imgRef;
    }

  return *this;
}

//////////////////////////////////////////////////////////////////////    
//
// Low-level Pixel Access Routines
//
// Also see the Pixels class, which provides support for multiple
// cache views. The low-level pixel access routines in the Image
// class are provided in order to support backward compatability.
//
//////////////////////////////////////////////////////////////////////

// Transfers read-only pixels from the image to the pixel cache as
// defined by the specified region
const Magick::PixelPacket* Magick::Image::getConstPixels
  ( const int x_, const int y_,
    const unsigned int columns_,
    const unsigned int rows_ ) const
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  const PixelPacket* p = (*AcquireImagePixels)( constImage(),
                                                x_, y_,
                                                columns_, rows_,
                                                &exceptionInfo );
  throwException( exceptionInfo );
  return p;
}

// Obtain read-only pixel indexes (valid for PseudoClass images)
const Magick::IndexPacket* Magick::Image::getConstIndexes ( void ) const
{
  const Magick::IndexPacket* result = GetIndexes( constImage() );

  if( !result )
    throwImageException();

  return result;
}

// Obtain image pixel indexes (valid for PseudoClass images)
Magick::IndexPacket* Magick::Image::getIndexes ( void ) const
{
  Magick::IndexPacket* result = GetIndexes( constImage() );

  if( !result )
    throwImageException();

  return ( result );
}

// Transfers pixels from the image to the pixel cache as defined
// by the specified region. Modified pixels may be subsequently
// transferred back to the image via syncPixels.
Magick::PixelPacket* Magick::Image::getPixels ( const int x_, const int y_,
						const unsigned int columns_,
						const unsigned int rows_ )
{
  modifyImage();
  PixelPacket* result = (*GetImagePixels)( image(),
                                           x_, y_,
                                           columns_, rows_ );
  if( !result )
    throwImageException();

  return result;
}

// Allocates a pixel cache region to store image pixels as defined
// by the region rectangle.  This area is subsequently transferred
// from the pixel cache to the image via syncPixels.
Magick::PixelPacket* Magick::Image::setPixels ( const int x_, const int y_,
						const unsigned int columns_,
						const unsigned int rows_ )
{
  modifyImage();
  PixelPacket* result = (*SetImagePixels)( image(),
                                           x_, y_,
                                           columns_, rows_ );
  if( !result )
    throwImageException();

  return result;
}

// Transfers the image cache pixels to the image.
void Magick::Image::syncPixels ( void )
{
  (*SyncImagePixels)( image() );
  throwImageException();
}

// Transfers one or more pixel components from a buffer or file
// into the image pixel cache of an image.
// Used to support image decoders.
void Magick::Image::readPixels ( const Magick::QuantumType quantum_,
                                 const  unsigned char *source_ )
{
  PushImagePixels( image(), quantum_, source_ );
  throwImageException();
}

// Transfers one or more pixel components from the image pixel
// cache to a buffer or file.
// Used to support image encoders.
void Magick::Image::writePixels ( const Magick::QuantumType quantum_,
                                  unsigned char *destination_ )
{
  PopImagePixels( image(), quantum_, destination_ );
  throwImageException();
}

/////////////////////////////////////////////////////////////////////
//
// No end-user methods beyond this point
//
/////////////////////////////////////////////////////////////////////


//
// Construct using Image and Magick::Options
//
Magick::Image::Image ( MagickLib::Image* image_ )
  : _imgRef(new ImageRef( image_, new Magick::Options() ))
{
}

// Get Magick::Options*
Magick::Options* Magick::Image::options( void )
{
  return _imgRef->options();
}
const Magick::Options* Magick::Image::constOptions( void ) const
{
  return _imgRef->options();
}

// Get MagickLib::Image*
MagickLib::Image*& Magick::Image::image( void )
{
  return _imgRef->image();
}
const MagickLib::Image* Magick::Image::constImage( void ) const
{
  return _imgRef->image();
}

// Get ImageInfo *
MagickLib::ImageInfo* Magick::Image::imageInfo( void )
{
  return _imgRef->options()->imageInfo();
}
const MagickLib::ImageInfo * Magick::Image::constImageInfo( void ) const
{
  return _imgRef->options()->imageInfo();
}

// Get QuantizeInfo *
MagickLib::QuantizeInfo* Magick::Image::quantizeInfo( void )
{
  return _imgRef->options()->quantizeInfo();
}
const MagickLib::QuantizeInfo * Magick::Image::constQuantizeInfo( void ) const
{
  return _imgRef->options()->quantizeInfo();
}

//
// Replace current image
//
MagickLib::Image * Magick::Image::replaceImage
  ( MagickLib::Image* replacement_ )
{
  MagickLib::Image* image;
  
  if( replacement_ )
    image = replacement_;
  else
    image = AllocateImage(constImageInfo());

  {
    Lock( &_imgRef->_mutexLock );

    if ( _imgRef->_refCount == 1 )
      {
        // We own the image, just replace it, and de-register
        _imgRef->id( -1 );
        _imgRef->image(image);
      }
    else
      {
        // We don't own the image, dereference and replace with copy
        --_imgRef->_refCount;
        _imgRef = new ImageRef( image, constOptions() );
      }
  }

  return _imgRef->_image;
}

//
// Prepare to modify image or image options
// Replace current image and options with copy if reference count > 1
//
void Magick::Image::modifyImage( void )
{
  {
    Lock( &_imgRef->_mutexLock );
    if ( _imgRef->_refCount == 1 )
      {
        // De-register image and return
        _imgRef->id( -1 );
        return;
      }
  }

  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  replaceImage( CloneImage( image(),
                            0, // columns
                            0, // rows
                            1, // orphan
                            &exceptionInfo) );
  throwException( exceptionInfo );
  return;
}

//
// Test for an ImageMagick reported error and throw exception if one
// has been reported.  Secretly resets image->exception back to default
// state even though this method is const.
//
void Magick::Image::throwImageException( void ) const
{
  // Throw C++ exception while resetting Image exception to default state
  throwException( const_cast<MagickLib::Image*>(constImage())->exception );
}

// Register image with image registry or obtain registration id
long Magick::Image::registerId( void )
{
  Lock( &_imgRef->_mutexLock );
  if( _imgRef->id() < 0 )
    {
      ExceptionInfo exceptionInfo;
      GetExceptionInfo( &exceptionInfo );
      _imgRef->id(SetMagickRegistry(ImageRegistryType, image(),
                                    sizeof(MagickLib::Image),
				    &exceptionInfo));
      throwException( exceptionInfo );
    }
  return _imgRef->id();
}

// Unregister image from image registry
void Magick::Image::unregisterId( void )
{
  modifyImage();
  _imgRef->id( -1 );
}

//
// Cleanup class to ensure that ImageMagick singletons are destroyed
// so as to avoid any resemblence to a memory leak (which seems to
// confuse users)
//
namespace Magick
{

  class MagickCleanUp
  {
  public:
    MagickCleanUp( void );
    ~MagickCleanUp( void );
  };

  // The destructor for this object is invoked when the destructors for
  // static objects in this translation unit are invoked.
  static MagickCleanUp magickCleanUpGuard;
}

Magick::MagickCleanUp::MagickCleanUp ( void )
{
  // InitializeMagick(NULL);
}

Magick::MagickCleanUp::~MagickCleanUp ( void )
{
  DestroyMagick();
}
