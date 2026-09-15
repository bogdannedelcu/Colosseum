#include "SimHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/FileHelper.h"

#include "Vehicles/Multirotor/SimModeWorldMultiRotor.h"
#include "Vehicles/Car/SimModeCar.h"
#include "Vehicles/ComputerVision/SimModeComputerVision.h"

#include "common/ColosseumSettings.hpp"
#include <stdexcept>

ASimHUD::ASimHUD()
{
    static ConstructorHelpers::FClassFinder<UUserWidget> hud_widget_class(TEXT("WidgetBlueprint'/Colosseum/Blueprints/BP_SimHUDWidget'"));
    widget_class_ = hud_widget_class.Succeeded() ? hud_widget_class.Class : nullptr;
}

void ASimHUD::BeginPlay()
{
    Super::BeginPlay();

    try {
        UColosseumBlueprintLib::OnBeginPlay();
        initializeSettings();
        loadLevel();

        // Prevent a MavLink connection being established if changing levels
        if (map_changed_) return;

        setUnrealEngineSettings();
        createSimMode();
        createMainWidget();
        setupInputBindings();
        if (simmode_)
            simmode_->startApiServer();
    }
    catch (std::exception& ex) {
        UColosseumBlueprintLib::LogMessageString("Error at startup: ", ex.what(), LogDebugLevel::Failure);
        //FGenericPlatformMisc::PlatformInit();
        //FGenericPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Error at Startup"), ANSI_TO_TCHAR(ex.what()));
        UColosseumBlueprintLib::ShowMessage(EAppMsgType::Ok, std::string("Error at startup: ") + ex.what(), "Error");
    }
}

void ASimHUD::Tick(float DeltaSeconds)
{
    if (simmode_ && simmode_->EnableReport)
        widget_->updateDebugReport(simmode_->getDebugReport());
}

void ASimHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (simmode_)
        simmode_->stopApiServer();

    if (widget_) {
        widget_->Destruct();
        widget_ = nullptr;
    }
    if (simmode_) {
        simmode_->Destroy();
        simmode_ = nullptr;
    }

    UColosseumBlueprintLib::OnEndPlay();

    Super::EndPlay(EndPlayReason);
}

void ASimHUD::toggleRecordHandler()
{
    simmode_->toggleRecording();
}

void ASimHUD::inputEventToggleRecording()
{
    toggleRecordHandler();
}

void ASimHUD::inputEventToggleReport()
{
    simmode_->EnableReport = !simmode_->EnableReport;
    widget_->setReportVisible(simmode_->EnableReport);
}

void ASimHUD::inputEventToggleHelp()
{
    widget_->toggleHelpVisibility();
}

void ASimHUD::inputEventToggleTrace()
{
    simmode_->toggleTraceAll();
}

void ASimHUD::updateWidgetSubwindowVisibility()
{
    for (int window_index = 0; window_index < ColosseumSettings::kSubwindowCount; ++window_index) {
        APIPCamera* camera = subwindow_cameras_[window_index];
        ImageType camera_type = getSubWindowSettings().at(window_index).image_type;

        bool is_visible = getSubWindowSettings().at(window_index).visible && camera != nullptr;

        if (camera != nullptr) {
            camera->setCameraTypeEnabled(camera_type, is_visible);
            //sub-window captures don't count as a request, set bCaptureEveryFrame and bCaptureOnMovement to display so we can show correctly the subwindow
            camera->setCameraTypeUpdate(camera_type, false);
        }

        widget_->setSubwindowVisibility(window_index,
                                        is_visible,
                                        is_visible ? camera->getRenderTarget(camera_type, false) : nullptr);
    }
}

bool ASimHUD::isWidgetSubwindowVisible(int window_index)
{
    return widget_->getSubwindowVisibility(window_index) != 0;
}

void ASimHUD::toggleSubwindowVisibility(int window_index)
{
    getSubWindowSettings().at(window_index).visible = !getSubWindowSettings().at(window_index).visible;
    updateWidgetSubwindowVisibility();
}

void ASimHUD::inputEventToggleSubwindow0()
{
    toggleSubwindowVisibility(0);
}

