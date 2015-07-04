#pragma once

struct Tag
{

  struct Extrinsics
  {
    cv::Mat R, T;
  };

  string id;
  float size;
  ofVec3f translation;
  vector<ofVec2f> corners;

  string str()
  {
    stringstream ss;
    ss << "[ tag "
      << "id: " << id
      << ", size: " << size
      << ", translation: " << translation
      << ", corners: " << corners.size()
      << " ]";
    return ss.str(); 
  }
};

