.PHONY:	all view print

all:	zx62m-b-5p.mod

zx62m-b-5p.mod:	zx62m-b-5p.fpd
	fped -k zx62m-b-5p.fpd

view:
	./kicad-nxn.pl idbg-Component.ps | gv -

print:
	./kicad-nxn.pl idbg-Component.ps | lpr
