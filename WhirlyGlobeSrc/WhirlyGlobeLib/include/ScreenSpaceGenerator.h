/*
 *  ScreenSpaceGenerator.h
 *  WhirlyGlobeLib
 *
 *  Created by Steve Gifford on 5/8/12.
 *  Copyright 2011-2012 mousebird consulting. All rights reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#import <math.h>
#import <set>
#import <map>
#import "Identifiable.h"
#import "Drawable.h"
#import "Generator.h"

namespace WhirlyKit 
{

/// Name of the shared screen space generate that a scene creates on startup
#define kScreenSpaceGeneratorShared "SharedScreenSpaceGenerator"
    
/** The Screen Space Generator keeps a list of objects in world space
    that need to be projected to the screen and drawn.  Overlays, basically.
 */
class ScreenSpaceGenerator : public Generator
{
public:
    ScreenSpaceGenerator(const std::string &name,Point2f margin);
    virtual ~ScreenSpaceGenerator();
    
    /// Generate drawables for the current frame
    void generateDrawables(WhirlyKitRendererFrameInfo *frameInfo,std::vector<DrawableRef> &drawables,std::vector<DrawableRef> &screenDrawables);
    
    typedef std::map<SimpleIdentity,BasicDrawable *> DrawableMap;
    
    /// A simple geometric representation used in shapes
    /// We do it this way so we can have multiple 
    class SimpleGeometry
    {
    public:
        SimpleGeometry();
        SimpleGeometry(SimpleIdentity texID,RGBAColor color,const std::vector<Point2f> &coords,const std::vector<TexCoord> &texCoords);

        SimpleIdentity texID;
        RGBAColor color;
        std::vector<Point2f> coords;
        std::vector<TexCoord> texCoords;
    };

    /** Simple convex shape to be drawn on the screen.
        It has a texture and a list of vertices as well as
        the usual minVis/maxVis values and draw priority.
      */
    class ConvexShape : public Identifiable
    {
    public:
        ConvexShape();

        /// Center location
        Point3f worldLoc;
        /// If true we'll use the rotation.  If not, we won't.
        bool useRotation;
        /// Rotation clockwise from north
        float rotation;
        NSTimeInterval fadeUp,fadeDown;
        int drawPriority;
        float minVis,maxVis;
        Point2f offset;
        
        /// List of geometry we'll transform to the destination
        std::vector<SimpleGeometry> geom;
    };
    
    /// Used to track the screen location of a single shape, by ID
    typedef struct
    {
        SimpleIdentity shapeID;
        Point2f screenLoc;
    } ProjectedPoint;

    /// Called by the marker generator build the geometry
    void addToDrawables(ConvexShape *,WhirlyKitRendererFrameInfo *frameInfo,DrawableMap &drawables,Mbr &frameMbr,std::vector<ProjectedPoint> &projPts);
    
    /// Called by the render to add shapes from a layer
    void addConvexShapes(std::vector<ConvexShape *> shape);
    
    /// Remove a single convex shape by ID
    void removeConvexShape(SimpleIdentity shapeID);
    
    /// Called by the render to remove zero or more shapes
    void removeConvexShapes(std::vector<SimpleIdentity> &shapeIDs);
    
    /// Return a convex shape.  Only used by the change request objects.
    ConvexShape *getConvexShape(SimpleIdentity shapeId);
    
    /// Get the projected points from the last frame.
    /// This will lock, make a copy and unlock so go wild.
    void getProjectedPoints(std::vector<ProjectedPoint> &projPoints);
    
protected:
    typedef std::set<ConvexShape *,IdentifiableSorter> ConvexShapeSet;
    ConvexShapeSet convexShapes;
    Point2f margin;
	pthread_mutex_t projectedPtsLock;
    std::vector<ProjectedPoint> projectedPoints;
};
    
/** A Screen Space Generator Add Request comes from a layer that needs to
    display objects in 2D.
 */
class ScreenSpaceGeneratorAddRequest : public GeneratorChangeRequest
{
public:
    /// Construct with a screen space generator ID and a convex shape to display
    ScreenSpaceGeneratorAddRequest(SimpleIdentity genID,ScreenSpaceGenerator::ConvexShape *);
    /// Construct with a list of convex shapes to display
    ScreenSpaceGeneratorAddRequest(SimpleIdentity genID,const std::vector<ScreenSpaceGenerator::ConvexShape *> &);
    ~ScreenSpaceGeneratorAddRequest();
    
    virtual void execute2(Scene *scene,NSObject<WhirlyKitESRenderer> *renderer,Generator *gen);

protected:
    std::vector<ScreenSpaceGenerator::ConvexShape *> shapes;
};
    
/** Remove one or more screen space objects.
 */
class ScreenSpaceGeneratorRemRequest : public GeneratorChangeRequest
{
public:
    /// Construct with a single shape ID to remove.
    ScreenSpaceGeneratorRemRequest(SimpleIdentity genID,SimpleIdentity shapeID);
    /// Construct with a list of shape IDs to remove
    ScreenSpaceGeneratorRemRequest(SimpleIdentity genID,const std::vector<SimpleIdentity> &);
    ~ScreenSpaceGeneratorRemRequest();
    
    virtual void execute2(Scene *scene,NSObject<WhirlyKitESRenderer> *renderer,Generator *gen);    

protected:
    std::vector<SimpleIdentity> shapeIDs;
};
    
/** Change the fade values on one or more shapes.  This would be in
    preparation for deleting them, usually.
 */
class ScreenSpaceGeneratorFadeRequest : public GeneratorChangeRequest
{
public:
    /// Construct with the IDs for the generator and shape and the fade up/down absolute times
    ScreenSpaceGeneratorFadeRequest(SimpleIdentity genID,SimpleIdentity shapeID,NSTimeInterval fadeUp,NSTimeInterval fadeDown);
    /// Construct with the ID for the generator, a list of IDs for shapes and the fade up/down absolute times
    ScreenSpaceGeneratorFadeRequest(SimpleIdentity genID,const std::vector<SimpleIdentity> shapeIDs,NSTimeInterval fadeUp,NSTimeInterval fadeDown);
    ~ScreenSpaceGeneratorFadeRequest();
    
    virtual void execute2(Scene *scene,NSObject<WhirlyKitESRenderer> *renderer,Generator *gen);

protected:
    NSTimeInterval fadeUp,fadeDown;
    std::vector<SimpleIdentity> shapeIDs;
};

}
