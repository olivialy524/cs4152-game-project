//
//  LMLumiaModel.h

//  This file will be used as reference for building the body of the main character Lumia
//
//  This file is based on the CS 4152 PlatformDemo by Walker White and Anthony Perello
//  Version: 3/5/21
//
#ifndef __LUMIA_MODEL_H__
#define __LUMIA_MODEL_H__
#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>
#include "LumiaNode.h"

#pragma mark Lumia Model
/**
* Player avatar for the plaform game.
*
* Note that this class uses a capsule shape, not a rectangular shape.  In our
* experience, using a rectangular shape for a character will regularly snag
* on a platform.  The round shapes on the end caps lead to smoother movement.
*/
class LumiaModel : public cugl::physics2::WheelObstacle {
#pragma mark Constants and Enums
protected:
    #define SIGNUM(x)  ((x > 0) - (x < 0))
    
    /** Debug color for the sensor */
    #define DEBUG_COLOR     Color4::YELLOW

    /** The base density of the character */
    static constexpr float LUMIA_DENSITY = 0.10f;
    /** The restitution of the character */
    static constexpr float LUMIA_RESTITUTION = 0.45f;
    /** The factor to multiply by the input */
    static constexpr float LUMIA_FORCE = 20.0f;
    /** The amount to slow the character down */
    static constexpr float LUMIA_DAMPING = 3.0f;
    /** The maximum character speed */
    static constexpr float LUMIA_MAXVELOCITY = 20.0f;

public:
    enum LumiaState {
        /** When Lumia is still or rolling */
        Idle,
        /** When Lumia is splitting */
        Splitting,
        /** When Lumia is merging */
        Merging
    };

    struct LumiaSize {
        /** Size of Lumia body */
        float radius;
        /** Density of Lumia body */
        float density;
    };

    static std::vector<LumiaSize> sizeLevels;

    
#pragma mark Attributes
    
private:
	/** This macro disables the copy constructor (not allowed on physics objects) */
	CU_DISALLOW_COPY_AND_ASSIGN(LumiaModel);

protected:
	/** The current velocity of Lumia */
	cugl::Vec2 _velocity;
    /** Whether we are actively launching */
    bool _isLaunching;
	/** Whether our feet are on the ground */
	bool _isGrounded;
    /** Whether Lumia is splitting into two */
    bool _isSplitting;
    
    bool _inCoolDown;
    /* Whether or not the Lumia body is due to be or has been removed */
    bool _removed;
    
    bool _dying;
    /** Radius of Lumia's body */
    float _radius;
	/** Ground sensor to represent our feet */
	b2Fixture*  _sensorFixture;
    /** Ground sensor to represent our feet */
    b2Fixture*  _sensorFixture2;
	/** Reference to the sensor name (since a constant cannot have a pointer) */
	std::string _launchSensorName;
    /** Reference to the sensor name (since a constant cannot have a pointer) */
    std::string _frictionSensorName;
	/** The node for debugging the sensor */
	std::shared_ptr<cugl::scene2::WireNode> _sensorNode;
    
    /** The node for debugging the sensor */
    std::shared_ptr<cugl::scene2::WireNode> _sensorNode2;
	/** The scene graph node for Lumia. */
	std::shared_ptr<LumiaNode> _sceneNode;
    
    Vec2 _lastPosition;

	/** The scale between the physics world and the screen (MUST BE UNIFORM) */
	float _drawScale;
    
    /** The current state of this Lumia*/
    LumiaState _state;

    /** The current size level of this Lumia body */
    int _sizeLevel;

	/**
	* Redraws the outline of the physics fixtures to the debug node
	*
	* The debug node is use to outline the fixtures attached to this object.
	* This is very useful when the fixtures have a very different shape than
	* the texture (e.g. a circular shape attached to a square texture).
	*/
	virtual void resetDebug() override;
    
    bool _isOnStickyWall;
    
    bool _isRolling;
    
    Vec2 _stickDirection;
    
