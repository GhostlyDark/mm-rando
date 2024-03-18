#ifndef _Z64EXTENDED_H_
#define _Z64EXTENDED_H_

#include <z64.h>

enum ocarinaItemValue {
    ITEM_DEKU_PIPES  = 0x26, // ITEM_EEL_BOTTLE
    ITEM_GORON_DRUMS = 0x1C, // ITEM_BLUE_FIRE
    ITEM_ZORA_GUITAR = 0x27, // ITEM_EMPTY_BOTTLE_2
};

typedef union PressedButtons {
    struct {
        uint8_t a;
        uint8_t b;
        uint8_t s;
        uint8_t l;
        uint8_t r;
        uint8_t z;
        uint8_t cu;
        uint8_t cr;
        uint8_t cd;
        uint8_t cl;
        uint8_t du;
        uint8_t dr;
        uint8_t dd;
        uint8_t dl;
    };
} PressedButtons;

typedef void (*z2_UpdateButtonIcon_proc)    (GlobalContext* ctxt, int button);
#define z2_UpdateButtonIcon                 ((z2_UpdateButtonIcon_proc)   0x80112B40)

typedef void (*z2_SetItemButton_proc)       (GlobalContext* ctxt, int item, int button);
#define z2_SetItemButton                    ((z2_SetItemButton_proc)      0x80114FD0)

#define gStaticContext                      (*(StaticContext*)            StaticContextAddr)
#define IS_PLAYABLE                         (gSaveContext.extra.titleSetupIndex == 0 && ctxt->playable_state != 0xFF08 && ctxt->playable_state != 0)
#define HAS_OCARINA                         (gSaveContext.perm.inv.items[0] == ITEM_OCARINA || gSaveContext.perm.inv.items[0] == ITEM_DEKU_PIPES || gSaveContext.perm.inv.items[0] == ITEM_GORON_DRUMS || gSaveContext.perm.inv.items[0] == ITEM_ZORA_GUITAR)
#define L_PAD_ENABLED                       (CFG_DUAL_DPAD_ENABLED ||CFG_SWAP_ENABLED || CFG_FLOW_OF_TIME_ENABLED || CFG_INSTANT_ELEGY_ENABLED || CFG_FPS_ENABLED)
#define IS_ABILITY_ACTIVATED(boss)          (boss == 0 || (gSaveContext.perm.inv.questStatus.odolwasRemains && boss == 1) || (gSaveContext.perm.inv.questStatus.gohtsRemains && boss == 2) || (gSaveContext.perm.inv.questStatus.gyorgsRemains && boss == 3) || (gSaveContext.perm.inv.questStatus.twinmoldsRemains && boss == 4))

#define r_minimap_disabled                  (*(uint8_t*)                  0x80383023)
#define clock_town_guard                    (*(uint8_t*)                  0x801F0574)

#define A_BUTTON_X                          (*(uint16_t*)                 0x80118DA2)
#define A_BUTTON_Y                          (*(uint16_t*)                 0x80118DA6)

/* Minimap SRAM Locations (801F0514) */
#define minimap_clock_town                  (gSaveContext.perm.minimapBitfield[0] & (1 << 4) )
#define minimap_milk_road                   (gSaveContext.perm.minimapBitfield[1] & (1 << 1) )
#define minimap_woodfall                    (gSaveContext.perm.minimapBitfield[3] & (1 << 0) )
#define minimap_snowhead                    (gSaveContext.perm.minimapBitfield[5] & (1 << 3) )
#define minimap_great_bay                   (gSaveContext.perm.minimapBitfield[1] & (1 << 4) )
#define minimap_stone_tower                 (gSaveContext.perm.minimapBitfield[0] & (1 << 5) )

/* Extra SRAM */
#define HAVE_EXTRA_SRAM                     (*(uint8_t*)                  0x801EF677) // (gSaveContext.perm.inv.quantities[0])
#define SAVE_DPAD                           (HAVE_EXTRA_SRAM & (1 << 1) )
#define HAVE_RAZOR_SWORD                    (HAVE_EXTRA_SRAM & (1 << 2) )
#define HAVE_GILDED_SWORD                   (HAVE_EXTRA_SRAM & (1 << 3) )
#define LOST_HERO_SHIELD                    (HAVE_EXTRA_SRAM & (1 << 4) )
#define HAVE_MIRROR_SHIELD                  (HAVE_EXTRA_SRAM & (1 << 5) )
#define HAVE_TALKED_GUARD                   (HAVE_EXTRA_SRAM & (1 << 6) )

/* D-PAD */
#define DPAD_SET1_UP                        (gSaveContext.perm.inv.quantities[0x0])
#define DPAD_SET1_RIGHT                     (gSaveContext.perm.inv.quantities[0x2])
#define DPAD_SET1_DOWN                      (gSaveContext.perm.inv.quantities[0x3])
#define DPAD_SET1_LEFT                      (gSaveContext.perm.inv.quantities[0x4])
#define DPAD_SET2_UP                        (gSaveContext.perm.inv.quantities[0x5])
#define DPAD_SET2_RIGHT                     (gSaveContext.perm.inv.quantities[0xB])
#define DPAD_SET2_DOWN                      (gSaveContext.perm.inv.quantities[0xE])
#define DPAD_SET2_LEFT                      (gSaveContext.perm.inv.quantities[0xF])

/* Smithy */
#define smithy                              (*(uint8_t*)                  0x801EFC4F)
#define REFORGING_KOKIRI_SWORD              ( (smithy & (1 << 0)) && (smithy & (1 << 1)) && !(smithy & (1 << 2)) )
#define REFORGING_RAZOR_SWORD               ( (smithy & (1 << 0)) && (smithy & (1 << 1)) &&  (smithy & (1 << 2)) )

/* Alpha */
#define magicRupeesAlpha                    (*(uint16_t*)                 0x80415D7A) // Rev0: 0x803FD77A, Buffer 4: 0x803F5D7A
#define minimapAlpha                        (*(uint16_t*)                 0x80415D7C) // Rev0: 0x803FD77C, Buffer 4: 0x803F5D7C

#endif