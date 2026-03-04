#include "modding.h"
#include "global.h"
#include "playermodelmanager_api.h"
#include "gSalemFierceDeitySkel.h"
#include "gSalemFierceDeityHairSkel.h"
#include "SalemFierceDeityHairPhysics.h"

#define GRAVITY -5.0f
#define LIMB_MASS 1.0f
#define PINNED 1
#define NOT_PINNED 0
#define CONTROL_X_ROT_FIX -0x4000

typedef struct {
    Vec3f curr_vel;
    Vec3f prev_vel;
    Vec3s rot;
} PhysPlayer;

typedef struct {
    Vec3f curr_pos;
    Vec3f prev_pos;
    Vec3f curr_vel;
    Vec3f prev_vel;
    Vec3s default_jointPos;
    f32 mass;
    u8 pinned;
} PhysLimb;

typedef struct {
    PhysLimb* limb_a;
    PhysLimb* limb_b;
    f32 bone_length;
} PhysBone;

typedef enum SalemHairBodyPart {
    SALEM_HAIR_BODYPART_ROOT = 0,
    SALEM_HAIR_BODYPART_CONTROL,
    SALEM_HAIR_BODYPART_HAIR1,
    SALEM_HAIR_BODYPART_HAIR2,
    SALEM_HAIR_BODYPART_HAIR3,
    SALEM_HAIR_BODYPART_MAX
} SalemHairBodyPart;

typedef enum SalemHairBoneIndex {
    SALEM_HAIR_BONE_ROOT_CONTROL = 0,
    SALEM_HAIR_BONE_CONTROL_HAIR1,
    SALEM_HAIR_BONE_HAIR1_HAIR2,
    SALEM_HAIR_BONE_HAIR2_HAIR3,
    SALEM_HAIR_BONE_MAX
} SalemHairBoneIndex;

typedef struct {
    Actor actor;
    Vec3s jointTable[GSALEMFIERCEDEITYHAIRSKEL_NUM_LIMBS];
    Vec3f bodyPartsPos[SALEM_HAIR_BODYPART_MAX];
} SalemFierceDeityHairRig;

static PlayerModelManagerHandle sSalemFierceDeityModelHandle = 0;
static SalemFierceDeityHairRig sHairRig;
static PhysPlayer sPhysPlayer;
static PhysLimb sPhysLimbs[SALEM_HAIR_BODYPART_MAX];
static PhysBone sPhysBones[SALEM_HAIR_BONE_MAX];
static u8 sHairRigInitialized = 0;
static u8 sHairWasActive = 0;

static Vec3f sHeadGlobalPos = { 0.0f, 0.0f, 0.0f };
static Vec3s sHeadRotate = { 0, 0, 0 };
static MtxF sHeadMtx;
static u8 sHasHeadPose = 0;
static PlayState* sLastPlay = NULL;

static void SalemHair_ForceRootLock(void) {
    Vec3s zeroRot = { 0, 0, 0 };
    Vec3f zeroVel = { 0.0f, 0.0f, 0.0f };

    sPhysLimbs[SALEM_HAIR_BODYPART_ROOT].pinned = PINNED;
    Math_Vec3f_Copy(&sPhysLimbs[SALEM_HAIR_BODYPART_ROOT].curr_pos, &sHeadGlobalPos);
    Math_Vec3f_Copy(&sPhysLimbs[SALEM_HAIR_BODYPART_ROOT].prev_pos, &sHeadGlobalPos);
    Math_Vec3f_Copy(&sPhysLimbs[SALEM_HAIR_BODYPART_ROOT].curr_vel, &zeroVel);
    Math_Vec3f_Copy(&sPhysLimbs[SALEM_HAIR_BODYPART_ROOT].prev_vel, &zeroVel);
    Math_Vec3f_Copy(&sHairRig.bodyPartsPos[SALEM_HAIR_BODYPART_ROOT], &sHeadGlobalPos);

    Math_Vec3s_Copy(&sHairRig.jointTable[GSALEMFIERCEDEITYHAIRSKEL_BONE000_GLINKFIERCEDEITYHAIRROOTLIMB_POS_LIMB], &zeroRot);
    Math_Vec3s_Copy(&sHairRig.jointTable[GSALEMFIERCEDEITYHAIRSKEL_BONE000_GLINKFIERCEDEITYHAIRROOTLIMB_ROT_LIMB], &zeroRot);
}

