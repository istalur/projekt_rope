#==============================================================================
#                   Makefile dla programu main
#==============================================================================


.SUFFIXES: .c .cpp .o .x .h



FILES = main.cpp lina.cpp
OBJ = main.o lina.o


#------------------------------------------------------------------------------
# Opcje kompilatora i linkera
CFLAGS = -Wall -O -DLINUX `sdl-config --cflags`
LFLAGS = `sdl-config --libs` -L/usr/X11R6/lib -lglut -lGL -lGLU




# Kompilator i linker 
CO = g++
LD = $(CO)


#regula niejawna tworzenia programu
%.x: %.cpp
	$(CO) $(CFLAGS) -o$@  $< $(LFLAGS)

# regula niejawna tworzenia plików obiektowych
.cpp.o:
	$(CO) $(CFLAGS) -c $<

#wykonanie kompilacji
all: $(OBJ)
	g++ -o main.x  $^ $(LFLAGS)


#uruchomienie programu 
.PHONY: run
run: all
	./main.x



#usuniecie pliku 
.PHONY: clean
clean: 
	rm -f *.x *.o
	


