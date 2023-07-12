#include "Extra.h"
#include "Reloc.h"

extern uint8_t CFG_WS_ENABLED;
extern uint8_t CFG_DUAL_DPAD_ENABLED;
extern uint8_t CFG_B_BUTTON_ITEM_ENABLED;
extern uint8_t CFG_FPS_ENABLED;
extern uint8_t CFG_HIDE_HUD_ENABLED;
extern uint8_t CFG_OCARINA_ICONS_ENABLED;
extern uint8_t CFG_FLOW_OF_TIME_ENABLED;
extern uint8_t CFG_INSTANT_ELEGY_ENABLED;
extern uint8_t CFG_INVENTORY_EDITOR_ENABLED;
extern uint8_t CFG_INFINITE_HEALTH;
extern uint8_t CFG_INFINITE_MAGIC;
extern uint8_t CFG_INFINITE_AMMO;
extern uint8_t CFG_INFINITE_RUPEES;
extern uint8_t CFG_UNEQUIP_ENABLED;
extern uint8_t CFG_RUPEE_DRAIN;

uint16_t deku_stick_timer_switch	= 0;
uint16_t last_time					= 0;
uint16_t started_timer				= 0;
uint16_t elegy_timer                = 0;
uint16_t time_tracker[3]            = { 0, 0, 0 };

uint8_t rupee_drain_frames          = 0;
uint8_t rupee_drain_secs            = 0;

uint8_t dpad_alt                    = 0;
uint8_t fps_switch					= 1;
uint8_t hud_hide                    = 0;
uint8_t hud_hearts_hide				= 1;
uint8_t hud_counter					= 0;

uint8_t block						= 0;
PressedButtons pressed;

void Handle_Extra_Functions(GlobalContext* ctxt) {
	if (CFG_DUAL_DPAD_ENABLED) {
		if (ctxt->state.input[0].current.buttons.l && ctxt->state.input[0].releaseEdge.buttons.r) {
			dpad_alt ^= 1;
			if (dpad_alt)
				z2_PlaySfx(0x4813);
			else z2_PlaySfx(0x4814);
		}
	}
	
	Handle_FPS(ctxt);
	Handle_Quick_Pad(ctxt);
	Handle_L_Button_Ingame(ctxt);
	Handle_L_Button_Paused(ctxt);
	Handle_Infinite();
	
	//Handle_Saving(ctxt);
	Handle_Unequipping(ctxt);
	
	if (ctxt->pauseCtx.state == 6 && ctxt->pauseCtx.debugMenu == 0)
		ctxt->state.input[0].current.buttons.dr = ctxt->state.input[0].current.buttons.dl = 0;
}

void Handle_Rupee_Drain(ActorPlayer* player, GlobalContext* ctxt) {
	if (CFG_RUPEE_DRAIN == 0)
		return;
	if (ctxt->pauseCtx.state != 0 || gSaveContext.extra.fileIndex == 0xFF || gSaveContext.extra.titleSetupIndex != 0)
		return;
	if (player->stateFlags.state1 & PLAYER_STATE1_GROTTO_IN || player->stateFlags.state1 & PLAYER_STATE1_TIME_STOP  || player->stateFlags.state1 & PLAYER_STATE1_SPECIAL_2   || player->stateFlags.state1 & PLAYER_STATE1_GET_ITEM || player->stateFlags.state1 & PLAYER_STATE1_TIME_STOP_2 ||
		player->stateFlags.state1 & PLAYER_STATE1_DEAD      || player->stateFlags.state1 & PLAYER_STATE1_MOVE_SCENE || player->stateFlags.state1 & PLAYER_STATE1_TIME_STOP_3 || player->stateFlags.state2 & PLAYER_STATE2_OCARINA  || player->stateFlags.state2 & PLAYER_STATE2_FROZEN      ||
		player->stateFlags.state3 & PLAYER_STATE3_TRANS_PART)
		return;
	
	if (gSaveContext.perm.unk24.currentLife > 1) {
		rupee_drain_frames++;
	
		if (rupee_drain_frames >= 60 / ctxt->state.framerateDivisor) {
			rupee_drain_frames = 0;
			rupee_drain_secs++;
		}
	
		if (rupee_drain_secs >= CFG_RUPEE_DRAIN) {
			rupee_drain_secs = 0;
			
			if (gSaveContext.perm.unk24.rupees > 0)
				gSaveContext.perm.unk24.rupees--;
			else {
				if (gSaveContext.perm.unk24.currentLife > 7)
					gSaveContext.perm.unk24.currentLife -= 4;
				else gSaveContext.perm.unk24.currentLife = 1;
				z2_LinkInvincibility(player, 0x14);
				z2_PlaySfx(0x6848);
			}
		}
	}
	else rupee_drain_frames = rupee_drain_secs = 0;
}

