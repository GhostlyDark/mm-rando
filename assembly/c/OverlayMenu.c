#include "OverlayMenu.h"

extern uint8_t CFG_SWAP_ENABLED;
extern uint8_t CFG_OVERLAY_DISPLAY_ENABLED;

// Whether or not the overlay menu is enabled.
bool overlayActive = false;

// Clock Town stray fairy icon image buffer, written to by randomizer
u8 TOWN_FAIRY_BYTES[0xC00] = { 0 };

static Sprite gSkulltulaIcon       = { NULL,             24, 24, 1, G_IM_FMT_RGBA, G_IM_SIZ_32b, 4 }; // Gold Skulltula HUD icon
static Sprite gSpriteFairy         = { NULL,             32, 24, 4, G_IM_FMT_RGBA, G_IM_SIZ_32b, 4 }; // Dungeon stray fairy icons
static Sprite gEquippedItemOutline = { NULL,             32, 32, 1, G_IM_FMT_IA,   G_IM_SIZ_8b,  1 }; // Equipped item outline icon
static Sprite gTownFairyIcon       = { TOWN_FAIRY_BYTES, 32, 24, 1, G_IM_FMT_RGBA, G_IM_SIZ_32b, 4 }; // Clock Town stray fairy icon

static void LoadAndDrawStrayFairyIconFlipped(DispBuf* db, Sprite* sprite, int tileIndex, int left, int top, int width, int height) {
    const int widthFactor = (1<<10) * sprite->tileW / width;
    const int heightFactor = (1<<10) * sprite->tileH / height;
    gDPLoadTextureBlock(db->p++,
            sprite->buf + (tileIndex * Sprite_GetBytesPerTile(sprite)),
            sprite->imFmt, sprite->imSiz,
            sprite->tileW, sprite->tileH,
            0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD
    );
    gSPTextureRectangle(db->p++,
            left<<2, top<<2,
            (left + width)<<2, (top + height)<<2,
            G_TX_RENDERTILE,
            0, 0,
            widthFactor, heightFactor);
}

struct DungeonEntry {
    u8 index;
    u8 remains;
    s8 item1;
    s8 item2;
    u8 isDungeon;
    u8 hasFairies;
    u8 hasTokens;
    char name[18];
};

static struct DungeonEntry gDungeons[9] = {
    { 0, 0,                     ITEM_OCARINA,       ITEM_GREAT_FAIRY_MASK, 0, 1, 0, "Clock Town"         },
    { 0, ITEM_ODOLWA_REMAINS,   ITEM_BOW,           -1,                    1, 1, 0, "Woodfall Temple"    },
    { 1, ITEM_GOHT_REMAINS,     ITEM_FIRE_ARROW,    -1,                    1, 1, 0, "Snowhead Temple"    },
    { 2, ITEM_GYORG_REMAINS,    ITEM_ICE_ARROW,     -1,                    1, 1, 0, "Great Bay Temple"   },
    { 3, ITEM_TWINMOLD_REMAINS, ITEM_LIGHT_ARROW,   ITEM_GIANT_MASK,       1, 1, 0, "Stone Tower Temple" },
    { 0, 0,                     ITEM_HOOKSHOT,      -1,                    0, 0, 0, "Pirates' Fortress"  },
    { 0, 0,                     ITEM_MIRROR_SHIELD, -1,                    0, 0, 0, "Beneath the Well"   },
    { 0, 0,                     ITEM_MASK_OF_TRUTH, -1,                    0, 0, 1, "Spider House 1"     },
    { 0, 0,                     ITEM_GIANT_WALLET,  -1,                    0, 0, 2, "Spider House 2"     },
};

static u8 gDungeonCount = 9;

/**
 * Get text for a specific amount, with a limited digit count (1 or 2).
 **/
static void GetCountText(int amount, char* c, int digits) {
    if (digits == 1) {
        // Get text for 1 digit, max of 9.
        if (amount > 9) {
            amount = 9;
        }
        c[0] = '0' + amount;
        c[1] = '\0';
    } else if (digits == 2) {
        // Get text for 2 digits, max of 99.
        if (amount >= 10) {
            if (amount > 99) {
                amount = 99;
            }
            c[0] = '0' + (amount / 10);
        }
        c[1] = '0' + amount % 10;
        c[2] = '\0';
    }
}

/**
 * Whether or not the player has boss remains for a specific dungeon index.
 **/
static bool HasRemains(u8 index) {
    return (gSaveContext.perm.inv.questStatus.value & (1 << index)) != 0;
}

/**
 * Whether or not the player has boss remains for a specific dungeon index.
 **/
