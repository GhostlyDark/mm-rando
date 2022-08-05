#include <z64.h>
#include <z64extended.h>
#include "Models.h"
#include "OverlayMenu.h"
#include "MMR.h"
#include "Music.h"
#include "Fps.h"

extern uint8_t CFG_OCARINA_ICONS_ENABLED;

bool Game_IsPlayerActor(void) {
    return s801D0B70.selected == &s801D0B70.playerActor;
}

bool Game_IsKaleidoScope(void) {
    return s801D0B70.selected == &s801D0B70.kaleidoScope;
}

/**
 * Hook function called after preparing display buffers for writing during current frame.
 **/
void Game_AfterPrepareDisplayBuffers(GraphicsContext* gfx) {
    // Check if models objheap should finish advancing.
    Models_AfterPrepareDisplayBuffers(gfx);
}

/**
 * Hook function called after game processes next frame.
 **/
void Game_AfterUpdate(GlobalContext* ctxt) {
    /*OverlayMenu_Draw(ctxt);
    Music_Update(ctxt);
    if (Game_IsPlayerActor()) {
        MMR_ProcessItemQueue(ctxt);
    }*/
	
	Handle_FPS(ctxt);
	Handle_L_Button(ctxt);
	Handle_Ocarina(ctxt);
}

void Handle_Ocarina(GlobalContext* ctxt) {
	if (!CFG_OCARINA_ICONS_ENABLED)
		return;
	
	if (gSaveContext.perm.currentForm == PLAYER_FORM_HUMAN) {
		if (gSaveContext.perm.inv.items[0] == ITEM_DEKU_PIPES || gSaveContext.perm.inv.items[0] == ITEM_GORON_DRUMS || gSaveContext.perm.inv.items[0] == ITEM_ZORA_GUITAR)
			for (uint8_t button=1; button<=3; button++) {
				if (gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_DEKU_PIPES || gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_GORON_DRUMS ||gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_ZORA_GUITAR) {
				gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] = ITEM_OCARINA;
				gSaveContext.perm.unk4C.formButtonSlots[0].buttons[button] = ITEM_OCARINA;
				z2_UpdateButtonIcon(ctxt, button);
				break;
			}
			gSaveContext.perm.inv.items[0] = ITEM_OCARINA;
		}
	}
	
	else if (gSaveContext.perm.currentForm == PLAYER_FORM_DEKU) {
		if (gSaveContext.perm.inv.items[0] == ITEM_OCARINA || gSaveContext.perm.inv.items[0] == ITEM_GORON_DRUMS || gSaveContext.perm.inv.items[0] == ITEM_ZORA_GUITAR)
			for (uint8_t button=1; button<=3; button++) {
				if (gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_OCARINA || gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_GORON_DRUMS ||gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_ZORA_GUITAR) {
				gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] = ITEM_DEKU_PIPES;
				gSaveContext.perm.unk4C.formButtonSlots[0].buttons[button] = ITEM_OCARINA;
				z2_UpdateButtonIcon(ctxt, button);
				break;
			}
			gSaveContext.perm.inv.items[0] = ITEM_DEKU_PIPES;
		}
	}
	
	else if (gSaveContext.perm.currentForm == PLAYER_FORM_GORON) {
		if (gSaveContext.perm.inv.items[0] == ITEM_DEKU_PIPES || gSaveContext.perm.inv.items[0] == ITEM_OCARINA || gSaveContext.perm.inv.items[0] == ITEM_ZORA_GUITAR)
			for (uint8_t button=1; button<=3; button++) {
				if (gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_DEKU_PIPES || gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_OCARINA ||gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_ZORA_GUITAR) {
				gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] = ITEM_GORON_DRUMS;
				gSaveContext.perm.unk4C.formButtonSlots[0].buttons[button] = ITEM_OCARINA;
				z2_UpdateButtonIcon(ctxt, button);
				break;
			}
			gSaveContext.perm.inv.items[0] = ITEM_GORON_DRUMS;
		}
	}
	
	else if (gSaveContext.perm.currentForm == PLAYER_FORM_ZORA) {
		if (gSaveContext.perm.inv.items[0] == ITEM_DEKU_PIPES || gSaveContext.perm.inv.items[0] == ITEM_GORON_DRUMS || gSaveContext.perm.inv.items[0] == ITEM_OCARINA)
			for (uint8_t button=1; button<=3; button++) {
				if (gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_DEKU_PIPES || gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_GORON_DRUMS ||gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_OCARINA) {
				gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] = ITEM_ZORA_GUITAR;
				gSaveContext.perm.unk4C.formButtonSlots[0].buttons[button] = ITEM_OCARINA;
				z2_UpdateButtonIcon(ctxt, button);
				break;
			}
			gSaveContext.perm.inv.items[0] = ITEM_ZORA_GUITAR;
		}
	}
}
