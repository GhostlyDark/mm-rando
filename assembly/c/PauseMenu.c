#include <z64.h>
#include "HudColors.h"
#include "Misc.h"
#include "QuestItemStorage.h"
#include "QuestItems.h"
#include "Reloc.h"
#include "SaveFile.h"
#include "macro.h"
#include "controller.h"
#include "PauseMenu.h"
#include "Text.h"

extern uint8_t CFG_WS_ENABLED;
extern uint8_t CFG_CLOCK_CONTROL_ENABLED;
extern uint8_t CFG_HIDE_HUD_ENABLED;
extern uint8_t CFG_SWAP_ENABLED;
extern uint8_t CFG_B_BUTTON_ITEM_ENABLED;
extern uint8_t CFG_BOSS_REMAINS_CLOCK_CONTROLS;

extern uint8_t dpad_alt;
extern uint8_t block;
extern uint8_t hud_hide;

uint8_t  redraw_b_button             = 0;
uint8_t  clock_controls              = 0;
uint16_t new_time                    = 0;
uint8_t  add_time                    = 0;
uint8_t  changed_time                = 0;
uint8_t  clock_control_button_frames = 0;

// Vertex buffers.
static Vtx gVertexBufs[(4 * 3) * 2];

// Vertex buffer pointers.
static Vtx* gVertex[3] = {
    &gVertexBufs[(4 * 0) * 2],
    &gVertexBufs[(4 * 1) * 2],
    &gVertexBufs[(4 * 2) * 2],
};

static Vtx* GetVtxBuffer(GlobalContext* ctxt, u32 vertIdx, int slot) {
    // Get vertex of current icon drawing to Item Select screen
    const Vtx* srcVtx = ctxt->pauseCtx.vtxBuf + vertIdx;

    // Get dest Vtx (factor in frame counter)
    int framebufIdx = ctxt->state.gfxCtx->displayListCounter & 1;
    Vtx* dstVtx = gVertex[slot] + (framebufIdx * 4);

    // Copy source Vtx over to dest Vtx
    for (int i = 0; i < 4; i++) {
        dstVtx[i] = srcVtx[i];
    }

    // Adjust X position
    dstVtx[0].v.ob[0] += 0x10;
    dstVtx[2].v.ob[0] += 0x10;

    // Adjust Y position
    dstVtx[0].v.ob[1] -= 0x10;
    dstVtx[1].v.ob[1] -= 0x10;

    return dstVtx;
}

static void DrawIcon(GraphicsContext* gfx, const Vtx* vtx, u32 segAddr, u16 width, u16 height, u16 qidx) {
    DispBuf* db = &gfx->polyOpa;
    // Instructions that happen before function
    gDPSetPrimColor(db->p++, 0, 0, 0xFF, 0xFF, 0xFF, gfx->globalContext->pauseCtx.itemAlpha & 0xFF);
    gSPVertex(db->p++, vtx, 4, 0); // Loads 4 vertices from RDRAM
    // Instructions that happen during function.
    gDPSetTextureImage(db->p++, G_IM_FMT_RGBA, G_IM_SIZ_32b, 1, (void*)segAddr);
    gDPSetTile(db->p++, G_IM_FMT_RGBA, G_IM_SIZ_32b, 0, 0, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);
    gDPLoadSync(db->p++);
    gDPLoadBlock(db->p++, 7, 0, 0, 0x400 - 1, 0x80);
    gDPPipeSync(db->p++);
    gDPSetTile(db->p++, G_IM_FMT_RGBA, G_IM_SIZ_32b, 8, 0, G_TX_RENDERTILE, 0, 0, 0, 0, 0, 0, 0);
    gDPSetTileSize(db->p++, 0, 0, 0, (width - 1) * 4, (height - 1) * 4);
    gSP1Quadrangle(db->p++, qidx + 0, qidx + 2, qidx + 3, qidx + 1, 0);
}

