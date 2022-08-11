#ifndef _Z64EXTENDED_H_
#define _Z64EXTENDED_H_

#include <z64.h>

typedef enum { // 803E6BC4
	SCENE_SOUTH_CLOCK_TOWN				= 0x006F,
	SCENE_EAST_CLOCK_TOWN				= 0x006C,
	SCENE_WEST_CLOCK_TOWN				= 0x006D,
	SCENE_NORTH_CLOCK_TOWN				= 0x006E,
	SCENE_LAUNDRY_POOL					= 0x0070,
	SCENE_TERMINA_FIELD					= 0x002D,
	
	SCENE_MILK_ROAD						= 0x0022,
	SCENE_GORMAN_TRACK					= 0x006A,
	SCENE_ROMANI_RANCH					= 0x0035,
	SCENE_DOGGY_RACETRACK				= 0x0041,
	SCENE_CUCCO_SHACK					= 0x0042,
	
	SCENE_ROAD_TO_SOUTHERN_SWAMP		= 0x0040,
	SCENE_SOUTHERN_SWAMP				= 0x0045,
	SCENE_SOUTHERN_SWAMP_CLEAR			= 0x0001,
	SCENE_DEKU_PALACE_EXT				= 0x002B,
	SCENE_DEKU_PALACE_INT				= 0x003E,
	SCENE_WOODFALL						= 0x0046,
	SCENE_SWAMP_SPIDER_HOUSE			= 0x0027,
	
	SCENE_ROAD_TO_MOUNTAIN_VILLAGE		= 0x001C,
	SCENE_MOUNTAIN_VILLAGE				= 0x0050,
	SCENE_MOUNTAIN_VILLAGE_CLEAR		= 0x005A,
	SCENE_ROAD_TO_SNOWHEAD				= 0x005B,
	SCENE_SNOWHEAD						= 0x005C,
	SCENE_ROAD_TO_GORON_VILLAGE			= 0x005D,
	SCENE_ROAD_TO_GORON_VILLAGE_CLEAR	= 0x005E,
	SCENE_GORON_RACETRACK				= 0x006B,
	SCENE_GORON_VILLAGE					= 0x004D,
	SCENE_GORON_VILLAGE_CLEAR			= 0x0048,
	SCENE_GORON_SHRINE					= 0x0032,
	
	SCENE_GREAT_BAY_COAST_1				= 0x0037,
	SCENE_PIRATES_FORTRESS_EXT			= 0x003B,
	SCENE_PIRATES_FORTRESS_INT			= 0x0014,
	SCENE_GREAT_BAY_COAST_2				= 0x0038,
	SCENE_WATERFALL_RAPIDS				= 0x004A,
	SCENE_PINACLE_ROCK					= 0x0025,
	SCENE_ZORA_HALL						= 0x0033,
	
	SCENE_ROAD_TO_IKANA_CANYON			= 0x0053,
	SCENE_GRAVEYARD						= 0x0043,
	SCENE_IKANA_CANYON					= 0x0013,
	SCENE_BOTTOM_OF_THE_WELL			= 0x004B,
	SCENE_ANCIENT_CASTLE_OF_IKANA		= 0x001D,
	SCENE_ANCIENT_CASTLE_OF_IKANA_BOSS	= 0x0056,
	SCENE_STONE_TOWER					= 0x0058,
	SCENE_STONE_TOWER_INV				= 0x0059,
	SCENE_GHOST_HUT						= 0x0051,
	SCENE_SECRET_SHRINE					= 0x0060,
	
	SCENE_WOODFALL_TEMPLE				= 0x001B,
	SCENE_SNOWHEAD_TEMPLE				= 0x0021,
	SCENE_GREAT_BAY_TEMPLE				= 0x0049,
	SCENE_STONE_TOWER_TEMPLE			= 0x0016,
	SCENE_STONE_TOWER_TEMPLE_INV		= 0x0018,
	SCENE_STONE_TOWER_TEMPLE_BOSS		= 0x0036,
} scene_id;

