#include "Game.h"

#include <Windows.h>
#include <memory>

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
	auto game = std::make_unique<Game>();
	game->run();
	return 0;
}
