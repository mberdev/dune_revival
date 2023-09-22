# GNU Make project makefile autogenerated by Premake
ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

ifndef CC
  CC = gcc
endif

ifndef CXX
  CXX = g++
endif

ifndef AR
  AR = ar
endif

ifeq ($(config),debug)
  OBJDIR     = obj/Debug
  TARGETDIR  = ../../Data
  TARGET     = $(TARGETDIR)/DuneR
  DEFINES   += -D_LINUX -DDEBUG
  INCLUDES  += -I../../../../../../external_libs/SDL/include/SDL -I../../../../../../external_libs/SDL_image/include -I../../../../../../external_libs/SDL_mixer/include -I../../../../../../external_libs/SDL_ttf/include -I../../Src -I../../Src/Core -I../../Src/Game -I../../Src/Gfx
  CPPFLAGS  += -MMD -MP $(DEFINES) $(INCLUDES)
  CFLAGS    += $(CPPFLAGS) $(ARCH) -g
  CXXFLAGS  += $(CFLAGS) 
  LDFLAGS   += -L../../../../../../external_libs/SDL/lib -L../../../../../../external_libs/SDL_image/lib -L../../../../../../external_libs/SDL_mixer/lib -L../../../../../../external_libs/SDL_ttf/lib
  LIBS      += -lSDLmain -lSDL -lSDL_image -lSDL_mixer -lSDL_ttf
  RESFLAGS  += $(DEFINES) $(INCLUDES) 
  LDDEPS    += 
  LINKCMD    = $(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(ARCH) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

ifeq ($(config),release)
  OBJDIR     = obj/Release
  TARGETDIR  = ../../Data
  TARGET     = $(TARGETDIR)/DuneR
  DEFINES   += -D_LINUX -DNDEBUG
  INCLUDES  += -I../../../../../../external_libs/SDL/include/SDL -I../../../../../../external_libs/SDL_image/include -I../../../../../../external_libs/SDL_mixer/include -I../../../../../../external_libs/SDL_ttf/include -I../../Src -I../../Src/Core -I../../Src/Game -I../../Src/Gfx
  CPPFLAGS  += -MMD -MP $(DEFINES) $(INCLUDES)
  CFLAGS    += $(CPPFLAGS) $(ARCH) -O2
  CXXFLAGS  += $(CFLAGS) 
  LDFLAGS   += -s -L../../../../../../external_libs/SDL/lib -L../../../../../../external_libs/SDL_image/lib -L../../../../../../external_libs/SDL_mixer/lib -L../../../../../../external_libs/SDL_ttf/lib
  LIBS      += -lSDLmain -lSDL -lSDL_image -lSDL_mixer -lSDL_ttf
  RESFLAGS  += $(DEFINES) $(INCLUDES) 
  LDDEPS    += 
  LINKCMD    = $(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(ARCH) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

OBJECTS := \
	$(OBJDIR)/DuneHsq.o \
	$(OBJDIR)/DuneMain.o \
	$(OBJDIR)/DuneText.o \
	$(OBJDIR)/DuneTools.o \
	$(OBJDIR)/DuneBackGround.o \
	$(OBJDIR)/DuneIntro.o \
	$(OBJDIR)/DuneMenu.o \
	$(OBJDIR)/DuneR.o \
	$(OBJDIR)/DuneRoot.o \
	$(OBJDIR)/DuneTest.o \
	$(OBJDIR)/DuneDataManager.o \
	$(OBJDIR)/DuneFont.o \
	$(OBJDIR)/DuneGfx.o \
	$(OBJDIR)/DunePlanet.o \
	$(OBJDIR)/DuneScn.o \
	$(OBJDIR)/DuneVoxel.o \

RESOURCES := \

SHELLTYPE := msdos
ifeq (,$(ComSpec)$(COMSPEC))
  SHELLTYPE := posix
endif
ifeq (/bin,$(findstring /bin,$(SHELL)))
  SHELLTYPE := posix
endif

.PHONY: clean prebuild prelink

all: $(TARGETDIR) $(OBJDIR) prebuild prelink $(TARGET)
	@:

$(TARGET): $(GCH) $(OBJECTS) $(LDDEPS) $(RESOURCES)
	@echo Linking DuneR
	$(SILENT) $(LINKCMD)
	$(POSTBUILDCMDS)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(TARGETDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(TARGETDIR))
endif

$(OBJDIR):
	@echo Creating $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(OBJDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(OBJDIR))
endif

clean:
	@echo Cleaning DuneR
ifeq (posix,$(SHELLTYPE))
	$(SILENT) rm -f  $(TARGET)
	$(SILENT) rm -rf $(OBJDIR)
else
	$(SILENT) if exist $(subst /,\\,$(TARGET)) del $(subst /,\\,$(TARGET))
	$(SILENT) if exist $(subst /,\\,$(OBJDIR)) rmdir /s /q $(subst /,\\,$(OBJDIR))
endif

prebuild:
	$(PREBUILDCMDS)

prelink:
	$(PRELINKCMDS)

ifneq (,$(PCH))
$(GCH): $(PCH)
	@echo $(notdir $<)
	-$(SILENT) cp $< $(OBJDIR)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
endif

$(OBJDIR)/DuneHsq.o: ../../Src/DuneHsq.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/DuneMain.o: ../../Src/DuneMain.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/DuneText.o: ../../Src/DuneText.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/DuneTools.o: ../../Src/DuneTools.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/DuneBackGround.o: ../../Src/Game/DuneBackGround.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/DuneIntro.o: ../../Src/Game/DuneIntro.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/DuneMenu.o: ../../Src/Game/DuneMenu.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/DuneR.o: ../../Src/Game/DuneR.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/DuneRoot.o: ../../Src/Game/DuneRoot.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/DuneTest.o: ../../Src/Game/DuneTest.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/DuneDataManager.o: ../../Src/Gfx/DuneDataManager.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/DuneFont.o: ../../Src/Gfx/DuneFont.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/DuneGfx.o: ../../Src/Gfx/DuneGfx.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/DunePlanet.o: ../../Src/Gfx/DunePlanet.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/DuneScn.o: ../../Src/Gfx/DuneScn.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/DuneVoxel.o: ../../Src/Gfx/DuneVoxel.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"

-include $(OBJECTS:%.o=%.d)
