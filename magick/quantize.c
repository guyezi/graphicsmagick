/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%           QQQ   U   U   AAA   N   N  TTTTT  IIIII   ZZZZZ  EEEEE            %
%          Q   Q  U   U  A   A  NN  N    T      I        ZZ  E                %
%          Q   Q  U   U  AAAAA  N N N    T      I      ZZZ   EEEEE            %
%          Q  QQ  U   U  A   A  N  NN    T      I     ZZ     E                %
%           QQQQ   UUU   A   A  N   N    T    IIIII   ZZZZZ  EEEEE            %
%                                                                             %
%                                                                             %
%         Methods to Reduce the Number of Unique Colors in an Image           %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                              July 1992                                      %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 GraphicsMagick Group, an organization dedicated         %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files                   %
%  ("GraphicsMagick"), to deal in GraphicsMagick without restriction,         %
%  including without limitation the rights to use, copy, modify, merge,       %
%  publish, distribute, sublicense, and/or sell copies of GraphicsMagick,     %
%  and to permit persons to whom GraphicsMagick is furnished to do so,        %
%  subject to the following conditions:                                       %
%                                                                             %
%  The above copyright notice and this permission notice shall be included    %
%  in all copies or substantial portions of GraphicsMagick.                   %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express    %
%  or implied, including but not limited to the warranties of                 %
%  merchantability, fitness for a particular purpose and noninfringement.     %
%  In no event shall GraphicsMagick Group be liable for any claim,            %
%  damages or other liability, whether in an action of contract, tort or      %
%  otherwise, arising from, out of or in connection with GraphicsMagick       %
%  or the use or other dealings in GraphicsMagick.                            %
%                                                                             %
%  Except as contained in this notice, the name of the GraphicsMagick         %
%  Group shall not be used in advertising or otherwise to promote the         %
%  sale, use or other dealings in GraphicsMagick without prior written        %
%  authorization from the GraphicsMagick Group.                               %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Realism in computer graphics typically requires using 24 bits/pixel to
%  generate an image.  Yet many graphic display devices do not contain the
%  amount of memory necessary to match the spatial and color resolution of
%  the human eye.  The Quantize methods takes a 24 bit image and reduces
%  the number of colors so it can be displayed on raster device with less
%  bits per pixel.  In most instances, the quantized image closely
%  resembles the original reference image.
%
%  A reduction of colors in an image is also desirable for image
%  transmission and real-time animation.
%
%  QuantizeImage() takes a standard RGB or monochrome images and quantizes
%  them down to some fixed number of colors.
%
%  For purposes of color allocation, an image is a set of n pixels, where
%  each pixel is a point in RGB space.  RGB space is a 3-dimensional
%  vector space, and each pixel, Pi,  is defined by an ordered triple of
%  red, green, and blue coordinates, (Ri, Gi, Bi).
%
%  Each primary color component (red, green, or blue) represents an
%  intensity which varies linearly from 0 to a maximum value, Cmax, which
%  corresponds to full saturation of that color.  Color allocation is
%  defined over a domain consisting of the cube in RGB space with opposite
%  vertices at (0,0,0) and (Cmax, Cmax, Cmax).  QUANTIZE requires Cmax =
%  255.
%
%  The algorithm maps this domain onto a tree in which each node
%  represents a cube within that domain.  In the following discussion
%  these cubes are defined by the coordinate of two opposite vertices:
%  The vertex nearest the origin in RGB space and the vertex farthest from
%  the origin.
%
%  The tree's root node represents the the entire domain, (0,0,0) through
%  (Cmax,Cmax,Cmax).  Each lower level in the tree is generated by
%  subdividing one node's cube into eight smaller cubes of equal size.
%  This corresponds to bisecting the parent cube with planes passing
%  through the midpoints of each edge.
%
%  The basic algorithm operates in three phases: Classification,
%  Reduction, and Assignment.  Classification builds a color description
%  tree for the image.  Reduction collapses the tree until the number it
%  represents, at most, the number of colors desired in the output image.
%  Assignment defines the output image's color map and sets each pixel's
%  color by restorage_class in the reduced tree.  Our goal is to minimize
%  the numerical discrepancies between the original colors and quantized
%  colors (quantization error).
%
%  Classification begins by initializing a color description tree of
%  sufficient depth to represent each possible input color in a leaf.
%  However, it is impractical to generate a fully-formed color description
%  tree in the storage_class phase for realistic values of Cmax.  If
%  colors components in the input image are quantized to k-bit precision,
%  so that Cmax= 2k-1, the tree would need k levels below the root node to
%  allow representing each possible input color in a leaf.  This becomes
%  prohibitive because the tree's total number of nodes is 1 +
%  sum(i=1, k, 8k).
%
%  A complete tree would require 19,173,961 nodes for k = 8, Cmax = 255.
%  Therefore, to avoid building a fully populated tree, QUANTIZE: (1)
%  Initializes data structures for nodes only as they are needed;  (2)
%  Chooses a maximum depth for the tree as a function of the desired
%  number of colors in the output image (currently log2(colormap size)).
%
%  For each pixel in the input image, storage_class scans downward from
%  the root of the color description tree.  At each level of the tree it
%  identifies the single node which represents a cube in RGB space
%  containing the pixel's color.  It updates the following data for each
%  such node:
%
%    n1: Number of pixels whose color is contained in the RGB cube which
%    this node represents;
%
%    n2: Number of pixels whose color is not represented in a node at
%    lower depth in the tree;  initially,  n2 = 0 for all nodes except
%    leaves of the tree.
%
%    Sr, Sg, Sb: Sums of the red, green, and blue component values for all
%    pixels not classified at a lower depth. The combination of these sums
%    and n2  will ultimately characterize the mean color of a set of
%    pixels represented by this node.
%
%    E: The distance squared in RGB space between each pixel contained
%    within a node and the nodes' center.  This represents the
%    quantization error for a node.
%
%  Reduction repeatedly prunes the tree until the number of nodes with n2
%  > 0 is less than or equal to the maximum number of colors allowed in
%  the output image.  On any given iteration over the tree, it selects
%  those nodes whose E count is minimal for pruning and merges their color
%  statistics upward. It uses a pruning threshold, Ep, to govern node
%  selection as follows:
%
%    Ep = 0
%    while number of nodes with (n2 > 0) > required maximum number of colors
%      prune all nodes such that E <= Ep
%      Set Ep to minimum E in remaining nodes
%
%  This has the effect of minimizing any quantization error when merging
%  two nodes together.
%
%  When a node to be pruned has offspring, the pruning procedure invokes
%  itself recursively in order to prune the tree from the leaves upward.
%  n2,  Sr, Sg,  and  Sb in a node being pruned are always added to the
%  corresponding data in that node's parent.  This retains the pruned
%  node's color characteristics for later averaging.
%
%  For each node, n2 pixels exist for which that node represents the
%  smallest volume in RGB space containing those pixel's colors.  When n2
%  > 0 the node will uniquely define a color in the output image. At the
%  beginning of reduction,  n2 = 0  for all nodes except a the leaves of
%  the tree which represent colors present in the input image.
%
%  The other pixel count, n1, indicates the total number of colors within
%  the cubic volume which the node represents.  This includes n1 - n2
%  pixels whose colors should be defined by nodes at a lower level in the
%  tree.
%
%  Assignment generates the output image from the pruned tree.  The output
%  image consists of two parts: (1)  A color map, which is an array of
%  color descriptions (RGB triples) for each color present in the output
%  image;  (2)  A pixel array, which represents each pixel as an index
%  into the color map array.
%
%  First, the assignment phase makes one pass over the pruned color
%  description tree to establish the image's color map.  For each node
%  with n2  > 0, it divides Sr, Sg, and Sb by n2 .  This produces the mean
%  color of all pixels that classify no lower than this node.  Each of
%  these colors becomes an entry in the color map.
%
%  Finally,  the assignment phase reclassifies each pixel in the pruned
%  tree to identify the deepest node containing the pixel's color.  The
%  pixel's value in the pixel array becomes the index of this node's mean
%  color in the color map.
%
%  This method is based on a similar algorithm written by Paul Raveling.
%
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "cache.h"
#include "color.h"
#include "enhance.h"
#include "quantize.h"
#include "monitor.h"
#include "utility.h"