void Handle_L_Button_Ingame(GlobalContext* ctxt) {
	if (!CFG_DUAL_DPAD_ENABLED && !CFG_FPS_ENABLED && !CFG_FLOW_OF_TIME_ENABLED && !CFG_INSTANT_ELEGY_ENABLED && !CFG_HIDE_HUD_ENABLED)
		return;
	if (ctxt->pauseCtx.state != 0 || gSaveContext.extra.fileIndex == 0xFF || gSaveContext.extra.titleSetupIndex != 0)
		return;
	
	InputPad padCurr = ctxt->state.input[0].current.buttons;
	
	if (padCurr.r)
		pressed.r = 1;
	if (padCurr.z)
		pressed.z = 1;
	if (padCurr.du)
		pressed.du = 1;
	if (padCurr.dr)
		pressed.dr = 1;
	if (padCurr.dd)
		pressed.dd = 1;
	if (padCurr.dl)
		pressed.dl = 1;
	
	if (ctxt->state.input[0].releaseEdge.buttons.l && !pressed.r && !pressed.z && !pressed.du && !pressed.dr && !pressed.dd && !pressed.dl) {
		Toggle_Minimap(ctxt);
		Hide_Hud(ctxt);
	}
	
	if (!padCurr.l)
		pressed.r = pressed.z = pressed.du = pressed.dr = pressed.dd = pressed.dl = 0;
	if (ctxt->state.input[0].pressEdge.buttons.l)
		block = 1;
	if (!padCurr.l)
		block = 0;
	if (block)
		ctxt->state.input[0].current.buttons.r = ctxt->state.input[0].current.buttons.z = ctxt->state.input[0].current.buttons.du = ctxt->state.input[0].current.buttons.dr = ctxt->state.input[0].current.buttons.dd = ctxt->state.input[0].current.buttons.dl = 0;
	ctxt->state.input[0].pressEdge.buttons.l = 0;	
}

void Handle_L_Button_Paused(GlobalContext* ctxt) {
	if (ctxt->pauseCtx.state != 6 || gSaveContext.extra.fileIndex == 0xFF || gSaveContext.extra.titleSetupIndex != 0)
		return;
	
	InputPad padCurr = ctxt->state.input[0].current.buttons;
	
	if (padCurr.du)
		pressed.du = 1;
	if (padCurr.dr)
		pressed.dr = 1;
	if (padCurr.dd)
		pressed.dd = 1;
	if (padCurr.dl)
		pressed.dl = 1;
	
	if (ctxt->state.input[0].releaseEdge.buttons.l && !pressed.du && !pressed.dr && !pressed.dd && !pressed.dl) {
		Inventory_Editor(ctxt);
		Hide_Hud(ctxt);
		Set_B_Button(ctxt);
	}
	
	if (!padCurr.l)
		pressed.du = pressed.dr = pressed.dd = pressed.dl = 0;
	if (ctxt->state.input[0].pressEdge.buttons.l)
		block = 1;
	if (!padCurr.l)
		block = 0;
	if (block)
		ctxt->state.input[0].current.buttons.du = ctxt->state.input[0].current.buttons.dr = ctxt->state.input[0].current.buttons.dd = ctxt->state.input[0].current.buttons.dl = 0;
	ctxt->state.input[0].pressEdge.buttons.l = 0;	
}

