#include "Audio.h"

#include <Windows.h>
#include <mmsystem.h>
#include <string>
#include <vector>
#include <unordered_set>

#pragma comment(lib, "Winmm.lib")

namespace {
	// 只在音频模块内部保存已打开的音效别名
	std::vector<std::wstring> soundAliases;
	std::unordered_set<std::wstring> playedThisFrame;
}

bool Audio::playMusic(const std::wstring& path) {
	// 先关闭上一次打开的音乐
	stopMusic();

	std::wstring command =
		L"open \"" + path + L"\" type mpegvideo alias pvz_music";

	if (mciSendStringW(command.c_str(), nullptr, 0, nullptr) != 0) {
		return false;
	}

	if (mciSendStringW(
		L"play pvz_music repeat", nullptr, 0, nullptr) != 0) {
		stopMusic();
		return false;
	}

	return true;
}

void Audio::stopMusic() {
	mciSendStringW(L"close pvz_music", nullptr, 0, nullptr);
}

void Audio::beginFrame() {
	playedThisFrame.clear();
}


bool Audio::loadSound(
	const std::wstring& path, const std::wstring& alias) {
	std::wstring command =
		L"open \"" + path + L"\" type waveaudio alias " + alias;

	if (mciSendStringW(command.c_str(), nullptr, 0, nullptr) != 0) {
		return false;
	}

	soundAliases.push_back(alias);
	return true;
}

bool Audio::playSound(const std::wstring& alias) {
	// 同一帧内，相同音效只播放一次。
	if (playedThisFrame.contains(alias)) return true;

	// 同一种音效再次触发时，从头重新播放
	std::wstring stopCommand = L"stop " + alias;
	mciSendStringW(stopCommand.c_str(), nullptr, 0, nullptr);

	std::wstring playCommand = L"play " + alias + L" from 0";
	bool success = mciSendStringW(
		playCommand.c_str(), nullptr, 0, nullptr) == 0;

	if (success) playedThisFrame.insert(alias);
	return success;
}

void Audio::stopSounds() {
	for (const std::wstring& alias : soundAliases) {
		std::wstring command = L"stop " + alias;
		mciSendStringW(command.c_str(), nullptr, 0, nullptr);
	}
}

void Audio::closeSounds() {
	// 关闭所有已成功打开的音效
	for (const std::wstring& alias : soundAliases) {
		std::wstring command = L"close " + alias;
		mciSendStringW(command.c_str(), nullptr, 0, nullptr);
	}
	soundAliases.clear();
	playedThisFrame.clear();
}