/*
  Define declarations.
*/
#define CacheShift  (QuantumDepth-6)
#define ExceptionQueueLength  16
#define MaxNodes  266817

#define ColorToNodeId(red,green,blue,index) ((unsigned int) \
            (((ScaleQuantumToChar(red) >> index) & 0x01) << 2 | \
             ((ScaleQuantumToChar(green) >> index) & 0x01) << 1 | \
             ((ScaleQuantumToChar(blue) >> index) & 0x01)))

/* Convert signed type to Quantum */
#define RoundSignedToQuantum(value) ((Quantum) (value < 0 ? 0 : \
            (value > MaxRGB) ? MaxRGB : value + 0.5))

/*
  Typedef declarations.
*/
#if QuantumDepth > 16 && defined(HAVE_LONG_DOUBLE)
  typedef long double ErrorSumType;
#else
  typedef double ErrorSumType;
#endif

typedef struct _NodeInfo
{
  struct _NodeInfo
    *parent,
    *child[8];

  double
    number_unique;

  double  /* was ErrorSumType */
    total_red,
    total_green,
    total_blue;

  ErrorSumType
    quantize_error;

  unsigned long
    color_number;

  unsigned char
    id,
    level,
    census;
} NodeInfo;

typedef struct _Nodes
{
  NodeInfo
    nodes[NodesInAList];

  struct _Nodes
    *next;
} Nodes;

typedef struct _CubeInfo
{
  NodeInfo
    *root;

  unsigned long
    colors;

  DoublePixelPacket
    color;

  double  /* was ErrorSumType */
    distance;

  ErrorSumType
    pruning_threshold,
    next_threshold;

  unsigned long
    nodes,
    free_nodes,
    color_number;

  NodeInfo
    *next_node;

  Nodes
    *node_queue;

  long
    *cache;

  DoublePixelPacket
    error[ExceptionQueueLength];

  double
    weights[ExceptionQueueLength];

  const QuantizeInfo
    *quantize_info;

  long
    x,
    y;

  unsigned long
    depth;
} CubeInfo;

/*
  Method prototypes.
*/
static void
  ClosestColor(Image *,CubeInfo *,const NodeInfo *);

static NodeInfo
  *GetNodeInfo(CubeInfo *,const unsigned int,const unsigned int,NodeInfo *);

static unsigned int
  DitherImage(CubeInfo *,Image *);

