default: lab3a

lab3a: lab3a.c
	gcc -o $@ -g -Wall lab3a.c

clean: lab3a
	rm -rf lab3a super.csv group.csv bitmap.csv inode.csv directory.csv indirect.csv lab3a-904582269.tar.gz

td: lab3a
	./lab3a disk-image

mt: td
	diff -u super.csv ./t/super.csv
	diff -u group.csv ./t/group.csv
	diff -u bitmap.csv ./t/bitmap.csv
	diff -u inode.csv ./t/inode.csv
	diff -u directory.csv ./t/directory.csv
	diff -u indirect.csv ./t/indirect.csv
dist: lab3a.c Makefile README
	tar -czvf lab3a-904582269.tar.gz lab3a.c Makefile README
