#include <stdbool.h>
#include <z64.h>
#include <z64addresses.h>
#include "Music.h"
#include "enums.h"
#include "macro.h"
#include "Text.h"
#include "Sprite.h"

extern u8 CFG_MUSIC_TRACK_DISPLAY_ENABLED;

static char sCurrentTrackName[SEQUENCE_NAME_MAX_SIZE] = "\0";
static ColorRGBA8 sColor                              = { 0xFF, 0xFF, 0xFF, 0xFF };
static ColorRGBA8 titleColor                          = { 0x00, 0xFF, 0x00, 0xFF };
static s16 sAlpha                                     = 0;
static u8 sLoadingSequenceId                          = 0;

char music_track_titles[SEQUENCES_LENGTH][SEQUENCE_NAME_MAX_SIZE] = {
/* 00 */    "",
/* 01 */    "",    
/* 02 */    "Termina Field",
/* 03 */    "Chase",
/* 04 */    "Majora's Theme",
/* 05 */    "The Clock Tower",
/* 06 */    "Stone Tower Temple",
/* 07 */    "Stone Tower Temple Upside-Down",
/* 08 */    "Missed Event 1",
/* 09 */    "Missed Event 2",
/* 0A */    "Happy Mask Salesman",
/* 0B */    "Song of Healing",
/* 0C */    "Southern Swamp",
/* 0D */    "Ghost Attack",
/* 0E */    "Boat Cruise",
/* 0F */    "Sharp's Curse",
/* 10 */    "Great Bay Coast",
/* 11 */    "Ikana Valley",
/* 12 */    "Deku Palace",
/* 13 */    "Mountain Village",
/* 14 */    "Pirates' Fortress",
/* 15 */    "Clock Town, First Day",
/* 16 */    "Clock Town, Second Day",
/* 17 */    "Clock Town, Third Day",
/* 18 */    "File Select",
/* 19 */    "Event Clear",
/* 1A */    "Battle",
/* 1B */    "Boss Battle",
/* 1C */    "Woodfall Temple",
/* 1D */    "Clock Town",
/* 1E */    "Opening",
/* 1F */    "House",                       
/* 20 */    "Game Over",
/* 21 */    "Boss Clear",
/* 22 */    "Item Catch",
/* 23 */    "Clock Town, Second Day",
/* 24 */    "Get A Heart Container!",
/* 25 */    "Mini Game",
/* 26 */    "Goron Race",
/* 27 */    "Music Box House",
/* 28 */    "Fairy's Fountain",
/* 29 */    "Zelda's Theme",
/* 2A */    "Rosa Sisters",
/* 2B */    "Open Treasure Box",
/* 2C */    "Marine Research Laboratory",
/* 2D */    "Giants' Theme",
/* 2E */    "Guru-Guru's Song",
/* 2F */    "Romani Ranch",
/* 30 */    "Goron Village",
/* 31 */    "Mayor's Meeting",
/* 32 */    "Ocarina \"Epona's Song\"",
/* 33 */    "Ocarina \"Sun's Song\"",
/* 34 */    "Ocarina \"Song of Time\"",
/* 35 */    "Ocarina \"Song of Storms\"",
/* 36 */    "Zora Hall",
/* 37 */    "Get A Mask!",
/* 38 */    "Mini Boss",
/* 39 */    "Small Item Catch",
/* 3A */    "Astral Observatory",
/* 3B */    "Cavern",
/* 3C */    "Milk Bar",
/* 3D */    "Enter Zelda",
/* 3E */    "Woods of Mystery",
/* 3F */    "Goron Race Goal",
/* 40 */    "Horse Race",
/* 41 */    "Horse Race Goal",
/* 42 */    "Gorman Track",
/* 43 */    "Magic Hags' Potion Shop",
/* 44 */    "Shop",
/* 45 */    "Owl",
/* 46 */    "Shooting Gallery",
/* 47 */    "Ocarina \"Song of Soaring\"",
/* 48 */    "Ocarina \"Song of Healing\"",
/* 49 */    "Ocrina \"Inverted Song of Time\"",
/* 4A */    "Ocarina \"Song of Double Time\"",
/* 4B */    "Sonatate of Awakening",
/* 4C */    "Goron Lullaby",
/* 4D */    "New Wave Bossa Nova",
/* 4E */    "Elegy of Emptiness",
/* 4F */    "Oath to Order",
/* 50 */    "Swordman's School",
/* 51 */    "Ocarina \"Goron Lullaby Intro\"",
/* 52 */    "Get The Ocarina!",
/* 53 */    "Bremen March",
/* 54 */    "Ballad of the Wind Wish",
/* 55 */    "Song of Soaring",
/* 56 */    "Milk Bar",
/* 57 */    "Last Day",
/* 58 */    "Mikau",
/* 59 */    "Mikau (End)",
/* 5A */    "Frog Song",
/* 5B */    "Ocarina \"Sonato of Awakening\"",
/* 5C */    "Ocarina \"Goron Lullaby\"",
/* 5D */    "Ocarina \"New Wave Bossa Nova\" ",
/* 5E */    "Ocarina \"Elegy of Emptiness\"",
/* 5F */    "Ocarina \"Oath to Order\"",
/* 60 */    "Majora's Lair",
/* 61 */    "Ocarina \"Goron Lullaby Intro\"",
/* 62 */    "Bass and Guitar Session",
/* 63 */    "Piano Solo",
/* 64 */    "The Indigo-Go's",
/* 65 */    "Snowhead Temple",
/* 66 */    "Great Bay Temple",
/* 67 */    "New Wave Bossa Nova (Saxophone)",
/* 68 */    "New Wave Bossa Nova (Vocals)",
/* 69 */    "Majora's Wrath Battle",
/* 6A */    "Majora's Incarnate Battle",
/* 6B */    "Majora's Mask Battle",
/* 6C */    "Bass Practice",
/* 6D */    "Drums Practice",
/* 6E */    "Piano Practice",
/* 6F */    "Ikana Castle",
/* 70 */    "Calling The Four Giants",
/* 71 */    "Kamaro's Dance",
/* 72 */    "Cremia's Carriage",
/* 73 */    "Keaton's Quiz",
/* 74 */    "The End / Credits",
/* 75 */    "Opening (Loop)",
/* 76 */    "Title Theme",
/* 77 */    "Woodfall Rises",
/* 78 */    "Southern Swamp Clear",
/* 79 */    "Snowhead Clear",
/* 7A */    "",
/* 7B */    "To The Moon",
/* 7C */    "The Giants' Exit",
/* 7D */    "Tatl & Tael",
/* 7E */    "Moon's Destruction",
/* 7F */    "The End / Credits II"
};