void ASimHUD::inputEventToggleSubwindow1()
{
    toggleSubwindowVisibility(1);
}

void ASimHUD::inputEventToggleSubwindow2()
{
    toggleSubwindowVisibility(2);
}

void ASimHUD::inputEventToggleAll()
{
    getSubWindowSettings().at(0).visible = !getSubWindowSettings().at(0).visible;
    getSubWindowSettings().at(1).visible = getSubWindowSettings().at(2).visible = getSubWindowSettings().at(0).visible;
    updateWidgetSubwindowVisibility();
}

void ASimHUD::createMainWidget()
{
    //create main widget
    if (widget_class_ != nullptr) {
        APlayerController* player_controller = this->GetWorld()->GetFirstPlayerController();
        APawn* pawn = player_controller->GetPawn();
        if (pawn) {
            std::string pawn_name = std::string(TCHAR_TO_ANSI(*pawn->GetName()));
            Utils::log(pawn_name);
        }
        else {
            UColosseumBlueprintLib::ShowMessage(EAppMsgType::Ok, std::string("There were no compatible vehicles created for current SimMode! Check your settings.json."), "Error");
            UColosseumBlueprintLib::LogMessage(TEXT("There were no compatible vehicles created for current SimMode! Check your settings.json."), TEXT(""), LogDebugLevel::Failure);
        }

        widget_ = CreateWidget<USimHUDWidget>(player_controller, widget_class_);
    }
    else {
        widget_ = nullptr;
        UColosseumBlueprintLib::LogMessage(TEXT("Cannot instantiate BP_SimHUDWidget blueprint!"), TEXT(""), LogDebugLevel::Failure);
    }

    initializeSubWindows();

    widget_->AddToViewport();

    //synchronize PIP views
    widget_->initializeForPlay();
    if (simmode_)
        widget_->setReportVisible(simmode_->EnableReport);
    widget_->setOnToggleRecordingHandler(std::bind(&ASimHUD::toggleRecordHandler, this));
    widget_->setRecordButtonVisibility(ColosseumSettings::singleton().is_record_ui_visible);
    updateWidgetSubwindowVisibility();
}

void ASimHUD::setUnrealEngineSettings()
{
    //TODO: should we only do below on SceneCapture2D components and cameras?
    //avoid motion blur so capture images don't get
    GetWorld()->GetGameViewport()->GetEngineShowFlags()->SetMotionBlur(false);

    //use two different methods to set console var because sometime it doesn't seem to work
    static const auto custom_depth_var = IConsoleManager::Get().FindConsoleVariable(TEXT("r.CustomDepth"));
    custom_depth_var->Set(3);

    //Equivalent to enabling Custom Stencil in Project > Settings > Rendering > Postprocessing
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), FString("r.CustomDepth 3"));

    //during startup we init stencil IDs to random hash and it takes long time for large environments
    //we get error that GameThread has timed out after 30 sec waiting on render thread
    static const auto render_timeout_var = IConsoleManager::Get().FindConsoleVariable(TEXT("g.TimeoutForBlockOnRenderFence"));
    render_timeout_var->Set(300000);
}

void ASimHUD::setupInputBindings()
{
    UColosseumBlueprintLib::EnableInput(this);

    UColosseumBlueprintLib::BindActionToKey("inputEventToggleRecording", EKeys::R, this, &ASimHUD::inputEventToggleRecording);
    UColosseumBlueprintLib::BindActionToKey("InputEventToggleReport", EKeys::Semicolon, this, &ASimHUD::inputEventToggleReport);
    UColosseumBlueprintLib::BindActionToKey("InputEventToggleHelp", EKeys::F1, this, &ASimHUD::inputEventToggleHelp);
    UColosseumBlueprintLib::BindActionToKey("InputEventToggleTrace", EKeys::T, this, &ASimHUD::inputEventToggleTrace);

    UColosseumBlueprintLib::BindActionToKey("InputEventToggleSubwindow0", EKeys::One, this, &ASimHUD::inputEventToggleSubwindow0);
    UColosseumBlueprintLib::BindActionToKey("InputEventToggleSubwindow1", EKeys::Two, this, &ASimHUD::inputEventToggleSubwindow1);
    UColosseumBlueprintLib::BindActionToKey("InputEventToggleSubwindow2", EKeys::Three, this, &ASimHUD::inputEventToggleSubwindow2);
    UColosseumBlueprintLib::BindActionToKey("InputEventToggleAll", EKeys::Zero, this, &ASimHUD::inputEventToggleAll);
}

