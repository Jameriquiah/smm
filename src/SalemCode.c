#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "playermodelmanager_api.h"
#include "gSalemHumanSkel.h"
#include "LeftHandBottleDL.h"
#include "LeftHandClosedDL.h"
#include "RightHandClosedDL.h"
#include "FPSArmDL.h"
#include "gSalemDekuSkel.h"
#include "gSalemGoronSkel.h"
#include "gSalemZoraSkel.h"
#include "ZoraLeftHandBottleDL.h"
#include "ZoraLeftHandClosedDL.h"
#include "ZoraRightHandClosedDL.h"
#include "ZoraLeftHandGuitarDL.h"
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
    PlayerModelManager_setDisplayList(h, PMM_DL_LHAND_BOTTLE, LeftHandBottleDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_LFIST, LeftHandClosedDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_RFIST, RightHandClosedDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_FPS_RHAND, FPSArmDL);
	PlayerModelManager_setDisplayList(h, PMM_DL_ELEGY_OF_EMPTINESS_SHELL_HUMAN, ElegySalemStatueDL);

    PlayerModelManager_setSkeleton(h, &gSalemHumanSkel);
}

PLAYERMODELMANAGER_CALLBACK_REGISTER_MODELS void registerDeku() {
    PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("mmdsalem", PMM_MODEL_TYPE_DEKU);

    PlayerModelManager_setAuthor(h, "Jameriquiah");
    PlayerModelManager_setDisplayName(h, "Deku Salem");

    PlayerModelManager_setSkeleton(h, &gSalemDekuSkel);
}

PLAYERMODELMANAGER_CALLBACK_REGISTER_MODELS void registerGoron() {
    PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("mmgsalem", PMM_MODEL_TYPE_GORON);

    PlayerModelManager_setAuthor(h, "Jameriquiah");
    PlayerModelManager_setDisplayName(h, "Goron Salem");

    PlayerModelManager_setSkeleton(h, &gSalemGoronSkel);
}

PLAYERMODELMANAGER_CALLBACK_REGISTER_MODELS void registerZora() {
    PlayerModelManagerHandle h = PLAYERMODELMANAGER_REGISTER_MODEL("mmzsalem", PMM_MODEL_TYPE_ZORA);

    PlayerModelManager_setAuthor(h, "Jameriquiah");
    PlayerModelManager_setDisplayName(h, "Zora Salem");
	PlayerModelManager_setEyesTextures(h, ZoraTexturesEyes);
    PlayerModelManager_setMouthTextures(h, ZoraTexturesMouth);
    PlayerModelManager_setDisplayList(h, PMM_DL_LHAND_BOTTLE, ZoraLeftHandBottleDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_LFIST, ZoraLeftHandClosedDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_RFIST, ZoraRightHandClosedDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_LHAND_GUITAR, ZoraLeftHandGuitarDL);
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
    PlayerModelManager_setDisplayList(h, PMM_DL_LHAND_BOTTLE, FDLeftHandBottleDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_LFIST, FDLeftHandClosedDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_RFIST, FDRightHandClosedDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_MASK_FIERCE_DEITY, FDMaskDL);
    PlayerModelManager_setDisplayList(h, PMM_DL_MASK_FIERCE_DEITY_SCREAM, FDMaskScreamDL);

    PlayerModelManager_setSkeleton(h, &gSalemFierceDeitySkel);
}