CompilerFlag = -std=c11 -pendantic -Wall -Werror -D_XOPEN_SOURCE=700

.Phony: all clean 

all: clash

clean:
  rm -f clash clash.o plist.o
  
clash: clash.o plist.o
  gcc $(CompilerFlag) -o clash clash.o plist.o
 
clash.o: clash.c plist.h
  gcc $(CompilerFlag) -c clash.c 
  
plist.o: plist.c plist.h
  gcc $(CompilerFlag) -c plist.c 
