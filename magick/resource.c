/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%           RRRR    EEEEE   SSSSS   OOO   U   U  RRRR    CCCC  EEEEE          %
%           R   R   E       SS     O   O  U   U  R   R  C      E              %
%           RRRR    EEE      SSS   O   O  U   U  RRRR   C      EEE            %
%           R R     E          SS  O   O  U   U  R R    C      E              %
%           R  R    EEEEE   SSSSS   OOO    UUU   R  R    CCCC  EEEEE          %
%                                                                             %
%                                                                             %
%                        Get/Set ImageMagick Resources.                       %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               September 2002                                %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated %
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
#include "studio.h"
#include "list.h"
#include "resource.h"
#include "utility.h"

/*
  Typedef declarations.
*/
typedef struct _ResourceInfo
{
  off_t
    memory,
    disk,
    memory_map;
} ResourceInfo;

/*
  Global declarations.
*/
static ResourceInfo
  resource_info = { ~0, ~0, ~0, ~0};

static SemaphoreInfo
  *resource_semaphore = (SemaphoreInfo *) NULL;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A c q u i r e M a g i c k R e s o u r c e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AcquireMagickResource() acquires resources for the specified type.  True
%  is returned if the resource is available otherwise False.
%
%  The format of the AcquireMagickResource() method is:
%
%      unsigned int AcquireMagickResource(const ResourceType type,
%        const off_t size)
%
%  A description of each parameter follows:
%
%    o type: The type of resource.
%
%    o size: The number of bytes needed from for this resource.
%
%
*/
MagickExport unsigned int AcquireMagickResource(const ResourceType type,
  const off_t size)
{
  unsigned int
    status;

  status=True;
  AcquireSemaphoreInfo(&resource_semaphore);
  switch (type)
  {
    case MemoryResource:
    {
      if (resource_info.memory == ~0)
        break;
      resource_info.memory-=size;
      status=resource_info.memory > 0;
      break;
    }
    case DiskResource:
    {
      if (resource_info.disk == ~0)
        break;
      resource_info.disk-=size;
      status=resource_info.disk > 0;
      break;
    }
    case MemoryMapResource:
    {
      if (resource_info.memory_map == ~0)
        break;
      resource_info.memory_map-=size;
      status=resource_info.memory_map > 0;
      break;
    }
    default:
      break;
  }
  LiberateSemaphoreInfo(&resource_semaphore);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y M a g i c k R e s o u r c e s                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyMagickResources() destroys the resource environment.
%
%  The format of the DestroyMagickResources() method is:
%
%      DestroyMagickResources(void)
%
%
*/
MagickExport void DestroyMagickResources(void)
{
  AcquireSemaphoreInfo(&resource_semaphore);
  DestroySemaphoreInfo(&resource_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M a g i c k R e s o u r c e s                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetMagickResources() returns the available size of the specified resource.
%
%  The format of the GetMagickResources() method is:
%
%      ResourceInfo GetMagickResources(const ResourceType type)
%
%  A description of each parameter follows:
%
%    o type: The type of resource.
%
%
%
*/
MagickExport off_t GetMagickResources(const ResourceType type)
{
  off_t
    size;

  AcquireSemaphoreInfo(&resource_semaphore);
  switch (type)
  {
    case MemoryResource: size=resource_info.memory; break;
    case DiskResource: size=resource_info.disk; break;
    case MemoryMapResource: size=resource_info.memory_map; break;
    default: size=0; break;
  }
  LiberateSemaphoreInfo(&resource_semaphore);
  return(size);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L i b e r a t e M a g i c k R e s o u r c e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  LiberateMagickResource() returns resources for the specified type.
%
%  The format of the LiberateMagickResource() method is:
%
%      void LiberateMagickResource(const ResourceType type,const off_t size)
%
%  A description of each parameter follows:
%
%    o type: The type of resource.
%
%    o size: The size of the resource.
%
%
*/
MagickExport void LiberateMagickResource(const ResourceType type,
  const off_t size)
{
  LiberateSemaphoreInfo(&resource_semaphore);
  switch (type)
  {
    case MemoryResource:
    {
      if (resource_info.memory == ~0)
        break;
      resource_info.memory+=size;
      break;
    }
    case DiskResource:
    {
      if (resource_info.disk == ~0)
        break;
      resource_info.disk+=size;
      break;
    }
    case MemoryMapResource:
    {
      if (resource_info.memory_map == ~0)
        break;
      resource_info.memory_map+=size; break;
    }
    default: break;
  }
  LiberateSemaphoreInfo(&resource_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t M a g i c k R e s o u r c e s                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetMagickResources() sets the limit for a particular resource.
%
%  The format of the SetMagickResources() method is:
%
%      void SetMagickResources(const ResourceType type,const off_t limit)
%
%  A description of each parameter follows:
%
%    o type: The type of resource.
%
%    o limit: The maximum limit for the resource (in megabytes).
%
%
*/
MagickExport void SetMagickResources(const ResourceType type,const off_t limit)
{
  AcquireSemaphoreInfo(&resource_semaphore);
  switch (type)
  {
    case MemoryResource: resource_info.memory=1024*1024*limit; break;
    case DiskResource: resource_info.disk=1024*1024*limit; break;
    case MemoryMapResource: resource_info.memory_map=1024*1024*limit; break;
    default: break;
  }
  LiberateSemaphoreInfo(&resource_semaphore);
}
