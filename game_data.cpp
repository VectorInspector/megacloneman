#include "game_data.h"
#include "sdl.h"
#include <iostream>

GameData::GameData () : game_input(play_inputs::count), rng_dev(std::random_device {} ()), settings("mega_settings.txt") {
	gl_state					= nullptr;
	sdl_state					= nullptr;
	renderer					= nullptr;
	
	start_anim_timer			= 0;
	timer						= -1;
	showing_level_collision		= 0;
	showing_room_transitions	= 0;
	showing_actor_hitboxes		= 0;
	showing_proj_hitboxes		= 0;
	room_transition_timer		= 0;
	room_transition_frames		= 120;
	
	backend_params.window_width		= settings.Get <int> ("window_width", backend_params.window_width);
	backend_params.window_height	= settings.Get <int> ("window_height", backend_params.window_height);
	backend_params.borderless		= TrueFalseString(settings.Get <std::string> ("borderless_window", "false"));
	backend_params.resizable		= TrueFalseString(settings.Get <std::string> ("resizable_window", "true"));
	
	sdl_state					= new SdlState(backend_params);
	renderer					= new SdlRenderer(sdl_state->renderer);
	running						= true;
	play_scale					= 2;
	nearest_play_scale			= TrueFalseString(settings.Get <std::string> ("nearest_scaling", "true"));
	skip_start					= TrueFalseString(settings.Get <std::string> ("skip_start", "false"));
	invincible_cheat			= TrueFalseString(settings.Get <std::string> ("invincible", "false"));
	start_room					= (level_rooms)(settings.Get <int> ("start_room", quickman_start));
	start_x_tile				= settings.Get <int> ("start_x_tile", 0);
	start_y_tile				= settings.Get <int> ("start_y_tile", 0);
}

bool GameData::TrueFalseString (std::string&& str) {
	return "true" == str;
}

GameData::~GameData () {
	if(sdl_state) { std::cout << "SDL? "; delete sdl_state; std::cout << "SDL state out." << std::endl; }
	if(gl_state) { std::cout << "GL? "; delete gl_state; std::cout << "GL state out." << std::endl; }
	if(renderer) { std::cout << "Renderer? "; delete renderer; std::cout << "Renderer out." << std::endl; }
}

