/*
 *  MaplyViewController.h
 *  MaplyComponent
 *
 *  Created by Steve Gifford on 9/6/12.
 *  Copyright 2012-2015 mousebird consulting
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

#import <UIKit/UIKit.h>
#import "MaplyCoordinate.h"
#import "MaplyScreenMarker.h"
#import "MaplyVectorObject.h"
#import "MaplyViewTracker.h"
#import "MaplyComponentObject.h"
#import "MaplySharedAttributes.h"
#import "MaplyBaseViewController.h"

@class MaplyViewControllerLayer;
@class MaplyViewController;


/** @brief Animation State used by the MaplyViewControllerAnimationDelegate.
 @details You fill out one of these when you're implementing the animation delegate.  Return it and the view controller will set the respective values to match.
 */
@interface MaplyViewControllerAnimationState : NSObject

/// @brief Heading is calculated from due north
/// @details If not set or set to MAXFLOAT, this is ignored
@property (nonatomic) double heading;

/// @brief Height above the map
@property (nonatomic) double height;

/// @brief Position to move to on the map
@property (nonatomic) MaplyCoordinateD pos;

/// @brief If set, this is a point on the screen where pos should be.
/// @details By default this is (-1,-1) meaning the screen position is just the middle.  Otherwise, this is where the position should wind up on the screen, if it can.
@property (nonatomic) CGPoint screenPos;

/** @brief Interpolate a new state between the given states A and B.
 @details This does a simple interpolation (lat/lon, not great circle) between the two animation states.
 */
+ (nonnull MaplyViewControllerAnimationState *)Interpolate:(double)t from:(MaplyViewControllerAnimationState *__nonnull)stateA to:(MaplyViewControllerAnimationState *__nonnull)stateB;

@end

/** @brief An animation delegate that can be set on a MaplyViewController to control the view over time.
 @details Filling out these methods will get you animation callbacks at the proper time to control position, heading and height on a frame basis.
 @details You pass the resulting object in to
 */
@protocol MaplyViewControllerAnimationDelegate <NSObject>

/** @brief This method is called when the animation starts.
 @details At the animation start we collect up the various parameters of the current visual view state and pas them in via the startState.  You probably want to keep track of this for later.
 @param viewC The view controller doing the animation.
 @param startState The starting point for the visual view animation.  Cache this somewhere for your own interpolation.
 @param startTime When the animation starts (e.g. now)
 @param endTime When the animation ends.  This is an absolute value.
 */
- (void)mapViewController:(MaplyViewController *__nonnull)viewC startState:(MaplyViewControllerAnimationState *__nonnull)startState startTime:(NSTimeInterval)startTime endTime:(NSTimeInterval)endTime;

/** @brief This method is called at the beginning of every frame draw to position the viewer.
 @details This is the method that does all the work.  You need to fill out the returned MaplyViewControllerAnimationState according to whatever interpolation your'e doing based on the currentTime.
 @param viewC The view controller doing the animation.
 @param currentTime The time for this frame.  Use this rather than calculating the time yourself.
 @return The MaplyViewControllerAnimationState expressing where you want the viewer to be and where they are looking.
 */
- (nonnull MaplyViewControllerAnimationState *)mapViewController:(MaplyViewController *__nonnull)viewC stateForTime:(NSTimeInterval)currentTime;

@optional

/** @brief This method is called at the end of the animation.
 @details The map view controller calls this method when the animation is finished.  Do your cleanup here if need be.
 @param viewC The map view controller.
 */
- (void)mapViewControllerDidFinishAnimation:(MaplyViewController *__nonnull)viewC;

@end

/** @brief A simple animation delegate for moving the map around.
 @details The animation delegate support provides a lot of flexibility.  This version just provides all the standard fields and interpolates from beginning to end.
 */
@interface MaplyViewControllerSimpleAnimationDelegate : NSObject <MaplyViewControllerAnimationDelegate>

/// @brief Initialize with an animation state to copy
- (nonnull instancetype)initWithState:(MaplyViewControllerAnimationState *__nonnull)endState;

/// @brief Location at the end of the animation
@property (nonatomic) MaplyCoordinateD loc;

/// @brief Heading at the end of the animation
@property (nonatomic) double heading;

/// @brief Height at the end of the animation
@property (nonatomic) double height;

@end

/** @brief A protocol to fill out for selection and tap messages from the MaplyViewController.
    @details Fill out the protocol when you want to get back selection and tap messages.  All the methods are optional.
  */
