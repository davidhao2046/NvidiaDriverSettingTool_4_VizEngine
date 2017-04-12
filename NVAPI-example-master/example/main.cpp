
#include <nvapi.h>
#include <NvApiDriverSettings.h>
#include <string>
#include <vector>
#include <iostream>

#pragma comment(lib, "nvapi.lib" )

namespace nv{ namespace info {

void print_error(NvAPI_Status status)
{
  NvAPI_ShortString error;
  NvAPI_GetErrorMessage(status, error);
  std::cout << error << "\n";
}

void print_setting_info(NVDRS_SETTING & setting)
{
  ::NvAPI_DRS_GetSettingNameFromId(setting.settingId, &setting.settingName);
  wprintf(L"Setting Name: %s\n", setting.settingName);
  printf("Setting ID: %X\n", setting.settingId);
  printf("Predefined? : %d\n", setting.isCurrentPredefined);
  switch (setting.settingType) {
  case NVDRS_DWORD_TYPE:
    printf("Setting Value: %X\n", setting.u32CurrentValue);
    break;
  case NVDRS_BINARY_TYPE: {
    unsigned int len;
    printf("Setting Binary (length=%d) :", setting.binaryCurrentValue.valueLength);
    for (len = 0; len < setting.binaryCurrentValue.valueLength; len++) {
      printf(" %02x", setting.binaryCurrentValue.valueData[len]);
    }
    printf("\n");
  } break;
  case NVDRS_WSTRING_TYPE:
    wprintf(L"Setting Value: %s\n", setting.wszCurrentValue);
    break;
  }
}

bool display_profile_contents( NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile ,int index)
{
  NvAPI_Status status;

  NVDRS_PROFILE profileInformation = { NVDRS_PROFILE_VER };

  status = ::NvAPI_DRS_GetProfileInfo(hSession, hProfile, &profileInformation);
  if (status != NVAPI_OK) {
    return false;
  }

  std::wstring str((wchar_t*)profileInformation.profileName);
  
  //printf("%s\r\n", (char*)(pdata));  //test by david
  std::string::size_type found = str.find(L"Editing");
  if (found != std::string::npos)
  { 
	  std::cout << "first 'Editing' found at: " << found << '\n';
	  std::wcout << str << std::endl;
	  wprintf(L"David \r\n");
	  ::NvAPI_DRS_SetCurrentGlobalProfile(hSession, profileInformation.profileName);
  }
  /*if (str.find("Editing"))
  {
	  ::NvAPI_DRS_SetCurrentGlobalProfile(hSession, profileInformation.profileName);
  }*/
  
  //test by david
  /*  wprintf(L"Profile Name: %s\n", profileInformation.profileName);
printf("Number of Applications associated with the Profile: %d\n", profileInformation.numOfApps);
  printf("Number of Settings associated with the Profile: %d\n", profileInformation.numOfSettings);*/
  //printf("Is Predefined: %d\n", profileInformation.isPredefined);

  if (profileInformation.numOfApps > 0) {
    std::vector<NVDRS_APPLICATION> appArray(profileInformation.numOfApps);

    NvU32 numAppsRead = profileInformation.numOfApps;
    NvU32 i;

    appArray[0] = { NVDRS_APPLICATION_VER };

    status = ::NvAPI_DRS_EnumApplications(hSession, hProfile, 0, &numAppsRead, appArray.data());
    if (status != NVAPI_OK) {
      print_error(status);
      return false;
    }
    for (i = 0; i < numAppsRead; i++) {
    /*  wprintf(L"Executable: %s\n", appArray[i].appName);
      wprintf(L"User Friendly Name: %s\n", appArray[i].userFriendlyName);
      printf("Is Predefined: %d\n", appArray[i].isPredefined);*/
    }
  }
  if (profileInformation.numOfSettings > 0) {
    std::vector<NVDRS_SETTING> setArray(profileInformation.numOfSettings);
    NvU32          numSetRead = profileInformation.numOfSettings, i;
    setArray[0] = { NVDRS_SETTING_VER };

    status = ::NvAPI_DRS_EnumSettings(hSession, hProfile, 0, &numSetRead, setArray.data());
    if (status != NVAPI_OK) {
      print_error(status);
      return false;
    }
    for (i = 0; i < numSetRead; i++) {
      NVDRS_SETTING & setting = setArray[i];

      if (setting.settingLocation != NVDRS_CURRENT_PROFILE_LOCATION) {
        continue;
      }
     // print_setting_info(setting);
    }
  }
  printf("\n");
  return true;
}

void enumerate_profiles_on_system()
{
  NvAPI_Status status;
  status = NvAPI_Initialize();
  if (status != NVAPI_OK)
    print_error(status);

  NvDRSSessionHandle hSession = 0;

  status = ::NvAPI_DRS_CreateSession(&hSession);
  if (status != NVAPI_OK)
    print_error(status);

  status = ::NvAPI_DRS_LoadSettings(hSession);
  if (status != NVAPI_OK)
    print_error(status);

  NvDRSProfileHandle hProfile = 0;
  unsigned int       index    = 0;
  while ((status = ::NvAPI_DRS_EnumProfiles(hSession, index, &hProfile)) == NVAPI_OK&&index<=1300) {
    //printf("Profile in position %d:\n", index);
    display_profile_contents(hSession, hProfile,index);
    index++;
  }
  if (status == NVAPI_END_ENUMERATION) {
    // this is expected at the end of the enumeration
  } else if (status != NVAPI_OK)
    print_error(status);

  ::NvAPI_DRS_DestroySession(hSession);
  hSession = 0;
}

}} //namespace nv::info

