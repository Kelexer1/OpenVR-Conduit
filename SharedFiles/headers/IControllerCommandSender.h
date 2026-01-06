#pragma once
#include <cstdint>

#include "ControllerTypes.h"

/**
 * @brief An abstract interface for sending and recieving data from the ControllerHooker driver
 * 
 * This class should *not* be instantiated directly. Instead, use the provided ControllerCommandSender class
 */
class IControllerPoseCommandSender {
public:
	virtual ~IControllerPoseCommandSender() = default;

	///=========================
	/// Pose Manipulations
	///=========================

	/**
	 * @brief Fully overrides the stored pose for the controller with deviceIndex
	 * 
	 * Note that this will override all stored positions, rotations, etc.
	 * 
	 * @param deviceIndex The index of the controller
	 * @param newPose the pose that should override the current pose
	 */
	virtual void setPose(const uint32_t deviceIndex, ControllerPose& pose) = 0;

	/**
	 * @brief Fills the vecPosition of the stored pose for the controller with deviceIndex
	 *
	 * Leaves the rest of the pose struct untouched
	 * 
	 * @param deviceIndex The index of the controller
	 * @param newPosition The new position
	 */
	virtual void setPosition(const uint32_t deviceIndex, ControllerVector3& newPosition) = 0;

	/**
	 * @brief Fills the qRotation of the stored pose for the controller with deviceIndex
	 *
	 * Leaves the rest of the pose struct untouched
	 * 
	 * @param deviceIndex The index of the controller
	 * @param newRotation The new rotation
	 */
	virtual void setRotation(const uint32_t deviceIndex, ControllerQuaternion& newRotation) = 0;

	/**
	 * @brief Fills the vecVelocity of the stored pose for the controller with deviceIndex
	 *
	 * Leaves the rest of the pose struct untouched
	 * 
	 * @param deviceIndex The index of the controller
	 * @param newVelocity The new velocity
	 */
	virtual void setVelocity(const uint32_t deviceIndex, ControllerVector3& newVelocity) = 0;

	/**
	 * @brief Fills the vecAngularVelocity of the stored pose for the controller with deviceIndex
	 *
	 * Leaves the rest of the pose struct untouched
	 * 
	 * @param deviceIndex The index of the controller
	 * @param newAngularVelocity The new angular velocity
	 */
	virtual void setAngularVelocity(const uint32_t deviceIndex, ControllerVector3& newAngularVelocity) = 0;

	/**
	 * @brief Fills the vecAcceleration of the stored pose for the controller with deviceIndex
	 *
	 * Leaves the rest of the pose struct untouched
	 * 
	 * @param deviceIndex The index of the controller
	 * @param newAcceleration The new acceleration
	 */
	virtual void setAcceleration(const uint32_t deviceIndex, ControllerVector3& newAcceleration) = 0;

	/**
	 * @brief Fills the vecAngularAcceleration of the stored pose for the controller with deviceIndex
	 *
	 * Leaves the rest of the pose struct untouched
	 * 
	 * @param deviceIndex The index of the controller
	 * @param newAngularAcceleration The new angular acceleration
	 */
	virtual void setAngularAcceleration(const uint32_t deviceIndex, ControllerVector3& newAngularAcceleration) = 0;

	/**
	 * @brief Returns the stored pose of this controller
	 *
	 * Note that this pose is NOT nesessarily the actual pose of the controller,
	 * it is essentially a queued pose that can be applied to the controller
	 * 
	 * @param deviceIndex The index of the controller
	 * @return A pointer to the controller pose, nullptr if something went wrong
	 */
	virtual ControllerPose* getPose(const uint32_t deviceIndex) const = 0;

	/**
	 * @brief Returns the *actual* position of this controller
	 *
	 * Note that this pose may not equal the stored pose of the controller
	 * 
	 * @param deviceIndex The index of the controller
	 * @return A pointer to the controllers natural pose, nullptr if something went wrong
	 */
	virtual ControllerPose* getNaturalPose(const uint32_t deviceIndex) const = 0;

	///=========================
	/// Override control
	///=========================

	/**
	 * @brief Returns the stored pose of the controller with deviceIndex
	 *
	 * Note that this pose is NOT nesessarily the actual pose of the controller,
	 * it is essentially a queued pose that can be applied to the controller
	 * 
	 * @param deviceIndex The index of the controller
	 * @param overridePose A boolean representing whether the controller should use it's overriden pose (true) or it's natural pose (false)
	 */
	virtual void setUseCustomPose(const uint32_t deviceIndex, const bool shouldOverridePose) = 0;

	/**
	 * @brief Returns whether the controller is using a custom pose
	 *
	 * Note that this pose may not equal the stored pose of the controller
	 * 
	 * @param deviceIndex The index of the controller
	 * @return A pointer to the result, nullptr if something went wrong
	 */
	virtual bool* getUseCustomPose(const uint32_t deviceIndex) const = 0;
};