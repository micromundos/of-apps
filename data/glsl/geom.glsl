//geom functions
//see ofxGeom

vec4 make_plane( vec3 v0, vec3 v1, vec3 v2 )
{
  vec3 normal = cross( v2-v0, v1-v0 );
  vec3 n = normalize( normal );
  return vec4( n, -(n.x * v0.x + n.y * v0.y + n.z * v0.z ) );
}

vec4 make_plane( vec3 point, vec3 normal )
{
  vec3 n = normalize( normal );
  return vec4( n, -(n.x * point.x + n.y * point.y + n.z * point.z ) );
}

float plane_distance( vec4 plane, vec3 v )
{
  return plane.x * v.x + plane.y * v.y + plane.z * v.z + plane.w;
}

/*unsigned relative angle from 0 to PI (180deg)*/
/*http://www.euclideanspace.com/maths/algebra/vectors/angleBetween/index.htm*/
float angle( vec3 v0, vec3 v1, bool _normalize )
{
  vec3 n0 = _normalize ? normalize(v0) : v0;
  vec3 n1 = _normalize ? normalize(v0) : v1;
  return acos( dot( n0, n1 ) );
}