@protocol MaplyViewControllerDelegate <NSObject>

@optional

/** @brief Called when the user taps on or near an object.
    @details You're given the object you passed in originally, such as a MaplyScreenMarker.  You can set a userObject on most of these to put your own data in there for tracking.
  */
- (void)maplyViewController:(MaplyViewController *__nonnull)viewC didSelect:(NSObject *__nonnull)selectedObj;

/** @brief User selected a given object and tapped at a given location.
    @details This is called when the user selects an object.  It differs from maplyViewController:didSelect: in that it passes on the location (in the local coordinate system) and the position on screen.
    @param viewC View Controller that saw the selection.
    @param selectedObj The object selected.  Probably one of MaplyVectorObject or MaplyScreenLabel or so on.
    @param coord Location in the local coordinate system where the user tapped.
    @param screenPt Location on screen where the user tapped.
 */
- (void)maplyViewController:(MaplyViewController *__nonnull)viewC didSelect:(NSObject *__nonnull)selectedObj atLoc:(WGCoordinate)coord onScreen:(CGPoint)screenPt;

/** @brief User selected one or more objects at a given location.
    @param viewC View Controller that saw the selection(s).
    @param selectedObjs The object(s) selected.  Probably one of MaplyVectorObject or MaplyScreenLabel or so on.
    @param coord Location in the local coordinate system where the user tapped.
    @param screenPt Location on screen where the user tapped.
  */
- (void)maplyViewController:(MaplyViewController *__nonnull)viewC allSelect:(NSArray *__nonnull)selectedObjs atLoc:(MaplyCoordinate)coord onScreen:(CGPoint)screenPt;

/** @brief User tapped at a given location.
    @details This is a tap at a specific location on the map.  This won't be called if they tapped and selected, just for taps.
  */
- (void)maplyViewController:(MaplyViewController *__nonnull)viewC didTapAt:(MaplyCoordinate)coord;

/** @brief Called when the map starts moving.
 @param viewC The map view controller.
 @param userMotion Set if this is motion being caused by the user, rather than a call to set location.
 @details This is called when something (probably the user) starts moving the map.
 */
- (void)maplyViewControllerDidStartMoving:(MaplyViewController *__nonnull)viewC userMotion:(bool)userMotion;

/** @brief Called when the map stops moving.
 @details This is called when the map stops moving.  It passes in the corners of the current viewspace.
 @param viewC The globe view controller.
 @param userMotion Set if this is motion being caused by the user, rather than a call to set location.
 @param corners An array of length 4 containing the corners of the view space (lower left, lower right, upper right, upper left).  If any of those corners does not intersect the map (think zoomed out), its values are set to MAXFLOAT.
 */
- (void)maplyViewController:(MaplyViewController *__nonnull)viewC didStopMoving:(MaplyCoordinate *__nonnull)corners userMotion:(bool)userMotion;

/** @brief Called whenever the viewpoint moves.
 @details This is called whenever the viewpoint moves.  That includes user motion as well as animations.
 @details It may be triggered as often as every frame.  If that's a problem, use one of the other variants.
 @param viewC The map view controller.
 @param corners An array of length 4 containing the corners of the view space (lower left, lower right, upper right, upper left).  If any of those corners does not intersect the globe (think zoomed out), its values are set to MAXFLOAT.
 */
- (void)maplyViewController:(MaplyViewController *__nonnull)viewC didMove:(MaplyCoordinate *__nonnull)corners;


/** @brief Called when the user taps on one of your annotations.
    @details This is called when the user taps on an annotation.
    @param annotation Which annotation they tapped on.
  */
- (void)maplyViewController:(MaplyViewController *__nonnull)viewC didTapAnnotation:(MaplyAnnotation*__nonnull)annotation;

/// Old version for compatibility.  Use tap instead.
- (void)maplyViewController:(MaplyViewController *__nonnull)viewC didClickAnnotation:(MaplyAnnotation*__nonnull)annotation __deprecated;

@end


typedef NS_ENUM(NSInteger, MaplyMapType) {
	MaplyMapType3D,
	MaplyMapTypeFlat,
};