void Toggle_Minimap(GlobalContext* ctxt) {
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

void Handle_Hud(GlobalContext* ctxt) {
	if (!hud_hide)
		return;
	
	if (ctxt->interfaceCtx.alphas.minimap != 0) {
		if (hud_counter < 8) {
			hud_counter++;
			return;
		}
	}
	else hud_counter = 0;
	
	if (ctxt->interfaceCtx.alphas.buttonA			> 40)	ctxt->interfaceCtx.alphas.buttonA		-= 40; else ctxt->interfaceCtx.alphas.buttonA		= 0;
	if (ctxt->interfaceCtx.alphas.buttonB			> 40)	ctxt->interfaceCtx.alphas.buttonB		-= 40; else ctxt->interfaceCtx.alphas.buttonB		= 0;
	if (ctxt->interfaceCtx.alphas.buttonCLeft		> 40)	ctxt->interfaceCtx.alphas.buttonCLeft	-= 40; else ctxt->interfaceCtx.alphas.buttonCLeft	= 0;
	if (ctxt->interfaceCtx.alphas.buttonCDown		> 40)	ctxt->interfaceCtx.alphas.buttonCDown	-= 40; else ctxt->interfaceCtx.alphas.buttonCDown	= 0;
	if (ctxt->interfaceCtx.alphas.buttonCRight		> 40)	ctxt->interfaceCtx.alphas.buttonCRight	-= 40; else ctxt->interfaceCtx.alphas.buttonCRight	= 0;
	if (ctxt->interfaceCtx.alphas.minimap			> 40)	ctxt->interfaceCtx.alphas.minimap		-= 40; else ctxt->interfaceCtx.alphas.minimap		= 0;
		
	if (hud_hearts_hide) {
		if (ctxt->interfaceCtx.alphas.life			> 40)	ctxt->interfaceCtx.alphas.life			-= 40; else ctxt->interfaceCtx.alphas.life			= 0;
		if (ctxt->interfaceCtx.alphas.magicRupees	> 40)	ctxt->interfaceCtx.alphas.magicRupees	-= 40; else ctxt->interfaceCtx.alphas.magicRupees	= 0;
	}
	else {
		if (ctxt->interfaceCtx.alphas.life			< 215)	ctxt->interfaceCtx.alphas.life			+= 40; else ctxt->interfaceCtx.alphas.life			= 255;
		if (ctxt->interfaceCtx.alphas.magicRupees	< 215)	ctxt->interfaceCtx.alphas.magicRupees	+= 40; else ctxt->interfaceCtx.alphas.magicRupees	= 255;
	}
}

void Hide_Hud(GlobalContext* ctxt) {
	if (!CFG_HIDE_HUD_ENABLED || ctxt->pauseCtx.debugMenu != 0)
		return;
	
	if (ctxt->pauseCtx.state == 6 && ctxt->pauseCtx.screenIndex == 1) {
		hud_hide ^= 1;
		if (hud_hide)
			z2_PlaySfx(0x4813);
		else z2_PlaySfx(0x4814);
	}
	else if (ctxt->pauseCtx.state == 0 && hud_hide)
		hud_hearts_hide ^= 1;
}

void Handle_FPS(GlobalContext* ctxt) {
	if (!CFG_FPS_ENABLED || ctxt->pauseCtx.state != 0 || gSaveContext.extra.titleSetupIndex != 0 || ctxt->state.framerateDivisor == 1)
		return;
	
	if (ctxt->state.input[0].current.buttons.l && ctxt->state.input[0].pressEdge.buttons.z) {
		fps_switch ^= 1;
		if (fps_switch)
			z2_PlaySfx(0x4814);
		else z2_PlaySfx(0x4813);
	}
	
	if (!fps_switch || ctxt->opening_door == 0x00800020)
		ctxt->state.framerateDivisor = link_animation_speed = 3;
	else if (ctxt->text_state == 1 || ctxt->jump_state == 1 || ctxt->opening_chest == 1 || var_801D7B44 == 1 || playing_ocarina)
		ctxt->state.framerateDivisor = link_animation_speed = 2;
	else if (ctxt->playable_state == 0xFF08)
		ctxt->state.framerateDivisor = link_animation_speed = 3;
	else if (fps_switch)
		ctxt->state.framerateDivisor = link_animation_speed = 2;
	else if (ctxt->playable_state == 0x3208 && ctxt->use_hookshot == 0x100B)
		ctxt->state.framerateDivisor = link_animation_speed = 3;
	
	if (ctxt->state.framerateDivisor == 2) {
		if (gStaticContext.timeSpeed == 3)
			gStaticContext.timeSpeed = 2;
		if (gSaveContext.perm.timeSpeed == -2)
			gSaveContext.perm.timeSpeed = -1;
		inverted_time_state = -1;
	
		var_801160AE = 0x0006;
		var_80116702 = 0x0000;
		
		if (ctxt->jump_gravity == 0xC0B0)
			ctxt->jump_gravity = 0xC050;
		else if (ctxt->var_803FFE64 == 0x5008 && ctxt->link_action == 0x4120)
			ctxt->jump_gravity = 0xBF00;
		else if (ctxt->jump_gravity == 0xBFB3 && ctxt->link_action == 0x4140)
			ctxt->jump_gravity = 0xBFD0;
		else if (ctxt->jump_height > 0x4120 || ctxt->link_action == 0x40E0 || ctxt->link_action == 0x4110 || ctxt->link_action == 0x4120 || ctxt->link_action == 0x4150 || ctxt->link_action == 0x4160 || ctxt->link_action == 0x4170)
			ctxt->jump_gravity = 0xBF34;
		
		if (ctxt->var_803FFE64 == 0x3208) {
			if (ctxt->link_action == 0x40A0 || ctxt->link_action == 0x4110)
				ctxt->var_8040000C = 0x3F4CCCCD;
		}
		else if (ctxt->var_803FFE64 == 0x5008) {
			if (ctxt->link_action == 0x40E0 || ctxt->link_action == 0x4110 || ctxt->link_action == 0x4120)
				ctxt->var_8040000C = 0x3F333333;
		}
		else if (ctxt->var_803FFE64 == 0xC808) {
			if (ctxt->link_action == 0x4140 || ctxt->link_action == 0x4198)
				ctxt->var_8040000C = 0x3F800000;
		}
		
		if (ctxt->var_8040000C == 0x3FA00000)
			ctxt->var_8040000C =  0x3F900000;
		
		if (ctxt->deku_stick_timer == 100 && !deku_stick_timer_switch) {
			ctxt->deku_stick_timer += 100;
			deku_stick_timer_switch = 1;
		}
		else if (ctxt->deku_stick_timer == 0)
			deku_stick_timer_switch = 0;
	}
	else if (ctxt->state.framerateDivisor == 3) {
		if (gStaticContext.timeSpeed == 2)
			gStaticContext.timeSpeed = 3;
		if (gSaveContext.perm.timeSpeed == -1)
			gSaveContext.perm.timeSpeed = -2;
		inverted_time_state = -2;
	}
}

void Handle_Ocarina_Icons(GlobalContext* ctxt) {
	if (!CFG_OCARINA_ICONS_ENABLED)
		return;
	
	if (gSaveContext.extra.buttonsState.alphaTransition == 0xB || gSaveContext.perm.currentForm == PLAYER_FORM_HUMAN || gSaveContext.perm.currentForm == PLAYER_FORM_FIERCE_DEITY) {
		if (gSaveContext.perm.inv.items[0] == ITEM_DEKU_PIPES || gSaveContext.perm.inv.items[0] == ITEM_GORON_DRUMS || gSaveContext.perm.inv.items[0] == ITEM_ZORA_GUITAR)
			for (uint8_t button=1; button<=3; button++) {
				if (gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_DEKU_PIPES || gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_GORON_DRUMS ||gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_ZORA_GUITAR) {
				gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] = ITEM_OCARINA;
				gSaveContext.perm.unk4C.formButtonSlots[0].buttons[button] = SLOT_OCARINA;
				z2_UpdateButtonIcon(ctxt, button);
				break;
			}
			gSaveContext.perm.inv.items[0] = ITEM_OCARINA;
		}
		if (DPAD_CONFIG.state != DPAD_STATE_DISABLED) {
			if (DPAD_SET1_UP    == ITEM_DEKU_PIPES || DPAD_SET1_UP    == ITEM_GORON_DRUMS || DPAD_SET1_UP    == ITEM_ZORA_GUITAR)
				DPAD_SET1_UP = ITEM_OCARINA;
			if (DPAD_SET1_RIGHT == ITEM_DEKU_PIPES || DPAD_SET1_RIGHT == ITEM_GORON_DRUMS || DPAD_SET1_RIGHT == ITEM_ZORA_GUITAR)
				DPAD_SET1_RIGHT = ITEM_OCARINA;
			if (DPAD_SET1_DOWN  == ITEM_DEKU_PIPES || DPAD_SET1_DOWN  == ITEM_GORON_DRUMS || DPAD_SET1_DOWN  == ITEM_ZORA_GUITAR)
				DPAD_SET1_DOWN = ITEM_OCARINA;
			if (DPAD_SET1_LEFT  == ITEM_DEKU_PIPES || DPAD_SET1_LEFT  == ITEM_GORON_DRUMS || DPAD_SET1_LEFT  == ITEM_ZORA_GUITAR)
				DPAD_SET1_LEFT = ITEM_OCARINA;
			
			if (DPAD_SET2_UP    == ITEM_DEKU_PIPES || DPAD_SET2_UP    == ITEM_GORON_DRUMS || DPAD_SET2_UP    == ITEM_ZORA_GUITAR)
				DPAD_SET2_UP = ITEM_OCARINA;
			if (DPAD_SET2_RIGHT == ITEM_DEKU_PIPES || DPAD_SET2_RIGHT == ITEM_GORON_DRUMS || DPAD_SET2_RIGHT == ITEM_ZORA_GUITAR)
				DPAD_SET2_RIGHT = ITEM_OCARINA;
			if (DPAD_SET2_DOWN  == ITEM_DEKU_PIPES || DPAD_SET2_DOWN  == ITEM_GORON_DRUMS || DPAD_SET2_DOWN  == ITEM_ZORA_GUITAR)
				DPAD_SET2_DOWN = ITEM_OCARINA;
			if (DPAD_SET2_LEFT  == ITEM_DEKU_PIPES || DPAD_SET2_LEFT  == ITEM_GORON_DRUMS || DPAD_SET2_LEFT  == ITEM_ZORA_GUITAR)
				DPAD_SET2_LEFT = ITEM_OCARINA;
		}
	}
	
	else if (gSaveContext.perm.currentForm == PLAYER_FORM_DEKU) {
		if (gSaveContext.perm.inv.items[0] == ITEM_OCARINA || gSaveContext.perm.inv.items[0] == ITEM_GORON_DRUMS || gSaveContext.perm.inv.items[0] == ITEM_ZORA_GUITAR)
			for (uint8_t button=1; button<=3; button++) {
				if (gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_OCARINA || gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_GORON_DRUMS ||gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_ZORA_GUITAR) {
				gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] = ITEM_DEKU_PIPES;
				gSaveContext.perm.unk4C.formButtonSlots[0].buttons[button] = SLOT_OCARINA;
				z2_UpdateButtonIcon(ctxt, button);
				break;
			}
			gSaveContext.perm.inv.items[0] = ITEM_DEKU_PIPES;
		}
		if (DPAD_CONFIG.state != DPAD_STATE_DISABLED) {
			if (DPAD_SET1_UP    == ITEM_OCARINA || DPAD_SET1_UP    == ITEM_GORON_DRUMS || DPAD_SET1_UP    == ITEM_ZORA_GUITAR)
				DPAD_SET1_UP = ITEM_DEKU_PIPES;
			if (DPAD_SET1_RIGHT == ITEM_OCARINA || DPAD_SET1_RIGHT == ITEM_GORON_DRUMS || DPAD_SET1_RIGHT == ITEM_ZORA_GUITAR)
				DPAD_SET1_RIGHT = ITEM_DEKU_PIPES;
			if (DPAD_SET1_DOWN  == ITEM_OCARINA || DPAD_SET1_DOWN  == ITEM_GORON_DRUMS || DPAD_SET1_DOWN  == ITEM_ZORA_GUITAR)
				DPAD_SET1_DOWN = ITEM_DEKU_PIPES;
			if (DPAD_SET1_LEFT  == ITEM_OCARINA || DPAD_SET1_LEFT  == ITEM_GORON_DRUMS || DPAD_SET1_LEFT  == ITEM_ZORA_GUITAR)
				DPAD_SET1_LEFT = ITEM_DEKU_PIPES;
			
			if (DPAD_SET2_UP    == ITEM_OCARINA || DPAD_SET2_UP    == ITEM_GORON_DRUMS || DPAD_SET2_UP    == ITEM_ZORA_GUITAR)
				DPAD_SET2_UP = ITEM_OCARINA;
			if (DPAD_SET2_RIGHT == ITEM_OCARINA || DPAD_SET2_RIGHT == ITEM_GORON_DRUMS || DPAD_SET2_RIGHT == ITEM_ZORA_GUITAR)
				DPAD_SET2_RIGHT = ITEM_OCARINA;
			if (DPAD_SET2_DOWN  == ITEM_OCARINA || DPAD_SET2_DOWN  == ITEM_GORON_DRUMS || DPAD_SET2_DOWN  == ITEM_ZORA_GUITAR)
				DPAD_SET2_DOWN = ITEM_OCARINA;
			if (DPAD_SET2_LEFT  == ITEM_OCARINA || DPAD_SET2_LEFT  == ITEM_GORON_DRUMS || DPAD_SET2_LEFT  == ITEM_ZORA_GUITAR)
				DPAD_SET2_LEFT = ITEM_OCARINA;
		}
	}
	
	else if (gSaveContext.perm.currentForm == PLAYER_FORM_GORON) {
		if (gSaveContext.perm.inv.items[0] == ITEM_DEKU_PIPES || gSaveContext.perm.inv.items[0] == ITEM_OCARINA || gSaveContext.perm.inv.items[0] == ITEM_ZORA_GUITAR)
			for (uint8_t button=1; button<=3; button++) {
				if (gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_DEKU_PIPES || gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_OCARINA ||gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_ZORA_GUITAR) {
				gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] = ITEM_GORON_DRUMS;
				gSaveContext.perm.unk4C.formButtonSlots[0].buttons[button] = SLOT_OCARINA;
				z2_UpdateButtonIcon(ctxt, button);
				break;
			}
			gSaveContext.perm.inv.items[0] = ITEM_GORON_DRUMS;
		}
		if (DPAD_CONFIG.state != DPAD_STATE_DISABLED) {
			if (DPAD_SET1_UP    == ITEM_OCARINA || DPAD_SET1_UP    == ITEM_DEKU_PIPES || DPAD_SET1_UP    == ITEM_ZORA_GUITAR)
				DPAD_SET1_UP = ITEM_GORON_DRUMS;
			if (DPAD_SET1_RIGHT == ITEM_OCARINA || DPAD_SET1_RIGHT == ITEM_DEKU_PIPES || DPAD_SET1_RIGHT == ITEM_ZORA_GUITAR)
				DPAD_SET1_RIGHT = ITEM_GORON_DRUMS;
			if (DPAD_SET1_DOWN  == ITEM_OCARINA || DPAD_SET1_DOWN  == ITEM_DEKU_PIPES || DPAD_SET1_DOWN  == ITEM_ZORA_GUITAR)
				DPAD_SET1_DOWN = ITEM_GORON_DRUMS;
			if (DPAD_SET1_LEFT  == ITEM_OCARINA || DPAD_SET1_LEFT  == ITEM_DEKU_PIPES || DPAD_SET1_LEFT  == ITEM_ZORA_GUITAR)
				DPAD_SET1_LEFT = ITEM_GORON_DRUMS;
			
			if (DPAD_SET2_UP    == ITEM_OCARINA || DPAD_SET2_UP    == ITEM_DEKU_PIPES || DPAD_SET2_UP    == ITEM_ZORA_GUITAR)
				DPAD_SET2_UP = ITEM_GORON_DRUMS;
			if (DPAD_SET2_RIGHT == ITEM_OCARINA || DPAD_SET2_RIGHT == ITEM_DEKU_PIPES || DPAD_SET2_RIGHT == ITEM_ZORA_GUITAR)
				DPAD_SET2_RIGHT = ITEM_GORON_DRUMS;
			if (DPAD_SET2_DOWN  == ITEM_OCARINA || DPAD_SET2_DOWN  == ITEM_DEKU_PIPES || DPAD_SET2_DOWN  == ITEM_ZORA_GUITAR)
				DPAD_SET2_DOWN = ITEM_GORON_DRUMS;
			if (DPAD_SET2_LEFT  == ITEM_OCARINA || DPAD_SET2_LEFT  == ITEM_DEKU_PIPES || DPAD_SET2_LEFT  == ITEM_ZORA_GUITAR)
				DPAD_SET2_LEFT = ITEM_GORON_DRUMS;
		}
	}
	
	else if (gSaveContext.perm.currentForm == PLAYER_FORM_ZORA) {
		if (gSaveContext.perm.inv.items[0] == ITEM_DEKU_PIPES || gSaveContext.perm.inv.items[0] == ITEM_GORON_DRUMS || gSaveContext.perm.inv.items[0] == ITEM_OCARINA)
			for (uint8_t button=1; button<=3; button++) {
				if (gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_DEKU_PIPES || gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_GORON_DRUMS ||gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_OCARINA) {
				gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] = ITEM_ZORA_GUITAR;
				gSaveContext.perm.unk4C.formButtonSlots[0].buttons[button] = SLOT_OCARINA;
				z2_UpdateButtonIcon(ctxt, button);
				break;
			}
			gSaveContext.perm.inv.items[0] = ITEM_ZORA_GUITAR;
		}
		if (DPAD_CONFIG.state != DPAD_STATE_DISABLED) {
			if (DPAD_SET1_UP    == ITEM_OCARINA || DPAD_SET1_UP    == ITEM_DEKU_PIPES || DPAD_SET1_UP    == ITEM_GORON_DRUMS)
				DPAD_SET1_UP = ITEM_ZORA_GUITAR;
			if (DPAD_SET1_RIGHT == ITEM_OCARINA || DPAD_SET1_RIGHT == ITEM_DEKU_PIPES || DPAD_SET1_RIGHT == ITEM_GORON_DRUMS)
				DPAD_SET1_RIGHT = ITEM_ZORA_GUITAR;
			if (DPAD_SET1_DOWN  == ITEM_OCARINA || DPAD_SET1_DOWN  == ITEM_DEKU_PIPES || DPAD_SET1_DOWN  == ITEM_GORON_DRUMS)
				DPAD_SET1_DOWN = ITEM_ZORA_GUITAR;
			if (DPAD_SET1_LEFT  == ITEM_OCARINA || DPAD_SET1_LEFT  == ITEM_DEKU_PIPES || DPAD_SET1_LEFT  == ITEM_GORON_DRUMS)
				DPAD_SET1_LEFT = ITEM_ZORA_GUITAR;
			
			if (DPAD_SET2_UP    == ITEM_OCARINA || DPAD_SET2_UP    == ITEM_DEKU_PIPES || DPAD_SET2_UP    == ITEM_GORON_DRUMS)
				DPAD_SET2_UP = ITEM_ZORA_GUITAR;
			if (DPAD_SET2_RIGHT == ITEM_OCARINA || DPAD_SET2_RIGHT == ITEM_DEKU_PIPES || DPAD_SET2_RIGHT == ITEM_GORON_DRUMS)
				DPAD_SET2_RIGHT = ITEM_ZORA_GUITAR;
			if (DPAD_SET2_DOWN  == ITEM_OCARINA || DPAD_SET2_DOWN  == ITEM_DEKU_PIPES || DPAD_SET2_DOWN  == ITEM_GORON_DRUMS)
				DPAD_SET2_DOWN = ITEM_ZORA_GUITAR;
			if (DPAD_SET2_LEFT  == ITEM_OCARINA || DPAD_SET2_LEFT  == ITEM_DEKU_PIPES || DPAD_SET2_LEFT  == ITEM_GORON_DRUMS)
				DPAD_SET2_LEFT = ITEM_ZORA_GUITAR;
		}
	}
}

