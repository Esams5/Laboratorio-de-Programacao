all: prog.e

prog.e: codigoPGM.o funcoes.o funcoes2.o
	gcc -o prog.e codigoPGM.o funcoes.o funcoes2.o

codigoPGM.o: codigoPGM.c
	gcc -o codigoPGM.o -c codigoPGM.c

funcoes.o: funcoes.c 
	gcc -o funcoes.o -c funcoes.c 

funcoes2.o: funcoes2.c 
	gcc -o funcoes2.o -c funcoes2.c

clean:
	rm -f *.o prog.e