    bool _isOnButton;

public:
    
#pragma mark Hidden Constructors
    /**
     * Creates a degenerate Lumia object.
     *
     * This constructor does not initialize any of the Lumia values beyond
     * the defaults.  To use a LumiaModel, you must call init().
     */
    LumiaModel() : cugl::physics2::WheelObstacle() { }
    
    
    /**
     * Destroys this LumiaModel, releasing all resources.
     */
    virtual ~LumiaModel(void) { dispose(); }
    
    /**
     * Disposes all resources and assets of this LumiaModel
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a LumiaModel may not be used until it is initialized again.
     */
    void dispose();
    
    /**
     * Initializes a new Lumia at the origin.
     *
     * The Lumia is a unit square scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init() override { return init(cugl::Vec2::ZERO, 1.0f, 1.0f); }
    
    /**
     * Initializes a new Lumia at the given position.
     *
     * The Lumia is unit square scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const cugl::Vec2 pos) override { return init(pos, 1.0f, 1.0f); }
    
    /**
     * Initializes a new Lumia at the given position.
     *
     * The Lumia has the given size, scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     * @param size  The size of the Lumia in world units
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const cugl::Vec2 pos, float radius) override {
        return init(pos, radius, 1.0f);
    }
    
    /**
     * Initializes a new Lumia at the given position.
     *
     * The Lumia is sized according to the given drawing scale.
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     * @param size  The size of the Lumia in world units
     * @param scale The drawing scale (world to screen)
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const cugl::Vec2& pos, float radius, float scale);
    
    /**
     * Sets the textures for this lumia.
     *
     * @param lumia      The texture for the lumia filmstrip
     */
    void setTextures(const std::shared_ptr<cugl::Texture>& idle, const std::shared_ptr<cugl::Texture>& splitting, const std::shared_ptr<cugl::Texture>& death,
                     const std::shared_ptr<Texture>& indicator);

    void setDrawScale(float scale);
    
#pragma mark -
#pragma mark Static Constructors
	/**
	 * Creates a new Lumia at the origin.
	 *
	 * The Lumia is a unit square scaled so that 1 pixel = 1 Box2d unit
	 *
	 * The scene graph is completely decoupled from the physics system.
	 * The node does not have to be the same size as the physics body. We
	 * only guarantee that the scene graph node is positioned correctly
	 * according to the drawing scale.
	 *
	 * @return  A newly allocated LumiaModel at the origin
	 */
	static std::shared_ptr<LumiaModel> alloc() {
		std::shared_ptr<LumiaModel> result = std::make_shared<LumiaModel>();
		return (result->init() ? result : nullptr);
	}

	/**
	 * Creates a new Lumia at the given position.
	 *
	 * The Lumia is a unit square scaled so that 1 pixel = 1 Box2d unit
	 *
	 * The scene graph is completely decoupled from the physics system.
	 * The node does not have to be the same size as the physics body. We
	 * only guarantee that the scene graph node is positioned correctly
	 * according to the drawing scale.
	 *
     * @param pos   Initial position in world coordinates
	 *
	 * @return  A newly allocated LumiaModel at the given position
	 */
	static std::shared_ptr<LumiaModel> alloc(const cugl::Vec2& pos) {
		std::shared_ptr<LumiaModel> result = std::make_shared<LumiaModel>();
		return (result->init(pos) ? result : nullptr);
	}

    /**
	 * Creates a new Lumia at the given position.
	 *
     * The Lumia has the given size, scaled so that 1 pixel = 1 Box2d unit
	 *
 	 * The scene graph is completely decoupled from the physics system.
	 * The node does not have to be the same size as the physics body. We
	 * only guarantee that the scene graph node is positioned correctly
	 * according to the drawing scale.
	 *
	 * @param pos   Initial position in world coordinates
     * @param size  The size of the Lumia in world units
	 *
	 * @return  A newly allocated LumiaModel at the given position with the given scale
	 */
	static std::shared_ptr<LumiaModel> alloc(const cugl::Vec2& pos, float radius) {
		std::shared_ptr<LumiaModel> result = std::make_shared<LumiaModel>();
		return (result->init(pos, radius) ? result : nullptr);
	}