void Handle_Infinite() {
	if (CFG_INFINITE_HEALTH) {
		if (gSaveContext.perm.unk24.currentLife < gSaveContext.perm.unk24.maxLife)
			gSaveContext.perm.unk24.currentLife = gSaveContext.perm.unk24.maxLife;
	}
	
	if (CFG_INFINITE_MAGIC) {
		if (gSaveContext.perm.unk24.magicLevel == 1)
			gSaveContext.perm.unk24.currentMagic = 0x30;
		else if (gSaveContext.perm.unk24.magicLevel == 2)
			gSaveContext.perm.unk24.currentMagic = 0x60;
	}
	
	if (CFG_INFINITE_AMMO) {
		if (gSaveContext.perm.inv.upgrades.dekuNut == 1)
			gSaveContext.perm.inv.quantities[0x09] = gItemUpgradeCapacity.nutCapacity[1];
		else if (gSaveContext.perm.inv.upgrades.dekuNut == 2)
			gSaveContext.perm.inv.quantities[0x09] = gItemUpgradeCapacity.nutCapacity[2];
		else if (gSaveContext.perm.inv.upgrades.dekuNut == 3)
			gSaveContext.perm.inv.quantities[0x09] = gItemUpgradeCapacity.nutCapacity[3];
		
		if (gSaveContext.perm.inv.upgrades.dekuStick == 1)
			gSaveContext.perm.inv.quantities[0x08] = gItemUpgradeCapacity.stickCapacity[1];
		else if (gSaveContext.perm.inv.upgrades.dekuStick == 2)
			gSaveContext.perm.inv.quantities[0x08] = gItemUpgradeCapacity.stickCapacity[2];
		else if (gSaveContext.perm.inv.upgrades.dekuStick == 3)
			gSaveContext.perm.inv.quantities[0x08] = gItemUpgradeCapacity.stickCapacity[3];

		if (gSaveContext.perm.inv.upgrades.quiver == 1)
			gSaveContext.perm.inv.quantities[0x01] = gItemUpgradeCapacity.arrowCapacity[1];
		else if (gSaveContext.perm.inv.upgrades.quiver == 2)
			gSaveContext.perm.inv.quantities[0x01] = gItemUpgradeCapacity.arrowCapacity[2];
		else if (gSaveContext.perm.inv.upgrades.quiver == 3)
			gSaveContext.perm.inv.quantities[0x01] = gItemUpgradeCapacity.arrowCapacity[3];
		
		if (gSaveContext.perm.inv.upgrades.bombBag == 1)
			gSaveContext.perm.inv.quantities[0x06] = gSaveContext.perm.inv.quantities[0x07] = gItemUpgradeCapacity.bombCapacity[1];
		else if (gSaveContext.perm.inv.upgrades.bombBag == 2)
			gSaveContext.perm.inv.quantities[0x06] = gSaveContext.perm.inv.quantities[0x07] = gItemUpgradeCapacity.bombCapacity[2];
		else if (gSaveContext.perm.inv.upgrades.bombBag == 3)
			gSaveContext.perm.inv.quantities[0x06] = gSaveContext.perm.inv.quantities[0x07] = gItemUpgradeCapacity.bombCapacity[3];
	}
	
	if (CFG_INFINITE_RUPEES) {
		if (gSaveContext.perm.inv.upgrades.wallet == 0)
			gSaveContext.perm.unk24.rupees = gItemUpgradeCapacity.walletCapacity[0];
		else if (gSaveContext.perm.inv.upgrades.wallet == 1)
			gSaveContext.perm.unk24.rupees = gItemUpgradeCapacity.walletCapacity[1];
		else if (gSaveContext.perm.inv.upgrades.wallet == 2)
			gSaveContext.perm.unk24.rupees = gItemUpgradeCapacity.walletCapacity[2];
		else if (gSaveContext.perm.inv.upgrades.wallet == 3)
			gSaveContext.perm.unk24.rupees = gItemUpgradeCapacity.walletCapacity[3];
	}
}

