/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                             L       OOO    GGGG                             %
%                             L      O   O  G                                 %
%                             L      O   O  G GG                              %
%                             L      O   O  G   G                             %
%                             LLLLL   OOO    GGG                              %
%                                                                             %
%                                                                             %
%                          Log GraphicsMagick Events                          %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                September 2002                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#if defined(WIN32) || defined(__CYGWIN__)
# include "magick/nt_feature.h"
#endif
#include "magick/blob.h"
#include "magick/log.h"
#include "magick/utility.h"

/*
  Define declarations.
*/
#define LogFilename  "log.mgk"

/*
  Typedef declarations.
*/
typedef enum
{ 
  DisabledOutput = 0x0000,
  UndefinedOutput = 0x0000,
  StdoutOutput = 0x0001,
  StderrOutput = 0x0002,
  XMLFileOutput = 0x0004,
  TXTFileOutput = 0x0008,
  Win32DebugOutput = 0x0010,
  Win32EventlogOutput = 0x0020,
} LogOutputType;

typedef struct _OutputInfo
{
  char *name;
  LogOutputType mask;
} OutputInfo;

typedef struct _LogInfo
{
  char
    *path,
    *filename;

  unsigned long
    generations,
    limit;

  char
    *format;

  FILE
    *file;

  unsigned long
    events,
    generation,
    count;

  LogOutputType
    output_type;

  TimerInfo
    timer;
} LogInfo;

typedef struct _EventInfo
{
  char *name;
  LogEventType mask;
  ExceptionType type;
} EventInfo;

/*
  Declare color map.
*/
static const char
  *MagickLog = (char *)
    "<?xml version=\"1.0\"?>"
    "<magicklog>"
    "  <log events=\"None\" />"
#if defined(WIN32)
    "  <log output=\"win32eventlog\" />"
#else
    "  <log output=\"stdout\" />"
#endif
    "  <log filename=\"Magick-%d.log\" />"
    "  <log generations=\"3\" />"
    "  <log limit=\"2000\" />"
    "  <log format=\"%t %r %u %p %m/%f/%l/%d/%s:\n  %e\" />"
    "</magicklog>";

static const EventInfo eventmask_map[] =
  {
    { "none", NoEventsMask, 0 },
    { "configure", ConfigureEventMask, ConfigureBase },
    { "annotate", AnnotateEventMask, AnnotateBase },
    { "render", RenderEventMask, RenderBase },
    { "transform", TransformEventMask, TransformBase },
    { "locale", LocaleEventMask, LocaleBase },
    { "coder", CoderEventMask, CoderBase },
    { "x11", X11EventMask, X11Base },
    { "cache", CacheEventMask, CacheBase },
    { "blob", BlobEventMask, BlobBase },
    { "deprecate", DeprecateEventMask, DeprecateBase },
    { "user", UserEventMask, UserBase },
    { "resource", ResourceEventMask, ResourceBase },
    { "temporaryfile", TemporaryFileEventMask, TemporaryFileBase },
    { "exception", ExceptionEventMask, ExceptionBase },
    { "option", OptionEventMask, OptionBase },
    { "all", AllEventsMask, 0 },
    { 0, 0 }
  };

static const OutputInfo output_map[] =
  {
    { "none", UndefinedOutput },
    { "disabled", DisabledOutput },
    { "stdout", StdoutOutput },
    { "stderr", StderrOutput },
    { "xmlfile", XMLFileOutput },
    { "txtfile", TXTFileOutput },
    { "win32debug", Win32DebugOutput },
    { "win32eventlog", Win32EventlogOutput },
    { 0, 0 }
  };

/*
  Static declarations.
*/
static volatile unsigned int
  log_initialize = True;

static LogInfo
  *log_info = (LogInfo *) NULL;

static SemaphoreInfo
  *log_semaphore = (SemaphoreInfo *) NULL;

/*
  Forward declarations.
*/
static unsigned int
  ReadConfigureFile(const char *,const unsigned long,ExceptionInfo *);

static void
  *LogToBlob(const char *,size_t *,ExceptionInfo *);