void Music_DrawCurrentTrackName(GlobalContext* ctxt, DispBuf* db) {
    if (!CFG_MUSIC_TRACK_DISPLAY_ENABLED || !s801D0B70.selected || !sAlpha)
        return;
    
    u8 i;
    sColor.a      = titleColor.a = sAlpha < 0x100 ? (u8)sAlpha : 0xFF;
    u8 spaceIndex = 0;
    u16 x         = 10;
    u16 y         = 90;
    Text_SetSize(5, 9);
    
    for (i=11; i<SEQUENCE_NAME_MAX_SIZE; i++)
        if (sCurrentTrackName[i] == ' ') {
            spaceIndex = i;
            break;
        }
    
    if (spaceIndex > 0) {
        char str1[SEQUENCE_NAME_MAX_SIZE];
        char str2[SEQUENCE_NAME_MAX_SIZE];
        
        for (i=0; i<spaceIndex; i++)
            str1[i] = sCurrentTrackName[i];
        for (i=spaceIndex+1; i<SEQUENCE_NAME_MAX_SIZE; i++)
            str2[i-spaceIndex-1] = sCurrentTrackName[i];
        
        Text_PrintWithColor(str1, x, y + 9,  sColor);
        Text_PrintWithColor(str2, x, y + 18, sColor);
    }
    else Text_PrintWithColor(sCurrentTrackName, x, y + 9, sColor);
    
    Text_PrintWithColor("Track Title:", x, y, titleColor);
    Text_Flush(db);
    z2_Math_StepToS(&sAlpha, 0, 0x10);
}