void Inventory_Editor(GlobalContext* ctxt) {
	if (!CFG_INVENTORY_EDITOR_ENABLED || ctxt->pauseCtx.state != 6)
		return;
	
	if (ctxt->pauseCtx.debugMenu == 0 && ctxt->pauseCtx.screenIndex == 2) {
		ctxt->pauseCtx.debugMenu = 2;
		z2_PlaySfx(0x4813);
	}
	else if (ctxt->pauseCtx.debugMenu == 2) {
		ctxt->pauseCtx.debugMenu = 0;
		z2_PlaySfx(0x4814);
	}
}

void Handle_Quick_Pad(GlobalContext* ctxt) {
	if (CFG_INSTANT_ELEGY_ENABLED && !elegy_anim_state && ctxt->link_anim_2 == 0x67)
		ctxt->link_anim_1 = 5;
	
	if (ctxt->playable_state == 0xFF08 || ctxt->pauseCtx.state != 0)
		return;
	
	if (gSaveContext.perm.inv.items[0] != ITEM_OCARINA && gSaveContext.perm.inv.items[0] != ITEM_DEKU_PIPES && gSaveContext.perm.inv.items[0] != ITEM_GORON_DRUMS && gSaveContext.perm.inv.items[0] != ITEM_ZORA_GUITAR)
		return;
	
	InputPad padPress = ctxt->state.input[0].pressEdge.buttons;
	InputPad paddCurr = ctxt->state.input[0].current.buttons;
	
	if (CFG_FLOW_OF_TIME_ENABLED && gSaveContext.perm.inv.questStatus.songOfTime && gSaveContext.perm.timeSpeed != TIME_SPEED_STOPPED) {
		if (paddCurr.l && padPress.du) { // Inverse Time
			if (gSaveContext.perm.timeSpeed == TIME_SPEED_NORMAL) {
				gSaveContext.perm.timeSpeed = TIME_SPEED_INVERTED;
				z2_PlaySfx(0x4813);
			}
			else {
				gSaveContext.perm.timeSpeed = TIME_SPEED_NORMAL;
				z2_PlaySfx(0x4814);
			}
		}
		else if (paddCurr.l && paddCurr.dl) { // Speed up time
			if (ctxt->state.framerateDivisor == 3)
				gSaveContext.perm.time += 6;
			else gSaveContext.perm.time += 9;
		}
		else if (paddCurr.l && paddCurr.dr) { // Speed up time by a lot
			if (ctxt->state.framerateDivisor == 3)
				gSaveContext.perm.time += 12;
			else gSaveContext.perm.time += 18;
		}
	}
	
	if (CFG_INSTANT_ELEGY_ENABLED) {
		if (!gSaveContext.perm.inv.questStatus.elegyOfEmptiness)
			return;
		if (ctxt->sceneNum != SCENE_ROAD_TO_IKANA_CANYON && ctxt->sceneNum != SCENE_IKANA_CANYON && ctxt->sceneNum != SCENE_BOTTOM_OF_THE_WELL && ctxt->sceneNum != SCENE_ANCIENT_CASTLE_OF_IKANA && ctxt->sceneNum != SCENE_ANCIENT_CASTLE_OF_IKANA_BOSS \
		&& ctxt->sceneNum != SCENE_STONE_TOWER && ctxt->sceneNum != SCENE_STONE_TOWER_INV && ctxt->sceneNum != SCENE_GHOST_HUT && ctxt->sceneNum != SCENE_SECRET_SHRINE && ctxt->sceneNum != SCENE_STONE_TOWER_TEMPLE && ctxt->sceneNum != SCENE_STONE_TOWER_TEMPLE_INV  \
		&& ctxt->sceneNum != SCENE_STONE_TOWER_TEMPLE_BOSS)
			return;
		
		if (paddCurr.l && padPress.dd && ctxt->link_anim_1 != 0x67) // Instant Elegy
			ctxt->link_anim_1 = 0x67;
	}
}

