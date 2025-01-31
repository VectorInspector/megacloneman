// #include "player_chara.h"
#include "game_data.h"

PlayerChara::PlayerChara () {
	ground_timer		= 0;
	slide_timer			= 0;
	hurt_timer			= 0;
	ladder_timer		= 0;
	move_timer			= 0;
	kill_timer			= 0;
	shoot_timer			= 0;
	invul_timer			= 0;
	jump_timer			= 0;
	enters_ladder		= false;
	intangible			= false;
	deal_damage			= 0;
	dir					= 1;
	
	grab_timer			= 0;
	uppercut_timer		= 0;
	crash_timer			= 0;
	momentum_timer		= 0;
	charge_timer		= 0;
	
	death_anim_ticks	= 40;
	invul_ticks			= 90;
	hurt_ticks			= 60;
	kill_ticks			= 140;
	slide_ticks			= 40;
	switch_ticks		= 15;
	hp					= 28;	
	
	kill_ticks			= 180;
	hurt_ticks			= 40;
	invul_ticks			= 90;
	shoot_ticks			= 35;
	ladder_transition_ticks	= -12;
	allow_slide			= true;
	allow_charge		= true;
	allow_uppercut		= false;
	allow_grab			= false;
	
	TickHitboxes();
}

void PlayerChara::TickHitboxes () {
	if(0 < slide_timer) {
		tile_collision_size		= Vec2(7.5, 12);
		hitbox_size				= Vec2(15, 12);
	}
	
	else {
		tile_collision_size		= Vec2(7.5, 22);
		hitbox_size				= Vec2(15, 22);
	}
	
	hitbox_pos = pos - Vec2(hitbox_size.x / 2, hitbox_size.y);
}

void PlayerChara::StartDropAnim () {
	drop_timer = std::max(1, drop_timer);
}

void PlayerChara::StartSwitchAnim () {
	switch_timer = std::max(1, switch_timer);
}

void PlayerChara::TickDrop (GameData& d) {
	// Start the drop animation just outside the room, over the viewable area.
	if(1 == drop_timer) {
		auto start_pos	= pos;
		drop_pos		= d.current_room.RoomTilePos(d.room_size, start_pos);
		drop_pos.x		= start_pos.x;
		drop_pos.y		-= 32;
		drop_end_pos	= drop_pos;
		drop_end_pos.y	= start_pos.y;
		drop_vel		= Vec2(0, 8.5);
	}

	if(1 == switch_timer) {
		// Play blip sound...
	}

	// Ticks! These ensure the animations eventually finish.
	if(0 < switch_timer) {
		switch_timer++;
		
		if(switch_ticks < switch_timer) {
			switch_timer = 0;
		}
	}

	if(0 < drop_timer) {
		drop_pos += drop_vel;
		drop_timer++;
		
		// Landing!
		if(drop_end_pos.y <= drop_pos.y) {
			drop_pos.y = drop_end_pos.y;
			drop_timer = 0;
			StartSwitchAnim();
		}
	}
}

