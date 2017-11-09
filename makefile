INCD=\
	D:\Programs\mingw64\lib\gcc\x86_64-w64-mingw32\7.1.0\include \
	D:\Programs\mingw64\include
INCD_P=$(foreach d, $(INCD), -I$d)

INC= 
INC_P=$(foreach i, $(INC), -l$i)

main: main.c
	gcc main.c -o main  $(INCD_P) $(INC_P) -g
	./main foo.txt