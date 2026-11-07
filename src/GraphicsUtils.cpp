#include "GraphicsUtils.h"

#include <Windows.h>
#include <easyx.h>

#pragma comment(lib, "Msimg32.lib")

void drawPng(int x, int y, int width, int height, IMAGE& image, unsigned char opacity) {
	BLENDFUNCTION blend = {};
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = opacity;
	blend.AlphaFormat = AC_SRC_ALPHA;

	AlphaBlend(
		GetImageHDC(), x, y, width, height,
		GetImageHDC(&image), 0, 0,
		image.getwidth(), image.getheight(),
		blend
	);
}

void drawPngWhite(int x, int y, int width, int height, IMAGE& image) {
	IMAGE whiteImage = image;
	DWORD* pixels = GetImageBuffer(&whiteImage);
	int count = whiteImage.getwidth() * whiteImage.getheight();

	for (int i = 0; i < count; ++i) {
		DWORD pixel = pixels[i];
		DWORD alpha = pixel >> 24;

		DWORD red = (pixel >> 16) & 0xFF;
		DWORD green = (pixel >> 8) & 0xFF;
		DWORD blue = pixel & 0xFF;

		// 叠加 50% 白色，保留原来的透明度和身体细节
		red = (red + alpha) / 2;
		green = (green + alpha) / 2;
		blue = (blue + alpha) / 2;

		pixels[i] = (alpha << 24) | (red << 16) | (green << 8) | blue;
	}

	drawPng(x, y, width, height, whiteImage);
}