void ASimHUD::initializeSettings()
{
    std::string settingsText;
    if (getSettingsText(settingsText))
        ColosseumSettings::initializeSettings(settingsText);
    else
        ColosseumSettings::createDefaultSettingsFile();

    ColosseumSettings::singleton().load(std::bind(&ASimHUD::getSimModeFromUser, this));
    for (const auto& warning : ColosseumSettings::singleton().warning_messages) {
        UColosseumBlueprintLib::LogMessageString(warning, "", LogDebugLevel::Failure);
    }
    for (const auto& error : ColosseumSettings::singleton().error_messages) {
        UColosseumBlueprintLib::ShowMessage(EAppMsgType::Ok, error, "settings.json");
    }
}

const std::vector<ASimHUD::ColosseumSettings::SubwindowSetting>& ASimHUD::getSubWindowSettings() const
{
    return ColosseumSettings::singleton().subwindow_settings;
}

std::vector<ASimHUD::ColosseumSettings::SubwindowSetting>& ASimHUD::getSubWindowSettings()
{
    return ColosseumSettings::singleton().subwindow_settings;
}

std::string ASimHUD::getSimModeFromUser()
{
    if (EAppReturnType::No == UColosseumBlueprintLib::ShowMessage(EAppMsgType::YesNo,
                                                            "Would you like to use car simulation? Choose no to use quadrotor simulation.",
                                                            "Choose Vehicle")) {
        return ColosseumSettings::kSimModeTypeMultirotor;
    }
    else
        return ColosseumSettings::kSimModeTypeCar;
}

void ASimHUD::loadLevel()
{
    UColosseumBlueprintLib::RunCommandOnGameThread([&]() { this->map_changed_ = UColosseumBlueprintLib::loadLevel(this->GetWorld(), FString(ColosseumSettings::singleton().level_name.c_str())); }, true);
}

void ASimHUD::createSimMode()
{
    std::string simmode_name = ColosseumSettings::singleton().simmode_name;

    FActorSpawnParameters simmode_spawn_params;
    simmode_spawn_params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    //spawn at origin. We will use this to do global NED transforms, for ex, non-vehicle objects in environment
    if (simmode_name == ColosseumSettings::kSimModeTypeMultirotor)
        simmode_ = this->GetWorld()->SpawnActor<ASimModeWorldMultiRotor>(FVector::ZeroVector,
                                                                         FRotator::ZeroRotator,
                                                                         simmode_spawn_params);
    else if (simmode_name == ColosseumSettings::kSimModeTypeCar)
        simmode_ = this->GetWorld()->SpawnActor<ASimModeCar>(FVector::ZeroVector,
                                                             FRotator::ZeroRotator,
                                                             simmode_spawn_params);
    else if (simmode_name == ColosseumSettings::kSimModeTypeComputerVision)
        simmode_ = this->GetWorld()->SpawnActor<ASimModeComputerVision>(FVector::ZeroVector,
                                                                        FRotator::ZeroRotator,
                                                                        simmode_spawn_params);
    else {
        UColosseumBlueprintLib::ShowMessage(EAppMsgType::Ok, std::string("SimMode is not valid: ") + simmode_name, "Error");
        UColosseumBlueprintLib::LogMessageString("SimMode is not valid: ", simmode_name, LogDebugLevel::Failure);
    }
}

