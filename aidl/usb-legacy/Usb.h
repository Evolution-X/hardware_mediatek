/*
 * Copyright (C) 2021 The Android Open Source Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <android-base/file.h>
#include <aidl/android/hardware/usb/BnUsb.h>
#include <aidl/android/hardware/usb/BnUsbCallback.h>
#include <utils/Log.h>

#define UEVENT_MSG_LEN     2048
#define UEVENT_MAX_EVENTS  64
// The type-c stack waits for 4.5 - 5.5 secs before declaring a port non-pd.
// The -partner directory would not be created until this is done.
// Having a margin of ~3 secs for the directory and other related bookeeping
// structures created and uvent fired.
#define PORT_TYPE_TIMEOUT 8

namespace aidl {
namespace android {
namespace hardware {
namespace usb {

using ::aidl::android::hardware::usb::IUsbCallback;
using ::aidl::android::hardware::usb::PortRole;
using ::android::base::ReadFileToString;
using ::android::base::WriteStringToFile;
using ::android::sp;
using ::ndk::ScopedAStatus;
using ::std::shared_ptr;
using ::std::string;

#define PULLUP_PATH "/config/usb_gadget/g1/UDC"

struct Usb : public BnUsb {
    Usb();

    ScopedAStatus enableContaminantPresenceDetection(const std::string& in_portName,
            bool in_enable, int64_t in_transactionId) override;
    ScopedAStatus queryPortStatus(int64_t in_transactionId) override;
    ScopedAStatus setCallback(const shared_ptr<IUsbCallback>& in_callback) override;
    ScopedAStatus switchRole(const string& in_portName, const PortRole& in_role,
            int64_t in_transactionId) override;
    ScopedAStatus enableUsbData(const string& in_portName, bool in_enable,
            int64_t in_transactionId) override;
    ScopedAStatus enableUsbDataWhileDocked(const string& in_portName,
            int64_t in_transactionId) override;
    ScopedAStatus limitPowerTransfer(const std::string& in_portName, bool in_limit,
            int64_t in_transactionId)override;
    ScopedAStatus resetUsbPort(const std::string& in_portName,
            int64_t in_transactionId)override;

    shared_ptr<IUsbCallback> mCallback;
    // Protects mCallback variable
    pthread_mutex_t mLock;
    // Protects roleSwitch operation
    pthread_mutex_t mRoleSwitchLock;
    // Threads waiting for the partner to come back wait here
    pthread_cond_t mPartnerCV;
    // lock protecting mPartnerCV
    pthread_mutex_t mPartnerLock;
    // Variable to signal partner coming back online after type switch
    bool mPartnerUp;
    // Usb Data status
    bool mUsbDataEnabled;
  private:
    pthread_t mPoll;
};

} // namespace usb
} // namespace hardware
} // namespace android
} // aidl