void PlayerChara::Tick (GameData& d) {
	
	// Trigger miss animation if you have no HP left.
	if(hp <= 0) {
		kill_timer = std::max(1, kill_timer);
	}
	
	// PEW PEW PEW...
	if(0 < kill_timer) {
		if(30 == kill_timer) {
			d.anims.SpawnQuadRing(death_anim, pos - Vec2(0, 12), 2, 2, 9, 0);
		}
		
		kill_timer++;
		return;
	}
	
	TileClipping clipping(d.tiles);
	TileScan scan(d.tiles);
	
	auto gravity				= 0.185;
	auto terminal_speed			= 6.0;
	auto move_speed				= 2.75 / 2;
	auto hor_accel				= 0.23;
	auto hor_decel				= 0.23;
	auto jump_speed				= 4.4;
	auto climbing_speed			= 1;
	auto slide_speed			= 3;
	
	// For reference, this allows checking whether the player character has moved at all.
	old_pos				= pos;
	TickHurt();
	
	// Allow moving horizontally!
	auto input			= d.game_input;
	hor_input			= 0;
	
	if(input.IsHeld(d.right)) { hor_input += 1; }
	if(input.IsHeld(d.left)) { hor_input -= 1; }
	
	// Allow shooting!
	if(
	((45 < charge_timer && input.IsReleased(d.shoot)) || input.IsPressed(d.shoot)) &&
	0 == grab_timer && 0 == uppercut_timer && 0 == crash_timer && 0 == hurt_timer) {
		shoot_timer = shoot_ticks;
	}
	
	if(input.IsHeld(d.shoot)) {
		charge_timer++;
	}
	
	// No movement or climbing when hurt, though.
	if(0 < hurt_timer) {
		hor_input		= 0;
		ladder_timer	= 0;
		shoot_timer		= 0;
	}
	
	// Transitioning in or out of a ladder (when you climb from a grounded position down a ladder,
	// or up a ladder into a grounded position at the top).
	if(ladder_timer < 0) {
		vel = Vec2(0, 0);
		ladder_timer++;
		
		// Check if this transition animation is supposed to lead into a ladder. If enters_ladder
		// is set, that means "I pressed down at the top of a ladder I stood on", which means the
		// transition leads to a ladder!
		if(0 <= ladder_timer && enters_ladder) {
			ladder_timer = 1;
			enters_ladder = false;
		}
	}
	
	// Ladder movement.
	else if(0 < ladder_timer) {
		hor_input		= 0;
		vel				= Vec2(0, 0);
		
		// Not many ways to go. Just up, down or jump off.
		if(input.IsHeld(d.up)) { vel.y = -climbing_speed; }
		if(input.IsHeld(d.down)) { vel.y = climbing_speed; }
		
		// Movement begins here in two steps. Check for head bump and then offsetting vertically.
		clipping(pos, vel, 0, tile_collision_size.y);
		
		// Head bump or currently shooting.
		if(clipping.Below()) { vel.y = 0; }
		if(0 < shoot_timer) { vel.y = 0; }
		
		// The timer counts up only if you're doing a climbing motion. This handles animation.
		pos = clipping(pos, vel);
		if(pos != old_pos) { ladder_timer++; }
		
		// You're jumping off.
		if(input.IsPressed(d.jump)) { ladder_timer = 0; ground_timer = 0; }
		
		// You climbed all the way down and touched a floor.
		if(clipping.Above()) { ladder_timer = 0; ground_timer = 1; vel = Vec2(0, 0); }
		
		// Check reaching the top of the ladder.
		d.tiles.InfoAtPos(pos - Vec2(0, d.tiles.tile_size));
		auto is_free = d.tiles.IsFree(d.tiles.__type);
		
		if(is_free && vel.y < 0) {
			ladder_timer	= ladder_transition_ticks;
			pos.y			= d.tiles.__br.y;
			ground_timer	= 1;
		}
		
		// Check reaching the bottom of the ladder (not touching a floor, just falling off a ladder)
		if(is_free && 0 < vel.y) {
			ladder_timer	= 0;
		}
	}
	
	// Handle shooting.
	if(shoot_ticks == shoot_timer) {
		if(60 < charge_timer) {
			d.projectiles.Spawn(charge_shot, pos + Vec2(dir * 18, -13), Vec2(dir * 7, 0));
		}
		
		else {
			d.projectiles.Spawn(buster_shot, pos + Vec2(dir * 18, -13), Vec2(dir * 7, 0));
		}
		
		charge_timer = 0;
	}
	
	if(input.IsUp(d.shoot)) {
		charge_timer = 0;
	}
	
	if(0 < shoot_timer) {
		shoot_timer--;
	}
	
	// Accelerate and deccelerate when input comes through.
	if(0 < hor_input) { vel.x = std::min(vel.x + hor_accel, move_speed); }
	if(hor_input < 0) { vel.x = std::max(vel.x - hor_accel, -move_speed); }
	
	// Braking is here, but sometimes, you want momentum to be retained.
	if(0 == hor_input && momentum_timer < 1) {
		if(0 < vel.x) { vel.x = std::max(0.0, vel.x - hor_decel); }
		if(vel.x < 0) { vel.x = std::min(0.0, vel.x + hor_decel); }
	}
	
	// Count momentum timer up which doesn't do much at this point.
	if(0 < momentum_timer) {
		momentum_timer++;
	}
	
	// You just fell from a tile!
	if(ground_timer < 0) {
		// Cancel sliding, if you did it before.
		slide_timer = 0;
		
		// Keep falling.
		ground_timer = 0;
	}
	
	// Now clip. This will move the character horizontally and vertically where possible.
	pos = clipping(pos, vel, tile_collision_size.x, tile_collision_size.y);
	
	// Check if you land from a fall.
	if(clipping.Above()) {
		ground_timer = std::max(1, ground_timer);
	}
	
	// You're in the air!
	if(ground_timer < 1) {
		vel.y = std::min(gravity + vel.y, terminal_speed);
	}
	
	// You just landed!
	if(1 == ground_timer) {
		// play landing sound here.
		momentum_timer = 0;
		uppercut_timer = 0;
		shoot_timer = 0;
		ground_timer++;
	}
	
	Vec2 clip_pos(0, 0);
	
	// You're grounded in general. You have no vertical velocity.
	if(0 < ground_timer) {
		// std::cout << ground_timer << " frames on ground " << std::endl;
		
		vel.y = 0;
		
		// Check if you're about to fall by passing over an area without tiles just underneath your
		// feet, the width of your tile check hitbox!
		scan.RowPx(pos + Vec2(-tile_collision_size.x, 2), 2 * tile_collision_size.x);
		
		if(!scan()) {
			
			// Start falling! Next frame starts by initiating falling motion.
			ground_timer = -1;
		}
		
		// You have ground beneath! Pick a nice, close tile and take its y-value as height.
		else {
			pos.y = scan.__push_tl.y;
			clip_pos = Vec2(scan.__push_tl.x, scan.__push_tl.y);
		}
		
		// Initiate sliding!
		if(input.IsHeld(d.down) && input.IsHeld(d.left) && input.IsHeld(d.up) && input.IsHeld(d.right)) {
			d.bar_filling.Start(fill_player_hp, 32);
		}
		
		if(allow_slide && input.IsHeld(d.down) && input.IsPressed(d.jump) && slide_timer < 1 && hurt_timer < 1) {
			slide_timer = 1;
		}
		
		// Initiate jumping!
		else if(input.IsPressed(d.jump) && hurt_timer < 1) {
			vel.y			= -jump_speed;
			jump_timer		= 1;
			ground_timer	= 0;
			
			// If you jump from a slide, please retain momentum.
			if(0 < slide_timer) {
				slide_timer		= 0;
				momentum_timer	= 1;
			}
		}
		
		// While sliding, you travel forwards by a fixed speed.
		if(0 < slide_timer) {
			vel.x = dir * slide_speed;
			slide_timer++;
			
			// Some effects!
			if(0 == slide_timer % 5) {
				d.anims.Spawn(smoke_anim, pos - Vec2(dir * 6, 0), Vec2(0, -1.5));
			}
			
			// You can only stop sliding if there's enough space to do so.
			scan.RowPx(pos + Vec2(-tile_collision_size.x, -d.tiles.tile_size - 2), 2 * tile_collision_size.x);
			
			if(slide_ticks <= slide_timer && !scan()) {
				slide_timer = 0;
				vel.x = 0;
			}
		}
	}
	
	// Try to initiate the Yuka grab.
	if(0 != grab_timer) {
		grab_timer++;
	}
	
	if(allow_grab && ground_timer <= 0 && uppercut_timer < 1 && hurt_timer < 1 && 0 == grab_timer && input.IsHeld(d.down) && input.IsPressed(d.jump)) {
		grab_timer = 1;
	}
	
	if(0 < grab_timer && grab_timer < 8) {
		vel = Vec2(0, 0);
	}
	
	else if(12 <= grab_timer) {
		vel = Vec2(dir * 3.5, 0);
	}
	
	if(40 < grab_timer) {
		vel = Vec2(dir * 2, 0);
		grab_timer = -15;
	}
	
	// When Yuka grabs someone, she goes into the "crash" animation!
	// if(1 == crash_timer) { Object.S_StartSound("megaman/grab", 1337); }
	
	if(0 != crash_timer) {
		crash_timer++;
		grab_timer = 0;
		slide_timer = 0;
		intangible = true;
	}
	
	if(2 == crash_timer) { vel.y = -12.5; }
	// if(15 <= crash_timer) { vel = (0, 9.5); }
	// if(2 < crash_timer) { vel.y += 0.666; }
	
	if(clipping.Above() && 0 < crash_timer) {
		vel = Vec2(0, 0);
		crash_timer = -30;
		d.actors.Spawn(d, big_explosion, pos, Vec2(5, 65), 4);
		d.ShakeScreen(30);
	}
	
	if(crash_timer < 0) {
		vel = Vec2(0, 0);
	}
	
	if(-1 == crash_timer) {
		ground_timer = 0;
		vel.y = -4;
	}
	
	// Try to initiate shoryuka.
	if(allow_uppercut && uppercut_timer < 1 && hurt_timer < 1 && 0 == grab_timer && input.IsHeld(d.up) && input.IsPressed(d.jump)) {
		uppercut_timer = 1;
	}
	
	// Uppercutting?
	if(1 == uppercut_timer) {
		slide_timer = 0;
	}
	
	if(0 < uppercut_timer && uppercut_timer <= 12) {
		vel = Vec2(dir * 2, 0);
	}
	
	else if(0 < uppercut_timer && uppercut_timer < 24) {
		vel = Vec2(dir * 3, -8);
		ground_timer = 0;
	}
	
	else if(0 < uppercut_timer && 50 < uppercut_timer) {
		uppercut_timer = -14;
	}
	
	if(0 != uppercut_timer) {
		uppercut_timer++;
	}
	
	// Certain things that make you invulnerable or painful to touch and such.
	intangible = 0 != uppercut_timer || 0 != grab_timer || 0 != crash_timer;
	deal_damage = 0 != uppercut_timer ? 6 : 0;
	
	// This merely tracks jumping. The timer only goes up if starting from a jump, you keep
	// holding the jump button. If you let go while the character goes upwards, the vertical
	// velocity will be reset, allowing you to control the jump height.
	if(0 < jump_timer && input.IsHeld(d.jump)) {
		jump_timer++;
	}
	
	// You let go of a jump.
	else if(0 < jump_timer && vel.y < 0) {
		vel.y		= 0;
		jump_timer	= 0;
	}
	
	// Keep counting if grounded movement options haven't cancelled it yet.
	if(0 < ground_timer) {
		ground_timer++;
	}
	
	// Detect ladders. There are two ways to engage a ladder: By pressing down or by pressing up.
	// It depends on whether the character is in the air and which direction the chara climbs that
	// there may or may not be an animation for it.
	if(0 == ladder_timer) for(auto kk = 0; kk < 2; kk++) {
		decltype(d.down) inputs []	= { d.down, d.up };
		int time_val []				= { ladder_transition_ticks, 1 };
		// decltype(inputs) other_inputs {};
		
		// Check just below the chara, and then the tile above that.
		auto check_pos	= pos - Vec2(0, kk * d.tiles.tile_size - 1);
		d.tiles.InfoAtPos(check_pos);
		
		// You're now on a ladder. Congrats.
		if(d.tiles.IsLadder(d.tiles.__type) && input.IsPressed(inputs [kk])) {
			ladder_timer = time_val [kk];
			pos = d.tiles.__br;
			pos.x -= d.tiles.tile_size / 2;
			pos.y -= 1;
			
			// A special thing is the transition from a grounded state into a ladder. The negative
			// ladder time indicates "halt movement and use transition frame". However, it is not
			// enough for indicating whether this transition is due to getting off or getting on a
			// ladder.
			if(0 < ground_timer && ladder_timer < 0) {
				enters_ladder = true;
			}
			
			// ...gecause if you aren't on the ground when climbing down, there shouldn't be a 
			// transition frame to begin with.
			else {
				ladder_timer = 1;
			}
			
			// No more need to check, since you're now on a ladder.
			break;
		}
	}
	
	// Head bump on the ceiling?
	clipping(pos - Vec2(0, tile_collision_size.y), vel, tile_collision_size.x);
	if(clipping.Below()) { vel.y = 0; }
	
	// Change Direction.
	auto can_turn = hurt_timer < 1 && uppercut_timer < 1;
	
	if(can_turn && 0 < hor_input) { dir = 1; }
	if(can_turn && hor_input < 0) { dir = -1; }
	
	// Count moving timer if you're having horizontal movement of any kind!
	if(0 < std::abs(pos.x - old_pos.x)) { move_timer++; }
	else { move_timer = 0; }
	
	// And finally update hitboxes.
	TickHitboxes();
}

