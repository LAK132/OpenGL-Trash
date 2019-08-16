CXX = g++-8 -std=c++17
SDL = /usr/include/SDL2

LIB = -ldl -lSDL2 -pthread
BIN = bin
OBJ = obj

imgui_SRC = lib/imgui
imgui_OBJ = imgui.cpp imgui_draw.cpp
imgui_INC =

libs_SRC = lib
libs_OBJ = gl3w.c imgui_impl_sdl_gl3.cpp
libs_INC = -Iinclude -I$(SDL) -Ilib/imgui

main_SRC = src
main_OBJ = main.cpp update.cpp draw.cpp
main_INC = -Iinclude -I$(SDL) -I/usr/local/include -Ilib/imgui

programs = main imgui libs

ALL_OBJ = $(foreach prog,$(programs),$(foreach obj,$($(prog)_OBJ),$(OBJ)/$(obj).o))

.PHONY: all
all: $(ALL_OBJ) | $(BIN)
	$(CXX) -o $(BIN)/out.elf $(ALL_OBJ) $(LIB)

define COMPILE_TEMPLATE =
$(2)/$(3).o: $(1)/$(3) | $(2)
	$(CXX) -c -o $(2)/$(3).o $(1)/$(3) $(4)
endef

$(foreach prog,$(programs),$(foreach obj,$($(prog)_OBJ),$(eval $(call COMPILE_TEMPLATE,$($(prog)_SRC),$(OBJ),$(obj),$($(prog)_INC)))))

clean:
	rm -f $(ALL_OBJ)

$(BIN):
	mkdir $(BIN)

$(OBJ):
	mkdir $(OBJ)
