;==================================================================================================
; Settings and tables which the front-end may write
;==================================================================================================

; This is used to determine if and how the options can be patched
; It this moves then the version will no longer be valid, so it is important that this does not move
OPTIONS_CONTEXT:

; CONF string
.byte 0x43
.byte 0x4F
.byte 0x4E
.byte 0x46

CFG_WS_ENABLED:
.byte 0x00
CFG_DUAL_DPAD_ENABLED:
.byte 0x00
CFG_OCARINA_ICONS_ENABLED:
.byte 0x00
CFG_HIDE_HUD_ENABLED:
.byte 0x00
CFG_SKIP_GUARD_ENABLED:
.byte 0x00
CFG_SWAP_ENABLED:
.byte 0x00
CFG_UNEQUIP_ENABLED:
.byte 0x00
CFG_B_BUTTON_ITEM_ENABLED:
.byte 0x00
CFG_FLOW_OF_TIME_ENABLED:
.byte 0x00
CFG_INSTANT_ELEGY_ENABLED:
.byte 0x00
CFG_INFINITE_HEALTH:
.byte 0x00
CFG_INFINITE_MAGIC:
.byte 0x00
CFG_INFINITE_AMMO:
.byte 0x00
CFG_INFINITE_RUPEES:
.byte 0x00
CFG_FPS_ENABLED:
.byte 0x00
CFG_INVENTORY_EDITOR_ENABLED:
.byte 0x00

.align 4