	/**
	 * Creates a new Lumia at the given position.
	 *
	 * The Lumia is sized according to the given drawing scale.
	 *
	 * The scene graph is completely decoupled from the physics system.
	 * The node does not have to be the same size as the physics body. We
	 * only guarantee that the scene graph node is positioned correctly
	 * according to the drawing scale.
	 *
	 * @param pos   Initial position in world coordinates
     * @param size  The size of the Lumia in world units
	 * @param scale The drawing scale (world to screen)
	 *
	 * @return  A newly allocated LumiaModel at the given position with the given scale
	 */
	static std::shared_ptr<LumiaModel> alloc(const cugl::Vec2& pos, float radius, float scale) {
		std::shared_ptr<LumiaModel> result = std::make_shared<LumiaModel>();
		return (result->init(pos, radius, scale) ? result : nullptr);
	}
    

#pragma mark -
#pragma mark Animation
    /**
     * Returns the scene graph node representing this LumiaModel.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @return the scene graph node representing this LumiaModel.
     */
	const std::shared_ptr<LumiaNode>& getSceneNode() { return _sceneNode; }
    
    
#pragma mark -
#pragma mark Attribute Properties
    /**
     * Returns current velocity of Lumia.
     *
     * This is the result of drag-and-release input.
     *
     * @return velocity of Lumia.
     */
    cugl::Vec2 getVelocity() const { return _velocity; }

    /**
     * Returns current position of Lumia (used for trajectory prediction).
     *
     * @return position of Lumia.
     */
    b2Vec2 getPos() const { return _body->GetPosition(); }
    
    
    Vec2 getAvatarPos() const {
        return Vec2(getPosition().x*_drawScale, getPosition().y*_drawScale);
        
    }
    
    Vec2 getLastPosition() const {
        return _lastPosition;
    }
    
    void setState(LumiaState state){
        _state = state;
        LumiaNode::LumiaAnimState s;
        switch (_state){
            case LumiaState::Idle:{
                s = LumiaNode::LumiaAnimState::Idle;
                break;
            }
            case LumiaState::Splitting:{
                _sceneNode->setAngle(0.0f);
                s = LumiaNode::LumiaAnimState::Splitting;
                break;
            }
            case LumiaState::Merging:{
                s = LumiaNode::LumiaAnimState::Merging;
                break;
            }
        }
        _sceneNode->setAnimState(s);
    }
    
    LumiaState getState(){
        return _state;
    }
    
    /** Returns the next biggest size level this Lumia body can grow to */
    int getBiggerSizeLevel() { 
        if (_sizeLevel == LumiaModel::sizeLevels.size() - 1) {
            return _sizeLevel;
        } else {
            return _sizeLevel + 1;
        }
    };

    /** Returns the next smallest size level this Lumia body can grow to */
    int getSmallerSizeLevel() {
        if (_sizeLevel == 0) {
            return _sizeLevel;
        } else {
            return _sizeLevel - 1;
        }
    };

    
    /** Returns size level of this Lumia body */
    int getSizeLevel() { return _sizeLevel; };

    /** Sets size level of this Lumia body */
    void setSizeLevel(int value) { _sizeLevel = value; };

    /**
     * Sets velocity of Lumia.
     *
     * This is the result of drag-and-release input.
     *
     * @param value velocity of Lumia.
     */
    void setVelocity(cugl::Vec2 value) { _velocity = value; }

    /**
     * Returns true if the Lumia is actively launching.
     *
     * @return true if the Lumia is actively launching.
     */
    bool isLaunching() const { return _isLaunching; }

    /**
     * Sets whether the Lumia is actively launching.
     *
     * @param value whether the Lumia is actively launching.
     */
    void setLaunching(bool value) { _isLaunching = value; }
    
    bool isDoneSplitting() const {
        if (_sceneNode!=nullptr){
            return _sceneNode->getAnimState() == LumiaNode::LumiaAnimState::SplitFinished;
        }
        return false;
        
    }
    
    bool isOnStickyWall() {
        return _isOnStickyWall;
    }
    