void GameData::Poll () {
	GameData& this_ref = *this;
	
	if(0 < timer) {
		timer++;
	}
	
	if(0 < showing_level_collision) { showing_level_collision++; }
	if(0 < showing_room_transitions) { showing_room_transitions++; }
	if(0 < showing_actor_hitboxes) { showing_actor_hitboxes++; }
	if(0 < showing_proj_hitboxes) { showing_proj_hitboxes++; }
	
	// This is where the game begins for the first time.
	if(-1 == timer) {
		// Basics about the visible play area and how big a room is in pixels.
		room_size = Vec2(256, 256);
		play_size = Vec2(256, 224);
		
		// Let's load some resources. The renderer, when destructed, will clear these resources
		// as well.
		// chara_sprite		= renderer->LoadPngSprite("graphics/yuka.png");
		chara_sprite		= renderer->LoadPngSprite("graphics/megaman.png");
		enemy_sprite		= renderer->LoadPngSprite("graphics/mmenemy.png");
		big_enemy_sprite	= renderer->LoadPngSprite("graphics/mmbigguy.png");
		checker_sprite		= renderer->LoadPngSprite("graphics/checker_tile.png", -1, Vec2(0, 0));
		anim_sprite			= renderer->LoadPngSprite("graphics/mmweapon.png", -1, Vec2(0.5, 0.5));
		item_sprite			= renderer->LoadPngSprite("graphics/mmitem.png", -1, Vec2(0.5, 1));
		bar_sprite			= renderer->LoadPngSprite("graphics/mmbar.png");
		font_sprite			= renderer->LoadPngSprite("graphics/mmfont.png", 8);
		quick_man_stage_sprite	= renderer->LoadPngSprite("graphics/quicstag.png", -1, Vec2(0, 0));;
		quick_man_sprite	= renderer->LoadPngSprite("graphics/mmquick.png");
		stage_sprite		= quick_man_stage_sprite;
		
		// Canvas to draw to and to scale up!
		sdl_target = renderer->CreateTarget(play_size.x, play_size.y);
		
		// How are the tiles?
		tiles = Tiles(16, 0.05);
		transition_areas = TransitionAreas(tiles.tile_size);
		timer = 1; // Start the game routine!
	}
		
	// Reset after losing a life.
	if(150 < chara.kill_timer) {
		timer = 1;
	}
	
	// This is where the game begins.
	if(1 == timer) {
		
		auto rrr = start_room;
		OnRoomLoad(this_ref, current_room, rrr);
		OnRoomSpawns(this_ref, current_room, rrr);
		camera.InnerOuter(room_size, current_room.Size(room_size));
		
		chara = PlayerChara {};
		chara.drop_timer	= 0;
		chara.pos			= Vec2(tiles.tile_size * start_x_tile, tiles.tile_size * start_y_tile);
		camera.ForceTo(chara.pos);
		start_anim_timer	= skip_start ? 0 : 1;
		
		chara.allow_charge = settings.Get("allow_charge", true);
		chara.allow_slide = settings.Get("allow_slide", true);
		chara.allow_uppercut = settings.Get("allow_uppercut", false);
		chara.allow_grab = settings.Get("allow_grab", false);
	}
	
	// if(timer % 60 == 0) {std::cout << "item count is " << items.count << std::endl;}
	
	// Gameplay when not frozen in a room transition! That's basically what happens mostly. The
	// game is also paused by hp bars filling.
	shutter_anims.TickShutters();
	bar_filling.Tick(this_ref);
	
	// The text that appears when a stage begins will be timed here.
	if(0 < start_anim_timer) {
		start_anim_timer++;
		
		// Here's what is supposed to happen after the text banner is done!
		if(start_anim_ticks < start_anim_timer) {
			chara.StartDropAnim();
			start_anim_timer = 0;
		}
	}
	
	// Drop animation handling for the player character!
	chara.TickDrop(this_ref);
	
	// And all this stuff is the main gameplay, which actually does not happen during a lot of the
	// transition animations the game has!
	if(0 == room_transition_timer && bar_filling.IsDone() && chara.drop_timer < 1 && start_anim_timer < 1 && chara.switch_timer < 1) {
		projectiles.Tick(this_ref);
		projectiles.TickDisappear(this_ref);
		items.Tick(this_ref);
		actors.Tick(this_ref);
		beams.TickForceBeams(this_ref);
		bosses.Tick(this_ref);
		chara.Tick(this_ref);
		anims.Tick();
		camera.Tick(chara.pos);
		
		shake_timer = std::max(0, shake_timer - 1);
		
		// Check if the player character touched a room transition.
		auto check_size		= Vec2(24, 32);
		auto check_pos		= chara.pos - Vec2(check_size.x / 2, check_size.y);
		
		for(auto kk = 0; kk < transition_areas.count; kk++) {
			if(HitboxIntersectsHitbox(check_pos, check_size, transition_areas.pos [kk], transition_areas.size [kk])) {
				TransitionToRoom(transition_areas.leads_to [kk], 0, 0, transition_areas.dir [kk]);
				break;
			}
		}
		
		// Check for collisions between actors, projectiles and the player.
		auto proj_vs_actor_damage = [&] (int proj, int actor) {
			if(charge_shot == proj) {
				return 4;
			}
			
			return 2;
		};
		
		auto proj_vs_player_damage = [&] (int proj) {
			return 4;
		};
		
		auto check_col_actor_vs_player = [&] (Actors& act, PlayerChara& c) {
			auto hb_pos		= c.hitbox_pos;
			auto hb_size	= c.hitbox_size;
			
			for(auto kk = 0; kk < act.count; kk++) {
				auto& cur_act = act.mo [kk];
				
				for(auto hh = 0; hh < cur_act.hitbox_count; hh++) {
					if(
					cur_act.is_hitbox_active [hh] &&
					cur_act.is_hitbox_deadly [hh] &&
					HitboxIntersectsHitbox(hb_pos, hb_size, cur_act.HitboxPos(hh), cur_act.HitboxSize(hh)) ) {
						if(0 < c.deal_damage && 0 == cur_act.grab_timer) {
							cur_act.TakeDamage(c.deal_damage);
						}
						
						else if(0 < c.grab_timer && 0 == cur_act.grab_timer) {
							cur_act.grab_timer = std::max(1, cur_act.grab_timer);
							c.grab_timer = 0;
							c.crash_timer = 1;
						}
						
						else if(!c.intangible && 0 == cur_act.grab_timer) {
							c.Hurt(this_ref, cur_act.hitbox_damage [hh], sgn(c.pos.x - cur_act.pos.x));
						}
						
						cur_act.CountCollision(hh);
					}
				}
			}
		};
		
		// Variant for when the player hurts enemies instead.
		auto check_col_player_vs_actor = [&] (Actors& act, PlayerChara& c) {
			auto hb_pos		= c.hitbox_pos;
			auto hb_size	= c.hitbox_size;
			
			for(auto kk = 0; kk < act.count; kk++) {
				auto& cur_act = act.mo [kk];
				
				for(auto hh = 0; hh < cur_act.hitbox_count; hh++) {
					if(
					cur_act.is_hitbox_active [hh] &&
					HitboxIntersectsHitbox(hb_pos, hb_size, cur_act.HitboxPos(hh), cur_act.HitboxSize(hh)) ) {
						cur_act.TakeDamage(c.deal_damage);
					}
				}
			}
		};
		
		auto check_col_proj_vs_player = [&] (Projectiles& proj, PlayerChara& c) {
			auto hb_pos		= c.hitbox_pos;
			auto hb_size	= c.hitbox_size;
			
			if(0 < c.hp) for(auto kk = 0; kk < proj.count; kk++) {
				if(
				proj.hits_players [kk] &&
				CircleIntersectsHitbox(proj.pos [kk], proj.size [kk], hb_pos, hb_size)) {
					proj.timer [kk] = 0;
					c.Hurt(this_ref, proj_vs_player_damage(proj.type [kk]), sgn(proj.vel [kk].x), false);
				}
			}
		};
		
		auto check_col_beams_vs_player = [&] (Actors& act, PlayerChara& c) {
			auto hb_pos		= c.hitbox_pos;
			auto hb_size	= c.hitbox_size;
			
			if(0 < c.hp) for(auto kk = 0; kk < act.count; kk++) {
				auto& cur_act	= act.mo [kk];
				auto fb_pos		= cur_act.ForceBeamHitboxPos(this_ref);
				auto fb_size	= cur_act.ForceBeamHitboxSize(this_ref);
				
				// It's instant death if it touches you.
				if(HitboxIntersectsHitbox(hb_pos, hb_size, fb_pos, fb_size)) {
					c.Hurt(this_ref, 143);
				}
			}
		};
		
		auto check_col_proj_vs_actors = [&] (Projectiles& proj, Actors& act) {
			
			// Each projectile.
			for(auto kk = 0; kk < proj.count; kk++) {
				if(!proj.hits_enemies [kk]) {
					continue;
				}
				
				auto proj_size = proj.size [kk];
				auto proj_pos = proj.pos [kk];
				
				// Each actor.
				for(auto mm = 0; mm < act.count; mm++) {
					auto& cur_act = act.mo [mm];
					
					// And each actor can have a whole bunch of hitboxes...
					for(auto hh = 0; hh < cur_act.hitbox_count; hh++) {
						auto hb_tl = cur_act.HitboxPos(hh);
						auto hb_size = cur_act.HitboxSize(hh);
						
						if(
						cur_act.is_hitbox_active [hh] &&
						CircleIntersectsHitbox(proj_pos, proj_size, hb_tl, hb_size)) {
							proj.pre_hit_counter [kk]++;
							
							// First of all, is this thing even damaging to this actor?
							auto possible_damage = proj_vs_actor_damage(proj.type [kk], cur_act.type_id);
							
							// TINK!
							if(cur_act.is_hitbox_reflective [hh] || possible_damage <= 0) {
								proj.vel [kk] = Vec2(-proj.vel [kk].x, -5);
							}
							
							// Ouch!
							else if(cur_act.is_hitbox_shootable [hh]) {
								cur_act.TakeDamage(possible_damage);
								
								// Ripping projectiles will continue if they kill their target.
								// Others will disappear! (Timer set to 0 means disappear.)
								if(!proj.rips [kk] || 0 < cur_act.hp) {
									proj.timer [kk] = 0;
								}
							}
						}
					}
				}
			}
			
			for(auto kk = 0; kk < proj.count; kk++) {
				proj.hit_counter [kk] = proj.pre_hit_counter [kk];
				proj.pre_hit_counter [kk] = 0;
			}
			
			// Look at this staircase which will not please the flat code society. But tell me what
			// you would rather do... pick through multiple files to find each loop-body, through
			// multiple functions (lambdas), or have it all in the same place, looking funny like
			// this? HMMMMM...
		};
		
		check_col_actor_vs_player(actors, chara);
		check_col_actor_vs_player(bosses, chara);
		check_col_proj_vs_player(projectiles, chara);
		check_col_beams_vs_player(beams, chara);
		check_col_proj_vs_actors(projectiles, actors);
		check_col_proj_vs_actors(projectiles, bosses);
	}
	
	// I wonder if templates or auto-type deduction can do this.
	// auto f = [&] (const auto& the_list) {
		// if(0 < the_list.count) { std::cout << the_list.pos [0] << std::endl; }
	// };
	
	// f(projectiles);
	// f(items);
	// f(anims);
	// f(actors);
	// OK auto type deduction is CRACKED.
	
	// Transition has started.
	if(-1 == room_transition_timer) {
		// Store the current room into the slot which is now the leaving room. The room that appears
		// to be scrolling into the screen is now the current room, and has its collision tiles
		// already loaded, as well as its dimensions and transition areas set.
		leaving_room = current_room;
		actors.Clear();
		beams.Clear();
		items.Clear();
		transition_areas.Clear();
		projectiles.Clear();
		anims.Clear();
		shutter_anims.TriggerShutters(false);
		// items.Clear();
		OnRoomLoad(this_ref, current_room, room_transition_room);
		camera.InnerOuter(room_size, current_room.Size(room_size));
		
		// This will calculate a few things pertaining to the visual offsets the leaving and current
		// room are taking during the animation.
		room_transition_info = RoomTransitionInfo(room_size, leaving_room, current_room, room_transition_x, room_transition_y, room_transition_dir, tiles.tile_size);
		room_transition_info(chara.pos);
		room_transition_timer = 1;
	}
	
	// Transition is ongoing.
	if(0 < room_transition_timer) {
		chara.pos = Linear(
			room_transition_timer,
			0, room_transition_info.scroll_anim_pos1,
			room_transition_frames, room_transition_info.scroll_anim_pos2);
		
		room_transition_timer++;
		
		// Transition finishes.
		if(room_transition_frames <= room_transition_timer) {
			shutter_anims.Clear();
			OnRoomSpawns(this_ref, current_room, room_transition_room);
			shutter_anims.TriggerShutters(true);
			chara.pos = room_transition_info.pos_in_dest_room;
			camera.ForceTo(chara.pos);
			room_transition_timer = 0;
		}
	}
	
	// Are we running on SDL? Then we are going to do SDL stuff!
	auto ticks_per_second = SDL_GetPerformanceFrequency();
	
	if(sdl_state) {
		old_ticks = SDL_GetPerformanceCounter();
		
		auto& event = sdl_state->event;
		
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
			
			case SDL_QUIT:
				running = false;
				break;
			
			// Handle things such as resize!
			case SDL_WINDOWEVENT:
				
				// YEAH... event.window.event. "window.event" is the thing that happened which in
				// our case is resize.
				if(SDL_WINDOWEVENT_RESIZED == event.window.event) {
					
					// Readable names.
					auto width		= event.window.data1;
					auto height		= event.window.data2;
					
					// Now make sure the SDL renderer knows how to reposition the screen.
					renderer->default_target_size = Vec2(width, height);
				}
				
				break;
			
			}
		}
	
		int num_keys = 0;
		auto key_state = SDL_GetKeyboardState(&num_keys);
		
		if(0 < num_keys) {
			auto bind_down = [&] (const char* bind_name) {
				return sdl_state->IsKeyDown(settings.Get <std::string> (bind_name, "up"));
			};
			
			game_input.Tick(up, bind_down("up_key"));
			game_input.Tick(left, bind_down("left_key"));
			game_input.Tick(down, bind_down("down_key"));
			game_input.Tick(right, bind_down("right_key"));
			game_input.Tick(quit, bind_down("quit_key"));
			game_input.Tick(shoot, bind_down("shoot_key"));
			game_input.Tick(jump, bind_down("jump_key"));
			game_input.Tick(special, bind_down("special_key"));
		}
		
		if(game_input.IsPressed(special)) {
			bar_filling.Start(1, 24);
		}
		
		// Testing room transition code.
		// if(game_input.IsPressed(shoot)) {
			// TransitionToRoom(quickman_drop1, 0, 0, dir_down);
		// }
	}
	
	// are we running on glfw? then things look a little different from SDL!
	else if(gl_state) {
	}
	
	// Game logic is not dependent on either these things now that all the data has been
	// well-read and well met, evidently..
	if(game_input.IsPressed(quit)) {
		running = false;
	}
	
	// Go back to polling the backend. Game logic is done and over with, and now we're
	// required to output the whole thing to the screen. One has two pretty potent options that
	// are fairly obvious:
	
	// (1) Implement an interface with things like "draw sprite", "clear background", etc. 
	// and have the scene drawn in an abstract manner, or
	
	// (2) Just have each backend render the scene completely by using the passed-in game data.
	
	// I opt for the latter because it's easier to setup, but it is admittedly more difficult in the
	// way that I will have to write multiple backend code to, say, draw projectiles in general.
	if(sdl_state) {
		// auto r = sdl_state->renderer;
		auto r = renderer;
		auto in_camera = [&] (const Vec2& pos) {
			return pos - camera.DrawPos();
		};
		
		// The real render area!
		r->Target(sdl_target);
		
		// Draw the stage background!
		auto draw_room = [&] (const Room& room, Vec2 pos = Vec2(0, 0)) {
			r->SpriteTo(stage_sprite); r->DrawCustomTiles(pos, room_size, room.x1, room.y1, room.x2, room.y2);
		};
		
		// In addition, boss shutters / gates!
		auto draw_shutters = [&] (const Anims& sh, Vec2 offset = Vec2(0, 0)) {
			r->Reset(); r->SpriteTo(anim_sprite);
		
			for(auto kk = 0; kk < sh.count; kk++) {
				auto f = sh.frame [kk];
				auto remainder_f = f % 8;
				auto full_tiles = f / 8;
				auto pos = in_camera(sh.pos [kk] + Vec2(16, 16)) - offset;
				
				for(auto mm = 0; mm < full_tiles; mm++) {
					r->DrawTile(pos, 38);
					pos += Vec2(0, 32);
				}
				
				if(0 < remainder_f) {
					r->DrawSplice(pos, 38 * r->Sprite().tile_size, 0, 39 * r->Sprite().tile_size, (int)remainder_f * 4);
				}
			}
		};
		
		r->Reset(); r->Rgb(30, 200, 255); r->Clear(); r->Reset();
		
		// During a transition, two rooms are drawn. One leaves, and one comes. For reference:
		// The transition is such that the camera and character are only warped when it ENDS, so
		// that's why the "leaving room" (the one the transition started in) is "in_camera" view.
		// "current" is called "current" because the tiles are already loaded.
		if(0 < room_transition_timer) {
			auto anim_offset	= room_transition_info.RoomScrollOffset(1.0 * room_transition_timer / room_transition_frames);
			auto leaving_pos	= -anim_offset;
			auto current_pos	= leaving_pos - room_transition_info.next_room_visual_offset + room_transition_info.next_room_dir_offset;
			
			draw_room(current_room, current_pos);
			draw_room(leaving_room, in_camera(leaving_pos));
			draw_shutters(shutter_anims, anim_offset);
		}
		
		// Outside that, one room is drawn with camera scrolling only.
		else {
			draw_room(current_room, in_camera(Vec2(0, 0)));
			draw_shutters(shutter_anims);
		}
		
		// Show collision map if desired!
		if(showing_level_collision) {
			r->Reset(); r->Rgb(255, 255, 255);
			auto size = Vec2(tiles.tile_size - 4, tiles.tile_size - 4);
			
			for(auto xx = 0; xx < tiles.x_size; xx++)
			for(auto yy = 0; yy < tiles.y_size; yy++) {
				auto tile = tiles.At(xx, yy);
				
				if(0 != tile) {
					r->DrawRect(Vec2(xx * tiles.tile_size + 2, yy * tiles.tile_size + 2), size);
				}
			}
		}
		
		// Show level transitions!
		if(showing_room_transitions) {
			r->Reset(); r->Rgb(0, 200, 100);
			for(auto mm = 0; mm < transition_areas.count; mm++) {
				r->DrawLineRect(transition_areas.pos [mm], transition_areas.size [mm]);
			}
		}
		
		// Collision boxes!
		if(showing_actor_hitboxes) {
			r->Reset(); r->Rgb(200, 150, 100);
			
			for(auto mm = 0; mm < actors.count; mm++) {
				r->DrawLineRect(actors.mo [mm].HitboxPos(0), actors.mo [mm].HitboxSize(0));
			}
		}
		
		// Draw the player character while controlled!
		if(chara.drop_timer < 1 && chara.switch_timer < 1 && start_anim_timer < 1) {
			auto frame			= chara.AnimFrame(this_ref);
			auto hurt_timer		= chara.hurt_timer;
			auto invul_timer	= chara.invul_timer;
			
			// The "ouch" effect during the hurt pose.
			if(0 < hurt_timer && (hurt_timer % 6) < 3) {
				r->SpriteTo(anim_sprite);
				r->DrawTile(in_camera(chara.pos - Vec2(0, 16)), 2, 0);
			}
			
			// Invulnerability flashing or normal drawing.
			else if(invul_timer < 1 || (invul_timer % 8) < 4) {
				r->SpriteTo(chara_sprite);
				r->Flip(0 < chara.dir);
				
				// While charging, flash!
				float flash_f = 0;
				auto flash_timer = chara.charge_timer;
				
				if(60 < flash_timer) {
					flash_f = (flash_timer % 3 < 2) ? 1 : 0.5;
				}
				
				else if(10 < flash_timer) {
					flash_f = (flash_timer % 5 < 2) ? 0.75 : 0.0;
				}
				
				r->DrawTileRgba(in_camera(chara.pos), frame, 0, 1, 1, 1, flash_f);
			}
			
			// Position indicator that helped centre the sprite properly.
			// r->DrawRect(in_camera(chara.pos), Vec2(2, 2));
		}
		
		// Draw the player character during the start animation or while switching weapons!
		else {
			auto frame = chara.SubAnimFrame(this_ref);
			r->SpriteTo(chara_sprite); r->Flip(0 < chara.dir);
			r->DrawTile(in_camera(chara.drop_pos), frame, 0);
		}
		
		// Draw the actors!
		auto draw_actors = [&] (const Actors& aa) {
			// auto hurt_flash_rate = 2;
			
			for(auto kk = 0; kk < aa.count; kk++) {
				const auto& cur_mo = aa.mo [kk];
				// DrawTextureFrame(tex, Camerad(pos, cell_size, (0.5, 1)), hor_flip: 0 < actors.mo [ii].dir, frame: aa.mo [ii].frame);
				
				auto hurt_timer = cur_mo.hurt_flash_timer;
				
				if(0 < hurt_timer && ((hurt_timer % 4) < 2)) {
					r->SpriteTo(anim_sprite);
					r->DrawTile(in_camera(cur_mo.pos - Vec2(0, 16)), 2);
				}
				
				else {
					r->Flip(0 < cur_mo.dir);
					r->SpriteTo(cur_mo.sprite_index);
					r->DrawTile(in_camera(cur_mo.pos), cur_mo.frame);
				}
			}
		};
		
		draw_actors(actors);
		draw_actors(bosses);
		
		// Draw the items!
		auto draw_items = [&] (const Items& mm) {
			r->Flip(false); r->SpriteTo(item_sprite);
			for(auto kk = 0; kk < mm.count; kk++) {
				auto frame = mm.frame [kk] + (timer / 8) % mm.frame_count [kk];
				r->DrawTile(in_camera(mm.pos [kk]), frame);
			}
		};
		
		draw_items(items);
		
		// Draw projectiles!
		auto draw_projectiles = [&] (const Projectiles& pp, int which_sprite) {
			r->SpriteTo(which_sprite);
			for(auto kk = 0; kk < projectiles.count; kk++) {
				r->Flip(0 < projectiles.vel [kk].x);
				r->DrawTile(in_camera(projectiles.pos [kk]), projectiles.frame [kk]);
			}
		};
		
		draw_projectiles(projectiles, anim_sprite);
		
		// Draw animations!
		auto draw_anims = [&] (const Anims& aa, int which_sprite) {
			r->SpriteTo(which_sprite); r->Flip(false);
			for(auto kk = 0; kk < anims.count; kk++) {
				r->DrawTile(in_camera(anims.pos [kk]), anims.frame [kk]);
			}
		};
		
		draw_anims(anims, anim_sprite);
		
		// Draw DEATH BEAMS!
		r->SpriteTo(anim_sprite);
		
		for(auto kk = 0; kk < beams.count; kk++) {
			const auto& cur_act = beams.mo [kk];
			auto len = std::abs(cur_act.pos.x - cur_act.old_pos.x);
			r->DrawExtendTile(in_camera(cur_act.old_pos - Vec2(0, 16)), 52, 0, 0 < cur_act.dir ? dir_right : dir_left, len);
		}
		
		// Draw any hp bars / weapon power (wp) bars!
		auto draw_bar = [&] (Vec2 pos, int amount) {
			r->Reset(); r->SpriteTo(bar_sprite);
			auto size			= r->Sprite().size;
			// r->Rgb(255, 255, 255); r->DrawRect(pos - Vec2(2, 2), size + Vec2(4, 4));
			// r->Rgb(0, 0, 0); r->DrawRect(pos - Vec2(1, 1), size + Vec2(2, 2));
			
			// if(amount <= 0) {
				// return;
			// }
			
			amount				= std::max(0, amount);
			auto blip_size		= 2;
			auto blip_count		= (int)size.y / blip_size;
			amount				= std::min(amount, blip_count);
			auto height			= amount * blip_size;
			
			r->DrawSprite(pos, false);
			if(0 < size.y - height) {
				r->Rgb(0, 0, 0); r->DrawRect(pos, Vec2(size.x, size.y - height));
			}
			
			// pos.y += size.y - height;
			// r->DrawSplice(pos, 0, size.y - height, size.x, height, false);
		};
		
		draw_bar(Vec2(16, 16), chara.hp);
		
		if(0 < bosses.count && 0 < bosses.mo [0].hp) {
			draw_bar(Vec2(play_size.x - 16, 16), bosses.mo [0].hp);
		}
		
		// The "ready" text when the game starts!
		if(0 < start_anim_timer && (start_anim_timer % 20 < 15)) {
			r->FontTo(font_sprite);
			r->DrawMonospaceText("Ready!", CompVec2(play_size, Vec2(0.5, 0.5)), Vec2(0.5, 0.5));
		}
		
		// Now draw the canvas into the window area.
		{
			r->Target(); r->TextureTo(sdl_target);
			r->Reset(); r->Rgb(230, 160, 90); r->Clear();
			
			// Offset the screen nicely. Use nearest scaling to fit the stuff in the box. An offset
			// of (0, 0) aligns the top left of the drawn area with the window, and (1, 1) aligns
			// the bottom right corners. (0.5, 0.5) naturally centres it!
			auto outer	= r->default_target_size;
			play_scale	= outer.y / play_size.y;
			
			// This does not fill every window height exactly, but it is the nicest possible
			// scaling which prevents pixel-swimming (every pixel is scaled by the same size).
			if(nearest_play_scale) {
				play_scale = std::floor(play_scale);
			}
			
			auto offset	= Vec2(0.5, 0.5);
			auto size	= play_scale * r->tex_size;
			Vec2 pos	= CompVec2(offset, outer - size);
			
			if(0 < shake_timer) {
				// pos.y += Linear(std::min(shake_timer, 60), 0, 0, 60, 5) * std::sin(shake_timer * 0.143);
				if((shake_timer % 4) < 2) {
					pos.y += 3;
				}
			}
			
			r->SpriteTo(checker_sprite); r->FillSprite(Vec2(0, 0), Vec2(0, 0), r->default_target_size);
			r->Rgb(255, 255, 255); r->DrawRect(pos - Vec2(6, 6), size + Vec2(12, 12));
			r->Rgb(0, 0, 0); r->DrawRect(pos - Vec2(4, 4), size + Vec2(8, 8));
			
			r->DrawTextureScale(pos, play_scale);
		}
		
		// Output! Yay.
		SDL_RenderPresent(sdl_state->renderer);
		
		// Now wait. Each frame takes a certain minimum amount of time. If the game happens to lag,
		// there will be no artificial speed up to compensate - each frame will still be individually
		// done, nonetheless! This is good for ease in code and not needing to worry about
		// accidentally clipping through collision or something, but can be bad if lag throws off
		// the player's timing due to relative speed changes!
		Uint64 dt = SDL_GetPerformanceCounter() - start_ticks;
		Uint64 freq = SDL_GetPerformanceFrequency();
		auto elapsed_milliseconds = 1.0 * dt / freq;
		SDL_Delay(1000 / 120);
	}
}

