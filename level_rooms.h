#ifndef LEVEL_ROOMS_H
#define LEVEL_ROOMS_H

// Forward declarations for the referenced types.
struct Room;
struct GameData;

enum level_rooms : int {
	quickman_start = 0
,	quickman_drop1
,	quickman_drop2
,	quickman_beams1
,	quickman_beams2
,	quickman_beams3
,	quickman_beams_end
,	quickman_torches
,	quickman_more_beams1
,	quickman_more_beams2
,	quickman_more_beams3
,	quickman_more_beams4
,	quickman_more_beams5
,	quickman_more_beams6
,	quickman_more_beams7
,	quickman_armor_joes
,	quickman_pre_boss
,	quickman_boss

,	heatman_start
,	heatman_drop1
,	heatman_hell
,	heatman_drop2
,	heatman_drop3
,	heatman_pre_boss
,	heatman_boss

,	crashman_start1
,	crashman_start2
,	crashman_transit1
,	crashman_mid1
,	crashman_mid2
,	crashman_mid3
,	crashman_transit2
,	crashman_climb1
,	crashman_climb2
,	crashman_climb3
,	crashman_climb4
,	crashman_climb5
,	crashman_climb6
,	crashman_climb7
,	crashman_transit3
,	crashman_pre_boss
,	crashman_boss
};

// If you wanna see g++ spit out a funny long template error message for some reason, remove the
// semicolon from this line alone and try to compile level_rooms.cpp with the -c flag (generate an
// object file without linkage, but remember to include sdl because "game_data.h" includes its
// headers).
void OnRoomLoad (GameData& d, Room& room, level_rooms room_id);
void OnRoomSpawns (GameData& d, Room& room, level_rooms room_id);

#endif
