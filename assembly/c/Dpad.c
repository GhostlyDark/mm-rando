#include <stdbool.h>
#include <z64.h>
#include "Buttons.h"
#include "Dpad.h"
#include "HudColors.h"
#include "Reloc.h"
#include "Sprite.h"
#include "Util.h"
#include "Extra.h"

extern uint8_t CFG_DUAL_DPAD_ENABLED;
extern uint8_t CFG_WS_ENABLED;
extern uint8_t CFG_B_BUTTON_ITEM_ENABLED;
extern uint8_t CFG_SWAP_ENABLED;
extern uint8_t CFG_FLOW_OF_TIME_ENABLED;
extern uint8_t CFG_INSTANT_ELEGY_ENABLED;
extern uint8_t CFG_FPS_ENABLED;
extern uint8_t dpad_alt;
extern uint8_t block;

// D-Pad configuration structure that can be set by a randomizer.
struct DpadConfig DPAD_CONFIG = {
    .magic = DPAD_CONFIG_MAGIC,
    .version = 0,
    .primary = { ITEM_NONE },
    .state = DPAD_STATE_DEFAULTS,
    .display = DPAD_DISPLAY_LEFT,
    .reserved = { 0 },
};

// Default D-Pad values that will be used if config values undefined.
const static u8 gDpadDefault[4] = {
    ITEM_DEKU_MASK,
    ITEM_ZORA_MASK,
    ITEM_OCARINA,
    ITEM_GORON_MASK,
};

// Indicates which item textures are currently loaded into our buffer.
static u8 gTextureItems[4] = {
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
};

// Position of D-Pad texture.
const static u16 gPosition[3][3] = {
    { 30,  60 },  // Left
    { 270, 75 },  // Right
    { 374, 75 },  // Right WS
};

// Positions of D-Pad item textures, relative to main texture.
const static s16 gPositions[4][2] = {
    { 1, -16 },
    { 15, 0 },
    { 1, 13 },
    { -15, 0 },
};

// Whether or not D-Pad items are usable, according to z2_UpdateButtonUsability.
static bool gUsable[4] = { false };

// Whether the previous frame was a "minigame" frame.
static bool gWasMinigame = false;

static bool IsMaskTaken(u8 mask) {
    if ( (mask == ITEM_POSTMAN_HAT        && 0x1  & gSaveContext.masksGivenOnMoon[1]) || \
         (mask == ITEM_ALL_NIGHT_MASK     && 0x4  & gSaveContext.masksGivenOnMoon[0]) || \
         (mask == ITEM_BLAST_MASK         && 0x2  & gSaveContext.masksGivenOnMoon[2]) || \
         (mask == ITEM_STONE_MASK         && 0x80 & gSaveContext.masksGivenOnMoon[1]) || \
         (mask == ITEM_GREAT_FAIRY_MASK   && 0x4  & gSaveContext.masksGivenOnMoon[1]) || \
         (mask == ITEM_DEKU_MASK          && 0x10 & gSaveContext.masksGivenOnMoon[2]) || \
         (mask == ITEM_KEATON_MASK        && 0x10 & gSaveContext.masksGivenOnMoon[0]) || \
         (mask == ITEM_BREMEN_MASK        && 0x1  & gSaveContext.masksGivenOnMoon[2]) || \
         (mask == ITEM_BUNNY_HOOD         && 0x8  & gSaveContext.masksGivenOnMoon[0]) || \
         (mask == ITEM_DON_GERO_MASK      && 0x10 & gSaveContext.masksGivenOnMoon[1]) || \
         (mask == ITEM_MASK_OF_SCENTS     && 0x4  & gSaveContext.masksGivenOnMoon[2]) || \
         (mask == ITEM_GORON_MASK         && 0x20 & gSaveContext.masksGivenOnMoon[2]) || \
         (mask == ITEM_ROMANI_MASK        && 0x40 & gSaveContext.masksGivenOnMoon[0]) || \
         (mask == ITEM_CIRCUS_LEADER_MASK && 0x80 & gSaveContext.masksGivenOnMoon[0]) || \
         (mask == ITEM_KAFEI_MASK         && 0x2  & gSaveContext.masksGivenOnMoon[0]) || \
         (mask == ITEM_COUPLE_MASK        && 0x2  & gSaveContext.masksGivenOnMoon[1]) || \
         (mask == ITEM_MASK_OF_TRUTH      && 0x2  & gSaveContext.masksGivenOnMoon[1]) || \
         (mask == ITEM_ZORA_MASK          && 0x40 & gSaveContext.masksGivenOnMoon[2]) || \
         (mask == ITEM_KAMARO_MASK        && 0x20 & gSaveContext.masksGivenOnMoon[1]) || \
         (mask == ITEM_GIBDO_MASK         && 0x8  & gSaveContext.masksGivenOnMoon[1]) || \
         (mask == ITEM_GARO_MASK          && 0x20 & gSaveContext.masksGivenOnMoon[0]) || \
         (mask == ITEM_CAPTAIN_HAT        && 0x40 & gSaveContext.masksGivenOnMoon[1]) || \
         (mask == ITEM_GIANT_MASK         && 0x8  & gSaveContext.masksGivenOnMoon[2]) || \
         (mask == ITEM_FIERCE_DEITY_MASK  && 0x80 & gSaveContext.masksGivenOnMoon[2]) )
        return 1;
    return 0;
}

