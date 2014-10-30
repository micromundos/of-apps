/*
 * this file is tranformed by make.js
 * TODO only add the systems needed parsing config.json on the fly
 * take the order from config.json
 */
#include "ecs/ECS.h"
#include "systems/Systems.h"
void ECS::add_systems()
{
	add_system(new BloqMakerSystem());
  add_system(new RGBDSystem());
  add_system(new ArucoSystem());
  add_system(new CamaraLucidaSystem());
  add_system(new DepthBlobsSystem());
  add_system(new DepthFlowFieldSystem());
  add_system(new ParticleSystem());
  add_system(new ParticleEmitterSystem());
  add_system(new ParticleDepthSlopeFlowSystem());
  add_system(new ParticleFlowFieldSystem());
  add_system(new ParticleDepthBlobsContainersSystem());
	//dont remove this line
};
