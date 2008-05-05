/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
  Copyright 1991-1999 E. I. du Pont de Nemours and Company
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  ImageMagick Application Programming Interface declarations.
*/
#ifndef _MAGICK_MAGICK_H
#define _MAGICK_MAGICK_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

typedef Image
  *(*DecoderHandler)(const ImageInfo *,ExceptionInfo *);

typedef unsigned int
  (*EncoderHandler)(const ImageInfo *,Image *),
  (*MagickHandler)(const unsigned char *,const size_t);

typedef struct _MagickInfo
{
  const char
    *name,              /* format ID ("magick") */
    *description,       /* format description */
    *note,              /* usage note for user */
    *version,           /* support library version */
    *module;            /* name of loadable module */

  DecoderHandler
    decoder;            /* function vector to decoding routine */

  EncoderHandler
    encoder;            /* function vector to encoding routine */

  MagickHandler
    magick;             /* function vector to format test routine */

  void
    *client_data;       /* arbitrary user supplied data */

  MagickBool
    adjoin,             /* coder may read/write multiple frames (default True) */
    raw,                /* coder requires that size be set (default False) */
    stealth,            /* coder should not appear in formats listing (default False) */
    seekable_stream,    /* coder requires BLOB "seek" and "tell" APIs (default False)
                         *   Note that SetImageInfo() currently always copies input
                         *   from a pipe, .gz, or .bz2 file, to a temporary file so
                         *   that it can retrieve a bit of the file header in order to
                         *   support the file header magic logic.
                         */
    blob_support,	/* coder uses BLOB APIs (default True) */
    thread_support;     /* coder is thread safe (default True) */

  unsigned long
    signature;          /* private, structure validator */

  struct _MagickInfo
    *previous,          /* private, previous member in list */
    *next;              /* private, next member in list */
} MagickInfo;

/*
  Magick method declaractions.
*/
extern MagickExport char
  *MagickToMime(const char *magick);

extern MagickExport const char
  *GetImageMagick(const unsigned char *magick,const size_t length);

extern MagickExport MagickBool
  IsMagickConflict(const char *magick);

extern MagickExport MagickPassFail
  ListModuleMap(FILE *file,ExceptionInfo *exception),
  ListMagickInfo(FILE *file,ExceptionInfo *exception),
  UnregisterMagickInfo(const char *name);

extern MagickExport void
  DestroyMagick(void),
  InitializeMagick(const char *path);

extern MagickExport const MagickInfo
  *GetMagickInfo(const char *name,ExceptionInfo *exception);

extern MagickExport MagickInfo
  **GetMagickInfoArray(ExceptionInfo *exception);

extern MagickExport MagickInfo
  *RegisterMagickInfo(MagickInfo *magick_info),
  *SetMagickInfo(const char *name);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