static bool HasInventoryItem(u8 item) {
    for (int i = 0; i < 0x18; i++) {
        if (gSaveContext.perm.inv.items[i] == item || (gSaveContext.perm.inv.masks[i] == item && !IsMaskTaken(item)) ) {
            return true;
        }
    }
    return false;
}

static bool HaveAny(const u8* dpad) {
    for (int i = 0; i < 4; i++) {
        if (HasInventoryItem(dpad[i])) {
            return true;
        }
    }
    return false;
}

static bool TryUseItem(GlobalContext* ctxt, ActorPlayer* player, u8 item) {
    // Magic arrow item
    if ( (item == ITEM_BOW_FIRE_ARROW  && gSaveContext.perm.inv.items[SLOT_FIRE_ARROW]  == ITEM_FIRE_ARROW) || \
         (item == ITEM_BOW_ICE_ARROW   && gSaveContext.perm.inv.items[SLOT_ICE_ARROW]   == ITEM_ICE_ARROW)  || \
         (item == ITEM_BOW_LIGHT_ARROW && gSaveContext.perm.inv.items[SLOT_LIGHT_ARROW] == ITEM_LIGHT_ARROW) ) {
            z2_UseItem(ctxt, player, item);
            return true;
         }
    
    // Try to find slot in item or mask inventories.
    if (HasInventoryItem(item)) {
        z2_UseItem(ctxt, player, item);
        return true;
    } else {
        return false;
    }
}

/**
 * Helper function used to check if C buttons are disabled due to the current entrance.
 **/
static bool AreCItemsDisabledByEntrance(GlobalContext* ctxt) {
    // Hardcoded entrance checks to disable C buttons, normally performed by function 0x80111CB4:
    // Id 0x8E10: Beaver Race
    // Id 0xD010: Goron Race
    // Checks execute state to prevent fading D-Pad when loading scene with entrance.
    return (gSaveContext.perm.entrance.value == 0x8E10 || gSaveContext.perm.entrance.value == 0xD010) && ctxt->state.running != 0;
}

static void GetDpadItemUsability(GlobalContext* ctxt, bool* dest) {
    uint8_t set1[4] = { DPAD_SET1_UP, DPAD_SET1_RIGHT, DPAD_SET1_DOWN, DPAD_SET1_LEFT };
    uint8_t set2[4] = { DPAD_SET2_UP, DPAD_SET2_RIGHT, DPAD_SET2_DOWN, DPAD_SET2_LEFT };
    
    for (int i = 0; i < 4; i++) {
        if (AreCItemsDisabledByEntrance(ctxt))
            dest[i] = false;
        if (!dpad_alt)
            dest[i] = Buttons_CheckCItemUsable(ctxt, set1[i]);
        else dest[i] = Buttons_CheckCItemUsable(ctxt, set2[i]);
    }
}

