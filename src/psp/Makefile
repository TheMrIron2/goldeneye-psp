# If GPROF is used it builds an ELF, hence the EBOOT needs to be in GAME150
#USE_GPROF		= 1

# Site specific variables.
INSTALL_DIR		= $(PSP_MOUNT)/PSP/GAME/Quake

# Compiler specific variables.
PSPSDK		= $(shell psp-config --pspsdk-path)
ifeq ($(PSPSDK),)
$(error PSPSDK wasn't set)
endif
PSPLIBSDIR		= $(PSPSDK)/..

# Project specific variables.
VERSION		= 3_WIP
SRC_DIR		= ..
OBJ_DIR		= obj
TARGET		= $(OBJ_DIR)/Quake
PSP_EBOOT_TITLE	= ADQuake v$(VERSION)
PSP_EBOOT_ICON	= pics/icon.png
PSP_EBOOT_PIC1	= NULL #pics/pic.png
PSP_EBOOT_SFO	= $(OBJ_DIR)/PARAM.SFO
PSP_EBOOT		= $(OBJ_DIR)/EBOOT.PBP
DIST_DIR		= dist
DIST_FILES		= readme.html gpl.txt Quake/EBOOT.PBP
ZIP_FILE		= Quake_v$(VERSION).zip
PSP_LARGE_MEMORY = 1

ifeq ($(USE_GPROF),1)
GPROF_LIBS      = -lpspprof
GPROF_FLAGS		= -pg -DPROFILE
else
BUILD_PRX		= 1
endif

# Object files used regardless of video back end.
COMMON_OBJS = \
    $(OBJ_DIR)/psp/cd.o \
    $(OBJ_DIR)/psp/mp3.o \
	$(OBJ_DIR)/psp/battery.o \
	$(OBJ_DIR)/psp/KernelHooks/ctrl/cwbhook_extender.o \
	$(OBJ_DIR)/psp/input.o \
	$(OBJ_DIR)/psp/main.o \
	$(OBJ_DIR)/psp/math.o \
	$(OBJ_DIR)/psp/sound.o \
	$(OBJ_DIR)/psp/fnmatch.o \
	$(OBJ_DIR)/psp/system.o \
	$(OBJ_DIR)/psp/module.o \
	$(OBJ_DIR)/psp/network.o \
	$(OBJ_DIR)/psp/network_infrastructure.o \
	$(OBJ_DIR)/psp/gethost.o \
	\
	$(OBJ_DIR)/chase.o \
	$(OBJ_DIR)/cl_demo.o \
	$(OBJ_DIR)/cl_input.o \
	$(OBJ_DIR)/cl_main.o \
	$(OBJ_DIR)/cl_parse.o \
	$(OBJ_DIR)/cl_tent.o \
	$(OBJ_DIR)/cmd.o \
	$(OBJ_DIR)/common.o \
	$(OBJ_DIR)/console.o \
	$(OBJ_DIR)/crc.o \
	$(OBJ_DIR)/cvar.o \
	$(OBJ_DIR)/host.o \
	$(OBJ_DIR)/host_cmd.o \
	$(OBJ_DIR)/keys.o \
	$(OBJ_DIR)/mathlib.o \
	$(OBJ_DIR)/matrixlib.o \
	$(OBJ_DIR)/menu.o \
	$(OBJ_DIR)/net_dgrm.o \
	$(OBJ_DIR)/net_loop.o \
	$(OBJ_DIR)/net_main.o \
	$(OBJ_DIR)/net_vcr.o \
	$(OBJ_DIR)/pr_cmds.o \
	$(OBJ_DIR)/pr_edict.o \
	$(OBJ_DIR)/pr_exec.o \
	$(OBJ_DIR)/r_part.o \
	$(OBJ_DIR)/snd_dma.o \
	$(OBJ_DIR)/snd_mem.o \
	$(OBJ_DIR)/snd_mix.o \
    $(OBJ_DIR)/snd_dsp_v1.o \
	$(OBJ_DIR)/sbar.o \
	$(OBJ_DIR)/sv_main.o \
	$(OBJ_DIR)/sv_move.o \
	$(OBJ_DIR)/sv_phys.o \
	$(OBJ_DIR)/sv_user.o \
	$(OBJ_DIR)/view.o \
	$(OBJ_DIR)/wad.o \
	$(OBJ_DIR)/world.o \
	$(OBJ_DIR)/zone.o \
    $(OBJ_DIR)/random.o \
    $(OBJ_DIR)/materials.o

# Object files used only under hardware video.
VIDEO_OBJS = \
	$(OBJ_DIR)/psp/clipping.o \
	$(OBJ_DIR)/psp/video_hardware.o \
	$(OBJ_DIR)/psp/video_hardware_hlmdl.o \
	$(OBJ_DIR)/psp/video_hardware_draw.o \
	$(OBJ_DIR)/psp/video_hardware_decals.o \
	$(OBJ_DIR)/psp/video_hardware_resample.o \
	$(OBJ_DIR)/psp/video_hardware_images.o \
	$(OBJ_DIR)/psp/video_hardware_dxtn.o \
	$(OBJ_DIR)/psp/video_hardware_entity_fragment.o \
	$(OBJ_DIR)/psp/video_hardware_light.o \
	$(OBJ_DIR)/psp/video_hardware_main.o \
	$(OBJ_DIR)/psp/video_hardware_mesh.o \
	$(OBJ_DIR)/psp/video_hardware_misc.o \
	$(OBJ_DIR)/psp/video_hardware_model.o \
	$(OBJ_DIR)/psp/video_hardware_screen.o \
	$(OBJ_DIR)/psp/video_hardware_surface.o \
	$(OBJ_DIR)/psp/video_hardware_warp.o
VIDEO_FLAGS = -DPSP_HARDWARE_VIDEO

OBJS	= $(VIDEO_OBJS) $(COMMON_OBJS) $(GPROF_OBJS)

# Compiler flags.
CFLAGS	= -ffast-math -O3 -G0 $(GPROF_FLAGS) -Wall -Wno-trigraphs -Winline -DPSP $(VIDEO_FLAGS) -g

# Libs.
IMG_LIBS    = -ljpeg -lpng -lz
GU_LIBS	    = -lpspvram -lpspgum_vfpu -lpspvfpu -lpspgu -lpspmath
AUDIO_LIBS	= -lpspaudiolib -lpspaudio -lpspmp3
MISC_LIBS	= -lpsprtc -lpsppower
STD_LIBS	= -lstdc++ -lm -lc
LIBS		= $(GPROF_LIBS) $(GU_LIBS) $(AUDIO_LIBS) $(MISC_LIBS) $(IMG_LIBS) $(STD_LIBS)

# What else to clean.
EXTRA_CLEAN	= $(foreach FILE,$(ZIP_FILE) Quake/EBOOT.PBP,$(DIST_DIR)/$(FILE))

# All target.
all: $(PSP_EBOOT)

# What is an install?
install: $(INSTALL_DIR)/EBOOT.PBP

# How to install.
$(INSTALL_DIR)/EBOOT.PBP: $(PSP_EBOOT)
	@echo Installing $(PSP_EBOOT) to $@...
	@-mkdir -p $(dir $@)
	@cp $(PSP_EBOOT) $@

# How to build the distribution.
distro: clean $(ZIP_FILE)
	@echo Distribution prepared.

$(DIST_DIR)/Quake/EBOOT.PBP: $(PSP_EBOOT)
	@echo Copying $< to $@...
	@-mkdir -p $(dir $@)
	@cp $< $@

$(ZIP_FILE): $(foreach FILE,$(DIST_FILES),$(DIST_DIR)/$(FILE))
	@echo Creating $(DIST_DIR)/$(ZIP_FILE)...
	@-rm -rf $(ZIP_FILE)
	@cd $(DIST_DIR) && zip -r -q -9 -o $(ZIP_FILE) $(DIST_FILES)

# How to compile an S file.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.S
	@echo $(notdir $<)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

# How to compile a C file.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo $(notdir $<)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

# How to compile a C++ file.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo $(notdir $<)
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Use the standard PSPSDK build stuff.
include $(PSPSDK)/lib/build.mak

ifeq ($(BUILD_PRX),1)
OBJ_TYPE	= prx
else
OBJ_TYPE	= elf
endif

