
#MYCC = arm-hismall-linux-gcc
MYCC = gcc
LIBS = -lNetLib
CFLAG = -c -O2 -Wall -DHI_OS_LINUX -lpthread
NAME = Nettest

ALL_OBJ = \
	hi_test.o

all:	${NAME} 
#cp

${NAME}: ${ALL_OBJ}
	${MYCC} -o $@ -L. ${ALL_OBJ} ${LIBS}

%.o: %.c
	${MYCC} -o $@ ${CFLAG} $<

cp:
	cp -f ${NAME} /mnt/hgfs/share

clean:
	rm -f ${NAME}
	rm -f *.o