/** @brief This view controller implements a map.
    @details This is the main entry point for displaying a 2D or 3D map.  Create one of these, fill it with data and let your users mess around with it.
    @details You can display a variety of features on the map, including tile base maps (MaplyQuadImageTilesLayer), vectors (MaplyVectorObject), shapes (MaplyShape), and others.  Check out the add calls in the MaplyBaseViewController for details.
    @details The Maply View Controller can be initialized in 3D map, 2D map mode.  The 2D mode can be tethered to a UIScrollView if you want to handle gestures that way.  That mode is very specific at the moment.
    @details To get selection and tap callbacks, fill out the MaplyViewControllerDelegate and assign the delegate.
    @details Most of the functionality is shared with MaplyBaseViewController.  Be sure to look in there first.
  */
@interface MaplyViewController : MaplyBaseViewController

/// @brief Initialize as a flat or 3D map.
- (nonnull instancetype)initWithMapType:(MaplyMapType)mapType;

/// @brief Initialize as a 3D map.
- (nonnull instancetype)init __deprecated;

/// @brief Initialize as a 2D map.
- (nonnull instancetype)initAsFlatMap __deprecated;

/** @brief Initialize as a 2D map tied to a UIScrollView.
    @details In this mode we disable all the the gestures.
    @param scrollView The UIScrollView to track.
    @param tetherView If set, we assume the scroll view is manipulating a blank UIView which we'll watch.
  */
- (nonnull instancetype)initAsTetheredFlatMap:(UIScrollView *__nonnull)scrollView tetherView:(UIView *__nullable)tetherView;

/** @brief Reset the UIScrollView for tethered mode.
    @details Occasionally we need to reset the UIScrollView and tether view.  This will do that.
    @param scrollView The UIScrollView to track.
    @param tetherView If set, we assume the scroll view is manipulating a blank UIView which we'll watch.
  */
- (void)resetTetheredFlatMap:(UIScrollView *__nonnull)scrollView tetherView:(UIView *__nullable)tetherView;

/// @brief Set if we're in 2D mode.
@property (nonatomic,readonly) bool flatMode;

/// @brief If we're in tethered flat map mode, this is the view we're monitoring for size and offset changes.
@property(nonatomic,weak) UIView *__nullable tetherView;

/// @brief If set before startup (after init), we'll turn off all gestures and work only in tethered mode.
@property(nonatomic,assign) bool tetheredMode;

/// @brief Set the coordinate system to use in display.
/// @details The coordinate system needs to be valid in flat mode.  The extents, if present, will be used to define the coordinate system origin.
/// @details nil is the default and will result in a full web style Spherical Mercator.
@property(nonatomic,strong) MaplyCoordinateSystem *__nullable coordSys;

/** @brief Set the center of the display coordinate system.
    @details This is (0,0,0) by default.  If you set it to something else all display coordinates will be offset from that origin.
    @details The option is useful when displaying small maps (of a city, say) at very high resolution.
  */
@property(nonatomic) MaplyCoordinate3d displayCenter;

/** @brief Turn the pinch (zoom) gesture recognizer on and off
    @details On by default.
  */
@property(nonatomic,assign) bool pinchGesture;

/** @brief Turn the rotate gesture recognizer on and off
    @details On by default.
 */
@property(nonatomic,assign) bool rotateGesture;

/** @brief Turn the pan gesture on and off
    @details Pan gesture is on by default
  */
@property(nonatomic,assign) bool panGesture;

/** @brief Turn the double tap to zoom gesture recognizer on and off
    @details On by default.
 */
@property(nonatomic,assign) bool doubleTapZoomGesture;

/** @brief Turn the 2 finger tap to zoom out gesture recognizer on and off
    @details On by default.
 */
@property(nonatomic,assign) bool twoFingerTapGesture;

/** @brief Turn on the double tap and drag gesture to zoom in and out.
    @details On by default.
  */
@property(nonatomic,assign) bool doubleTapDragGesture;

/** @brief If set, we use a modified pan gesture recognizer to play nice
 with the scroll view.  For the UIScrollView object, set clipsToBounds,
 pagingEnabled, and delaysContentTouches to YES, and set scrollEnabled
 and canCancelContentTouches to NO.  Add swipe gesture recognizers
 to the scroll view to control paging, and call
 requirePanGestureRecognizerToFailForGesture: for each.
 @details Off by default.
 */
@property(nonatomic,assign) bool inScrollView;

/** @brief turn the touch to cancel animation gesture on and off
    @details off by default
 */
@property(nonatomic,assign) bool cancelAnimationOnTouch;

/** @brief The current rotation away from north.
 */
@property(nonatomic,assign) float heading;

/** @brief If set, we'll automatically move to wherever the user tapped.
    @details When on we'll move the current location to wherever the user tapped if they tapped the globe.  That's true for selection as well.  On by default.
 */
