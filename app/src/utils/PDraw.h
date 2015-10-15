#ifndef app_PDraw_h
#define app_PDraw_h
#include "ofMain.h"

class PDraw
{
public:
  
  PDraw(){
    mesh = ofMesh();
  };
  ~PDraw(){};
  

  void    circle(int  _inner_radius,int _outer_radius,int   _points,ofColor _color1,ofColor _color2,int   _init = 0,int   _end = 360,bool _draw = true)
  {
    mesh.clear();
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    for(int i=0;i<_points;i++)
    {
      float portion = ofMap(i,0,_points,_init,_end);
      float angle = ofDegToRad(portion);
      float x = cos(angle);
      float y = sin(angle);
      
      ofVec3f vertex1 = ofVec3f(x*_inner_radius,y*_inner_radius);
      ofVec3f vertex2 = ofVec3f(x*_outer_radius,y*_outer_radius);
      
      mesh.addColor(_color1);
      mesh.addVertex(vertex1);
      mesh.addColor(_color2);
      mesh.addVertex(vertex2);
      
      if(_init == 0 && _end == 360 && i == _points-1)
      {
        mesh.addColor(_color1);
        mesh.addVertex(mesh.getVertices().at(0));
        mesh.addColor(_color2);
        mesh.addVertex(mesh.getVertices().at(1));
      }
    }
    if(_draw)
      mesh.draw();
    
  };
  
  void  create_circle(int  _inner_radius,int _outer_radius,int   _points,ofColor _color1,ofColor _color2,int   _init = 0,int   _end = 360)
  {
    circle(_inner_radius,_outer_radius,_points,_color1,_color2,_init,_end,false);
  }
  
  void  draw()
  {
    mesh.draw();
  }
  
private:
  ofMesh  mesh;

};






#endif




