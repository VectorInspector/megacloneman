#include "level_rooms.h"
#include "tiles.h"
#include "game_data.h"

void OnRoomLoad (GameData& d, Room& room, level_rooms room_id) {
	auto& t		= d.tiles;
	auto& ta	= d.transition_areas;
	
	// Reset the whole board of tiles.
	t.Resize();
	t.Fill(0);
	
	room.TilesTo(0, 0, 0, 0);
	room.which = room_id;
	
	switch(room_id) {
	default:
		room.TilesTo(0, 0);
		t.Rect(1, 0, 0, 16, 16);
		t.Yline(-1, 4, 0, 3);
		t.Yline(-1, 7, 4, 8);
		t.Yline(-1, 11, 8, 11);
		break;
	
	case crashman_start1:
		room.TilesTo(0, 3);
		t.StringCursor(1, 0, "d 6 r 2 l 2 d 6 r 13 u 8 l 2 r 2 u 3 l 5 r 5 u 1");
		t.Yline(-1, 4, 12, 15);
		t.Yline(-1, 8, 7, 10);
		t.Yline(-1, 5, 0, 5);
		break;
	
	case crashman_start2:
		room.TilesTo(0, 2);
		t.StringCursor(6, 0, "d 1 r 1 d 2 r 6 d 2 l 2 r 2 d 2 r 1 d 4 l 2 r 2 d 4");
		t.Yline(1, 1, 0, 15);
		t.Point(1, 2, 6);
		t.Xline(1, 12, 2, 6);
		t.Xline(1, 8, 7, 10);
		t.Yline(-1, 5, 0, 5);
		t.Yline(-1, 4, 12, 15);
		t.Yline(-1, 7, 8, 10);
		break;
	
	case crashman_transit1:
		room.TilesTo(3, 3, 4, 3);
		t.StringCursor(1, 15, "u 10 r 6 u 2 r 10 u 2 r 9 u 2");
		t.StringCursor(6, 15, "u 4 r 7 u 1 r 8 u 1 r 4 u 1 r 5 u 8");
		t.Yline(-1, 5, 11, 15);
		t.Yline(-1, 28, 0, 4);
		break;
	
	case heatman_boss:
		room.TilesTo(3, 1, 3, 1);
		t.Rect(1, 0, 1, 15, 12);
		break;
	case heatman_hell:
		room.TilesTo(0, 2, 10, 2);
		t.Rect(1, 3, 0, 79, 1);
		t.Rect(1, 0, 12, 20, 15);
		t.Rect(1, 23, 12, 27, 15);
		t.Rect(1, 30, 12, 39, 15);
		t.Rect(1, 42, 12, 49, 15);
		t.Rect(1, 52, 12, 58, 15);
		t.Rect(1, 62, 12, 65, 15);
		t.Rect(1, 68, 12, 71, 15);
		t.Rect(1, 74, 12, 109, 15);
		t.Rect(1, 160, 12, 172, 15);
		t.Rect(-1, 2, 0, 2, 1);
		t.Rect(-1, 173, 12, 173, 15);
		t.Yline(1, 11, 13, 110);
		t.Yline(1, 11, 13, 139);
		t.Yline(1, 0, 11, 1);
		t.Yline(1, 0, 15, 174);
		t.Yline(1, 8, 11, 20);
		t.Yline(1, 5, 11, 27);
		t.Yline(1, 8, 9, 43);
		t.Yline(1, 5, 7, 53);
		t.Yline(1, 10, 11, 53);
		t.Yline(1, 8, 9, 62);
		t.Xline(1, 82, 91, 9);
		t.Xline(1, 86, 91, 8);
		ta.Add(172, 14, 174, 15, heatman_drop2, dir_down);
		break;
	case heatman_pre_boss:
		room.TilesTo(2, 1, 2, 1);
		t.Xline(1, 0, 15, 5);
		t.Xline(1, 0, 15, 10);
		ta.Add(14, 5, 14, 9, heatman_boss, dir_right);
		break;
	case heatman_drop3:
		room.TilesTo(1, 1, 1, 1);
		t.StringCursor(2, 0, "d 10 r 13");
		t.Rect(-1, 3, 0, 3, 2);
		t.Rect(1, 4, 0, 7, 2);
		t.Rect(1, 13, 0, 15, 5);
		ta.Add(14, 5, 14, 9, heatman_pre_boss, dir_right);
		break;
	case heatman_drop2:
		room.TilesTo(0, 1, 0, 1);
		t.Rect(-1, 13, 0, 13, 10);
		t.Rect(-1, 3, 11, 3, 15);
		t.StringCursor(12, 0, "d 3 l 10 d 10");
		t.StringCursor(14, 0, "d 11 l 10 d 3");
		ta.Add(2, 14, 4, 15, heatman_drop3, dir_down);
		break;
	case heatman_start:
		room.TilesTo(0, 0, 7, 0);
		t.StringCursor(1, 0, "d 11 r 14");
		// t.Point(2, 6, 10);
		// t.Point(1, 7, 10);
		// t.Point(3, 8, 10);
		// t.Rect(1, 3, 9, 19, 9);
		t.Rect(1, 10, 0, 21, 1);
		t.Rect(1, 24, 0, 29, 1);
		t.Rect(1, 32, 0, 39, 1);
		t.Rect(1, 42, 0, 95, 1);
		t.Xline(1, 19, 23, 11);
		t.Xline(1, 27, 31, 11);
		t.Xline(1, 35, 46, 11);
		t.Xline(1, 49, 50, 11);
		t.Rect(1, 53, 9, 54, 11);
		t.Rect(1, 57, 11, 57, 11);
		t.StringCursor(59, 8, "d 3 r 1");
		t.Rect(1, 63, 8, 63, 11);
		t.Rect(1, 67, 10, 67, 11);
		t.Rect(1, 70, 8, 70, 11);
		t.Rect(1, 74, 10, 74, 11);
		t.Rect(1, 77, 10, 77, 11);
		t.Rect(1, 80, 10, 80, 11);
		t.Rect(1, 83, 10, 83, 11);
		t.Rect(1, 86, 10, 86, 11);
		t.Rect(1, 89, 10, 89, 11);
		t.Rect(1, 93, 10, 93, 11);
		t.Rect(1, 96, 10, 96, 10);
		t.Rect(1, 99, 9, 99, 9);
		t.Rect(1, 100, 6, 100, 6);
		t.Rect(1, 104, 11, 104, 11);
		t.Rect(1, 106, 9, 106, 9);
		t.StringCursor(110, 12, "r 8 u 2 r 4 u 4 r 1 d 6 r 1");
		t.Rect(1, 112, 8, 115, 8);
		t.Rect(1, 114, 5, 117, 5);
		t.Rect(1, 120, 6, 121, 6);
		t.StringCursor(112, 0, "d 1 r 14 d 11");
		t.Rect(-1, 125, 12, 125, 15);
		ta.Add(124, 14, 126, 15, heatman_drop1, dir_down);
		break;
		
	case heatman_drop1:
		room.TilesTo(7, 1);
		t.StringCursor(1, 0, "d 15");
		t.StringCursor(14, 0, "d 13 l 10");
		t.Yline(1, 4, 15, 3);
		t.Yline(1, 0, 1, 12);
		t.Yline(1, 10, 10, 5);
		t.Yline(1, 10, 10, 8);
		t.Yline(1, 10, 10, 11);
		t.Yline(1, 10, 10, 13);
		t.Rect(-1, 13, 0, 13, 9);
		t.Rect(-1, 2, 4, 2, 15);
		ta.Add(0, 14, 2, 15, heatman_hell, dir_down);
		break;
	
	case quickman_start:
		room.TilesTo(0, 0);
		// t.StringCursor(5, 0, "d 10", -1);
		// t.StringCursor(7, 3, "d 4", -1);
		// t.StringCursor(8, 8, "r 1", -1);
		t.StringCursor(1, 0, "d 6 r 2 d 4 r 6 d 6");
		t.StringCursor(14, 0, "d 10 l 2 d 6");
		ta.Add(0, 13, 15, 13, quickman_drop1, dir_down);
		break;
	
	case quickman_drop1:
		room.TilesTo(1, 0);
		t.StringCursor(9, 0, "d 2 l 2 d 2 l 1 u 1 l 3 d 2 l 1 d 4 l 1 d 6");
		t.StringCursor(12, 0, "d 6 l 2 d 4 l 3 u 2 l 1 d 4 l 2 d 3");
		ta.Add(0, 13, 15, 13, quickman_drop2, dir_down);
		break;
		
	case quickman_drop2:
		room.TilesTo(2, 0);
		t.StringCursor(1, 0, "d 4 r 2 d 1 r 4 d 1 r 4 d 1 l 4 d 2 l 4 d 6");
		t.StringCursor(4, 0, "d 1 r 4 d 1 r 2 d 1 r 4 d 7 l 2 d 6");
		ta.Add(0, 13, 15, 13, quickman_beams1, dir_down);
		break;
	
	case quickman_beams1:
		room.TilesTo(3, 0);
		t.Rect(1, 0, 0, 3, 3);
		t.Rect(1, 0, 5, 3, 6);
		t.Rect(1, 0, 8, 3, 15);
		t.Rect(1, 12, 0, 15, 3);
		t.Rect(1, 12, 5, 15, 6);
		t.StringCursor(15, 8, "l 5 d 2 l 2 d 2 l 2 d 3");
		ta.Add(0, 13, 15, 13, quickman_beams2, dir_down);
		break;
	
	case quickman_beams2:
		room.TilesTo(4, 0);
		t.StringCursor(3, 0, "d 1 l 2 d 3 l 1");
		t.StringCursor(6, 0, "d 1 r 6 d 3 r 3");
		t.Rect(1, 0, 6, 1, 7);
		t.Rect(1, 0, 9, 1, 11);
		t.Rect(1, 0, 13, 1, 15);
		t.Rect(1, 4, 10, 5, 11);
		t.Rect(1, 4, 13, 5, 15);
		t.Rect(1, 12, 6, 15, 7);
		t.Rect(1, 12, 9, 15, 11);
		t.Rect(1, 12, 13, 15, 15);
		ta.Add(0, 13, 15, 13, quickman_beams3, dir_down);
		break;
	
	case quickman_beams3:
		room.TilesTo(0, 1);
		t.Rect(1, 0, 0, 1, 3);
		t.Rect(1, 4, 0, 5, 3);
		t.Rect(1, 12, 0, 15, 3);
		t.Rect(1, 0, 5, 1, 7);
		t.Rect(1, 4, 5, 6, 7);
		t.Point(1, 6, 8);
		t.Yline(1, 5, 8, 9);
		t.Xline(1, 12, 15, 5);
		t.Rect(1, 0, 9, 1, 15);
		t.Rect(1, 12, 7, 15, 11);
		t.Rect(1, 12, 13, 15, 15);
		t.Xline(1, 6, 9, 11);
		t.StringCursor(4, 15, "u 4 r 1 d 2 r 4 d 2");
		ta.Add(0, 13, 15, 13, quickman_beams_end, dir_down);
		break;
	
	case quickman_beams_end:
		room.TilesTo(1, 1);
		t.StringCursor(4, 0, "d 10 r 1 u 7 r 4 u 3");
		t.StringCursor(12, 0, "d 3 r 1 d 7 l 1 d 3 l 7 d 2");
		t.Yline(1, 0, 15, 1);
		t.Xline(1, 6, 9, 10);
		t.Rect(1, 8, 6, 10, 7);
		ta.Add(0, 13, 15, 13, quickman_torches, dir_down);
		break;
	
	case quickman_torches:
		room.TilesTo(0, 2, 4, 2);
		t.StringCursor(1, 0, "d 12 r 15 u 2 r 4 u 2 r 3 d 2 r 2 d 2 r 7 u 2 r 4 u 2 r 3 d 2 r 4 d 2 r 5 u 2 r 12 u 2 r 13 d 6 l 10 d 1");
		t.StringCursor(5, 0, "d 1 r 22 d 2 r 21 d 2 r 28 d 9 r 3");
		ta.Add(64, 13, 79, 13, quickman_more_beams1, dir_down);
		break;
	
	case quickman_more_beams1:
		room.TilesTo(2, 1);
		t.Xline(1, 11, 15, 4);
		t.Xline(1, 7, 8, 6);
		t.Xline(1, 9, 15, 8);
		t.Xline(1, 0, 6, 9);
		t.Xline(1, 6, 9, 12);
		ta.Add(0, 13, 15, 13, quickman_more_beams2, dir_down);
		break;
	
	case quickman_more_beams2:
		room.TilesTo(3, 1);
		t.Xline(1, 0, 5, 4);
		t.Xline(1, 10, 15, 4);
		t.Xline(1, 6, 9, 7);
		t.Xline(1, 4, 11, 10);
		ta.Add(0, 13, 15, 13, quickman_more_beams3, dir_down);
		break;
	
	case quickman_more_beams3:
		room.TilesTo(4, 1);
		t.Xline(1, 0, 5, 4);
		t.Xline(1, 10, 15, 4);
		t.Rect(1, 4, 12, 15, 14);
		ta.Add(0, 13, 15, 13, quickman_more_beams4, dir_down);
		break;
	
	case quickman_more_beams4:
		room.TilesTo(0, 3);
		t.Xline(1, 0, 3, 5);
		t.Xline(1, 0, 5, 7);
		t.Xline(1, 0, 9, 9);
		t.Rect(1, 8, 12, 15, 15);
		ta.Add(0, 13, 15, 13, quickman_more_beams5, dir_down);
		break;
	
	case quickman_more_beams5:
		room.TilesTo(1, 3);
		t.Rect(1, 8, 0, 15, 5);
		t.Rect(1, 8, 7, 15, 10); t.Xline(1, 6, 7, 7);
		t.Rect(1, 8, 12, 15, 14); t.Xline(1, 6, 7, 12);
		ta.Add(0, 13, 15, 13, quickman_more_beams6, dir_down);
		break;
	
	case quickman_more_beams6:
		room.TilesTo(2, 3);
		t.Rect(1, 0, 5, 7, 6);
		t.Rect(1, 7, 9, 15, 10);
		ta.Add(0, 13, 15, 13, quickman_more_beams7, dir_down);
		break;
	
	case quickman_more_beams7:
		room.TilesTo(3, 3);
		t.Rect(1, 0, 4, 7, 5);
		t.Rect(1, 13, 4, 15, 5);
		t.Rect(1, 0, 8, 7, 9);
		t.Rect(1, 13, 8, 15, 9);
		t.Rect(1, 0, 8, 1, 15);
		t.Rect(1, 4, 12, 15, 15);
		ta.Add(0, 13, 15, 13, quickman_armor_joes, dir_down);
		break;
	
	case quickman_armor_joes:
		room.TilesTo(0, 4, 2, 4);
		t.StringCursor(1, 0, "d 12 r 19 u 1 r 1 d 1 r 5 u 1 r 1 d 1 r 5 u 1 r 1 d 1 r 5 u 2 r 6 u 2 r 3");
		t.StringCursor(4, 0, "d 3 r 22 d 2 r 18 u 2 r 3");
		ta.Add(46, 0, 47, 13, quickman_pre_boss, dir_right);
		break;
	
	case quickman_pre_boss:
		room.TilesTo(3, 4);
		t.Xline(1, 0, 15, 3);
		t.Xline(1, 0, 15, 8);
		ta.Add(14, 0, 15, 13, quickman_boss, dir_right);
		break;
	
	case quickman_boss:
		room.TilesTo(4, 4);
		t.StringCursor(15, 0, "d 12 l 3 u 1 l 2 u 1 l 1 d 1 l 3 u 1 l 1 d 1 l 2 d 1 l 3 u 4");
		t.Yline(1, 0, 3, 0);
		break;
	}
	
	// Now shrink to fit.
	t.Resize((1 + room.x2 - room.x1) * 16, (1 + room.y2 - room.y1) * 14);
}

