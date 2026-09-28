# Compile with Watcom 10.6 or OpenWatcom

CC = wcc386
CFLAGS = -i=app -i=engine -i=hal -i=deps/data -i=deps/env -4r -s -otexan
APPNAME = bmpio
OBJDIR = bin

# Object files in BIN
OBJS = $(OBJDIR)/main.obj $(OBJDIR)/assets.obj $(OBJDIR)/data.obj $(OBJDIR)/vgaregs.obj $(OBJDIR)/video.obj $(OBJDIR)/env.obj $(OBJDIR)/input.obj $(OBJDIR)/log.obj $(OBJDIR)/math.obj $(OBJDIR)/game.obj $(OBJDIR)/engine.obj $(OBJDIR)/space.obj $(OBJDIR)/test.obj $(OBJDIR)/mem.obj $(OBJDIR)/deps_mem.obj $(OBJDIR)/settings.obj

# Linker directive file
LNK = $(OBJDIR)/bmpio.lnk

# Final executable
bmpio.exe: $(OBJS)
	@%create $(LNK)
	@%append $(LNK) system dos4g
	@%append $(LNK) name bmpio
	@for %i in ($(OBJS)) do @%append $(LNK) file %i
	wlink @$(LNK)

# Compile rules for each .c -> .obj in BIN
$(OBJDIR)/deps_mem.obj: deps/mem/mem.c
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -fo=$@ $<
$(OBJDIR)/vgaregs.obj: hal/vgaregs.c
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJDIR)/video.obj: engine/video.c
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJDIR)/mem.obj: engine/mem.c
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJDIR)/data.obj: deps/data/data.c
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJDIR)/math.obj: engine/math.c
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJDIR)/space.obj: engine/space.c
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJDIR)/env.obj: deps/env/env.c
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJDIR)/log.obj: deps/log/log.c
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJDIR)/input.obj: deps/input/input.c
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJDIR)/game.obj: engine/game.c
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJDIR)/settings.obj: engine/settings/settings.c
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJDIR)/assets.obj: engine/assets.c
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJDIR)/engine.obj: engine/engine.c
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJDIR)/main.obj: app/main.c
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJDIR)/test.obj: engine/test.c
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -fo=$@ $<


preclean: .SYMBOLIC
	@if exist $(OBJDIR)/*.obj del $(OBJDIR)/*.obj
	@if exist $(OBJDIR)/*.err del $(OBJDIR)/*.err
	@if exist $(OBJDIR)/*.lnk del $(OBJDIR)/*.lnk
	@if exist $(DEPSDIR)/*.obj del $(DEPSDIR)/*.obj
	@if exist $(DEPSDIR)/*.lnk del $(DEPSDIR)/*.lnk
	@if exist $(DEPSDIR)/*.err del $(DEPSDIR)/*.err
	@if exist *.obj del *.obj
	@if exist *.err del *.err
	@if exist *.lnk del *.lnk
	@if exist *.exe del *.exe

postclean: .SYMBOLIC
	@if exist *.obj del *.obj
	@if exist $(DEPSDIR)/*.obj del $(DEPSDIR)/*.obj
	@if exist bin/dos/*.obj del bin/dos/*.obj
	@if exist *.lnk del *.lnk
	@if exist bin/dos/*.lnk del bin/dos/*.lnk
	@if exist $(APPNAME).exe copy $(APPNAME).exe $(OBJDIR)/$(APPNAME).exe
	del $(APPNAME).exe

build: preclean $(APPNAME).exe postclean
