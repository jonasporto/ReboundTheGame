#pragma once

#include <string>
#include <SFML/System.hpp>
#include <OutputStream.hpp>
#include "InputSettings.hpp"
#include "VideoSettings.hpp"
#include "AudioSettings.hpp"

constexpr size_t SettingsVersion = 1;

struct Settings
{
    InputSettings inputSettings;
    VideoSettings videoSettings;
    AudioSettings audioSettings;
    std::string languageFile;
};

bool readFromStream(sf::InputStream &stream, Settings& settings);
bool writeToStream(OutputStream& stream, const Settings& settings);

Settings loadSettingsFile(bool *success = nullptr);
bool storeSettingsFile(const Settings& settings);
