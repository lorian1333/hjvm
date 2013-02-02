TARGET = hjvm.exe
OBJS = hjvm.o cp.o interfaces.o jcf.o fields.o attributes.o methods.o util.o

CFLAGS = -I./include -Wno-multichar
LDFLAGS = -L./lib 
LIBS =

CC = gcc
CXX = g++
LD = gcc

RM = rm
CP = cp
MKDIR = mkdir


.PHONY:
all: $(TARGET)
	@echo Done building all

$(TARGET): $(OBJS)
ifeq ($(V),1)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LINK_OBJS) $(LIBS)
else
	@$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LINK_OBJS) $(LIBS)
	@echo "LD	$(TARGET)"
endif

$(OBJS): %.o:

%.o: src/%.c
ifeq ($(V),1)
	$(CC) $(CFLAGS) -c $< -o $@
else
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "CC	$@"
endif

clean:
ifeq ($(V),1)
	$(RM) -f $(OBJS)
	$(RM) -f $(TARGET)
else
	@echo "CLEAN	objects"
	@$(RM) -f $(OBJS)
	@echo "CLEAN	$(TARGET)"
	@$(RM) -f $(TARGET)
endif	
	@echo Done cleaning all
rebuild: clean all
	@echo Done rebuilding all