static void CycleQuestItem(GlobalContext* ctxt, u8 item, u8 slot) {
    u8 orig = gSaveContext.perm.inv.items[slot];
    // Replace item in inventory.
    gSaveContext.perm.inv.items[slot] = item;
    // Replace item in C buttons.
    for (int i = 1; i < 4; i++) {
        if (orig != ITEM_NONE && gSaveContext.perm.unk4C.formButtonItems[0].buttons[i] == orig) {
            gSaveContext.perm.unk4C.formButtonItems[0].buttons[i] = item;
            z2_ReloadButtonTexture(ctxt, i);
        }
    }
    // Play sound effect.
    z2_PlaySfx(0x4808);
}

static bool IsQuestItemInCorrectSlot(u8 item, int slot) {
    int cell;
    return QuestItems_GetSlot(&cell, item) && cell == slot;
}

static bool IsQuestItemWithStorageSelected(GlobalContext* ctxt) {
    // Get cell and selected item.
    s16 cell = ctxt->pauseCtx.cells1.item;
    u8 item = gSaveContext.perm.inv.items[cell];

    // Check if on a quest item slot.
    bool quest = QuestItems_IsQuestSlot(cell);

    // Verify we are in the right cell for this item.
    bool correctSlot = IsQuestItemInCorrectSlot(item, cell);

    // Check if there's a next item.
    u8 next = QuestItemStorage_Next(&SAVE_FILE_CONFIG.questStorage, item);

    // Check if on "Z" or "R" side buttons.
    bool side = ctxt->pauseCtx.sideButton != 0;

    return (quest && correctSlot && !side && item != ITEM_NONE && next != ITEM_NONE);
}

/**
 * Hook function called during the draw loop for item icons on the "Select Item" subscreen.
 *
 * Used to draw the next quest item in storage for quest item slots.
 **/
void PauseMenu_SelectItemDrawIcon(GraphicsContext* gfx, u8 item, u16 width, u16 height, int slot, u16 quadIdx, u32 vertIdx) {
    // Call original function to draw underlying item texture
    u32 origSegAddr = gItemTextureSegAddrTable[item];
    z2_PauseDrawItemIcon(gfx, origSegAddr, width, height, quadIdx);
    // If quest item storage, draw next quest item texture on bottom-right of current texture
    if (MISC_CONFIG.flags.questItemStorage && IsQuestItemInCorrectSlot(item, slot)) {
        struct QuestItemStorage* storage = &SAVE_FILE_CONFIG.questStorage;
        if (QuestItemStorage_Has(storage, item)) {
            int sslot, unused;
            u8 next = QuestItemStorage_Next(storage, item);
            if (next != ITEM_NONE && QuestItemStorage_GetSlot(&sslot, &unused, next)) {
                u32 segAddr = gItemTextureSegAddrTable[next];
                Vtx* vtx = GetVtxBuffer(gfx->globalContext, vertIdx, sslot);
                DrawIcon(gfx, vtx, segAddr, width, height, quadIdx);
            }
        }
    }
}

/**
 * Hook function called after the main processing for the "Select Item" subscreen.
 *
 * Used to set the text on the A button to "Decide" for selecting quest items.
 **/
void PauseMenu_SelectItemSubscreenAfterProcess(GlobalContext* ctxt) {
    if (MISC_CONFIG.flags.questItemStorage) {
        u16 text = ctxt->interfaceCtx.buttonATextCurrent;
        if (IsQuestItemWithStorageSelected(ctxt)) {
            // Set A button text to "Decide" (only if on "Info")
            if (text == BUTTON_TEXT_INFO) {
                z2_HudSetAButtonText(ctxt, BUTTON_TEXT_DECIDE);
            }
        } else {
            // Set A button text to "Info" (only if on "Decide")
            if (text == BUTTON_TEXT_DECIDE) {
                z2_HudSetAButtonText(ctxt, BUTTON_TEXT_INFO);
            }
        }
    }
}

