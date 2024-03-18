#include "Extra.h"
#include "Reloc.h"
#include "GiantMask.h"

extern uint8_t CFG_WS_ENABLED;
extern uint8_t CFG_DUAL_DPAD_ENABLED;
extern uint8_t CFG_SWAP_ENABLED;
extern uint8_t CFG_B_BUTTON_ITEM_ENABLED;
extern uint8_t CFG_FPS_ENABLED;
extern uint8_t CFG_HIDE_HUD_ENABLED;
extern uint8_t CFG_OCARINA_ICONS_ENABLED;
extern uint8_t CFG_FLOW_OF_TIME_ENABLED;
extern uint8_t CFG_INSTANT_ELEGY_ENABLED;
extern uint8_t CFG_INVENTORY_EDITOR_ENABLED;
extern uint8_t CFG_INFINITE_HEALTH;
extern uint8_t CFG_INFINITE_MAGIC;
extern uint8_t CFG_INFINITE_AMMO;
extern uint8_t CFG_INFINITE_RUPEES;
extern uint8_t CFG_UNEQUIP_ENABLED;
extern uint8_t CFG_RUPEE_DRAIN;

uint16_t deku_stick_timer_switch = 0;
uint16_t last_time               = 0;
uint16_t started_timer           = 0;
uint16_t elegy_timer             = 0;
uint16_t time_tracker[3]         = { 0, 0, 0 };

uint8_t rupee_drain_frames       = 0;
uint8_t rupee_drain_secs         = 0;

uint8_t dpad_alt                 = 0;
uint8_t fps_switch               = 1;
uint8_t hud_hide                 = 0;
uint8_t hud_counter              = 0;

uint8_t block                    = 0;
uint8_t used_block               = 0;
PressedButtons pressed;

void Handle_Extra_Functions(GlobalContext* ctxt) {
    Handle_FPS(ctxt);
    Handle_L_Button_Ingame(ctxt);
    Handle_L_Button_Paused(ctxt);
    Handle_Infinite();
    Handle_Items_On_Dpad(ctxt);

    if (CFG_B_BUTTON_ITEM_ENABLED && gSaveContext.perm.unk4C.formButtonItems[0].buttons[0] == ITEM_FAIRY_SWORD && gSaveContext.perm.inv.items[SLOT_FAIRY_SWORD] != ITEM_FAIRY_SWORD) {
        gSaveContext.perm.unk4C.formButtonItems[0].buttons[0] = gSaveContext.perm.unk4C.formButtonSlots[0].buttons[0] = ITEM_NONE;
        z2_UpdateButtonIcon(ctxt, 0);
    }

    if (ctxt->pauseCtx.state == 6 && IS_PLAYABLE && ctxt->pauseCtx.debugMenu == 0) {
        Handle_Unequipping(ctxt);
        Set_B_Button(ctxt);
    }
    
    if (CFG_INSTANT_ELEGY_ENABLED && !elegy_anim_state && ctxt->link_anim_2 == 0x67)
        ctxt->link_anim_1 = 5;
    
    if (ctxt->pauseCtx.state == 6 && ctxt->pauseCtx.debugMenu == 0)
        ctxt->state.input[0].current.buttons.dr = ctxt->state.input[0].current.buttons.dl = 0;
}

void Handle_Items_On_Dpad(GlobalContext* ctxt) {
    if (DPAD_CONFIG.state == DPAD_STATE_DISABLED)
        return;
    
    Handle_Item_Slot_On_Dpad(ITEM_MOON_TEAR,   ITEM_OCEAN_DEED, gSaveContext.perm.inv.items[SLOT_QUEST1]);
    Handle_Item_Slot_On_Dpad(ITEM_MAMA_LETTER, ITEM_PENDANT,    gSaveContext.perm.inv.items[SLOT_QUEST3]);
}

void Handle_Item_Slot_On_Dpad(uint8_t button_min, uint8_t button_max, uint8_t slot) {
    if (slot == ITEM_NONE)
        return;
    
    if (DPAD_SET1_UP >= button_min && DPAD_SET1_UP <= button_max && slot != DPAD_SET1_UP)
        DPAD_SET1_UP = slot;
    if (DPAD_SET1_RIGHT >= button_min && DPAD_SET1_RIGHT <= button_max && slot != DPAD_SET1_RIGHT)
        DPAD_SET1_RIGHT = slot;
    if (DPAD_SET1_DOWN >= button_min && DPAD_SET1_DOWN <= button_max && slot != DPAD_SET1_DOWN)
        DPAD_SET1_DOWN = slot;
    if (DPAD_SET1_LEFT >= button_min && DPAD_SET1_LEFT <= button_max && slot != DPAD_SET1_LEFT)
        DPAD_SET1_LEFT = slot;
    
    if (CFG_DUAL_DPAD_ENABLED) {
        if (DPAD_SET2_UP >= button_min && DPAD_SET2_UP <= button_max && slot != DPAD_SET2_UP)
            DPAD_SET2_UP = slot;
        if (DPAD_SET2_RIGHT >= button_min && DPAD_SET2_RIGHT <= button_max && slot != DPAD_SET2_RIGHT)
            DPAD_SET2_RIGHT = slot;
        if (DPAD_SET2_DOWN >= button_min && DPAD_SET2_DOWN <= button_max && slot != DPAD_SET2_DOWN)
            DPAD_SET2_DOWN = slot;
        if (DPAD_SET2_LEFT >= button_min && DPAD_SET2_LEFT <= button_max && slot != DPAD_SET2_LEFT)
            DPAD_SET2_LEFT = slot;
    }
}