// The frame the character has is largely dependent on just the state of a few timers that indicate
// things like shooting, falling, walking, sliding, being on the ground or in the air...
int PlayerChara::AnimFrame (GameData& d) const {
	// auto start_timer = 0;
	
	if(30 < kill_timer) { return -1; }
	// if(start_timer < 0) { return 21 - ((-start_timer % 6) < 3); }
	
	// This handles walking, too! If the move_timer is increasing, that's a walk!
	auto frame = 6 + ((int)(std::floor(move_timer - 6) / 10) % 4);
	
	// Yuka's crashing grab of DEATH!
	if(grab_timer < 0) { return 27; }
	else if(12 < grab_timer) { return 26; }
	else if(0 < grab_timer) { return 25; }
	if(8 < crash_timer || crash_timer < 0) { return 29; }
	else if(0 < crash_timer) { return 28; }
	
	// Shoryukaken!
	if(uppercut_timer < 0) { return 25; }
	else if(8 < uppercut_timer) { return 24; }
	else if(0 < uppercut_timer) { return 23; }
	
	// Ouchies, sliding and ladder usage.
	if(0 < hurt_timer) { return 17; }
	if(0 < slide_timer) { return 22; }
	if(ladder_timer < 0) { return 16; }
	if(0 < ladder_timer) {
		if(0 < shoot_timer) { return 18; }
		return 14 + (ladder_timer % 18 < 9 ? 0 : 1);
	}
	
	// Shooting! Depending on which position the player character is while shooting, the frame is
	// advanced a bit, which allows the player to run and shoot, or jump and shoot!
	if(0 < shoot_timer) { frame += 4; }
	if(move_timer < 1) { frame = 0; if(0 < shoot_timer) { frame = 2; } }
	else if(move_timer < 6) { frame = 5; if(0 < shoot_timer) { frame = 2; } }
	if(ground_timer < 1) { frame = 3; if(0 < shoot_timer) { frame = 4; } }
	return frame;
}

