#pragma once

#include "Image.h"

void Crop(Image& filtered, const Image& image, std::vector<std::string_view> filter);
void EdgeDetection(Image& filtered, const Image& image, std::vector<std::string_view> filter);
void GaussianBlur(Image& filtered, const Image& image, std::vector<std::string_view> filter);
void Grayscale(Image& filtered, const Image& image, std::vector<std::string_view> filter);
void Negative(Image& filtered, const Image& image, std::vector<std::string_view> filter);
void Noise(Image& filtered, const Image& image, std::vector<std::string_view> filter);
void Sharpening(Image& filtered, const Image& image, std::vector<std::string_view> filter);

void ApplyFilter(const Image& image, std::pair<std::string, std::vector<std::string_view>> filter);
void FilterChain(Image& image, std::vector<std::pair<std::string, std::vector<std::string_view>>> filters);