void Handle_Rupee_Drain(ActorPlayer* player, GlobalContext* ctxt) {
    if (CFG_RUPEE_DRAIN == 0)
        return;
    if (ctxt->pauseCtx.state != 0 || !IS_PLAYABLE)
        return;
    if (player->stateFlags.state1 & PLAYER_STATE1_GROTTO_IN || player->stateFlags.state1 & PLAYER_STATE1_TIME_STOP  || player->stateFlags.state1 & PLAYER_STATE1_SPECIAL_2   || player->stateFlags.state1 & PLAYER_STATE1_GET_ITEM || player->stateFlags.state1 & PLAYER_STATE1_TIME_STOP_2 ||
        player->stateFlags.state1 & PLAYER_STATE1_DEAD      || player->stateFlags.state1 & PLAYER_STATE1_MOVE_SCENE || player->stateFlags.state1 & PLAYER_STATE1_TIME_STOP_3 || player->stateFlags.state2 & PLAYER_STATE2_OCARINA  || player->stateFlags.state2 & PLAYER_STATE2_FROZEN      ||
        player->stateFlags.state3 & PLAYER_STATE3_TRANS_PART)
        return;
    
    if (gSaveContext.perm.unk24.currentLife > 1) {
        rupee_drain_frames++;
    
        if (rupee_drain_frames >= 60 / ctxt->state.framerateDivisor) {
            rupee_drain_frames = 0;
            rupee_drain_secs++;
        }
    
        if (rupee_drain_secs >= CFG_RUPEE_DRAIN) {
            rupee_drain_secs = 0;
            
            if (gSaveContext.perm.unk24.rupees > 0)
                gSaveContext.perm.unk24.rupees--;
            else {
                if (gSaveContext.perm.unk24.currentLife > 7)
                    gSaveContext.perm.unk24.currentLife -= 4;
                else gSaveContext.perm.unk24.currentLife = 1;
                z2_LinkInvincibility(player, 0x14);
                z2_PlaySfx(0x6848);
            }
        }
    }
    else rupee_drain_frames = rupee_drain_secs = 0;
}

void Handle_L_Button_Ingame(GlobalContext* ctxt) {
    //if (!CFG_DUAL_DPAD_ENABLED && !CFG_FPS_ENABLED && !CFG_FLOW_OF_TIME_ENABLED && !CFG_INSTANT_ELEGY_ENABLED && !CFG_HIDE_HUD_ENABLED)
    //    return;
    if (ctxt->pauseCtx.state != 0 || !IS_PLAYABLE || !L_PAD_ENABLED)
        return;
    
    InputPad padCurr = ctxt->state.input[0].current.buttons;
    
    if (padCurr.r)
        pressed.r = 1;
    if (padCurr.z)
        pressed.z = 1;
    if (padCurr.du)
        pressed.du = 1;
    if (padCurr.dr)
        pressed.dr = 1;
    if (padCurr.dd)
        pressed.dd = 1;
    if (padCurr.dl)
        pressed.dl = 1;
    
    if (ctxt->state.input[0].releaseEdge.buttons.l && !pressed.r && !pressed.z && !pressed.du && !pressed.dr && !pressed.dd && !pressed.dl)
        Toggle_Minimap(ctxt);
    
    if (!padCurr.l)
        pressed.r = pressed.z = pressed.du = pressed.dr = pressed.dd = pressed.dl = 0;
    if (ctxt->state.input[0].pressEdge.buttons.l)
        block = 1;
    if (!padCurr.l)
        block = 0;
    if (block)
        ctxt->state.input[0].current.buttons.r = ctxt->state.input[0].current.buttons.z = 0;
    ctxt->state.input[0].pressEdge.buttons.l = 0;    
}

void Handle_L_Button_Paused(GlobalContext* ctxt) {
    if (ctxt->pauseCtx.state == 0 || ctxt->pauseCtx.state > 6 || !IS_PLAYABLE)
        return;
    
    InputPad padCurr = ctxt->state.input[0].current.buttons;
    
    if (padCurr.a)
        pressed.a = 1;
    if (padCurr.b)
        pressed.b = 1;
    if (padCurr.s)
        pressed.s = 1;
    if (padCurr.r)
        pressed.r = 1;
    if (padCurr.z)
        pressed.z = 1;
    if (padCurr.cu)
        pressed.cu = 1;
    if (padCurr.cr)
        pressed.cr = 1;
    if (padCurr.cd)
        pressed.cd = 1;
    if (padCurr.cl)
        pressed.cl = 1;
    if (padCurr.du)
        pressed.du = 1;
    if (padCurr.dr)
        pressed.dr = 1;
    if (padCurr.dd)
        pressed.dd = 1;
    if (padCurr.dl)
        pressed.dl = 1;
    
    if (!padCurr.l)
        pressed.a = pressed.b = pressed.s = pressed.r = pressed.z = pressed.cu = pressed.cr = pressed.cd = pressed.cl = pressed.du = pressed.dr = pressed.dd = pressed.dl = 0;
    if (ctxt->state.input[0].pressEdge.buttons.l)
        block = 1;
    if (!padCurr.l)
        used_block = block = 0;
    if (block) {
        Inventory_Editor(ctxt);
        if (ctxt->state.input[0].pressEdge.buttons.b)
            Hide_Hud(ctxt);
        if (ctxt->state.input[0].pressEdge.buttons.r)
            Handle_Dual_Dpad(ctxt);
        
        ctxt->state.input[0].current.buttons.a    = ctxt->state.input[0].current.buttons.b    = ctxt->state.input[0].current.buttons.s   = ctxt->state.input[0].current.buttons.r     = ctxt->state.input[0].current.buttons.z   = \
        ctxt->state.input[0].current.buttons.cu   = ctxt->state.input[0].current.buttons.cr   = ctxt->state.input[0].current.buttons.cd   = ctxt->state.input[0].current.buttons.cl   = \
        ctxt->state.input[0].current.buttons.du   = ctxt->state.input[0].current.buttons.dr   = ctxt->state.input[0].current.buttons.dd   = ctxt->state.input[0].current.buttons.dl   = 0;
        ctxt->state.input[0].pressEdge.buttons.a  = ctxt->state.input[0].pressEdge.buttons.b  = ctxt->state.input[0].pressEdge.buttons.s  = ctxt->state.input[0].pressEdge.buttons.r  = ctxt->state.input[0].pressEdge.buttons.z = \
        ctxt->state.input[0].pressEdge.buttons.cu = ctxt->state.input[0].pressEdge.buttons.cr = ctxt->state.input[0].pressEdge.buttons.cd = ctxt->state.input[0].pressEdge.buttons.cl = \
        ctxt->state.input[0].pressEdge.buttons.du = ctxt->state.input[0].pressEdge.buttons.dr = ctxt->state.input[0].pressEdge.buttons.dd = ctxt->state.input[0].pressEdge.buttons.dl = 0;
    }
    ctxt->state.input[0].pressEdge.buttons.l = 0;
}