static bool IsAnyItemUsable(const u8* dpad, const bool* usable) {
    for (int i = 0; i < 4; i++) {
        if (HasInventoryItem(dpad[i]) && usable[i]) {
            return true;
        }
    }
    return false;
}

static void LoadTexture(u8* buf, int idx, int length, u8 item) {
    u32 phys = z2_GetFilePhysAddr(ItemTextureFileVROM);
    u8* dest = buf + (idx * length);
    z2_LoadFileFromArchive(phys, item, dest, length);
    gTextureItems[idx] = item;
}

static void LoadTextureFromSprite(Sprite* sprite, int idx, u8 item) {
    int tileLen = Sprite_GetBytesPerTile(sprite);
    LoadTexture(sprite->buf, idx, tileLen, item);
}

static u16 UpdateYPosition(u16 x, u16 y, u16 padding) {
    u16 heartCount = gSaveContext.perm.unk24.maxLife / 0x10;

    // Check if we have second row of hearts
    bool hearts = heartCount > 10;
    // Check if we have magic
    bool magic = gSaveContext.perm.unk24.hasMagic != 0;
    // Check if there's a timer
    bool timer = IS_TIMER_VISIBLE(gSaveContext.extra.timers[TIMER_INDEX_POE_SISTERS]) ||
                 IS_TIMER_VISIBLE(gSaveContext.extra.timers[TIMER_INDEX_TREASURE_CHEST_GAME]) ||
                 IS_TIMER_VISIBLE(gSaveContext.extra.timers[TIMER_INDEX_DROWNING]);

    // If on left-half of screen
    if (x < 160) {
        // Calculate a minimum y position based on heart rows and magic
        // This is to avoid the D-Pad textures interfering with the hearts/magic UI
        u16 minimum = 50 + padding;
        if (hearts)
            minimum += 10;
        if (magic)
            minimum += 16;
        if (timer) {
            if (magic) {
                minimum += 10;
            } else {
                minimum += 26;
            }
        }
        y = (y > minimum ? y : minimum);
    }

    return y;
}

static bool IsMinigameFrame(void) {
    bool result = false;

    if (gWasMinigame) {
        result = true;
    }

    // Note on state 6:
    // If on Epona, and holding "B" for bow, then press "A" while holding "B", the game state
    // will go from: 0xC, 0x6, 0x32, 0xC. Thus we need to mark 0x6 as a "minigame frame" as well.
    // Riding Epona to a new area: 0xC, 0x32, 0x6, 0x1, 0x1...
    //
    // Note on state 1 (transition):
    // In the Deku playground, can go from 0xC to 0x1 when cutscene-transitioning to the business scrub.
    // Thus, if the minigame state goes directly to the transition state, consider that a minigame frame.
    gWasMinigame = (gSaveContext.extra.buttonsState.state == BUTTONS_STATE_MINIGAME ||
                   (gWasMinigame && gSaveContext.extra.buttonsState.state == BUTTONS_STATE_TRANSITION) ||
                    gSaveContext.extra.buttonsState.state == 6);
    return result || gWasMinigame;
}

void Dpad_BeforePlayerActorUpdate(ActorPlayer* player, GlobalContext* ctxt) {
    // If disabled, do nothing
    if (DPAD_CONFIG.state == DPAD_STATE_DISABLED) {
        return;
    }

    // Update usability flags for later use in Dpad_Draw.
    GetDpadItemUsability(ctxt, gUsable);
}

void Dpad_ClearItemTextures(void) {
    for (int i = 0; i < 4; i++) {
        gTextureItems[i] = ITEM_NONE;
    }
}

void Dpad_Init(void) {
    // If using default values, overwrite DPAD_CONFIG items with defaults
    if (DPAD_CONFIG.state == DPAD_STATE_DEFAULTS) {
        for (int i = 0; i < 4; i++) {
            DPAD_CONFIG.primary.values[i] = gDpadDefault[i];
        }
    }
}