static void SalemHair_Vec3sSum(Vec3s* l, Vec3s* r, Vec3s* dest) {
    dest->x = l->x + r->x;
    dest->y = l->y + r->y;
    dest->z = l->z + r->z;
}

static void SalemHair_Vec3sDiff(Vec3s* l, Vec3s* r, Vec3s* dest) {
    dest->x = l->x - r->x;
    dest->y = l->y - r->y;
    dest->z = l->z - r->z;
}

static void SalemHair_Vec3sScaleToVec3f(Vec3s* src, f32 scale, Vec3f* dest) {
    Vec3f temp = { 0.0f, 0.0f, 0.0f };

    Math_Vec3s_ToVec3f(&temp, src);
    Math_Vec3f_Scale(&temp, scale);
    Math_Vec3f_Copy(dest, &temp);
}

static void SalemHair_Vec3sRotateByX(Vec3s* src, s16 rotAngle, Vec3s* dest) {
    f32 sinValue = Math_SinS(rotAngle);
    f32 cosValue = Math_CosS(rotAngle);
    s16 x = src->x;
    s16 y = src->y;
    s16 z = src->z;

    dest->x = (s16)(x + 0.0f + 0.0f);
    dest->y = (s16)(0.0f + (y * cosValue) - (z * sinValue));
    dest->z = (s16)(0.0f + (y * sinValue) + (z * cosValue));
}

static void SalemHair_Vec3sRotateByY(Vec3s* src, s16 rotAngle, Vec3s* dest) {
    f32 sinValue = Math_SinS(rotAngle);
    f32 cosValue = Math_CosS(rotAngle);
    s16 x = src->x;
    s16 y = src->y;
    s16 z = src->z;

    dest->x = (s16)((x * cosValue) + 0.0f + (z * sinValue));
    dest->y = (s16)(0.0f + y + 0.0f);
    dest->z = (s16)((-x * sinValue) + 0.0f + (z * cosValue));
}

static void SalemHair_Vec3sRotateByZ(Vec3s* src, s16 rotAngle, Vec3s* dest) {
    f32 sinValue = Math_SinS(rotAngle);
    f32 cosValue = Math_CosS(rotAngle);
    s16 x = src->x;
    s16 y = src->y;
    s16 z = src->z;

    dest->x = (s16)((x * cosValue) - (y * sinValue) + 0.0f);
    dest->y = (s16)((x * sinValue) + (y * cosValue) + 0.0f);
    dest->z = (s16)(0.0f + 0.0f + z);
}

static void SalemHair_Vec3sRotate(Vec3s* src, Vec3s* rotAngle, Vec3s* dest) {
    SalemHair_Vec3sRotateByX(src, rotAngle->x, dest);
    SalemHair_Vec3sRotateByY(dest, rotAngle->y, dest);
    SalemHair_Vec3sRotateByZ(dest, rotAngle->z, dest);
}

static void SalemHair_Vec3fRotateByX(Vec3f* src, s16 rotAngle, Vec3f* dest) {
    f32 sinValue = Math_SinS(rotAngle);
    f32 cosValue = Math_CosS(rotAngle);
    f32 x = src->x;
    f32 y = src->y;
    f32 z = src->z;

    dest->x = (f32)(x + 0.0f + 0.0f);
    dest->y = (f32)(0.0f + (y * cosValue) - (z * sinValue));
    dest->z = (f32)(0.0f + (y * sinValue) + (z * cosValue));
}

static void SalemHair_Vec3fRotateByY(Vec3f* src, s16 rotAngle, Vec3f* dest) {
    f32 sinValue = Math_SinS(rotAngle);
    f32 cosValue = Math_CosS(rotAngle);
    f32 x = src->x;
    f32 y = src->y;
    f32 z = src->z;

    dest->x = (f32)((x * cosValue) + 0.0f + (z * sinValue));
    dest->y = (f32)(0.0f + y + 0.0f);
    dest->z = (f32)((-x * sinValue) + 0.0f + (z * cosValue));
}

static void SalemHair_Vec3fRotateByZ(Vec3f* src, s16 rotAngle, Vec3f* dest) {
    f32 sinValue = Math_SinS(rotAngle);
    f32 cosValue = Math_CosS(rotAngle);
    f32 x = src->x;
    f32 y = src->y;
    f32 z = src->z;

    dest->x = (f32)((x * cosValue) - (y * sinValue) + 0.0f);
    dest->y = (f32)((x * sinValue) + (y * cosValue) + 0.0f);
    dest->z = (f32)(0.0f + 0.0f + z);
}

