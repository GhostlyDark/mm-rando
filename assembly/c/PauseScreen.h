#ifndef PAUSESCREEN_H
#define PAUSESCREEN_H

#include <z64.h>
#include <z64extended.h>
#include "HudColors.h"
#include "Misc.h"
#include "QuestItemStorage.h"
#include "QuestItems.h"
#include "Reloc.h"
#include "SaveFile.h"
#include "Input.h"

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
void Handle_Shield_Swap(GlobalContext* ctxt);
void Handle_Unequipping(GlobalContext* ctxt);
	
#endif