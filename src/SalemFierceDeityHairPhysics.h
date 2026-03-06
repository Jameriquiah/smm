#ifndef SALEM_FIERCE_DEITY_HAIR_PHYSICS_H
#define SALEM_FIERCE_DEITY_HAIR_PHYSICS_H

#include <stdbool.h>

#include "playermodelmanager_api.h"

struct PlayState;
struct Player;

void SalemFierceDeityHairPhysics_Init(void);
void SalemFierceDeityHairPhysics_SetEnabled(bool enabled);
bool SalemFierceDeityHairPhysics_IsEnabled(void);
void SalemFierceDeityHairPhysics_OnHeadLimbDraw(struct PlayState* play, struct Player* player);
void SalemFierceDeityHairPhysics_SetModelHandle(PlayerModelManagerHandle handle);

#endif // SALEM_FIERCE_DEITY_HAIR_PHYSICS_H
