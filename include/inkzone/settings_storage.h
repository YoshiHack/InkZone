#pragma once

#include "inkzone/settings.h"

namespace inkzone {

bool saveSettings(const Settings& settings);
bool loadSettings(Settings& settings);
void clearSavedSettings();

}  // namespace inkzone