bool Dpad_IsEnabled(void) {
    return (DPAD_CONFIG.state == DPAD_STATE_ENABLED)
        || (DPAD_CONFIG.state == DPAD_STATE_DEFAULTS);
}

/**
 * Hook function used to check whether or not to call z2_UseItem.
 *
 * Checks D-Pad input for whether or not to use an item, and if so returns true to exit the caller
 * function early.
 **/
bool Dpad_Handle(ActorPlayer* player, GlobalContext* ctxt) {
    InputPad padPressed = ctxt->state.input[0].pressEdge.buttons;

    // If disabled, do nothing
    if (DPAD_CONFIG.state == DPAD_STATE_DISABLED) {
        return false;
    }
    
    // Check general buttons state to know if we can use C buttons at all
    // Note: After collecting a stray fairy (and possibly in other cases) the state flags are set
    // to 0 despite the game running normally.
    if (gSaveContext.extra.buttonsState.state != BUTTONS_STATE_NORMAL &&
        gSaveContext.extra.buttonsState.state != BUTTONS_STATE_BLACK_SCREEN) {
        return false;
    }

    // Make sure certain Link state flags are cleared before processing D-Pad input.
    u32 flags1 = PLAYER_STATE1_HOLD | PLAYER_STATE1_MOVE_SCENE | PLAYER_STATE1_EPONA;
    if ((player->stateFlags.state1 & flags1) != 0) {
        return false;
    }
    
    if (ctxt->state.input[0].current.buttons.l && L_PAD_ENABLED) {
        if (padPressed.du)
            Handle_Instant_Elegy(ctxt);
        if (padPressed.dr)
            Handle_Sword_Toggle(ctxt);
        if (padPressed.dd)
            Handle_Invert_Time();
        if (padPressed.dl)
            Handle_Shield_Toggle(ctxt);
        if (padPressed.z)
            Handle_FPS_Toggle();
        if (padPressed.r)
            Handle_Dual_Dpad(ctxt);
        return false;
    }
    
    // Lens of Truth, Masks, Items not unequiping when not on C buttons
    (*(uint16_t*) 0x8011660C) = (*(uint16_t*) 0x80116618) = (*(uint16_t*) 0x80116624) = (*(uint16_t*) 0x806ED4B0) = (*(uint16_t*) 0x806ED4FC) = (*(uint16_t*) 0x8018BFE0) = (*(uint16_t*) 0x806ED30C) = 0x1000;
    if (gSaveContext.perm.stolenItem == ITEM_FAIRY_SWORD && player->heldItemActionParam == PLAYER_IA_SWORD_GREAT_FAIRY)
        (*(uint16_t*) 0x806ED4B0) = (*(uint16_t*) 0x806ED4FC) = (*(uint16_t*) 0x8018BFE0) = 0x1440;
    
    if (!dpad_alt) {
        if (padPressed.du && gUsable[0])
            return TryUseItem(ctxt, player, DPAD_SET1_UP);
        else if (padPressed.dr && gUsable[1])
            return TryUseItem(ctxt, player, DPAD_SET1_RIGHT);
        else if (padPressed.dd && gUsable[2])
            return TryUseItem(ctxt, player, DPAD_SET1_DOWN);
        else if (padPressed.dl && gUsable[3])
            return TryUseItem(ctxt, player, DPAD_SET1_LEFT);
    }
    else {
        if (padPressed.du && gUsable[0])
            return TryUseItem(ctxt, player, DPAD_SET2_UP);
        else if (padPressed.dr && gUsable[1])
            return TryUseItem(ctxt, player, DPAD_SET2_RIGHT);
        else if (padPressed.dd && gUsable[2])
            return TryUseItem(ctxt, player, DPAD_SET2_DOWN);
        else if (padPressed.dl && gUsable[3])
            return TryUseItem(ctxt, player, DPAD_SET2_LEFT);
    }

    return false;
}

