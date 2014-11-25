/*
 * this file is tranformed by ecs.js
 * no tabs, no spaces
 *
 * TODO only add the systems needed parsing config.json on the fly
 * take the order from config.json
 */
#pragma once

#include "ofxECS.h"
#include "systems/Systems.h"

class PlabSystemFactory
{
public:
PlabSystemFactory(){};

void add_systems( ECS& ecs )
{
ofLogNotice("PlabSystemFactory") << "add systems";
ecs.add_system(new BloqMakerSystem("bloq_maker"));
ecs.add_system(new CamaraLucidaSystem("camara_lucida"));
ecs.add_system(new RGBDSystem("rgbd"));
ecs.add_system(new BlobsSystem("blobs"));
ecs.add_system(new ArucoSystem("aruco"));
ecs.add_system(new DepthFlowFieldSystem("depth_flow_field"));
ecs.add_system(new FisicaSystem("fisica"));
ecs.add_system(new ParticleSystem("particle"));
ecs.add_system(new ParticleEmitterSystem("particle_emitter"));
ecs.add_system(new ParticleDepthSlopeFlowSystem("particle_depth_slope_flow"));
ecs.add_system(new ParticleFlowFieldSystem("particle_flow_field"));
ecs.add_system(new ParticleBlobsContainersSystem("particle_blobs_containers"));
ecs.add_system(new TimeBenderSystem("time_bender"));
//dont remove this line
};

};