static void
  DefineImageColormap(Image *,NodeInfo *),
  HilbertCurve(CubeInfo *,Image *,const unsigned long,const unsigned int),
  PruneLevel(CubeInfo *,const NodeInfo *),
  PruneToCubeDepth(CubeInfo *,const NodeInfo *),
  ReduceImageColors(CubeInfo *,const unsigned long,ExceptionInfo *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   A s s i g n I m a g e C o l o r s                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AssignImageColors() generates the output image from the pruned tree.  The
%  output image consists of two parts: (1)  A color map, which is an array
%  of color descriptions (RGB triples) for each color present in the
%  output image;  (2)  A pixel array, which represents each pixel as an
%  index into the color map array.
%
%  First, the assignment phase makes one pass over the pruned color
%  description tree to establish the image's color map.  For each node
%  with n2  > 0, it divides Sr, Sg, and Sb by n2 .  This produces the mean
%  color of all pixels that classify no lower than this node.  Each of
%  these colors becomes an entry in the color map.
%
%  Finally,  the assignment phase reclassifies each pixel in the pruned
%  tree to identify the deepest node containing the pixel's color.  The
%  pixel's value in the pixel array becomes the index of this node's mean
%  color in the color map.
%
%  The format of the AssignImageColors() method is:
%
%      unsigned int AssignImageColors(CubeInfo *cube_info,Image *image)
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the Cube structure.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%
*/
static unsigned int AssignImageColors(CubeInfo *cube_info,Image *image)
{
#define AssignImageTag  "Assign/Image"

  IndexPacket
    index;

  long
    count,
    y;

  register IndexPacket
    *indexes;

  register long
    i,
    x;

  register const NodeInfo
    *node_info;

  register PixelPacket
    *q;

  unsigned int
    dither;

  unsigned int
    id,
    is_monochrome;

  /*
    Allocate image colormap.
  */
  if (!AllocateImageColormap(image,cube_info->colors))
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      "UnableToQuantizeImage");
  image->colors=0;
  is_monochrome=image->is_monochrome;
  DefineImageColormap(image,cube_info->root);
  if (cube_info->quantize_info->colorspace == TransparentColorspace)
    image->storage_class=DirectClass;
  /*
    Create a reduced color image.
  */
  dither=cube_info->quantize_info->dither;
  if (dither)
    dither=DitherImage(cube_info,image);
  if (!dither)
    for (y=0; y < (long) image->rows; y++)
    {
      q=GetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      indexes=GetIndexes(image);
      for (x=0; x < (long) image->columns; x+=count)
      {
        /*
          Identify the deepest node containing the pixel's color.
        */
        for (count=1; (x+count) < (long) image->columns; count++)
          if (!ColorMatch(q,q+count))
            break;
        node_info=cube_info->root;
        for (index=MaxTreeDepth-1; (long) index > 0; index--)
        {
          id=ColorToNodeId(q->red,q->green,q->blue,index);
          if ((node_info->census & (1 << id)) == 0)
            break;
          node_info=node_info->child[id];
        }
        /*
          Find closest color among siblings and their children.
        */
        cube_info->color.red=q->red;
        cube_info->color.green=q->green;
        cube_info->color.blue=q->blue;
        cube_info->distance=3.0*((double) MaxRGB+1.0)*((double) MaxRGB+1.0);
        ClosestColor(image,cube_info,node_info->parent);
        index=(IndexPacket) cube_info->color_number;
        for (i=0; i < count; i++)
        {
          if (image->storage_class == PseudoClass)
            indexes[x+i]=index;
          if (!cube_info->quantize_info->measure_error)
            {
              q->red=image->colormap[index].red;
              q->green=image->colormap[index].green;
              q->blue=image->colormap[index].blue;
            }
          q++;
        }
      }
      if (!SyncImagePixels(image))
        break;
      if (QuantumTick(y,image->rows))
        if (!MagickMonitor(AssignImageTag,y,image->rows,&image->exception))
          break;
    }
  if ((cube_info->quantize_info->number_colors == 2) &&
      (cube_info->quantize_info->colorspace == GRAYColorspace))
    {
      Quantum
        intensity;

      /*
        Monochrome image.
      */
      is_monochrome=True;
      q=image->colormap;
      for (i=(long) image->colors; i > 0; i--)
      {
        intensity=(Quantum) (PixelIntensityToQuantum(q) <
          (MaxRGB/2) ? 0 : MaxRGB);
        q->red=intensity;
        q->green=intensity;
        q->blue=intensity;
        q++;
      }
    }
  if (cube_info->quantize_info->measure_error)
    {
      (void) GetImageQuantizeError(image);
      SyncImage(image);
    }
  image->is_monochrome=is_monochrome;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C l a s s i f y I m a g e C o l o r s                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ClassifyImageColors() begins by initializing a color description tree
%  of sufficient depth to represent each possible input color in a leaf.
%  However, it is impractical to generate a fully-formed color
%  description tree in the storage_class phase for realistic values of
%  Cmax.  If colors components in the input image are quantized to k-bit
%  precision, so that Cmax= 2k-1, the tree would need k levels below the
%  root node to allow representing each possible input color in a leaf.
%  This becomes prohibitive because the tree's total number of nodes is
%  1 + sum(i=1,k,8k).
%
%  A complete tree would require 19,173,961 nodes for k = 8, Cmax = 255.
%  Therefore, to avoid building a fully populated tree, QUANTIZE: (1)
%  Initializes data structures for nodes only as they are needed;  (2)
%  Chooses a maximum depth for the tree as a function of the desired
%  number of colors in the output image (currently log2(colormap size)).
%
%  For each pixel in the input image, storage_class scans downward from
%  the root of the color description tree.  At each level of the tree it
%  identifies the single node which represents a cube in RGB space
%  containing It updates the following data for each such node:
%
%    n1 : Number of pixels whose color is contained in the RGB cube
%    which this node represents;
%
%    n2 : Number of pixels whose color is not represented in a node at
%    lower depth in the tree;  initially,  n2 = 0 for all nodes except
%    leaves of the tree.
%
%    Sr, Sg, Sb : Sums of the red, green, and blue component values for
%    all pixels not classified at a lower depth. The combination of
%    these sums and n2  will ultimately characterize the mean color of a
%    set of pixels represented by this node.
%
%    E: The distance squared in RGB space between each pixel contained
%    within a node and the nodes' center.  This represents the quantization
%    error for a node.
%
%  The format of the ClassifyImageColors() method is:
%
%      unsigned int ClassifyImageColorsCubeInfo *cube_info,const Image *image,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the Cube structure.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%
*/

static unsigned int ClassifyImageColors(CubeInfo *cube_info,const Image *image,
  ExceptionInfo *exception)
{
#define ClassifyImageTag  "Classify/Image"

  double
    bisect;

  DoublePixelPacket
    mid,
    pixel;

  long
    count,
    y;

  NodeInfo
    *node_info;

  register long
    x;

  register const PixelPacket
    *p;

  unsigned long
    index,
    level;

  unsigned int
    id;

  /*
    Classify the first 256 colors to a tree depth of 8.
  */
  for (y=0; (y < (long) image->rows) && (cube_info->colors < 256); y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      break;
    if (cube_info->nodes > MaxNodes)
      {
        /*
          Prune one level if the color tree is too large.
        */
        PruneLevel(cube_info,cube_info->root);
        cube_info->depth--;
      }
    for (x=0; x < (long) image->columns; x+=count)
    {
      /*
        Start at the root and descend the color cube tree.
      */
      for (count=1; (x+count) < (long) image->columns; count++)
        if (!ColorMatch(p,p+count))
          break;
      index=MaxTreeDepth-1;
      bisect=((double) MaxRGB+1.0)/2.0;
      mid.red=MaxRGB/2.0;
      mid.green=MaxRGB/2.0;
      mid.blue=MaxRGB/2.0;
      node_info=cube_info->root;
      for (level=1; level <= 8; level++)
      {
        bisect/=2;
        id=ColorToNodeId(p->red,p->green,p->blue,index);
        mid.red+=id & 4 ? bisect : -bisect;
        mid.green+=id & 2 ? bisect : -bisect;
        mid.blue+=id & 1 ? bisect : -bisect;
        if (node_info->child[id] == (NodeInfo *) NULL)
          {
            /*
              Set colors of new node to contain pixel.
            */
            node_info->census|=(1 << id);
            node_info->child[id]=GetNodeInfo(cube_info,id,level,node_info);
            if (node_info->child[id] == (NodeInfo *) NULL)
              ThrowException(exception,ResourceLimitError,
                "MemoryAllocationFailed","UnableToQuantizeImage");
            if (level == 8)
              cube_info->colors++;
          }
        /*
          Approximate the quantization error represented by this node.
        */
        node_info=node_info->child[id];
        pixel.red=p->red-mid.red;
        pixel.green=p->green-mid.green;
        pixel.blue=p->blue-mid.blue;
        node_info->quantize_error+=count*pixel.red*pixel.red+
          count*pixel.green*pixel.green+count*pixel.blue*pixel.blue;
        cube_info->root->quantize_error+=node_info->quantize_error;
        index--;
      }
      /*
        Sum RGB for this leaf for later derivation of the mean cube color.
      */
      node_info->number_unique+=count;
      node_info->total_red+=(double) count*p->red;
      node_info->total_green+=(double) count*p->green;
      node_info->total_blue+=(double) count*p->blue;
      p+=count;
    }
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(ClassifyImageTag,y,image->rows,exception))
        break;
  }
  if (y == (long) image->rows)
    return(True);
  /*
    More than 256 colors;  classify to the cube_info->depth tree depth.
  */
  PruneToCubeDepth(cube_info,cube_info->root);
  for ( ; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      break;
    if (cube_info->nodes > MaxNodes)
      {
        /*
          Prune one level if the color tree is too large.
        */
        PruneLevel(cube_info,cube_info->root);
        cube_info->depth--;
      }
    for (x=0; x < (long) image->columns; x+=count)
    {
      /*
        Start at the root and descend the color cube tree.
      */
      for (count=1; (x+count) < (long) image->columns; count++)
        if (!ColorMatch(p,p+count))
          break;
      index=MaxTreeDepth-1;
      bisect=((double) MaxRGB+1.0)/2.0;
      mid.red=MaxRGB/2.0;
      mid.green=MaxRGB/2.0;
      mid.blue=MaxRGB/2.0;
      node_info=cube_info->root;
      for (level=1; level <= cube_info->depth; level++)
      {
        bisect/=2;
        id=ColorToNodeId(p->red,p->green,p->blue,index);
        mid.red+=id & 4 ? bisect : -bisect;
        mid.green+=id & 2 ? bisect : -bisect;
        mid.blue+=id & 1 ? bisect : -bisect;
        if (node_info->child[id] == (NodeInfo *) NULL)
          {
            /*
              Set colors of new node to contain pixel.
            */
            node_info->census|=(1 << id);
            node_info->child[id]=GetNodeInfo(cube_info,id,level,node_info);
            if (node_info->child[id] == (NodeInfo *) NULL)
              ThrowException(exception,ResourceLimitError,
                "MemoryAllocationFailed","UnableToQuantizeImage");
            if (level == cube_info->depth)
              cube_info->colors++;
          }
        /*
          Approximate the quantization error represented by this node.
        */
        node_info=node_info->child[id];
        pixel.red=p->red-mid.red;
        pixel.green=p->green-mid.green;
        pixel.blue=p->blue-mid.blue;
        node_info->quantize_error+=count*pixel.red*pixel.red+
          count*pixel.green*pixel.green+count*pixel.blue*pixel.blue;
        cube_info->root->quantize_error+=node_info->quantize_error;
        index--;
      }
      /*
        Sum RGB for this leaf for later derivation of the mean cube color.
      */
      node_info->number_unique+=count;
      node_info->total_red+=(double) count*p->red;
      node_info->total_green+=(double) count*p->green;
      node_info->total_blue+=(double) count*p->blue;
      p+=count;
    }
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(ClassifyImageTag,y,image->rows,exception))
        break;
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e Q u a n t i z e I n f o                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneQuantizeInfo() makes a duplicate of the given quantize info structure,
%  or if quantize info is NULL, a new one.
%
%  The format of the CloneQuantizeInfo method is:
%
%      QuantizeInfo *CloneQuantizeInfo(const QuantizeInfo *quantize_info)
%
%  A description of each parameter follows:
%
%    o clone_info: Method CloneQuantizeInfo returns a duplicate of the given
%      quantize info, or if image info is NULL a new one.
%
%    o quantize_info: a structure of type info.
%
%
*/
MagickExport QuantizeInfo *CloneQuantizeInfo(const QuantizeInfo *quantize_info)
{
  QuantizeInfo
    *clone_info;

  clone_info=(QuantizeInfo *) AcquireMemory(sizeof(QuantizeInfo));
  if (clone_info == (QuantizeInfo *) NULL)
    MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
      "UnableToAllocateQuantizeInfo");
  GetQuantizeInfo(clone_info);
  if (quantize_info == (QuantizeInfo *) NULL)
    return(clone_info);
  clone_info->number_colors=quantize_info->number_colors;
  clone_info->tree_depth=quantize_info->tree_depth;
  clone_info->dither=quantize_info->dither;
  clone_info->colorspace=quantize_info->colorspace;
  clone_info->measure_error=quantize_info->measure_error;
  return(clone_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C l o s e s t C o l o r                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ClosestColor() traverses the color cube tree at a particular node and
%  determines which colormap entry best represents the input color.
%
%  The format of the ClosestColor method is:
%
%      void ClosestColor(Image *image,CubeInfo *cube_info,
%        const NodeInfo *node_info)
%
%  A description of each parameter follows.
%
%    o image: The image.
%
%    o cube_info: A pointer to the Cube structure.
%
%    o node_info: The address of a structure of type NodeInfo which points to a
%      node in the color cube tree that is to be pruned.
%
%
*/
static void ClosestColor(Image *image,CubeInfo *cube_info,
  const NodeInfo *node_info)
{
  register unsigned int
    id;

  /*
    Traverse any children.
  */
  if (node_info->census != 0)
    for (id=0; id < MaxTreeDepth; id++)
      if (node_info->census & (1 << id))
        ClosestColor(image,cube_info,node_info->child[id]);
  if (node_info->number_unique != 0)
    {
      double
        distance;

      DoublePixelPacket
        pixel;

      register PixelPacket
        *color;

      /*
        Determine if this color is "closest".
      */
      color=image->colormap+node_info->color_number;
      pixel.red=color->red-cube_info->color.red;
      distance=pixel.red*pixel.red;
      if (distance < cube_info->distance)
        {
          pixel.green=color->green-cube_info->color.green;
          distance+=pixel.green*pixel.green;
          if (distance < cube_info->distance)
            {
              pixel.blue=color->blue-cube_info->color.blue;
              distance+=pixel.blue*pixel.blue;
              if (distance < cube_info->distance)
                {
                  cube_info->distance=distance;
                  cube_info->color_number=node_info->color_number;
                }
            }
        }
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o m p r e s s I m a g e C o l o r m a p                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CompressImageColormap() compresses an image colormap by removing any
%  duplicate or unused color entries.
%
%  The format of the CompressImageColormap method is:
%
%      void CompressImageColormap(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport void CompressImageColormap(Image *image)
{
  QuantizeInfo
    quantize_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (!IsPaletteImage(image,&image->exception))
    return;
  GetQuantizeInfo(&quantize_info);
  quantize_info.number_colors=image->colors;
  quantize_info.tree_depth=8;
  (void) QuantizeImage(&quantize_info,image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e f i n e I m a g e C o l o r m a p                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DefineImageColormap() traverses the color cube tree and notes each colormap
%  entry.  A colormap entry is any node in the color cube tree where the
%  of unique colors is not zero.
%
%  The format of the DefineImageColormap method is:
%
%      DefineImageColormap(Image *image,NodeInfo *node_info)
%
%  A description of each parameter follows.
%
%    o image: The image.
%
%    o node_info: The address of a structure of type NodeInfo which points to a
%      node in the color cube tree that is to be pruned.
%
%
*/
static void DefineImageColormap(Image *image,NodeInfo *node_info)
{
  register unsigned int
    id;

  /*
    Traverse any children.
  */
  if (node_info->census != 0)
    for (id=0; id < MaxTreeDepth; id++)
      if (node_info->census & (1 << id))
        DefineImageColormap(image,node_info->child[id]);
  if (node_info->number_unique != 0)
    {
      register double
        number_unique;

      /*
        Colormap entry is defined by the mean color in this cube.
      */
      number_unique=node_info->number_unique;
      image->colormap[image->colors].red=(Quantum)
        (node_info->total_red/number_unique+0.5);
      image->colormap[image->colors].green=(Quantum)
        (node_info->total_green/number_unique+0.5);
      image->colormap[image->colors].blue=(Quantum)
        (node_info->total_blue/number_unique+0.5);
      node_info->color_number=image->colors++;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y C u b e I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyCubeInfo() deallocates memory associated with an image.
%
%  The format of the DestroyCubeInfo method is:
%
%      DestroyCubeInfo(CubeInfo *cube_info)
%
%  A description of each parameter follows:
%
%    o cube_info: The address of a structure of type CubeInfo.
%
%
*/
static void DestroyCubeInfo(CubeInfo *cube_info)
{
  register Nodes
    *nodes;

  /*
    Release color cube tree storage.
  */
  do
  {
    nodes=cube_info->node_queue->next;
    LiberateMemory((void **) &cube_info->node_queue);
    cube_info->node_queue=nodes;
  } while (cube_info->node_queue != (Nodes *) NULL);
  if (!cube_info->quantize_info->dither)
    return;
  LiberateMemory((void **) &cube_info->cache);
  LiberateMemory((void **) &cube_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y Q u a n t i z e I n f o                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyQuantizeInfo() deallocates memory associated with an QuantizeInfo
%  structure.
%
%  The format of the DestroyQuantizeInfo method is:
%
%      DestroyQuantizeInfo(QuantizeInfo *quantize_info)
%
%  A description of each parameter follows:
%
%    o quantize_info: Specifies a pointer to an QuantizeInfo structure.
%
%
*/
MagickExport void DestroyQuantizeInfo(QuantizeInfo *quantize_info)
{
  assert(quantize_info != (QuantizeInfo *) NULL);
  assert(quantize_info->signature == MagickSignature);
  LiberateMemory((void **) &quantize_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D i t h e r                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Dither() distributes the difference between an original image and the
%  corresponding color reduced algorithm to neighboring pixels along a Hilbert
%  curve.
%
%  The format of the Dither method is:
%
%      unsigned int Dither(CubeInfo *cube_info,Image *image,
%        const unsigned int direction)
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the Cube structure.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%    o direction:  This unsigned direction describes which direction
%      to move to next to follow the Hilbert curve.
%
*/
static unsigned int Dither(CubeInfo *cube_info,Image *image,
  const unsigned int direction)
{
  DoublePixelPacket
    error;

  IndexPacket
    index;

  PixelPacket
    pixel;

  register CubeInfo
    *p;

  register IndexPacket
    *indexes;

  register long
    i;

  register PixelPacket
    *q;

  p=cube_info;
  if ((p->x >= 0) && (p->x < (long) image->columns) &&
      (p->y >= 0) && (p->y < (long) image->rows))
    {
      /*
        Distribute error.
      */
      q=GetImagePixels(image,p->x,p->y,1,1);
      if (q == (PixelPacket *) NULL)
        return(False);
      indexes=GetIndexes(image);
      error.red=q->red;
      error.green=q->green;
      error.blue=q->blue;
      for (i=0; i < ExceptionQueueLength; i++)
      {
        error.red+=p->error[i].red*p->weights[i];
        error.green+=p->error[i].green*p->weights[i];
        error.blue+=p->error[i].blue*p->weights[i];
      }

      pixel.red=RoundSignedToQuantum(error.red);
      pixel.green=RoundSignedToQuantum(error.green);
      pixel.blue=RoundSignedToQuantum(error.blue);

      i=(pixel.blue >> CacheShift) << 12 | (pixel.green >> CacheShift) << 6 |
        (pixel.red >> CacheShift);
      if (p->cache[i] < 0)
        {
          register NodeInfo
            *node_info;

          register unsigned int
            id;

          /*
            Identify the deepest node containing the pixel's color.
          */
          node_info=p->root;
          for (index=MaxTreeDepth-1; (long) index > 0; index--)
          {
            id=ColorToNodeId(pixel.red,pixel.green,pixel.blue,index);
            if ((node_info->census & (1 << id)) == 0)
              break;
            node_info=node_info->child[id];
          }
          /*
            Find closest color among siblings and their children.
          */
          p->color.red=pixel.red;
          p->color.green=pixel.green;
          p->color.blue=pixel.blue;
          p->distance=3.0*((double) MaxRGB+1.0)*((double) MaxRGB+1.0);
          ClosestColor(image,p,node_info->parent);
          p->cache[i]=(long) p->color_number;
        }
      /*
        Assign pixel to closest colormap entry.
      */
      index=(IndexPacket) p->cache[i];
      if (image->storage_class == PseudoClass)
        *indexes=index;
      if (!cube_info->quantize_info->measure_error)
        {
          q->red=image->colormap[index].red;
          q->green=image->colormap[index].green;
          q->blue=image->colormap[index].blue;
        }
      if (!SyncImagePixels(image))
        return(False);
      /*
        Propagate the error as the last entry of the error queue.
      */
      for (i=0; i < (ExceptionQueueLength-1); i++)
        p->error[i]=p->error[i+1];
      p->error[i].red=pixel.red-(double) image->colormap[index].red;
      p->error[i].green=pixel.green-(double) image->colormap[index].green;
      p->error[i].blue=pixel.blue-(double) image->colormap[index].blue;
    }
  switch (direction)
  {
    case WestGravity: p->x--; break;
    case EastGravity: p->x++; break;
    case NorthGravity: p->y--; break;
    case SouthGravity: p->y++; break;
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D i t h e r I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DitherImage() distributes the difference between an original image and the
%  corresponding color reduced algorithm to neighboring pixels along a Hilbert
%  curve.  DitherImage returns True if the image is dithered otherwise False.
%
%  This algorithm is strongly based on a similar algorithm by Thiadmer
%  Riemersma.
%
%  The format of the DitherImage method is:
%
%      unsigned int DitherImage(CubeInfo *cube_info,Image *image)
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the Cube structure.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%
*/
static unsigned int DitherImage(CubeInfo *cube_info,Image *image)
{
  register long
    i;

  unsigned long
    depth;

  /*
    Initialize error queue.
  */
  for (i=0; i < ExceptionQueueLength; i++)
  {
    cube_info->error[i].red=0.0;
    cube_info->error[i].green=0.0;
    cube_info->error[i].blue=0.0;
  }
  /*
    Distribute quantization error along a Hilbert curve.
  */
  cube_info->x=0;
  cube_info->y=0;
  i=(long) (image->columns > image->rows ? image->columns : image->rows);
  for (depth=1; i != 0; depth++)
    i>>=1;
  HilbertCurve(cube_info,image,depth-1,NorthGravity);
  (void) Dither(cube_info,image,ForgetGravity);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t C u b e I n f o                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetCubeInfo() initialize the Cube data structure.
%
%  The format of the GetCubeInfo method is:
%
%      CubeInfo GetCubeInfo(const QuantizeInfo *quantize_info,
%        unsigned int depth)
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the Cube structure.
%
%    o quantize_info: Specifies a pointer to an QuantizeInfo structure.
%
%    o depth: Normally, this integer value is zero or one.  A zero or
%      one tells Quantize to choose a optimal tree depth of Log4(number_colors).
%      A tree of this depth generally allows the best representation of the
%      reference image with the least amount of memory and the fastest
%      computational speed.  In some cases, such as an image with low color
%      dispersion (a few number of colors), a value other than
%      Log4(number_colors) is required.  To expand the color tree completely,
%      use a value of 8.
%
%
*/
static CubeInfo *GetCubeInfo(const QuantizeInfo *quantize_info,
  unsigned long depth)
{
  CubeInfo
    *cube_info;

  double
    sum,
    weight;

  register long
    i;

  /*
    Initialize tree to describe color cube_info.
  */
  cube_info=(CubeInfo *) AcquireMemory(sizeof(CubeInfo));
  if (cube_info == (CubeInfo *) NULL)
    return((CubeInfo *) NULL);
  (void) memset(cube_info,0,sizeof(CubeInfo));
  if (depth > MaxTreeDepth)
    depth=MaxTreeDepth;
  if (depth < 2)
    depth=2;
  cube_info->depth=depth;
  /*
    Initialize root node.
  */
  cube_info->root=GetNodeInfo(cube_info,0,0,(NodeInfo *) NULL);
  if (cube_info->root == (NodeInfo *) NULL)
    return((CubeInfo *) NULL);
  cube_info->root->parent=cube_info->root;
  cube_info->quantize_info=quantize_info;
  if (!cube_info->quantize_info->dither)
    return(cube_info);
  /*
    Initialize dither resources.
  */
  cube_info->cache=(long *) AcquireMemory((1 << 18)*sizeof(long));
  if (cube_info->cache == (long *) NULL)
    return((CubeInfo *) NULL);
  /*
    Initialize color cache.
  */
  for (i=0; i < (1 << 18); i++)
    cube_info->cache[i]=(-1);
  /*
    Distribute weights along a curve of exponential decay.
  */
  weight=1.0;
  for (i=0; i < ExceptionQueueLength; i++)
  {
    cube_info->weights[ExceptionQueueLength-i-1]=1.0/weight;
    weight*=exp(log(((double) MaxRGB+1.0))/(ExceptionQueueLength-1.0));
  }
  /*
    Normalize the weighting factors.
  */
  weight=0.0;
  for (i=0; i < ExceptionQueueLength; i++)
    weight+=cube_info->weights[i];
  sum=0.0;
  for (i=0; i < ExceptionQueueLength; i++)
  {
    cube_info->weights[i]/=weight;
    sum+=cube_info->weights[i];
  }
  cube_info->weights[0]+=1.0-sum;
  return(cube_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t N o d e I n f o                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetNodeInfo() allocates memory for a new node in the color cube tree and
%  presets all fields to zero.
%
%  The format of the GetNodeInfo method is:
%
%      NodeInfo *GetNodeInfo(CubeInfo *cube_info,const unsigned int id,
%        const unsigned int level,NodeInfo *parent)
%
%  A description of each parameter follows.
%
%    o node: The GetNodeInfo method returns this integer address.
%
%    o id: Specifies the child number of the node.
%
%    o level: Specifies the level in the storage_class the node resides.
%
%
*/
static NodeInfo *GetNodeInfo(CubeInfo *cube_info,const unsigned int id,
  const unsigned int level,NodeInfo *parent)
{
  NodeInfo
    *node_info;

  if (cube_info->free_nodes == 0)
    {
      Nodes
        *nodes;

      /*
        Allocate a new nodes of nodes.
      */
      nodes=(Nodes *) AcquireMemory(sizeof(Nodes));
      if (nodes == (Nodes *) NULL)
        return((NodeInfo *) NULL);
      nodes->next=cube_info->node_queue;
      cube_info->node_queue=nodes;
      cube_info->next_node=nodes->nodes;
      cube_info->free_nodes=NodesInAList;
    }
  cube_info->nodes++;
  cube_info->free_nodes--;
  node_info=cube_info->next_node++;
  (void) memset(node_info,0,sizeof(NodeInfo));
  node_info->parent=parent;
  node_info->id=id;
  node_info->level=level;
  return(node_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  G e t I m a g e Q u a n t i z e E r r o r                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageQuantizeError() measures the difference between the original
%  and quantized images.  This difference is the total quantization error.
%  The error is computed by summing over all pixels in an image the distance
%  squared in RGB space between each reference pixel value and its quantized
%  value.  These values are computed:
%
%    o mean_error_per_pixel:  This value is the mean error for any single
%      pixel in the image.
%
%    o normalized_mean_square_error:  This value is the normalized mean
%      quantization error for any single pixel in the image.  This distance
%      measure is normalized to a range between 0 and 1.  It is independent
%      of the range of red, green, and blue values in the image.
%
%    o normalized_maximum_square_error:  Thsi value is the normalized
%      maximum quantization error for any single pixel in the image.  This
%      distance measure is normalized to a range between 0 and 1.  It is
%      independent of the range of red, green, and blue values in your image.
%
%
%  The format of the GetImageQuantizeError method is:
%
%      unsigned int GetImageQuantizeError(Image *image)
%
%  A description of each parameter follows.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%
*/
MagickExport unsigned int GetImageQuantizeError(Image *image)
{
  double
    distance,
    maximum_error_per_pixel,
    normalize;

  DoublePixelPacket
    pixel;

  IndexPacket
    index;

  long
    y;

  ErrorSumType
    total_error;

  register const PixelPacket
    *p;

  register IndexPacket
    *indexes;

  register long
    x;

  /*
    Initialize measurement.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  image->total_colors=GetNumberColors(image,(FILE *) NULL,&image->exception);
  memset(&image->error,0,sizeof(ErrorInfo));
  if (image->storage_class == DirectClass)
    return(True);
  /*
    For each pixel, collect error statistics.
  */
  maximum_error_per_pixel=0;
  total_error=0;
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (long) image->columns; x++)
    {
      index=indexes[x];
      pixel.red=p->red-(double) image->colormap[index].red;
      pixel.green=p->green-(double) image->colormap[index].green;
      pixel.blue=p->blue-(double) image->colormap[index].blue;
      distance=pixel.red*pixel.red+pixel.green*pixel.green+
        pixel.blue*pixel.blue;
      total_error+=distance;
      if (distance > maximum_error_per_pixel)
        maximum_error_per_pixel=distance;
      p++;
    }
  }
  /*
    Compute final error statistics.
  */
  normalize=3.0*((double) MaxRGB+1.0)*((double) MaxRGB+1.0);
  image->error.mean_error_per_pixel=total_error/image->columns/image->rows;
  image->error.normalized_mean_error=
    image->error.mean_error_per_pixel/normalize;
  image->error.normalized_maximum_error=maximum_error_per_pixel/normalize;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t Q u a n t i z e I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetQuantizeInfo() initializes the QuantizeInfo structure.
%
%  The format of the GetQuantizeInfo method is:
%
%      GetQuantizeInfo(QuantizeInfo *quantize_info)
%
%  A description of each parameter follows:
%
%    o quantize_info: Specifies a pointer to a QuantizeInfo structure.
%
%
*/
MagickExport void GetQuantizeInfo(QuantizeInfo *quantize_info)
{
  assert(quantize_info != (QuantizeInfo *) NULL);
  (void) memset(quantize_info,0,sizeof(QuantizeInfo));
  quantize_info->number_colors=256;
  quantize_info->dither=True;
  quantize_info->colorspace=RGBColorspace;
  quantize_info->signature=MagickSignature;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   H i l b e r t C u r v e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  HilbertCurve() s a space filling curve that visits every point in a square
%  grid with any power of 2.  Hilbert is useful in dithering due to the
%  coherence between neighboring pixels.  Here, the quantization error is
%  distributed along the Hilbert curve.
%
%  The format of the HilbertCurve method is:
%
%      void HilbertCurve(CubeInfo *cube_info,Image *image,
%        const unsigned long level,const unsigned int direction)
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the Cube structure.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%    o direction:  This unsigned direction describes which direction
%      to move to next to follow the Hilbert curve.
%
%
*/
static void HilbertCurve(CubeInfo *cube_info,Image *image,
  const unsigned long level,const unsigned int direction)
{
  if (level == 1)
    {
      switch (direction)
      {
        case WestGravity:
        {
          (void) Dither(cube_info,image,EastGravity);
          (void) Dither(cube_info,image,SouthGravity);
          (void) Dither(cube_info,image,WestGravity);
          break;
        }
        case EastGravity:
        {
          (void) Dither(cube_info,image,WestGravity);
          (void) Dither(cube_info,image,NorthGravity);
          (void) Dither(cube_info,image,EastGravity);
          break;
        }
        case NorthGravity:
        {
          (void) Dither(cube_info,image,SouthGravity);
          (void) Dither(cube_info,image,EastGravity);
          (void) Dither(cube_info,image,NorthGravity);
          break;
        }
        case SouthGravity:
        {
          (void) Dither(cube_info,image,NorthGravity);
          (void) Dither(cube_info,image,WestGravity);
          (void) Dither(cube_info,image,SouthGravity);
          break;
        }
        default:
          break;
      }
      return;
    }
  switch (direction)
  {
    case WestGravity:
    {
      HilbertCurve(cube_info,image,level-1,NorthGravity);
      (void) Dither(cube_info,image,EastGravity);
      HilbertCurve(cube_info,image,level-1,WestGravity);
      (void) Dither(cube_info,image,SouthGravity);
      HilbertCurve(cube_info,image,level-1,WestGravity);
      (void) Dither(cube_info,image,WestGravity);
      HilbertCurve(cube_info,image,level-1,SouthGravity);
      break;
    }
    case EastGravity:
    {
      HilbertCurve(cube_info,image,level-1,SouthGravity);
      (void) Dither(cube_info,image,WestGravity);
      HilbertCurve(cube_info,image,level-1,EastGravity);
      (void) Dither(cube_info,image,NorthGravity);
      HilbertCurve(cube_info,image,level-1,EastGravity);
      (void) Dither(cube_info,image,EastGravity);
      HilbertCurve(cube_info,image,level-1,NorthGravity);
      break;
    }
    case NorthGravity:
    {
      HilbertCurve(cube_info,image,level-1,WestGravity);
      (void) Dither(cube_info,image,SouthGravity);
      HilbertCurve(cube_info,image,level-1,NorthGravity);
      (void) Dither(cube_info,image,EastGravity);
      HilbertCurve(cube_info,image,level-1,NorthGravity);
      (void) Dither(cube_info,image,NorthGravity);
      HilbertCurve(cube_info,image,level-1,EastGravity);
      break;
    }
    case SouthGravity:
    {
      HilbertCurve(cube_info,image,level-1,EastGravity);
      (void) Dither(cube_info,image,NorthGravity);
      HilbertCurve(cube_info,image,level-1,SouthGravity);
      (void) Dither(cube_info,image,WestGravity);
      HilbertCurve(cube_info,image,level-1,SouthGravity);
      (void) Dither(cube_info,image,SouthGravity);
      HilbertCurve(cube_info,image,level-1,WestGravity);
      break;
    }
    default:
      break;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a p I m a g e                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MapImage() replaces the colors of an image with the closest color from a
%  reference image.
%
%  The format of the MapImage method is:
%
%      unsigned int MapImage(Image *image,const Image *map_image,
%        const unsigned int dither)
%
%  A description of each parameter follows:
%
%    o image: Specifies a pointer to an Image structure.
%
%    o map_image: Specifies a pointer to a Image structure.  Reduce
%      image to a set of colors represented by this image.
%
%    o dither: Set this integer value to something other than zero to
%      dither the quantized image.
%
%
*/
MagickExport unsigned int MapImage(Image *image,const Image *map_image,
  const unsigned int dither)
{
  CubeInfo
    *cube_info;

  QuantizeInfo
    quantize_info;

  unsigned int
    status;

  /*
    Initialize color cube.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(map_image != (Image *) NULL);
  assert(map_image->signature == MagickSignature);
  GetQuantizeInfo(&quantize_info);
  quantize_info.dither=dither;
  quantize_info.colorspace=
    image->matte ? TransparentColorspace : RGBColorspace;
  cube_info=GetCubeInfo(&quantize_info,8);
  if (cube_info == (CubeInfo *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      "UnableToMapImage");
  status=ClassifyImageColors(cube_info,map_image,&image->exception);
  if (status != False)
    {
      /*
        Classify image colors from the reference image.
      */
      quantize_info.number_colors=cube_info->colors;
      status=AssignImageColors(cube_info,image);
    }
  DestroyCubeInfo(cube_info);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a p I m a g e s                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MapImages() replaces the colors of a sequence of images with the closest
%  color from a reference image.
%
%  The format of the MapImage method is:
%
%      unsigned int MapImages(Image *images,Image *map_image,
%        const unsigned int dither)
%
%  A description of each parameter follows:
%
%    o image: Specifies a pointer to a set of Image structures.
%
%    o map_image: Specifies a pointer to a Image structure.  Reduce
%      image to a set of colors represented by this image.
%
%    o dither: Set this integer value to something other than zero to
%      dither the quantized image.
%
%
*/
MagickExport unsigned int MapImages(Image *images,const Image *map_image,
  const unsigned int dither)
{
  CubeInfo
    *cube_info;

  Image
    *image;

  QuantizeInfo
    quantize_info;

  unsigned int
    status;

  assert(images != (Image *) NULL);
  assert(images->signature == MagickSignature);
  GetQuantizeInfo(&quantize_info);
  quantize_info.dither=dither;
  image=images;
  if (map_image == (Image *) NULL)
    {
      /*
        Create a global colormap for an image sequence.
      */
      for ( ; image != (Image *) NULL; image=image->next)
        if (image->matte)
          quantize_info.colorspace=TransparentColorspace;
      status=QuantizeImages(&quantize_info,images);
      return(status);
    }
  /*
    Classify image colors from the reference image.
  */
  cube_info=GetCubeInfo(&quantize_info,8);
  if (cube_info == (CubeInfo *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      "UnableToMapImageSequence");
  status=ClassifyImageColors(cube_info,map_image,&image->exception);
  if (status != False)
    {
      /*
        Classify image colors from the reference image.
      */
      quantize_info.number_colors=cube_info->colors;
      for (image=images; image != (Image *) NULL; image=image->next)
      {
        quantize_info.colorspace=image->matte ? TransparentColorspace :
          RGBColorspace;
        status=AssignImageColors(cube_info,image);
        if (status == False)
          break;
      }
    }
  DestroyCubeInfo(cube_info);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   O r d e r e d D i t h e r I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  OrderedDitherImage() uses the ordered dithering technique of reducing color
%  images to monochrome using positional information to retain as much
%  information as possible.
%
%  The format of the OrderedDitherImage method is:
%
%      unsigned int OrderedDitherImage(Image *image)
%
%  A description of each parameter follows.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%
*/
MagickExport unsigned int OrderedDitherImage(Image *image)
{
#define DitherImageTag  "Dither/Image"

  static Quantum
    DitherMatrix[8][8] =
    {
      {   0, 192,  48, 240,  12, 204,  60, 252 },
      { 128,  64, 176, 112, 140,  76, 188, 124 },
      {  32, 224,  16, 208,  44, 236,  28, 220 },
      { 160,  96, 144,  80, 172, 108, 156,  92 },
      {   8, 200,  56, 248,   4, 196,  52, 244 },
      { 136,  72, 184, 120, 132,  68, 180, 116 },
      {  40, 232,  24, 216,  36, 228,  20, 212 },
      { 168, 104, 152,  88, 164, 100, 148,  84 }
    };

  IndexPacket
    index;

  long
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  /*
    Initialize colormap.
  */
  (void) NormalizeImage(image);
  if (!AllocateImageColormap(image,2))
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      "UnableToDitherImage");
  /*
    Dither image with the ordered dithering technique.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (long) image->columns; x++)
    {
      index=(Quantum) (PixelIntensityToQuantum(q) >
        ScaleCharToQuantum(DitherMatrix[y & 0x07][x & 0x07]) ? 1 : 0);
      indexes[x]=index;
      q->red=image->colormap[index].red;
      q->green=image->colormap[index].green;
      q->blue=image->colormap[index].blue;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(DitherImageTag,y,image->rows,&image->exception))
        break;
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   P r u n e C h i l d                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PruneChild() deletes the given node and merges its statistics into its
%  parent.
%
%  The format of the PruneSubtree method is:
%
%      PruneChild(CubeInfo *cube_info,const NodeInfo *node_info)
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the Cube structure.
%
%    o node_info: pointer to node in color cube tree that is to be pruned.
%
%
*/
static void PruneChild(CubeInfo *cube_info,const NodeInfo *node_info)
{
  NodeInfo
    *parent;

  register unsigned int
    id;

  /*
    Traverse any children.
  */
  if (node_info->census != 0)
    for (id=0; id < MaxTreeDepth; id++)
      if (node_info->census & (1 << id))
        PruneChild(cube_info,node_info->child[id]);
  /*
    Merge color statistics into parent.
  */
  parent=node_info->parent;
  parent->census&=~(1 << node_info->id);
  parent->number_unique+=node_info->number_unique;
  parent->total_red+=node_info->total_red;
  parent->total_green+=node_info->total_green;
  parent->total_blue+=node_info->total_blue;
  cube_info->nodes--;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  P r u n e L e v e l                                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PruneLevel() deletes all nodes at the bottom level of the color tree merging
%  their color statistics into their parent node.
%
%  The format of the PruneLevel method is:
%
%      PruneLevel(CubeInfo *cube_info,const NodeInfo *node_info)
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the Cube structure.
%
%    o node_info: pointer to node in color cube tree that is to be pruned.
%
%
*/
static void PruneLevel(CubeInfo *cube_info,const NodeInfo *node_info)
{
  register unsigned int
    id;

  /*
    Traverse any children.
  */
  if (node_info->census != 0)
    for (id=0; id < MaxTreeDepth; id++)
      if (node_info->census & (1 << id))
        PruneLevel(cube_info,node_info->child[id]);
  if (node_info->level == cube_info->depth)
    PruneChild(cube_info,node_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  P r u n e T o C u b e D e p t h                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PruneToCubeDepth() deletes any nodes ar a depth greater than
%  cube_info->depth while merging their color statistics into their parent
%  node.
%
%  The format of the PruneToCubeDepth method is:
%
%      PruneToCubeDepth(CubeInfo *cube_info,const NodeInfo *node_info)
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the Cube structure.
%
%    o node_info: pointer to node in color cube tree that is to be pruned.
%
%
*/
static void PruneToCubeDepth(CubeInfo *cube_info,const NodeInfo *node_info)
{
  register unsigned int
    id;

  /*
    Traverse any children.
  */
  if (node_info->census != 0)
    for (id=0; id < MaxTreeDepth; id++)
      if (node_info->census & (1 << id))
        PruneToCubeDepth(cube_info,node_info->child[id]);
  if (node_info->level > cube_info->depth)
    PruneChild(cube_info,node_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  Q u a n t i z e I m a g e                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  QuantizeImage() analyzes the colors within a reference image and chooses a
%  fixed number of colors to represent the image.  The goal of the algorithm
%  is to minimize the color difference between the input and output image while
%  minimizing the processing time.
%
%  The format of the QuantizeImage method is:
%
%      unsigned int QuantizeImage(const QuantizeInfo *quantize_info,
%        Image *image)
%
%  A description of each parameter follows:
%
%    o quantize_info: Specifies a pointer to an QuantizeInfo structure.
%
%    o image: Specifies a pointer to a Image structure.
%
*/
MagickExport unsigned int QuantizeImage(const QuantizeInfo *quantize_info,
  Image *image)
{
  CubeInfo
    *cube_info;

  unsigned int
    status;

  unsigned long
    depth,
    number_colors;

  assert(quantize_info != (const QuantizeInfo *) NULL);
  assert(quantize_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  number_colors=quantize_info->number_colors;
  if (number_colors == 0)
    number_colors=MaxColormapSize;
  if (number_colors > MaxColormapSize)
    number_colors=MaxColormapSize;
  /*
    For grayscale images, use a fast translation to PseudoClass,
    which assures that the maximum number of colors is equal to, or
    less than MaxColormapSize.
  */
  if (quantize_info->colorspace == GRAYColorspace)
    TransformColorspace(image,quantize_info->colorspace);
  if (IsGrayImage(image,&image->exception))
    GrayscalePseudoClassImage(image,True);
  /*
    If the image colors do not require further reduction, then simply
    return.
  */
  if ((image->storage_class == PseudoClass) &&
      (image->colors <= number_colors))
    return(True);
  depth=quantize_info->tree_depth;
  if (depth == 0)
    {
      unsigned long
        colors;

      /*
        Depth of color tree is: Log4(colormap size)+2.
      */
      colors=number_colors;
      for (depth=1; colors != 0; depth++)
        colors>>=2;
      if (quantize_info->dither)
        depth--;
      if (image->storage_class == PseudoClass)
        depth+=2;
    }
  /*
    Initialize color cube.
  */
  cube_info=GetCubeInfo(quantize_info,depth);
  if (cube_info == (CubeInfo *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      "UnableToQuantizeImage");
  if (quantize_info->colorspace != RGBColorspace)
    TransformColorspace(image,quantize_info->colorspace);
  status=ClassifyImageColors(cube_info,image,&image->exception);
  if (status != False)
    {
      /*
        Reduce the number of colors in the image.
      */
      ReduceImageColors(cube_info,number_colors,&image->exception);
      status=AssignImageColors(cube_info,image);
      if (quantize_info->colorspace != RGBColorspace)
        TransformColorspace(image,quantize_info->colorspace);
    }
  DestroyCubeInfo(cube_info);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Q u a n t i z e I m a g e s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  QuantizeImages() analyzes the colors within a set of reference images and
%  chooses a fixed number of colors to represent the set.  The goal of the
%  algorithm is to minimize the color difference between the input and output
%  images while minimizing the processing time.
%
%  The format of the QuantizeImages method is:
%
%      unsigned int QuantizeImages(const QuantizeInfo *quantize_info,
%        Image *images)
%
%  A description of each parameter follows:
%
%    o quantize_info: Specifies a pointer to an QuantizeInfo structure.
%
%    o images: Specifies a pointer to a list of Image structures.
%
%
*/
MagickExport unsigned int QuantizeImages(const QuantizeInfo *quantize_info,
  Image *images)
{
  CubeInfo
    *cube_info;

  int
    depth;

  MonitorHandler
    handler;

  Image
    *image;

  register long
    i;

  unsigned int
    status;

  unsigned long
    number_colors,
    number_images;

  assert(quantize_info != (const QuantizeInfo *) NULL);
  assert(quantize_info->signature == MagickSignature);
  assert(images != (Image *) NULL);
  assert(images->signature == MagickSignature);
  if (images->next == (Image *) NULL)
    {
      /*
        Handle a single image with QuantizeImage.
      */
      status=QuantizeImage(quantize_info,images);
      return(status);
    }
  status=False;
  image=images;
  number_colors=quantize_info->number_colors;
  if (number_colors == 0)
    number_colors=MaxColormapSize;
  if (number_colors > MaxColormapSize)
    number_colors=MaxColormapSize;
  depth=quantize_info->tree_depth;
  if (depth == 0)
    {
      int
        pseudo_class;

      unsigned long
        colors;

      /*
        Depth of color tree is: Log4(colormap size)+2.
      */
      colors=number_colors;
      for (depth=1; colors != 0; depth++)
        colors>>=2;
      if (quantize_info->dither)
        depth--;
      pseudo_class=True;
      for (image=images; image != (Image *) NULL; image=image->next)
        pseudo_class|=(image->storage_class == PseudoClass);
      if (pseudo_class)
        depth+=2;
    }
  /*
    Initialize color cube.
  */
  cube_info=GetCubeInfo(quantize_info,depth);
  if (cube_info == (CubeInfo *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      "UnableToQuantizeImageSequence");
  image=images;
  for (i=0; image != (Image *) NULL; i++)
  {
    if (quantize_info->colorspace != RGBColorspace)
      TransformColorspace(image,quantize_info->colorspace);
    image=image->next;
  }
  number_images=i;
  image=images;
  for (i=0; image != (Image *) NULL; i++)
  {
    handler=SetMonitorHandler((MonitorHandler) NULL);
    status=ClassifyImageColors(cube_info,image,&image->exception);
    if (status == False)
      break;
    image=image->next;
    (void) SetMonitorHandler(handler);
    if (!MagickMonitor(ClassifyImageTag,i,number_images,&image->exception))
      break;
  }
  if (status != False)
    {
      /*
        Reduce the number of colors in an image sequence.
      */
      ReduceImageColors(cube_info,number_colors,&image->exception);
      image=images;
      for (i=0; image != (Image *) NULL; i++)
      {
        handler=SetMonitorHandler((MonitorHandler) NULL);
        status=AssignImageColors(cube_info,image);
        if (status == False)
          break;
        if (quantize_info->colorspace != RGBColorspace)
          TransformColorspace(image,quantize_info->colorspace);
        image=image->next;
        (void) SetMonitorHandler(handler);
        if (!MagickMonitor(AssignImageTag,i,number_images,&image->exception))
          break;
      }
    }
  DestroyCubeInfo(cube_info);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e d u c e                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Reduce() traverses the color cube tree and prunes any node whose
%  quantization error falls below a particular threshold.
%
%  The format of the Reduce method is:
%
%      Reduce(CubeInfo *cube_info,const NodeInfo *node_info)
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the Cube structure.
%
%    o node_info: pointer to node in color cube tree that is to be pruned.
%
%
*/
static void Reduce(CubeInfo *cube_info,const NodeInfo *node_info)
{
  register unsigned int
    id;

  /*
    Traverse any children.
  */
  if (node_info->census != 0)
    for (id=0; id < MaxTreeDepth; id++)
      if (node_info->census & (1 << id))
        Reduce(cube_info,node_info->child[id]);
  if (node_info->quantize_error <= cube_info->pruning_threshold)
    PruneChild(cube_info,node_info);
  else
    {
      /*
        Find minimum pruning threshold.
      */
      if (node_info->number_unique > 0)
        cube_info->colors++;
      if (node_info->quantize_error < cube_info->next_threshold)
        cube_info->next_threshold=node_info->quantize_error;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e d u c e I m a g e C o l o r s                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReduceImageColors() repeatedly prunes the tree until the number of nodes
%  with n2 > 0 is less than or equal to the maximum number of colors allowed
%  in the output image.  On any given iteration over the tree, it selects
%  those nodes whose E value is minimal for pruning and merges their
%  color statistics upward. It uses a pruning threshold, Ep, to govern
%  node selection as follows:
%
%    Ep = 0
%    while number of nodes with (n2 > 0) > required maximum number of colors
%      prune all nodes such that E <= Ep
%      Set Ep to minimum E in remaining nodes
%
%  This has the effect of minimizing any quantization error when merging
%  two nodes together.
%
%  When a node to be pruned has offspring, the pruning procedure invokes
%  itself recursively in order to prune the tree from the leaves upward.
%  n2,  Sr, Sg,  and  Sb in a node being pruned are always added to the
%  corresponding data in that node's parent.  This retains the pruned
%  node's color characteristics for later averaging.
%
%  For each node, n2 pixels exist for which that node represents the
%  smallest volume in RGB space containing those pixel's colors.  When n2
%  > 0 the node will uniquely define a color in the output image. At the
%  beginning of reduction,  n2 = 0  for all nodes except a the leaves of
%  the tree which represent colors present in the input image.
%
%  The other pixel count, n1, indicates the total number of colors
%  within the cubic volume which the node represents.  This includes n1 -
%  n2  pixels whose colors should be defined by nodes at a lower level in
%  the tree.
%
%  The format of the ReduceImageColors method is:
%
%      ReduceImageColors(CubeInfo *cube_info,const unsigned int number_colors,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the Cube structure.
%
%    o number_colors: This integer value indicates the maximum number of
%      colors in the quantized image or colormap.  The actual number of
%      colors allocated to the colormap may be less than this value, but
%      never more.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
static void ReduceImageColors(CubeInfo *cube_info,
  const unsigned long number_colors,ExceptionInfo *exception)
{
#define ReduceImageTag  " Reduce/Image"

  unsigned int
    status;

  unsigned long
    span;

  span=cube_info->colors;
  cube_info->next_threshold=0.0;
  while (cube_info->colors > number_colors)
  {
    cube_info->pruning_threshold=cube_info->next_threshold;
    cube_info->next_threshold=cube_info->root->quantize_error-1;
    cube_info->colors=0;
    Reduce(cube_info,cube_info->root);
    status=MagickMonitor(ReduceImageTag,span-cube_info->colors,
      span-number_colors+1,exception);
    if (status == False)
      break;
  }
}