static void LoadTrackName(u32 id) {
    for (u8 index=0; index < SEQUENCE_NAME_MAX_SIZE; index++)
      sCurrentTrackName[index] = music_track_titles[id][index]; 
    sAlpha = 0x300;
}

void Music_SetLoadingSequenceId(u32 id) {
    if (id == 0x00 || id == 0x01 || id == 0x18 || id == 0x1A || id == 0x76 || id == 0x7A)
        return;
    
    sLoadingSequenceId = id;
    if (sLoadingSequenceId)
        LoadTrackName(sLoadingSequenceId);
}

/*struct MusicConfig MUSIC_CONFIG = {
    .magic = MUSIC_CONFIG_MAGIC,
    .version = 1,
};*/

/*static MusicState musicState = {
    .currentState = 0,
};*/

//static bool sIsMusicIndoors = false;
//static bool sIsMusicCave = false;

/*static void LoadMuteMask() {
    u8 sequenceId = gSequenceContext->sequenceId;
    if (musicState.loadedSequenceId != sequenceId) {
        musicState.loadedSequenceId = sequenceId;

        u32 index = MUSIC_CONFIG.sequenceMaskFileIndex;
        if (index) {
            DmaEntry entry = dmadata[index];

            u32 start = entry.romStart + (sequenceId * SEQUENCE_DATA_SIZE);

            z2_RomToRam(start, &musicState.playMask, SEQUENCE_DATA_SIZE);
            musicState.hasSequenceMaskFile = 1;
        } else {
            musicState.hasSequenceMaskFile = 0;
        }
    }
}*/

//static const u8* sAudioBaseFilter = (u8*)0x801D66E0;