void ASimHUD::initializeSubWindows()
{
    if (!simmode_)
        return;

    auto default_vehicle_sim_api = simmode_->getVehicleSimApi();

    if (default_vehicle_sim_api) {
        auto camera_count = default_vehicle_sim_api->getCameraCount();

        //setup defaults
        if (camera_count > 0) {
            subwindow_cameras_[0] = default_vehicle_sim_api->getCamera("");
            subwindow_cameras_[1] = default_vehicle_sim_api->getCamera(""); //camera_count > 3 ? 3 : 0
            subwindow_cameras_[2] = default_vehicle_sim_api->getCamera(""); //camera_count > 4 ? 4 : 0
        }
        else
            subwindow_cameras_[0] = subwindow_cameras_[1] = subwindow_cameras_[2] = nullptr;
    }

    for (const auto& setting : getSubWindowSettings()) {
        APIPCamera* camera = simmode_->getCamera(colosseum::CameraDetails(setting.camera_name, setting.vehicle_name, setting.external));
        if (camera)
            subwindow_cameras_[setting.window_index] = camera;
        else
            UColosseumBlueprintLib::LogMessageString("Invalid Camera settings in <SubWindows> element",
                                               std::to_string(setting.window_index),
                                               LogDebugLevel::Failure);
    }
}

FString ASimHUD::getLaunchPath(const std::string& filename)
{
    FString launch_rel_path = FPaths::LaunchDir();
    FString abs_path = FPaths::ConvertRelativePathToFull(launch_rel_path);
    return FPaths::Combine(abs_path, FString(filename.c_str()));
}

// Attempts to parse the settings text from one of multiple locations.
// First, check the command line for settings provided via "-s" or "--settings" arguments
// Next, check the executable's working directory for the settings file.
// Finally, check the user's documents folder.
// If the settings file cannot be read, throw an exception

bool ASimHUD::getSettingsText(std::string& settingsText)
{
    return (getSettingsTextFromCommandLine(settingsText) ||
            readSettingsTextFromFile(FString(colosseum::Settings::getExecutableFullPath("settings.json").c_str()), settingsText) ||
            readSettingsTextFromFile(getLaunchPath("settings.json"), settingsText) ||
            readSettingsTextFromFile(FString(colosseum::Settings::Settings::getUserDirectoryFullPath("settings.json").c_str()), settingsText));
}

// Attempts to parse the settings file path or the settings text from the command line
// Looks for the flag "-settings=". If it exists, settingsText will be set to the value.
// Example (Path): Colosseum.exe -settings="C:\path\to\settings.json"
// Example (Text): Colosseum.exe -settings={"foo":"bar"} -> settingsText will be set to {"foo":"bar"}
// Returns true if the argument is present, false otherwise.
bool ASimHUD::getSettingsTextFromCommandLine(std::string& settingsText)
{
    const TCHAR* commandLineArgs = FCommandLine::Get();
    FString settingsJsonFString;

    if (FParse::Value(commandLineArgs, TEXT("-settings="), settingsJsonFString, false)) {
        if (readSettingsTextFromFile(settingsJsonFString, settingsText)) {
            return true;
        }
        else {
            UColosseumBlueprintLib::LogMessageString("Loaded settings from commandline: ", TCHAR_TO_UTF8(*settingsJsonFString), LogDebugLevel::Informational);
            settingsText = TCHAR_TO_UTF8(*settingsJsonFString);
            return true;
        }
    }

    return false;
}

bool ASimHUD::readSettingsTextFromFile(const FString& settingsFilepath, std::string& settingsText)
{
    bool found = FPaths::FileExists(settingsFilepath);
    if (found) {
        FString settingsTextFStr;
        bool readSuccessful = FFileHelper::LoadFileToString(settingsTextFStr, *settingsFilepath);
        if (readSuccessful) {
            UColosseumBlueprintLib::LogMessageString("Loaded settings from ", TCHAR_TO_UTF8(*settingsFilepath), LogDebugLevel::Informational);
            settingsText = TCHAR_TO_UTF8(*settingsTextFStr);
        }
        else {
            UColosseumBlueprintLib::LogMessageString("Cannot read file ", TCHAR_TO_UTF8(*settingsFilepath), LogDebugLevel::Failure);
            throw std::runtime_error("Cannot read settings file.");
        }
    }

    return found;
}
