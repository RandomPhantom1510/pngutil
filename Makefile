CC=gcc

pngutil: pngutil.c tests.c
	$(CC) -o pngutil pngutil.c tests.c