int main()
{
	std::cout << "*************************************" << std::endl;
	std::cout << "* Viz Engine，Viz Artist显卡设置工具*" << std::endl;
	std::cout << "*************************************" << std::endl;
	std::cout << "***    Made by David，CCS乐田.    ***" << std::endl;
	std::cout << "\n";
	NvAPI_ShortString str1{};
	NvU32 dvers;
	NvAPI_SYS_GetDriverAndBranchVersion(&dvers, str1);
	std::string temp = std::to_string(dvers);
	temp.insert(3, ".");
	std::cout << "当前显卡驱动版本：" << temp << std::endl;
	std::cout << std::endl;
	int va;
	std::cout << "请直接输入数字1或者2来设置不同的显卡驱动:" << std::endl;
	std::cout << " 1.VGA版本，设计工作站。" << std::endl;
	std::cout << " 2.带视频板卡输出引擎。" << std::endl;
	std::cin >> va;
	while (std::cin.fail() || va>2 || va<0) {
		std::cout << "--------------------------" << std::endl;
		std::cout << "Hey,请输入数字1或者2，OK !" << std::endl;
		std::cout << "--------------------------" << std::endl;
		std::cout << "" << std::endl;
		std::cin.clear();
		std::cin.ignore(256, '\n');
		std::cout << "请直接输入数字1或者2来设置不同的显卡驱动:" << std::endl;
		std::cout << " 1.VGA版本，设计工作站。" << std::endl;
		std::cout << " 2.带视频板卡输出引擎。" << std::endl;
		std::cin >> va;
	}
	if (va == 1)
	{
		std::cout << "你选择的是 1.VGA版本，设计工作站。\n";
	}
	else if (va == 2)
	{
		std::cout << "你选择的是 2. 带视频板卡输出引擎。\n";	
	}
	else if (va == 0)
	{
		std::cout << "被你发现了，这个功能还没加进去！\n";
	}
  NvDRSSessionHandle _session{};
  NvDRSProfileHandle _profile{};
  NVDRS_PROFILE      _profInfo{};

  if (NvAPI_Initialize() != NVAPI_OK)
    throw std::runtime_error("NVIDIA Api not initialized");

  if (::NvAPI_DRS_CreateSession(&_session) != NVAPI_OK)
    throw std::runtime_error("can't create session");

  if (::NvAPI_DRS_LoadSettings(_session) != NVAPI_OK)
    throw std::runtime_error("can't load system settings");

  if (::NvAPI_DRS_GetCurrentGlobalProfile(_session, &_profile) != NVAPI_OK)
    throw std::runtime_error("can't get current global profile");
  if (::NvAPI_DRS_GetBaseProfile(_session, &_profile) != NVAPI_OK)
	  throw std::runtime_error("can't get base profile");

  NvPhysicalGpuHandle nvGPUHandle[NVAPI_MAX_PHYSICAL_GPUS];
  NvAPI_Status        status;
  NvU32               GpuCount;
  NvU32               DeviceId;
  NvU32               SubSystemId;
  NvU32               RevisionId;
  NvU32               ExtDeviceId;
  NvU32               BusId;
  NvU32               BiosRevision;
  NvU32               BiosRevisionOEM;
  NV_BOARD_INFO       BoardInfo{ NV_BOARD_INFO_VER };
  NvU32               ConfiguredFeatureMask;
  NvU32               ConsistentFeatureMask;
  NvU32               CoreCount;

  NvAPI_EnumPhysicalGPUs(nvGPUHandle, &GpuCount);

  NvAPI_ShortString str{};
  NvAPI_GPU_GetFullName          (nvGPUHandle[0], str);
  NvAPI_GPU_GetPCIIdentifiers    (nvGPUHandle[0], &DeviceId, &SubSystemId, &RevisionId, &ExtDeviceId);
  NvAPI_GPU_GetBusId             (nvGPUHandle[0], &BusId);
  NvAPI_GPU_GetVbiosRevision     (nvGPUHandle[0], &BiosRevision);
  NvAPI_GPU_GetVbiosOEMRevision  (nvGPUHandle[0], &BiosRevisionOEM);
  NvAPI_GPU_GetVbiosVersionString(nvGPUHandle[0], str);

  status = NvAPI_GPU_GetBoardInfo            (nvGPUHandle[0], &BoardInfo);
  status = NvAPI_GPU_WorkstationFeatureQuery (nvGPUHandle[0], &ConfiguredFeatureMask, &ConsistentFeatureMask);
  status = NvAPI_GPU_GetGpuCoreCount         (nvGPUHandle[0], &CoreCount);

  NV_CHIPSET_INFO info{ NV_CHIPSET_INFO_VER_4 };

  status = NvAPI_SYS_GetChipSetInfo(&info);
  NvAPI_GetInterfaceVersionString(str);
 
 // nv::info::enumerate_profiles_on_system();
  //nv::info::display_profile_contents(_session, _profile);

 /* Ambient Occlusion : Off */
  NVDRS_SETTING drsSetting = { 0 };
  drsSetting.version = NVDRS_SETTING_VER;
  drsSetting.settingId = AO_MODE_ID;
  drsSetting.settingType = NVDRS_DWORD_TYPE;
  drsSetting.u32CurrentValue = AO_MODE_OFF;


  status = NvAPI_DRS_SetSetting(_session, _profile, &drsSetting);
  if (status != NVAPI_OK)
	  throw std::runtime_error("failed to set:  Ambient Occlusion : Off");

  //Anisotropic filtering : Application controlled
  drsSetting = { 0 };
  drsSetting.version = NVDRS_SETTING_VER;
  drsSetting.settingId = ANISO_MODE_SELECTOR_ID;
  drsSetting.settingType = NVDRS_DWORD_TYPE;
  drsSetting.u32CurrentValue = ANISO_MODE_SELECTOR_APP;

  status = NvAPI_DRS_SetSetting(_session, _profile, &drsSetting);
  if (status != NVAPI_OK)
	  throw std::runtime_error("failed to set:  Anisotropic filtering : Application controlled");


  //Antialiasing - Gamma correction : Off
  drsSetting = { 0 };
  drsSetting.version = NVDRS_SETTING_VER;
  drsSetting.settingId = AA_MODE_GAMMACORRECTION_ID;
  drsSetting.settingType = NVDRS_DWORD_TYPE;
  drsSetting.u32CurrentValue = AA_MODE_GAMMACORRECTION_OFF;

  status = NvAPI_DRS_SetSetting(_session, _profile, &drsSetting);
  if (status != NVAPI_OK)
	  throw std::runtime_error("failed to set:  Antialiasing - Gamma correction : Off");

  //Antialiasing - Mode : Override any application settings
 
  drsSetting = { 0 };
  drsSetting.version = NVDRS_SETTING_VER;
  drsSetting.settingId = AA_MODE_SELECTOR_ID;
  drsSetting.settingType = NVDRS_DWORD_TYPE;
  drsSetting.u32CurrentValue = AA_MODE_SELECTOR_OVERRIDE;

  status = NvAPI_DRS_SetSetting(_session, _profile, &drsSetting);
  if (status != NVAPI_OK)
	  throw std::runtime_error("failed to set:   Antialiasing - Mode : Override any application settings");

  //Antialiasing - FXAA : Off
  drsSetting = { 0 };
  drsSetting.version = NVDRS_SETTING_VER;
  drsSetting.settingId = FXAA_ENABLE_ID;
  drsSetting.settingType = NVDRS_DWORD_TYPE;
  drsSetting.u32CurrentValue = FXAA_ENABLE_OFF;

  status = NvAPI_DRS_SetSetting(_session, _profile, &drsSetting);
  if (status != NVAPI_OK)
	  throw std::runtime_error("failed to set:    Antialiasing - FXAA : Off");

  // Antialiasing - Setting : 4x(4xMS)
  drsSetting = { 0 };
  drsSetting.version = NVDRS_SETTING_VER;
  drsSetting.settingId = AA_MODE_METHOD_ID;
  drsSetting.settingType = NVDRS_DWORD_TYPE;
  drsSetting.u32CurrentValue =AA_MODE_METHOD_MULTISAMPLE_4X;

  status = NvAPI_DRS_SetSetting(_session, _profile, &drsSetting);
  if (status != NVAPI_OK)
	  throw std::runtime_error("failed to set:      Antialiasing - Setting : 4x(4xMS)");

  //Antialiasing - Transparency : Off
  drsSetting = { 0 };
  drsSetting.version = NVDRS_SETTING_VER;
  drsSetting.settingId = AA_MODE_REPLAY_ID;
  drsSetting.settingType = NVDRS_DWORD_TYPE;
  drsSetting.u32CurrentValue = AA_MODE_REPLAY_MODE_OFF;

  status = NvAPI_DRS_SetSetting(_session, _profile, &drsSetting);
  if (status != NVAPI_OK)
	  throw std::runtime_error("failed to set:    Antialiasing - Transparency : Off ");

  //Power management mode : Prefer maximum performance
  drsSetting = { 0 };
  drsSetting.version = NVDRS_SETTING_VER;
  drsSetting.settingId = PREFERRED_PSTATE_ID;
  drsSetting.settingType = NVDRS_DWORD_TYPE;
  drsSetting.u32CurrentValue = PREFERRED_PSTATE_PREFER_MAX;

  status = NvAPI_DRS_SetSetting(_session, _profile, &drsSetting);
  if (status != NVAPI_OK)
	  throw std::runtime_error("failed to set:    Power management mode : Prefer maximum performance");

  

  //vertical sync
  drsSetting = { 0 };
  drsSetting.version = NVDRS_SETTING_VER;
  drsSetting.settingId = VSYNCMODE_ID;
  drsSetting.settingType = NVDRS_DWORD_TYPE; 
  if (va == 1)
  {
	  drsSetting.u32CurrentValue = VSYNCMODE_FORCEON;
  }
  else if(va == 2)
  {
	  drsSetting.u32CurrentValue = VSYNCMODE_FORCEOFF;
  }

  status = NvAPI_DRS_SetSetting(_session, _profile, &drsSetting);
  if (status != NVAPI_OK)
	  throw std::runtime_error("failed to set:    vertical sync");


  //save all the setting above.
  status = NvAPI_DRS_SaveSettings(_session);
  if (status != NVAPI_OK)
  {
	  throw std::runtime_error("Something wrong happened!");
  } 
  else
  {
	  std::cout << "\n";
	  std::cout << "Job done! \n";
	  std::cout << "\n";
  }


  //::NvAPI_DRS_GetProfileInfo(_session,_profile,)


  //::NvAPI_DRS_FindProfileByName(_session,,)

  /*NvAPI_UnicodeString profileName= NvAPI_DRS_FindProfileByName;
  ::NvAPI_DRS_SetCurrentGlobalProfile(_session, profileName);
  NvAPI_DRS_SetCurrentGlobalProfile(NvDRSSessionHandle hSession, NvAPI_UnicodeString wszGlobalProfileName);*/

  ::NvAPI_DRS_DestroySession(_session);


  /*
  NvDRSSessionHandle hSession = 0;

  status = ::NvAPI_DRS_CreateSession(&hSession);
  if (status != NVAPI_OK)
	  printf("Create error");

  status = ::NvAPI_DRS_LoadSettings(hSession);
  if (status != NVAPI_OK)
	  printf("loading error");

  NvDRSProfileHandle hProfile = 0;
  unsigned int       index = 0;
  while ((status = ::NvAPI_DRS_EnumProfiles(hSession, index, &hProfile)) == NVAPI_OK)
  {
	  NVDRS_PROFILE profileInformation = { NVDRS_PROFILE_VER };

	  status = ::NvAPI_DRS_GetProfileInfo(hSession, hProfile, &profileInformation);
	  if (status != NVAPI_OK) {
		  return false;
	  }

	  std::wstring str((wchar_t*)profileInformation.profileName);

	  std::string::size_type found = str.find(L"Editing");
	  if (found != std::string::npos)
	  {
		  std::cout << "first 'Editing' found at: " << found << '\n';
		  std::wcout << str << std::endl;
		  wprintf(L"David \r\n");
		  ::NvAPI_DRS_SetCurrentGlobalProfile(hSession, profileInformation.profileName);
	  }
	  index++;
  }
  if (status == NVAPI_END_ENUMERATION) {
	  // this is expected at the end of the enumeration
  }
  else if (status != NVAPI_OK)
	  printf("wrong");

  ::NvAPI_DRS_DestroySession(hSession);

  */


  system("pause");

  return 0;
}