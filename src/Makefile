release=

RM=rm -f

DYNAMIC_COMPILE_OPT=-fPIC
DYNAMIC_LINK_OPT=-shared
COMILE_OPT=-fvisibility=hidden
MACROS=-D__LOG_LEVEL__=3 

ifndef release
VER_OPT=-g -Wall 
else
VER_OPT=-s -O3 -Wall
endif

CC=g++ $(VER_OPT) $(COMILE_OPT) $(MACROS) 

dyn_libname:=libopcrules.so
test_prog:=bin_test
inc_dir=
lib_dir=
libs=-lm -lrt

lib_srcs=libexport.cpp coordtype.cpp nodetree.cpp utiltime.cpp \
		edgefilter.cpp opcpinchrule.cpp	writer.cpp nodedeal.cpp 
lib_objs=$(lib_srcs:.cpp=.o)

test_srcs = main.cpp test02.cpp test01.cpp
test_objs = $(test_srcs:.cpp=.o)

all_objs = $(lib_objs) $(test_objs)

default:all
.PHONY:default

all:lib prog
.PHONY:all

lib:$(dyn_libname)
.PHONY:lib

prog:$(test_prog)
.PHONY:prog

$(test_prog):$(test_objs) $(dyn_libname)
	$(CC) -o $@ $(test_objs) $(lib_dir) $(dyn_libname) $(libs)

$(dyn_libname):$(lib_objs)
	$(CC) $(DYNAMIC_LINK_OPT) -o $@  $^ $(lib_dir) $(libs)

$(lib_objs):%.o:%.cpp
	$(CC) $(DYNAMIC_COMPILE_OPT) -c -o $@ $< $(inc_dir)

$(test_objs):%.o:%.cpp
	$(CC) -c -o $@ $< $(inc_dir)	
	
clean:
	-@$(RM)  $(all_objs) $(test_prog) $(dyn_libname)
.PHONY:clean