    void setOnStickyWall(bool value){
        _isOnStickyWall = value;
    }
    
    bool isOnButton() {
        return _isOnButton;
    }
    
    void setOnButton(bool value){
        _isOnButton = value;
    }
    
    void setInCoolDown(bool value){
        _inCoolDown = value;
    }
    
    bool getInCoolDown(){
        return _inCoolDown;
    }
    
    void unStick(){
        setOnStickyWall(false);
        setOnButton(false);
        _stickDirection = Vec2::ZERO;
    }
    
    void setStickDirection(Vec2 dir){
        _stickDirection = dir.normalize() * 0.6f;
    }
    /**
     * Returns true if the Lumia is on the ground.
     *
     * @return true if the Lumia is on the ground.
     */
    bool isGrounded() const { return _isGrounded; }
    
    /**
     * Sets whether the Lumia is on the ground.
     *
     * @param value whether the Lumia is on the ground.
     */
    void setGrounded(bool value) { _isGrounded = value; }
    /**
     * Returns true if the Lumia is on the ground.
     *
     * @return true if the Lumia is on the ground.
     */
    bool isRolling() const { return _isRolling; }
    
    /**
     * Sets whether the Lumia is on the ground.
     *
     * @param value whether the Lumia is on the ground.
     */
    void setRolling(bool value) { _isRolling = value; }
    
    /**
     * Returns whether the Lumia is dying
     *
     * @param value whether the Lumia is dying
     */
    bool isDying() const { return _dying; }
    
    /**
     * Sets whether the Lumia is dying
     *
     * @param value whether the Lumia is dying
    */
    void setDying(bool value) {
        _dying = value;
        _sceneNode->setAnimState(LumiaNode::LumiaAnimState::Dying);
    }
    /**
     * Returns whether the Lumia is dead
     *
     * @param value whether the Lumia is dead
     */
    bool isDead() const {
        if (_sceneNode != nullptr){
            return _sceneNode->getAnimState() == LumiaNode::LumiaAnimState::Dead;
        }
        return false;
    }
    /**
     * Returns how much force to apply to get the Lumia moving
     *
     * Multiply this by the input to get the movement value.
     *
     * @return how much force to apply to get the Lumia moving
     */
    float getForce() const { return LUMIA_FORCE; }
    
    /**
     * Returns ow hard the brakes are applied to get a Lumia to stop moving
     *
     * @return ow hard the brakes are applied to get a Lumia to stop moving
     */
    float getDamping() const { return LUMIA_DAMPING; }
    
    /**
     * Returns the upper limit on Lumia's velocity.
     *
     * @return the upper limit on Lumia'v velocity.
     */
    float getMaxVelocity() const { return LUMIA_MAXVELOCITY; }
    
    /**
     * Returns the name of the ground sensor
     *
     * This is used by ContactListener
     *
     * @return the name of the ground sensor
     */
    std::string* getLaunchSensorName() { return &_launchSensorName; }
    
    std::string* getFrictionSensorName() { return &_frictionSensorName; }
    
    void setRemoved(bool value) { _removed = value; }

    /* Returns whether or not this energy item is due to be or has been removed */
    bool getRemoved() { return _removed; }

    
#pragma mark -
#pragma mark Physics Methods
    /**
     * Creates the physics Body(s) for this object, adding them to the world.
     *
     * This method overrides the base method to keep your ship from spinning.
     *
     * @param world Box2D world to store body
     *
     * @return true if object allocation succeeded
     */
    void createFixtures() override;
    
    /**
     * Release the fixtures for this body, reseting the shape
     *
     * This is the primary method to override for custom physics objects.
     */
    void releaseFixtures() override;
    
    /**
     * Updates the object's physics state (NOT GAME LOGIC).
     *
     * We use this method to reset cooldowns.
     *
     * @param delta Number of seconds since last animation frame
     */
    void update(float dt) override;
    
    /**
     * Applies the force to the body of this Lumia
     *
     * This method should be called after the force attribute is set.
     */
    void applyForce();
};

#endif /* __LUMIA_MODEL_H__ */
