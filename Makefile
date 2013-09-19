OUT=Znake
SOURCES=src
INCLUDES=include
ODIR = obj

CFILES= $(SOURCES)/znake.c main.c
OBJS=  $(addprefix $(ODIR)/, $(notdir $(CFILES:.c=.o)))
LIBS = -lncurses
CFLAGS = 

all: directories $(OUT)

directories:
	mkdir -p $(ODIR)

$(OUT): $(OBJS)
	gcc $(OBJS) -o $(OUT) $(LIBS)
	

$(ODIR)/%.o: $(SOURCES)/%.c
	gcc -I$(INCLUDES) -c $<  -o $@
	

clean:
	rm -f $(ODIR)/*.o $(OUT)