/*static u16 CalculateCurrentState() {
    u16 state;
    ActorPlayer* player = GET_PLAYER(&gGlobalContext);
    if (player) {
        state = 1 << player->form;

        if (!sIsMusicIndoors && !sIsMusicCave) {
            state = musicState.cumulativeStates.outdoors ? state | SEQUENCE_PLAY_STATE_OUTDOORS : SEQUENCE_PLAY_STATE_OUTDOORS;
        }
        if (sIsMusicIndoors) {
            state = musicState.cumulativeStates.indoors ? state | SEQUENCE_PLAY_STATE_INDOORS : SEQUENCE_PLAY_STATE_INDOORS;
        }
        if (sIsMusicCave) {
            state = musicState.cumulativeStates.cave ? state | SEQUENCE_PLAY_STATE_CAVE : SEQUENCE_PLAY_STATE_CAVE;
        }
        if (player->stateFlags.state1 & PLAYER_STATE1_EPONA) {
            state = musicState.cumulativeStates.epona ? state | SEQUENCE_PLAY_STATE_EPONA : SEQUENCE_PLAY_STATE_EPONA;
        }
        if (player->stateFlags.state1 & PLAYER_STATE1_SWIM || player->stateFlags.state3 & PLAYER_STATE3_ZORA_SWIM || *sAudioBaseFilter == 0x20) {
            state = musicState.cumulativeStates.swimming ? state | SEQUENCE_PLAY_STATE_SWIM : SEQUENCE_PLAY_STATE_SWIM;
        }
        if (player->stateFlags.state3 & PLAYER_STATE3_GORON_SPIKE) {
            state = musicState.cumulativeStates.spikeRolling ? state | SEQUENCE_PLAY_STATE_SPIKE_ROLLING : SEQUENCE_PLAY_STATE_SPIKE_ROLLING;
        }
        if (gGlobalContext.actorCtx.targetContext.nearbyEnemy) {
            state = musicState.cumulativeStates.combat ? state | SEQUENCE_PLAY_STATE_COMBAT : SEQUENCE_PLAY_STATE_COMBAT;
        }
        if (z2_LifeMeter_IsCritical()) {
            state = musicState.cumulativeStates.criticalHealth ? state | SEQUENCE_PLAY_STATE_CRITICAL_HEALTH : SEQUENCE_PLAY_STATE_CRITICAL_HEALTH;
        }
    } else if (gGameStateInfo.fileSelect.loadedRamAddr) {
        u16 formMask = 0;
        u16 cumulativeStates = musicState.cumulativeStates.value;
        u16 nonCumulativeStates = ~cumulativeStates;
        if (gGlobalContext.state.input[0].pressEdge.buttons.du || gGlobalContext.state.input[0].pressEdge.buttons.l) {
            u8 startIndex = musicState.fileSelectMusicFormIndex;
            do {
                musicState.fileSelectMusicFormIndex = (musicState.fileSelectMusicFormIndex + 1) & 0xF;
                formMask = (1 << musicState.fileSelectMusicFormIndex) & nonCumulativeStates;
            } while (!formMask && musicState.fileSelectMusicFormIndex != startIndex);
        } else if (gGlobalContext.state.input[0].pressEdge.buttons.dd) {
            u8 startIndex = musicState.fileSelectMusicFormIndex;
            do {
                musicState.fileSelectMusicFormIndex = (musicState.fileSelectMusicFormIndex - 1) & 0xF;
                formMask = (1 << musicState.fileSelectMusicFormIndex) & nonCumulativeStates;
            } while (!formMask && musicState.fileSelectMusicFormIndex != startIndex);
        } else {
            formMask = (1 << musicState.fileSelectMusicFormIndex) & nonCumulativeStates;
        }

        u16 miscMask = 0;
        if (gGlobalContext.state.input[0].pressEdge.buttons.dr) {
            u8 startIndex = musicState.fileSelectMusicMiscIndex;
            do {
                musicState.fileSelectMusicMiscIndex = (musicState.fileSelectMusicMiscIndex + 1) & 0xF;
                miscMask = (1 << musicState.fileSelectMusicMiscIndex) & cumulativeStates;
            } while (!miscMask && musicState.fileSelectMusicMiscIndex != startIndex && musicState.fileSelectMusicMiscIndex);
        } else if (gGlobalContext.state.input[0].pressEdge.buttons.dl) {
            u8 startIndex = musicState.fileSelectMusicMiscIndex;
            do {
                musicState.fileSelectMusicMiscIndex = (musicState.fileSelectMusicMiscIndex - 1) & 0xF;
                miscMask = (1 << musicState.fileSelectMusicMiscIndex) & cumulativeStates;
            } while (!miscMask && musicState.fileSelectMusicMiscIndex != startIndex && musicState.fileSelectMusicMiscIndex);
        } else {
            miscMask = (1 << musicState.fileSelectMusicMiscIndex) & cumulativeStates;
        }

        state = formMask | miscMask;
    } else if (musicState.currentState) {
        state = musicState.currentState;
    } else {
        state = 1;
    }
    return state;
}*/

/*static void ProcessChannel(u8 channelIndex, u16 stateMask) {
    SequenceChannelContext* channelContext = gSequenceContext->channels[channelIndex];
    if (channelContext->playState.playing) {
        bool shouldBeMuted = false;
        if (!channelContext->playState.muted) {
            musicState.forceMute &= ~(1 << channelIndex);
        } else {
            shouldBeMuted = musicState.forceMute & (1 << channelIndex);
        }
        if (!shouldBeMuted) {
            u16 playMask = musicState.playMask[channelIndex];
            u16 formMask = playMask & ~musicState.cumulativeStates.value;
            u16 miscMask = playMask & musicState.cumulativeStates.value;
            u16 formState = stateMask & ~musicState.cumulativeStates.value;
            u16 miscState = stateMask & musicState.cumulativeStates.value;
            bool shouldPlay = (!miscMask || (miscMask & miscState)) && (formMask & formState);
            shouldBeMuted = !shouldPlay;
        }
        bool isMuted = channelContext->playState.muted;
        if (!isMuted && shouldBeMuted) {
            channelContext->playState.muted = true;
        } else if (isMuted && !shouldBeMuted) {
            channelContext->playState.muted = false;
        }
    }
}*/

/*static void HandleFormChannels(GlobalContext* ctxt) {
    LoadMuteMask();

    if (musicState.hasSequenceMaskFile) {
        u16 state = CalculateCurrentState();
        if (musicState.currentState != state) {
            musicState.currentState = state;

            for (u8 i = 0; i < sizeof(gSequenceContext->channels) / sizeof(SequenceChannelContext*); i++) {
                ProcessChannel(i, state);
            }
        }
    }
}*/