void Toggle_Minimap(GlobalContext* ctxt) {
    switch (ctxt->sceneNum) {
        case SCENE_CLOCKTOWER: // Clock Town & Termina Map
        case SCENE_BACKTOWN:
        case SCENE_ICHIBA:
        case SCENE_TOWN:
        case SCENE_ALLEY:
        case SCENE_00KEIKOKU:
            if (minimap_clock_town)
                Set_Minimap_Toggle();
            break;
        case SCENE_ROMANYMAE: // Milk Road Map
        case SCENE_KOEPONARACE:
        case SCENE_F01:
        case SCENE_F01_B:
        case SCENE_F01C:
            if (minimap_milk_road)
                Set_Minimap_Toggle();
            break;
        case SCENE_24KEMONOMITI: // Woodfall Map
        case SCENE_20SICHITAI:
        case SCENE_20SICHITAI2:
        case SCENE_22DEKUCITY:
        case SCENE_DEKU_KING:
        case SCENE_21MITURINMAE:
            if (minimap_woodfall)
                Set_Minimap_Toggle();
            break;
        case SCENE_13HUBUKINOMITI: // Snowhead Map
        case SCENE_10YUKIYAMANOMURA:
        case SCENE_10YUKIYAMANOMURA2:
        case SCENE_17SETUGEN:
        case SCENE_17SETUGEN2:
        case SCENE_GORONRACE:
        case SCENE_11GORONNOSATO:
        case SCENE_11GORONNOSATO2:
        case SCENE_16GORON_HOUSE:
        case SCENE_14YUKIDAMANOMITI:
        case SCENE_12HAKUGINMAE:
            if (minimap_snowhead)
                Set_Minimap_Toggle();
            break;
        case SCENE_30GYOSON: // Great Bay Map
        case SCENE_TORIDE:
        case SCENE_KAIZOKU:
        case SCENE_SINKAI:
        case SCENE_31MISAKI:
        case SCENE_35TAKI:
        case SCENE_33ZORACITY:
            if (minimap_great_bay)
                Set_Minimap_Toggle();
            break;
        case SCENE_IKANAMAE: // Stone Tower Map
        case SCENE_BOTI:
        case SCENE_IKANA:
        case SCENE_REDEAD:
        case SCENE_CASTLE:
        case SCENE_F40:
        case SCENE_F41:
            if (minimap_stone_tower)
                Set_Minimap_Toggle();
            break;
        case SCENE_MITURIN: // Woodfall Temple Dungeon Map
            if (gSaveContext.perm.inv.dungeonItems[0].map)
                Set_Minimap_Toggle();
            break;
        case SCENE_HAKUGIN: // Snowhead Temple Dungeon Map
            if (gSaveContext.perm.inv.dungeonItems[1].map)
                Set_Minimap_Toggle();
            break;
        case SCENE_SEA: // Great Bay Temple Dungeon Map
            if (gSaveContext.perm.inv.dungeonItems[2].map)
                Set_Minimap_Toggle();
            break;
        case SCENE_INISIE_N: // Stone Tower Temple Dungeon Map
        case SCENE_INISIE_R:
            if (gSaveContext.perm.inv.dungeonItems[3].map)
                Set_Minimap_Toggle();
            break;
    }
}

void Set_Minimap_Toggle() {
    r_minimap_disabled ^= 1;
    if (r_minimap_disabled)
        z2_PlaySfx(0x4813);
    else z2_PlaySfx(0x4814);
}

void Handle_Hud(GlobalContext* ctxt) {
    if (!hud_hide || ctxt->pauseCtx.state != 0)
        return;
    
    if (ctxt->interfaceCtx.alphas.buttonA != 0) {
        if (hud_counter < 8) {
            hud_counter++;
            return;
        }
    }
    else hud_counter = 0;
    
    if (ctxt->interfaceCtx.alphas.buttonA      > 40)   ctxt->interfaceCtx.alphas.buttonA      -= 40; else ctxt->interfaceCtx.alphas.buttonA      = 0;
    if (ctxt->interfaceCtx.alphas.buttonB      > 40)   ctxt->interfaceCtx.alphas.buttonB      -= 40; else ctxt->interfaceCtx.alphas.buttonB      = 0;
    if (ctxt->interfaceCtx.alphas.buttonCLeft  > 40)   ctxt->interfaceCtx.alphas.buttonCLeft  -= 40; else ctxt->interfaceCtx.alphas.buttonCLeft  = 0;
    if (ctxt->interfaceCtx.alphas.buttonCDown  > 40)   ctxt->interfaceCtx.alphas.buttonCDown  -= 40; else ctxt->interfaceCtx.alphas.buttonCDown  = 0;
    if (ctxt->interfaceCtx.alphas.buttonCRight > 40)   ctxt->interfaceCtx.alphas.buttonCRight -= 40; else ctxt->interfaceCtx.alphas.buttonCRight = 0;
    if (ctxt->interfaceCtx.alphas.buttonCRight > 40)   ctxt->interfaceCtx.alphas.buttonCRight -= 40; else ctxt->interfaceCtx.alphas.buttonCRight = 0;
        
    if (hud_hide == 2) {
        if (ctxt->interfaceCtx.alphas.life        > 40)   ctxt->interfaceCtx.alphas.life        -= 40; else ctxt->interfaceCtx.alphas.life        = 0;
        if (ctxt->interfaceCtx.alphas.magicRupees > 40)   ctxt->interfaceCtx.alphas.magicRupees -= 40; else ctxt->interfaceCtx.alphas.magicRupees = 0;
    }
}

