#include <iostream>

#include <chilitags.hpp>

#include <opencv2/core/core.hpp> // for cv::Mat
#include <opencv2/core/core_c.h> // CV_AA
#include <opencv2/highgui/highgui.hpp> // for camera capture
#include <opencv2/imgproc/imgproc.hpp> // for camera capture

// osc
#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"
#define OUTPUT_BUFFER_SIZE 1024

using namespace std;
using namespace cv;

cv::Scalar text_color = cv::Scalar(0, 0, 255);

void render_tags_2d( cv::Mat outputImage, std::map<int, chilitags::Quad> const& tags2d );
vector <string> ofSplitString ( const string & source, const string & delimiter, bool ignoreEmpty = false );

int main(int argc, char* argv[])
{
  cout
    << "Usage: "<< argv[0]
    << " [-c <tag configuration (YAML)>]"
    << " [-i <camera calibration (YAML)>]"
    << " [-d <camera device index (num)>]"
    << " [-w <camera camera width (num)>]"
    << " [-h <camera camera width (num)>]"
    << " [-a <osc ip address (string)>]"
    << " [-p <osc port (num)>]"
    << "\n";

  string ip = "127.0.0.1";
  int port = 7000;

  static const float DEFAULT_SIZE = 20.f;

  const char* intrinsicsFilename = 0;
  const char* configFilename = 0;
  int cameraIndex = 0;
  int width = 0;
  int height = 0;

  for ( int i = 1; i < argc; i++ )
  {
    if ( strcmp(argv[i], "-c") == 0 )
    {
      configFilename = argv[++i];
    }
    else if ( strcmp(argv[i], "-i") == 0 )
    {
      intrinsicsFilename = argv[++i];
    }
    else if ( strcmp(argv[i], "-d") == 0 )
    {
      cameraIndex = std::atoi(argv[++i]);
    }
    else if ( strcmp(argv[i], "-w") == 0 )
    {
      width = std::atoi(argv[++i]);
    }
    else if ( strcmp(argv[i], "-h") == 0 )
    {
      height = std::atoi(argv[++i]);
    }

    //osc
    else if ( strcmp(argv[i], "-a") == 0 )
    {
      ip = argv[++i];
    }
    else if ( strcmp(argv[i], "-p") == 0 )
    {
      port = std::atoi(argv[++i]);
    }
  }

  /*****************************/
  /*    osc                    */
  /*****************************/
  UdpTransmitSocket socket( IpEndpointName( ip.c_str(), port ) ); 

  /*****************************/
  /*    Init camera capture    */
  /*****************************/
  cv::VideoCapture capture(cameraIndex);
  if (!capture.isOpened())
  {
    cerr << "Unable to initialise video capture.\n";
    return 1;
  }

  /******************************/
  /* Setting up pose estimation */
  /******************************/

  if ( width == 0 )
  {
#ifdef OPENCV3
    width  = capture.get(cv::CAP_PROP_FRAME_WIDTH);
#else
    width  = capture.get(CV_CAP_PROP_FRAME_WIDTH);
#endif
  }

  if ( height == 0 )
  {
#ifdef OPENCV3
    height = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
#else
    height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
#endif
  }

  chilitags::Chilitags3D chilitags3D( Size( width, height ) );
  chilitags3D.setDefaultTagSize(DEFAULT_SIZE);

  if (configFilename) chilitags3D.readTagConfiguration(configFilename);

  if (intrinsicsFilename) 
  {
    Size calibratedImageSize = chilitags3D.readCalibration(intrinsicsFilename);
    //width = calibratedImageSize.width;
    //height = calibratedImageSize.height;
  }

#ifdef OPENCV3
    capture.set(cv::CAP_PROP_FRAME_WIDTH, width);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, height);
#else
    capture.set(CV_CAP_PROP_FRAME_WIDTH, width);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, height);
