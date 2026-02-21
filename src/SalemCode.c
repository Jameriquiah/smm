#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "playermodelmanager_api.h"
#include "gSalemHumanSkel.h"
#include "gSalemHumanHandsClosedSkel.h"
#include "gSalemHumanLeftHandBottleSkel.h"
#include "gSalemHumanRightHandOcarinaSkel.h"
#include "FPSArmDL.h"
#include "gSalemDekuSkel.h"
#include "gSalemGoronSkel.h"
#include "GoronLeftHandClosedDL.h"
#include "GoronRightHandClosedDL.h"
#include "GoronLeftHandBottleDL.h"
#include "GoronCurledDL.h"
#include "gSalemZoraSkel.h"
#include "gSalemZoraLeftHandBottleSkel.h"
#include "gSalemZoraHandsClosedSkel.h"
#include "gSalemZoraLeftHandGuitarSkel.h"
#include "ZoraLeftFinDL.h"
#include "ZoraRightFinDL.h"
#include "ZoraLeftSwimmingFinDL.h"
#include "ZoraRightSwimmingFinDL.h"
#include "gSalemFierceDeitySkel.h"
#include "FDLeftHandBottleDL.h"
#include "FDLeftHandClosedDL.h"
#include "FDRightHandClosedDL.h"
#include "FDMaskDL.h"
#include "FDMaskScreamDL.h"
#include "ElegySalemStatueDL.h"

TexturePtr sFlipbookTexturesEyes[] = {
    EyesOpenTex,
    EyesHalfTex,
    EyesClosedTex,
    EyesLookRightTex,
    EyesLookLeftTex,
    EyesLookDownTex,
    EyesLookUpTex,
    EyeWinceTex,
};

TexturePtr sMouthTextures[] = {
    MouthClosedTex,
    MouthHalfTex,
    MouthOpenTex,
    MouthSmileTex,
};

TexturePtr DekuTexturesEyes[] = {
    DekuEyesOpenTex,
    DekuEyesHalfTex,
    DekuEyesClosedTex,
    DekuEyesRightTex,
    DekuEyesLeftTex,
    DekuEyesDownTex,
    DekuEyesUpTex,
    DekuEyesWinceTex,
};

TexturePtr GoronTexturesEyes[] = {
    GoronEyesOpenTex,
    GoronEyesHalfTex,
    GoronEyesClosedTex,
    GoronEyesShockedTex,
};

TexturePtr ZoraTexturesEyes[] = {
    ZoraEyesOpenTex,
    ZoraEyesHalfTex,
    ZoraEyesClosedTex,
    ZoraEyesRightTex,
    ZoraEyesLeftTex,
    ZoraEyesDownTex,
    ZoraEyesUpTex,
    ZoraEyesWinceTex,
};

TexturePtr ZoraTexturesMouth[] = {
    ZoraMouthClosedTex,
    ZoraMouthHalfTex,
    ZoraMouthOpenTex,
    ZoraMouthSmileTex,
};

TexturePtr FDTexturesEyes[] = {
    FDEyesOpenTex,
    FDEyesHalfTex,
    FDEyesClosedTex,
    FDEyesRightTex,
    FDEyesLeftTex,
    FDEyesDownTex,
    FDEyesUpTex,
    FDEyesWinceTex,
};

TexturePtr FDTexturesMouth[] = {
    FDMouthClosedTex,
    FDMouthHalfTex,
    FDMouthOpenTex,
    FDMouthSmileTex,
};

PLAYERMODELMANAGER_CALLBACK_REGISTER_MODELS void registerHuman() {
    PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("mmsalem", PMM_MODEL_TYPE_CHILD);

    PlayerModelManager_setAuthor(h, "Jameriquiah");
    PlayerModelManager_setDisplayName(h, "Salem");
    PlayerModelManager_setEyesTextures(h, sFlipbookTexturesEyes);
    PlayerModelManager_setMouthTextures(h, sMouthTextures);
    PlayerModelManager_setDisplayList(h, PMM_DL_LFIST, gSalemHumanHandsClosedSkel_bone015_gLinkHumanLeftHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_RFIST, gSalemHumanHandsClosedSkel_bone018_gLinkHumanRightHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_LHAND_BOTTLE, gSalemHumanLeftHandBottleSkel_bone015_gLinkHumanLeftHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_OCARINA_TIME, gSalemHumanRightHandOcarinaSkel_bone015_gLinkHumanLeftHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_FPS_RHAND, FPSArmDL);
	PlayerModelManager_setDisplayList(h, PMM_DL_ELEGY_OF_EMPTINESS_SHELL_HUMAN, ElegySalemStatueDL);

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
    PlayerModelManager_setDisplayList(h, PMM_DL_LFIST, GoronLeftHandClosedDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_RFIST, GoronRightHandClosedDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_LHAND_BOTTLE, GoronLeftHandBottleDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_CURLED, GoronCurledDL);

    PlayerModelManager_setSkeleton(h, &gSalemGoronSkel);
}

PLAYERMODELMANAGER_CALLBACK_REGISTER_MODELS void registerZora() {
    PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("mmzsalem", PMM_MODEL_TYPE_ZORA);

    PlayerModelManager_setAuthor(h, "Jameriquiah");
    PlayerModelManager_setDisplayName(h, "Zora Salem");
	PlayerModelManager_setEyesTextures(h, ZoraTexturesEyes);
    PlayerModelManager_setMouthTextures(h, ZoraTexturesMouth);
    PlayerModelManager_setDisplayList(h, PMM_DL_LFIST, gSalemZoraHandsClosedSkel_bone015_gLinkZoraLeftHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_RFIST, gSalemZoraHandsClosedSkel_bone018_gLinkZoraRightHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_LHAND_BOTTLE, gSalemZoraLeftHandBottleSkel_bone015_gLinkZoraLeftHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_LHAND_GUITAR, gSalemZoraLeftHandGuitarSkel_bone015_gLinkZoraLeftHandLimb_mesh_layer_Opaque);
    PlayerModelManager_setDisplayList(h, PMM_DL_LFIN, ZoraLeftFinDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_RFIN, ZoraRightFinDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_LFIN_SWIM, ZoraLeftSwimmingFinDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_RFIN_SWIM, ZoraRightSwimmingFinDL);

    PlayerModelManager_setSkeleton(h, &gSalemZoraSkel);
}

PLAYERMODELMANAGER_CALLBACK_REGISTER_MODELS void registerFierceDeity() {
    PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("mmfdsalem", PMM_MODEL_TYPE_FIERCE_DEITY);

    PlayerModelManager_setAuthor(h, "Jameriquiah");
    PlayerModelManager_setDisplayName(h, "Fierce Deity Salem");
    PlayerModelManager_setEyesTextures(h, FDTexturesEyes);
    PlayerModelManager_setMouthTextures(h, FDTexturesMouth);
    PlayerModelManager_setDisplayList(h, PMM_DL_LFIST, FDLeftHandClosedDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_RFIST, FDRightHandClosedDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_LHAND_BOTTLE, FDLeftHandBottleDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_MASK_FIERCE_DEITY, FDMaskDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_MASK_FIERCE_DEITY_SCREAM, FDMaskScreamDL);

    PlayerModelManager_setSkeleton(h, &gSalemFierceDeitySkel);
}