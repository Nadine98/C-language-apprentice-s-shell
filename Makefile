CompilerFlag = -std=c11 -pedantic -Wall -Werror -D_XOPEN_SOURCE=700

.Phony: all clean 

all: clash

clean:
	rm -rf  *.o clash
  
clash: clash.o plist.o
	gcc $(CompilerFlag) -o clash clash.o plist.o
 
clash.o: clash.c plist.h
	gcc $(CompilerFlag) -c clash.c 
  
plist.o: plist.c plist.h
	gcc $(CompilerFlag) -c plist.c 
