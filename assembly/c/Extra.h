#ifndef EXTRA_H
#define EXTRA_H

#include <z64.h>
#include <z64extended.h>
#include "Input.h"
#include "Dpad.h"

void Handle_Extra_Functions(GlobalContext* ctxt);
void Handle_L_Button_Ingame(GlobalContext* ctxt);
void Handle_L_Button_Paused(GlobalContext* ctxt);
void Toggle_Minimap(GlobalContext* ctxt);
void Handle_Hud(GlobalContext* ctxt);
void Hide_Hud(GlobalContext* ctxt);
void Handle_FPS(GlobalContext* ctxt);
void Handle_Ocarina_Icons(GlobalContext* ctxt);
void Handle_Infinite();
void Inventory_Editor(GlobalContext* ctxt);
void Handle_Quick_Pad(GlobalContext* ctxt);
void Set_B_Button(GlobalContext* ctxt);

#define map_select_active			(*(uint32_t*)			0x8022A174) // 8024A484, 803E6C44
#define time_modifier				(*(uint32_t*)			0x801EF684)
#define playing_ocarina				(*(uint8_t*)			0x801D6FB4)
#define link_animation_speed		(*(uint8_t*)			0x803825E1)
#define var_801160AE				(*(uint16_t*)			0x801160AE)
#define var_80116702				(*(uint16_t*)			0x80116702)
#define var_801D7B44				(*(uint8_t*)			0x801D7B44)

#define opening_door				(*(uint32_t*)			0x80400820)
#define deku_hovering				(*(uint16_t*)			0x8040081C)
#define playable_state				(*(uint16_t*)			0x803FFE66)
#define use_hookshot				(*(uint16_t*)			0x803FFE00)
#define jump_state					(*(uint16_t*)			0x80400897)
#define deku_stick_timer			(*(uint16_t*)			0x804008D8)
#define jump_height					(*(uint16_t*)			0x803FFE20)
#define jump_gravity				(*(uint16_t*)			0x803FFE24)
#define link_action					(*(uint16_t*)			0x80400004)
#define text_state					(*(uint8_t*)			0x803FFDB6)
#define opening_chest				(*(uint8_t*)			0x803FFF68)
#define var_8040000C				(*(uint32_t*)			0x8040000C)
#define var_803FFE64				(*(uint16_t*)			0x803FFE64)

#define opening_door_ws				(*(uint32_t*)			0x80418E20)
#define deku_hovering_ws			(*(uint16_t*)			0x80418E1C)
#define playable_state_ws			(*(uint16_t*)			0x80418466)
#define use_hookshot_ws				(*(uint16_t*)			0x80418400)
#define jump_state_ws				(*(uint16_t*)			0x80418E97)
#define deku_stick_timer_ws			(*(uint16_t*)			0x80418ED8)
#define jump_height_ws				(*(uint16_t*)			0x80418420)
#define jump_gravity_ws				(*(uint16_t*)			0x80418424)
#define link_action_ws				(*(uint16_t*)			0x80418604)
#define text_state_ws				(*(uint8_t*)			0x804183B6)
#define opening_chest_ws			(*(uint8_t*)			0x80418568)
#define var_8040000C_ws				(*(uint32_t*)			0x8041860C)
#define var_803FFE64_ws				(*(uint16_t*)			0x80418464)

#define link_anim_1					(*(uint8_t*)			0x80400144)
#define link_anim_2					(*(uint8_t*)			0x80400145)
#define link_anim_1_ws				(*(uint8_t*)			0x80418744)
#define link_anim_2_ws				(*(uint8_t*)			0x80418745)
#define elegy_anim_state			(*(uint8_t*)			0x801B1BEE)

#endif
