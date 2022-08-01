#include "FPS.h"
#include "Input.h"

uint8_t	 CFG_FPS_ENABLED			= 1;

uint8_t	 fps_switch					= 1;
uint16_t deku_stick_timer_switch	= 0;
uint16_t last_time					= 0;
uint16_t started_timer				= 0;

void Handle_FPS(GlobalContext* ctxt) {
	
	if (!CFG_FPS_ENABLED || ctxt->pauseCtx.state != 0 || gSaveContext.extra.titleSetupIndex != 0)
		return;
	
	if (gPlayUpdateInput.pressEdge.buttons.l) {
		fps_switch ^= 1;
		if (fps_switch)
			z2_PlaySfx(0x4814);
		else z2_PlaySfx(0x4813);
	}
	
	if (!fps_switch)
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