/*
 * this file is transformed by ecs.js
 * no tabs, no spaces
 *
 * XXX order is very important!
 *
 * TODO 
 * take the order from config.json
 * only add the systems needed parsing config.json on the fly
 */
#pragma once

#include "ofxECS.h"
#include "Systems.h"

class PlabSystemFactory
{
public:
PlabSystemFactory(){};

void add_systems( ECS& ecs )
{
ofLogNotice("PlabSystemFactory") << "add systems";
ecs.add_system(new BloqMakerSystem("bloq_maker"));
ecs.add_system(new CamaraLucidaSystem("camara_lucida"));
ecs.add_system(new KinectSystem("kinect"));
ecs.add_system(new Ps3EyeSystem("ps_3_eye"));
ecs.add_system(new TableCalibSystem("table_calib"));
ecs.add_system(new DepthHoleFillerSystem("depth_hole_filler"));
ecs.add_system(new DepthProcessingSystem("depth_processing"));
ecs.add_system(new EntradasSystem("entradas"));
ecs.add_system(new ArucoSystem("aruco"));
ecs.add_system(new DepthFlowFieldSystem("depth_flow_field"));
ecs.add_system(new FisicaSystem("fisica"));
ecs.add_system(new ParticleSystem("particle"));
ecs.add_system(new ParticleEmitterSystem("particle_emitter"));
ecs.add_system(new ParticleFlowFieldSystem("particle_flow_field"));
ecs.add_system(new KnobSystem("knob"));
ecs.add_system(new TimeBenderSystem("time_bender"));
//dont remove this line
};

};