bool GameData::IsRunning () const { return running; }

// Signals the game to go out of the current room, scroll, and load the specified room in the
// specified x,y-tiles.
void GameData::TransitionToRoom (level_rooms which_room, int x, int y, Direction dir) {
	if(0 == room_transition_timer) {
		room_transition_timer = -1;
		room_transition_room = which_room;
		room_transition_x = x;
		room_transition_y = y;
		room_transition_dir = dir;
	}
}

bool GameData::CircleIntersectsHitbox (Vec2 cp, double cr, Vec2 hp, Vec2 hs) const {
	
	// Pick the position in the box that is closest to the circle in question.
	auto corner_pos	= ClampVec2(cp, hp, hp + hs);
	
	// Now distance check to it!
	auto diff		= corner_pos - cp;
	return diff.Dot(diff) <= cr * cr;
}

bool GameData::HitboxIntersectsHitbox (Vec2 p1, Vec2 s1, Vec2 p2, Vec2 s2) const {
	
	// Two axis-aligned hitboxes intersect of on each axis, the opposite corners would overlap.
	auto diff1 = p2 - (p1 + s1);
	auto diff2 = (p2 + s2) - p1;
	
	// Simply check if the signs are the same for both outer and inner differences.
	if(
	(diff1.x < 0 && diff2.x < 0) ||
	(diff1.y < 0 && diff2.y < 0)) {
		return false;
	}
	
	if(
	(0 < diff1.x && 0 < diff2.x) ||
	(0 < diff1.y && 0 < diff2.y)) {
		return false;
	}
	
	return true;
}

int GameData::Random (int low, int high) {
	return low + ((rng_dev() - rng_dev.min()) % (high - low + 1));
}

double GameData::Frandom (double low, double high) {
	auto f = 1.0 * (rng_dev() - rng_dev.min()) / (rng_dev.max() - rng_dev.min());
	return low + f * (high - low);
}

void GameData::ShakeScreen (int frames) {
	shake_timer = std::max(frames, shake_timer);
}
