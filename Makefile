# -----------------------------------------------------------------------------
# The path to your g++ install, leaving it as g++ is normally good enough
# -----------------------------------------------------------------------------
GPP			=	$(shell fltk-config --cxx)

# -----------------------------------------------------------------------------
# TARGET   : The final output file for
# BUILD    : The folder where .o files go (not implemented)
# SOURCES  : The folder(s) containing the c++ sources
# INCLUDES : The folder containing the header files
# CPPFLAGS : The optional compiler flags to use
# -----------------------------------------------------------------------------
TARGET		=	Runes
BUILD		=	build
SOURCES		=	source source/3rd_party
INCLUDES	=	include include/3rd_party
CPPFLAGS	=	-Wall -g -DDEBUG
LDFLAGS		=	-static-libgcc -static-libstdc++ -static -g

# -----------------------------------------------------------------------------
# Builds lists of the files to be put into the compiler
# -----------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I"$(CURDIR)/$(dir)")
export SOURCE	:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.cpp))
SOURCE			+=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.c))
#export OFILES	:=	$(foreach dir,$(BUILD),$(notdir $(wildcard $(dir)/*.o)))
export OFILES	:=	$(foreach dir,$(SOURCE),./$(basename $(notdir $(dir))).o)
export OBJECTS	:=	$(foreach dir,$(OFILES),-o $(dir))
export OUTPUT	:=	$(CURDIR)/$(TARGET)

all:
	"$(GPP)" -c $(SOURCE) $(shell fltk-config --use-gl --use-images --cxxflags ) $(INCLUDE) $(CPPFLAGS)
	"$(GPP)" -o $(TARGET).exe $(OFILES) $(LDFLAGS) $(shell fltk-config --use-gl --use-images --ldstaticflags )
osx:
	"$(GPP)" -c $(SOURCE) $(INCLUDE) $(CPPFLAGS)
	"$(GPP)" -o $(TARGET) $(OFILES)
debug:
	@echo $(SOURCE)