/*
 * this file is tranformed by make.js
 * TODO only add the systems needed parsing config.json on the fly
 * take the order from config.json
 */
#include "ofApp.h"
void ofApp::add_systems()
{
  ecs.add_system(new RGBDSystem());
  ecs.add_system(new ArucoSystem());
	ecs.add_system(new CamaraLucidaSystem());
  ecs.add_system(new ParticleSystem());
  ecs.add_system(new ParticleEmitterSystem());
  ecs.add_system(new ParticleDepthSlopeConstraintSystem());
  ecs.add_system(new ParticleFlowFieldSystem());
  ecs.add_system(new DepthFlowFieldSystem());
	//dont remove this line
};
