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

CFG_SKIP_GUARD_ENABLED:
.byte 0x00
CFG_SWAP_ENABLED:
.byte 0x00
CFG_FPS_ENABLED:
.byte 0x00

.align 4