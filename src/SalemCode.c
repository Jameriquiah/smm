#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "playermodelmanager_api.h"
#include "gSalemHumanSkel.h"
#include "gSalemHumanHandSkel.h"
#include "gSalemHumanHandClosedSkel.h"
#include "gSalemHumanLeftHandBottleSkel.h"
#include "gSalemHumanRightHandOcarinaSkel.h"
#include "gSalemHumanFPSHandDL.h"
#include "gSalemDekuSkel.h"
#include "gSalemGoronSkel.h"
#include "gSalemGoronHandSkel.h"
#include "gSalemGoronHandClosedSkel.h"
#include "gSalemGoronHandBottleSkel.h"
#include "gSalemGoronShieldingSkel.h"
#include "gSalemGoronCurledDL.h"
#include "gSalemGoronCurledSpikesDL.h"
#include "gSalemZoraSkel.h"
#include "gSalemZoraHandSkel.h"
#include "gSalemZoraHandClosedSkel.h"
#include "gSalemZoraHandBottleSkel.h"
#include "gSalemZoraHandGuitarSkel.h"
#include "gSalemZoraLeftFinDL.h"
#include "gSalemZoraRightFinDL.h"
#include "gSalemZoraLeftSwimmingFinDL.h"
#include "gSalemZoraRightSwimmingFinDL.h"
#include "gSalemZoraLeftBoomerangDL.h"
#include "gSalemZoraRightBoomerangDL.h"
#include "gSalemZoraShieldDL.h"
#include "gSalemZoraMagicBarrierDL.h"
#include "gSalemFierceDeitySkel.h"
#include "gSalemFierceDeityHairSkel.h"
#include "gSalemFierceDeityHandSkel.h"
#include "FDMaskDL.h"
#include "FDMaskScreamDL.h"
#include "gElegyShellSalemHumanDL.h"

TexturePtr HumanTexturesEyes[] = {
    gSalemHumanEyesOpenTex,
    gSalemHumanEyesHalfTex,
    gSalemHumanEyesClosedTex,
    gSalemHumanEyesRightTex,
    gSalemHumanEyesLeftTex,
    gSalemHumanEyesDownTex,
    gSalemHumanEyesUpTex,
    gSalemHumanEyesWincingTex,
};

TexturePtr HumanTexturesMouth[] = {
    gSalemHumanMouthClosedTex,
    gSalemHumanMouthHalfTex,
    gSalemHumanMouthOpenTex,
    gSalemHumanMouthSmileTex,
};

TexturePtr DekuTexturesEyes[] = {
    gSalemDekuEyesOpenTex,
    gSalemDekuEyesHalfTex,
    gSalemDekuEyesClosedTex,
    gSalemDekuEyesRightTex,
    gSalemDekuEyesLeftTex,
    gSalemDekuEyesDownTex,
    gSalemDekuEyesUpTex,
    gSalemDekuEyesWincingTex,
};

TexturePtr GoronTexturesEyes[] = {
    gSalemGoronEyesOpenTex,
    gSalemGoronEyesHalfTex,
    gSalemGoronEyesClosedTex,
    gSalemGoronEyesSurprisedTex,
};

TexturePtr ZoraTexturesEyes[] = {
    gSalemZoraEyesOpenTex,
    gSalemZoraEyesHalfTex,
    gSalemZoraEyesClosedTex,
    gSalemZoraEyesRightTex,
    gSalemZoraEyesLeftTex,
    gSalemZoraEyesDownTex,
    gSalemZoraEyesUpTex,
    gSalemZoraEyesWincingTex,
};

TexturePtr ZoraTexturesMouth[] = {
    gSalemZoraMouthClosedTex,
    gSalemZoraMouthHalfTex,
    gSalemZoraMouthOpenTex,
    gSalemZoraMouthSmileTex,
};

