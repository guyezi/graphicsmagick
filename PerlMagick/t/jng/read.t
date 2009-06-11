#!/usr/local/bin/perl
# Copyright (C) 2003 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#
# Test read image method on non-interlaced JPEG.
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1; print "1..11\n"; }
END {print "not ok $test\n" unless $loaded;}
use Graphics::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/jng' || die 'Cd failed';

#
# 1) Gray
# 
testReadCompare('input_gray.jng', '../reference/jng/read_gray.miff', q//, 0, 0);
#
# 2) Gray with IDAT encoding
# 
++$test;
testReadCompare('input_gray_idat.jng', '../reference/jng/read_gray_idat.miff', q//, 0, 0);
#
# 3) Gray with JDAA encoding
# 
++$test;
testReadCompare('input_gray_jdaa.jng', '../reference/jng/read_gray_jdaa.miff', q//, 0, 0);
#
# 4) Gray Progressive
# 
++$test;
testReadCompare('input_gray_prog.jng', '../reference/jng/read_gray_prog.miff', q//, 0, 0);
#
# 5) Gray progressive with IDAT encoding
# 
++$test;
testReadCompare('input_gray_prog_idat.jng', '../reference/jng/read_gray_prog_idat.miff', q//, 0, 0);
#
# 6) Gray progressive with JDAA encoding
# 
++$test;
testReadCompare('input_gray_prog_jdaa.jng', '../reference/jng/read_gray_prog_jdaa.miff', q//, 0, 0);
#
# 7) Color with JDAA encoding
# 
++$test;
testReadCompare('input_idat.jng', '../reference/jng/read_idat.miff', q//, 0.009, 0.13);
#
# 8) Color with JDAA encoding
# 
++$test;
testReadCompare('input_jdaa.jng', '../reference/jng/read_jdaa.miff', q//, 0.009, 0.13);
#
# 9) Color progressive
# 
++$test;
testReadCompare('input_prog.jng', '../reference/jng/read_prog.miff', q//, 0.009, 0.13);
#
# 10) Color progressive with IDAT encoding
# 
++$test;
testReadCompare('input_prog_idat.jng', '../reference/jng/read_prog_idat.miff', q//, 0.009, 0.13);
#
# 11) Color progressive with JDAA encoding
# 
++$test;
testReadCompare('input_prog_jdaa.jng', '../reference/jng/read_prog_jdaa.miff', q//, 0.009, 0.13);
