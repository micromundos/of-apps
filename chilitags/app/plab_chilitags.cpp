#include <iostream>

#include <chilitags.hpp>

#include <opencv2/core/core.hpp> // for cv::Mat
#include <opencv2/core/core_c.h> // CV_AA
#include <opencv2/highgui/highgui.hpp> // for camera capture
#include <opencv2/imgproc/imgproc.hpp> // for camera capture

using namespace std;
using namespace cv;

cv::Scalar text_color = cv::Scalar(0, 0, 255);

void draw_tags2d(cv::Mat outputImage, std::map<int, chilitags::Quad> const& tags);

int main(int argc, char* argv[])
{
  cout
    << "Usage: "<< argv[0]
    << " [-c <tag configuration (YAML)>]"
    << " [-i <camera calibration (YAML)>]"
    << " [-d <camera device index (num)>]"
    << " [-w <camera camera width (num)>]"
    << " [-h <camera camera width (num)>]"
    << "\n";

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
  }

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

  while ('q' != (keyPressed = (char) cv::waitKey(1))) {

    switch(keyPressed)
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
    }

    cv::Mat inputImage;
    capture.read(inputImage);
    cv::Mat outputImage = inputImage.clone();

    const char* trigName = "ASYNC_DETECT_PERIODICALLY";
    chilitags::Chilitags::DetectionTrigger trig = chilitags::Chilitags::ASYNC_DETECT_PERIODICALLY; //Runs the detection in the background, with a period, only tracking in the call to `find()`, period defaults to 15, i.e. out of 15 consecutive calls to `find()`, the background thread will be informed to run detection. After this, a new detection will be done as soon as a new image frame is presented in the call to `find()`. If the background thread takes more time than 15 calls to `find()`, it will be running as frequently as possible
    //chilitags::Chilitags::DetectionTrigger trig = chilitags::Chilitags::DETECT_PERIODICALLY; //tracking most of the time, eventually run a full detection
    //chilitags::Chilitags::DetectionTrigger trig = chilitags::Chilitags::ASYNC_DETECT_ALWAYS; //detection is run as frequently as possible, i.e a new detection is started as soon as the new image frame is presented in the call to `find()` after the previous detection is finished

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

    //cv::putText(outputImage,
        //cv::format("Run 'top -H -p `pgrep async-detection`' to see running threads", trigName),
        //cv::Point(8, 68),
        //cv::FONT_HERSHEY_SIMPLEX, 0.5, text_color);

    //auto tags2d = chilitags.find( inputImage, trig );
    //auto tags3d = chilitags3D.estimate( tags2d );
    auto tags3d = chilitags3D.estimate( inputImage, trig );

    //draw_tags2d( outputImage, tags2d );

    for ( auto& tag : tags3d )
    {

      static const cv::Vec4d UNITS[4] {
        {0.f, 0.f, 0.f, 1.f},
        {DEFAULT_SIZE, 0.f, 0.f, 1.f},
        {0.f, DEFAULT_SIZE, 0.f, 1.f},
        {0.f, 0.f, DEFAULT_SIZE, 1.f},
      };

      cv::Matx44d transformation = tag.second;
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

      for (int i : {1,2,3}) 
      {
        cv::line(
            outputImage,
            PRECISION*t2DPoints[0],
            PRECISION*t2DPoints[i],
            AXIS_COLORS[i-1],
#ifdef OPENCV3
            2, cv::LINE_AA, SHIFT);
#else
            2, CV_AA, SHIFT);
#endif

        cv::putText( outputImage, AXIS_NAMES[i-1], t2DPoints[i],
            cv::FONT_HERSHEY_SIMPLEX, 0.5f, AXIS_COLORS[i-1]);
      }
    }

    cv::imshow("Pose Estimation", outputImage);
  }

  cv::destroyWindow("Pose Estimation");
  capture.release();

  return 0;
}

void draw_tags2d(
    cv::Mat outputImage,
    const std::map<int, chilitags::Quad> &tags)
{
  for ( const auto& tag : tags )
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
          text_color, 3, cv::LINE_AA, SHIFT);
#else
          text_color, 3, CV_AA, SHIFT);
#endif
    }

    cv::Point2f center = 0.5*(corners(0) + corners(2));
    cv::putText(outputImage, cv::format("%d", tag.first), center,
        cv::FONT_HERSHEY_SIMPLEX, 0.5, text_color);
  }
}

