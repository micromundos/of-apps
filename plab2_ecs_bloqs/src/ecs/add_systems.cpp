/*
 * this file is tranformed by make.js
 * TODO only add the systems needed parsing config.json on the fly
 * take the order from config.json
 */
#include "ecs/ECS.h"
#include "systems/Systems.h"
void ECS::add_systems()
{
  add_system(new BloqMakerSystem("bloq_maker"));
  add_system(new RGBDSystem("rgbd"));
  add_system(new DepthBlobsTrackerSystem("depth_blobs_tracker"));
  add_system(new ArucoSystem("aruco"));
  add_system(new CamaraLucidaSystem("camara_lucida"));
  add_system(new DepthFlowFieldSystem("depth_flow_field"));
  add_system(new FisicaSystem("fisica"));
  add_system(new ParticleSystem("particle"));
  add_system(new ParticleEmitterSystem("particle_emitter"));
  add_system(new ParticleDepthSlopeFlowSystem("particle_depth_slope_flow"));
  add_system(new ParticleFlowFieldSystem("particle_flow_field"));
  add_system(new ParticleBlobsContainersSystem("particle_blobs_containers"));
	//dont remove this line
};
