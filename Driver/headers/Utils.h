#pragma once
#include <openvr_driver.h>

#include "DeviceTypes.h"

/**
 * @brief Returns the device class for a given device index (DEPRECATED)
 * @param deviceIndex The device index of the device
 * @return The class of the device
 */
vr::ETrackedDeviceClass getDeviceClass(uint32_t deviceIndex);

/**
 * @brief Converts an OpenVR DriverPose_t to a DevicePose
 * @param pose The OpenVR driver pose to convert
 * @return The converted DevicePose
 */
DevicePose FromDriverPose(const vr::DriverPose_t& pose);

/**
 * @brief Converts a DevicePose to an OpenVR DriverPose_t
 * @param cp The DevicePose to convert
 * @return The converted OpenVR driver pose
 */
vr::DriverPose_t ToDriverPose(const DevicePose& cp);

/**
 * @brief Converts a DeviceMatrix34 to an OpenVR HmdMatrix34_t
 * @param mat The DeviceMatrix34 to convert
 * @return The converted OpenVR matrix
 */
vr::HmdMatrix34_t ToHmdMatrix34(const DeviceMatrix34& mat);

/**
 * @brief Converts an OpenVR HmdMatrix34_t to a DeviceMatrix34
 * @param mat The OpenVR matrix to convert
 * @return The converted DeviceMatrix34
 */
DeviceMatrix34 FromHmdMatrix34(const vr::HmdMatrix34_t& mat);

/**
 * @brief Converts a SkeletonInput to an array of OpenVR VRBoneTransform_t
 * @param input The SkeletonInput containing bone data
 * @param outTransforms Pointer to the output array of bone transforms
 */
void ToVRBoneTransforms(const SkeletonInput& input, vr::VRBoneTransform_t* outTransforms);

/**
 * @brief Converts an array of OpenVR VRBoneTransform_t to a SkeletonInput
 * @param transforms Pointer to the array of bone transforms
 * @param count The number of bone transforms in the array
 * @param outInput The output SkeletonInput to populate
 */
void FromVRBoneTransforms(const vr::VRBoneTransform_t* transforms, uint32_t count, SkeletonInput& outInput);

/**
 * @brief Converts an EyeTrackingData to an OpenVR VREyeTrackingData_t
 * @param data The EyeTrackingData to convert
 * @return The converted OpenVR eye tracking data
 */
vr::VREyeTrackingData_t ToVREyeTrackingData(const EyeTrackingData& data);

/**
 * @brief Converts an OpenVR VREyeTrackingData_t to an EyeTrackingData
 * @param data The OpenVR eye tracking data to convert
 * @return The converted EyeTrackingData
 */
EyeTrackingData FromVREyeTrackingData(const vr::VREyeTrackingData_t& data);