/*
  Parse an event specification string and return the equivalent bits.
*/
static unsigned long ParseEvents(const char *event_string)
{
  const char
    *p;

  int
    i;

  unsigned long
    events=0;

  for (p=event_string; p != 0; p=strchr(p,','))
    {
      while ((*p != 0) && (isspace((int)(*p)) || *p == ','))
        p++;

      for (i=0; eventmask_map[i].name != 0; i++)
        {
          if (LocaleNCompare(p,eventmask_map[i].name,strlen(eventmask_map[i].name)) == 0)
            {
              events|=eventmask_map[i].mask;
              break;
            }
        }
    }

  return events;
}
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y L o g I n f o                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyLogInfo deallocates memory associated with the log list.
%
%  The format of the DestroyLogInfo method is:
%
%      DestroyLogInfo(void)
%
%
*/
MagickExport void DestroyLogInfo(void)
{
  AcquireSemaphoreInfo(&log_semaphore);
  if (log_info != (LogInfo *) NULL)
    {
      if (log_info->file != (FILE *) NULL)
        if ((log_info->file != stdout) && (log_info->file != stderr))
          {
            (void) fprintf(log_info->file,"</log>\n");
            (void) fclose(log_info->file);
          }
      if (log_info->filename != (char *) NULL)
        MagickFreeMemory(log_info->filename);
      if (log_info->path != (char *) NULL)
        MagickFreeMemory(log_info->path);
      if (log_info->format != (char *) NULL)
        MagickFreeMemory(log_info->format);
      MagickFreeMemory(log_info);
    }
  log_info=(LogInfo *) NULL;
  log_initialize=True;
  LiberateSemaphoreInfo(&log_semaphore);
  DestroySemaphoreInfo(&log_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  G e t L o g B l o b                                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetLogBlob() returns the specified configure file as a blob.
%
%  The format of the GetLogBlob method is:
%
%      void *GetLogBlob(const char *filename,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o filename: The configure file name.
%
%    o path: return the full path information of the configure file.
%
%    o length: This pointer to a size_t integer sets the initial length of the
%      blob.  On return, it reflects the actual length of the blob.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

#if !defined(UseInstalledMagick) && defined(POSIX)
static void ChopPathComponents(char *path,const unsigned long components)
{
  long
    count;

  register char
    *p;

  if (*path == '\0')
    return;
  p=path+strlen(path);
  if (*p == *DirectorySeparator)
    *p='\0';
  for (count=0; (count < (long) components) && (p > path); p--)
    if (*p == *DirectorySeparator)
      {
        *p='\0';
        count++;
      }
}
#endif

static void *GetLogBlob(const char *filename,char *path,size_t *length,
  ExceptionInfo *exception)
{
  assert(filename != (const char *) NULL);
  assert(path != (char *) NULL);
  assert(length != (size_t *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  (void) strncpy(path,filename,MaxTextExtent-1);
#if defined(UseInstalledMagick)
# if defined(MagickLibPath)
  /*
    Search hard coded paths.
  */
  FormatString(path,"%.1024s%.1024s",MagickLibPath,filename);
  return(LogToBlob(path,length,exception));
# else
#  if defined(WIN32)
  {
    char
      *key_value;

    /*
      Locate file via registry key.
    */
    key_value=NTRegistryKeyLookup("ConfigurePath");
    if (!key_value)
      {
        ThrowException(exception,ConfigureError,"RegistryKeyLookupFailed",
          "ConfigurePath");
        return 0;
      }

    FormatString(path,"%.1024s%s%.1024s",key_value,DirectorySeparator,
      filename);
    return(LogToBlob(path,length,exception));
  }
#  endif /* defined(WIN32) */
# endif /* !defined(MagickLibPath) */
# if !defined(MagickLibPath) && !defined(WIN32)
#  error MagickLibPath or WIN32 must be defined when UseInstalledMagick is defined
# endif
#else

  /*
    Search MAGICK_HOME.
  */
  if (getenv("MAGICK_HOME") != (char *) NULL)
    {
#if defined(POSIX)
      FormatString(path,"%.1024s/lib/%s/%.1024s",getenv("MAGICK_HOME"),
        MagickLibSubdir,filename);
#else
      FormatString(path,"%.1024s%s%.1024s",getenv("MAGICK_HOME"),
        DirectorySeparator,filename);
#endif
      if (IsAccessibleNoLogging(path))
        return(LogToBlob(path,length,exception));
    }

  /*
    Search $HOME/.magick.
  */
  if (getenv("HOME") != (char *) NULL)
    {
      FormatString(path,"%.1024s%s%s%.1024s",getenv("HOME"),
        *getenv("HOME") == '/' ? "/.magick" : "",DirectorySeparator,filename);
      if (IsAccessibleNoLogging(path))
        return(LogToBlob(path,length,exception));
    }

  /*
    Search based on executable directory if directory is known.
  */
  if (*SetClientPath((char *) NULL) != '\0')
    {
#if defined(POSIX)
      char
        prefix[MaxTextExtent];
      (void) strncpy(prefix,SetClientPath((char *) NULL),MaxTextExtent-1);
      ChopPathComponents(prefix,1);
      FormatString(path,"%.1024s/lib/%s/%.1024s",prefix,MagickLibSubdir,
        filename);
#else
      FormatString(path,"%.1024s%s%.1024s",SetClientPath((char *) NULL),
        DirectorySeparator,filename);
#endif
      if (IsAccessibleNoLogging(path))
        return(LogToBlob(path,length,exception));
    }
  /*
    Search current directory.
  */
  (void) strncpy(path,filename,MaxTextExtent-1);
  if (IsAccessibleNoLogging(path))
    return(LogToBlob(path,length,exception));
#if defined(WIN32)
  {
    void
      *resource;

    resource=NTResourceToBlob(filename);
    if (resource)
      return resource;
  }
#endif
#endif
  ThrowException(exception,ConfigureError,"UnableToAccessLogFile",filename);
  return 0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   I n i t i a l i z e L o g I n f o                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method InitializeLogInfo initializes the structures required to support
%  logging. It may be explicitly invoked before any other logging API is
%  invoked, or it may be implicitly invoked (as required) by IsEventLogging(),
%  LogMagickEvent(), or SetLogEventMask(). True is returned if initialization
%  is successful.  False is returned if initialization fails, and detailed
%  error info may be available (not currently assured) via the exception
%  parameter. Since logging is not a critical function, failure to initialize
%  the logging subsystem is not normally considered a fatal error.
%
%  The format of the InitializeLogInfo method is:
%
%      unsigned int InitializeLogInfo(ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o status: True on success.
%
%    o exception: May be filled with error information if there is
%    an error.
%
*/
MagickExport unsigned int InitializeLogInfo(ExceptionInfo *exception)
{
  unsigned int
    initialize;

  initialize=False;
  if ((log_info == (LogInfo *) NULL) && (log_initialize == True))
    {
      AcquireSemaphoreInfo(&log_semaphore);
      if ((log_info == (LogInfo *) NULL) && (log_initialize == True))
        {
          log_initialize=False;
          initialize=True;
        }
      LiberateSemaphoreInfo(&log_semaphore);

      if (initialize == True)
        (void) ReadConfigureFile(LogFilename,0,exception);
    }
  return(log_info != (LogInfo *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I s E v e n t L o g g i n g                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsEventLogging() returns True if logging of events is enabled otherwise
%  False.
%
%  The format of the IsEventLogging method is:
%
%      unsigned int IsEventLogging(void)
%
%
*/
MagickExport unsigned int IsEventLogging(void)
{
  if (log_initialize == True)
    {
      ExceptionInfo
        exception;

      GetExceptionInfo(&exception);
      (void) InitializeLogInfo(&exception);
      DestroyExceptionInfo(&exception);
    }
  return ((log_info != (LogInfo *) NULL) && (log_info->events != NoEventsMask));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   L o g T o B l o b                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  LogToBlob() returns the contents of a file as a blob.  It returns the
%  file as a blob and its length.  If an error occurs, NULL is returned.
%
%  The format of the LogToBlob method is:
%
%      void *LogToBlob(const char *filename,size_t *length,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o blob:  LogToBlob() returns the contents of a file as a blob.  If
%      an error occurs NULL is returned.
%
%    o filename: The filename.
%
%    o length: This pointer to a size_t integer sets the initial length of the
%      blob.  On return, it reflects the actual length of the blob.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static void *LogToBlob(const char *filename,size_t *length,
  ExceptionInfo *exception)
{
  int
    file;

  magick_off_t
    offset;

  unsigned char
    *blob;

  void
    *map;

  assert(filename != (const char *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  SetExceptionInfo(exception,UndefinedException);
  file=open(filename,O_RDONLY | O_BINARY,0777);
  if (file == -1)
    return(0);
  offset=MagickSeek(file,0,SEEK_END);
  if ((offset < 0) || (offset != (size_t) offset))
    {
      (void) close(file);
      return(0);
    }
  *length=(size_t) offset;
  blob=MagickAllocateMemory(unsigned char *,*length+1);
  if (blob == 0)
    {
      (void) close(file);
      return(0);
    }
  map=MapBlob(file,ReadMode,0,*length);
  if (map != 0)
    {
      (void) memcpy(blob,map,*length);
      UnmapBlob(map,*length);
    }
  else
    {
      ssize_t
        count;

      register size_t
        i;

      count=0;
      for (i=0; i < *length; i+=count)
      {
        count=read(file,blob+i,*length-i);
        if (count <= 0)
          break;
      }
      if (i < *length)
        {
          (void) close(file);
          MagickFreeMemory(blob);
          return(0);
        }
    }
  blob[*length]='\0';
  (void) close(file);
  return(blob);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L o g M a g i c k E v e n t                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  LogMagickEvent() logs an event as determined by the log configuration file.
%  If an error occurs, False is returned otherwise True.
%
%  The format of the LogMagickEvent method is:
%
%      unsigned int LogMagickEvent(const LogEventType type,const char *module,
%        const char *function,const unsigned long line,const char *format,...)
%
%  A description of each parameter follows:
%
%    o type: The event type.
%
%    o filename: The source module filename.
%
%    o function: The function name.
%
%    o line: The line number of the source module.
%
%    o format: The output format.
%
%
*/
MagickExport unsigned int LogMagickEvent(const ExceptionType type,
  const char *module,const char *function,const unsigned long line,
  const char *format,...)
{
  char
    *domain,
    *severity,
#if defined(WIN32)
    nteventtype,
#endif
    event[MaxTextExtent],
    srcname[MaxTextExtent],
    timestamp[MaxTextExtent];

  double
    elapsed_time,
    user_time;

  register const char
    *p;

  struct tm
    *time_meridian;

  time_t
    seconds;

  va_list
    operands;

  if (!IsEventLogging())
    return(False);

  if (log_info->events != AllEventsMask)
    {
      int
        i;

      /* first translate the base type of the event to a mask */
      for (i=0; eventmask_map[i].name != 0; i++)
        {
          if ((type % 100) == eventmask_map[i].type)
            {
              if (!(log_info->events & eventmask_map[i].mask))
                return(True);
              break;
            }
        }
    }

  /* fixup module info to just include the filename - and not the
     whole path to the file. This makes the log huge for no good
     reason */
  GetPathComponent(module,TailPath,srcname);

  AcquireSemaphoreInfo(&log_semaphore);
  switch (type % 100)
  {
    case UndefinedException: domain=(char *) "Undefined"; break;
    case ExceptionBase: domain=(char *) "Exception"; break;
    case ResourceBase: domain=(char *) "Resource"; break;
    /* case ResourceLimitBase: domain=(char *) "ResourceLimit"; break; */
    case TypeBase: domain=(char *) "Type"; break;
    /* case AnnotateBase: domain=(char *) "Annotate"; break; */
    case OptionBase: domain=(char *) "Option"; break;
    case DelegateBase: domain=(char *) "Delegate"; break;
    case MissingDelegateBase: domain=(char *) "MissingDelegate"; break;
    case CorruptImageBase : domain=(char *) "CorruptImage"; break;
    case FileOpenBase: domain=(char *) "FileOpen"; break;
    case BlobBase: domain=(char *) "Blob"; break;
    case StreamBase: domain=(char *) "Stream"; break;
    case CacheBase: domain=(char *) "Cache"; break;
    case CoderBase: domain=(char *) "Coder"; break;
    case ModuleBase: domain=(char *) "Module"; break;
    case DrawBase: domain=(char *) "Draw"; break;
    /* case RenderBase: domain=(char *) "Render"; break; */
    case ImageBase: domain=(char *) "image"; break;
    case TemporaryFileBase: domain=(char *) "TemporaryFile"; break;
    case TransformBase: domain=(char *) "Transform"; break;
    case XServerBase: domain=(char *) "XServer"; break;
    case X11Base: domain=(char *) "X11"; break;
    case UserBase: domain=(char *) "User"; break;
    case MonitorBase: domain=(char *) "Monitor"; break;
    case LocaleBase: domain=(char *) "Locale"; break;
    case DeprecateBase: domain=(char *) "Deprecate"; break;
    case RegistryBase: domain=(char *) "Registry"; break;
    case ConfigureBase: domain=(char *) "Configure"; break;
    default: domain=(char *) "UnknownEvent"; break;
  }
  switch ((type / 100) * 100)
  {
    case EventException: severity=(char *) "Event"; break;
    case WarningException: severity=(char *) "Warning"; break;
    case ErrorException: severity=(char *) "Error"; break;
    case FatalErrorException: severity=(char *) "FatalError"; break;
    default: severity=(char *) "Unknown"; break;
  }
#if defined(WIN32) || defined(__CYGWIN__)
  switch ((type / 100) * 100)
  {
    case EventException: nteventtype=EVENTLOG_INFORMATION_TYPE; break;
    case WarningException: nteventtype=EVENTLOG_WARNING_TYPE; break;
    case ErrorException: nteventtype=EVENTLOG_ERROR_TYPE; break;
    case FatalErrorException: nteventtype=EVENTLOG_ERROR_TYPE; break;
    default: nteventtype=EVENTLOG_INFORMATION_TYPE; break;
  }
#endif
  va_start(operands,format);
#if defined(HAVE_VSNPRINTF)
  (void) vsnprintf(event,MaxTextExtent,format,operands);
#else
#  if defined(HAVE_VSPRINTF)
  (void) vsprintf(event,format,operands);
#  else
#    error Neither vsnprintf or vsprintf is available.
#  endif
#endif
  va_end(operands);
  seconds=time((time_t *) NULL);
  time_meridian=localtime(&seconds);
  elapsed_time=GetElapsedTime(&log_info->timer);
  user_time=GetUserTime(&log_info->timer);
  ContinueTimer((TimerInfo *) &log_info->timer);
  FormatString(timestamp,"%04d%02d%02d%02d%02d%02d",time_meridian->tm_year+
    1900,time_meridian->tm_mon+1,time_meridian->tm_mday,
    time_meridian->tm_hour,time_meridian->tm_min,time_meridian->tm_sec);
  if (log_info->output_type & XMLFileOutput)
    {
      /*
        Log to a file in the XML format.
      */
      log_info->count++;
      if (log_info->count >= log_info->limit)
        {
          (void) fprintf(log_info->file,"</log>\n");
          (void) fclose(log_info->file);
          log_info->file=(FILE *) NULL;
          log_info->count=0;
        }
      if (log_info->file == (FILE *) NULL)
        {
          char
            filename[MaxTextExtent];

          FormatString(filename,log_info->filename,
            log_info->generation);
          log_info->file=fopen(filename,"w");
          if (log_info->file == (FILE *) NULL)
            {
              LiberateSemaphoreInfo(&log_semaphore);
              return(False);
            }
          (void) fprintf(log_info->file,"<?xml version=\"1.0\"?>\n");
          (void) fprintf(log_info->file,"<log>\n");
          log_info->generation++;
          if (log_info->generation >= log_info->generations)
            log_info->generation=0;
        }
      (void) fprintf(log_info->file,"<record>\n");
      (void) fprintf(log_info->file,"  <timestamp>%.1024s</timestamp>\n",
        timestamp);
      (void) fprintf(log_info->file,
        "  <elapsed-time>%ld:%02ld</elapsed-time>\n",
        (long) (elapsed_time/60.0),(long) ceil(fmod(elapsed_time,60.0)));
      (void) fprintf(log_info->file,"  <user-time>%0.3f</user-time>\n",
        user_time);
      (void) fprintf(log_info->file,"  <pid>%ld</pid>\n",(long) getpid());
      (void) fprintf(log_info->file,"  <module>%.1024s</module>\n",srcname);
      (void) fprintf(log_info->file,"  <function>%.1024s</function>\n",
        function);
      (void) fprintf(log_info->file,"  <line>%lu</line>\n",line);
      (void) fprintf(log_info->file,"  <domain>%.1024s</domain>\n",domain);
      (void) fprintf(log_info->file,"  <severity>%.1024s</severity>\n",severity);
      (void) fprintf(log_info->file,"  <event>%.1024s</event>\n",event);
      (void) fprintf(log_info->file,"</record>\n");
      (void) fflush(log_info->file);
      LiberateSemaphoreInfo(&log_semaphore);
      return(True);
    }
  if (log_info->output_type & TXTFileOutput)
    {
      /*
        Log to a file in the TXT format.
      */
      log_info->count++;
      if (log_info->count >= log_info->limit)
        {
          (void) fclose(log_info->file);
          log_info->file=(FILE *) NULL;
          log_info->count=0;
        }
      if (log_info->file == (FILE *) NULL)
        {
          char
            filename[MaxTextExtent];

          FormatString(filename,log_info->filename,
            log_info->generation);
          log_info->file=fopen(filename,"w");
          if (log_info->file == (FILE *) NULL)
            {
              LiberateSemaphoreInfo(&log_semaphore);
              return(False);
            }
          log_info->generation++;
          if (log_info->generation >= log_info->generations)
            log_info->generation=0;
        }
        (void) fprintf(log_info->file,
          "%.1024s %ld:%02ld %0.3f %ld %.1024s %.1024s %lu %.1024s %.1024s %.1024s\n",
            timestamp, (long) (elapsed_time/60.0), (long) ceil(fmod(elapsed_time,60.0)),
              user_time, (long) getpid(), srcname, function, line, domain, severity, event);
      (void) fflush(log_info->file);
      LiberateSemaphoreInfo(&log_semaphore);
      return(True);
    }
#if defined(WIN32) || defined(__CYGWIN__)
  if (log_info->output_type & Win32DebugOutput)
    {
      char
        buffer[MaxTextExtent];

      FormatString(buffer,
        "%.1024s %ld:%02ld %0.3f %ld %.1024s %.1024s %lu %.1024s %.1024s %.1024s\n",
          timestamp, (long) (elapsed_time/60.0), (long) ceil(fmod(elapsed_time,60.0)),
            user_time, (long) getpid(), srcname, function, line, domain, severity, event);
      OutputDebugString(buffer);
    }
  if (log_info->output_type & Win32EventlogOutput)
    {
#define LOGGING_ERROR_CODE 0
      char
        buffer[MaxTextExtent],
        *szList[1];

      HANDLE
        hSource;

      FormatString(buffer,
        "%.1024s %ld:%02ld %0.3f %ld %.1024s %.1024s %lu %.1024s %.1024s %.1024s\n",
          timestamp, (long) (elapsed_time/60.0), (long) ceil(fmod(elapsed_time,60.0)),
            user_time, (long) getpid(), srcname, function, line, domain, severity, event);
      hSource = RegisterEventSource(NULL, "GraphicsMagick");
      if (hSource != NULL)
        {
          szList[0]=buffer;
          ReportEvent(hSource,nteventtype,0,LOGGING_ERROR_CODE,NULL,1,0,szList,NULL);
          DeregisterEventSource(hSource);
        }
    }
#endif
  if ((log_info->output_type & StdoutOutput) || (log_info->output_type & StderrOutput))
    {
      FILE
        *file;
      /*
        Log to stdout in a "human readable" format.
      */
      file = stdout;
      if (log_info->output_type & StderrOutput)
        file = stderr;
      for (p=log_info->format; *p != '\0'; p++)
      {
        /*
          Process formatting characters in text.
        */
        if ((*p == '\\') && (*(p+1) == 'r'))
          {
            (void) fprintf(file,"\r");
            p++;
            continue;
          }
        if ((*p == '\\') && (*(p+1) == 'n'))
          {
            (void) fprintf(file,"\n");
            p++;
            continue;
          }
        if (*p != '%')
          {
            (void) fprintf(file,"%c",*p);
            continue;
          }
        p++;
        switch (*p)
        {
          case 'd':
          {
            (void) fprintf(file,"%.1024s",domain);
            break;
          }
          case 'e':
          {
            (void) fprintf(file,"%.1024s",event);
            break;
          }
          case 'f':
          {
            (void) fprintf(file,"%.1024s",function);
            break;
          }
          case 'l':
          {
            (void) fprintf(file,"%lu",line);
            break;
          }
          case 'm':
          {
            register const char
              *p;

            for (p=srcname+strlen(srcname)-1; p > srcname; p--)
              if (*p == *DirectorySeparator)
                {
                  p++;
                  break;
                }
            (void) fprintf(file,"%.1024s",p);
            break;
          }
          case 'p':
          {
            (void) fprintf(file,"%ld",(long) getpid());
            break;
          }
          case 'r':
          {
            (void) fprintf(file,"%ld:%02ld",(long) (elapsed_time/60.0),
              (long) ceil(fmod(elapsed_time,60.0)));
            break;
          }
          case 's':
          {
            (void) fprintf(file,"%.1024s",severity);
            break;
          }
          case 't':
          {
            (void) fprintf(file,"%02d:%02d:%02d",time_meridian->tm_hour,
              time_meridian->tm_min,time_meridian->tm_sec);
            break;
          }
          case 'u':
          {
            (void) fprintf(file,"%0.3fu",user_time);
            break;
          }
          default:
          {
            (void) fprintf(file,"%%");
            (void) fprintf(file,"%c",*p);
            break;
          }
        }
      }
      (void) fprintf(file,"\n");
      (void) fflush(file);
    }
  LiberateSemaphoreInfo(&log_semaphore);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d C o n f i g u r e F i l e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadConfigureFile() reads the log configuration file.
%
%  The format of the ReadConfigureFile method is:
%
%      unsigned int ReadConfigureFile(const char *basename,
%        const unsigned long depth,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: ReadConfigureFile() returns True if at least one log entry
%      is defined otherwise False.
%
%    o basename:  The log configuration filename.
%
%    o depth: depth of <include /> statements.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static unsigned int ReadConfigureFile(const char *basename,
  const unsigned long depth,ExceptionInfo *exception)
{
  char
    keyword[MaxTextExtent],
    path[MaxTextExtent],
    *q,
    *token,
    *xml;

  size_t
    length=0;

  /*
    Read the log configure file.
  */
  (void) strcpy(path,basename);
  if (depth == 0)
    xml=(char *) GetLogBlob(basename,path,&length,exception);
  else
    xml=(char *) LogToBlob(basename,&length,exception);
  if (xml == (char *) NULL)
    xml=AllocateString(MagickLog);
  token=AllocateString(xml);
  for (q=xml; *q != '\0'; )
  {
    /*
      Interpret Coder.
    */
    GetToken(q,&q,token);
    if (*token == '\0')
      break;
    (void) strncpy(keyword,token,MaxTextExtent-1);
    if (LocaleNCompare(keyword,"<!--",4) == 0)
      {
        /*
          Comment element.
        */
        while ((LocaleNCompare(q,"->",2) != 0) && (*q != '\0'))
          GetToken(q,&q,token);
        continue;
      }
    if (LocaleCompare(keyword,"<include") == 0)
      {
        /*
          Include element.
        */
        while ((*token != '>') && (*q != '\0'))
        {
          (void) strncpy(keyword,token,MaxTextExtent-1);
          GetToken(q,&q,token);
          if (*token != '=')
            continue;
          GetToken(q,&q,token);
          if (LocaleCompare(keyword,"file") == 0)
            {
              if (depth > 200)
                (void) fprintf(stderr,"%.1024s: <include /> nested too deeply",
                  path);
              else
                {
                  char
                    filename[MaxTextExtent];

                  GetPathComponent(path,HeadPath,filename);
                  if (*filename != '\0')
                    (void) strcat(filename,DirectorySeparator);
                  (void) strncat(filename,token,MaxTextExtent-
                    strlen(filename)-1);
                  (void) ReadConfigureFile(filename,depth+1,exception);
                }
            }
        }
        continue;
      }
    if (LocaleCompare(keyword,"<magicklog>") == 0)
      {
        /*
          Allocate memory for the log list.
        */
        log_info=MagickAllocateMemory(LogInfo *,sizeof(LogInfo));
        if (log_info == (LogInfo *) NULL)
          MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
            "UnableToAllocateLogInfo");
        (void) memset((void *) log_info,0,sizeof(LogInfo));
        log_info->path=AcquireString(path);
        GetTimerInfo((TimerInfo *) &log_info->timer);
        continue;
      }
    if (log_info == (LogInfo *) NULL)
      continue;
    GetToken(q,(char **) NULL,token);
    if (*token != '=')
      continue;
    GetToken(q,&q,token);
    GetToken(q,&q,token);
    switch (*keyword)
    {
      case 'E':
      case 'e':
      {
        if (LocaleCompare((char *) keyword,"events") == 0)
          log_info->events|=ParseEvents(token);
        break;
      }
      case 'F':
      case 'f':
      {
        if (LocaleCompare((char *) keyword,"filename") == 0)
          {
            log_info->filename=AcquireString(token);
            break;
          }
        if (LocaleCompare((char *) keyword,"format") == 0)
          {
            log_info->format=AcquireString(token);
            break;
          }
        break;
      }
      case 'G':
      case 'g':
      {
        if (LocaleCompare((char *) keyword,"generations") == 0)
          {
            log_info->generations=atol(token);
            break;
          }
        break;
      }
      case 'L':
      case 'l':
      {
        if (LocaleCompare((char *) keyword,"limit") == 0)
          {
            log_info->limit=atol(token);
            break;
          }
        break;
      }
      case 'O':
      case 'o':
      {
       if (LocaleCompare((char *) keyword,"output") == 0)
          {
            int i;

            for (i=0; output_map[i].name != 0; i++)
            {
              if (LocaleNCompare(token,output_map[i].name,strlen(output_map[i].name)) == 0)
                {
                  /* We do not OR these flags despite the fact that they are bit masks
                     because they are still mutually exclusive implementations. Asking for
                     XML and TXT format files each use the file handle field and others to
                     do their work, so they can not be used together */
                  log_info->output_type=output_map[i].mask;
                  break;
                }
            }
            break;
          }
        break;
      }
      default:
        break;
    }
  }
  MagickFreeMemory(token);
  MagickFreeMemory(xml);
  if (log_info == (LogInfo *) NULL)
    return(False);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t L o g E v e n t M a s k                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetLogEventMask() accepts a list that determines which events to log.  All
%  other events are ignored.  By default, no logging is enabled.  This method
%  returns the previous log event mask.
%
%  The format of the AcquireString method is:
%
%      unsigned long SetLogEventMask(const char *events)
%
%  A description of each parameter follows:
%
%    o events: log these events.
%
%
*/
MagickExport unsigned long SetLogEventMask(const char *events)
{

  if (log_initialize == True)
    {
      ExceptionInfo
            exception;

      GetExceptionInfo(&exception);
      (void) InitializeLogInfo(&exception);
      DestroyExceptionInfo(&exception);
    }
  AcquireSemaphoreInfo(&log_semaphore);
  if (log_info == (LogInfo *) NULL)
    {
      LiberateSemaphoreInfo(&log_semaphore);
      return(NoEventsMask);
    }
  log_info->events=NoEventsMask;
  if (events == (const char *) NULL)
    {
      LiberateSemaphoreInfo(&log_semaphore);
      return(log_info->events);
    }
  log_info->events=ParseEvents(events);
  LiberateSemaphoreInfo(&log_semaphore);
  return(log_info->events);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t L o g F o r m a t                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetLogFormat() sets the format for the "human readable" log record.
%
%  The format of the LogMagickFormat method is:
%
%      SetLogFormat(const char *format)
%
%  A description of each parameter follows:
%
%    o format: The log record format.
%
%
*/
MagickExport void SetLogFormat(const char *format)
{
  AcquireSemaphoreInfo(&log_semaphore);
  if (log_info->format != (char *) NULL)
    MagickFreeMemory(log_info->format);
  log_info->format=AcquireString(format);
  LiberateSemaphoreInfo(&log_semaphore);
}