uint8_t Dpad_DrawAlt(GlobalContext* ctxt, DispBuf* db) {    
    if (!IS_PLAYABLE || !block || !L_PAD_ENABLED || DPAD_CONFIG.display == DPAD_DISPLAY_NONE)
        return false;
    
    // Get index of main sprite position (left or right)
    uint8_t posIdx = 0;
    if (DPAD_CONFIG.display == DPAD_DISPLAY_RIGHT)
        posIdx == CFG_WS_ENABLED ? 2 : 1;

    // Main sprite position
    u16 x = gPosition[posIdx][0];
    u16 y = gPosition[posIdx][1];
    y = UpdateYPosition(x, y, 10);
    
    gDPSetPrimColor(db->p++, 0, 0, HUD_COLOR_CONFIG.dpad.r, HUD_COLOR_CONFIG.dpad.g, HUD_COLOR_CONFIG.dpad.b, 255);
    gDPSetCombineMode(db->p++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    Sprite_Load(db, &gSpriteDpad, 0, 1);
    Sprite_Draw(db, &gSpriteDpad, 0, x, y, 16, 16);
    
    Sprite* sprite = Sprite_GetItemTexturesSprite();
    gDPSetPrimColor(db->p++, 0, 0, 0xFF, 0xFF, 0xFF, 255);
    
    // Right
    if (CFG_SWAP_ENABLED && gSaveContext.perm.unk4C.equipment.sword > 0) {
        LoadTextureFromSprite(sprite, 1, ITEM_KOKIRI_SWORD + gSaveContext.perm.unk4C.equipment.sword - 1);
        Sprite_Load(db, sprite, 1, 1);
        Sprite_Draw(db, sprite, 0, x + gPositions[1][0], y + gPositions[1][1], 16, 16);
    }
    
    // Left
    if (CFG_SWAP_ENABLED && gSaveContext.perm.unk4C.equipment.shield > 0) {
        LoadTextureFromSprite(sprite, 3, ITEM_HERO_SHIELD + gSaveContext.perm.unk4C.equipment.shield - 1);
        Sprite_Load(db, sprite, 3, 1);
        Sprite_Draw(db, sprite, 0, x + gPositions[3][0], y + gPositions[3][1], 16, 16);
    }
    
    // Top
    if (CFG_INSTANT_ELEGY_ENABLED) {
        gDPSetPrimColor(db->p++, 0, 0, 255, 165, 0, Can_Use_Elegy(ctxt) ? 255 : 160);
        Sprite_Load(db, &gDungeonMapLinkHead, 0, 1);
        Sprite_Draw(db, &gDungeonMapLinkHead, 0, x + gPositions[0][0] + 1, y + gPositions[0][1] + 4, 12, 12);
    }
    
    // Bottom
    if (CFG_FLOW_OF_TIME_ENABLED && gStaticContext.timeSpeed != 0) {
        if (gSaveContext.perm.timeSpeed == 0)
            gDPSetPrimColor(db->p++, 0, 0, 0, 170, 100, 255);
        else gDPSetPrimColor(db->p++, 0, 0, 0, 100, 205, 255);
        Sprite_Load(db, &gParameterSunMoon, !gSaveContext.perm.isNight, 1);
        Sprite_Draw(db, &gParameterSunMoon, 0, x + gPositions[2][0] - 1, y + gPositions[2][1], 16, 16);
    }
    
    // Top-Right
    if (CFG_DUAL_DPAD_ENABLED) {
        if (!dpad_alt)
            gDPSetPrimColor(db->p++, 0, 0, 255, 0, 0, 255);
        else gDPSetPrimColor(db->p++, 0, 0, 0, 255, 0, 255);
        Sprite_Load(db, &gSpriteDpad, 0, 1);
        Sprite_Draw(db, &gSpriteDpad, 0, x + gPositions[0][0] + 16, y + gPositions[0][1] + 4, 12, 12);
    }
    
    // Top-Left
    if (CFG_FPS_ENABLED) {
        x = x + gPositions[0][0] - 20;
        y = y + gPositions[0][1] + 4;
    
        uint16_t fps = 60 / ctxt->state.framerateDivisor;
        if (fps == 20)
            gDPSetPrimColor(db->p++, 0, 0, 255, 0, 0, 255);
        else gDPSetPrimColor(db->p++, 0, 0, 0, 255, 0, 255);
    
        Sprite_Load(db, &gParameterAmmoDigit, fps / 10, 1);
        Sprite_Draw(db, &gParameterAmmoDigit, 0, x, y + 0, 8, 8);
        Sprite_Load(db, &gParameterAmmoDigit, fps % 10, 1);
        Sprite_Draw(db, &gParameterAmmoDigit, 0, x + 8, y , 8, 8);
    }
    
    return true;
}

/**
 * Hook function called directly before drawing triangles and item textures on C buttons.
 *
 * Draws D-Pad textures to the overlay display list.
 **/
void Dpad_Draw(GlobalContext* ctxt, DispBuf* db) {
    bool isMinigame = IsMinigameFrame();
    ActorPlayer* player = GET_PLAYER(ctxt);
    
    // Draw ammo for B button
    if (CFG_B_BUTTON_ITEM_ENABLED && !isMinigame && IS_PLAYABLE && player->form == PLAYER_FORM_HUMAN && ctxt->pauseCtx.state <= 5)
        Draw_Ammo(db, ctxt->interfaceCtx.alphas.buttonB, gSaveContext.perm.unk4C.formButtonItems[0].buttons[0], CFG_WS_ENABLED ? 0x10A : 0xA2, 0x23, 8, -2);

    // If disabled or hiding, don't draw
    if (DPAD_CONFIG.state == DPAD_STATE_DISABLED || DPAD_CONFIG.display == DPAD_DISPLAY_NONE) {
        return;
    }
    
    uint8_t set1[4] = { DPAD_SET1_UP, DPAD_SET1_RIGHT, DPAD_SET1_DOWN, DPAD_SET1_LEFT };
    uint8_t set2[4] = { DPAD_SET2_UP, DPAD_SET2_RIGHT, DPAD_SET2_DOWN, DPAD_SET2_LEFT };
    
    // If we don't have any D-Pad items, draw nothing
    if (!HaveAny(set1) && !dpad_alt)
        return;
    else if (!HaveAny(set2) && dpad_alt)
        return;

    // Check for minigame frame, and do nothing unless transitioning into minigame
    // In which case the C-buttons alpha will be used instead for fade-in
    if (isMinigame && gSaveContext.extra.buttonsState.previousState != BUTTONS_STATE_MINIGAME) {
        return;
    }

    // Check if C button items are disabled for a specific entrance.
    // Used to prevent drawing D-Pad during 1 frame before Goron Race.
    if (AreCItemsDisabledByEntrance(ctxt) && ctxt->interfaceCtx.alphas.buttonCLeft == 0) {
        return;
    }

    // Use button A alpha by default for fading textures out
    u8 primAlpha = ctxt->interfaceCtx.alphas.buttonA & 0xFF;
    // If in minigame, the C buttons fade out and so should the D-Pad
    if (gSaveContext.extra.buttonsState.state == BUTTONS_STATE_MINIGAME ||
        gSaveContext.extra.buttonsState.state == BUTTONS_STATE_BOAT_ARCHERY ||
        gSaveContext.extra.buttonsState.state == BUTTONS_STATE_SWORDSMAN_GAME ||
        isMinigame) {
        primAlpha = ctxt->interfaceCtx.alphas.buttonCLeft & 0xFF;
    }

    // Check if any items shown on the D-Pad are usable
    // If none are, draw main D-Pad sprite faded    
    if (!IsAnyItemUsable(set1, gUsable) && primAlpha > 0x4A && !dpad_alt)
        primAlpha = 0x4A;
    else if (!IsAnyItemUsable(set2, gUsable) && primAlpha > 0x4A && dpad_alt)
        primAlpha = 0x4A;
    
    if (ctxt->pauseCtx.state == 0) {
        if (!dpad_alt) {
            if (set1[0] == ITEM_NONE && set1[1] == ITEM_NONE && set1[2] == ITEM_NONE && set1[3] == ITEM_NONE)
                return;
        }
        else {
            if (set2[0] == ITEM_NONE && set2[1] == ITEM_NONE && set2[2] == ITEM_NONE && set2[3] == ITEM_NONE)
                return;
        }
    }

    // Show faded while flying as a Deku
    if (((player->stateFlags.state3 & PLAYER_STATE3_DEKU_AIR) != 0) && primAlpha > 0x4A) {
        primAlpha = 0x4A;
    }

    // Get index of main sprite position (left or right)
    u8 posIdx = 0;
    if (DPAD_CONFIG.display == DPAD_DISPLAY_RIGHT)
        posIdx == CFG_WS_ENABLED ? 2 : 1;

    // Main sprite position
    u16 x = gPosition[posIdx][0];
    u16 y = gPosition[posIdx][1];
    y = UpdateYPosition(x, y, 10);

    // Main sprite color
    Color color = HUD_COLOR_CONFIG.dpad;
    
    if (ctxt->pauseCtx.state == 6)
        primAlpha = 0xFF;
    
    gDPSetPrimColor(db->p++, 0, 0, color.r, color.g, color.b, primAlpha);
    gDPSetCombineMode(db->p++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    Sprite_Load(db, &gSpriteDpad, 0, 1);
    Sprite_Draw(db, &gSpriteDpad, 0, x, y, 16, 16);
    
    Sprite* sprite = Sprite_GetItemTexturesSprite();
    for (int i = 0; i < 4; i++) {
        u8 value;
        if (!dpad_alt)
            value = set1[i];
        else value = set2[i];

        // Calculate x/y from relative positions
        u16 ix = x + gPositions[i][0];
        u16 iy = y + gPositions[i][1];
        
        // Update value for magic arrows and Ssow nothing if D-Pad slot is empty, or item is not in inventory.
        if ( (value == ITEM_BOW_FIRE_ARROW  && gSaveContext.perm.inv.items[SLOT_FIRE_ARROW]  == ITEM_FIRE_ARROW) || \
             (value == ITEM_BOW_ICE_ARROW   && gSaveContext.perm.inv.items[SLOT_ICE_ARROW]   == ITEM_ICE_ARROW)  || \
             (value == ITEM_BOW_LIGHT_ARROW && gSaveContext.perm.inv.items[SLOT_LIGHT_ARROW] == ITEM_LIGHT_ARROW) ) { }
        else if (value == ITEM_NONE || !HasInventoryItem(value))
            continue;

        // Draw faded
        u8 alpha = primAlpha;
        if (!gUsable[i] && alpha > 0x4A) {
            alpha = 0x4A;
        }
        gDPSetPrimColor(db->p++, 0, 0, 0xFF, 0xFF, 0xFF, alpha);

        // If D-Pad item has changed, load new texture on the fly.
        if (gTextureItems[i] != value) {
            LoadTextureFromSprite(sprite, i, value);
        }
        
        // Icon size when mask equipped
        u8 size = ( (player->mask > 0 && player->mask < 15 && player->mask == value - ITEM_FIERCE_DEITY_MASK) || \
        (player->form == PLAYER_FORM_DEKU && value == ITEM_DEKU_MASK) || (player->form == PLAYER_FORM_GORON && value == ITEM_GORON_MASK) || (player->form == PLAYER_FORM_ZORA && value == ITEM_ZORA_MASK) || (player->form == PLAYER_FORM_FIERCE_DEITY && value == ITEM_FIERCE_DEITY_MASK) ) \
        ? 18 : 16;
        u8 correction = (size - 16) / 2;
        
        Sprite_Load(db, sprite, i, 1);
        Sprite_Draw(db, sprite, 0, ix - correction, iy - correction, size, size);
        
        // Draw ammo
        Draw_Ammo(db, primAlpha, value, ix + 1, iy + 11, 4, -1);
    }
}

void Draw_Ammo(DispBuf* db, u8 alpha, u8 item, u8 x, u8 y, u8 size, s8 spacing) {
    s8 ammo     = -1;
    u8 capacity = 0;
    
    if (item >= ITEM_BOW && item <= ITEM_LIGHT_ARROW || item >= ITEM_BOW_FIRE_ARROW && item <= ITEM_BOW_LIGHT_ARROW) {
        ammo     = gSaveContext.perm.inv.quantities[SLOT_BOW];
        capacity = gItemUpgradeCapacity.arrowCapacity[gSaveContext.perm.inv.upgrades.quiver];
    }
    else if (item == ITEM_BOMB) {
        ammo     = gSaveContext.perm.inv.quantities[SLOT_BOMB];
        capacity = gItemUpgradeCapacity.bombCapacity[gSaveContext.perm.inv.upgrades.bombBag];
    }
    else if (item == ITEM_BOMBCHU) {
        ammo     = gSaveContext.perm.inv.quantities[SLOT_BOMBCHU];
        capacity = gItemUpgradeCapacity.bombCapacity[gSaveContext.perm.inv.upgrades.bombBag];
    }
    else if (item == ITEM_DEKU_STICK) {
        ammo     = gSaveContext.perm.inv.quantities[SLOT_DEKU_STICK];
        capacity = gItemUpgradeCapacity.stickCapacity[gSaveContext.perm.inv.upgrades.dekuStick];
    }
    else if (item == ITEM_DEKU_NUT) {
        ammo     = gSaveContext.perm.inv.quantities[SLOT_DEKU_NUT];
        capacity = gItemUpgradeCapacity.nutCapacity[gSaveContext.perm.inv.upgrades.dekuNut];
    }
    else if (item == ITEM_MAGIC_BEAN) {
        ammo     = gSaveContext.perm.inv.quantities[SLOT_MAGIC_BEAN];
        capacity = 20;
    }
    else if (item == ITEM_POWDER_KEG) {
        ammo     = gSaveContext.perm.inv.quantities[SLOT_POWDER_KEG];
        capacity = 1;
    }
    else if (item == ITEM_PICTOGRAPH) {
        ammo     = gSaveContext.perm.inv.quantities[SLOT_PICTOGRAPH];
        capacity = 1;
    }
        
    if (ammo >= 0) {
        if (ammo == capacity)
            gDPSetPrimColor(db->p++, 0, 0, 120, 255, 0, alpha);
        else if (ammo == 0)
            gDPSetPrimColor(db->p++, 0, 0, 100, 100, 100, alpha);
        else gDPSetPrimColor(db->p++, 0, 0, 255, 255, 255, alpha);
        
        if (ammo >= 10) {
            Sprite_Load(db, &gParameterAmmoDigit, ammo / 10, 1);
            Sprite_Draw(db, &gParameterAmmoDigit, 0, x, y, size, size);
        }
        Sprite_Load(db, &gParameterAmmoDigit, ammo % 10, 1);
        Sprite_Draw(db, &gParameterAmmoDigit, 0, x + size + spacing, y , size, size);
    }
}

/**
 * Hook function used to determine whether or not to skip the transformation cutscene based on input.
 *
 * Allows D-Pad input to also skip the cutscene if the D-Pad is enabled.
 **/
u16 Dpad_SkipTransformationCheck(ActorPlayer* player, GlobalContext* ctxt, u16 cur) {
    InputPad pad;
    pad.value = 0;

    // Set flags for original buttons: A, B, C buttons (0xC00F)
    pad.a = pad.b = pad.cd = pad.cl = pad.cr = pad.cu = 1;

    if (Dpad_IsEnabled()) {
        // Set flags for D-Pad (0xCF0F)
        pad.dd = pad.dl = pad.dr = pad.du = 1;
    }

    return cur & pad.value;
}

u8 Dpad_MaskIdToItemId(s32 maskIdMinusOne) {
    u8 itemId = z2_Player_MaskIdToItemId(maskIdMinusOne);

    for (u8 i = 0; i < ARRAY_COUNT(DPAD_CONFIG.primary.values); i++) {
        if (DPAD_CONFIG.primary.values[i] == itemId) {
            return ITEM_NONE;
        }
    }

    return itemId;
}
