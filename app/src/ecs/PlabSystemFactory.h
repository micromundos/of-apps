/*
 * this file is transformed by ecs.js
 * no tabs, no spaces
 *
 * XXX order is very important!
 *
 * TODO take the order of systems processsing from config.json
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
ecs.add_system(new TagsReceiverSystem("tags_receiver"));
ecs.add_system(new TagsSystem("tags"));
ecs.add_system(new TagSenderSystem("tag_sender"));
ecs.add_system(new TableCalibSystem("table_calib"));
ecs.add_system(new DepthProcessingSystem("depth_processing"));
ecs.add_system(new FlowFieldContainerSystem("flow_field_container"));
ecs.add_system(new FlowFieldGradientSystem("flow_field_gradient"));
ecs.add_system(new FlowFieldAttractorsSystem("flow_field_attractors"));
ecs.add_system(new FlowFieldSystem("flow_field"));
ecs.add_system(new FlowFieldRenderSystem("flow_field_render"));
ecs.add_system(new FisicaSystem("fisica"));
ecs.add_system(new KnobSystem("knob"));
ecs.add_system(new ParticleSystem("particle"));
ecs.add_system(new ParticleEmitterSystem("particle_emitter"));
ecs.add_system(new ParticleAttractorSystem("particle_attractor"));
ecs.add_system(new ParticleFlowFieldSystem("particle_flow_field"));
ecs.add_system(new TimeBenderSystem("time_bender"));
//dont remove this line
};

};

