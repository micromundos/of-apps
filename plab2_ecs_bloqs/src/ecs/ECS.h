#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
//#include "systems/RenderSystem.h"

//ofxECS, ofxArtemis..?
class ECS
{
  public:

    ECS(){};
    ~ECS(){};

    static float FPS;

    void init()
    {
      sm = world.getSystemManager();
      em = world.getEntityManager();
    };

    void update()
    {
      //ofLogNotice("ECS") << "---update";
      //TODO game/sim loop accum
      world.loopStart();
      //deciseconds ???
      //https://github.com/vinova/Artemis-Cpp
      //world.setDelta( 0.0016f ); 
      world.setDelta( 1000/FPS*0.0001f ); 
      int len = _systems.size();
      for (int i = 0; i < len; i++)
        _systems[i]->process();
    };

    //void render()
    //{};

    void init_systems()
    {
      //init_default_systems();
      sm->initializeAll();
    };

    void add_system( artemis::EntityProcessingSystem* sys )
    {
      _systems.push_back( (artemis::EntityProcessingSystem*)sm->setSystem( sys ) );
    }; 

    artemis::EntityManager* entities()
    {
      return em;
    };

    artemis::SystemManager* systems()
    {
      return sm;
    };

  private: 

    artemis::World world;
    artemis::SystemManager *sm;
    artemis::EntityManager *em;

    vector<artemis::EntityProcessingSystem*> _systems; 

    //void init_default_systems()
    //{
      //_systems.push_back( (artemis::EntityProcessingSystem*)sm->setSystem( new RenderSystem() ) );
    //};  

};