/*void Music_Update(GlobalContext* ctxt) {
    HandleFormChannels(ctxt);
}*/

/*void Music_AfterChannelInit(SequenceContext* sequence, u8 channelIndex) {
    if (sequence == gSequenceContext) {
        LoadMuteMask();

        if (musicState.hasSequenceMaskFile) {
            u16 state = CalculateCurrentState();
            musicState.currentState = state;
            ProcessChannel(channelIndex, state);
        }
    }
}*/

/*void Music_HandleChannelMute(SequenceChannelContext* channelContext, ChannelState* channelState, SequenceContext* sequence, u8 channelIndex) {
    u8 shouldBeMuted = channelState->param;
    if (shouldBeMuted) {
        if (musicState.hasSequenceMaskFile && sequence == gSequenceContext && !channelContext->playState.stopped) {
            musicState.forceMute |= (1 << channelIndex);
        }
        channelContext->playState.muted = true;
    } else {
        if (musicState.hasSequenceMaskFile && sequence == gSequenceContext) {
            musicState.forceMute &= ~(1 << channelIndex);
            ProcessChannel(channelIndex, musicState.currentState);
        } else {
            channelContext->playState.muted = false;
        }
    }
}*/

/*s8 Music_GetAudioLoadType(AudioInfo* audioInfo, u8 audioType) {
    s8 defaultLoadType = audioInfo[1].metadata[1];
    if (audioType == 1 && defaultLoadType != 0 && sLoadingSequenceId != 0) {
        AudioInfo* sequenceInfoTable = z2_GetAudioTable(0);
        return sequenceInfoTable[sLoadingSequenceId + 1].metadata[1];
    } else {
        return defaultLoadType;
    }
}*/

//static u8 sLastSeqId = 0xFF;

