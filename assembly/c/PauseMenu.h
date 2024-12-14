#ifndef PAUSESCREEN_H
#define PAUSESCREEN_H

#include <z64.h>
#include <z64extended.h>
#include "Misc.h"
#include "Reloc.h"
#include "Input.h"
#include "Dpad.h"
#include "Sprite.h"
#include "Text.h"
#include "macro.h"
#include "controller.h"

#define TIME_ONE_HOUR            (0x2000                 / 3)
#define NEW_TIME_HOURS           (new_time               / TIME_ONE_HOUR)
#define REAL_TIME_HOURS          (gSaveContext.perm.time / TIME_ONE_HOUR)
#define TIME_DIFFERENCE_IN_HOURS (NEW_TIME_HOURS         - REAL_TIME_HOURS)

static Vtx* GetVtxBuffer(GlobalContext* ctxt, u32 vertIdx, int slot);
static void DrawIcon(GraphicsContext* gfx, const Vtx* vtx, u32 segAddr, u16 width, u16 height, u16 qidx);
static void CycleQuestItem(GlobalContext* ctxt, u8 item, u8 slot);
static bool IsQuestItemWithStorageSelected(GlobalContext* ctxt);
void PauseMenu_SelectItemDrawIcon(GraphicsContext* gfx, u8 item, u16 width, u16 height, int slot, u16 quadIdx, u32 vertIdx);
void PauseMenu_SelectItemSubscreenAfterProcess(GlobalContext* ctxt);
bool PauseMenu_SelectItemProcessAButton(GlobalContext* ctxt, u32 curVal, u32 noneVal);
bool PauseMenu_SelectItemShowAButtonEnabled(GlobalContext* ctxt);
void PauseMenu_BeforeUpdate(GlobalContext* ctxt);
void Handle_Sword_Swap(GlobalContext* ctxt);
void Handle_Equip_Sword(GlobalContext* ctxt);
void Handle_Shield_Swap(GlobalContext* ctxt);
void Handle_Mapping_Items(GlobalContext* ctxt);
uint8_t Handle_Mapping_Item(uint8_t button, uint8_t item);
void Handle_Clock_Controls(GlobalContext* ctxt);
void Draw_Clock_Controls(GlobalContext* ctxt, DispBuf* db);
void Draw_Hud_Toggle(GlobalContext* ctxt, DispBuf* db);
    
#endif