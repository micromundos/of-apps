#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647693
#define HALF_PI 1.57079632679489661923
#define EPSILON 1e-30

float lerp( float n, float min, float max ) 
{
  return (1.0 - n) * (max - min) + min;
}

float lerp2d( float x, float x1, float x2, float y1, float y2 ) 
{
  return (x-x1) / (x2-x1) * (y2-y1) + y1;
}