/**
 * Hook function called before processing A button input on the "Select Item" subscreen.
 *
 * Checks if A button would be used to cycle quest items.
 **/
bool PauseMenu_SelectItemProcessAButton(GlobalContext* ctxt, u32 curVal, u32 noneVal) {
    if (MISC_CONFIG.flags.questItemStorage && IsQuestItemWithStorageSelected(ctxt)) {
        s16 cell = ctxt->pauseCtx.cells1.item;
        if (curVal != noneVal) {
            u8 item = (u8)curVal;
            // Check input for A button, and swap to next quest item.
            InputPad pad = ctxt->state.input->pressEdge.buttons;
            u8 next = QuestItemStorage_Next(&SAVE_FILE_CONFIG.questStorage, item);
            if (pad.a && next != ITEM_NONE) {
                ctxt->state.input->pressEdge.buttons.a = 0;
                CycleQuestItem(ctxt, next, (u8)cell);
            }
        }
    }
    // Perform original check.
    return curVal == noneVal;
}

/**
 * Hook function called before determining if the A button should be shown as enabled or not.
 *
 * Checks if a quest item with storage is selected. If so, always show the A button as enabled.
 **/
bool PauseMenu_SelectItemShowAButtonEnabled(GlobalContext* ctxt) {
    if (MISC_CONFIG.flags.questItemStorage && IsQuestItemWithStorageSelected(ctxt)) {
        // If on a quest item with storage, show A button as enabled even during "Item Prompt."
        return true;
    } else {
        // Perform original check.
        return ctxt->msgCtx.unk11F10 == 0;
    }
}

/**
 * Hook function called while on pause menu before processing each frame.
 **/
void PauseMenu_BeforeUpdate(GlobalContext* ctxt) {
    // Update pause menu colors.
    //HudColors_UpdatePauseMenuColors(ctxt);
    
    if (ctxt->pauseCtx.debugMenu != 0)
        return;
    
    Handle_Sword_Swap(ctxt);
    Handle_Shield_Swap(ctxt);
    Handle_Mapping_Items(ctxt);
}

static bool sHoldingStart = false;
bool PauseMenu_SetupUpdate_HasPressedStart(GlobalContext* ctxt) {
    Input* input = CONTROLLER1(ctxt);

    if (CHECK_BTN_ALL(input->pressEdge.buttons.value, BTN_START)) {
        return true;
    }

    if (MISC_CONFIG.flags.easyFrameByFrame) {
        if (CHECK_BTN_ALL(input->current.buttons.value, BTN_START)) {
            if (sHoldingStart) {
                sHoldingStart = false;
                return true;
            }
            sHoldingStart = true;
        } else {
            sHoldingStart = false;
        }
    }

    return false;
}

void Handle_Sword_Swap(GlobalContext* ctxt) {
    if (ctxt->pauseCtx.screenIndex != 2 || ctxt->pauseCtx.cells2.values[2] != 5)
        return;
    
    if (CFG_B_BUTTON_ITEM_ENABLED && gPlayUpdateInput.pressEdge.buttons.cu)
        Handle_Equip_Sword(ctxt);
    
    if (!CFG_SWAP_ENABLED || (!gPlayUpdateInput.pressEdge.buttons.cl && !gPlayUpdateInput.pressEdge.buttons.cr) )
        return;
    uint8_t sword = gSaveContext.perm.unk4C.equipment.sword;
    
    if (sword == 2 && !HAVE_RAZOR_SWORD)
        HAVE_EXTRA_SRAM |= 4;
    if (sword == 3 && !HAVE_GILDED_SWORD) {
        HAVE_EXTRA_SRAM |= 4;
        HAVE_EXTRA_SRAM |= 8;
    }
    
    if (gPlayUpdateInput.pressEdge.buttons.cl) {
        sword--;
        if (sword == 2 && (!HAVE_RAZOR_SWORD || gSaveContext.perm.stolenItem == ITEM_RAZOR_SWORD || REFORGING_RAZOR_SWORD) )
            sword--;
        if (sword == 1 && (gSaveContext.perm.stolenItem == ITEM_KOKIRI_SWORD || REFORGING_KOKIRI_SWORD) )
            sword--;
    }
    else if (gPlayUpdateInput.pressEdge.buttons.cr) {
        sword++;
        if (sword == 1 && gSaveContext.perm.stolenItem == ITEM_KOKIRI_SWORD || REFORGING_KOKIRI_SWORD)
            sword++;
        if (sword == 2 && (!HAVE_RAZOR_SWORD || gSaveContext.perm.stolenItem == ITEM_RAZOR_SWORD || REFORGING_RAZOR_SWORD) )
            sword++;
        if (sword == 3 && (!HAVE_GILDED_SWORD || gSaveContext.perm.stolenItem == ITEM_GILDED_SWORD) )
            sword++;
    }
    
    if (sword >= 0 && sword <= 3 && sword != gSaveContext.perm.unk4C.equipment.sword) {
        gSaveContext.perm.unk4C.equipment.sword    = sword;
        Handle_Equip_Sword(ctxt);
    }
}

