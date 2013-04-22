top_srcdir              ?= ../..

include $(top_srcdir)/include/mk/env_pre.mk

CFLAGS		+=	-O2 -Wall -W -Wstrict-prototypes
resched		=	resched
getsched	=	getsched
sched 		=	sched
chrt		=	chrt
ionice		=	ionice
taskset		=	taskset

MAKE_TARGETS    := ${getsched} ${resched} ${sched} ${chrt} ${ionice} ${taskset}

getsched: getsched.o util.o config.h
	${CC} ${LIBS} ${LDFLAGS} -o ${getsched} getsched.o util.o 
sched: sched.o util.o isnumber.o
	${CC} ${LIBS} ${LDFLAGS} -o ${sched} sched.o util.o isnumber.o
resched: resched.o util.o isnumber.o
	${CC} ${LIBS} ${LDFLAGS} -o ${resched} resched.o util.o isnumber.o
util.o: util.c util.h config.h
	${CC} ${CFLAGS} ${LDFLAGS} -o  util.o -c util.c
isnumber.o: isnumber.c isnumber.h
	${CC} ${CFLAGS} ${LDFLAGS} -o  isnumber.o -c isnumber.c

resched.o: resched.c config.h util.h isnumber.h
	${CC} ${CFLAGS} ${LDFLAGS} -o  resched.o -c resched.c
sched.o: sched.c util.h config.h 
	${CC} ${CFLAGS} ${LDFLAGS} -o  sched.o -c sched.c
getsched.o: getsched.c util.h config.h 
	${CC} ${CFLAGS} ${LDFLAGS} -o  getsched.o -c getsched.c

chrt: chrt.c
	$(CC) $(CFLAGS) ${LDFLAGS} -DVERSION=\"$(ver)\" -o ${chrt} chrt.c
ionice: ionice.c
	$(CC) $(CFLAGS) ${LDFLAGS} -DVERSION=\"$(ver)\" -o ${ionice} ionice.c
taskset: taskset.c
	$(CC) $(CFLAGS) ${LDFLAGS} -DVERSION=\"$(ver)\" -o ${taskset} taskset.c


include $(top_srcdir)/include/mk/generic_leaf_target.mk
