CC = gcc

default: lab0

lab0: lab0.c
	$(CC) lab0.c -g -o $@

tests = checkinput checkoutput checksegfault checkcatch

check: $(tests)
	count=0; \
	cat check.txt | while read line; do \
		if [ "$$line" = "success" ]; then \
			count=$$((count+1)); \
			echo "$$count out of 4 smoke test passed"; \
		fi; \
		if [ $$count -eq 4 ]; then \
			echo "all smoke tests passed"; \
		fi; \
	done; \
	rm check.txt; 

ball = lab0-904582269.tar.gz

dist: $(ball)
files = lab0.c part5backtrace.png part6nullptr.png Makefile README

lab0-904582269.tar.gz: $(files) 
	tar -czvf $@ $(files) 

clean:
	rm lab0 $(ball)

checkinput:
	echo "blah blah cat master" > newfile.txt; \
	./lab0 --input=newfile.txt; \
	if [ $$? -eq 0 ]; then \
	echo "success" >> check.txt; \
	else \
	echo "fail" >> check.txt; \
	fi;
	rm newfile.txt

checkoutput:
	touch out.txt
	echo "blah blah cat master" > in.txt
	./lab0 --input in.txt --output out.txt
	cat out.txt | while read line; do \
		if [ "$$line" = "blah blah cat master" ]; then \
		echo "success" >> check.txt; \
		else \
		echo "fail" >> check.txt; \
		fi; \
	done; \
	rm in.txt out.txt

checksegfault:
	./lab0 --segfault; \
	if [ $$? -eq 139 ]; then \
	echo "success" >> check.txt; \
	else \
	echo "fail" >> check.txt; \
	fi;

checkcatch:
	./lab0 --segfault --catch; \
	if [ $$? -eq 3 ]; then \
	echo "success" >> check.txt; \
	else \
	echo "fail" >> check.txt; \
	fi;

