#pragma once

class IMAGE;

void drawPng(int x, int y, int width, int height, IMAGE& image, unsigned char opacity = 255);
void drawPngWhite(int x, int y, int width, int height, IMAGE& image);