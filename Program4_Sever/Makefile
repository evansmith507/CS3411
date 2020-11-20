all: r_server r_client pgm1 pgm2
r_server: r_server.c
	gcc -g -Wall -o r_server r_server.c
r_client: r_client.c r_client.h pgm.h
	gcc -g -Wall -c r_client.c
pgm1: r_client pgm1.c pgm.h
	gcc -g -Wall -o pgm1 pgm1.c r_client.o
pgm2: r_client pgm2.c pgm.h
	gcc -g -Wall -o pgm2 pgm2.c r_client.o
clean:
	rm r_client.o r_server pgm1 pgm2
submission:
	gtar czvf prog4.tgz Makefile r_server.c r_client.h r_client.c pgm1.c pgm2.c
