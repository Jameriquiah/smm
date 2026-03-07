#include "SalemFierceDeityHairPhysics.h"

#include <math.h>

#include "gSalemFierceDeityHairSkel.h"
#include "global.h"
#include "modding.h"
#include "sys_matrix.h"

#define SALEM_HAIR_NODE_COUNT 4
#define SALEM_HAIR_SEGMENT_COUNT 3
#define SALEM_HAIR_CONSTRAINT_ITERS 5

#define SALEM_HAIR_DAMPING 0.95f
#define SALEM_HAIR_ROOT_INERTIA 0.06f
#define SALEM_HAIR_GRAVITY -48.0f
#define SALEM_HAIR_MAX_VELOCITY 180.0f
#define SALEM_HAIR_TELEPORT_RESET 500.0f
#define SALEM_HAIR_MAX_LOCAL_ROT 0x5000
#define SALEM_HAIR_PLAYER_VEL_FORCE_FWD 9.0f
#define SALEM_HAIR_PLAYER_ACCEL_FORCE_FWD 15.0f
#define SALEM_HAIR_PLAYER_FORCE_SMOOTH 0.52f
#define SALEM_HAIR_PLAYER_FORCE_SMOOTH_MOVING 0.30f
#define SALEM_HAIR_PLAYER_FORCE_MAX 90.0f
#define SALEM_HAIR_PLAYER_SPEED_MOVING 0.10f
#define SALEM_HAIR_BONE001_ANCHOR_MOVING 0.20f
#define SALEM_HAIR_HEAD_YAW_FORCE 0.0028f
#define SALEM_HAIR_HEAD_YAW_FORCE_MAX 70.0f
#define SALEM_HAIR_BONE001_ANCHOR 0.40f
#define SALEM_HAIR_IDLE_SWAY_SPEED 0.08f
#define SALEM_HAIR_IDLE_SWAY_SIDE 1.0f
#define SALEM_HAIR_IDLE_SWAY_FWD 0.4f

static bool sSalemHairEnabled;
static bool sSalemHairHasHeadMtx;
static bool sSalemHairChainInitialized;
static bool sSalemHairBaseReady;
static u32 sSalemHairLastUpdateFrame;
static u32 sSalemHairHeadMtxFrame;
static MtxF sSalemHairHeadMtx;
static Vec3s sSalemHairHeadRot;
static s16 sSalemHairPrevHeadYaw;
static Vec3f sSalemHairPrevRoot;
static Vec3f sSalemHairPrevPlayerVel;
static Vec3f sSalemHairPrevPlayerPos;
static Vec3f sSalemHairMoveForce;
static bool sSalemHairPrevPlayerPosValid;
static Vec3f sSalemHairCurr[SALEM_HAIR_NODE_COUNT];
static Vec3f sSalemHairPrev[SALEM_HAIR_NODE_COUNT];
static f32 sSalemHairSegmentLength[SALEM_HAIR_SEGMENT_COUNT];
static Vec3f sSalemHairBaseSegmentLocal[SALEM_HAIR_SEGMENT_COUNT];
static Vec3s sSalemHairBaseAbsRot[SALEM_HAIR_SEGMENT_COUNT];
static Vec3s sSalemHairJointTable[GSALEMFIERCEDEITYHAIRSKEL_NUM_LIMBS];
static PlayerModelManagerHandle sSalemHairModelHandle;

static const Vec3f sSalemHairRawSegmentLocal[SALEM_HAIR_SEGMENT_COUNT] = {
    { -509.0f, 666.0f, 0.0f },
    { -300.0f, 1100.0f, 0.0f },
    { -300.0f, 1200.0f, 0.0f },
};

static const s32 sSalemHairJointIndex[SALEM_HAIR_SEGMENT_COUNT] = {
    GSALEMFIERCEDEITYHAIRSKEL_BONE001_GLINKFIERCEDEITYHAIRCONTROLLIMB_LIMB,
    GSALEMFIERCEDEITYHAIRSKEL_BONE002_GLINKFIERCEDEITYHAIR1LIMB_LIMB,
    GSALEMFIERCEDEITYHAIRSKEL_BONE003_GLINKFIERCEDEITYHAIR2LIMB_LIMB,
};

