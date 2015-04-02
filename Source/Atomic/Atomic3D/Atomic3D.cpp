
#include "StaticModel.h"
#include "StaticModelGroup.h"
#include "Skybox.h"
#include "Animation.h"
#include "Model.h"
#include "AnimatedModel.h"
#include "AnimationController.h"
#include "BillboardSet.h"
#include "ParticleEffect.h"
#include "ParticleEmitter.h"
#include "CustomGeometry.h"
#include "DecalSet.h"
#include "Terrain.h"
#include "TerrainPatch.h"


namespace Atomic
{


void RegisterAtomic3DLibrary(Context* context)
{
    Animation::RegisterObject(context);
    Model::RegisterObject(context);
    StaticModel::RegisterObject(context);
    StaticModelGroup::RegisterObject(context);
    Skybox::RegisterObject(context);
    AnimatedModel::RegisterObject(context);
    AnimationController::RegisterObject(context);
    BillboardSet::RegisterObject(context);
    ParticleEffect::RegisterObject(context);
    ParticleEmitter::RegisterObject(context);
    CustomGeometry::RegisterObject(context);
    DecalSet::RegisterObject(context);
    Terrain::RegisterObject(context);
    TerrainPatch::RegisterObject(context);
}

}