void Handle_Equip_Sword(GlobalContext* ctxt) {
    if (gSaveContext.perm.unk4C.equipment.sword != 0)
        gSaveContext.perm.unk4C.formButtonItems[0].buttons[0] = ITEM_KOKIRI_SWORD + gSaveContext.perm.unk4C.equipment.sword - 1;
    else gSaveContext.perm.unk4C.formButtonItems[0].buttons[0] = ITEM_NONE;
    z2_UpdateButtonIcon(ctxt, 0);
    z2_PlaySfx(0x4808);
}

void Handle_Shield_Swap(GlobalContext* ctxt) {
    if (!CFG_SWAP_ENABLED || ctxt->pauseCtx.screenIndex != 2 || ctxt->pauseCtx.cells2.values[2] != 4 || (!gPlayUpdateInput.pressEdge.buttons.cl && !gPlayUpdateInput.pressEdge.buttons.cr) )
        return;
    uint8_t shield = gSaveContext.perm.unk4C.equipment.shield;
    
    if (shield == 2 && !HAVE_MIRROR_SHIELD)
        HAVE_EXTRA_SRAM |= 32;
            
    if (gPlayUpdateInput.pressEdge.buttons.cl) {
        shield--;
        if (shield == 1 && LOST_HERO_SHIELD)
            shield--;
    }
    else if (gPlayUpdateInput.pressEdge.buttons.cr) {
        shield++;
        if (shield == 1 && LOST_HERO_SHIELD)
            shield++;
        if (shield == 2 && !HAVE_MIRROR_SHIELD)
            shield++;
    }
    
    if (shield >= 0 && shield <= 2 && shield != gSaveContext.perm.unk4C.equipment.shield) {
        gSaveContext.perm.unk4C.equipment.shield = ctxt->equipped_shield = shield;
        z2_PlaySfx(0x4808);
    }
}

void Handle_Mapping_Items(GlobalContext* ctxt) {
    uint8_t index = ctxt->pauseCtx.screenIndex;
    if (index != 0 && index != 3)
        return;
    
    uint8_t item = ctxt->pauseCtx.selectedItem;
    uint8_t slot = ctxt->pauseCtx.cells1.item;
    
    InputPad pad = gPlayUpdateInput.pressEdge.buttons;

    if (index == 0 && slot >= SLOT_BOTTLE_1)
        return;
    
    if (item == ITEM_FIRE_ARROW || item == ITEM_ICE_ARROW || item == ITEM_LIGHT_ARROW)
        item += 0x48;
    
    if (!dpad_alt) {
        if (pad.du)
            DPAD_SET1_UP    = Handle_Mapping_Item(DPAD_SET1_UP,    item);
        else if (pad.dr)
            DPAD_SET1_RIGHT = Handle_Mapping_Item(DPAD_SET1_RIGHT, item);
        else if (pad.dd)
            DPAD_SET1_DOWN  = Handle_Mapping_Item(DPAD_SET1_DOWN,  item);
        else if (pad.dl)
            DPAD_SET1_LEFT  = Handle_Mapping_Item(DPAD_SET1_LEFT,  item);
    }
    else {
        if (pad.du)
            DPAD_SET2_UP    = Handle_Mapping_Item(DPAD_SET2_UP,    item);
        else if (pad.dr)
            DPAD_SET2_RIGHT = Handle_Mapping_Item(DPAD_SET2_RIGHT, item);
        else if (pad.dd)
            DPAD_SET2_DOWN  = Handle_Mapping_Item(DPAD_SET2_DOWN,  item);
        else if (pad.dl)
            DPAD_SET2_LEFT  = Handle_Mapping_Item(DPAD_SET2_LEFT,  item);
    }
    
}

