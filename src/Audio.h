#pragma once

#include <string>

class Audio {
public:
	static bool playMusic(const std::wstring& path);
	static void stopMusic();

	// 加载音效，并为它指定一个播放别名
	static bool loadSound(const std::wstring& path, const std::wstring& alias);

	// 从头播放已经加载的音效
	static bool playSound(const std::wstring& alias);

	// 停止正在播放的事件音效，但保留资源供结算播放使用
	static void stopSounds();
	static void closeSounds();
};