void Hide_Hud(GlobalContext* ctxt) {
    if (!CFG_HIDE_HUD_ENABLED || ctxt->pauseCtx.debugMenu != 0 || ctxt->pauseCtx.state != 6)
        return;
    
    hud_hide++;
    if (hud_hide > 2)
        hud_hide = 0;
    z2_PlaySfx(!hud_hide ? 0x4813 : 0x4814);
}

void Handle_FPS(GlobalContext* ctxt) {
    if (!CFG_FPS_ENABLED || ctxt->pauseCtx.state != 0 || gSaveContext.extra.titleSetupIndex != 0 || ctxt->state.framerateDivisor == 1)
        return;
    
    if (!fps_switch || ctxt->opening_door == 0x00800020)
        ctxt->state.framerateDivisor = link_animation_speed = 3;
    else if (ctxt->text_state == 1 || ctxt->jump_state == 1 || ctxt->opening_chest == 1 || var_801D7B44 == 1 || playing_ocarina)
        ctxt->state.framerateDivisor = link_animation_speed = 2;
    else if (ctxt->playable_state == 0xFF08)
        ctxt->state.framerateDivisor = link_animation_speed = 3;
    else if (fps_switch)
        ctxt->state.framerateDivisor = link_animation_speed = 2;
    else if (ctxt->playable_state == 0x3208 && ctxt->use_hookshot == 0x100B)
        ctxt->state.framerateDivisor = link_animation_speed = 3;
    
    if (ctxt->state.framerateDivisor == 2) {
        if (gStaticContext.timeSpeed == 3)
            gStaticContext.timeSpeed = 2;
        if (gSaveContext.perm.timeSpeed == -2)
            gSaveContext.perm.timeSpeed = -1;
        inverted_time_state = -1;
    
        var_801160AE = 0x0006;
        var_80116702 = 0x0000;
        
        if (ctxt->jump_gravity == 0xC0B0) // Vanilla: 0xBF80
            ctxt->jump_gravity = 0xC050;
        else if (ctxt->var_803FFE64 == 0x5008 && ctxt->link_action == 0x4120)
            ctxt->jump_gravity = 0xBF00;
        else if (ctxt->jump_gravity == 0xBFB3 && ctxt->link_action == 0x4140)
            ctxt->jump_gravity = 0xBFD0;
        else if (ctxt->jump_height > 0x4120 || ctxt->link_action == 0x40E0 || ctxt->link_action == 0x4110 || ctxt->link_action == 0x4120 || ctxt->link_action == 0x4150 || ctxt->link_action == 0x4160 || ctxt->link_action == 0x4170)
            if (!GiantMask_IsGiant())
                ctxt->jump_gravity = 0xBF34;
        
        if (ctxt->var_803FFE64 == 0x3208) {
            if (ctxt->link_action == 0x40A0 || ctxt->link_action == 0x4110)
                ctxt->var_8040000C = 0x3F4CCCCD;
        }
        else if (ctxt->var_803FFE64 == 0x5008) {
            if (ctxt->link_action == 0x40E0 || ctxt->link_action == 0x4110 || ctxt->link_action == 0x4120)
                ctxt->var_8040000C = 0x3F333333;
        }
        else if (ctxt->var_803FFE64 == 0xC808) {
            if (ctxt->link_action == 0x4140 || ctxt->link_action == 0x4198)
                ctxt->var_8040000C = 0x3F800000;
        }
        
        if (ctxt->var_8040000C == 0x3FA00000)
            ctxt->var_8040000C =  0x3F900000;
        
        if (ctxt->deku_stick_timer == 100 && !deku_stick_timer_switch) {
            ctxt->deku_stick_timer += 100;
            deku_stick_timer_switch = 1;
        }
        else if (ctxt->deku_stick_timer == 0)
            deku_stick_timer_switch = 0;
    }
    else if (ctxt->state.framerateDivisor == 3) {
        if (gStaticContext.timeSpeed == 2)
            gStaticContext.timeSpeed = 3;
        if (gSaveContext.perm.timeSpeed == -1)
            gSaveContext.perm.timeSpeed = -2;
        inverted_time_state = -2;
    }
}

