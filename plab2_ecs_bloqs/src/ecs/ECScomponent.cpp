#include <ecs/ECScomponent.h>

template<> 
string ECScomponent::get_value( Json::Value d ) 
{ return d.asString(); };

template<> 
bool ECScomponent::get_value( Json::Value d ) 
{ return d.asBool(); };

template<> 
float ECScomponent::get_value( Json::Value d ) 
{ return d.asFloat(); };

template<> 
int ECScomponent::get_value( Json::Value d ) 
{ return d.asInt(); };