void OnRoomSpawns (GameData& d, Room& room, level_rooms room_id) {
	int beam_time = 30;
	
	// quickman_force_beam_time = 5;
	auto& a			= d.actors;
	auto tile_size	= d.tiles.tile_size;
	auto tile_pos	= [&] (int x, int y) { auto tp = Vec2(tile_size * x, tile_size * y); return tp; };
	auto beam_delay	= 40;
	
	auto QuickmanForceBeam = [&] (int x, int y, bool both_sides = false, int delay = 65) {
		d.beams.SpawnForceBeam(d, tile_size * Vec2(x, y), beam_time, 5, x < 8 ? 1 : -1);
		
		if(both_sides) {
			d.beams.SpawnForceBeam(d, tile_size * Vec2(16 - x, y), beam_time, 5, x < 8 ? -1 : 1);
		}
		
		beam_time += delay;
	};
	
	d.projectiles.Clear();
	d.actors.Clear();
	d.beams.Clear();
	
	switch(room_id) {
	default:
		break;
		
	case heatman_boss:
		d.shutter_anims.AddShutter(tile_pos(0, 6), false);
		break;
	case heatman_drop3:
		d.shutter_anims.AddShutter(tile_pos(15, 6), true);
		break;
	case quickman_boss:
		// Object.S_ChangeMusic("boss");
		d.shutter_anims.AddShutter(tile_pos(0, 4), false);
		d.bosses.Spawn(d, quick_man, tile_pos(14, 1), Vec2(0, 0), -1);
		break;
		
	case quickman_pre_boss:
		d.shutter_anims.AddShutter(tile_pos(0, 4), false);
		d.shutter_anims.AddShutter(tile_pos(15, 4), true);
		break;
		
	case heatman_pre_boss:
		d.shutter_anims.AddShutter(tile_pos(0, 6), false);
		d.shutter_anims.AddShutter(tile_pos(15, 6), true);
		break;
		
	case heatman_hell:
		a.Spawn(d, telly_spawner, tile_pos(11, 5));
		a.Spawn(d, telly_spawner, tile_pos(14, 7));
		a.Spawn(d, telly_spawner, tile_pos(21, 4));
		a.Spawn(d, telly_spawner, tile_pos(32, 8));
		a.Spawn(d, telly_spawner, tile_pos(35, 7));
		a.Spawn(d, telly_spawner, tile_pos(38, 4));
		a.Spawn(d, telly_spawner, tile_pos(45, 4));
		a.Spawn(d, telly_spawner, tile_pos(57, 4));
		a.Spawn(d, crash_wall, tile_pos(43, 10));
		a.Spawn(d, crash_wall, tile_pos(53, 8));
		a.Spawn(d, crash_wall, tile_pos(62, 10));
		break;
	
	case heatman_start:
		a.Spawn(d, telly_spawner, tile_pos(42, 5));
		a.Spawn(d, telly_spawner, tile_pos(46, 7));
		a.Spawn(d, telly_spawner, tile_pos(52, 4));
		a.Spawn(d, telly_spawner, tile_pos(56, 6));
		a.Spawn(d, telly_spawner, tile_pos(61, 9));
		a.Spawn(d, telly_spawner, tile_pos(66, 5));
		a.Spawn(d, telly_spawner, tile_pos(72, 5));
		a.Spawn(d, telly_spawner, tile_pos(78, 3));
		a.Spawn(d, telly_spawner, tile_pos(84, 5));
		a.Spawn(d, telly_spawner, tile_pos(92, 5));
		a.Spawn(d, springer, tile_pos(119, 10));
		a.Spawn(d, springer, tile_pos(121, 6));
		a.Spawn(d, fly_boy, tile_pos(23, 1));
		a.Spawn(d, fly_boy, tile_pos(31, 1));
		a.Spawn(d, fly_boy, tile_pos(41, 1));
		break;
	
	case heatman_drop1:
		// a.SpawnYokuBlock(d, 6, 3, 70, 0, 80, 150);
		// a.SpawnYokuBlock(d, 10, 7, 70, 0, 80, 150);
		// a.SpawnYokuBlock(d, 10, 4, 70, 40, 120, 150);
		// a.SpawnYokuBlock(d, 7, 5, 70, 80, 150, 150);
		break;
	
	case quickman_start:
		d.items.Spawn(extra_life, tile_pos(3, 5));
		break;
	case quickman_drop1:
		a.Spawn(d, scworm_spawner, tile_pos(9, 10));
		a.Spawn(d, scworm_spawner, tile_pos(5, 12));
		break;
	case quickman_beams1:
		a.Spawn(d, springer, tile_pos(7, 12));
		QuickmanForceBeam(16, 4, false, beam_delay);
		QuickmanForceBeam(0, 7, false, beam_delay);
		break;
	case quickman_beams2:
		QuickmanForceBeam(0, 5, false, beam_delay);
		QuickmanForceBeam(0, 8, false, beam_delay);
		QuickmanForceBeam(0, 12, false, beam_delay);
		break;
	case quickman_beams3:
		QuickmanForceBeam(16, 4, false, beam_delay);
		QuickmanForceBeam(16, 6, false, beam_delay);
		QuickmanForceBeam(0, 8, false, beam_delay);
		QuickmanForceBeam(16, 12, false, beam_delay);
		a.Spawn(d, springer, tile_pos(9, 11));
		break;
	case quickman_beams_end:
		d.items.Spawn(energy_can, tile_pos(8, 10));
		d.items.Spawn(big_wp, tile_pos(12, 10) + Vec2(1, 0));
		d.items.Spawn(extra_life, tile_pos(10, 6));
		break;
	case quickman_torches:
		a.Spawn(d, fire_maker, tile_pos(21, 8), Vec2(0, 0), -1);
		a.Spawn(d, fire_maker, tile_pos(33, 10), Vec2(0, 0), -1);
		a.Spawn(d, fire_maker, tile_pos(49, 10), Vec2(0, 0), -1);
		break;
	case quickman_more_beams1:
		QuickmanForceBeam(0, 1, true, beam_delay);
		QuickmanForceBeam(0, 3, true, beam_delay);
		QuickmanForceBeam(0, 5, true, beam_delay);
		QuickmanForceBeam(0, 7, true, beam_delay);
		QuickmanForceBeam(0, 10, true, beam_delay);
		QuickmanForceBeam(0, 11, true, beam_delay);
		break;
	case quickman_more_beams2:
		QuickmanForceBeam(0, 3, true, beam_delay);
		QuickmanForceBeam(0, 5, true, beam_delay);
		QuickmanForceBeam(0, 7, true, beam_delay);
		QuickmanForceBeam(0, 9, true, beam_delay);
		QuickmanForceBeam(0, 11, true, beam_delay);
		break;
	case quickman_more_beams3:
		QuickmanForceBeam(0, 1, true, beam_delay);
		QuickmanForceBeam(0, 2, true, beam_delay);
		QuickmanForceBeam(16, 10, false, beam_delay);
		d.items.Spawn(big_wp, tile_pos(12, 12));
		break;
	case quickman_more_beams4:
		QuickmanForceBeam(16, 1, false, beam_delay);
		QuickmanForceBeam(16, 3, false, beam_delay);
		QuickmanForceBeam(0, 6, false, beam_delay);
		QuickmanForceBeam(0, 8, false, beam_delay);
		QuickmanForceBeam(16, 10, false, beam_delay);
		break;
	case quickman_more_beams5:
		d.items.Spawn(big_hp, tile_pos(7, 6));
		d.items.Spawn(extra_life, tile_pos(7, 11));
		QuickmanForceBeam(16, 6, false, beam_delay);
		QuickmanForceBeam(16, 11, false, beam_delay);
		break;
	case quickman_more_beams6:
		QuickmanForceBeam(16, 2, false, beam_delay);
		QuickmanForceBeam(16, 4, false, beam_delay);
		QuickmanForceBeam(16, 6, false, beam_delay);
		QuickmanForceBeam(16, 7, false, beam_delay);
		QuickmanForceBeam(0, 10, false, beam_delay);
		break;
	case quickman_more_beams7:
		d.items.Spawn(big_wp, tile_pos(7, 8));
		QuickmanForceBeam(16, 1, false, beam_delay);
		QuickmanForceBeam(16, 2, false, beam_delay);
		QuickmanForceBeam(0, 6, false, beam_delay);
		QuickmanForceBeam(16, 10, false, beam_delay);
		break;
	case quickman_armor_joes:
		d.shutter_anims.AddShutter(tile_pos(47, 4), true);
		a.Spawn(d, armor_joe, tile_pos(16, 12), Vec2(0, 0), -1);
		a.Spawn(d, armor_joe, tile_pos(30, 12), Vec2(0, 0), -1);
		break;
	}
}