/*bool Music_ShouldFadeOut(GlobalContext* ctxt, s16 sceneLayer) {
    // TODO handle alternate exit scenarios
    // TODO handle taking a water void exit: z_player line 5760
    s16 currentScene = ctxt->sceneNum;
    u16 entrance = ctxt->warpDestination + sceneLayer;
    if (MUSIC_CONFIG.flags.removeMinorMusic && currentScene != SCENE_SPOT00) { // not cutscene
        if (z2_AudioSeq_GetActiveSeqId(3) != 0xFFFF) { // SEQ_PLAYER_BGM_SUB is playing
            return true;
        }
        u16 activeBgm = z2_AudioSeq_GetActiveSeqId(0);
        switch (activeBgm) {
            case 0x0D: // NA_BGM_ALIEN_INVASION
            case 0x38: // NA_BGM_MINI_BOSS
            case 0x55: // NA_BGM_SONG_OF_SOARING
            case 0xFFFF: // Nothing playing
                return true;
        }

        s32 nextScene = z2_Entrance_GetSceneIdAbsolute(entrance);
        if (nextScene == currentScene) {
            switch (activeBgm) {
                case 0x1D: // NA_BGM_MARKET // Clock Town
                    return gSaveContext.extra.voidFlag == -4 // Day transition
                        || (CHECK_EVENTINF(0x43) && !CHECK_EVENTINF(0x42)); // grandma story and not grandma short story
                default:
                    return false;
            }
        }
        if (gSaveContext.extra.voidFlag != -2) { // after-minigame respawn
            switch (currentScene) {
                case SCENE_KAKUSIANA: // Grottos
                case SCENE_WITCH_SHOP: // Potion Shop
                case SCENE_AYASHIISHOP: // Curiosity Shop
                case SCENE_OMOYA: // Ranch House and Barn
                case SCENE_BOWLING: // Honey and Darling
                case SCENE_SONCHONOIE: // Mayor's Residence
                //case SCENE_MILK_BAR: // Milk Bar
                case SCENE_TAKARAYA: // Treasure Chest Shop
                case SCENE_DEKUTES: // Deku Scrub Playground
                case SCENE_SYATEKI_MIZU: // Town Shooting Gallery
                case SCENE_SYATEKI_MORI: // Swamp Shooting Gallery
                case SCENE_SINKAI: // Pinnacle Rock
                case SCENE_YOUSEI_IZUMI: // Fairy's Fountain
                case SCENE_KAJIYA: // Mountain Smithy
                case SCENE_POSTHOUSE: // Post Office
                case SCENE_LABO: // Marine Research Lab
                case SCENE_8ITEMSHOP: // Trading Post
                case SCENE_TAKARAKUJI: // Lottery Shop
                case SCENE_FISHERMAN: // Fisherman's Hut
                case SCENE_GORONSHOP: // Goron Shop
                //case SCENE_35TAKI: // Waterfall Rapids
                case SCENE_BANDROOM: // Zora Hall Rooms
                case SCENE_GORON_HAKA: // Goron Graveyard
                case SCENE_TOUGITES: // Poe Hut
                case SCENE_DOUJOU: // Swordsman's School
                case SCENE_MAP_SHOP: // Tourist Information
                case SCENE_YADOYA: // Stock Pot Inn
                case SCENE_BOMYA: // Bomb Shop
                    return false;
            }
            switch (nextScene) {
                case SCENE_KAKUSIANA: // Grottos
                case SCENE_WITCH_SHOP: // Potion Shop
                case SCENE_AYASHIISHOP: // Curiosity Shop
                case SCENE_OMOYA: // Ranch House and Barn
                case SCENE_BOWLING: // Honey and Darling
                case SCENE_SONCHONOIE: // Mayor's Residence
                //case SCENE_MILK_BAR: // Milk Bar
                case SCENE_TAKARAYA: // Treasure Chest Shop
                case SCENE_DEKUTES: // Deku Scrub Playground
                case SCENE_MITURIN_BS: // Odolwa's Lair
                case SCENE_SYATEKI_MIZU: // Town Shooting Gallery
                case SCENE_SYATEKI_MORI: // Swamp Shooting Gallery
                case SCENE_SINKAI: // Pinnacle Rock
                case SCENE_YOUSEI_IZUMI: // Fairy's Fountain
                case SCENE_KAJIYA: // Mountain Smithy
                case SCENE_POSTHOUSE: // Post Office
                case SCENE_LABO: // Marine Research Lab
                case SCENE_8ITEMSHOP: // Trading Post
                case SCENE_INISIE_BS: // Twinmold's Lair
                case SCENE_TAKARAKUJI: // Lottery Shop
                case SCENE_FISHERMAN: // Fisherman's Hut
                case SCENE_GORONSHOP: // Goron Shop
                case SCENE_HAKUGIN_BS: // Goht's Lair
                //case SCENE_35TAKI: // Waterfall Rapids
                case SCENE_BANDROOM: // Zora Hall Rooms
                case SCENE_GORON_HAKA: // Goron Graveyard
                case SCENE_TOUGITES: // Poe Hut
                case SCENE_DOUJOU: // Swordsman's School
                case SCENE_MAP_SHOP: // Tourist Information
                case SCENE_SEA_BS: // Gyorg's Lair
                case SCENE_YADOYA: // Stock Pot Inn
                case SCENE_BOMYA: // Bomb Shop
                    return false;
                case SCENE_INISIE_R: // Inverted Stone Tower Temple
                    return currentScene != SCENE_F41; // Inverted Stone Tower
                case SCENE_F41: // Inverted Stone Tower
                    return currentScene != SCENE_INISIE_R; // Inverted Stone Tower Temple
            }
        }
    }
    return !(z2_Entrance_GetTransitionFlags(entrance) & 0x8000);
}*/

