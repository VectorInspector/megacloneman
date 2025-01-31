# !!!TOP TIER MAKEFILE INCOMING!!!

# HA-AHEM. So, here's how to build this thing. You will need lodepng and SDL2. The variables allow
# you to set the directories. In addition, mega_dir is the source directory. Depending on where your
# work directory is, you want to change it to the relative path. That should be simple enough for
# g++, which this little game was developed with... all right?

# The source files are first compiled into objects before being linked. This made it faster to do
# changes and re-compile where necessary.

# put -g on the mega_debug_flag if you want to generate debug symbols for gdb.
# optimisation flags can go on make_mega_exe.

# When linker errors about the main function occur, the DSDL_MAIN_HANDLED flag might be funny.

mega_debug_flag :=
mega_test_dir := tests/
mega_graphics_def := -Dmega_GRAPHICS_DIR "megaman/graphics"
mega_sdl := -I../sdl/include/SDL2 -L../sdl/lib -lSDL2main -lSDL2
mega_lodepng := -I../lodepng
mega_lodepng_cpp := ../lodepng/lodepng.cpp
mega_objects = $(wildcard *.o)
make_mega_obj := $(mega_debug_flag) -Wall -std=c++17 -c -O0
make_mega_exe := $(mega_debug_flag) -Wall -std=c++17 -O0

# SKILL ISSUE (yeah idk but if it works it works)

megamega_rest:
#	g++ $(make_mega_obj) vec2.cpp -o vec2.o
#	g++ $(make_mega_obj) hp_bar.cpp -o hp_bar.o $(mega_sdl)
#	g++ $(make_mega_obj) level_rooms.cpp -o level_rooms.o $(mega_sdl)
#	g++ $(make_mega_obj) actors.cpp -o actors.o $(mega_sdl)
#	g++ $(make_mega_obj) items.cpp -o items.o $(mega_sdl)
#	g++ $(make_mega_obj) projectiles.cpp -o projectiles.o $(mega_sdl)
#	g++ $(make_mega_obj) inputs.cpp -o inputs.o
#	g++ $(make_mega_obj) tiles.cpp -o tiles.o
	g++ $(make_mega_obj) player_chara.cpp -o player_chara.o $(mega_sdl)
	g++ $(make_mega_obj) settings_file.cpp -o settings_file.o
	g++ $(make_mega_obj) game_data.cpp -o game_data.o $(mega_sdl) $(mega_sdl)
	g++ $(make_mega_obj) backend_sdl.cpp -o backend_sdl.o $(mega_lodepng) $(mega_sdl)
	g++ $(make_mega_obj) $(mega_lodepng_cpp) -o lodepng.o $(mega_lodepng)
	g++ $(make_mega_exe) vec2.o hp_bar.o level_rooms.o actors.o items.o settings_file.o projectiles.o inputs.o tiles.o player_chara.o game_data.o backend_sdl.o lodepng.o main.cpp -o mega_game.exe -Wall -std=c++17 $(mega_lodepng) $(mega_sdl) -DSDL_MAIN_HANDLED
	mega_game.exe
	
megamega_settings_test:
	g++ $(make_mega_obj) settings_file.cpp -o settings_file.o -I
	g++ $(mega_test_dir)settings.cpp settings_file.o -Wall -std=c++17 -o settings_test -I
	
megamega:
	g++ $(make_mega_obj) stage_select.cpp -o stage_select.o $(mega_sdl)
