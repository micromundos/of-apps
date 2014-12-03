#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxPCL.h"
#include "ofxGeom.h"
#include "TableTopDetector.h"

class ofApp : public ofBaseApp{

  public:
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);


    ofEasyCam cam;

    ofxOscReceiver receiver;

    ofImage kinect_img;
    void update_kinect_data();

    ofxPCL::PointCloud cloud;
    void update_cloud();

    TableTopDetector<pcl::PointXYZ> detector;
    //TableTopDetector<pcl::PointNormal> detector;
    void detect_table();

    ofMesh mesh;
    void update_mesh();

    ofMesh mesh_downsampled;
    void update_mesh_downsampled();

    ofMesh mesh_table;
    void update_mesh_table();

    ofMesh mesh_table_projected;
    void update_mesh_table_projected();

};