static s8 HasItem(s8 item) {
    if (item < 0)
        return -1;
    
    if (item == ITEM_GIANT_WALLET && gSaveContext.perm.weekEventReg.receivedOceansideWallet)
        return 1;
    else if (item == ITEM_MIRROR_SHIELD && (gSaveContext.perm.unk4C.equipment.shield == 2 || (CFG_SWAP_ENABLED && HAVE_MIRROR_SHIELD) ) )
        return 1;
    
    for (u8 i=0; i<24; i++)
        if (gSaveContext.perm.inv.items[i] == item || gSaveContext.perm.inv.masks[i] == item)
            return 1;
    
    return 0;
}

/**
 * Try to draw overlay menu.
 **/
void OverlayMenu_Draw(GlobalContext* ctxt) {
    if (!CFG_OVERLAY_DISPLAY_ENABLED)
        return;
    
    if (ctxt->pauseCtx.state == 6 && ctxt->pauseCtx.switchingScreen == 0 && ctxt->pauseCtx.screenIndex == 1 && ctxt->state.input[0].pressEdge.buttons.l) {
        overlayActive = overlayActive ? false : true;
        overlayActive ? z2_PlaySfx(0x4813) : z2_PlaySfx(0x4814);
    }
    else if (ctxt->pauseCtx.state != 6 || ctxt->pauseCtx.switchingScreen != 0 || ctxt->pauseCtx.screenIndex != 1)
        overlayActive = false;
    
    if (!overlayActive)
        return;
    
    DispBuf* db = &ctxt->state.gfxCtx->overlay;
    db->p = db->buf;

    // Call setup display list
    gSPDisplayList(db->p++, &gSetupDb);

    // General variables
    u8 iconSize  = 10;
    u8 padding   = 1;
    u8 rows      = gDungeonCount + 1;
    u8 nameWidth = 95;
    u8 i;
    
    // Set font size
    Text_SetSize(4, 7);

    // Background rectangle
    u16 bgWidth  =    (8 * iconSize) + 130;
    u16 bgHeight =  rows * iconSize;
    u16 bgLeft   = (SCREEN_WIDTH  - bgWidth)  / 2;
    u16 bgTop    = (SCREEN_HEIGHT - bgHeight) / 2 + 10;

    // Update sprite pointers
    gSpriteIcon.buf          = ctxt->pauseCtx.iconItemStatic;
    gSpriteIcon24.buf        = ctxt->pauseCtx.iconItem24;
    gSkulltulaIcon.buf       = (u8*)ctxt->interfaceCtx.parameterStatic + 0x31E0;
    gSpriteFairy.buf         = (u8*)ctxt->interfaceCtx.parameterStatic + 0x8998;
    gEquippedItemOutline.buf = (u8*)ctxt->interfaceCtx.parameterStatic + 0x1360;

    // Draw background
    gDPSetCombineMode(db->p++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetPrimColor(db->p++, 0, 0, 0x00, 0x00, 0x00, 0xD0);
    gSPTextureRectangle(db->p++, bgLeft << 2, bgTop << 2, (bgLeft + bgWidth) << 2, (bgTop + bgHeight) << 2, 0, 0, 0, 1 << 10, 1 << 10);
    gDPPipeSync(db->p++);

    // Draw legend panel background
    u16 legendLeft   = bgLeft +  iconSize + nameWidth + (padding * 3);
    u16 legendTop    = bgTop  - (iconSize + (padding * 3));
    u16 legendWidth  = (iconSize * 5) + (padding * 5);
    u16 legendHeight =  iconSize      + (padding * 2);
    gDPSetPrimColor(db->p++, 0, 0, 0x00, 0x00, 0x00, 0xA0);
    gSPTextureRectangle(db->p++, legendLeft << 2, legendTop << 2, (legendLeft + legendWidth) << 2, (legendTop + legendHeight) << 2, 0, 0, 0, 1 << 10, 1 << 10);
    gDPPipeSync(db->p++);
    gDPSetCombineMode(db->p++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    gDPSetPrimColor(db->p++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);

    // Draw legend panel icons: Hero's Bow, Small Key, Boss Key, Map, Compass
    s8 legendIcons[5] = { 10, 6, 8, 7, 1 };
    for (i=0; i<sizeof(legendIcons); i++) {
        u8  index = legendIcons[i];
        u16 lleft = legendLeft + ((iconSize + padding) * i);
        u16 top   = legendTop + padding;
        Sprite sprite = (i == sizeof(legendIcons) - 1) ? gSpriteIcon : gSpriteIcon24;
        Sprite_Load(db, &sprite, index, 1);
        Sprite_Draw(db, &sprite, 0, lleft, top, iconSize, iconSize);
    }
    
    // Draw scene info
    for (i=0; i<gDungeonCount; i++) {
        struct DungeonEntry* d = &gDungeons[i];
        
        // Initialize variables
        u16 left      = bgLeft + padding;
        u16 startTop  = bgTop  + padding;
        u16 top       = startTop + ((iconSize + padding) * i);
        u8  total     = 0; // Get total count and maximum count for stray fairies or skulltula tokens
        u8  maximum   = 0;
        s8  itemCheck = 0;
        
        // Draw remains
        if (d->isDungeon && HasRemains(d->index)) {
            Sprite_Load(db, &gSpriteIcon, d->remains, 1);
            Sprite_Draw(db, &gSpriteIcon, 0, left, top, iconSize, iconSize);
        }
        
        // Draw names
        left += iconSize + padding;
        Text_Print(d->name, left, top + 1);
        
        // Draw dungeon item. small keys, boss keys, maps and compasses
        left += nameWidth + padding;
        if (d->isDungeon) {
            u8 keys = gSaveContext.perm.inv.dungeonKeys[d->index]; // Get key count for dungeon
            char count[2] = "0";
            GetCountText(keys, count, 1); // Get key count as text
            Text_Print(count, left + 4, top + 1); // Draw key count as text
            
            left += iconSize + padding;
            if (gSaveContext.perm.inv.dungeonItems[d->index].bossKey) { // Boss Key
                Sprite_Load(db, &gSpriteIcon24, 6, 1);
                Sprite_Draw(db, &gSpriteIcon24, 0, left, top, iconSize, iconSize);
            }
            
            left += iconSize + padding;
            if (gSaveContext.perm.inv.dungeonItems[d->index].map) { // Map
                Sprite_Load(db, &gSpriteIcon24, 8, 1);
                Sprite_Draw(db, &gSpriteIcon24, 0, left, top, iconSize, iconSize);
            }
            
            left += iconSize + padding;
            if (gSaveContext.perm.inv.dungeonItems[d->index].compass) { // Compass
                Sprite_Load(db, &gSpriteIcon24, 7, 1);
                Sprite_Draw(db, &gSpriteIcon24, 0, left, top, iconSize, iconSize);
            }
        }
        else left += (iconSize + padding) * 3;
        
        // Draw progression item 1 icon
        left += iconSize + padding;
        itemCheck = HasItem(d->item1);
        if (itemCheck >= 0) {
            Sprite sprite = itemCheck ? gSpriteIcon : gEquippedItemOutline;
            Sprite_Load(db, &sprite, itemCheck ? d->item1 : 0, 1);
            Sprite_Draw(db, &sprite, 0, left, top, iconSize, iconSize);
        }
            
        // Draw progression item 2 icon
        left += iconSize + padding;
        itemCheck = HasItem(d->item2);
        if (itemCheck >= 0) {
            Sprite sprite = itemCheck == 1 ? gSpriteIcon : gEquippedItemOutline;
            Sprite_Load(db, &sprite, itemCheck ? d->item2 : 0, 1);
            Sprite_Draw(db, &sprite, 0, left, top, iconSize, iconSize);
        }
        
        // Draw stray fairy, skulltula token icons
        left += iconSize + padding;
        if (d->hasFairies) { // Draw dungeon or Clock Town fairy icon
            if (d->isDungeon) {
                LoadAndDrawStrayFairyIconFlipped(db, &gSpriteFairy, d->index, left, top - 2, 20, 15);
                total   = gSaveContext.perm.inv.strayFairies[d->index];
                maximum = 15;
            }
            else {
                Sprite_Load(db, &gTownFairyIcon, 0, 1);
                Sprite_Draw(db, &gTownFairyIcon, 0, left, top - 2, 20, 15);
                total   = gSaveContext.perm.weekEventReg.hasTownFairy ? 1 : 0;
                maximum = 1;
            }
        }
        else if (d->hasTokens) { // Draw skulltula token icon
            Sprite_Load(db, &gSkulltulaIcon, 0, 1);
            Sprite_DrawCropped(db, &gSkulltulaIcon, 0, left + 2, top - 2, 16, 12, CROP(0, 6));
            total   = gSaveContext.perm.skullTokens[d->hasTokens - 1];
            maximum = 30;
        }
        
        // Draw stray fairy count, skulltula token count
        left += 20 + padding;
        if (d->hasFairies || d->hasTokens) { // Display count as text
            char count[3] = " 0";
            GetCountText(total, count, 2); // Get count as text
            if (total >= maximum) { // Draw fairy/token count as text
                ColorRGBA8 color = { 0x78, 0xFF, 0x00, 0xFF }; // Use green text if at maximum
                Text_PrintWithColor(count, left, top + 1, color);
            }
            else Text_Print(count, left, top + 1);
        }
    }

    // Flush text and finish
    Text_Flush(db);
    gDPFullSync(db->p++);
    gSPEndDisplayList(db->p++);
}
