# Compile with Watcom 10.6 or OpenWatcom
CC = wcc386
APPNAME = sort

CFLAGS = -dSORT_STANDALONE -i=../data -4r -s -otexan
OBJDIR = .

# Object files in BIN
OBJS = $(OBJDIR)/$(APPNAME).obj $(OBJDIR)/data.obj $(OBJDIR)/mem.obj $(OBJDIR)/log.obj

# Linker directive file
LNK = $(OBJDIR)/$(APPNAME).lnk

# Final executable
$(APPNAME).exe: $(OBJS)
        @%create $(LNK) 
        @%append $(LNK) system dos4g
        @%append $(LNK) name $(APPNAME).exe
        @for %i in ($(OBJS)) do @%append $(LNK) file %i

        #IF USES 32 BIT DOS INCLUDES
        @%append $(LNK) library clib3r.lib
        wlink @$(LNK)

# Compile rules for each .c -> .obj in BIN
# External DEPS
$(OBJDIR)/data.obj: ../data/data.c
    if not exist $(OBJDIR) mkdir $(OBJDIR)
    $(CC) $(CFLAGS) -fo=$@ $<
# External DEPS
$(OBJDIR)/mem.obj: ../mem/mem.c
    if not exist $(OBJDIR) mkdir $(OBJDIR)
    $(CC) $(CFLAGS) -fo=$@ $<
# External DEPS
$(OBJDIR)/log.obj: ../log/log.c
    if not exist $(OBJDIR) mkdir $(OBJDIR)
    $(CC) $(CFLAGS) -fo=$@ $<

# Main
$(OBJDIR)/$(APPNAME).obj: sort.c
    if not exist $(OBJDIR) mkdir $(OBJDIR)
    $(CC) $(CFLAGS) -fo=$@ $<


preclean: .SYMBOLIC
    del *.obj
    del *.exe
    del *.err
    del *.lnk

postclean: .SYMBOLIC
    del *.obj
    del *.lnk

build: preclean $(APPNAME).exe postclean
