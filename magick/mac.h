/*
  Macintosh specific include declarations.
*/
#ifndef _MAC_H
#define _MAC_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#include <stat.h>
#include <errno.h>
#include <Errors.h>

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
extern DIR
  *opendir(char *);

extern int
  Exit(int),
  MACSystemCommand(const char *);

extern long
  telldir(DIR *);

extern struct dirent
  *readdir(DIR *);
 
extern void
  closedir(DIR *),
  MACErrorHandler(const char *,const char *),
  MACWarningHandler(const char *,const char *),
  ProcessPendingEvents(char *),
  seekdir(DIR *,long),
  SetApplicationType(const char *,const char *,OSType);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