uint8_t Handle_Mapping_Item(uint8_t button, uint8_t item) {
    if (button == item) {
        z2_PlaySfx(0x480A);
        return ITEM_NONE;
    }
    else {
        z2_PlaySfx(0x4808);
        return item;
    }
}

//                     11PM    12AM    1AM     2AM     3AM     4AM     5AM     6AM     7AM     8AM     9AM     10AM    11AM    12PM    1PM     2PM     3PM     4PM     5PM     6PM     7PM     8PM     9PM     10PM    11PM    12AM
u16 hour_table[26] = { 0xF555, 0x0000, 0x0AAA, 0x1555, 0x2000, 0x2AAA, 0x3555, 0x4000, 0x4AAA, 0x5555, 0x6000, 0x6AAA, 0x7555, 0x8000, 0x8AAA, 0x9555, 0xA000, 0xAAAA, 0xB555, 0xC000, 0xCAAA, 0xD555, 0xE000, 0xEAAA, 0xF555, 0x0000 };

u16 Get_Next_Hour_From_Table(u16 time) {
    for (uint8_t i = 1; i <= 24; i++)
        if (new_time >= hour_table[i] && new_time < hour_table[i+1])
            return hour_table[i+1];
    return 0;
}

u16 Get_Previous_Hour_From_Table(u16 time) {
    for (uint8_t i = 1; i <= 24; i++)
        if (new_time == hour_table[i])
            return hour_table[i-1];
    return 0;
}

uint8_t Get_Hours_Remaining_From_Table(u16 time) {
    for (uint8_t i = 1; i <= 24; i++)
        if (new_time >= hour_table[i])
            if ( (i < 24 && new_time < hour_table[i+1]) || i == 24) {
                if (i <= 6 || (i == 7 && add_time > 0) )
                    return (i + 18 - 1);
                else return (i - 6 - 1);
            }
    return 0;
}

