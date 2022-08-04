#include "FPS.h"
#include "Input.h"

extern uint8_t CFG_FPS_ENABLED;

uint16_t deku_stick_timer_switch	= 0;
uint16_t last_time					= 0;
uint16_t started_timer				= 0;

uint8_t fps_switch					= 1;
uint8_t block						= 0;
uint8_t pressed_r					= 0;
uint8_t pressed_z					= 0;

void Handle_L_Button(GlobalContext* ctxt) {
	InputPad padReleased = ctxt->state.input[0].releaseEdge.buttons;
	
	if (ctxt->state.input[0].current.buttons.r)
		pressed_r = 1;
	if (ctxt->state.input[0].current.buttons.z)
		pressed_z = 1;
	if (padReleased.l && !pressed_r && !pressed_z) {
		Toggle_Minimap(ctxt);
	}
	if (!ctxt->state.input[0].current.buttons.l)
		pressed_r = pressed_z = 0;
	
	if (ctxt->state.input[0].pressEdge.buttons.l && !ctxt->state.input[0].current.buttons.r)
		block = 1;
	else if (ctxt->state.input[0].pressEdge.buttons.l && !ctxt->state.input[0].current.buttons.z)
		block = 1;
	if (!ctxt->state.input[0].current.buttons.l)
		block = 0;
	
	if (block) {
		ctxt->state.input[0].current.buttons.r = ctxt->state.input[0].pressEdge.buttons.r = 0;
		ctxt->state.input[0].current.buttons.z = ctxt->state.input[0].pressEdge.buttons.z = 0;
	}
	
	ctxt->state.input[0].pressEdge.buttons.l = 0;	
}

void Toggle_Minimap(GlobalContext* ctxt) {
	if (ctxt->pauseCtx.state != 0)
		return;
	uint8_t proceed = 0;
	
	if (ctxt->sceneNum == SCENE_SOUTH_CLOCK_TOWN || ctxt->sceneNum == SCENE_NORTH_CLOCK_TOWN || ctxt->sceneNum == SCENE_WEST_CLOCK_TOWN || ctxt->sceneNum == SCENE_EAST_CLOCK_TOWN || ctxt->sceneNum == SCENE_LAUNDRY_POOL || ctxt->sceneNum == SCENE_TERMINA_FIELD) {
		if (minimap_clock_town)
			proceed = 1;
	}
	else if (ctxt->sceneNum == SCENE_MILK_ROAD || ctxt->sceneNum == SCENE_GORMAN_TRACK || ctxt->sceneNum == SCENE_ROMANI_RANCH || ctxt->sceneNum == SCENE_DOGGY_RACETRACK || ctxt->sceneNum == SCENE_CUCCO_SHACK) {
		if (minimap_milk_road)
			proceed = 1;
	}
	else if (ctxt->sceneNum == SCENE_ROAD_TO_SOUTHERN_SWAMP || ctxt->sceneNum == SCENE_SOUTHERN_SWAMP || ctxt->sceneNum == SCENE_SOUTHERN_SWAMP_CLEAR || ctxt->sceneNum == SCENE_DEKU_PALACE_EXT || ctxt->sceneNum == SCENE_DEKU_PALACE_INT || ctxt->sceneNum == SCENE_WOODFALL) {
		if (minimap_woodfall)
			proceed = 1;
	}
	else if (ctxt->sceneNum == SCENE_ROAD_TO_MOUNTAIN_VILLAGE || ctxt->sceneNum == SCENE_MOUNTAIN_VILLAGE || ctxt->sceneNum == SCENE_MOUNTAIN_VILLAGE_CLEAR || ctxt->sceneNum == SCENE_ROAD_TO_SNOWHEAD || ctxt->sceneNum == SCENE_SNOWHEAD || ctxt->sceneNum == SCENE_ROAD_TO_GORON_VILLAGE || ctxt->sceneNum == SCENE_ROAD_TO_GORON_VILLAGE_CLEAR || ctxt->sceneNum == SCENE_GORON_RACETRACK || ctxt->sceneNum == SCENE_GORON_VILLAGE || ctxt->sceneNum == SCENE_GORON_VILLAGE_CLEAR || ctxt->sceneNum == SCENE_GORON_SHRINE) {
		if (minimap_snowhead)
			proceed = 1;
	}
	else if (ctxt->sceneNum == SCENE_GREAT_BAY_COAST_1 || ctxt->sceneNum == SCENE_PIRATES_FORTRESS_EXT || ctxt->sceneNum == SCENE_PIRATES_FORTRESS_INT || ctxt->sceneNum == SCENE_GREAT_BAY_COAST_2 || ctxt->sceneNum == SCENE_WATERFALL_RAPIDS || ctxt->sceneNum == SCENE_PINACLE_ROCK || ctxt->sceneNum == SCENE_ZORA_HALL) {
		if (minimap_great_bay)
			proceed = 1;
	}
	else if (ctxt->sceneNum == SCENE_ROAD_TO_IKANA_CANYON || ctxt->sceneNum == SCENE_GRAVEYARD || ctxt->sceneNum == SCENE_IKANA_CANYON || ctxt->sceneNum == SCENE_BOTTOM_OF_THE_WELL || ctxt->sceneNum == SCENE_ANCIENT_CASTLE_OF_IKANA || ctxt->sceneNum == SCENE_STONE_TOWER || ctxt->sceneNum == SCENE_STONE_TOWER_INV) {
		if (minimap_stone_tower)
			proceed = 1;
	}
	else if (ctxt->sceneNum == SCENE_WOODFALL_TEMPLE) {
		if (gSaveContext.perm.inv.dungeonItems[0].map)
			proceed = 1;
	}
	else if (ctxt->sceneNum == SCENE_SNOWHEAD_TEMPLE) {
		if (gSaveContext.perm.inv.dungeonItems[1].map)
			proceed = 1;
	}
	else if (ctxt->sceneNum == SCENE_GREAT_BAY_TEMPLE) {
		if (gSaveContext.perm.inv.dungeonItems[2].map)
			proceed = 1;
	}
	else if (ctxt->sceneNum == SCENE_STONE_TOWER_TEMPLE || ctxt->sceneNum == SCENE_STONE_TOWER_TEMPLE_INV) {
		if (gSaveContext.perm.inv.dungeonItems[3].map)
			proceed = 1;
	}
	
	if (!proceed)
		return;
	
	r_minimap_disabled ^= 1;
	
	if (r_minimap_disabled)
		z2_PlaySfx(0x4813);
	else z2_PlaySfx(0x4814);
}

