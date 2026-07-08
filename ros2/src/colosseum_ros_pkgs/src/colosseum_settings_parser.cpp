#include "colosseum_settings_parser.h"

ColosseumSettingsParser::ColosseumSettingsParser(const std::string& host_ip)
    : host_ip_(host_ip)
{
    success_ = initializeSettings();
}

bool ColosseumSettingsParser::success()
{
    return success_;
}

bool ColosseumSettingsParser::getSettingsText(std::string& settings_text) const
{
    colosseum::RpcLibClientBase colosseum_client(host_ip_);
    colosseum_client.confirmConnection();

    settings_text = colosseum_client.getSettingsString();

    return !settings_text.empty();
}

std::string ColosseumSettingsParser::getSimMode()
{
    const auto& settings_json = colosseum::Settings::loadJSonString(settings_text_);
    return settings_json.getString("SimMode", "");
}

// mimics void ASimHUD::initializeSettings()
bool ColosseumSettingsParser::initializeSettings()
{
    if (getSettingsText(settings_text_)) {
        ColosseumSettings::initializeSettings(settings_text_);

        ColosseumSettings::singleton().load(std::bind(&ColosseumSettingsParser::getSimMode, this));
        std::cout << "SimMode: " << ColosseumSettings::singleton().simmode_name << std::endl;

        return true;
    }

    return false;
}
