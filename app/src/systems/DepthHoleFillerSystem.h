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
      scale = 0.7;
    };

    virtual void added(Entity &e) 
    {
      DepthComponent* depth_data = depth_m.get(e);

      int dw = depth_data->width;
      int dh = depth_data->height;
      int w = dw * scale;
      int h = dh * scale;

      DHF.setDimensions(w,h);
      DHF.setDepthHistory(1);

      cv_float_depth.allocate( dw, dh );
      cv_float_depth_holes.allocate( dw, dh );
      cv_float_depth_holes_scaled.allocate( w, h );
      cv_gray_image.allocate( dw, dh );
      cv_gray_image_scaled.allocate( w, h );

      cv_float_depth.setNativeScale( 0.0, 5000.0 );
      cv_float_depth_holes.setNativeScale( 0.0, 5000.0 );
      cv_float_depth_holes_scaled.setNativeScale( 0.0, 5000.0 );
    };

    virtual void processEntity(Entity &e)
    {
      DepthComponent* depth_data = depth_m.get(e);
      DepthHoleFillerComponent* depth_hole_filler_data = depth_hole_filler_m.get(e);

      int dw = depth_data->width;
      int dh = depth_data->height;
      int w = dw * scale;
      int h = dh * scale;

      bool use_history = depth_hole_filler_data->use_history;
      bool use_contour = depth_hole_filler_data->use_contour;
      bool use_closing = depth_hole_filler_data->use_closing;

      depth_hole_filler_data->enabled = use_history || use_contour || use_closing;

      if ( !depth_data->dirty )
        return;

      cv_float_depth.setFromPixels( depth_data->f_depth_ofpix_mm->getPixels(), dw, dh );
      //cv_gray_image.setFromFloatImage( cv_float_depth ); //maps scale min-max -> 0-255
      cv_gray_image.setFromPixels( depth_data->depth_ofpix_grey->getPixels(), dw, dh );
      cv_gray_image_scaled.scaleIntoMe( cv_gray_image );

      if (use_history)
      {
        TS_START("DepthHoleFillerSystem history");
        int depthHistory = depth_hole_filler_data->history;
        DHF.setDepthHistory(depthHistory);
        DHF.updatePreProcessingDepthHistory(cv_gray_image_scaled);
        DHF.fillHolesUsingHistory(cv_gray_image_scaled);
        TS_STOP("DepthHoleFillerSystem history");
      }

      if (use_closing)
      {
        TS_START("DepthHoleFillerSystem closing");
        int nClosingPasses = depth_hole_filler_data->closing_passes;
        DHF.performMorphologicalClose(cv_gray_image_scaled,nClosingPasses);
        TS_STOP("DepthHoleFillerSystem closing");
      }

      if (use_contour)
      {
        TS_START("DepthHoleFillerSystem contours");

        int maxContourHoleToFillArea = depth_hole_filler_data->contour_max_area;
        int maxNContoursToFill = depth_hole_filler_data->contour_max_holes;
        int maxLevelDelta = depth_hole_filler_data->contour_delta;
        int maxHoleFillWidth = depth_hole_filler_data->contour_fill_width;
        DHF.fillHolesUsingContourFinder (cv_gray_image_scaled, maxContourHoleToFillArea, maxNContoursToFill, maxLevelDelta, maxHoleFillWidth);

        cv_float_depth_holes_scaled.setFromPixels( DHF.get_blobs_pixels(), w, h ); //maps 0-255 -> scale min-max
        cv_float_depth_holes.scaleIntoMe( cv_float_depth_holes_scaled );

        cvMax(
            cv_float_depth.getCvImage(), 
            cv_float_depth_holes.getCvImage(), 
            cv_float_depth.getCvImage() );

        //copy
        depth_hole_filler_data->output.setFromPixels( cv_float_depth.getFloatPixelsRef() );

        TS_STOP("DepthHoleFillerSystem contours");
      }

    }; 

    virtual void renderEntity(Entity &e)
    {
      DepthHoleFillerComponent* depth_hole_filler_data = depth_hole_filler_m.get(e);

      if( ! depth_hole_filler_data->render )
        return;

      int w = 640 * .75;
      int h = 480 * .75;

      ofSetColor(255,255);
      cv_float_depth.draw( 0,0, w,h );
      cv_float_depth_holes.draw( w,0, w,h );
      cv_gray_image_scaled.draw( w*2,0, w,h );
    };

  private:

    ComponentMapper<DepthComponent> depth_m;
    ComponentMapper<DepthHoleFillerComponent> depth_hole_filler_m;

    DepthHoleFiller			DHF;
    ofxCvFloatImage       cv_float_depth;
    ofxCvFloatImage       cv_float_depth_holes;
    ofxCvFloatImage       cv_float_depth_holes_scaled;
    ofxCvGrayscaleImage   cv_gray_image;
    ofxCvGrayscaleImage   cv_gray_image_scaled;
    ofxCvContourFinder		contourFinder;

    float scale;
  
};

