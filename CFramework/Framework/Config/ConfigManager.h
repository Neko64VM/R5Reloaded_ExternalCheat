#pragma once
#include <string>
#include <thread>
#include <vector>
#include <guiddef.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// スレッドセーフを全く考慮していない.
// 実用上問題が出てことが無いのでセーフということにしておく.
class ConfigManager
{
private:
    std::string m_szConfigPath;
    std::string m_szDefaultConfName{ "default.json" };

    json GetDefaultConfig();
    std::string ConvertWideToMultiByte(const std::wstring& target_str);
    std::string GetAppDataPath(const GUID& id); // GUID: https://learn.microsoft.com/ja-jp/windows/win32/shell/knownfolderid

public:
    // AppData\Local 内に作るフォルダの名前
    bool InitConfigSystem(const std::string& configDirName);
    bool InitConfigSystem(const std::string& baseDirName, const std::string& configDirName);

    // Create & Delete
    bool CreateConfig(const std::string& configName);
    void DeleteConfig(const std::string& configName);

    // ConfigDir内の設定ファイル一覧を取得
    std::vector<std::string> GetConfigList();

    // 本体
    void LoadSetting(const std::string& configName);
    void SaveSetting(const std::string& configName);

    // Worker
	void LoadWorker(const std::string& configName) {
		std::jthread([&]() { LoadSetting(configName); }).detach();
	}
	void SaveWorker(const std::string& configName) {
        std::jthread([&]() { SaveSetting(configName); }).detach();
	}
};

extern ConfigManager config;