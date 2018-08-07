package com.mousebirdconsulting.autotester.TestCases;

import android.app.Activity;
import android.graphics.Color;
import android.os.Handler;
import android.util.Log;

import com.mousebird.maply.GlobeController;
import com.mousebird.maply.MapController;
import com.mousebird.maply.MaplyBaseController;
import com.mousebird.maply.Point2d;
import com.mousebird.maply.Point3d;
import com.mousebird.maply.QuadImageTileLayer;
import com.mousebird.maply.RemoteTileInfo;
import com.mousebird.maply.RemoteTileSource;
import com.mousebird.maply.SphericalMercatorCoordSystem;
import com.mousebird.maply.VectorInfo;
import com.mousebird.maply.VectorObject;
import com.mousebirdconsulting.autotester.ConfigOptions;
import com.mousebirdconsulting.autotester.Framework.MaplyTestCase;

import java.io.File;


public class StamenRemoteTestCase extends MaplyTestCase {

    public StamenRemoteTestCase(Activity activity) {
        super(activity);

        setTestName("Stamen Remote Test");
        setDelay(4);
        this.implementation = TestExecutionImplementation.Both;

        // Just trying...
        //this.remoteResources.add("https://manuals.info.apple.com/en_US/macbook_retina_12_inch_early2016_essentials.pdf");
    }

    private QuadImageTileLayer setupImageLayer(ConfigOptions.TestType testType, MaplyBaseController baseController) {
        String cacheDirName = "stamen_watercolor3";
        File cacheDir = new File(getActivity().getCacheDir(), cacheDirName);
        cacheDir.mkdir();
        RemoteTileSource remoteTileSource = new RemoteTileSource(baseController,new RemoteTileInfo("http://tile.stamen.com/watercolor/", "png", 0, 18));
        remoteTileSource.setCacheDir(cacheDir);
        // Note: Turn this on to get more information from the tile source
//		remoteTileSource.debugOutput = true;
        SphericalMercatorCoordSystem coordSystem = new SphericalMercatorCoordSystem();
        final QuadImageTileLayer baseLayer = new QuadImageTileLayer(baseController, coordSystem, remoteTileSource);
        if (testType == ConfigOptions.TestType.MapTest) {
//			baseLayer.setSingleLevelLoading(true);
//			baseLayer.setUseTargetZoomLevel(true);
//			baseLayer.setMultiLevelLoads(new int[]{-3});
            baseLayer.setCoverPoles(false);
            baseLayer.setHandleEdges(false);
        } else {
            baseLayer.setCoverPoles(true);
            baseLayer.setHandleEdges(true);
        }
        baseLayer.setImageDepth(1);
        baseLayer.setSingleLevelLoading(false);
        baseLayer.setCoverPoles(true);
        baseLayer.setHandleEdges(true);

        final Handler handler = new Handler();
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                baseLayer.reload();
            }
        }, 4000);

        return baseLayer;

    }

    @Override
    public boolean setUpWithGlobe(GlobeController globeVC) throws Exception {
        globeVC.addLayer(setupImageLayer(ConfigOptions.TestType.GlobeTest, globeVC));
//        globeVC.setZoomLimits(0.033, 1.03);
        globeVC.animatePositionGeo(-3.6704803, 40.5023056, 0.7, 1.0);
//		globeVC.setZoomLimits(0.0,1.0);



        VectorInfo vectorInfo = new VectorInfo();
        vectorInfo.setColor(Color.RED);
        vectorInfo.setLineWidth(4f);



//        VectorObject vectorObject = new VectorObject();
//		Point3d[] pointsArray = {new Point3d(Point2d.FromDegrees(151.177216, -33.9461098), 0.0), new Point3d(Point2d.FromDegrees(116.092591, -1.870512), 0.0)};
        Point3d[] pointsArray = {new Point3d(Point2d.FromDegrees(151.177216, -33.9461098), 0.0), new Point3d(Point2d.FromDegrees(114.1095, 22.3964), 0.0)};
        Point3d[] pointsArray3 = {new Point3d(Point2d.FromDegrees(151.177216, -33.9461098), 0.0), new Point3d(Point2d.FromDegrees( 103.8198, 1.3521), 0.0)};
        Point3d[] pointsArray2 = {new Point3d(Point2d.FromDegrees(151.177216, -33.9461098), 0.0), new Point3d(Point2d.FromDegrees(-149.4937, 64.2008), 0.0)};
        Point3d[] pointsArray4 = {new Point3d(Point2d.FromDegrees(151.177216, -33.9461098), 0.0), new Point3d(Point2d.FromDegrees(149.1300, -35.2809), 0.0)};
//		vectorObject.addLinear(pointsArray);
//
//        for (int i = 0; i < 100; i++) {
//            VectorObject vectorObject = new VectorObject();
//            vectorObject.addLinear3d(pointsArray);
//        vectorObject.subdivideToGlobeGreatCircle(0.0000001);
//        globeVC.addVector(vectorObject, vectorInfo, MaplyBaseController.ThreadMode.ThreadAny, 0.9f);
//        }

//        VectorObject vectorObject = new VectorObject();
//        vectorObject.addLinear3d(pointsArray);
////        vectorObject.subdivideToGlobeGreatCircle(0.001);
//        vectorObject.subdivideToGlobeGreatCircle(0.0000001);

        createVectorObject(globeVC, pointsArray);
        createVectorObject(globeVC, pointsArray2);
        createVectorObject(globeVC, pointsArray3);
        createVectorObject(globeVC, pointsArray4);
//        globeVC.addVector(vectorObject, vectorInfo, MaplyBaseController.ThreadMode.ThreadAny, 0.5f);

        return true;
    }
    private void createVectorObject(GlobeController globeVC, Point3d[] point3ds) {
        VectorObject vectorObject = new VectorObject();

        VectorInfo vectorInfo = new VectorInfo();
        vectorInfo.setColor(Color.RED);
        vectorInfo.setLineWidth(4f);


        vectorObject.addLinear3d(point3ds);
        vectorObject.subdivideToGlobeGreatCircle(0.0000001);
        globeVC.addVector(vectorObject, vectorInfo, MaplyBaseController.ThreadMode.ThreadAny, 0.5f);
    }

    @Override
    public boolean setUpWithMap(MapController mapVC) {
        mapVC.addLayer(setupImageLayer(ConfigOptions.TestType.MapTest, mapVC));
        mapVC.animatePositionGeo(-3.6704803, 40.5023056, 5, 1.0);
        mapVC.setAllowRotateGesture(true);
//		mapVC.setZoomLimits(0.0,1.0);
        return true;
    }
}
