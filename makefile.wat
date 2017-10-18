
all: .SYMBOLIC
	cd src && wmake -h /f makefile.wat

clean: .SYMBOLIC
	cd src && wmake -h /f makefile.wat clean

test: .SYMBOLIC
	cd src && wmake -h /f makefile.wat test
