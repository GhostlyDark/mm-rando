#ifndef _Z64EXTENDED_H_
#define _Z64EXTENDED_H_

#include <z64.h>

#define gStaticContext				(*(StaticContext*)		StaticContextAddr)

#define r_minimap_disabled			(*(uint8_t*)			0x80383023)

/* Minimap SRAM Locations (801F0514) */
#define minimap_clock_town			(gSaveContext.perm.minimapBitfield[0] & (1 << 4) )
#define minimap_milk_road			(gSaveContext.perm.minimapBitfield[1] & (1 << 1) )
#define minimap_woodfall			(gSaveContext.perm.minimapBitfield[3] & (1 << 0) )
#define minimap_snowhead			(gSaveContext.perm.minimapBitfield[5] & (1 << 3) )
#define minimap_great_bay			(gSaveContext.perm.minimapBitfield[1] & (1 << 4) )
#define minimap_stone_tower			(gSaveContext.perm.minimapBitfield[0] & (1 << 5) )

/* Extra SRAM */
#define active_shield				(*(uint8_t*)			0x803FFEF4)
#define HAVE_EXTRA_SRAM				(gSaveContext.perm.inv.quantities[0])
#define TALKED_GUARD				(HAVE_EXTRA_SRAM & (1 << 0) )
#define HAVE_KOKIRI_SWORD			(HAVE_EXTRA_SRAM & (1 << 1) )
#define HAVE_RAZOR_SWORD			(HAVE_EXTRA_SRAM & (1 << 2) )
#define HAVE_GILDED_SWORD			(HAVE_EXTRA_SRAM & (1 << 3) )
#define HAVE_HERO_SHIELD			(HAVE_EXTRA_SRAM & (1 << 4) )
#define HAVE_MIRROR_SHIELD			(HAVE_EXTRA_SRAM & (1 << 5) )

#endif