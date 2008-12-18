#!/usr/bin/perl
#
# This little script automates the process of placing an NxM array of layouts
# on a board of given size.
#

#
# Known issues:
# - only tested with a very simple board that has nothing but lines.
#   More complex ones should also have circles.
# - doesn't take the trace width into account
# - should calculate nx and ny automatically and then maximize borders
# - doesn't know space-optimizing tricks like rotation or diagonal offset
# - should support other paper formats
#

# array size

$nx = 1;
$ny = 2;

# border around layouts, in mm

$b_left = 2;
$b_right = 2;
$b_top = 2;
$b_bottom = 2;

# bounding box (PCB) size, in mm

$bw = 50;
$bh = 50;

# page size, in mm (A4 portrait)

$sx = 210;
$sy = 297;


while (<>) {
    if (/^(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+line/) {
	($x0, $x1) = $1 < $3 ? ($1, $3) : ($3, $1);
	($y0, $y1) = $2 < $4 ? ($2, $4) : ($4, $2);
	$xm0 = $x0 if $x0 < $xm0 || !defined $xm0;
	$ym0 = $y0 if $y0 < $ym0 || !defined $ym0;
	$xm1 = $x1 if $x1 > $xm1 || !defined $xm1;
	$ym1 = $y1 if $y1 > $ym1 || !defined $ym1;
    }
    if (/^%/) {
	($t = $_) =~ s/Landscape/Portrait/;
	$pre .= $t;
    } else {
	$ps .= $_ unless /showpage|rotate/;
    }
}

# kicad's unit is decimil. convert to mm.

for $v (*xm0, *xm1, *ym0, *ym1) {
    *t = $v;
    $t *= 25.4e-4;
}

# width and height

$w = $xm1-$xm0;
$h = $ym1-$ym0;

# step size

$dx = $w+$b_left+$b_right;
$dy = $h+$b_top+$b_bottom;

# offset

$ox = $sx/2-$xm0+$b_left;
$oy = $sy/2-$ym0+$b_top;

print "$pre\n";
for ($y = 0; $y != $ny; $y++) {
    for ($x = 0; $x != $nx; $x++) {
	$px = $ox+($x-$nx/2)*$dx;
	$py = $oy+($y-$ny/2)*$dy;
	print "gsave\n";
	if (1) {
	    print "-1 1 scale\n";	# reverse for toner transfer method
	    $px -= $xm0+$xm1;
	}
	# Postscript units
	$px = $px/25.4*72;
	$py = $py/25.4*72;
	print "$px $py translate\n";
	print "$ps\n";
	print "grestore\n";
    }
}

# switch to mm
print 72/25.4, " ", 72/25.4, " scale\n";
# origin is at center
print $sx/2, " ", $sy/2, " translate\n";

print "1 setlinewidth\n";
# grow bounding box so that inner border has the desired size
$bw += 1;
$bh += 1;
print -$bw/2, " ", -$bh/2, " moveto\n";
print "$bw 0 rlineto\n";
print "0 $bh rlineto\n";
print "-$bw 0 rlineto\n";
print "0 -$bh rlineto\n";
print "stroke\n";

print "0.1 setlinewidth\n";
for ($x = 1; $x != $nx; $x++) {
    print $dx*($x-$nx/2), " ", -$bh/2, " moveto\n";
    print "0 $bh rlineto stroke\n";
}
for ($y = 1; $y != $ny; $y++) {
    print -$bw/2, " ", $dy*($y-$ny/2), " moveto\n";
    print "$bw 0 rlineto stroke\n";
}

print "0 ", $bh/2+5, " moveto\n";
print "0 10 rlineto\n";
print "-3 0 rmoveto\n";
print "3 5 rlineto 3 -5 rlineto\n";
print "stroke\n";

print "showpage\n%%EOF\n";