static void SalemHair_Vec3fInverseRotate(Vec3f* src, Vec3s* rotAngle, Vec3f* dest) {
    SalemHair_Vec3fRotateByZ(src, -rotAngle->x, dest);
    SalemHair_Vec3fRotateByY(dest, -rotAngle->y, dest);
    SalemHair_Vec3fRotateByX(dest, -rotAngle->z, dest);
}

static s16 SalemHair_Vec3fPitch(Vec3f* b, Vec3f* a) {
    return Math_Atan2S_XY(b->z - a->z, b->y - a->y);
}

static s16 SalemHair_Vec3fRoll(Vec3f* a, Vec3f* b) {
    f32 xDist = b->x - a->x;
    f32 yDist = b->y - a->y;

    if (yDist <= 0.0f) {
        return 32768 + Math_Atan2S_XY(yDist, xDist);
    } else {
        return Math_Atan2S_XY(yDist, xDist);
    }
}

static void SalemHair_VerletInitPhysPlayer(PhysPlayer* targetPlayer, Player* player) {
    targetPlayer->curr_vel = player->actor.velocity;
    targetPlayer->prev_vel = player->actor.velocity;
    targetPlayer->rot = player->actor.world.rot;
}

static void SalemHair_VerletUpdatePhysPlayerVelocity(PhysPlayer* targetPlayer, Player* player) {
    targetPlayer->prev_vel = targetPlayer->curr_vel;
    targetPlayer->curr_vel = player->actor.velocity;
    targetPlayer->rot = player->actor.world.rot;
}

static void SalemHair_VerletCalcNetForce(PhysPlayer* targetPlayer, f32 gravity, Vec3f* netForce) {
    Vec3f velocityDiff = { 0.0f, 0.0f, 0.0f };

    netForce->x = 0.0f;
    netForce->y = gravity;
    netForce->z = 0.0f;
    Math_Vec3f_Diff(&targetPlayer->curr_vel, &targetPlayer->prev_vel, &velocityDiff);
    Math_Vec3f_Sum(netForce, &velocityDiff, netForce);
}

static void SalemHair_VerletInitLimb(PhysLimb* targetLimb, Vec3f pos, Vec3f vel, f32 mass, u8 pinStatus) {
    targetLimb->curr_pos = pos;
    targetLimb->prev_pos = pos;
    targetLimb->curr_vel = vel;
    targetLimb->prev_vel = vel;
    targetLimb->mass = mass;
    targetLimb->pinned = pinStatus;
}

static void SalemHair_VerletLimbUpdatePos(PhysLimb* targetLimb, Vec3f* applyForce, Vec3f* applyVel) {
    if (targetLimb->pinned == 0) {
        Vec3f newVelocity = { 0.0f, 0.0f, 0.0f };
        Vec3f opposingVel = { 0.0f, 0.0f, 0.0f };
        Vec3f accel = { 0.0f, 0.0f, 0.0f };
        Vec3f newPosition = { 0.0f, 0.0f, 0.0f };

        Math_Vec3f_Diff(&targetLimb->curr_pos, &targetLimb->prev_pos, &newVelocity);
        Math_Vec3f_ScaleAndStore(applyVel, -1.0f, &opposingVel);
        Math_Vec3f_Sum(&newVelocity, &opposingVel, &newVelocity);
        Math_Vec3f_Copy(&targetLimb->curr_vel, &newVelocity);

        targetLimb->prev_pos = targetLimb->curr_pos;

        Math_Vec3f_ScaleAndStore(applyForce, (1.0f / targetLimb->mass), &accel);

        Math_Vec3f_Copy(&newPosition, &targetLimb->curr_pos);
        Math_Vec3f_Sum(&newPosition, &newVelocity, &newPosition);
        Math_Vec3f_Sum(&newPosition, &accel, &newPosition);
        Math_Vec3f_Copy(&targetLimb->curr_pos, &newPosition);
    }
}

