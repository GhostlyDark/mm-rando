#ifndef FPS_H
#define FPS_H

#include <z64.h>

void Handle_L_Button(GlobalContext* ctxt);
void Toggle_Minimap(GlobalContext* ctxt);
void Handle_FPS(GlobalContext* ctxt);

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
	SCENE_STONE_TOWER					= 0x0058,
	SCENE_STONE_TOWER_INV				= 0x0059,
	
	SCENE_WOODFALL_TEMPLE				= 0x001B,
	SCENE_SNOWHEAD_TEMPLE				= 0x0021,
	SCENE_GREAT_BAY_TEMPLE				= 0x0049,
	SCENE_STONE_TOWER_TEMPLE			= 0x0016,
	SCENE_STONE_TOWER_TEMPLE_INV		= 0x0018,
} scene_id;

#define gStaticContext				(*(StaticContext*)		StaticContextAddr)

#define time_modifier				(*(uint32_t*)			0x801EF684)
#define deku_hovering				(*(uint16_t*)			0x8040081C)
#define playable_state				(*(uint16_t*)			0x803FFE66)
#define use_hookshot				(*(uint16_t*)			0x803FFE00)
#define jump_state					(*(uint16_t*)			0x80400897)
#define deku_stick_timer			(*(uint16_t*)			0x804008D8)
#define jump_height					(*(uint16_t*)			0x803FFE20)
#define jump_gravity				(*(uint16_t*)			0x803FFE24)
#define link_action					(*(uint16_t*)			0x80400004)
#define text_state					(*(uint8_t*)			0x803FFDB6)
#define link_animation_speed		(*(uint8_t*)			0x803825E1)
#define opening_chest				(*(uint8_t*)			0x803FFF68)

#define var_8040000C				(*(uint32_t*)			0x8040000C)
#define var_801160AE				(*(uint16_t*)			0x801160AE)
#define var_80116702				(*(uint16_t*)			0x80116702)
#define var_803FFE64				(*(uint16_t*)			0x803FFE64)
#define var_801D7B44				(*(uint8_t*)			0x801D7B44)

#define r_minimap_disabled			(*(uint8_t*)			0x80383023)

/* Minimap SRAM Locations (801F0514) */
#define minimap_clock_town			(gSaveContext.perm.minimapBitfield[0] & (1 << 4) )
#define minimap_milk_road			(gSaveContext.perm.minimapBitfield[1] & (1 << 1) )
#define minimap_woodfall			(gSaveContext.perm.minimapBitfield[3] & (1 << 0) )
#define minimap_snowhead			(gSaveContext.perm.minimapBitfield[5] & (1 << 3) )
#define minimap_great_bay			(gSaveContext.perm.minimapBitfield[1] & (1 << 4) )
#define minimap_stone_tower			(gSaveContext.perm.minimapBitfield[0] & (1 << 5) )


#endif