void Handle_Ocarina_Icons(GlobalContext* ctxt) {
    if (!CFG_OCARINA_ICONS_ENABLED)
        return;
    
    if (gSaveContext.extra.buttonsState.alphaTransition == 0xB || gSaveContext.perm.currentForm == PLAYER_FORM_HUMAN || gSaveContext.perm.currentForm == PLAYER_FORM_FIERCE_DEITY) {
        if (gSaveContext.perm.inv.items[0] == ITEM_DEKU_PIPES || gSaveContext.perm.inv.items[0] == ITEM_GORON_DRUMS || gSaveContext.perm.inv.items[0] == ITEM_ZORA_GUITAR)
            for (uint8_t button=1; button<=3; button++) {
                if (gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_DEKU_PIPES || gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_GORON_DRUMS ||gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_ZORA_GUITAR) {
                gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] = ITEM_OCARINA;
                gSaveContext.perm.unk4C.formButtonSlots[0].buttons[button] = SLOT_OCARINA;
                z2_UpdateButtonIcon(ctxt, button);
                break;
            }
            gSaveContext.perm.inv.items[0] = ITEM_OCARINA;
        }
        if (DPAD_CONFIG.state != DPAD_STATE_DISABLED) {
            if (DPAD_SET1_UP    == ITEM_DEKU_PIPES || DPAD_SET1_UP    == ITEM_GORON_DRUMS || DPAD_SET1_UP    == ITEM_ZORA_GUITAR)
                DPAD_SET1_UP = ITEM_OCARINA;
            if (DPAD_SET1_RIGHT == ITEM_DEKU_PIPES || DPAD_SET1_RIGHT == ITEM_GORON_DRUMS || DPAD_SET1_RIGHT == ITEM_ZORA_GUITAR)
                DPAD_SET1_RIGHT = ITEM_OCARINA;
            if (DPAD_SET1_DOWN  == ITEM_DEKU_PIPES || DPAD_SET1_DOWN  == ITEM_GORON_DRUMS || DPAD_SET1_DOWN  == ITEM_ZORA_GUITAR)
                DPAD_SET1_DOWN = ITEM_OCARINA;
            if (DPAD_SET1_LEFT  == ITEM_DEKU_PIPES || DPAD_SET1_LEFT  == ITEM_GORON_DRUMS || DPAD_SET1_LEFT  == ITEM_ZORA_GUITAR)
                DPAD_SET1_LEFT = ITEM_OCARINA;
            
            if (DPAD_SET2_UP    == ITEM_DEKU_PIPES || DPAD_SET2_UP    == ITEM_GORON_DRUMS || DPAD_SET2_UP    == ITEM_ZORA_GUITAR)
                DPAD_SET2_UP = ITEM_OCARINA;
            if (DPAD_SET2_RIGHT == ITEM_DEKU_PIPES || DPAD_SET2_RIGHT == ITEM_GORON_DRUMS || DPAD_SET2_RIGHT == ITEM_ZORA_GUITAR)
                DPAD_SET2_RIGHT = ITEM_OCARINA;
            if (DPAD_SET2_DOWN  == ITEM_DEKU_PIPES || DPAD_SET2_DOWN  == ITEM_GORON_DRUMS || DPAD_SET2_DOWN  == ITEM_ZORA_GUITAR)
                DPAD_SET2_DOWN = ITEM_OCARINA;
            if (DPAD_SET2_LEFT  == ITEM_DEKU_PIPES || DPAD_SET2_LEFT  == ITEM_GORON_DRUMS || DPAD_SET2_LEFT  == ITEM_ZORA_GUITAR)
                DPAD_SET2_LEFT = ITEM_OCARINA;
        }
    }
    
    else if (gSaveContext.perm.currentForm == PLAYER_FORM_DEKU) {
        if (gSaveContext.perm.inv.items[0] == ITEM_OCARINA || gSaveContext.perm.inv.items[0] == ITEM_GORON_DRUMS || gSaveContext.perm.inv.items[0] == ITEM_ZORA_GUITAR)
            for (uint8_t button=1; button<=3; button++) {
                if (gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_OCARINA || gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_GORON_DRUMS ||gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_ZORA_GUITAR) {
                gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] = ITEM_DEKU_PIPES;
                gSaveContext.perm.unk4C.formButtonSlots[0].buttons[button] = SLOT_OCARINA;
                z2_UpdateButtonIcon(ctxt, button);
                break;
            }
            gSaveContext.perm.inv.items[0] = ITEM_DEKU_PIPES;
        }
        if (DPAD_CONFIG.state != DPAD_STATE_DISABLED) {
            if (DPAD_SET1_UP    == ITEM_OCARINA || DPAD_SET1_UP    == ITEM_GORON_DRUMS || DPAD_SET1_UP    == ITEM_ZORA_GUITAR)
                DPAD_SET1_UP = ITEM_DEKU_PIPES;
            if (DPAD_SET1_RIGHT == ITEM_OCARINA || DPAD_SET1_RIGHT == ITEM_GORON_DRUMS || DPAD_SET1_RIGHT == ITEM_ZORA_GUITAR)
                DPAD_SET1_RIGHT = ITEM_DEKU_PIPES;
            if (DPAD_SET1_DOWN  == ITEM_OCARINA || DPAD_SET1_DOWN  == ITEM_GORON_DRUMS || DPAD_SET1_DOWN  == ITEM_ZORA_GUITAR)
                DPAD_SET1_DOWN = ITEM_DEKU_PIPES;
            if (DPAD_SET1_LEFT  == ITEM_OCARINA || DPAD_SET1_LEFT  == ITEM_GORON_DRUMS || DPAD_SET1_LEFT  == ITEM_ZORA_GUITAR)
                DPAD_SET1_LEFT = ITEM_DEKU_PIPES;
            
            if (DPAD_SET2_UP    == ITEM_OCARINA || DPAD_SET2_UP    == ITEM_GORON_DRUMS || DPAD_SET2_UP    == ITEM_ZORA_GUITAR)
                DPAD_SET2_UP = ITEM_OCARINA;
            if (DPAD_SET2_RIGHT == ITEM_OCARINA || DPAD_SET2_RIGHT == ITEM_GORON_DRUMS || DPAD_SET2_RIGHT == ITEM_ZORA_GUITAR)
                DPAD_SET2_RIGHT = ITEM_OCARINA;
            if (DPAD_SET2_DOWN  == ITEM_OCARINA || DPAD_SET2_DOWN  == ITEM_GORON_DRUMS || DPAD_SET2_DOWN  == ITEM_ZORA_GUITAR)
                DPAD_SET2_DOWN = ITEM_OCARINA;
            if (DPAD_SET2_LEFT  == ITEM_OCARINA || DPAD_SET2_LEFT  == ITEM_GORON_DRUMS || DPAD_SET2_LEFT  == ITEM_ZORA_GUITAR)
                DPAD_SET2_LEFT = ITEM_OCARINA;
        }
    }
    
    else if (gSaveContext.perm.currentForm == PLAYER_FORM_GORON) {
        if (gSaveContext.perm.inv.items[0] == ITEM_DEKU_PIPES || gSaveContext.perm.inv.items[0] == ITEM_OCARINA || gSaveContext.perm.inv.items[0] == ITEM_ZORA_GUITAR)
            for (uint8_t button=1; button<=3; button++) {
                if (gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_DEKU_PIPES || gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_OCARINA ||gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_ZORA_GUITAR) {
                gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] = ITEM_GORON_DRUMS;
                gSaveContext.perm.unk4C.formButtonSlots[0].buttons[button] = SLOT_OCARINA;
                z2_UpdateButtonIcon(ctxt, button);
                break;
            }
            gSaveContext.perm.inv.items[0] = ITEM_GORON_DRUMS;
        }
        if (DPAD_CONFIG.state != DPAD_STATE_DISABLED) {
            if (DPAD_SET1_UP    == ITEM_OCARINA || DPAD_SET1_UP    == ITEM_DEKU_PIPES || DPAD_SET1_UP    == ITEM_ZORA_GUITAR)
                DPAD_SET1_UP = ITEM_GORON_DRUMS;
            if (DPAD_SET1_RIGHT == ITEM_OCARINA || DPAD_SET1_RIGHT == ITEM_DEKU_PIPES || DPAD_SET1_RIGHT == ITEM_ZORA_GUITAR)
                DPAD_SET1_RIGHT = ITEM_GORON_DRUMS;
            if (DPAD_SET1_DOWN  == ITEM_OCARINA || DPAD_SET1_DOWN  == ITEM_DEKU_PIPES || DPAD_SET1_DOWN  == ITEM_ZORA_GUITAR)
                DPAD_SET1_DOWN = ITEM_GORON_DRUMS;
            if (DPAD_SET1_LEFT  == ITEM_OCARINA || DPAD_SET1_LEFT  == ITEM_DEKU_PIPES || DPAD_SET1_LEFT  == ITEM_ZORA_GUITAR)
                DPAD_SET1_LEFT = ITEM_GORON_DRUMS;
            
            if (DPAD_SET2_UP    == ITEM_OCARINA || DPAD_SET2_UP    == ITEM_DEKU_PIPES || DPAD_SET2_UP    == ITEM_ZORA_GUITAR)
                DPAD_SET2_UP = ITEM_GORON_DRUMS;
            if (DPAD_SET2_RIGHT == ITEM_OCARINA || DPAD_SET2_RIGHT == ITEM_DEKU_PIPES || DPAD_SET2_RIGHT == ITEM_ZORA_GUITAR)
                DPAD_SET2_RIGHT = ITEM_GORON_DRUMS;
            if (DPAD_SET2_DOWN  == ITEM_OCARINA || DPAD_SET2_DOWN  == ITEM_DEKU_PIPES || DPAD_SET2_DOWN  == ITEM_ZORA_GUITAR)
                DPAD_SET2_DOWN = ITEM_GORON_DRUMS;
            if (DPAD_SET2_LEFT  == ITEM_OCARINA || DPAD_SET2_LEFT  == ITEM_DEKU_PIPES || DPAD_SET2_LEFT  == ITEM_ZORA_GUITAR)
                DPAD_SET2_LEFT = ITEM_GORON_DRUMS;
        }
    }
    
    else if (gSaveContext.perm.currentForm == PLAYER_FORM_ZORA) {
        if (gSaveContext.perm.inv.items[0] == ITEM_DEKU_PIPES || gSaveContext.perm.inv.items[0] == ITEM_GORON_DRUMS || gSaveContext.perm.inv.items[0] == ITEM_OCARINA)
            for (uint8_t button=1; button<=3; button++) {
                if (gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_DEKU_PIPES || gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_GORON_DRUMS ||gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == ITEM_OCARINA) {
                gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] = ITEM_ZORA_GUITAR;
                gSaveContext.perm.unk4C.formButtonSlots[0].buttons[button] = SLOT_OCARINA;
                z2_UpdateButtonIcon(ctxt, button);
                break;
            }
            gSaveContext.perm.inv.items[0] = ITEM_ZORA_GUITAR;
        }
        if (DPAD_CONFIG.state != DPAD_STATE_DISABLED) {
            if (DPAD_SET1_UP    == ITEM_OCARINA || DPAD_SET1_UP    == ITEM_DEKU_PIPES || DPAD_SET1_UP    == ITEM_GORON_DRUMS)
                DPAD_SET1_UP = ITEM_ZORA_GUITAR;
            if (DPAD_SET1_RIGHT == ITEM_OCARINA || DPAD_SET1_RIGHT == ITEM_DEKU_PIPES || DPAD_SET1_RIGHT == ITEM_GORON_DRUMS)
                DPAD_SET1_RIGHT = ITEM_ZORA_GUITAR;
            if (DPAD_SET1_DOWN  == ITEM_OCARINA || DPAD_SET1_DOWN  == ITEM_DEKU_PIPES || DPAD_SET1_DOWN  == ITEM_GORON_DRUMS)
                DPAD_SET1_DOWN = ITEM_ZORA_GUITAR;
            if (DPAD_SET1_LEFT  == ITEM_OCARINA || DPAD_SET1_LEFT  == ITEM_DEKU_PIPES || DPAD_SET1_LEFT  == ITEM_GORON_DRUMS)
                DPAD_SET1_LEFT = ITEM_ZORA_GUITAR;
            
            if (DPAD_SET2_UP    == ITEM_OCARINA || DPAD_SET2_UP    == ITEM_DEKU_PIPES || DPAD_SET2_UP    == ITEM_GORON_DRUMS)
                DPAD_SET2_UP = ITEM_ZORA_GUITAR;
            if (DPAD_SET2_RIGHT == ITEM_OCARINA || DPAD_SET2_RIGHT == ITEM_DEKU_PIPES || DPAD_SET2_RIGHT == ITEM_GORON_DRUMS)
                DPAD_SET2_RIGHT = ITEM_ZORA_GUITAR;
            if (DPAD_SET2_DOWN  == ITEM_OCARINA || DPAD_SET2_DOWN  == ITEM_DEKU_PIPES || DPAD_SET2_DOWN  == ITEM_GORON_DRUMS)
                DPAD_SET2_DOWN = ITEM_ZORA_GUITAR;
            if (DPAD_SET2_LEFT  == ITEM_OCARINA || DPAD_SET2_LEFT  == ITEM_DEKU_PIPES || DPAD_SET2_LEFT  == ITEM_GORON_DRUMS)
                DPAD_SET2_LEFT = ITEM_ZORA_GUITAR;
        }
    }
}