#endif

  cv::Mat projectionMat = cv::Mat::zeros(4,4,CV_64F);
  chilitags3D.getCameraMatrix().copyTo(projectionMat(cv::Rect(0,0,3,3)));
  cv::Matx44d projection = projectionMat;
  projection(3,2) = 1;

  /*****************************/
  /*             Go!           */
  /*****************************/
  cv::namedWindow("plab chilitags");

  char keyPressed;
  bool filterEnabled = true;

  cv::Mat Q = (cv::Mat_<float>(7,7) <<
      1e-3f,  0,      0,      0,      0,      0,      0,
      0,      1e-3f,  0,      0,      0,      0,      0,
      0,      0,      1e-3f,  0,      0,      0,      0,
      0,      0,      0,      1e-4f,  0,      0,      0,
      0,      0,      0,      0,      1e-4f,  0,      0,
      0,      0,      0,      0,      0,      1e-4f,  0,
      0,      0,      0,      0,      0,      0,      1e-4f);
  float alphaQ = 1.0f;

  cv::Mat R = (cv::Mat_<float>(7,7) <<
      1e-3f,  0,      0,      0,      0,      0,      0,
      0,      1e-3f,  0,      0,      0,      0,      0,
      0,      0,      1e-1f,  0,      0,      0,      0,
      0,      0,      0,      1e-3f,  0,      0,      0,
      0,      0,      0,      0,      1e-2f,  0,      0,
      0,      0,      0,      0,      0,      1e-2f,  0,
      0,      0,      0,      0,      0,      0,      1e-5f);
  float alphaR = 1.0f;

  const char* trigName = "ASYNC_DETECT_PERIODICALLY";
  chilitags::Chilitags::DetectionTrigger trig = chilitags::Chilitags::ASYNC_DETECT_PERIODICALLY; //Runs the detection in the background, with a period, only tracking in the call to `find()`, period defaults to 15, i.e. out of 15 consecutive calls to `find()`, the background thread will be informed to run detection. After this, a new detection will be done as soon as a new image frame is presented in the call to `find()`. If the background thread takes more time than 15 calls to `find()`, it will be running as frequently as possible
  //chilitags::Chilitags::DetectionTrigger trig = chilitags::Chilitags::DETECT_PERIODICALLY; //tracking most of the time, eventually run a full detection
  //chilitags::Chilitags::DetectionTrigger trig = chilitags::Chilitags::ASYNC_DETECT_ALWAYS; //detection is run as frequently as possible, i.e a new detection is started as soon as the new image frame is presented in the call to `find()` after the previous detection is finished

  while ('q' != (keyPressed = (char) cv::waitKey(1)))
  {

    switch ( keyPressed )
    {
      case 'f':
        filterEnabled = !filterEnabled;
        chilitags3D.enableFilter(filterEnabled);
        break;
      case 'w':
        alphaQ *= 10.0f;
        chilitags3D.setFilterProcessNoiseCovariance(alphaQ*Q);
        break;
      case 's':
        alphaQ /= 10.0f;
        chilitags3D.setFilterProcessNoiseCovariance(alphaQ*Q);
        break;
      case 'e':
        alphaR *= 10.0f;
        chilitags3D.setFilterObservationNoiseCovariance(alphaR*R);
        break;
      case 'd':
        alphaR /= 10.0f;
        chilitags3D.setFilterObservationNoiseCovariance(alphaR*R);
        break;

      case 't':
        if ( trig == chilitags::Chilitags::DETECT_PERIODICALLY ) 
        {
          trig = chilitags::Chilitags::ASYNC_DETECT_PERIODICALLY;
          trigName = "ASYNC_DETECT_PERIODICALLY";
        }
        else if ( trig == chilitags::Chilitags::ASYNC_DETECT_PERIODICALLY )
        {
          trig = chilitags::Chilitags::ASYNC_DETECT_ALWAYS;
          trigName = "ASYNC_DETECT_ALWAYS";
        }
        else
        {
          trig = chilitags::Chilitags::DETECT_PERIODICALLY;
          trigName = "DETECT_PERIODICALLY";
        }
        break;
    }

    cv::Mat inputImage;
    capture.read(inputImage);
    cv::Mat outputImage = inputImage.clone(); 

    cv::putText(outputImage, 
        cv::format("Filtering %s, press 'f' to toggle",filterEnabled ? "ENABLED" : "DISABLED"),
        cv::Point(8,20), 
        cv::FONT_HERSHEY_SIMPLEX, 0.5, text_color);

    cv::putText(outputImage, 
        cv::format("Process covariance multiplier: %f, press 'w' or 's' to modify", alphaQ),
        cv::Point(8,36), 
        cv::FONT_HERSHEY_SIMPLEX, 0.5, text_color);

    cv::putText(outputImage, 
        cv::format("Observation covariance multiplier: %f, press 'e' or 'd' to modify", alphaR),
        cv::Point(8,52), 
        cv::FONT_HERSHEY_SIMPLEX, 0.5, text_color);

    cv::putText(outputImage,
        cv::format("Detection trigger: %s (press 't' to toggle)", trigName),
        cv::Point(8,68),
        cv::FONT_HERSHEY_SIMPLEX, 0.5, text_color);

    //cv::putText(outputImage,
        //cv::format("Run 'top -H -p `pgrep async-detection`' to see running threads", trigName),
        //cv::Point(8, 84),
        //cv::FONT_HERSHEY_SIMPLEX, 0.5, text_color);


    auto tags2d = chilitags3D.getChilitags().find( inputImage, trig );
    auto tags3d = chilitags3D.estimate( tags2d );
    //auto tags3d = chilitags3D.estimate( inputImage, trig );

    render_tags_2d( outputImage, tags2d );

    // osc message
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );

    p << osc::BeginBundleImmediate << osc::BeginMessage( "/tags" );

    //length
    p << (int)tags3d.size(); 

    for ( auto& tag : tags3d )
    {
      string id = tag.first;
      string idxs = ofSplitString( id, "_" )[1];
      int idx = std::atoi( idxs.c_str() );

      chilitags::Quad& tag2d = tags2d[idx];
      const cv::Mat_<cv::Point2f> corners( tag2d );

      cv::Matx44d transformation = tag.second;

      p << idxs.c_str() //id.c_str() //id
        //size
        << (float)DEFAULT_SIZE 
        //corners
        << (float)corners(0).x << (float)corners(0).y
        << (float)corners(1).x << (float)corners(1).y
        << (float)corners(2).x << (float)corners(2).y
        << (float)corners(3).x << (float)corners(3).y
        //translation
        << (float)transformation(0,3)
        << (float)transformation(1,3)
        << (float)transformation(2,3); 

      static const cv::Vec4d UNITS[4] {
        {0.f, 0.f, 0.f, 1.f},
        {DEFAULT_SIZE, 0.f, 0.f, 1.f},
        {0.f, DEFAULT_SIZE, 0.f, 1.f},
        {0.f, 0.f, DEFAULT_SIZE, 1.f},
      };

      cv::Vec4f referential[4] = {
        projection*transformation*UNITS[0],
        projection*transformation*UNITS[1],
        projection*transformation*UNITS[2],
        projection*transformation*UNITS[3],
      };

      std::vector<cv::Point2f> t2DPoints;
      for ( auto homogenousPoint : referential )
      {
        t2DPoints.push_back( cv::Point2f(
          homogenousPoint[0]/homogenousPoint[3],
          homogenousPoint[1]/homogenousPoint[3] ) );
      }

      static const int SHIFT = 16;
      static const float PRECISION = 1<<SHIFT;
      static const std::string AXIS_NAMES[3] = { "x", "y", "z" };
      static const cv::Scalar AXIS_COLORS[3] = {
        {0,0,255},{0,255,0},{255,0,0},
      };

      for ( int i : {1,2,3} )
      {
        cv::line(
            outputImage,
            PRECISION*t2DPoints[0],
            PRECISION*t2DPoints[i],
            AXIS_COLORS[i-1],
#ifdef OPENCV3
            1, cv::LINE_AA, SHIFT);
#else
            1, CV_AA, SHIFT);
#endif

        cv::putText( outputImage, AXIS_NAMES[i-1], t2DPoints[i],
            cv::FONT_HERSHEY_SIMPLEX, 0.5f, AXIS_COLORS[i-1]);
      }
    }

    p << osc::EndMessage << osc::EndBundle;
    socket.Send( p.Data(), p.Size() );

    cv::imshow("plab chilitags", outputImage);
  }

  cv::destroyWindow("plab chilitags");
  capture.release();

  return 0;
}