void Handle_Clock_Controls(GlobalContext* ctxt) {
    if (!CFG_CLOCK_CONTROL_ENABLED || !IS_PLAYABLE || ctxt->pauseCtx.debugMenu != 0 || ctxt->pauseCtx.unk1F0 != 0)
        return;
    
    if (changed_time && ctxt->pauseCtx.state == 0x1A) {
        if (new_time == 0x4000)
            gSaveContext.perm.time = 0x3FD0;
        else gSaveContext.perm.time = new_time;
        changed_time = add_time = new_time = 0;
    }
    
    if (ctxt->pauseCtx.state != 6)
     return;
    
    if (!IS_ABILITY_ACTIVATED(CFG_CLOCK_CONTROL_ENABLED-1) || (gSaveContext.perm.inv.items[0] != ITEM_OCARINA && gSaveContext.perm.inv.items[0] != ITEM_DEKU_PIPES && gSaveContext.perm.inv.items[0] != ITEM_GORON_DRUMS && gSaveContext.perm.inv.items[0] != ITEM_ZORA_GUITAR) || !gSaveContext.perm.inv.questStatus.songOfTime) {
        if (ctxt->state.input[0].pressEdge.buttons.b)
            z2_PlaySfx(0x4806);
        ctxt->state.input[0].pressEdge.buttons.b = 0;
        return;
    }
        
    if (ctxt->state.input[0].pressEdge.buttons.b && !block) {
        clock_controls ^= 1;
        clock_control_button_frames = 0;
        if (clock_controls) {
            z2_PlaySfx(0x4813);
            if (!changed_time) {
                new_time = gSaveContext.perm.time;
                add_time = 0;
            }
        }
        else z2_PlaySfx(0x4814);
    }
    ctxt->state.input[0].pressEdge.buttons.b = 0;
    
    if (ctxt->state.input[0].pressEdge.buttons.s)
        clock_controls = 0;
    
    if (!clock_controls)
        return;
    
    if (ctxt->state.input[0].pressEdge.buttons.a) {
        clock_controls = 0;
        z2_PlaySfx(0x4814);
        changed_time = (new_time != gSaveContext.perm.time);
    }
    
    if (clock_control_button_frames > 0 && clock_control_button_frames != 255)
        clock_control_button_frames--;
    if (ctxt->state.input[0].pressEdge.buttons.cl || ctxt->state.input[0].pressEdge.buttons.cr)
        clock_control_button_frames = 0;
    
    if (ctxt->state.input[0].current.buttons.cl && clock_control_button_frames == 0) {
        clock_control_button_frames = 10;
        if (add_time > 1) {
            new_time = Get_Previous_Hour_From_Table(new_time);
            add_time--;
            z2_PlaySfx(0x4808);
        }
        else if (new_time != gSaveContext.perm.time) {
            new_time = gSaveContext.perm.time;
            add_time = 0;
            z2_PlaySfx(0x4808);
        }
        else {
            z2_PlaySfx(0x4806);
            clock_control_button_frames = 255;
        }
    }
    else if (ctxt->state.input[0].current.buttons.cr && clock_control_button_frames == 0) {
        clock_control_button_frames = 10;
        if (add_time > 0 && new_time == 0x4000) {
            z2_PlaySfx(0x4806);
            clock_control_button_frames = 255;
        }
        else if (gSaveContext.perm.day == 3 && new_time >= 0x3555 && new_time < 0x4000) {
            z2_PlaySfx(0x4806);
            clock_control_button_frames = 255;
        }
        else {
            new_time = Get_Next_Hour_From_Table(new_time);
            add_time++;
            z2_PlaySfx(0x4808);
        }
    }
    
    ctxt->state.input[0].pressEdge.buttons.a  = 0;
    ctxt->state.input[0].pressEdge.buttons.du = ctxt->state.input[0].pressEdge.buttons.dr = ctxt->state.input[0].pressEdge.buttons.dd = ctxt->state.input[0].pressEdge.buttons.dl = 0;
    ctxt->state.input[0].pressEdge.buttons.cu = ctxt->state.input[0].pressEdge.buttons.cr = ctxt->state.input[0].pressEdge.buttons.cd = ctxt->state.input[0].pressEdge.buttons.cl = 0;
}