static f32 SalemHair_Length(Vec3f v) {
    return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

static Vec3f SalemHair_Sub(Vec3f a, Vec3f b) {
    Vec3f out;
    out.x = a.x - b.x;
    out.y = a.y - b.y;
    out.z = a.z - b.z;
    return out;
}

static Vec3f SalemHair_Add(Vec3f a, Vec3f b) {
    Vec3f out;
    out.x = a.x + b.x;
    out.y = a.y + b.y;
    out.z = a.z + b.z;
    return out;
}

static Vec3f SalemHair_Scale(Vec3f v, f32 s) {
    Vec3f out;
    out.x = v.x * s;
    out.y = v.y * s;
    out.z = v.z * s;
    return out;
}

static Vec3f SalemHair_ClampVecLength(Vec3f v, f32 maxLen) {
    f32 len = SalemHair_Length(v);

    if ((len <= maxLen) || (len <= 0.0001f)) {
        return v;
    }

    return SalemHair_Scale(v, maxLen / len);
}

static void SalemHair_ResetJointTable(void) {
    s32 i;

    for (i = 0; i < GSALEMFIERCEDEITYHAIRSKEL_NUM_LIMBS; i++) {
        sSalemHairJointTable[i].x = 0;
        sSalemHairJointTable[i].y = 0;
        sSalemHairJointTable[i].z = 0;
    }
}

static void SalemHair_RotateVecByX(Vec3f* src, s16 angle, Vec3f* dst) {
    f32 s = Math_SinS(angle);
    f32 c = Math_CosS(angle);

    dst->x = src->x;
    dst->y = (src->y * c) - (src->z * s);
    dst->z = (src->y * s) + (src->z * c);
}

static void SalemHair_RotateVecByY(Vec3f* src, s16 angle, Vec3f* dst) {
    f32 s = Math_SinS(angle);
    f32 c = Math_CosS(angle);

    dst->x = (src->x * c) + (src->z * s);
    dst->y = src->y;
    dst->z = (-src->x * s) + (src->z * c);
}

static void SalemHair_RotateVecByZ(Vec3f* src, s16 angle, Vec3f* dst) {
    f32 s = Math_SinS(angle);
    f32 c = Math_CosS(angle);

    dst->x = (src->x * c) - (src->y * s);
    dst->y = (src->x * s) + (src->y * c);
    dst->z = src->z;
}

static void SalemHair_RotateVecByVec3s(Vec3f* src, Vec3s* rot, Vec3f* dst) {
    SalemHair_RotateVecByX(src, rot->x, dst);
    SalemHair_RotateVecByY(dst, rot->y, dst);
    SalemHair_RotateVecByZ(dst, rot->z, dst);
}

static s16 SalemHair_DirToPitch(Vec3f dir) {
    return Math_Atan2S_XY(dir.z, dir.y);
}

static s16 SalemHair_DirToRoll(Vec3f dir) {
    if (dir.y <= 0.0f) {
        return 32768 + Math_Atan2S_XY(dir.y, dir.x);
    }
    return Math_Atan2S_XY(dir.y, dir.x);
}

static s16 SalemHair_ClampBinang(s16 value, s16 min, s16 max) {
    if (value < min) {
        return min;
    }
    if (value > max) {
        return max;
    }
    return value;
}

static void SalemHair_InverseRotateVecByVec3s(Vec3f* src, Vec3s* rot, Vec3f* dst) {
    SalemHair_RotateVecByZ(src, -rot->z, dst);
    SalemHair_RotateVecByY(dst, -rot->y, dst);
    SalemHair_RotateVecByX(dst, -rot->x, dst);
}

static void SalemHair_BuildBasePoseData(void) {
    s32 i;

    if (sSalemHairBaseReady) {
        return;
    }

    for (i = 0; i < SALEM_HAIR_SEGMENT_COUNT; i++) {
        Vec3f rotated = sSalemHairRawSegmentLocal[i];
        Vec3f origin = { 0.0f, 0.0f, 0.0f };
        Vec3f dir;

        sSalemHairBaseSegmentLocal[i] = rotated;

        dir = SalemHair_Sub(rotated, origin);
        sSalemHairBaseAbsRot[i].x = SalemHair_DirToPitch(dir);
        sSalemHairBaseAbsRot[i].y = 0;
        sSalemHairBaseAbsRot[i].z = SalemHair_DirToRoll(dir);
    }

    sSalemHairBaseReady = true;
}

static void SalemHair_ResetChainState(void) {
    Vec3f zero = { 0.0f, 0.0f, 0.0f };

    sSalemHairHasHeadMtx = false;
    sSalemHairHeadMtxFrame = 0xFFFFFFFF;
    sSalemHairChainInitialized = false;
    sSalemHairLastUpdateFrame = 0xFFFFFFFF;
    sSalemHairPrevHeadYaw = 0;
    sSalemHairPrevPlayerVel = zero;
    sSalemHairPrevPlayerPos = zero;
    sSalemHairMoveForce = zero;
    sSalemHairPrevPlayerPosValid = false;
}

static bool SalemHair_ShouldDraw(Player* player) {
    if (!sSalemHairEnabled || (player == NULL)) {
        return false;
    }

    if (player->transformation != PLAYER_FORM_FIERCE_DEITY) {
        return false;
    }

    if ((player->actor.draw == NULL) || (player->stateFlags2 & PLAYER_STATE2_20000000)) {
        return false;
    }

    if (sSalemHairModelHandle == 0) {
        return false;
    }

    return PlayerModelManager_isApplied(sSalemHairModelHandle);
}

static Vec3f SalemHair_GetPlayerVelocity(Player* player) {
    Vec3f vel;

    if (!sSalemHairPrevPlayerPosValid) {
        sSalemHairPrevPlayerPos = player->actor.world.pos;
        sSalemHairPrevPlayerPosValid = true;
    }

    vel = SalemHair_Sub(player->actor.world.pos, sSalemHairPrevPlayerPos);
    sSalemHairPrevPlayerPos = player->actor.world.pos;

    if (((fabsf(vel.x) + fabsf(vel.z)) < 0.01f) && (fabsf(player->actor.speed) > 0.01f)) {
        vel.x = Math_SinS(player->actor.world.rot.y) * player->actor.speed;
        vel.z = Math_CosS(player->actor.world.rot.y) * player->actor.speed;
    }

    vel.y = player->actor.velocity.y;
    return vel;
}

static void SalemHair_InitChainFromHead(Vec3f playerVel) {
    Vec3f root = { sSalemHairHeadMtx.xw, sSalemHairHeadMtx.yw, sSalemHairHeadMtx.zw };
    Vec3f cursor = root;
    s32 i;

    sSalemHairCurr[0] = root;
    sSalemHairPrev[0] = root;

    for (i = 0; i < SALEM_HAIR_SEGMENT_COUNT; i++) {
        Vec3f worldSeg;

        SalemHair_RotateVecByVec3s(&sSalemHairBaseSegmentLocal[i], &sSalemHairHeadRot, &worldSeg);
        cursor = SalemHair_Add(cursor, worldSeg);
        sSalemHairCurr[i + 1] = cursor;
        sSalemHairPrev[i + 1] = cursor;
        sSalemHairSegmentLength[i] = SalemHair_Length(worldSeg);
    }

    sSalemHairPrevRoot = root;
    sSalemHairPrevHeadYaw = sSalemHairHeadRot.y;
    sSalemHairPrevPlayerVel = playerVel;
    sSalemHairMoveForce.x = 0.0f;
    sSalemHairMoveForce.y = 0.0f;
    sSalemHairMoveForce.z = 0.0f;
    sSalemHairChainInitialized = true;
}

static void SalemHair_ConstrainSegment(s32 a, s32 b, f32 length, bool aPinned) {
    Vec3f delta = SalemHair_Sub(sSalemHairCurr[b], sSalemHairCurr[a]);
    f32 dist = SalemHair_Length(delta);
    f32 diff;

    if (dist <= 0.0001f) {
        return;
    }

    diff = (dist - length) / dist;
    if (aPinned) {
        sSalemHairCurr[b] = SalemHair_Sub(sSalemHairCurr[b], SalemHair_Scale(delta, diff));
    } else {
        Vec3f half = SalemHair_Scale(delta, diff * 0.5f);
        sSalemHairCurr[a] = SalemHair_Add(sSalemHairCurr[a], half);
        sSalemHairCurr[b] = SalemHair_Sub(sSalemHairCurr[b], half);
    }
}

static void SalemHair_BuildJointRotationsFromChain(void) {
    Vec3s accumulated = { 0, 0, 0 };
    s32 i;

    for (i = 0; i < SALEM_HAIR_SEGMENT_COUNT; i++) {
        Vec3f worldDir = SalemHair_Sub(sSalemHairCurr[i + 1], sSalemHairCurr[i]);
        Vec3f localDir;
        Vec3s absRot;
        Vec3s deltaAbs;
        Vec3s localRot;

        SalemHair_InverseRotateVecByVec3s(&worldDir, &sSalemHairHeadRot, &localDir);

        absRot.x = SalemHair_DirToPitch(localDir);
        absRot.y = 0;
        absRot.z = SalemHair_DirToRoll(localDir);

        deltaAbs.x = BINANG_SUB(absRot.x, sSalemHairBaseAbsRot[i].x);
        deltaAbs.y = 0;
        deltaAbs.z = BINANG_SUB(absRot.z, sSalemHairBaseAbsRot[i].z);

        localRot.x = BINANG_SUB(deltaAbs.x, accumulated.x);
        localRot.y = 0;
        localRot.z = BINANG_SUB(deltaAbs.z, accumulated.z);

        localRot.x = SalemHair_ClampBinang(localRot.x, -SALEM_HAIR_MAX_LOCAL_ROT, SALEM_HAIR_MAX_LOCAL_ROT);
        localRot.z = SalemHair_ClampBinang(localRot.z, -SALEM_HAIR_MAX_LOCAL_ROT, SALEM_HAIR_MAX_LOCAL_ROT);

        sSalemHairJointTable[sSalemHairJointIndex[i]] = localRot;

        accumulated.x += localRot.x;
        accumulated.y += localRot.y;
        accumulated.z += localRot.z;
    }
}

static void SalemHair_UpdatePhysicsOncePerFrame(u32 gameplayFrames, Vec3f playerVel, f32 playerSpeed) {
    Vec3f root;
    Vec3f rootDelta;
    Vec3f playerAccel;
    Vec3f targetMoveForce;
    Vec3f headForward;
    Vec3f headRight;
    Vec3f baseTargetOffset;
    Vec3f baseTarget;
    Vec3f headYawForce;
    f32 bone001Anchor;
    bool isMoving;
    s16 headYawDelta;
    s32 i;
    s32 iter;

    if (sSalemHairLastUpdateFrame == gameplayFrames) {
        return;
    }

    SalemHair_BuildBasePoseData();

    if (!sSalemHairChainInitialized) {
        SalemHair_InitChainFromHead(playerVel);
        SalemHair_BuildJointRotationsFromChain();
        sSalemHairLastUpdateFrame = gameplayFrames;
        return;
    }

    root.x = sSalemHairHeadMtx.xw;
    root.y = sSalemHairHeadMtx.yw;
    root.z = sSalemHairHeadMtx.zw;

    rootDelta = SalemHair_Sub(root, sSalemHairPrevRoot);
    if (SalemHair_Length(rootDelta) > SALEM_HAIR_TELEPORT_RESET) {
        SalemHair_InitChainFromHead(playerVel);
        SalemHair_BuildJointRotationsFromChain();
        sSalemHairLastUpdateFrame = gameplayFrames;
        return;
    }

    playerAccel = SalemHair_Sub(playerVel, sSalemHairPrevPlayerVel);
    playerAccel = SalemHair_ClampVecLength(playerAccel, 8.0f);
    sSalemHairPrevPlayerVel = playerVel;

    headForward.x = Math_SinS(sSalemHairHeadRot.y);
    headForward.y = 0.0f;
    headForward.z = Math_CosS(sSalemHairHeadRot.y);
    headRight.x = Math_CosS(sSalemHairHeadRot.y);
    headRight.y = 0.0f;
    headRight.z = -Math_SinS(sSalemHairHeadRot.y);

    targetMoveForce = SalemHair_Add(SalemHair_Scale(playerVel, -SALEM_HAIR_PLAYER_VEL_FORCE_FWD),
                                    SalemHair_Scale(playerAccel, -SALEM_HAIR_PLAYER_ACCEL_FORCE_FWD));
    targetMoveForce.y = 0.0f;
    targetMoveForce = SalemHair_ClampVecLength(targetMoveForce, SALEM_HAIR_PLAYER_FORCE_MAX);

    isMoving = fabsf(playerSpeed) > SALEM_HAIR_PLAYER_SPEED_MOVING;
    sSalemHairMoveForce =
        SalemHair_Add(SalemHair_Scale(sSalemHairMoveForce,
                                      isMoving ? SALEM_HAIR_PLAYER_FORCE_SMOOTH_MOVING : SALEM_HAIR_PLAYER_FORCE_SMOOTH),
                      SalemHair_Scale(targetMoveForce,
                                      isMoving ? (1.0f - SALEM_HAIR_PLAYER_FORCE_SMOOTH_MOVING)
                                               : (1.0f - SALEM_HAIR_PLAYER_FORCE_SMOOTH)));
    bone001Anchor = isMoving ? SALEM_HAIR_BONE001_ANCHOR_MOVING : SALEM_HAIR_BONE001_ANCHOR;

    headYawDelta = BINANG_SUB(sSalemHairHeadRot.y, sSalemHairPrevHeadYaw);
    sSalemHairPrevHeadYaw = sSalemHairHeadRot.y;
    headYawForce.x = Math_CosS(sSalemHairHeadRot.y) * (-((f32)headYawDelta) * SALEM_HAIR_HEAD_YAW_FORCE);
    headYawForce.y = 0.0f;
    headYawForce.z = -Math_SinS(sSalemHairHeadRot.y) * (-((f32)headYawDelta) * SALEM_HAIR_HEAD_YAW_FORCE);
    headYawForce = SalemHair_ClampVecLength(headYawForce, SALEM_HAIR_HEAD_YAW_FORCE_MAX);
    SalemHair_RotateVecByVec3s(&sSalemHairBaseSegmentLocal[0], &sSalemHairHeadRot, &baseTargetOffset);
    baseTarget = SalemHair_Add(root, baseTargetOffset);

    for (i = 1; i < SALEM_HAIR_NODE_COUNT; i++) {
        f32 chainT = (f32)(i - 1) / (f32)(SALEM_HAIR_NODE_COUNT - 2);
        f32 rootFollow = (i == 1) ? 0.0f : (SALEM_HAIR_ROOT_INERTIA * (0.35f + (0.65f * chainT)));

        if (isMoving) {
            rootFollow = 0.0f;
        }
        Vec3f velocity = SalemHair_Sub(sSalemHairCurr[i], sSalemHairPrev[i]);
        Vec3f moveBias = SalemHair_Scale(sSalemHairMoveForce, 0.10f + (0.70f * chainT));
        Vec3f yawBias = SalemHair_Scale(headYawForce, 0.02f + (0.08f * chainT));
        f32 swayPhase = ((f32)gameplayFrames * SALEM_HAIR_IDLE_SWAY_SPEED) + ((f32)i * 0.85f);
        Vec3f swayBias = SalemHair_Add(
            SalemHair_Scale(headRight, sinf(swayPhase) * (SALEM_HAIR_IDLE_SWAY_SIDE * chainT)),
            SalemHair_Scale(headForward, cosf(swayPhase * 0.65f) * (SALEM_HAIR_IDLE_SWAY_FWD * chainT)));

        if (isMoving) {
            yawBias = SalemHair_Scale(yawBias, 0.20f);
            swayBias.x = 0.0f;
            swayBias.y = 0.0f;
            swayBias.z = 0.0f;
        }

        velocity = SalemHair_ClampVecLength(velocity, SALEM_HAIR_MAX_VELOCITY);

        if (i == 1) {
            moveBias = SalemHair_Scale(sSalemHairMoveForce, 0.12f);
            yawBias = SalemHair_Scale(headYawForce, isMoving ? 0.006f : 0.03f);
        }

        sSalemHairPrev[i] = sSalemHairCurr[i];
        sSalemHairCurr[i] = SalemHair_Add(sSalemHairCurr[i], SalemHair_Scale(velocity, SALEM_HAIR_DAMPING));
        sSalemHairCurr[i] = SalemHair_Add(sSalemHairCurr[i], SalemHair_Scale(rootDelta, rootFollow));
        sSalemHairCurr[i] = SalemHair_Add(sSalemHairCurr[i], moveBias);
        sSalemHairCurr[i] = SalemHair_Add(sSalemHairCurr[i], yawBias);
        sSalemHairCurr[i] = SalemHair_Add(sSalemHairCurr[i], swayBias);
        sSalemHairCurr[i].y += SALEM_HAIR_GRAVITY * (1.0f + (0.12f * i));

        if (i == 1) {
            sSalemHairCurr[1] = SalemHair_Add(SalemHair_Scale(sSalemHairCurr[1], 1.0f - bone001Anchor),
                                              SalemHair_Scale(baseTarget, bone001Anchor));
        }
    }

    for (iter = 0; iter < SALEM_HAIR_CONSTRAINT_ITERS; iter++) {
        sSalemHairCurr[0] = root;
        SalemHair_ConstrainSegment(0, 1, sSalemHairSegmentLength[0], true);
        SalemHair_ConstrainSegment(1, 2, sSalemHairSegmentLength[1], false);
        SalemHair_ConstrainSegment(2, 3, sSalemHairSegmentLength[2], false);
    }

    sSalemHairCurr[0] = root;
    sSalemHairPrev[0] = root;
    sSalemHairPrevRoot = root;

    SalemHair_BuildJointRotationsFromChain();
    sSalemHairLastUpdateFrame = gameplayFrames;
}

static void SalemHair_DrawRig(PlayState* play) {
    Matrix_Push();
    Matrix_Put(&sSalemHairHeadMtx);
    SkelAnime_DrawFlexOpa(play, gSalemFierceDeityHairSkel.sh.segment, sSalemHairJointTable,
                          gSalemFierceDeityHairSkel.dListCount, NULL, NULL, NULL);
    Matrix_Pop();
}

void SalemFierceDeityHairPhysics_Init(void) {
    sSalemHairEnabled = false;
    sSalemHairModelHandle = 0;
    sSalemHairBaseReady = false;
    SalemHair_ResetJointTable();
    SalemHair_ResetChainState();
}

void SalemFierceDeityHairPhysics_SetEnabled(bool enabled) {
    sSalemHairEnabled = enabled;
    if (!enabled) {
        SalemHair_ResetJointTable();
        SalemHair_ResetChainState();
    }
}

bool SalemFierceDeityHairPhysics_IsEnabled(void) {
    return sSalemHairEnabled;
}

void SalemFierceDeityHairPhysics_SetModelHandle(PlayerModelManagerHandle handle) {
    sSalemHairModelHandle = handle;
    SalemFierceDeityHairPhysics_SetEnabled(handle != 0);
}

void SalemFierceDeityHairPhysics_OnHeadLimbDraw(PlayState* play, Player* player) {
    MtxF headMtx;

    Matrix_Get(&headMtx);

    sSalemHairHeadMtx = headMtx;

    f32 sx = sqrtf(headMtx.xx*headMtx.xx + headMtx.yx*headMtx.yx + headMtx.zx*headMtx.zx);
    f32 sy = sqrtf(headMtx.xy*headMtx.xy + headMtx.yy*headMtx.yy + headMtx.zy*headMtx.zy);
    f32 sz = sqrtf(headMtx.xz*headMtx.xz + headMtx.yz*headMtx.yz + headMtx.zz*headMtx.zz);
    if (sx > 0.0001f) { headMtx.xx /= sx; headMtx.yx /= sx; headMtx.zx /= sx; }
    if (sy > 0.0001f) { headMtx.xy /= sy; headMtx.yy /= sy; headMtx.zy /= sy; }
    if (sz > 0.0001f) { headMtx.xz /= sz; headMtx.yz /= sz; headMtx.zz /= sz; }

    Matrix_MtxFToYXZRot(&headMtx, &sSalemHairHeadRot, 0);
    sSalemHairHasHeadMtx = true;
    sSalemHairHeadMtxFrame = play->gameplayFrames;
}

RECOMP_HOOK("Player_PostLimbDrawGameplay")
void SalemFierceDeityHairPhysics_PlayerPostLimbDrawHook(PlayState* play, s32 limbIndex, Gfx** dList1, Gfx** dList2,
                                                         Vec3s* rot, Actor* actor) {
    Player* player;

    (void)dList1;
    (void)dList2;
    (void)rot;

    if ((actor == NULL) || (actor->id != ACTOR_PLAYER)) {
        return;
    }

    player = (Player*)actor;
    if (!SalemHair_ShouldDraw(player)) {
        SalemHair_ResetChainState();
        return;
    }

    if (limbIndex == PLAYER_LIMB_HEAD) {
        Vec3f playerVel = SalemHair_GetPlayerVelocity(player);

        SalemFierceDeityHairPhysics_OnHeadLimbDraw(play, player);
        SalemHair_UpdatePhysicsOncePerFrame(play->gameplayFrames, playerVel, player->actor.speed);
        return;
    }

    if ((limbIndex == PLAYER_LIMB_TORSO) && sSalemHairHasHeadMtx && (sSalemHairHeadMtxFrame == play->gameplayFrames)) {

        OPEN_DISPS(play->state.gfxCtx);
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        func_80122868(play, player);
        Matrix_Push();
        SalemHair_DrawRig(play);
        Matrix_Pop();
        if ((player->invincibilityTimer > 0) || (gSaveContext.jinxTimer != 0)) {
            POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
        }
        CLOSE_DISPS(play->state.gfxCtx);
        sSalemHairHasHeadMtx = false;
    }
}
