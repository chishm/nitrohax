#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules

#---------------------------------------------------------------------------------
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# DATA is a list of directories containing binary files
# all directories are relative to this makefile
#---------------------------------------------------------------------------------
BUILD		:=	build
IMAGES		:=	graphics
SOURCES		:=	source $(IMG_DATA)
INCLUDES	:=	include
DATA		:=	data

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-march=armv5te -mtune=arm946e-s -mthumb -mthumb-interwork

CFLAGS	:=	-g -Wall -O2\
			-fomit-frame-pointer\
			-ffast-math \
			-Wall -Wextra -Werror \
			$(ARCH)

CFLAGS	+=	$(INCLUDE) -DARM9
CXXFLAGS	:=	$(CFLAGS)

ASFLAGS	:=	-g $(ARCH) $(INCLUDE)

LDFLAGS	=	-specs=ds_arm9.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:= -lfat -lnds9
 
#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=	$(LIBNDS)
 
#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------
 
export ARM9ELF	:=	$(CURDIR)/$(TARGET).elf
export DEPSDIR := $(CURDIR)/$(BUILD)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))
 
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

BMPFILES	:=	$(foreach dir,$(IMAGES),$(notdir $(wildcard $(dir)/*.bmp)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES	:=	$(addsuffix .o,$(BINFILES)) $(BMPFILES:.bmp=.o) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
 
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-isystem $(dir)/include) \
			-I$(CURDIR)/$(BUILD)
 
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)
 
.PHONY: $(BUILD) clean
 
#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
 
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) *.elf *.nds* *.bin 
 
#---------------------------------------------------------------------------------
else

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(ARM9BIN)	:	$(ARM9ELF) 
	@$(OBJCOPY) -O binary $< $@
	@echo built ... $(notdir $@)

$(ARM9ELF)	:	$(OFILES)
	@echo linking $(notdir $@)
	@$(LD)  $(LDFLAGS) $(OFILES) $(LIBPATHS) $(LIBS) -o $@


#---------------------------------------------------------------------------------
# graphics
#---------------------------------------------------------------------------------
bgtop.s			:	../$(IMAGES)/bgtop.bmp
	grit	$< -gB4 -gzl -fts -o $@ -q
	
bgsub.s			:	../$(IMAGES)/bgsub.bmp
	grit	$< -gB4 -gzl -fts -o $@ -q

cursor.s		:	../$(IMAGES)/cursor.bmp
	grit	$< -gB4 -fts -Mw8 -Mh4 -o $@ -q

font.s			:	../$(IMAGES)/font.bmp
	grit	$< -gB4 -gzl -fts -o font.s -q

button_go.s		:	../$(IMAGES)/button_go.bmp
	grit	$< -gB4 -fts -Mw16 -Mh4 -o $@ -q

button_on.s		:	../$(IMAGES)/button_on.bmp
	grit	$< -gB4 -fts -Mw30 -Mh2 -o $@ -q

button_off.s	:	../$(IMAGES)/button_off.bmp
	grit	$< -gB4 -fts -Mw30 -Mh2 -o $@ -q

button_folder.s	:	../$(IMAGES)/button_folder.bmp
	grit	$< -gB4 -fts -Mw30 -Mh2 -o $@ -q

button_file.s	:	../$(IMAGES)/button_file.bmp
	grit	$< -gB4 -fts -Mw30 -Mh2 -o $@ -q

scrollbar.s		:	../$(IMAGES)/scrollbar.bmp
	grit	$< -gB4 -Mw2 -Mh2 -fts -o scrollbar.s -q

textbox.s		:	../$(IMAGES)/textbox.bmp
	grit	$< -gB4 -fts -o textbox.s -q


#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data 
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPSDIR)/*.d
 
#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