/*void Handle_Saving(GlobalContext* ctxt) {
	if (ctxt->pauseCtx.state != 6 || gSaveContext.extra.fileIndex == 0xFF || gSaveContext.extra.titleSetupIndex != 0)
		return;
	if (!ctxt->state.input[0].pressEdge.buttons.b)
		return;
	ctxt->state.input[0].pressEdge.buttons.b = 0;
	
	ctxt->pauseCtx.state = 7;
}*/

void Handle_Unequipping(GlobalContext* ctxt) {
	if (ctxt->pauseCtx.state != 6 || gSaveContext.extra.fileIndex == 0xFF || gSaveContext.extra.titleSetupIndex != 0)
		return;
	if (!CFG_UNEQUIP_ENABLED || (ctxt->pauseCtx.screenIndex != 0 && ctxt->pauseCtx.screenIndex != 3) )
		return;
	
	uint8_t button;
	if (ctxt->state.input[0].pressEdge.buttons.cl)
		button = 1;
	else if (ctxt->state.input[0].pressEdge.buttons.cd)
		button = 2;
	else if (ctxt->state.input[0].pressEdge.buttons.cr)
		button = 3;
	else return;
	
	uint8_t item = ctxt->pauseCtx.selectedItem;
	if (item == 0x2)
		item = 0x4A;
	else if (item == 0x3)
		item = 0x4B;
	else if (item == 0x4)
		item = 0x4C;
	
	if (gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == item) {
		ctxt->state.input[0].pressEdge.buttons.cl = ctxt->state.input[0].pressEdge.buttons.cd = ctxt->state.input[0].pressEdge.buttons.cr = 0;
		gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] = gSaveContext.perm.unk4C.formButtonSlots[0].buttons[button] = 0xFF;
		z2_PlaySfx(0x480A);
	}
}

