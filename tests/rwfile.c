/*
 *
 * Test file encode/decode operations via write/read/write/read
 * sequence to detect any data corruption problems.  This does not
 * verify that the image is correct, only that encode/decode process
 * is repeatable.
 *
 * Written by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
 *
 * The image returned by both reads must be identical (or deemed close
 * enough) in order for the test to pass.
 * */

#if !defined(_VISUALC_)
#include <magick_config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#if defined(_VISUALC_)
#include <stdlib.h>
#include <sys\types.h>
#endif
#include <time.h>
#include <magick/api.h>

int main ( int argc, char **argv )
{
  Image *original = (Image *)NULL;
  Image *final = (Image *)NULL;
  char infile[MaxTextExtent];
  char format[MaxTextExtent];
  char *size = NULL;
  int rows, columns = 0;
  char filename[80];
  double fuzz_factor = 0;
  ImageInfo imageInfo;
  ExceptionInfo exception;

  if ( argc != 3 )
    {
      printf ( "Usage: %s infile format\n", argv[0] );
      exit( 1 );
    }

  if (LocaleNCompare("rwfile",argv[0],7) == 0)
    InitializeMagick(GetExecutionPath(argv[0]));
  else
    InitializeMagick(*argv);

  strncpy( infile, argv[1], MaxTextExtent-1 );
  strncpy( format, argv[2], MaxTextExtent-1 );

  printf("%s %s %s\n",
    argv[0], infile, format );
  fflush(stdout);

  /*
   * Read original image
   */
  GetImageInfo( &imageInfo );
  GetExceptionInfo( &exception );
  imageInfo.dither = 0;
  strncpy( imageInfo.filename, infile, MaxTextExtent-1 );
  fflush(stdout);
  original = ReadImage ( &imageInfo, &exception );
  if (exception.severity != UndefinedException)
    CatchException(&exception);
  if ( original == (Image *)NULL )
    {
      printf ( "Failed to read original image %s\n", imageInfo.filename );
      exit(1);
    }

  /*
   * Obtain original image size if format requires it
   */
  rows    = original->rows;
  columns = original->columns;
  size = (char *) AcquireMemory( 40 );
  sprintf( size, "%dx%d", columns, rows );

  /*
   * Save image to file
   */
  sprintf( filename, "out_1.%s", format );
  strncpy( original->magick, format, MaxTextExtent-1 );
  strncpy( original->filename, filename, MaxTextExtent-1 );
  original->delay = 10;
  fflush(stdout);
  WriteImage ( &imageInfo, original );
  imageInfo.depth=original->depth;
  DestroyImage( original );
  original = (Image*)NULL;

  /*
   * Read image back from file
   */
  strncpy( imageInfo.magick, format, MaxTextExtent-1 );
  strncpy( imageInfo.filename, filename, MaxTextExtent-1 );
  if (size != NULL)
    CloneString( &imageInfo.size, size );
  fflush(stdout);
  original = ReadImage ( &imageInfo, &exception );
  if (exception.severity != UndefinedException)
    CatchException(&exception);
  if ( original == (Image *)NULL )
    {
      printf ( "Failed to read image from file in format %s\n",
         imageInfo.magick );
      exit(1);
    }

  /*
   * Save image to file
   */
  sprintf( filename, "out_2.%s", format );
  strncpy( original->magick, format, MaxTextExtent-1 );
  strncpy( original->filename, filename, MaxTextExtent-1 );
  original->delay = 10;
  fflush(stdout);
  WriteImage ( &imageInfo, original );

  /*
   * Read image back from file
   */
  strncpy( imageInfo.magick, format, MaxTextExtent-1 );
  strncpy( imageInfo.filename, filename, MaxTextExtent-1 );
  if ( size != NULL )
    CloneString( &imageInfo.size, size );
  fflush(stdout);
  final = ReadImage ( &imageInfo, &exception );
  if (exception.severity != UndefinedException)
    CatchException(&exception);
  if ( final == (Image *)NULL )
    {
      printf ( "Failed to read image from file in format %s\n",
         imageInfo.magick );
      exit(1);
    }

  /*
   * Check final output
   */

  if ( !strcmp( "JPEG", format ) ||
       !strcmp( "JPG", format ) ||
       !strcmp( "JPG24", format ) ||
       !strcmp( "JP2", format ) ||
       !strcmp( "PAL", format ) ||
       !strcmp( "GRAY", format ) ||
       !strcmp( "CMYK", format ) ||
       !strcmp( "PCD", format ) ||
       !strcmp( "PCDS", format ) ||
       !strcmp( "UYVY", format ) ||
       !strcmp( "YUV", format ) )
    fuzz_factor = 0.008;

  if ( !IsImagesEqual(original, final ) &&
       (original->normalized_mean_error > fuzz_factor) )
    {
      printf( "R/W file check for format \"%s\" failed: %u/%.6f/%.6fe\n",
        format,(unsigned int) original->mean_error_per_pixel,original->normalized_mean_error,
        original->normalized_maximum_error);
      exit(1);
    }

  DestroyImage( original );
  original = (Image*)NULL;
  DestroyImage( final );
  final = (Image*)NULL;

  DestroyMagick();

  return 0;
}