TexturePtr FierceDeityTexturesEyes[] = {
    gSalemFierceDeityEyesOpenTex,
    gSalemFierceDeityEyesHalfTex,
    gSalemFierceDeityEyesClosedTex,
    gSalemFierceDeityEyesRightTex,
    gSalemFierceDeityEyesLeftTex,
    gSalemFierceDeityEyesDownTex,
    gSalemFierceDeityEyesUpTex,
    gSalemFierceDeityEyesWincingTex,
};

TexturePtr FierceDeityTexturesMouth[] = {
    gSalemFierceDeityMouthClosedTex,
    gSalemFierceDeityMouthHalfTex,
    gSalemFierceDeityMouthOpenTex,
    gSalemFierceDeityMouthSmileTex,
};

static const Gfx gSalemZoraLeftHandDL[] = {
    gsSPMatrix(0x0D000440, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandSkel_bone020_gLinkZoraTorsoLimb_mesh_layer_Opaque),
    gsSPMatrix(0x0D000280, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandSkel_bone013_gLinkZoraLeftShoulderLimb_mesh_layer_Opaque),
    gsSPMatrix(0x0D0002C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandSkel_bone014_gLinkZoraLeftForearmLimb_mesh_layer_Opaque),
    gsSPMatrix(0x0D000300, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandSkel_bone015_gLinkZoraLeftHandLimb_mesh_layer_Opaque),
    gsSPEndDisplayList(),
};

static const Gfx gSalemZoraRightHandDL[] = {
    gsSPMatrix(0x0D000440, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandSkel_bone020_gLinkZoraTorsoLimb_mesh_layer_Opaque),
    gsSPMatrix(0x0D000340, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandSkel_bone016_gLinkZoraRightShoulderLimb_mesh_layer_Opaque),
    gsSPMatrix(0x0D000380, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandSkel_bone017_gLinkZoraRightForearmLimb_mesh_layer_Opaque),
    gsSPMatrix(0x0D0003C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandSkel_bone018_gLinkZoraRightHandLimb_mesh_layer_Opaque),
    gsSPEndDisplayList(),
};

static const Gfx gSalemZoraLeftHandClosedDL[] = {
    gsSPMatrix(0x0D000440, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandSkel_bone020_gLinkZoraTorsoLimb_mesh_layer_Opaque),
    gsSPMatrix(0x0D000280, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandSkel_bone013_gLinkZoraLeftShoulderLimb_mesh_layer_Opaque),
    gsSPMatrix(0x0D0002C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandSkel_bone014_gLinkZoraLeftForearmLimb_mesh_layer_Opaque),
    gsSPMatrix(0x0D000300, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandSkel_bone015_gLinkZoraLeftHandLimb_mesh_layer_Opaque),
    gsSPEndDisplayList(),
};

static const Gfx gSalemZoraRightHandClosedDL[] = {
    gsSPMatrix(0x0D000440, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandClosedSkel_bone020_gLinkZoraTorsoLimb_mesh_layer_Opaque),
    gsSPMatrix(0x0D000340, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandClosedSkel_bone016_gLinkZoraRightShoulderLimb_mesh_layer_Opaque),
    gsSPMatrix(0x0D000380, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandClosedSkel_bone017_gLinkZoraRightForearmLimb_mesh_layer_Opaque),
    gsSPMatrix(0x0D0003C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandClosedSkel_bone018_gLinkZoraRightHandLimb_mesh_layer_Opaque),
    gsSPEndDisplayList(),
};

static const Gfx gSalemZoraLeftHandBottleDL[] = {
    gsSPMatrix(0x0D000440, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandBottleSkel_bone020_gLinkZoraTorsoLimb_mesh_layer_Opaque),
    gsSPMatrix(0x0D000280, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandBottleSkel_bone013_gLinkZoraLeftShoulderLimb_mesh_layer_Opaque),
    gsSPMatrix(0x0D0002C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandBottleSkel_bone014_gLinkZoraLeftForearmLimb_mesh_layer_Opaque),
    gsSPMatrix(0x0D000300, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandBottleSkel_bone015_gLinkZoraLeftHandLimb_mesh_layer_Opaque),
    gsSPEndDisplayList(),
};

