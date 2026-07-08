#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"

#include "ColosseumBlueprintLib.h"
#include "api/VehicleSimApiBase.hpp"
#include "Recording/RecordingFile.h"
#include "physics/Kinematics.hpp"
#include <memory>
#include "common/ClockFactory.hpp"
#include "common/ColosseumSettings.hpp"
#include "common/WorkerThread.hpp"

class FRecordingThread : public FRunnable
{
public:
    typedef colosseum::ColosseumSettings::RecordingSetting RecordingSetting;
    typedef colosseum::VehicleSimApiBase VehicleSimApiBase;
    typedef colosseum::ImageCaptureBase ImageCaptureBase;

public:
    FRecordingThread();
    virtual ~FRecordingThread();

    static void init();
    static void startRecording(const RecordingSetting& settings,
                               const common_utils::UniqueValueMap<std::string, VehicleSimApiBase*>& vehicle_sim_apis);
    static void stopRecording();
    static void killRecording();
    static bool isRecording();

protected:
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;
    virtual void Exit() override;

private:
    FThreadSafeCounter stop_task_counter_;

    static std::unique_ptr<FRecordingThread> running_instance_;
    static std::unique_ptr<FRecordingThread> finishing_instance_;
    static colosseum::WorkerThreadSignal finishing_signal_;
    static bool first_;

    static std::unique_ptr<FRecordingThread> instance_;

    std::unique_ptr<FRunnableThread> thread_;

    RecordingSetting settings_;
    std::unique_ptr<RecordingFile> recording_file_;
    common_utils::UniqueValueMap<std::string, VehicleSimApiBase*> vehicle_sim_apis_;
    std::unordered_map<std::string, const ImageCaptureBase*> image_captures_;
    std::unordered_map<std::string, colosseum::Pose> last_poses_;

    colosseum::TTimePoint last_screenshot_on_;

    bool is_ready_;
};