/*
  Macintosh Utility Methods for ImageMagick.
*/
#ifndef _MAC_H
#define _MAC_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#include <locale.h>
#include <stat.h>
#include <errno.h>

/*
  Define declarations.
*/
#define S_IREAD  00400
#define S_IWRITE  00200

/*
  Typedef declarations.
*/
typedef struct _DIR
{
  int
    d_VRefNum;

  long int
    d_DirID;

  int
    d_index;
} DIR;

struct dirent
{
  char
     d_name[255];
 
  int
    d_namlen;
};

/*
  Macintosh utilities routines.
*/
extern Export DIR
  *opendir(char *);

Export Image
  *ReadPICTImage(const ImageInfo *,ExceptionInfo *);
 
extern Export int
  Exit(int),
  ImageFormatConflict(const char *),
  MACSystemCommand(const char *);

extern Export long
  telldir(DIR *);

extern Export struct dirent
  *readdir(DIR *);

extern Export void
  closedir(DIR *),
  MACErrorHandler(const ExceptionType,const char *,const char *),
  MACWarningHandler(const ExceptionType,const char *,const char *),
  ProcessPendingEvents(const char *),
  seekdir(DIR *,long),
  SetApplicationType(const char *,const char *,OSType);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
