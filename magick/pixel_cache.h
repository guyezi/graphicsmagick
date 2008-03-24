/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick Pixel Cache Methods.
*/
#ifndef _MAGICK_CACHE_H
#define _MAGICK_CACHE_H

#include "magick/forward.h"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

  /*
    Enum declaractions.
  */

  typedef enum
    {
      UndefinedVirtualPixelMethod,
      ConstantVirtualPixelMethod,
      EdgeVirtualPixelMethod,
      MirrorVirtualPixelMethod,
      TileVirtualPixelMethod
    } VirtualPixelMethod;

  /*
    Typedef declaractions.
  */

  typedef _CacheInfoPtr_ Cache;
  typedef void *ViewInfo;


  /*****
   *
   * Default View interfaces
   *
   *****/

  /*
    Read only access to a rectangular pixel region.
  */
  MagickExport const PixelPacket
  *AcquireImagePixels(const Image *image,const long x,const long y,
                      const unsigned long columns,
                      const unsigned long rows,ExceptionInfo *exception);

  /*
    Return one pixel at the the specified (x,y) location.
  */
  extern MagickExport PixelPacket
  AcquireOnePixel(const Image *image,const long x,const long y,
                  ExceptionInfo *exception);

  /*
    DestroyImagePixels() deallocates memory associated with the pixel cache.

    Used only by DestroyImage().
  */
  extern MagickExport void
  DestroyImagePixels(Image *image);

  /*
    GetImagePixels() obtains a pixel region for read/write access.
  */
  extern MagickExport PixelPacket
  *GetImagePixels(Image *image,const long x,const long y,
                  const unsigned long columns,const unsigned long rows);

  /*
    GetImageVirtualPixelMethod() gets the "virtual pixels" method for the image.
  */
  extern MagickExport VirtualPixelMethod
  GetImageVirtualPixelMethod(const Image *image);

  /*
    GetIndexes() returns the colormap indexes associated with the last
    call to SetImagePixels() or GetImagePixels().
  */
  extern MagickExport IndexPacket
  *GetIndexes(const Image *image);

  /*
    GetOnePixel() returns a single pixel at the specified (x,y) location.
  */
  extern MagickExport PixelPacket
  GetOnePixel(Image *image,const long x,const long y);

  /*
    GetPixelCacheArea() returns the area (width * height in pixels)
    consumed by the current pixel cache.
  */
  extern MagickExport magick_off_t
  GetPixelCacheArea(const Image *image);

  /*
    GetPixels() returns the pixels associated with the last call to
    SetImagePixels() or GetImagePixels().
  */
  extern MagickExport PixelPacket
  *GetPixels(const Image *image);

  /*
    PersistCache() attaches to or initializes a persistent pixel cache.

    Used only by ReadMPCImage() and WriteMPCImage().
  */
  extern MagickExport unsigned int
  PersistCache(Image *image,const char *filename,const unsigned int attach,
               magick_off_t *offset,ExceptionInfo *exception);

  /*
    SetImagePixels() initializes a pixel region for write-only access.
  */
  extern MagickExport PixelPacket
  *SetImagePixels(Image *image,const long x,const long y,
                  const unsigned long columns,const unsigned long rows);

  /*
    SetImageVirtualPixelMethod() sets the "virtual pixels" method for
    the image.
  */
  extern MagickExport unsigned int
  SetImageVirtualPixelMethod(const Image *image,
                             const VirtualPixelMethod method);

  /*
    SyncImagePixels() saves the image pixels to the in-memory or disk
    cache.
  */
  extern MagickExport unsigned int
  SyncImagePixels(Image *image);

  /****
   *
   * Cache view interfaces.
   *
   ****/

  /*
    AcquireCacheView() obtains a pixel region from a cache view for read-only access.
  */
  extern MagickExport const PixelPacket
  *AcquireCacheView(const ViewInfo *,const long,const long,const unsigned long,
                    const unsigned long,ExceptionInfo *);

  /*
    CloseCacheView() closes a cache view.
  */
  extern MagickExport void
  CloseCacheView(ViewInfo *);

  /*
    GetCacheView() obtains a pixel region from a cache view for read/write access.
  */
  extern MagickExport PixelPacket
  *GetCacheView(ViewInfo *,const long,const long,const unsigned long,
                const unsigned long);

  /*
    GetCacheViewIndexes() returns the indexes associated with a cache view.
  */
  extern MagickExport IndexPacket
  *GetCacheViewIndexes(const ViewInfo *);

  /*
     OpenCacheView() opens a view into the pixel cache.
  */
  extern MagickExport ViewInfo
  *OpenCacheView(Image *);

  /*
    SetCacheView() gets blank pixels from the pixel cache view.
  */
  extern MagickExport PixelPacket
  *SetCacheView(ViewInfo *,const long,const long,const unsigned long,
                const unsigned long);

  /*
    SyncCacheView() saves any changes to the pixel cache view.
  */
  extern MagickExport unsigned int
  SyncCacheView(ViewInfo *);


  /*
    Stream interfaces.
  */
  
  /*
    ReadStream() makes the image pixels available to a user supplied
    callback method immediately upon reading a scanline with the
    ReadImage() method.

    Used only by ReadXTRNImage(), PingBlob(), and PingImage().
  */
  extern MagickExport Image
  *ReadStream(const ImageInfo *,StreamHandler,ExceptionInfo *);
  
  /*
    WriteStream() makes the image pixels available to a user supplied
    callback method immediately upon writing pixel data with the
    WriteImage() method.

    Used only by WriteXTRNImage()
  */
  extern MagickExport unsigned int
  WriteStream(const ImageInfo *,Image *,StreamHandler);

#if defined(MAGICK_IMPLEMENTATION)

  /****
   *
   * Private interfaces.
   *
   ****/

  /*
    ClonePixelCacheMethods() clones the pixel cache methods from one cache to another

    Used only by AllocateImage() in the case where a pixel cache is passed via ImageInfo.
  */
  extern void
  ClonePixelCacheMethods(Cache clone_info,const Cache cache_info);

  /*
    DestroyCacheInfo() deallocates memory associated with the pixel cache.

    Used only by DestroyImageInfo() to destroy a pixel cache associated with ImageInfo.
  */
  extern void
  DestroyCacheInfo(Cache cache);

  /*
    GetCacheInfo() initializes the Cache structure.

    Used only by AllocateImage() and CloneImage().
  */
  extern void
  GetCacheInfo(Cache *cache);

  /*
    GetPixelCachePresent() tests to see the pixel cache is present
    and contains pixels.
  */
  extern MagickBool
  GetPixelCachePresent(const Image *image);

  /*
    ReferenceCache() increments the reference count associated with
    the pixel cache.

    Used only by CloneImage() and CloneImageInfo().
  */
  extern Cache
  ReferenceCache(Cache cache);


#endif /* defined(MAGICK_IMPLEMENTATION) */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_CACHE_H */
