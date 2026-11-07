#include "GraphicsUtils.h"

#include <Windows.h>
#include <easyx.h>
#include <unordered_map>

#pragma comment(lib, "Msimg32.lib")

namespace {
	std::unordered_map<IMAGE*, IMAGE> whiteImages;
}

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

void prepareWhiteImage(IMAGE& image) {
	// 每张原图只生成一次闪白缓存
	auto [it, inserted] = whiteImages.try_emplace(&image, image);
	if (!inserted) return;

	IMAGE& whiteImage = it->second;
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

		pixels[i] = (alpha << 24) | (red << 16) |
			(green << 8) | blue;
	}
}

void drawPngWhite(int x, int y, int width, int height, IMAGE& image) {
	prepareWhiteImage(image);
	drawPng(x, y, width, height, whiteImages.at(&image));
}

void resizePng(IMAGE& image, int width, int height) {
	int sourceWidth = image.getwidth();
	int sourceHeight = image.getheight();
	if (width <= 0 || height <= 0 ||
		sourceWidth <= 0 || sourceHeight <= 0) return;
	if (sourceWidth == width && sourceHeight == height) return;

	IMAGE resized(width, height);
	const DWORD* source = GetImageBuffer(&image);
	DWORD* target = GetImageBuffer(&resized);

	for (int y = 0; y < height; ++y) {
		double sy = height > 1
			? double(y) * (sourceHeight - 1) / (height - 1) : 0;
		int y0 = static_cast<int>(sy);
		int y1 = y0 + 1 < sourceHeight ? y0 + 1 : y0;
		double ty = sy - y0;

		for (int x = 0; x < width; ++x) {
			double sx = width > 1
				? double(x) * (sourceWidth - 1) / (width - 1) : 0;
			int x0 = static_cast<int>(sx);
			int x1 = x0 + 1 < sourceWidth ? x0 + 1 : x0;
			double tx = sx - x0;
			DWORD pixel = 0;

			// 颜色与透明度使用相同权重，保持透明边缘。
			for (int shift = 0; shift < 32; shift += 8) {
				double top = ((source[y0 * sourceWidth + x0] >> shift) & 255) * (1 - tx)
					+ ((source[y0 * sourceWidth + x1] >> shift) & 255) * tx;
				double bottom = ((source[y1 * sourceWidth + x0] >> shift) & 255) * (1 - tx)
					+ ((source[y1 * sourceWidth + x1] >> shift) & 255) * tx;
				pixel |= static_cast<DWORD>(top * (1 - ty) + bottom * ty + 0.5) << shift;
			}
			target[y * width + x] = pixel;
		}
	}
	image = resized;
}