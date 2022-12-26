.PHONY : all build clean help

all : build 

build : 
	cd src && make
	cd test && make
	cp -f src/reversi ./

clean : 
	rm -rf *.o reversi
	cd src && make clean
	cd test && make clean

help :
	@echo 'Makefile usage :'
	@echo ' . all: Run the target build;'
	@echo ' . build: Build the executable file reversi recursively in the directory src/'
	@echo ' . clean: Remove all files produced by compilation;'
	@echo ' . help: Display the main targets of this Makefile with a short description.'