static void SalemHair_VerletInitBone(PhysBone* targetBone, PhysLimb* limbA, PhysLimb* limbB) {
    targetBone->limb_a = limbA;
    targetBone->limb_b = limbB;
    targetBone->bone_length = Math_Vec3f_DistXYZ(&limbA->curr_pos, &limbB->curr_pos);
}

static void SalemHair_VerletBoneConstraint(PhysBone* targetBone) {
    f32 currentDist = Math_Vec3f_DistXYZ(&targetBone->limb_a->curr_pos, &targetBone->limb_b->curr_pos);
    Vec3f direction = { 0.0f, 0.0f, 0.0f };
    Vec3f offset = { 0.0f, 0.0f, 0.0f };
    f32 diff;
    f32 percent;

    if (currentDist <= 0.0001f) {
        return;
    }

    diff = targetBone->bone_length - currentDist;
    percent = diff / currentDist;
    Math_Vec3f_Diff(&targetBone->limb_b->curr_pos, &targetBone->limb_a->curr_pos, &direction);

    if ((targetBone->limb_a->pinned == 0) && (targetBone->limb_b->pinned == 0)) {
        percent = percent / 1.0f;
        Math_Vec3f_ScaleAndStore(&direction, percent, &offset);
        Math_Vec3f_Sum(&targetBone->limb_b->curr_pos, &offset, &targetBone->limb_b->curr_pos);
    } else if ((targetBone->limb_a->pinned == 1) && (targetBone->limb_b->pinned == 0)) {
        percent = percent / 1.0f;
        Math_Vec3f_ScaleAndStore(&direction, percent, &offset);
        Math_Vec3f_Sum(&targetBone->limb_b->curr_pos, &offset, &targetBone->limb_b->curr_pos);
    }
}

static s32 SalemHair_IsModelActive(Player* player) {
    if (player == NULL) {
        return false;
    }

    if (player->transformation != PLAYER_FORM_FIERCE_DEITY) {
        return false;
    }

    if ((player->actor.draw == NULL) || (player->stateFlags2 & PLAYER_STATE2_20000000)) {
        return false;
    }

    if (sSalemFierceDeityModelHandle == 0) {
        return false;
    }

    return PlayerModelManager_isApplied(sSalemFierceDeityModelHandle);
}

static void SalemHair_SetDefaultBodyPartsPos(Player* player) {
    s32 i;
    Vec3s rootPos = { 0, 0, 0 };
    Vec3s rootRot = { 0, 0, 0 };
    static const Vec3s defaultJointPos[SALEM_HAIR_BODYPART_MAX] = {
        { 0, 0, 0 },
        { 0, 0, 0 },
        { -800, -250, 0 },
        { -1100, -300, 0 },
        { -1200, -300, 0 },
    };

    Math_Vec3f_Copy(&sHairRig.actor.world.pos, &sHeadGlobalPos);
    Math_Vec3s_Copy(&sHairRig.actor.shape.rot, &sHeadRotate);
    Math_Vec3s_Copy(&sHairRig.actor.world.rot, &sHeadRotate);

    Math_Vec3f_Copy(&sHairRig.bodyPartsPos[SALEM_HAIR_BODYPART_ROOT], &sHeadGlobalPos);
    {
        Vec3f zeroVel = { 0.0f, 0.0f, 0.0f };
        SalemHair_VerletInitLimb(&sPhysLimbs[SALEM_HAIR_BODYPART_ROOT], sHairRig.actor.world.pos, zeroVel, LIMB_MASS, PINNED);
    }
    Math_Vec3f_Copy(&sPhysLimbs[SALEM_HAIR_BODYPART_ROOT].prev_pos, &sPhysLimbs[SALEM_HAIR_BODYPART_ROOT].curr_pos);
    Math_Vec3f_Copy(&sPhysLimbs[SALEM_HAIR_BODYPART_ROOT].prev_vel, &sPhysLimbs[SALEM_HAIR_BODYPART_ROOT].curr_vel);

    rootPos.x = 0;
    rootPos.y = 0;
    rootPos.z = 0;
    Math_Vec3s_Copy(&sHairRig.jointTable[GSALEMFIERCEDEITYHAIRSKEL_BONE000_GLINKFIERCEDEITYHAIRROOTLIMB_POS_LIMB], &rootPos);
    Math_Vec3s_Copy(&sPhysLimbs[SALEM_HAIR_BODYPART_ROOT].default_jointPos, &rootPos);
    Math_Vec3s_Copy(&sHairRig.jointTable[GSALEMFIERCEDEITYHAIRSKEL_BONE000_GLINKFIERCEDEITYHAIRROOTLIMB_ROT_LIMB], &rootRot);

    for (i = SALEM_HAIR_BODYPART_CONTROL; i < SALEM_HAIR_BODYPART_MAX; i++) {
        Vec3s rotatedOffset = { 0, 0, 0 };
        Vec3f transform = { 0.0f, 0.0f, 0.0f };

        Math_Vec3s_Copy(&sPhysLimbs[i].default_jointPos, (Vec3s*)&defaultJointPos[i]);
        SalemHair_Vec3sRotate(&sPhysLimbs[i].default_jointPos, &sHeadRotate, &rotatedOffset);
        SalemHair_Vec3sScaleToVec3f(&rotatedOffset, sHairRig.actor.scale.x, &transform);
        Math_Vec3f_Sum(&sHairRig.bodyPartsPos[i - 1], &transform, &sHairRig.bodyPartsPos[i]);

        if (i == SALEM_HAIR_BODYPART_CONTROL) {
            Vec3f noVelocity = { 0.0f, 0.0f, 0.0f };
            SalemHair_VerletInitLimb(&sPhysLimbs[i], sHairRig.bodyPartsPos[i], noVelocity, LIMB_MASS, NOT_PINNED);
        } else {
            Vec3f noVelocity = { 0.0f, 0.0f, 0.0f };
            SalemHair_VerletInitLimb(&sPhysLimbs[i], sHairRig.bodyPartsPos[i], noVelocity, LIMB_MASS, NOT_PINNED);
        }
    }

    for (i = 0; i < SALEM_HAIR_BONE_MAX; i++) {
        SalemHair_VerletInitBone(&sPhysBones[i], &sPhysLimbs[i], &sPhysLimbs[i + 1]);
    }

    SalemHair_ForceRootLock();
}