void render_tags_2d(
    cv::Mat outputImage,
    const std::map<int, chilitags::Quad> &tags2d)
{
  for ( const auto& tag : tags2d )
  {
    const cv::Mat_<cv::Point2f> corners(tag.second);

    for ( size_t i = 0; i < 4; ++i )
    {
      const int SHIFT = 16;
      const float PRECISION = 1 << SHIFT;

      cv::line(
          outputImage,
          PRECISION*corners(i),
          PRECISION*corners((i+1)%4),
#ifdef OPENCV3
          text_color, 1, cv::LINE_AA, SHIFT);
#else
          text_color, 1, CV_AA, SHIFT);
#endif
    }

    cv::Point2f center = 0.5*(corners(0) + corners(2));
    cv::putText(outputImage, cv::format("%d", tag.first), center,
        cv::FONT_HERSHEY_SIMPLEX, 0.5, text_color);
  }
}

vector <string> ofSplitString ( const string & source, const string & delimiter, bool ignoreEmpty ) 
{
  vector<string> result;
  if (delimiter.empty()) {
    result.push_back(source);
    return result;
  }
  string::const_iterator substart = source.begin(), subend;
  while (true) {
    subend = search(substart, source.end(), delimiter.begin(), delimiter.end());
    string sub(substart, subend);
    //if(trim) {
      //Poco::trimInPlace(sub);
    //}
    if (!ignoreEmpty || !sub.empty()) {
      result.push_back(sub);
    }
    if (subend == source.end()) {
      break;
    }
    substart = subend + delimiter.size();
  }
  return result;
}