void Set_B_Button(GlobalContext* ctxt) {
	if (!CFG_B_BUTTON_ITEM_ENABLED || ctxt->pauseCtx.state != 6 || ctxt->pauseCtx.screenIndex != 0 || ctxt->pauseCtx.debugMenu != 0)
		return;
	
	uint8_t item = ctxt->pauseCtx.selectedItem;
	
	if (item == ITEM_FIRE_ARROW)
		item = ITEM_BOW_FIRE_ARROW;
	else if (item == ITEM_ICE_ARROW)
		item = ITEM_BOW_ICE_ARROW;
	else if (item == ITEM_LIGHT_ARROW)
		item = ITEM_BOW_LIGHT_ARROW;
	else if (item >= ITEM_EMPTY_BOTTLE)
		return;
	else if (item >= ITEM_BOW && item <= ITEM_LIGHT_ARROW)
		return;
	else if (item == ITEM_BOMB || item == ITEM_BOMBCHU || item == ITEM_LENS || item == ITEM_POWDER_KEG)
		return;
	
	gSaveContext.perm.unk4C.formButtonItems[0].buttons[0] = gSaveContext.perm.unk4C.formButtonSlots[0].buttons[0] = item;
	z2_UpdateButtonIcon(ctxt, 0);
	z2_PlaySfx(0x4808);
}