/*void Music_HandleCommandSoundSettings(GlobalContext* ctxt, SceneCmd* cmd) {
    ctxt->sequenceCtx.ambienceId = cmd->soundSettings.ambienceId;

    sIsMusicCave = false;
    sIsMusicIndoors = false;
    switch (ctxt->sceneNum) {
        case SCENE_KAKUSIANA: // Grottos
        case SCENE_DEKUTES: // Deku Scrub Playground
        case SCENE_YOUSEI_IZUMI: // Fairy's Fountain
        case SCENE_GORON_HAKA: // Goron Graveyard
            sIsMusicCave = true;
            break;
        case SCENE_WITCH_SHOP: // Potion Shop
        case SCENE_AYASHIISHOP: // Curiosity Shop
        case SCENE_OMOYA: // Ranch House and Barn
        case SCENE_BOWLING: // Honey and Darling
        case SCENE_SONCHONOIE: // Mayor's Residence
        //case SCENE_MILK_BAR: // Milk Bar
        case SCENE_TAKARAYA: // Treasure Chest Shop
        case SCENE_SYATEKI_MIZU: // Town Shooting Gallery
        case SCENE_SYATEKI_MORI: // Swamp Shooting Gallery
        case SCENE_KAJIYA: // Mountain Smithy
        case SCENE_POSTHOUSE: // Post Office
        case SCENE_LABO: // Marine Research Lab
        case SCENE_8ITEMSHOP: // Trading Post
        case SCENE_TAKARAKUJI: // Lottery Shop
        case SCENE_FISHERMAN: // Fisherman's Hut
        case SCENE_GORONSHOP: // Goron Shop
        case SCENE_BANDROOM: // Zora Hall Rooms
        case SCENE_TOUGITES: // Poe Hut
        case SCENE_DOUJOU: // Swordsman's School
        case SCENE_MAP_SHOP: // Tourist Information
        case SCENE_YADOYA: // Stock Pot Inn
        case SCENE_BOMYA: // Bomb Shop
            sIsMusicIndoors = true;
            break;
    }

    if (!MUSIC_CONFIG.flags.removeMinorMusic) {
        ctxt->sequenceCtx.seqId = cmd->soundSettings.seqId;
        return;
    }
    bool shouldContinueMusic = sIsMusicIndoors || sIsMusicCave;
    if (!shouldContinueMusic) {
        switch (ctxt->sceneNum) {
            case SCENE_MITURIN_BS: // Odolwa's Lair
            case SCENE_SINKAI: // Pinnacle Rock
            case SCENE_INISIE_BS: // Twinmold's Lair
            case SCENE_HAKUGIN_BS: // Goht's Lair
            case SCENE_SEA_BS: // Gyorg's Lair
                shouldContinueMusic = true;
                break;
        }
    }

    u8 seqId = gSaveContext.extra.seqId;

    if (shouldContinueMusic && seqId != 0xFF) {
        sLastSeqId = seqId;
    } else if (!shouldContinueMusic || sLastSeqId == 0xFF) {
        sLastSeqId = cmd->soundSettings.seqId;
    }
    ctxt->sequenceCtx.seqId = sLastSeqId;
}*/

/*static bool ObjSound_ShouldSetBgm(Actor* objSound, GlobalContext* ctxt) {
    if (MUSIC_CONFIG.flags.removeMinorMusic) {
        switch (ctxt->sceneNum) {
            case SCENE_BOWLING: // Honey and Darling
            //case SCENE_MILK_BAR: // Milk Bar
            case SCENE_TAKARAYA: // Treasure Chest Shop
            case SCENE_SYATEKI_MIZU: // Town Shooting Gallery
            case SCENE_SYATEKI_MORI: // Swamp Shooting Gallery
            case SCENE_8ITEMSHOP: // Trading Post
            case SCENE_TAKARAKUJI: // Lottery Shop
            case SCENE_GORONSHOP: // Goron Shop
            case SCENE_BANDROOM: // Zora Hall Rooms
            case SCENE_MAP_SHOP: // Tourist Information
            case SCENE_BOMYA: // Bomb Shop
                return false;
        }
    }
    return true;
}*/

/*void Music_ObjSound_PlayBgm(Actor* objSound, GlobalContext* ctxt) {
    if (!ObjSound_ShouldSetBgm(objSound, ctxt)) {
        z2_ActorUnload(objSound);
        return;
    }
    z2_Audio_PlayObjSoundBgm(&objSound->projectedPos, objSound->params);
}*/

/*void Music_ObjSound_StopBgm(Actor* objSound, GlobalContext* ctxt) {
    if (ObjSound_ShouldSetBgm(objSound, ctxt)) {
        z2_Audio_PlayObjSoundBgm(NULL, 0);
    }
}*/