void Handle_Infinite() {
    if (CFG_INFINITE_HEALTH) {
        if (gSaveContext.perm.unk24.currentLife < gSaveContext.perm.unk24.maxLife)
            gSaveContext.perm.unk24.currentLife = gSaveContext.perm.unk24.maxLife;
    }
    
    if (CFG_INFINITE_MAGIC) {
        if (gSaveContext.perm.unk24.magicLevel == 1)
            gSaveContext.perm.unk24.currentMagic = 0x30;
        else if (gSaveContext.perm.unk24.magicLevel == 2)
            gSaveContext.perm.unk24.currentMagic = 0x60;
    }
    
    if (CFG_INFINITE_AMMO) {
        gSaveContext.perm.inv.quantities[SLOT_DEKU_NUT] =                                              gItemUpgradeCapacity.nutCapacity[gSaveContext.perm.inv.upgrades.dekuNut];
        gSaveContext.perm.inv.quantities[SLOT_DEKU_STICK] =                                            gItemUpgradeCapacity.stickCapacity[gSaveContext.perm.inv.upgrades.dekuStick];
        gSaveContext.perm.inv.quantities[SLOT_BOW]  =                                                  gItemUpgradeCapacity.arrowCapacity[gSaveContext.perm.inv.upgrades.quiver];
        gSaveContext.perm.inv.quantities[SLOT_BOMB] = gSaveContext.perm.inv.quantities[SLOT_BOMBCHU] = gItemUpgradeCapacity.bombCapacity[gSaveContext.perm.inv.upgrades.bombBag];
    }
    
    if (CFG_INFINITE_RUPEES)
        gSaveContext.perm.unk24.rupees = gItemUpgradeCapacity.walletCapacity[gSaveContext.perm.inv.upgrades.wallet];
}