void Draw_Clock_Controls(GlobalContext* ctxt, DispBuf* db) {
    if (!clock_controls || !CFG_CLOCK_CONTROL_ENABLED || !IS_PLAYABLE)
        return;
    
    gDPSetCombineMode(db->p++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    
    u16 x = 320 + (CFG_WS_ENABLED * 104) - 90;
    u16 y = 240 - 40;
    
    gDPSetPrimColor(db->p++, 0, 0, 255, 255, 153, 255);
    
    Sprite_Load(db, &gParameterNoteButtons, 3, 1);
    Sprite_Draw(db, &gParameterNoteButtons, 0, x + 10, y + 0,  16, 16);
    Sprite_Load(db, &gParameterNoteButtons, 2, 1);
    Sprite_Draw(db, &gParameterNoteButtons, 0, x + 50, y + 0,  16, 16);
    
    const uint8_t isDay = (NEW_TIME_HOURS >= 6 && NEW_TIME_HOURS < 18) ? 1: 0;
    if (isDay)
        gDPSetPrimColor(db->p++, 0, 0, 255, 100, 110, 255);
    else gDPSetPrimColor(db->p++, 0, 0, 255, 255, 55, 255);
    
    Sprite_Load(db, &gParameterSunMoon,     isDay, 1);
    Sprite_Draw(db, &gParameterSunMoon,     0, x + 0,  y + 15, 16, 16);
    
    gDPSetPrimColor(db->p++, 0, 0, 255, 255, 255, 255);
    
    uint8_t hours_left = 72 - (24 * (gSaveContext.perm.day - 1)) - Get_Hours_Remaining_From_Table(new_time);
    
    if (hours_left < 10) {
        Sprite_Load(db, &gParameterCounter, hours_left, 1);
        Sprite_Draw(db, &gParameterCounter, 0, x + 34, y + 0,  8,  16);
    }
    else {
        Sprite_Load(db, &gParameterCounter, hours_left / 10, 1);
        Sprite_Draw(db, &gParameterCounter, 0, x + 28, y + 0,  8,  16);
        Sprite_Load(db, &gParameterCounter, hours_left % 10, 1);
        Sprite_Draw(db, &gParameterCounter, 0, x + 38, y + 0,  8,  16);
    }
    
    uint8_t hours = (NEW_TIME_HOURS > 12) ? (NEW_TIME_HOURS - 12) : NEW_TIME_HOURS;
    
    if (hours >= 10) {
        Sprite_Load(db, &gParameterCounter, hours / 10, 1);
        Sprite_Draw(db, &gParameterCounter, 0, x + 20, y + 15, 8,  16);
    }
    Sprite_Load(db, &gParameterCounter,     hours % 10, 1);
    Sprite_Draw(db, &gParameterCounter,     0, x + 30, y + 15, 8,  16);
    
    Sprite_Load(db, &gParameterCounter,     10, 1);
    Sprite_Draw(db, &gParameterCounter,     0, x + 40, y + 15, 8,  16);
    
    u16 minutes = new_time;
    while (minutes >= TIME_ONE_HOUR)
        minutes -= TIME_ONE_HOUR;
    minutes /= 45;
    
    Sprite_Load(db, &gParameterCounter,     minutes / 10, 1);
    Sprite_Draw(db, &gParameterCounter,     0, x + 50, y + 15, 8,  16);
    Sprite_Load(db, &gParameterCounter,     minutes % 10, 1);
    Sprite_Draw(db, &gParameterCounter,     0, x + 60, y + 15, 8,  16);
}

void Draw_Hud_Toggle(GlobalContext* ctxt, DispBuf* db) {
    if (ctxt->pauseCtx.state != 6 || !block || !CFG_HIDE_HUD_ENABLED)
        return;
    
    uint8_t x     = 50;
    uint8_t y     = 10;
    uint8_t width = 10;
    
    if (hud_hide == 0)
        gDPSetPrimColor(db->p++, 0, 0, 0, 255, 0, 255);
    else if (hud_hide == 1)
        gDPSetPrimColor(db->p++, 0, 0, 255, 255, 0, 255);
    else gDPSetPrimColor(db->p++, 0, 0, 255, 0, 0, 255);
    
    gDPSetCombineMode(db->p++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gSPTextureRectangle(db->p++, (x + 1) << 2, (y + 1) << 2, (x + width - 1) << 2, (y + width - 1) << 2, 0, 0, 0, 1 << 10, 1 << 10);
    
    gDPSetCombineMode(db->p++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColor(db->p++, 0, 0, 255, 255, 255, 255);
    Sprite_Load(db, &gHudToggle, 0, 1);
    Sprite_Draw(db, &gHudToggle, 0, x, y, width, width);
    
    Text_Print("HUD", x - 25, y - 1);
    Text_Flush(db);
}