static const Gfx gSalemZoraLeftHandGuitarDL[] = {
    gsSPMatrix(0x0D000440, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandGuitarSkel_bone020_gLinkZoraTorsoLimb_mesh_layer_Opaque),
    gsSPMatrix(0x0D000280, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandGuitarSkel_bone013_gLinkZoraLeftShoulderLimb_mesh_layer_Opaque),
    gsSPMatrix(0x0D0002C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandGuitarSkel_bone014_gLinkZoraLeftForearmLimb_mesh_layer_Opaque),
    gsSPMatrix(0x0D000300, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPDisplayList(gSalemZoraHandGuitarSkel_bone015_gLinkZoraLeftHandLimb_mesh_layer_Opaque),
    gsSPEndDisplayList(),
};

PLAYERMODELMANAGER_CALLBACK_REGISTER_MODELS void registerHuman() {
    PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("mmsalem", PMM_MODEL_TYPE_CHILD);

    PlayerModelManager_setAuthor(h, "Jameriquiah");
    PlayerModelManager_setDisplayName(h, "Salem");
    PlayerModelManager_setEyesTextures(h, HumanTexturesEyes);
    PlayerModelManager_setMouthTextures(h, HumanTexturesMouth);
    PlayerModelManager_setDisplayList(h, PMM_DL_LHAND, gSalemHumanHandSkel_bone015_gLinkHumanLeftHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_RHAND, gSalemHumanHandSkel_bone018_gLinkHumanRightHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_LFIST, gSalemHumanHandClosedSkel_bone015_gLinkHumanLeftHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_RFIST, gSalemHumanHandClosedSkel_bone018_gLinkHumanRightHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_LHAND_BOTTLE, gSalemHumanLeftHandBottleSkel_bone015_gLinkHumanLeftHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_OPT_RHAND_OCARINA, gSalemHumanRightHandOcarinaSkel_bone018_gLinkHumanRightHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_FPS_RHAND, gSalemHumanFPSHandDL);
	PlayerModelManager_setDisplayList(h, PMM_DL_ELEGY_OF_EMPTINESS_SHELL_HUMAN, gElegyShellSalemHumanDL);

    PlayerModelManager_setSkeleton(h, &gSalemHumanSkel);
}

PLAYERMODELMANAGER_CALLBACK_REGISTER_MODELS void registerDeku() {
    PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("mmdsalem", PMM_MODEL_TYPE_DEKU);

    PlayerModelManager_setAuthor(h, "Jameriquiah");
    PlayerModelManager_setDisplayName(h, "Deku Salem");
    PlayerModelManager_setEyesTextures(h, DekuTexturesEyes);
    PlayerModelManager_setDisplayList(h, PMM_DL_LFIST, gSalemDekuSkel_bone015_gLinkDekuLeftHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_RFIST, gSalemDekuSkel_bone018_gLinkDekuRightHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_LHAND_BOTTLE, gSalemDekuSkel_bone015_gLinkDekuLeftHandLimb_mesh_layer_Opaque);

    PlayerModelManager_setSkeleton(h, &gSalemDekuSkel);
}

PLAYERMODELMANAGER_CALLBACK_REGISTER_MODELS void registerGoron() {
    PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("mmgsalem", PMM_MODEL_TYPE_GORON);

    PlayerModelManager_setAuthor(h, "Jameriquiah");
    PlayerModelManager_setDisplayName(h, "Goron Salem");
    PlayerModelManager_setEyesTextures(h, GoronTexturesEyes);
    PlayerModelManager_setDisplayList(h, PMM_DL_LHAND, gSalemGoronHandSkel_bone015_gLinkGoronLeftHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_RHAND, gSalemGoronHandSkel_bone018_gLinkGoronRightHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_LFIST, gSalemGoronHandClosedSkel_bone015_gLinkGoronLeftHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_RFIST, gSalemGoronHandClosedSkel_bone018_gLinkGoronRightHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_LHAND_BOTTLE, gSalemGoronHandBottleSkel_bone015_gLinkGoronLeftHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_BODY_SHIELD_BODY, gSalemGoronShieldingSkel_bone001_gLinkGoronShieldingBodyLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_BODY_SHIELD_HEAD, gSalemGoronShieldingSkel_bone002_gLinkGoronShieldingHeadLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_BODY_SHIELD_ARMS_AND_LEGS, gSalemGoronShieldingSkel_bone003_gLinkGoronShieldingArmsAndLegsLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_CURLED, gSalemGoronCurledDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_SPIKES, gSalemGoronCurledSpikesDL);

    PlayerModelManager_setSkeleton(h, &gSalemGoronSkel);
}

PLAYERMODELMANAGER_CALLBACK_REGISTER_MODELS void registerZora() {
    PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("mmzsalem", PMM_MODEL_TYPE_ZORA);

    PlayerModelManager_setAuthor(h, "Jameriquiah");
    PlayerModelManager_setDisplayName(h, "Zora Salem");
	PlayerModelManager_setEyesTextures(h, ZoraTexturesEyes);
    PlayerModelManager_setMouthTextures(h, ZoraTexturesMouth);
    PlayerModelManager_setDisplayList(h, PMM_DL_LHAND, gSalemZoraLeftHandDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_RHAND, gSalemZoraRightHandDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_LFIST, gSalemZoraLeftHandClosedDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_RFIST, gSalemZoraRightHandClosedDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_LHAND_BOTTLE, gSalemZoraLeftHandBottleDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_LHAND_GUITAR, gSalemZoraLeftHandGuitarDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_LFIN, gSalemZoraLeftFinDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_RFIN, gSalemZoraRightFinDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_LFIN_SWIM, gSalemZoraLeftSwimmingFinDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_RFIN_SWIM, gSalemZoraRightSwimmingFinDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_LFIN_BOOMERANG, gSalemZoraLeftBoomerangDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_RFIN_BOOMERANG, gSalemZoraRightBoomerangDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_FIN_SHIELD, gSalemZoraShieldDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_MAGIC_BARRIER, gSalemZoraMagicBarrierDL);

    PlayerModelManager_setSkeleton(h, &gSalemZoraSkel);
}

PLAYERMODELMANAGER_CALLBACK_REGISTER_MODELS void registerFierceDeity() {
    PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("mmfdsalem", PMM_MODEL_TYPE_FIERCE_DEITY);

    PlayerModelManager_setAuthor(h, "Jameriquiah");
    PlayerModelManager_setDisplayName(h, "Fierce Deity Salem");
    PlayerModelManager_setEyesTextures(h, FierceDeityTexturesEyes);
    PlayerModelManager_setMouthTextures(h, FierceDeityTexturesMouth);
    PlayerModelManager_setDisplayList(h, PMM_DL_LHAND, gSalemFierceDeityHandSkel_bone015_gLinkFierceDeityLeftHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_RHAND, gSalemFierceDeityHandSkel_bone018_gLinkFierceDeityRightHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_LFIST, gSalemFierceDeityHandSkel_bone015_gLinkFierceDeityLeftHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_RFIST, gSalemFierceDeityHandSkel_bone018_gLinkFierceDeityRightHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_LHAND_BOTTLE, gSalemFierceDeityHandSkel_bone015_gLinkFierceDeityLeftHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_MASK_FIERCE_DEITY, FDMaskDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_MASK_FIERCE_DEITY_SCREAM, FDMaskScreamDL);

    PlayerModelManager_setSkeleton(h, &gSalemFierceDeitySkel);
}