@property(nonatomic,assign) bool autoMoveToTap;

/** @brief Delegate for selection and location tapping.
    @details Fill in the MaplyViewControllerDelegate and assign it here to get callbacks for object selection and tapping.
  */
@property(nonatomic,weak) NSObject<MaplyViewControllerDelegate> *__nullable delegate;

/** @brief Current height above terrain.
    @details In 3D map mode this is the height from which the user is viewing the map.  Maps are usually -PI to +PI along their horizontal edges.
  */
@property (nonatomic,assign) float height;

/** @brief 2D visual views can do some simple wrapping.  This turns that on and off (off by default).
    @details On some 2D visual views we're allowed to wrap across the edge of the world.  This will attempt to do that.
  */
@property (nonatomic,assign) bool viewWrap;

/** @brief The box the view point can be in.
 @details This is the box the view point is allowed to be within.  The view controller will constrain it to be within that box.  Coordinates are in geographic (radians).
 */
- (MaplyBoundingBox)getViewExtents;

/** @brief The box the view point can be in.
    @details This is the box the view point is allowed to be within.  The view controller will constrain it to be within that box.  Coordinates are in geographic (radians).
  */
- (void)getViewExtentsLL:(MaplyCoordinate *__nonnull)ll ur:(MaplyCoordinate *__nonnull)ur;

/** @brief The box the view point can be in.
 @details This is the box the view point is allowed to be within.  The view controller will constrain it to be within that box. Coordinates are in geographic (radians).
 */
- (void)setViewExtents:(MaplyBoundingBox)box;

/** @brief The box the view point can be in.
    @details This is the box the view point is allowed to be within.  The view controller will constrain it to be within that box. Coordinates are in geographic (radians).
 */
- (void)setViewExtentsLL:(MaplyCoordinate)ll ur:(MaplyCoordinate)ur;

/** @brief Animate to the given position over time.
    @param newPos A coordinate in geographic (lon/lat radians)
    @param howLong A time in seconds.
  */
- (void)animateToPosition:(MaplyCoordinate)newPos time:(NSTimeInterval)howLong;

/** @brief Animate to new window extents over time.
    @details This method is similar to animateToPosition:time: but only works in 2D flat map mode.
    @param windowSize The new window size we're matching.
    @param contentOffset The contentOffset from the UIScrollView.
    @param howLong The length of time to take getting there.
  */
- (void)animateToExtentsWindowSize:(CGSize)windowSize contentOffset:(CGPoint)contentOffset time:(NSTimeInterval)howLong;

/** @brief Animate the given position to the screen position over time.
    @details This is similar to animateToPosition:time: except that it will attempt to match up the screen position and the geographic position.  This is how you offset the location you're looking at.
    @details If it's impossible to move newPos to loc, then nothing happens.
    @param newPos The geographic position (lon/lat in radians) to move to.
    @param loc The location on the screen where we'd like it to go.
    @param howLong How long in seconds to take getting there.
  */
- (bool)animateToPosition:(MaplyCoordinate)newPos onScreen:(CGPoint)loc time:(NSTimeInterval)howLong;

/** @brief Animate the given position and height to the screen position over time.
 @details This is similar to animateToPosition:time: but it also takes a height paramater
 @param newPos The geographic position (lon/lat in radians) to move to.
 @param newHeight  the view point height above the map.
 @param howLong How long in seconds to take getting there.
 */
- (void)animateToPosition:(MaplyCoordinate)newPos height:(float)newHeight time:(NSTimeInterval)howLong;

/** @brief Animate to the given position, heading and height over time.
 @param newPos A coordinate in geographic (lon/lat radians)
 @param newHeight New height to animate to.
 @param newHeading New heading to finish on.
 @param howLong A time interval in seconds.
 */
- (bool)animateToPosition:(MaplyCoordinate)newPos height:(float)newHeight heading:(float)newHeading time:(NSTimeInterval)howLong;

/** @brief Animate to the given position, heading and height over time.
 @param newPos A coordinate in geographic (lon/lat radians) (double precision)
 @param newHeight New height to animate to. (double)
 @param newHeading New heading to finish on. (double)
 @param howLong A time interval in seconds.
 */
- (bool)animateToPositionD:(MaplyCoordinateD)newPos height:(double)newHeight heading:(double)newHeading time:(NSTimeInterval)howLong;

