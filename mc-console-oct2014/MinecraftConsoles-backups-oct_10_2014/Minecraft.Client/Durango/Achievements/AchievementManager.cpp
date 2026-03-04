#include "stdafx.h"
#include "AchievementManager.h"

using namespace Platform;
using namespace Windows::Data;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Xbox::System;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Details;
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::Achievements;


AchievementManager *AchievementManager::m_instance = new AchievementManager(); //Singleton instance of the Achievement Manager