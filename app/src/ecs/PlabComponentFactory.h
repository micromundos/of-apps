/*
 * this file is transformed by ecs.js
 * no tabs, no spaces
 */
#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"

class PlabComponentFactory : ComponentFactory
{
public:
PlabComponentFactory(){};

virtual ECScomponent* make(string id)
{
ofLogNotice("PlabComponentFactory") << "\t make " << id;

if (id == "bloq") return new BloqComponent(id);
else if (id == "config") return new ConfigComponent(id);
else if (id == "bloq_maker") return new BloqMakerComponent(id);
else if (id == "bloq_events") return new BloqEventsComponent(id);
else if (id == "camara_lucida") return new CamaraLucidaComponent(id);
else if (id == "render") return new RenderComponent(id);
else if (id == "rgb") return new RgbComponent(id);
else if (id == "depth") return new DepthComponent(id);
else if (id == "aruco") return new ArucoComponent(id);
else if (id == "blobs") return new BlobsComponent(id);
else if (id == "flow_field") return new FlowFieldComponent(id);
else if (id == "particle_system") return new ParticleSystemComponent(id);
else if (id == "particle_emitter") return new ParticleEmitterComponent(id);
else if (id == "particle_flow_field") return new ParticleFlowFieldComponent(id);
else if (id == "particle_depth_slope_flow") return new ParticleDepthSlopeFlowComponent(id);
else if (id == "particle_blobs_containers") return new ParticleBlobsContainersComponent(id);
else if (id == "fisica") return new FisicaComponent(id);
else if (id == "time_bender") return new TimeBenderComponent(id);
else if (id == "knob") return new KnobComponent(id);
else if (id == "depth_smoothing") return new DepthSmoothingComponent(id);
else if (id == "depth_segmentation") return new DepthSegmentationComponent(id);
//dont remove this line

return NULL;
};

};
