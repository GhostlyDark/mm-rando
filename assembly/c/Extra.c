#include "Extra.h"

extern uint8_t CFG_DUAL_DPAD_ENABLED;
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

uint16_t deku_stick_timer_switch	= 0;
uint16_t last_time					= 0;
uint16_t started_timer				= 0;
uint16_t elegy_timer                = 0;

uint8_t dpad_alt                    = 0;
uint8_t fps_switch					= 1;
uint8_t hud_hide                    = 0;
uint8_t hud_hearts_hide				= 1;
uint8_t hud_counter					= 0;

uint8_t block						= 0;
uint8_t pressed_r					= 0;
uint8_t pressed_z					= 0;
uint8_t pressed_du					= 0;
uint8_t pressed_dr					= 0;
uint8_t pressed_dd					= 0;
uint8_t pressed_dl					= 0;

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
	Handle_L_Button(ctxt);
	Handle_Infinite();
	
	if (ctxt->pauseCtx.state == 6) {
		ctxt->state.input[0].current.buttons.dl = 0;
		ctxt->state.input[0].current.buttons.dr = 0;
	}
}

void Handle_L_Button(GlobalContext* ctxt) {
	InputPad paddCurr = ctxt->state.input[0].current.buttons;
	
	if (ctxt->state.input[0].releaseEdge.buttons.l && !paddCurr.r && !paddCurr.z && !paddCurr.du && !paddCurr.dr && !paddCurr.dd && !paddCurr.dl) {
		Toggle_Minimap(ctxt);
		Hide_Hud(ctxt);
		Inventory_Editor(ctxt);
	}
	
	if (ctxt->state.input[0].pressEdge.buttons.l)
		block = 1;
	if (!paddCurr.l)
		block = 0;
	
	if (block) {
		ctxt->state.input[0].current.buttons.r  = 0;
		ctxt->state.input[0].current.buttons.z  = 0;
		ctxt->state.input[0].current.buttons.du = 0;
		ctxt->state.input[0].current.buttons.dr = 0;
		ctxt->state.input[0].current.buttons.dd = 0;
		ctxt->state.input[0].current.buttons.dl = 0;
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
	
	if (!fps_switch || opening_door == 0x00800020)
		ctxt->state.framerateDivisor = link_animation_speed = 3;
	else if (text_state == 0x01 || jump_state == 0x0100 || opening_chest == 0x01 || var_801D7B44 == 0x01)
		ctxt->state.framerateDivisor = link_animation_speed = 2;
	else if (playable_state == 0xFF08)
		ctxt->state.framerateDivisor = link_animation_speed = 3;
	else if (fps_switch)
		ctxt->state.framerateDivisor = link_animation_speed = 2;
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

void Handle_Ocarina_Icons(GlobalContext* ctxt) {
	if (!CFG_OCARINA_ICONS_ENABLED)
		return;
	
	if (gSaveContext.perm.currentForm == PLAYER_FORM_HUMAN || gSaveContext.perm.currentForm == PLAYER_FORM_FIERCE_DEITY) {
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
		if (DPAD_CONFIG.state != DPAD_STATE_DISABLED)
			for (uint8_t dpad=0; dpad<4; dpad++)
				if (DPAD_CONFIG.primary.values[dpad] == ITEM_DEKU_PIPES || DPAD_CONFIG.primary.values[dpad] == ITEM_GORON_DRUMS || DPAD_CONFIG.primary.values[dpad] == ITEM_ZORA_GUITAR)
					DPAD_CONFIG.primary.values[dpad] = ITEM_OCARINA;
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
		if (DPAD_CONFIG.state != DPAD_STATE_DISABLED)
			for (uint8_t dpad=0; dpad<4; dpad++)
				if (DPAD_CONFIG.primary.values[dpad] == ITEM_OCARINA || DPAD_CONFIG.primary.values[dpad] == ITEM_GORON_DRUMS || DPAD_CONFIG.primary.values[dpad] == ITEM_ZORA_GUITAR)
					DPAD_CONFIG.primary.values[dpad] = ITEM_DEKU_PIPES;
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
		if (DPAD_CONFIG.state != DPAD_STATE_DISABLED)
			for (uint8_t dpad=0; dpad<4; dpad++)
				if (DPAD_CONFIG.primary.values[dpad] == ITEM_DEKU_PIPES || DPAD_CONFIG.primary.values[dpad] == ITEM_OCARINA || DPAD_CONFIG.primary.values[dpad] == ITEM_ZORA_GUITAR)
					DPAD_CONFIG.primary.values[dpad] = ITEM_GORON_DRUMS;
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
		if (DPAD_CONFIG.state != DPAD_STATE_DISABLED)
			for (uint8_t dpad=0; dpad<4; dpad++)
				if (DPAD_CONFIG.primary.values[dpad] == ITEM_DEKU_PIPES || DPAD_CONFIG.primary.values[dpad] == ITEM_GORON_DRUMS || DPAD_CONFIG.primary.values[dpad] == ITEM_OCARINA)
					DPAD_CONFIG.primary.values[dpad] = ITEM_ZORA_GUITAR;
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
	if (CFG_INSTANT_ELEGY_ENABLED && !elegy_anim_state && link_anim_2 == 0x67)
		link_anim_1 = 5;
	
	if (playable_state == 0xFF08 || ctxt->pauseCtx.state != 0)
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
		
		if (paddCurr.l && padPress.dd && link_anim_1 != 0x67) { // Instant Elegy
			link_anim_1 = 0x67;
		}
	}
	
}