#-L /u/eng/class/classave/lib/ -lprofiler
default: lab2_list

build: lab2_list

lab2_list: lab2_list.c SortedList.c
	gcc -o lab2_list -g -lrt -pthread SortedList.c lab2_list.c


clean:
	rm -rf lab2_list lab2b-904582269.tar.gz $(filesclean)

files = README.txt Makefile lab2_list.c SortedList.h SortedList.c lab2b_graphs.gp lab2_add.csv lab_2b_list.csv lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png profile.gperf libprofiler.so.0 pprof 

filesclean = lab_2b_list.csv lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png profile.gperf
lab2b-904582269.tar.gz:
	tar -czvf $@ $(files)

tarball: lab2b-904582269.tar.gz

graphs: build
	gnuplot lab2b_graphs.gp
tests: build csv

cleancsv:
	rm -rf $(filesclean)

profile: build
	rm -rf ./raw.gperf
	LD_PRELOAD=./libprofiler.so.0 CPUPROFILE=./raw.gperf ./lab2_list --threads=12 --iterations=1000 --sync=s
	./pprof --text ./lab2_list ./raw.gperf > profile.gperf
	./pprof --list=thread_func2 ./lab2_list ./raw.gperf >> profile.gperf
	rm -f ./raw.gperf

csv:
	rm -rf lab_2b_list.csv
	# generate time per operation vs number of iterations
	./lab2_list --threads=1  --iterations=10              >> lab_2b_list.csv
	./lab2_list --threads=1  --iterations=100             >> lab_2b_list.csv
	./lab2_list --threads=1  --iterations=1000            >> lab_2b_list.csv
	./lab2_list --threads=1  --iterations=10000           >> lab_2b_list.csv
	./lab2_list --threads=1  --iterations=20000           >> lab_2b_list.csv
	# conflicts w/o yielding
	-./lab2_list --threads=2  --iterations=10             >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=100            >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=1000           >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=10             >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=100            >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=1000           >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=10             >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=100            >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=1000           >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=10             >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=100            >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=1000           >> lab_2b_list.csv
	# generate insert conflicts
	-./lab2_list --threads=2  --iterations=2   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=4   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=8   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=16  --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=32  --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=2   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=4   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=8   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=16  --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=2   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=4   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=8   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=16  --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=2   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=4   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=8   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=16  --yield=i  >> lab_2b_list.csv
	# generate delete conflicts
	-./lab2_list --threads=2  --iterations=2   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=4   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=8   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=16  --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=32  --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=2   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=4   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=8   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=16  --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=2   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=4   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=8   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=16  --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=2   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=4   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=8   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=16  --yield=d  >> lab_2b_list.csv
	# generate insert/lookup conflicts
	-./lab2_list --threads=2  --iterations=2   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=4   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=8   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=16  --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=32  --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=2   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=4   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=8   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=16  --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=2   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=4   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=8   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=16  --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=2   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=4   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=8   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=16  --yield=il >> lab_2b_list.csv
	# generate delete/lookup conflicts
	-./lab2_list --threads=2  --iterations=2   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=4   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=8   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=16  --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=32  --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=2   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=4   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=8   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=16  --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=2   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=4   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=8   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=16  --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=2   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=4   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=8   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=16  --yield=dl >> lab_2b_list.csv
	# generate insert/delete conflicts
	-./lab2_list --lists=4 --threads=1  --iterations=1   --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=1  --iterations=2   --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=1  --iterations=4   --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=1  --iterations=8   --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=1  --iterations=16  --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=4  --iterations=1   --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=4  --iterations=2   --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=4  --iterations=4   --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=4  --iterations=8   --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=4  --iterations=16  --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=8  --iterations=1   --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=8  --iterations=2   --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=8  --iterations=4   --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=8  --iterations=8   --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=8  --iterations=16  --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=12 --iterations=1   --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=12 --iterations=2   --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=12 --iterations=4   --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=12 --iterations=8   --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=12 --iterations=16  --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=16 --iterations=1   --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=16 --iterations=2   --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=16 --iterations=4   --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=16 --iterations=8   --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=16 --iterations=16  --yield=id >> lab_2b_list.csv
	# generate insert/delete conflicts with mutex
	-./lab2_list --lists=4 --threads=1  --iterations=10  --sync=m --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=1  --iterations=20  --sync=m --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=1  --iterations=40  --sync=m --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=1  --iterations=80  --sync=m --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=4  --iterations=10  --sync=m --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=4  --iterations=20  --sync=m --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=4  --iterations=40  --sync=m --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=4  --iterations=80  --sync=m --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=8  --iterations=10  --sync=m --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=8  --iterations=20  --sync=m --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=8  --iterations=40  --sync=m --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=8  --iterations=80  --sync=m --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=12 --iterations=10  --sync=m --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=12 --iterations=20  --sync=m --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=12 --iterations=40  --sync=m --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=12 --iterations=80  --sync=m --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=16 --iterations=10  --sync=m --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=16 --iterations=20  --sync=m --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=16 --iterations=40  --sync=m --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=16 --iterations=80  --sync=m --yield=id >> lab_2b_list.csv
	# generate insert/delete conflicts with spin locks
	-./lab2_list --lists=4 --threads=1  --iterations=10  --sync=s --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=1  --iterations=20  --sync=s --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=1  --iterations=40  --sync=s --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=1  --iterations=80  --sync=s --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=4  --iterations=10  --sync=s --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=4  --iterations=20  --sync=s --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=4  --iterations=40  --sync=s --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=4  --iterations=80  --sync=s --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=8  --iterations=10  --sync=s --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=8  --iterations=20  --sync=s --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=8  --iterations=40  --sync=s --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=8  --iterations=80  --sync=s --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=12 --iterations=10  --sync=s --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=12 --iterations=20  --sync=s --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=12 --iterations=40  --sync=s --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=12 --iterations=80  --sync=s --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=16 --iterations=10  --sync=s --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=16 --iterations=20  --sync=s --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=16 --iterations=40  --sync=s --yield=id >> lab_2b_list.csv
	-./lab2_list --lists=4 --threads=16 --iterations=80  --sync=s --yield=id >> lab_2b_list.csv
	# demonstrate the efficacy of mutex and spin-lock synchronization
	./lab2_list --threads=12 --iterations=32 --yield=i  --sync=m >> lab_2b_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=d  --sync=m >> lab_2b_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=il --sync=m >> lab_2b_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=dl --sync=m >> lab_2b_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=i  --sync=s >> lab_2b_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=d  --sync=s >> lab_2b_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=il --sync=s >> lab_2b_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=dl --sync=s >> lab_2b_list.csv
	# measure the performance
	./lab2_list --threads=1  --iterations=1000          >> lab_2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s >> lab_2b_list.csv
	#lab2b graphs lab2b_4
	#./lab2_list --threads=1  --iterations=1000 --sync=m >> lab_2b_list.csv
	#./lab2_list --threads=2  --iterations=1000 --sync=m >> lab_2b_list.csv
	#./lab2_list --threads=4  --iterations=1000 --sync=m >> lab_2b_list.csv
	#./lab2_list --threads=8  --iterations=1000 --sync=m >> lab_2b_list.csv
	#./lab2_list --threads=12 --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --lists=4  --threads=1  --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --lists=4  --threads=2  --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --lists=4  --threads=4  --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --lists=4  --threads=8  --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --lists=4  --threads=12 --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --lists=8  --threads=1  --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --lists=8  --threads=2  --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --lists=8  --threads=4  --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --lists=8  --threads=8  --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --lists=8  --threads=12 --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --lists=16 --threads=1  --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --lists=16 --threads=2  --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --lists=16 --threads=4  --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --lists=16 --threads=8  --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --lists=16 --threads=12 --iterations=1000 --sync=m >> lab_2b_list.csv
	#lab2b graphs lab2b_5
	#./lab2_list --threads=1  --iterations=1000 --sync=s >> lab_2b_list.csv
	#./lab2_list --threads=2  --iterations=1000 --sync=s >> lab_2b_list.csv
	#./lab2_list --threads=4  --iterations=1000 --sync=s >> lab_2b_list.csv
	#./lab2_list --threads=8  --iterations=1000 --sync=s >> lab_2b_list.csv
	#./lab2_list --threads=12 --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --lists=4  --threads=1  --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --lists=4  --threads=2  --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --lists=4  --threads=4  --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --lists=4  --threads=8  --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --lists=4  --threads=12 --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --lists=8  --threads=1  --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --lists=8  --threads=2  --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --lists=8  --threads=4  --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --lists=8  --threads=8  --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --lists=8  --threads=12 --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --lists=16 --threads=1  --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --lists=16 --threads=2  --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --lists=16 --threads=4  --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --lists=16 --threads=8  --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --lists=16 --threads=12 --iterations=1000 --sync=s >> lab_2b_list.csv
