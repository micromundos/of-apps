#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxOpenCv.h"
#include "DepthHoleFiller.h"

using namespace artemis;

class DepthHoleFillerSystem : public ECSsystem 
{ 

  public:

    DepthHoleFillerSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<DepthComponent>();
      addComponentType<DepthHoleFillerComponent>();
    };

    virtual void initialize() 
    {
      depth_hole_filler_m.init( *world );
      depth_m.init( *world );
      
      //  HOLES
      DHF_inited = false;
   
    };

    virtual void added(Entity &e) 
    {
      //depth_hole_filler_m.get(e)->data;
      depth_data = depth_m.get(e);
      if(!DHF_inited)
      {
        DHF_inited = true;
        DHF.setDimensions(depth_data->width,depth_data->height);
        
        DHF.setDepthHistory(1);
        cv_float_depth.allocate(depth_data->width,depth_data->height);
        cv_gray_image.allocate(depth_data->width,depth_data->height);
  
      }

    };

    virtual void processEntity(Entity &e)
    {
    
      depth_data = depth_m.get(e);
      
      if ( !depth_data->dirty )
        return;
      
      DepthHoleFillerComponent* depth_hole_filler_data = depth_hole_filler_m.get(e);
      
      if(!depth_hole_filler_data)
        return;

      bool bFillHolesUsingHistory	= depth_hole_filler_data->use_history;
      bool bFillHolesUsingContours = depth_hole_filler_data->use_contour;
      bool bFillHolesUsingClosing	= depth_hole_filler_data->use_closing;
      
      cv_float_depth.setFromPixels(depth_data->f_depth_ofpix_mm->getPixels(), depth_data->width, depth_data->height);
      cv_gray_image.setFromFloatImage(cv_float_depth);
      
      if (bFillHolesUsingHistory){
        int depthHistory = depth_hole_filler_data->depth_history;
        DHF.setDepthHistory(depthHistory);
        DHF.updatePreProcessingDepthHistory(cv_gray_image);
        DHF.fillHolesUsingHistory(cv_gray_image);
      }
      
      if (bFillHolesUsingClosing){
        int nClosingPasses = depth_hole_filler_data->passes;
        DHF.performMorphologicalClose(cv_gray_image,nClosingPasses);
      }
      
      if (bFillHolesUsingContours){
        int maxContourHoleToFillArea = depth_hole_filler_data->contour_max_area;
        int maxNContoursToFill = depth_hole_filler_data->contour_max_holes;
        int maxLevelDelta = depth_hole_filler_data->contour_delta;
        int maxHoleFillWidth = depth_hole_filler_data->contour_fill_width;
        DHF.fillHolesUsingContourFinder (cv_gray_image, maxContourHoleToFillArea, maxNContoursToFill, maxLevelDelta, maxHoleFillWidth);
      }
      
    }; 

    virtual void renderEntity(Entity &e)
    {
      DepthHoleFillerComponent* depth_hole_filler_data = depth_hole_filler_m.get(e);

      if(!depth_hole_filler_data->render)
        return;

      int w = 640 * .75;
      int h = 480 * .75;

      ofSetColor(255,255);
      cv_float_depth.draw(0,0,w,h);
      cv_gray_image.draw(w,0,w,h);
    };

  private:
    ComponentMapper<DepthComponent> depth_m;
    DepthComponent* depth_data;
    ComponentMapper<DepthHoleFillerComponent> depth_hole_filler_m;
    //
    bool                DHF_inited;
    DepthHoleFiller			DHF;
    ofxCvFloatImage       cv_float_depth;
    ofxCvGrayscaleImage   cv_gray_image;
    ofxCvContourFinder		contourFinder;
  
};

