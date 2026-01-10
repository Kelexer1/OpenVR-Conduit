//#include "DevicePoseCommandSender.h"
//
//DevicePoseCommandSender::DevicePoseCommandSender(SharedDeviceMemoryIO* sharedMemory) :
//	sharedMemory(sharedMemory) {}
//
//void DevicePoseCommandSender::setPose(const uint32_t deviceIndex, DevicePose& pose) {
//	bool result;
//	if (sharedMemory && sharedMemory->isDeviceSlotActive(deviceIndex, result) && result) {
//		sharedMemory->writeOverridenPose(deviceIndex, pose);
//	}
//}
//
//void DevicePoseCommandSender::setPosition(const uint32_t deviceIndex, DeviceVector3& newPosition) {
//	bool result;
//	if (sharedMemory && sharedMemory->isDeviceSlotActive(deviceIndex, result) && result) {
//		DevicePose overridePose;
//
//		if (sharedMemory->readOverridenPose(deviceIndex, overridePose)) {
//			overridePose.vecPosition[0] = newPosition.v[0];
//			overridePose.vecPosition[1] = newPosition.v[1];
//			overridePose.vecPosition[2] = newPosition.v[2];
//
//			sharedMemory->writeOverridenPose(deviceIndex, overridePose);
//		}
//	}
//}
//
//void DevicePoseCommandSender::setRotation(const uint32_t deviceIndex, DeviceQuaternion& newRotation) {
//	bool result;
//	if (sharedMemory && sharedMemory->isDeviceSlotActive(deviceIndex, result) && result) {
//		DevicePose overridePose;
//
//		if (sharedMemory->readOverridenPose(deviceIndex, overridePose)) {
//			overridePose.qRotation.w = newRotation.w;
//			overridePose.qRotation.x = newRotation.x;
//			overridePose.qRotation.y = newRotation.y;
//			overridePose.qRotation.z = newRotation.z;
//
//			sharedMemory->writeOverridenPose(deviceIndex, overridePose);
//		}
//	}
//}
//
//void DevicePoseCommandSender::setVelocity(const uint32_t deviceIndex, DeviceVector3& newVelocity) {
//	bool result;
//	if (sharedMemory && sharedMemory->isDeviceSlotActive(deviceIndex, result) && result) {
//		DevicePose overridePose;
//
//		if (sharedMemory->readOverridenPose(deviceIndex, overridePose)) {
//			overridePose.vecVelocity[0] = newVelocity.v[0];
//			overridePose.vecVelocity[1] = newVelocity.v[1];
//			overridePose.vecVelocity[2] = newVelocity.v[2];
//
//			sharedMemory->writeOverridenPose(deviceIndex, overridePose);
//		}
//	}
//}
//
//void DevicePoseCommandSender::setAngularVelocity(const uint32_t deviceIndex, DeviceVector3& newAngularVelocity) {
//	bool result;
//	if (sharedMemory && sharedMemory->isDeviceSlotActive(deviceIndex, result) && result) {
//		DevicePose overridePose;
//
//		if (sharedMemory->readOverridenPose(deviceIndex, overridePose)) {
//			overridePose.vecAngularVelocity[0] = newAngularVelocity.v[0];
//			overridePose.vecAngularVelocity[1] = newAngularVelocity.v[1];
//			overridePose.vecAngularVelocity[2] = newAngularVelocity.v[2];
//
//			sharedMemory->writeOverridenPose(deviceIndex, overridePose);
//		}
//	}
//}
//
//void DevicePoseCommandSender::setAcceleration(const uint32_t deviceIndex, DeviceVector3& newAcceleration) {
//	bool result;
//	if (sharedMemory && sharedMemory->isDeviceSlotActive(deviceIndex, result) && result) {
//		DevicePose overridePose;
//
//		if (sharedMemory->readOverridenPose(deviceIndex, overridePose)) {
//			overridePose.vecAcceleration[0] = newAcceleration.v[0];
//			overridePose.vecAcceleration[1] = newAcceleration.v[1];
//			overridePose.vecAcceleration[2] = newAcceleration.v[2];
//
//			sharedMemory->writeOverridenPose(deviceIndex, overridePose);
//		}
//	}
//}
//
//void DevicePoseCommandSender::setAngularAcceleration(const uint32_t deviceIndex, DeviceVector3& newAngularAcceleration) {
//	bool result;
//	if (sharedMemory && sharedMemory->isDeviceSlotActive(deviceIndex, result) && result) {
//		DevicePose overridePose;
//
//		if (sharedMemory->readOverridenPose(deviceIndex, overridePose)) {
//			overridePose.vecAngularAcceleration[0] = newAngularAcceleration.v[0];
//			overridePose.vecAngularAcceleration[1] = newAngularAcceleration.v[1];
//			overridePose.vecAngularAcceleration[2] = newAngularAcceleration.v[2];
//
//			sharedMemory->writeOverridenPose(deviceIndex, overridePose);
//		}
//	}
//}
//
//DevicePose* DevicePoseCommandSender::getPose(const uint32_t deviceIndex) const {
//	bool result;
//	if (sharedMemory && sharedMemory->isDeviceSlotActive(deviceIndex, result) && result) {
//		DevicePose overridePose;
//
//		if (sharedMemory->readOverridenPose(deviceIndex, overridePose)) {
//			return &overridePose;
//		}
//	}
//	return nullptr;
//}
//
//DevicePose* DevicePoseCommandSender::getNaturalPose(const uint32_t deviceIndex) const {
//	bool result;
//	if (sharedMemory && sharedMemory->isDeviceSlotActive(deviceIndex, result) && result) {
//		DevicePose naturalPose;
//
//		if (sharedMemory->readNaturalPose(deviceIndex, naturalPose)) {
//			return &naturalPose;
//		}
//	}
//	return nullptr;
//}
//
//void DevicePoseCommandSender::setUseCustomPose(const uint32_t deviceIndex, const bool shouldOverridePose) {
//	bool result;
//	if (sharedMemory && sharedMemory->isDeviceSlotActive(deviceIndex, result) && result) {
//		sharedMemory->writeUseCustomPose(deviceIndex, shouldOverridePose);
//	}
//}
//
//bool* DevicePoseCommandSender::getUseCustomPose(const uint32_t deviceIndex) const {
//	bool result;
//	if (sharedMemory && sharedMemory->isDeviceSlotActive(deviceIndex, result) && result) {
//		bool shouldOverridePose;
//		if (sharedMemory->readUseCustomPose(deviceIndex, shouldOverridePose)) {
//			return &shouldOverridePose;
//		}
//	}
//	return nullptr;
//}
//
//void DevicePoseCommandSender::setUsePingPong(const uint32_t deviceIndex, const bool usePingPong) {
//	bool result;
//	if (sharedMemory && sharedMemory->isDeviceSlotActive(deviceIndex, result) && result) {
//		sharedMemory->writeUsePingPong(deviceIndex, usePingPong);
//	}
//}
//
//bool* DevicePoseCommandSender::getUsePingPong(const uint32_t deviceIndex) {
//	bool result;
//	if (sharedMemory && sharedMemory->isDeviceSlotActive(deviceIndex, result) && result) {
//		bool usePingPong;
//		if (sharedMemory->readUsePingPong(deviceIndex, usePingPong)) {
//			return &usePingPong;
//		}
//	}
//	return nullptr;
//}