static void SalemHair_UpdateBodyPartsPos(Player* player, Vec3f applyForce) {
    s32 i;
    Vec3s rootPos = { 0, 0, 0 };
    Vec3s rootRot = { 0, 0, 0 };

    Math_Vec3f_Copy(&sHairRig.actor.prevPos, &player->actor.prevPos);
    Math_Vec3f_Copy(&sHairRig.actor.world.pos, &sHeadGlobalPos);
    Math_Vec3s_Copy(&sHairRig.actor.shape.rot, &sHeadRotate);
    Math_Vec3s_Copy(&sHairRig.actor.world.rot, &sHeadRotate);

    Math_Vec3f_Copy(&sHairRig.bodyPartsPos[SALEM_HAIR_BODYPART_ROOT], &sHeadGlobalPos);
    Math_Vec3f_Copy(&sPhysLimbs[SALEM_HAIR_BODYPART_ROOT].curr_pos, &sHairRig.actor.world.pos);
    Math_Vec3f_Copy(&sPhysLimbs[SALEM_HAIR_BODYPART_ROOT].prev_pos, &sPhysLimbs[SALEM_HAIR_BODYPART_ROOT].curr_pos);
    sPhysLimbs[SALEM_HAIR_BODYPART_ROOT].curr_vel.x = 0.0f;
    sPhysLimbs[SALEM_HAIR_BODYPART_ROOT].curr_vel.y = 0.0f;
    sPhysLimbs[SALEM_HAIR_BODYPART_ROOT].curr_vel.z = 0.0f;
    Math_Vec3f_Copy(&sPhysLimbs[SALEM_HAIR_BODYPART_ROOT].prev_vel, &sPhysLimbs[SALEM_HAIR_BODYPART_ROOT].curr_vel);
    Math_Vec3s_Copy(&sHairRig.jointTable[GSALEMFIERCEDEITYHAIRSKEL_BONE000_GLINKFIERCEDEITYHAIRROOTLIMB_POS_LIMB], &rootPos);

    rootRot.x = 0;
    rootRot.y = 0;
    rootRot.z = 0;
    Math_Vec3s_Copy(&sHairRig.jointTable[GSALEMFIERCEDEITYHAIRSKEL_BONE000_GLINKFIERCEDEITYHAIRROOTLIMB_ROT_LIMB], &rootRot);

    for (i = SALEM_HAIR_BODYPART_CONTROL; i < SALEM_HAIR_BODYPART_MAX; i++) {
        if (sPhysLimbs[i].pinned == 1) {
            Vec3s rotatedOffset = { 0, 0, 0 };
            Vec3f transform = { 0.0f, 0.0f, 0.0f };

            Math_Vec3f_Copy(&sPhysLimbs[i].prev_pos, &sPhysLimbs[i].curr_pos);
            Math_Vec3f_Copy(&sPhysLimbs[i].prev_vel, &sPhysLimbs[i].curr_vel);

            SalemHair_Vec3sRotate(&sPhysLimbs[i].default_jointPos, &sHairRig.actor.shape.rot, &rotatedOffset);
            SalemHair_Vec3sScaleToVec3f(&rotatedOffset, sHairRig.actor.scale.x, &transform);

            Math_Vec3f_Sum(&sHairRig.bodyPartsPos[i - 1], &transform, &sHairRig.bodyPartsPos[i]);
            Math_Vec3f_Copy(&sPhysLimbs[i].curr_pos, &sHairRig.bodyPartsPos[i]);
            Math_Vec3f_Copy(&sPhysLimbs[i].curr_vel, &player->actor.velocity);
        } else {
            Vec3f playerVel = { 0.0f, 0.0f, 0.0f };

            Math_Vec3f_Copy(&playerVel, &player->actor.velocity);
            if (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                playerVel.y = 0.0f;
            }

            SalemHair_VerletLimbUpdatePos(&sPhysLimbs[i], &applyForce, &playerVel);
        }
    }

    for (i = 0; i < SALEM_HAIR_BONE_MAX; i++) {
        SalemHair_VerletBoneConstraint(&sPhysBones[i]);
    }

    SalemHair_ForceRootLock();
}

