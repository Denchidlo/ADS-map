test:
	g++ test.cpp -o test.run
	./test.run
	rm -f test.run

memory_check:
	g++ -g test.cpp -o memtest.run
	valgrind ./memtest.run
	rm -f memtest.run