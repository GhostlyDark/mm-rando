#include <z64.h>
#include "Misc.h"

extern uint8_t CFG_BOSS_REMAINS_DEKU_HOP;
#define IS_ABILITY_ACTIVATED(boss) (boss == 0 || (gSaveContext.perm.inv.questStatus.odolwasRemains && boss == 1) || (gSaveContext.perm.inv.questStatus.gohtsRemains && boss == 2) || (gSaveContext.perm.inv.questStatus.gyorgsRemains && boss == 3) || (gSaveContext.perm.inv.questStatus.twinmoldsRemains && boss == 4))

struct DekuHopState {
    u8 lastHop;
    u8 isUsingContinuousDekuHop;
};

static struct DekuHopState gDekuHopState = {
    .lastHop = 5,
    .isUsingContinuousDekuHop = 0,
};

void DekuHop_Handle(ActorPlayer* player, GlobalContext* ctxt) {
    if (!MISC_CONFIG.flags.continuousDekuHop || !IS_ABILITY_ACTIVATED(CFG_BOSS_REMAINS_DEKU_HOP)) {
        return;
    }

    if (player->stateFlags.state3 & PLAYER_STATE3_DEKU_HOP) {
        if (player->dekuHopCounter < 5 && player->dekuHopCounter > 0 && player->dekuHopCounter != gDekuHopState.lastHop) {
            if (ctxt->state.input[0].pressEdge.buttons.a) {
                ctxt->state.input[0].pressEdge.buttons.a = 0;
                player->dekuHopCounter++;
                gDekuHopState.lastHop = player->dekuHopCounter;
                gDekuHopState.isUsingContinuousDekuHop = true;
            }
        } else {
            gDekuHopState.lastHop = player->dekuHopCounter;
        }
    } else {
        gDekuHopState.isUsingContinuousDekuHop = false;
    }

}

f32 DekuHop_GetSpeedModifier() {
    if (gDekuHopState.isUsingContinuousDekuHop) {
        return 1;
    }
    return 0.5;
}
