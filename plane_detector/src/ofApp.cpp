#include "ofApp.h"

using namespace ofxPCL;

bool do_detection = false;
bool done_detection = false;
bool render_kinect = true;
bool render_table = true;
bool render_table_projected = true;
bool render_downsampled = true;

void ofApp::setup()
{

  cloud = ofxPCL::New<PointCloud>();

  kinect.setRegistration(true);
  // ir, rgb, texture
  kinect.init(false,true,true);
  kinect.open();

  // Initialise the detector
  detector.setDepthLimits(10, 3000);
  detector.setBackgroundVoxelSize(0.05);
  detector.initialize();

}

void ofApp::exit() 
{
  kinect.close();
}

void ofApp::update()
{
  ofSetWindowTitle(ofToString(ofGetFrameRate(),2));

  kinect.update();
  if ( ! kinect.isFrameNew() )
    return; 

  update_mesh();
  update_cloud(); 
  detect_table();
  update_mesh_downsampled();
  update_mesh_table();
  update_mesh_table_projected();

}

void ofApp::draw()
{

  ofBackground(0);

  //kinect.drawDepth(10,10);
  //kinect.draw(420, 10, 400, 300);

  //Eigen::Vector4f ctr;
  //pcl::compute3DCentroid(*cloud,ctr); 
  //cam.setTarget( ofVec3f(ctr[0],ctr[1],ctr[2]) );
  //cam.setDistance( 100 );

  cam.begin();
  glPointSize(1);
  ofPushMatrix();
  ofScale(1, -1, -1);
  //ofTranslate(0, 0, -1000);
  glEnable(GL_DEPTH_TEST);
  if ( render_kinect ) 
    mesh.drawVertices();
  if ( render_downsampled ) 
    mesh_downsampled.drawVertices();
  if ( render_table ) 
    mesh_table.drawVertices();
  if ( render_table_projected ) 
    mesh_table_projected.drawFaces();
  glDisable(GL_DEPTH_TEST);
  ofPopMatrix();
  cam.end();

}

void ofApp::detect_table()
{
  if ( !do_detection ) 
    return;

  printf("Doing some detection...\n");
  done_detection = false;

  // Create a smaller cloud and detect it!
  //pcl::PointCloud<pcl::PointNormal>::Ptr small_cloud = detector.preprocessImageCloud(cloud);
  //detector.detect(*small_cloud);
  //detector.detect(*cloud);

  detector.filter(*cloud);
  detector.findTable();
  //detector.findTableObjects();
  //detector.splitTableObjects();

  done_detection = true;
  do_detection = false;
  printf("Detection complete.\n");

}

void ofApp::update_mesh_table_projected()
{
  if ( ! done_detection )
    return;

  const Eigen::Vector4f& p = detector.plane();
  ofxPlane plane(p[0],p[1],p[2],p[3]);  
  ofVec3f plane_normal = plane.normal();

  ofMesh& _mesh = mesh_table_projected;

  _mesh.setMode(OF_PRIMITIVE_TRIANGLES);
  _mesh.clear();

  const size_t npts = detector.tableProjectedCloud()->points.size(); 

  if ( npts < 3 ) return;

  //a single triangle is enough for a infinite plane

  //Eigen::Vector4f table_ctr;
  //pcl::compute3DCentroid( *detector.tableProjectedCloud(), table_ctr ); 

  ofVec3f table_ctr;
  for (int i = 0; i < npts; i++)
  {
    table_ctr += ofVec3f(detector.tableProjectedCloud()->points[i].x, detector.tableProjectedCloud()->points[i].y, detector.tableProjectedCloud()->points[i].z);
  }
  table_ctr /= npts;

  ofVec3f apoint(detector.tableProjectedCloud()->points[0].x, detector.tableProjectedCloud()->points[0].y, detector.tableProjectedCloud()->points[0].z);
  ofVec3f tang = table_ctr - apoint;
  tang.normalize();
  tang *= 1000;

  ofVec3f* triang = new ofVec3f[3];
  for (int i = 0; i < 3; i++)
  {
    triang[i] = table_ctr + tang.rotated( (360/3)*i, plane_normal );
  }

  for (int i = 0; i < 3; i++)
  {
    _mesh.addColor( ofColor::red );
    _mesh.addVertex( triang[i] );
  }

  delete[] triang;
}

void ofApp::update_mesh_table()
{
  if ( ! done_detection )
    return;

  ofMesh& _mesh = mesh_table;

  _mesh.setMode(OF_PRIMITIVE_POINTS);
  _mesh.clear();

  //ofxPCL::convert( detector.tableCloud(), _mesh );

  const size_t npts = detector.tableCloud()->points.size();
  //if (_mesh.getNumVertices() != npts) _mesh.getVertices().resize(npts);

  for (int i = 0; i < npts; i++)
  {
    _mesh.addColor( ofColor::white );
    _mesh.addVertex( ofVec3f(detector.tableCloud()->points[i].x, detector.tableCloud()->points[i].y, detector.tableCloud()->points[i].z) );
  }

}

void ofApp::update_mesh_downsampled()
{
  if ( ! done_detection )
    return;

  ofMesh& _mesh = mesh_downsampled;

  _mesh.setMode(OF_PRIMITIVE_POINTS);
  _mesh.clear();

  //ofxPCL::convert( detector.cloud_downsampled_, _mesh );

  const size_t npts = detector.cloud_downsampled_->points.size();
  //if (_mesh.getNumVertices() != npts) _mesh.getVertices().resize(npts);

  for (int i = 0; i < npts; i++)
  {
    _mesh.addColor( ofColor::green );
    _mesh.addVertex( ofVec3f(detector.cloud_downsampled_->points[i].x, detector.cloud_downsampled_->points[i].y, detector.cloud_downsampled_->points[i].z) );
  }

}

void ofApp::update_mesh()
{
  int w = kinect.width;
  int h = kinect.height;
  int step = 1;

  mesh.setMode(OF_PRIMITIVE_POINTS);
  mesh.clear();

  for (int y = 0; y < h; y += step) 
  {
    for (int x = 0; x < w; x += step) 
    {
      if ( kinect.getDistanceAt(x, y) > 0 ) 
      {
        mesh.addColor( kinect.getColorAt(x,y) );
        mesh.addVertex( kinect.getWorldCoordinateAt(x,y) );
      }
    }
  }
}

void ofApp::update_cloud()
{
  ofxPCL::convert( mesh.getVertices(), cloud );
}



void ofApp::keyPressed(int key){

  if (key == 'd')
    do_detection = true;

  if (key == 'k')
    render_kinect = !render_kinect;
  if (key == 't')
    render_table = !render_table;
  if (key == 'p')
    render_table_projected = !render_table_projected;
  if (key == 'w')
    render_downsampled = !render_downsampled;

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
