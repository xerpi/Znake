OUT=Znake
SOURCES=src
INCLUDES=include
ODIR = obj

CFILES= $(SOURCES)/znake.c main.c
OBJS=  $(addprefix $(ODIR)/, $(notdir $(CFILES:.c=.o)))
LIBS = -lncurses
CFLAGS = 

$(OUT): $(OBJS)
	gcc $(OBJS) -o $(OUT) $(LIBS)
	

$(ODIR)/%.o: $(SOURCES)/%.c
	gcc -I$(INCLUDES) -c $<  -o $@
	

clean:
	rm -f $(ODIR)/*.o $(OUT)

