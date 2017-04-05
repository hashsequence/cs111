default: simpsh

simpsh: lab1a.c
	gcc -g -std=c11 -Wall -o simpsh lab1a.c

clean: 
	rm -rf simpsh lab1-averywong.tar.gz

dir = lab1-averywong/
ball = simpsh check lab1-averywong.tar.gz 
files = $(dir)Makefile $(dir)lab1a.c $(dir)testscript.sh $(dir)README

dist: $(ball) 

lab1-averywong.tar.gz: Makefile lab1a.c testscript.sh README
	mkdir lab1-averywong; \
	cp Makefile lab1a.c testscript.sh README ./lab1-averywong; \
	tar -czvf $@ $(files); \
	rm -rf lab1-averywong
check: simpsh
	./testscript.sh