void Inventory_Editor(GlobalContext* ctxt) {
    if (!CFG_INVENTORY_EDITOR_ENABLED || ctxt->pauseCtx.state != 6 || used_block)
        return;
    
    if ( (ctxt->pauseCtx.debugMenu == 0 && ctxt->state.input[0].pressEdge.buttons.z) || ctxt->pauseCtx.debugMenu == 2) {
        ctxt->pauseCtx.debugMenu ^= 2;
        z2_PlaySfx(ctxt->pauseCtx.debugMenu == 0 ? 0x4813 : 0x4814);
        used_block = 1;
    }
}

void Handle_FPS_Toggle() {
    if (!CFG_FPS_ENABLED)
        return;
    
    fps_switch ^= 1;
    if (fps_switch)
        z2_PlaySfx(0x4814);
    else z2_PlaySfx(0x4813);
}

void Handle_Invert_Time() {
    if (!CFG_FLOW_OF_TIME_ENABLED || !gSaveContext.perm.inv.questStatus.songOfTime || gSaveContext.perm.timeSpeed == TIME_SPEED_STOPPED || gStaticContext.timeSpeed == 0 || !HAS_OCARINA)
        return;
    
    if (gSaveContext.perm.timeSpeed == TIME_SPEED_NORMAL) {
        gSaveContext.perm.timeSpeed = TIME_SPEED_INVERTED;
        z2_PlaySfx(0x4813);
    }
    else {
        gSaveContext.perm.timeSpeed = TIME_SPEED_NORMAL;
        z2_PlaySfx(0x4814);
    }
}

uint8_t Can_Use_Elegy(GlobalContext* ctxt) {
    for (uint8_t i=0; i<12; i++)
        if (ctxt->sceneNum == (*(uint16_t*) (0x8015546A + i * 8) ) )
            return 1;
    return 0;
}

void Handle_Instant_Elegy(GlobalContext* ctxt) {
    if (CFG_INSTANT_ELEGY_ENABLED && gSaveContext.perm.inv.questStatus.elegyOfEmptiness && HAS_OCARINA && Can_Use_Elegy(ctxt))
        if (ctxt->link_anim_1 != 0x67) // Instant Elegy
            ctxt->link_anim_1 = 0x67;
}

