$!
$! Make ImageMagick X image utilities for VMS.
$!
$
$if (f$trnlnm("X11") .eqs. "") then define/nolog X11 decw$include:
$compile_options="/nodebug/optimize"
$if (f$search("sys$system:decc$compiler.exe") .nes. "") 
$then     ! VAX with DEC C
$  compile_options="/decc/nodebug/optimize/warning=(disable=rightshiftovr)"
$else     ! VAX with VAX C
$define/nolog lnk$library sys$library:vaxcrtl
$define/nolog sys sys$share
$endif
$if (f$getsyi("HW_MODEL") .gt. 1023)
$then     ! Alpha with DEC C
$  define/nolog sys decc$library_include
$  compile_options="/nodebug/optimize/prefix=all"
$endif
$
$write sys$output "Making Magick..."
$call Make PreRvIcccm.c
$call Make animate.c
$call Make annotate.c
$call Make attributes.c
$call Make avs.c
$call Make bim.c
$call Make blob.c
$call Make bmp.c
$call Make cache.c
$call Make cache_view.c
$call Make cmyk.c
$call Make colors.c
$call Make compress.c
$call Make constitute.c
$call Make dcm.c
$call Make decorate.c
$call Make delegates.c
$call Make display.c
$call Make dps.c
$call Make draw.c
$call Make effects.c
$call Make enhance.c
$call Make ept.c
$call Make error.c
$call Make fax.c
$call Make fits.c
$call Make fpx.c
$call Make gems.c
$call Make gif.c
$call Make gradation.c
$call Make gray.c
$call Make hdf.c
$call Make histogram.c
$call Make html.c
$call Make icc.c
$call Make icon.c
$call Make image.c
$call Make iptc.c
$call Make jbig.c
$call Make jpeg.c
$call Make label.c
$call Make logo.c
$call Make magick.c
$call Make map.c
$call Make matte.c
$call Make memory.c
$call Make miff.c
$call Make monitor.c
$call Make mono.c
$call Make montage.c
$call Make mtv.c
$call Make pcd.c
$call Make pdb.c
$call Make pcl.c
$call Make pcx.c
$call Make pdf.c
$call Make pict.c
$call Make pix.c
$call Make pixel_cache.c
$call Make plasma.c
$call Make png.c
$call Make pnm.c
$call Make preview.c
$call Make ps.c
$call Make ps2.c
$call Make ps3.c
$call Make psd.c
$call Make pwp.c
$call Make quantize.c
$call Make rgb.c
$call Make rla.c
$call Make rle.c
$call Make segment.c
$call Make sct.c
$call Make sfw.c
$call Make sgi.c
$call Make shear.c
$call Make signature.c
$call Make stegano.c
$call Make stream.c
$call Make sun.c
$call Make svg.c
$call Make tga.c
$call Make tiff.c
$call Make tile.c
$call Make tim.c
$call Make timer.c
$call Make transform.c
$call Make ttf.c
$call Make txt.c
$call Make uil.c
$call Make utility.c
$call Make uyvy.c
$call Make vicar.c
$call Make vid.c
$call Make viff.c
$call Make vms.c
$call Make wbmp.c
$call Make widget.c
$call Make x.c
$call Make xbm.c
$call Make xc.c
$call Make xpm.c
$call Make xwd.c
$call Make xwindows.c
$call Make yuv.c
$call Make zoom.c
$call Make 8bim.c
$library/create libmagick.olb PreRvIcccm,animate,annotate,-
  attributes,avs,bim,blob,bmp,cache,cache_view,cmyk,colors,compress,-
  constitute,dcm,decorate,delegates,display,dps,draw,effects,-
  enhance,ept,error,fax,fits,fpx,gems,gif,gradation,gray,-
  hdf, histogram,html,icc,icon,image,iptc,jbig,jpeg,label,-
  logo,magick, map,matte,memory,miff,monitor,mono,montage,mtv,-
  pdb,pcd,pcl,pcx, pdf,pict,pix,pixel_cache,plasma,png,pnm,-
  preview,ps,ps2,ps3,psd,pwp,quantize,rgb,rla,rle,segment,-
  sct,sfw,sgi,shear,signature,stegano,stream,sun,svg,tga,tiff,-
  tile,tim,timer,transform,ttf,txt,uil,utility,uyvy,vicar,-
  vid,viff,vms,widget,wbmp,x,xbm,xc,xpm,xwd,xwindows,yuv,-
  zoom,8bim
$exit
$
$Make: subroutine
$!
$! Primitive MMS hack for DCL.
$!
$if (p1 .eqs. "") then exit
$source_file=f$search(f$parse(p1,".c"))
$if (source_file .nes. "")
$then
$  object_file=f$parse(source_file,,,"name")+".obj"
$  object_file=f$search( object_file )
$  if (object_file .nes. "")
$  then
$    object_time=f$file_attribute(object_file,"cdt")
$    source_time=f$file_attribute(source_file,"cdt")
$    if (f$cvtime(object_time) .lts. f$cvtime(source_time)) then -
$      object_file=""
$  endif
$  if (object_file .eqs. "")
$  then
$    write sys$output "Compiling ",p1
$    cc'compile_options'/include_directory=([-],[-.jpeg],[-.png], -
       [-.tiff],[-.ttf],[-.zlib]) 'source_file'  
$  endif
$endif
$exit
$endsubroutine
