/*
  ImageMagick Utility Methods.
*/
#ifndef _MAGICK_UTILITY_H
#define _MAGICK_UTILITY_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif


/*
  Typedef declarations.
*/
typedef struct _TokenInfo
{
  int
    state;

  unsigned int
    flag;

  long
    offset;

  char
    quote;
} TokenInfo;

/*
  Utilities methods.
*/
extern MagickExport char
  *AllocateString(const char *),
  *Base64Encode(const unsigned char *,const size_t,size_t *),
  *EscapeString(const char *,const char),
  *FindConfigurationFile(const char *,ExceptionInfo *),
  *FindFontFile(const char *,ExceptionInfo *),
  *FindModuleFile(const char *,ExceptionInfo *),
  *GetPageGeometry(const char *),
  **ListFiles(const char *,const char *,long *),
  *PostscriptGeometry(const char *),
  *SetClientName(const char *),
  **StringToArgv(const char *,int *),
  **StringToList(const char *),
  *TranslateText(const ImageInfo *,Image *,const char *);

extern MagickExport const char
  *AcquireString(const char *),
  *SetClientPath(const char *);

extern MagickExport double
  StringToDouble(const char *,const double);

extern MagickExport int
  GetGeometry(const char *,long *,long *,unsigned long *,unsigned long *),
  GlobExpression(const char *,const char *),
  LocaleNCompare(const char *,const char *,const size_t),
  LocaleCompare(const char *,const char *),
  GetMagickGeometry(const char *,long *,long *,unsigned long *,unsigned long *),
  ParseImageGeometry(const char *,long *,long *,unsigned long *,
    unsigned long *),
  SubstituteString(char **,const char*,const char *),
  SystemCommand(const unsigned int,const char *),
  Tokenizer(TokenInfo *,unsigned,char *,size_t,char *,char *,char *,char *,
    char,char *,int *,char *);

extern MagickExport unsigned char
  *Base64Decode(const char *, size_t *);

extern MagickExport unsigned int
  CloneString(char **,const char *),
  ConcatenateString(char **,const char *),
  ExpandFilenames(int *,char ***),
  GetExecutionPath(char *),
  IsAccessible(const char *),
  IsGeometry(const char *),
  IsGlob(const char *);

extern MagickExport unsigned long
  MultilineCensus(const char *);

extern MagickExport void
  *AcquireMemory(const size_t),
  AppendImageFormat(const char *,char *),
  *CloneMemory(void *,const void *,const size_t),
  ExpandFilename(char *),
  GetPathComponent(const char *,PathType,char *),
  GetToken(const char *,char **,char *),
  LiberateMemory(void **),
  LocaleLower(char *),
  LocaleUpper(char *),
  ReacquireMemory(void **,const size_t),
  Strip(char *),
  SetGeometry(const Image *,RectangleInfo *),
  TemporaryFilename(char *);

extern MagickExport void
#if defined(__GNUC__)
  FormatString(char *,const char *,...) __attribute__((format (printf,2,3)));
#else
  FormatString(char *,const char *,...);
#endif

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
