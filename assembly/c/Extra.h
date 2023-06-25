#ifndef EXTRA_H
#define EXTRA_H

#include <z64.h>
#include <z64extended.h>
#include "Input.h"
#include "Dpad.h"

void Handle_Extra_Functions(GlobalContext* ctxt);
void Handle_Rupee_Drain(ActorPlayer* player, GlobalContext* ctxt);
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
//void Handle_Saving(GlobalContext* ctxt);
void Handle_Unequipping(GlobalContext* ctxt);
void Set_B_Button(GlobalContext* ctxt);

#define map_select_active			(*(uint32_t*)			0x8022A174) // 8024A484, 803E6C44
#define time_modifier				(*(uint32_t*)			0x801EF684)
#define playing_ocarina				(*(uint8_t*)			0x801D6FB4)
#define link_animation_speed		(*(uint8_t*)			0x803825E1)
#define var_801160AE				(*(uint16_t*)			0x801160AE)
#define var_80116702				(*(uint16_t*)			0x80116702)
#define var_801D7B44				(*(uint8_t*)			0x801D7B44)
#define elegy_anim_state			(*(uint8_t*)			0x801B1BEE)

#endif