void Handle_FPS(GlobalContext* ctxt) {
	
	if (!CFG_FPS_ENABLED || ctxt->pauseCtx.state != 0 || gSaveContext.extra.titleSetupIndex != 0 || gRspSegmentPhysAddrs.gameplayKeep == 0 || map_select_active == 0x00000222)
		return;
	
	if ( (ctxt->state.input[0].current.buttons.l && ctxt->state.input[0].pressEdge.buttons.z) || (ctxt->state.input[0].current.buttons.z && ctxt->state.input[0].pressEdge.buttons.l) ) {
		fps_switch ^= 1;
		if (fps_switch)
			z2_PlaySfx(0x4814);
		else z2_PlaySfx(0x4813);
	}
	
	if (!fps_switch || opening_door == 0x00800020)
		ctxt->state.framerateDivisor = link_animation_speed = 3;
	else if (text_state == 0x01 || jump_state == 0x0100 || opening_chest == 0x01 || var_801D7B44 == 0x01)
		ctxt->state.framerateDivisor = link_animation_speed = 2;
	else if (fps_switch)
		ctxt->state.framerateDivisor = link_animation_speed = 2;
	else if (playable_state == 0xFF08)
		ctxt->state.framerateDivisor = link_animation_speed = 3;
	else if (playable_state == 0x3208 && use_hookshot == 0x100B)
		ctxt->state.framerateDivisor  = link_animation_speed = 3;
	
	if (ctxt->state.framerateDivisor == 2) {
		if (gStaticContext.timeSpeed == 3)
			gStaticContext.timeSpeed = 2;
		
		var_801160AE = 0x0006;
		var_80116702 = 0x0000;
		
		if (jump_gravity == 0xC0B0)
			jump_gravity = 0xC050;
		else if (var_803FFE64 == 0x5008 && link_action == 0x4120)
			jump_gravity = 0xBF00;
		else if (jump_gravity == 0xBFB3 && link_action == 0x4140)
			jump_gravity = 0xBFD0;
		else if (jump_height > 0x4120 || link_action == 0x40E0 || link_action == 0x4110 || link_action == 0x4120 || link_action == 0x4150 || link_action == 0x4160 || link_action == 0x4170)
			jump_gravity = 0xBF34;
		
		if (var_803FFE64 == 0x3208) {
			if (link_action == 0x40A0 || link_action == 0x4110)
				var_8040000C = 0x3F4CCCCD;
		}
		else if (var_803FFE64 == 0x5008) {
			if (link_action == 0x40E0 || link_action == 0x4110 || link_action == 0x4120)
				var_8040000C = 0x3F333333;
		}
		else if (var_803FFE64 == 0xC808) {
			if (link_action == 0x4140 || link_action == 0x4198)
				var_8040000C = 0x3F800000;
		}
		
		if (var_8040000C == 0x3FA00000)
			var_8040000C =  0x3F900000;
		
		if (deku_stick_timer == 100 && !deku_stick_timer_switch) {
			deku_stick_timer += 100;
			deku_stick_timer_switch = 1;
		}
		else if (deku_stick_timer == 0)
			deku_stick_timer_switch = 0;
		
		/*if ( (z64_timer_type == 0x4 || z64_timer_type == 0x8 || z64_timer_type == 0xE) && !started_timer) {
			started_timer	= 1;
			last_time		= z64_time_remaining;
		}
		else if (z64_timer_type == 0 && started_timer)
			started_timer	= 0;
		
		if (z64_timer_type == 0x4 || z64_timer_type == 0x8) { // Decreasing
			if (last_time == z64_time_remaining + 3) {
				z64_time_remaining++;
				z64_seconds_left++;
				last_time = z64_time_remaining;
			}
		}
		else if (z64_timer_type == 0xE) { // Increasing
			if (last_time == z64_time_remaining - 3) {
				z64_time_remaining--;
				z64_seconds_left--;
				last_time = z64_time_remaining;
			}
		}*/
	}
	else if (ctxt->state.framerateDivisor == 3) {
		if (gStaticContext.timeSpeed == 2)
			gStaticContext.timeSpeed = 3;
	}
}