enum ocarinaItemValue {
	ITEM_DEKU_PIPES		= 0x0B,
	ITEM_GORON_DRUMS	= 0x1C,
	ITEM_ZORA_GUITAR	= 0x26,
};

typedef union PressedButtons {
    struct {
		uint8_t r;
		uint8_t z;
		uint8_t du;
		uint8_t dr;
		uint8_t dd;
		uint8_t dl;
    };
} PressedButtons;

typedef void (*z2_UpdateButtonIcon_proc)	(GlobalContext* ctxt, int button);
#define z2_UpdateButtonIcon					((z2_UpdateButtonIcon_proc)		0x80112B40)

typedef void (*z2_SetItemButton_proc)		(GlobalContext* ctxt, int item, int button);
#define z2_SetItemButton					((z2_SetItemButton_proc)		0x80114FD0)

#define gStaticContext						(*(StaticContext*)				StaticContextAddr)

#define r_minimap_disabled					(*(uint8_t*)					0x80383023)
#define clock_town_guard					(*(uint8_t*)					0x801F0574)

/* Minimap SRAM Locations (801F0514) */
#define minimap_clock_town					(gSaveContext.perm.minimapBitfield[0] & (1 << 4) )
#define minimap_milk_road					(gSaveContext.perm.minimapBitfield[1] & (1 << 1) )
#define minimap_woodfall					(gSaveContext.perm.minimapBitfield[3] & (1 << 0) )
#define minimap_snowhead					(gSaveContext.perm.minimapBitfield[5] & (1 << 3) )
#define minimap_great_bay					(gSaveContext.perm.minimapBitfield[1] & (1 << 4) )
#define minimap_stone_tower					(gSaveContext.perm.minimapBitfield[0] & (1 << 5) )

/* Extra SRAM */
#define active_shield						(*(uint8_t*)					0x803FFEF4)
#define active_shield_ws					(*(uint8_t*)					0x804184F4)
#define HAVE_EXTRA_SRAM						(*(uint8_t*)					0x801EF677)	// (gSaveContext.perm.inv.quantities[0])
#define SAVE_DPAD							(HAVE_EXTRA_SRAM & (1 << 1) )
#define HAVE_RAZOR_SWORD					(HAVE_EXTRA_SRAM & (1 << 2) )
#define HAVE_GILDED_SWORD					(HAVE_EXTRA_SRAM & (1 << 3) )
#define LOST_HERO_SHIELD					(HAVE_EXTRA_SRAM & (1 << 4) )
#define HAVE_MIRROR_SHIELD					(HAVE_EXTRA_SRAM & (1 << 5) )
#define HAVE_TALKED_GUARD					(HAVE_EXTRA_SRAM & (1 << 6) )

/* D-PAD */
#define DPAD_SET1_UP						(gSaveContext.perm.inv.quantities[0x0])
#define DPAD_SET1_RIGHT						(gSaveContext.perm.inv.quantities[0x2])
#define DPAD_SET1_DOWN						(gSaveContext.perm.inv.quantities[0x3])
#define DPAD_SET1_LEFT						(gSaveContext.perm.inv.quantities[0x4])
#define DPAD_SET2_UP						(gSaveContext.perm.inv.quantities[0x5])
#define DPAD_SET2_RIGHT						(gSaveContext.perm.inv.quantities[0xB])
#define DPAD_SET2_DOWN						(gSaveContext.perm.inv.quantities[0xE])
#define DPAD_SET2_LEFT						(gSaveContext.perm.inv.quantities[0xF])

/* Smithy */
#define smithy								(*(uint8_t*)					0x801EFC4F)
#define REFORGING_KOKIRI_SWORD				( (smithy & (1 << 0)) && (smithy & (1 << 1)) && !(smithy & (1 << 2)) )
#define REFORGING_RAZOR_SWORD				( (smithy & (1 << 0)) && (smithy & (1 << 1)) &&  (smithy & (1 << 2)) )

#endif