static void SalemHair_RotateJoints(void) {
    Vec3s offsetRotation = { 0, 0, 0 };
    s32 i;

    for (i = SALEM_HAIR_BONE_CONTROL_HAIR1; i < SALEM_HAIR_BONE_MAX; i++) {
        if ((sPhysBones[i].limb_a->pinned == 1) && (sPhysBones[i].limb_b->pinned == 1)) {
            continue;
        } else {
            Vec3f boneDirection = { 0.0f, 0.0f, 0.0f };
            Vec3f origin = { 0.0f, 0.0f, 0.0f };
            Vec3s actorRotWithOffset = { 0, 0, 0 };
            Vec3s currRotate = { 0, 0, 0 };

            Math_Vec3f_Diff(&sPhysBones[i].limb_b->curr_pos, &sPhysBones[i].limb_a->curr_pos, &boneDirection);
            Math_Vec3s_Copy(&actorRotWithOffset, &sHairRig.actor.shape.rot);
            SalemHair_Vec3fInverseRotate(&boneDirection, &actorRotWithOffset, &boneDirection);

            currRotate.x = SalemHair_Vec3fPitch(&boneDirection, &origin);
            currRotate.y = 0;
            currRotate.z = SalemHair_Vec3fRoll(&origin, &boneDirection);

            Vec3s applyRot = { 0, 0, 0 };

            SalemHair_Vec3sDiff(&currRotate, &offsetRotation, &applyRot);
            Math_Vec3s_Copy(&sHairRig.jointTable[i + 1], &applyRot);
            SalemHair_Vec3sSum(&offsetRotation, &applyRot, &offsetRotation);
        }
    }
}