/** @brief Animate to the given position, screen position, heading and height over time.
 @details If it's impossible to move newPos to loc, then nothing happens.
 @param newPos A coordinate in geographic (lon/lat radians)
 @param loc The location on the screen where we'd like it to go.
 @param newHeight New height to animate to.
 @param newHeading New heading to finish on.
 @param howLong A time interval in seconds.
 */
- (bool)animateToPosition:(MaplyCoordinate)newPos onScreen:(CGPoint)loc height:(float)newHeight heading:(float)newHeading time:(NSTimeInterval)howLong;

/** @brief Set the center of the screen to the given position immediately.
    @param newPos The geographic position (lon/lat in radians) to move to.
  */
- (void)setPosition:(MaplyCoordinate)newPos;

/** @brief Set the center of the screen and the height offset immediately.
    @param newPos The geographic position (lon/lat in radians) to move to.
    @param height Height the view point above the map.
  */
- (void)setPosition:(MaplyCoordinate)newPos height:(float)height;

/** @brief Return the current center position
 */
- (MaplyCoordinate)getPosition;

/** @brief Return the current view point's height above the map.
 */
- (float)getHeight;

/** @brief Return the current center position and height.
    @param pos The center of the screen in geographic (lon/lat in radians).
    @param height The current view point's height above the map.
  */
- (void)getPosition:(MaplyCoordinate *__nonnull)pos height:(float *__nonnull)height;


/** @brief Set the viewing state all at once
 @details This sets the position, height, screen position and heading all at once.
 */
- (void)setViewState:(MaplyViewControllerAnimationState *__nonnull)viewState;

/** @brief Make a MaplyViewControllerAnimationState object from the current view state.
 @details This returns the current view parameters in a single MaplyViewControllerAnimationState.
 */
- (nullable MaplyViewControllerAnimationState *)getViewState;

/** @brief Return the closest a viewer is allowed to get to the map surface.
 @return FLT_MIN if there's no pitchDelegate set
 */
- (float)getMinZoom;

/** @brief Return the farthest away a viewer is allowed to get from the map surface
 @return FLT_MIN if there's no pitchDelegate set
 */
- (float)getMaxZoom;

/** @brief Return the zoom limits for 3D map mode.
    @param minHeight The closest a viewer is allowed to get to the map surface.
    @param maxHeight The farthest away a viewer is allowed to get from the map surface.
  */
- (void)getZoomLimitsMin:(float *__nonnull)minHeight max:(float *__nonnull)maxHeight;

/** @brief Set the zoom limits for 3D map mode.
    @param minHeight The closest a viewer is allowed to get to the map surface.
    @param maxHeight The farthest away a viewer is allowed to get from the map surface.
 */
- (void)setZoomLimitsMin:(float)minHeight max:(float)maxHeight;

/** @brief Return the geographic (lon/lat radians) coordinate in radians for a given screen point.
    @return Returns the geo coordinate corresponding to a given screen point in radians.
 */
- (MaplyCoordinate)geoFromScreenPoint:(CGPoint)point;

/** @brief Find a height that shows the given bounding box.
    @details This method will search for a height that shows the given bounding box within the view.  The search is inefficient, so don't call this a lot.
    @param bbox The bounding box (in radians) we're trying to view.
    @param pos Where the view will be looking.
  */
- (float)findHeightToViewBounds:(MaplyBoundingBox)bbox pos:(MaplyCoordinate)pos;

/** @brief Find a height that shows the given bounding box.
 @details This method will search for a height that shows the given bounding box within the view.  The search is inefficient, so don't call this a lot.
 @details This version takes a margin to add around the outside of the area.
 @param bbox The bounding box (in radians) we're trying to view.
 @param pos Where the view will be looking.
 @param marginX Horizontal boundary around the area
 @param marginY Vertical boundary around the area
 */
- (float)findHeightToViewBounds:(MaplyBoundingBox)bbox pos:(MaplyCoordinate)pos marginX:(double)marginX marginY:(double)marginY;

/**
 @brief Return the extents of the current view
 @return Returns the Bounding Box (in radians) corresponding to the current view
 */
- (MaplyBoundingBox)getCurrentExtents;

/**
 @brief Make a gesture recognizer's success depend on the pan gesture
 recognizer's failure.
 @details When using the map view within a scroll view, add swipe gesture
 recognizers to the scroll view to control paging, and call this method
 for each.  See also the inScrollView property and its comment.
 @param other The other, subordinate gesture recognizer.
 */
- (void)requirePanGestureRecognizerToFailForGesture:(UIGestureRecognizer *__nullable)other;

@end