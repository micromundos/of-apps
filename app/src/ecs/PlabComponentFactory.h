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
else if (id == "flow_field") return new FlowFieldComponent(id);
else if (id == "particle_system") return new ParticleSystemComponent(id);
else if (id == "particle_emitter") return new ParticleEmitterComponent(id);
else if (id == "particle_flow_field") return new ParticleFlowFieldComponent(id);
else if (id == "fisica") return new FisicaComponent(id);
else if (id == "time_bender") return new TimeBenderComponent(id);
else if (id == "knob") return new KnobComponent(id);
else if (id == "table_calib") return new TableCalibComponent(id);
else if (id == "depth_processing") return new DepthProcessingComponent(id);
else if (id == "entradas") return new EntradasComponent(id);
else if (id == "depth_hole_filler") return new DepthHoleFillerComponent(id);
else if (id == "tags_receiver") return new TagsReceiverComponent(id);
else if (id == "tags_processing") return new TagsProcessingComponent(id);
//dont remove this line

return NULL;
};

};