static void SalemHair_DrawRig(PlayState* play) {
    static const Vec3f limbJointPos[5] = {
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f },
        { -800.0f, -250.0f, 0.0f },
        { -1100.0f, -300.0f, 0.0f },
        { -1200.0f, -300.0f, 0.0f },
    };
    GraphicsContext* __gfxCtx = play->state.gfxCtx;
    Mtx* limbMatrices = GRAPH_ALLOC(play->state.gfxCtx, 5 * sizeof(Mtx));
    Gfx* gfx;
    Vec3f pos = { 0.0f, 0.0f, 0.0f };
    Vec3s rot = { 0, 0, 0 };

    if (limbMatrices == NULL) {
        return;
    }

    gSPSegment(POLY_OPA_DISP++, 0x0D, limbMatrices);

    Matrix_Push();
    Matrix_Put(&sHeadMtx);

    rot.x = 0;
    rot.y = 0;
    rot.z = 0;
    Matrix_TranslateRotateZYX(&pos, &rot);
    gfx = POLY_OPA_DISP;
    gSPMatrix(&gfx[0], Matrix_ToMtx(&limbMatrices[0]), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(&gfx[1], gSalemFierceDeityHairSkel_bone000_gLinkFierceDeityHairRootLimb_mesh_layer_Opaque);
    POLY_OPA_DISP = &gfx[2];

    Matrix_Push();
    Math_Vec3f_Copy(&pos, (Vec3f*)&limbJointPos[1]);
    Math_Vec3s_Copy(&rot, &sHairRig.jointTable[GSALEMFIERCEDEITYHAIRSKEL_BONE001_GLINKFIERCEDEITYHAIRCONTROLLIMB_LIMB]);
    rot.x += CONTROL_X_ROT_FIX;
    Matrix_TranslateRotateZYX(&pos, &rot);
    gfx = POLY_OPA_DISP;
    gSPMatrix(&gfx[0], Matrix_ToMtx(&limbMatrices[1]), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(&gfx[1], gSalemFierceDeityHairSkel_bone001_gLinkFierceDeityHairControlLimb_mesh_layer_Opaque);
    POLY_OPA_DISP = &gfx[2];

    Matrix_Push();
    Math_Vec3f_Copy(&pos, (Vec3f*)&limbJointPos[2]);
    Math_Vec3s_Copy(&rot, &sHairRig.jointTable[GSALEMFIERCEDEITYHAIRSKEL_BONE002_GLINKFIERCEDEITYHAIR1LIMB_LIMB]);
    Matrix_TranslateRotateZYX(&pos, &rot);
    gfx = POLY_OPA_DISP;
    gSPMatrix(&gfx[0], Matrix_ToMtx(&limbMatrices[2]), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(&gfx[1], gSalemFierceDeityHairSkel_bone002_gLinkFierceDeityHair1Limb_mesh_layer_Opaque);
    POLY_OPA_DISP = &gfx[2];

    Matrix_Push();
    Math_Vec3f_Copy(&pos, (Vec3f*)&limbJointPos[3]);
    Math_Vec3s_Copy(&rot, &sHairRig.jointTable[GSALEMFIERCEDEITYHAIRSKEL_BONE003_GLINKFIERCEDEITYHAIR2LIMB_LIMB]);
    Matrix_TranslateRotateZYX(&pos, &rot);
    gfx = POLY_OPA_DISP;
    gSPMatrix(&gfx[0], Matrix_ToMtx(&limbMatrices[3]), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(&gfx[1], gSalemFierceDeityHairSkel_bone003_gLinkFierceDeityHair2Limb_mesh_layer_Opaque);
    POLY_OPA_DISP = &gfx[2];

    Matrix_Push();
    Math_Vec3f_Copy(&pos, (Vec3f*)&limbJointPos[4]);
    Math_Vec3s_Copy(&rot, &sHairRig.jointTable[GSALEMFIERCEDEITYHAIRSKEL_BONE004_GLINKFIERCEDEITYHAIR3LIMB_LIMB]);
    Matrix_TranslateRotateZYX(&pos, &rot);
    gfx = POLY_OPA_DISP;
    gSPMatrix(&gfx[0], Matrix_ToMtx(&limbMatrices[4]), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(&gfx[1], gSalemFierceDeityHairSkel_bone004_gLinkFierceDeityHair3Limb_mesh_layer_Opaque);
    POLY_OPA_DISP = &gfx[2];
    Matrix_Pop();

    Matrix_Pop();
    Matrix_Pop();
    Matrix_Pop();
    Matrix_Pop();
}

static void SalemHair_InitRig(PlayState* play, Player* player) {
    Vec3s zeroRot = { 0, 0, 0 };

    (void)play;
    Actor_SetScale(&sHairRig.actor, 0.01f);
    Math_Vec3s_Copy(&sHairRig.jointTable[GSALEMFIERCEDEITYHAIRSKEL_BONE000_GLINKFIERCEDEITYHAIRROOTLIMB_POS_LIMB], &zeroRot);
    Math_Vec3s_Copy(&sHairRig.jointTable[GSALEMFIERCEDEITYHAIRSKEL_BONE000_GLINKFIERCEDEITYHAIRROOTLIMB_ROT_LIMB], &zeroRot);
    Math_Vec3s_Copy(&sHairRig.jointTable[GSALEMFIERCEDEITYHAIRSKEL_BONE001_GLINKFIERCEDEITYHAIRCONTROLLIMB_LIMB], &zeroRot);
    Math_Vec3s_Copy(&sHairRig.jointTable[GSALEMFIERCEDEITYHAIRSKEL_BONE002_GLINKFIERCEDEITYHAIR1LIMB_LIMB], &zeroRot);
    Math_Vec3s_Copy(&sHairRig.jointTable[GSALEMFIERCEDEITYHAIRSKEL_BONE003_GLINKFIERCEDEITYHAIR2LIMB_LIMB], &zeroRot);
    Math_Vec3s_Copy(&sHairRig.jointTable[GSALEMFIERCEDEITYHAIRSKEL_BONE004_GLINKFIERCEDEITYHAIR3LIMB_LIMB], &zeroRot);
    SalemHair_VerletInitPhysPlayer(&sPhysPlayer, player);
    SalemHair_SetDefaultBodyPartsPos(player);
    sHairRigInitialized = 1;
}

RECOMP_HOOK("Player_PostLimbDrawGameplay")
void SalemHair_on_Player_PostLimbDrawGameplay(PlayState* play, s32 limbIndex, Gfx** dList1, Gfx** dList2, Vec3s* rot,
                                              Actor* actor) {
    Player* player = GET_PLAYER(play);
    MtxF* mtx;

    if (sLastPlay != play) {
        sLastPlay = play;
        sHasHeadPose = 0;
        sHairWasActive = 0;
        sHairRigInitialized = 0;
    }

    if (actor != &player->actor) {
        return;
    }

    if (!SalemHair_IsModelActive(player)) {
        return;
    }

    if (limbIndex != PLAYER_LIMB_HEAD) {
        return;
    }

    sLastPlay = play;
    OPEN_DISPS(play->state.gfxCtx);
    Matrix_Push();
    mtx = Matrix_GetCurrent();
    sHeadGlobalPos.x = mtx->xw;
    sHeadGlobalPos.y = mtx->yw;
    sHeadGlobalPos.z = mtx->zw;
    Matrix_MtxFCopy(&sHeadMtx, mtx);
    Matrix_MtxFToYXZRot(mtx, &sHeadRotate, 1);
    Matrix_Pop();
    CLOSE_DISPS(play->state.gfxCtx);
    sHasHeadPose = 1;
}

RECOMP_HOOK_RETURN("Player_Draw")
void SalemHair_on_Player_Draw_Return(void) {
    Player* player;
    Vec3f netForce = { 0.0f, 0.0f, 0.0f };

    if ((sLastPlay == NULL) || !sHasHeadPose) {
        sHairWasActive = 0;
        sHasHeadPose = 0;
        return;
    }

    player = GET_PLAYER(sLastPlay);
    if (!SalemHair_IsModelActive(player)) {
        sHairWasActive = 0;
        sHasHeadPose = 0;
        return;
    }

    if (!sHairRigInitialized) {
        SalemHair_InitRig(sLastPlay, player);
    }

    if (!sHairWasActive) {
        SalemHair_VerletInitPhysPlayer(&sPhysPlayer, player);
        SalemHair_SetDefaultBodyPartsPos(player);
        sHairWasActive = 1;
    }

    SalemHair_VerletUpdatePhysPlayerVelocity(&sPhysPlayer, player);
    SalemHair_VerletCalcNetForce(&sPhysPlayer, GRAVITY, &netForce);
    SalemHair_UpdateBodyPartsPos(player, netForce);
    SalemHair_RotateJoints();
    SalemHair_ForceRootLock();

    OPEN_DISPS(sLastPlay->state.gfxCtx);
    Gfx_SetupDL25_Opa(sLastPlay->state.gfxCtx);
    func_80122868(sLastPlay, player);
    SalemHair_DrawRig(sLastPlay);
    if (player->invincibilityTimer > 0 || gSaveContext.jinxTimer != 0) {
        POLY_OPA_DISP = Play_SetFog(sLastPlay, POLY_OPA_DISP);
    }
    CLOSE_DISPS(sLastPlay->state.gfxCtx);
    sHasHeadPose = 0;
}

void SalemFierceDeityHairPhysics_SetModelHandle(PlayerModelManagerHandle handle) {
    sSalemFierceDeityModelHandle = handle;
}
