#ifndef EXTRA_H
#define EXTRA_H

#include <z64.h>
#include <z64extended.h>
#include "Input.h"
#include "Dpad.h"
#include "PauseMenu.h"
#include "Reloc.h"
#include "GiantMask.h"

void Handle_Extra_Functions(GlobalContext* ctxt);
void Handle_Items_On_Dpad(GlobalContext* ctxt);
void Handle_Item_Slot_On_Dpad(uint8_t button_min, uint8_t button_max, uint8_t slot);
void Handle_Rupee_Drain(ActorPlayer* player, GlobalContext* ctxt);
void Handle_L_Button_Ingame(GlobalContext* ctxt);
void Handle_L_Button_Paused(GlobalContext* ctxt);
void Toggle_Minimap(GlobalContext* ctxt);
void Set_Minimap_Toggle();
void Handle_Hud(GlobalContext* ctxt);
void Hide_Hud(GlobalContext* ctxt);
void Handle_FPS(GlobalContext* ctxt);
void Handle_Ocarina_Icons(GlobalContext* ctxt);
void Handle_Infinite();
void Inventory_Editor(GlobalContext* ctxt);
void Handle_FPS_Toggle();
void Handle_Invert_Time();
uint8_t Can_Use_Elegy(GlobalContext* ctxt);
void Handle_Instant_Elegy(GlobalContext* ctxt);
void Handle_Use_Pictobox(GlobalContext* ctxt, ActorPlayer* player);
void Handle_Dual_Dpad(GlobalContext* ctxt);
void Handle_Sword_Toggle(GlobalContext* ctxt);
void Handle_Shield_Toggle(GlobalContext* ctxt);
void Handle_Unequipping(GlobalContext* ctxt);
void Set_B_Button(GlobalContext* ctxt);

//#define map_select_active         (*(uint32_t*)           0x8022A174) // 8024A484, 803E6C44
#define time_modifier               (*(uint32_t*)           0x801EF684)
#define playing_ocarina             (*(uint8_t*)            0x801D6FB4)
#define link_animation_speed        (*(uint8_t*)            0x803825E1)
#define var_801160AE                (*(uint16_t*)           0x801160AE)
#define var_80116702                (*(uint16_t*)           0x80116702)
#define var_801D7B44                (*(uint8_t*)            0x801D7B44)
#define elegy_anim_state            (*(uint8_t*)            0x801B1BEE)
#define inverted_time_state         (*(int8_t*)             0x80119B1F)

#endif