// This handles other animation frames, namely when you are not really in control. Those include the
// weapon switching animation (same as landing animation) and drop-in animation when the level
// does start.
int PlayerChara::SubAnimFrame (GameData& d) const {
	auto frame = 19;
	
	if(0 < drop_timer) { frame = 19; }
	else if(switch_timer < 5) { frame = 20; }
	else if(switch_timer < 10) { frame = 21; }
	else if(switch_timer < 15) { frame = 20; }
	
	return frame;
}

void PlayerChara::TickHurt () {
	// You just got hurt!
	if(hurt_ticks == hurt_timer) {
		// play hurt sound here.
		vel = Vec2(0, 0);
	}
	
	// Still taking knockback!
	if(0 < hurt_timer) {
		dir = 0 == hurt_dir ? dir : -hurt_dir;
		vel.x = hurt_dir * 0.5;
	}
	
	// The hurt stun is ending, so have some invulnerability!
	if(1 == hurt_timer) {
		invul_timer = std::max(invul_timer, invul_ticks);
	}
	
	// Count down both hurt and invulnerability timers.
	hurt_timer		= std::max(0, hurt_timer - 1);
	invul_timer		= std::max(0, invul_timer - 1);
}

void PlayerChara::Kill () {
	hp = 0;
	vel = Vec2(0, 0);
	kill_timer = std::max(1, kill_timer);
}

bool PlayerChara::Hurt (GameData& d, int damage, int dir, bool ignore_invul) {
	if(d.invincible_cheat) {
		return false;
	}
	
	if(hurt_timer <= 0 && (invul_timer <= 0 || ignore_invul)) {
		auto new_hp = hp - damage;
		
		if(0 < new_hp) {
			hurt_timer = hurt_ticks;
			hurt_dir = dir;
			hp = new_hp;
		}
		
		else if(0 < hp) {
			Kill();
		}
		
		return true;
	}
	
	return false;
}
