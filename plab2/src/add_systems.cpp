/*
 * this file is tranformed by make.js
 * TODO only add the systems needed parsing config.json on the fly
 * take the order from config.json
 */
#include "ofxECS.h"
#include "ofApp.h"
#include "systems/Systems.h"
void ofApp::add_systems()
{
  ecs.add_system(new BloqMakerSystem("bloq_maker"));
  ecs.add_system(new RGBDSystem("rgbd"));
  ecs.add_system(new DepthBlobsTrackerSystem("depth_blobs_tracker"));
  ecs.add_system(new ArucoSystem("aruco"));
  ecs.add_system(new CamaraLucidaSystem("camara_lucida"));
  ecs.add_system(new DepthFlowFieldSystem("depth_flow_field"));
  ecs.add_system(new FisicaSystem("fisica"));
  ecs.add_system(new ParticleSystem("particle"));
  ecs.add_system(new ParticleEmitterSystem("particle_emitter"));
  ecs.add_system(new ParticleDepthSlopeFlowSystem("particle_depth_slope_flow"));
  ecs.add_system(new ParticleFlowFieldSystem("particle_flow_field"));
  ecs.add_system(new ParticleBlobsContainersSystem("particle_blobs_containers"));
	//dont remove this line
};