/*void Handle_Use_Pictobox(GlobalContext* ctxt, ActorPlayer* player) {
    if (CFG_QUICK_PICTOBOX_ENABLED && gSaveContext.perm.inv.items[13] == ITEM_PICTOGRAPH)
        z2_UseItem(ctxt, player, ITEM_DEKU_STICK);
}*/

void Handle_Dual_Dpad(GlobalContext* ctxt) {
    if (!CFG_DUAL_DPAD_ENABLED)
        return;
    dpad_alt ^= 1;
    z2_PlaySfx(dpad_alt == 1 ? 0x4813 : 0x4814);
}

void Handle_Sword_Toggle(GlobalContext* ctxt) {
    if (!CFG_SWAP_ENABLED)
        return;
    uint8_t sword = gSaveContext.perm.unk4C.equipment.sword;
    
    if (sword == 2 && !HAVE_RAZOR_SWORD)
        HAVE_EXTRA_SRAM |= 4;
    if (sword == 3 && !HAVE_GILDED_SWORD) {
        HAVE_EXTRA_SRAM |= 4;
        HAVE_EXTRA_SRAM |= 8;
    }
    
    sword++;
    if (sword == 1 && gSaveContext.perm.stolenItem == ITEM_KOKIRI_SWORD || REFORGING_KOKIRI_SWORD)
        sword++;
    if (sword == 2 && (!HAVE_RAZOR_SWORD || gSaveContext.perm.stolenItem == ITEM_RAZOR_SWORD || REFORGING_RAZOR_SWORD) )
        sword++;
    if (sword == 3 && (!HAVE_GILDED_SWORD || gSaveContext.perm.stolenItem == ITEM_GILDED_SWORD) )
        sword++;
    if (sword > 3)
        sword = 0;
    
    if (sword != gSaveContext.perm.unk4C.equipment.sword) {
        gSaveContext.perm.unk4C.equipment.sword    = sword;
        Handle_Equip_Sword(ctxt);
    }
}

void Handle_Shield_Toggle(GlobalContext* ctxt) {
    if (!CFG_SWAP_ENABLED)
        return;
    uint8_t shield = gSaveContext.perm.unk4C.equipment.shield;
    
    if (shield == 2 && !HAVE_MIRROR_SHIELD)
        HAVE_EXTRA_SRAM |= 32;
    
    shield++;
    if (shield == 1 && LOST_HERO_SHIELD)
        shield++;
    if (shield == 2 && !HAVE_MIRROR_SHIELD)
        shield++;
    if (shield > 2)
        shield = 0;
    
    if (shield != gSaveContext.perm.unk4C.equipment.shield) {
        gSaveContext.perm.unk4C.equipment.shield = ctxt->equipped_shield = shield;
        z2_PlaySfx(0x4808);
    }
}

/*void Handle_Saving(GlobalContext* ctxt) {
    if (ctxt->pauseCtx.state != 6 || !IS_PLAYABLE)
        return;
    if (!ctxt->state.input[0].pressEdge.buttons.b)
        return;
    ctxt->state.input[0].pressEdge.buttons.b = 0;
    
    ctxt->pauseCtx.state = 7;
}*/

void Handle_Unequipping(GlobalContext* ctxt) {
    if (!CFG_UNEQUIP_ENABLED || (ctxt->pauseCtx.screenIndex != 0 && ctxt->pauseCtx.screenIndex != 3) )
        return;
    
    uint8_t button;
    if (ctxt->state.input[0].pressEdge.buttons.cl)
        button = 1;
    else if (ctxt->state.input[0].pressEdge.buttons.cd)
        button = 2;
    else if (ctxt->state.input[0].pressEdge.buttons.cr)
        button = 3;
    else return;
    
    uint8_t item = ctxt->pauseCtx.selectedItem;
    if (item == 0x2)
        item = 0x4A;
    else if (item == 0x3)
        item = 0x4B;
    else if (item == 0x4)
        item = 0x4C;
    
    if (gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] == item) {
        ctxt->state.input[0].pressEdge.buttons.cl = ctxt->state.input[0].pressEdge.buttons.cd = ctxt->state.input[0].pressEdge.buttons.cr = 0;
        gSaveContext.perm.unk4C.formButtonItems[0].buttons[button] = gSaveContext.perm.unk4C.formButtonSlots[0].buttons[button] = 0xFF;
        z2_PlaySfx(0x480A);
    }
}

void Set_B_Button(GlobalContext* ctxt) {
    if (!CFG_B_BUTTON_ITEM_ENABLED || ctxt->pauseCtx.screenIndex != 0 || !ctxt->state.input[0].pressEdge.buttons.cu)
        return;
    
    uint8_t item = ctxt->pauseCtx.selectedItem;
    
    if (item == ITEM_FIRE_ARROW)
        item = ITEM_BOW_FIRE_ARROW;
    else if (item == ITEM_ICE_ARROW)
        item = ITEM_BOW_ICE_ARROW;
    else if (item == ITEM_LIGHT_ARROW)
        item = ITEM_BOW_LIGHT_ARROW;
    else if (item >= ITEM_EMPTY_BOTTLE)
        return;
    else if (item >= ITEM_BOW && item <= ITEM_LIGHT_ARROW)
        return;
    else if (item == ITEM_BOMB || item == ITEM_BOMBCHU || item == ITEM_LENS || item == ITEM_POWDER_KEG)
        return;
    
    gSaveContext.perm.unk4C.formButtonItems[0].buttons[0] = gSaveContext.perm.unk4C.formButtonSlots[0].buttons[0] = item;
    z2_UpdateButtonIcon(ctxt, 0);
    z2_PlaySfx(0x4808);
}