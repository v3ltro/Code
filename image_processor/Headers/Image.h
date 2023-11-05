#pragma once

#include "Exceptions.h"

#include <vector>
#include <iostream>
#include <fstream>

struct Color {
    double r, g, b;

    Color();
    Color(double r, double g, double b);
    Color operator*(double coeff) const;
    Color operator+(const Color& color) const;
    Color& operator+=(const Color& color);
};

class Image {
public:
    Image(size_t width, size_t height);  // с size_t перестает адекватно работать поиск ближайшей клетки

    Color GetColor(size_t x, size_t y) const;
    void SetColor(const Color& color, size_t x, size_t y);
    void SetWidth(size_t width);
    void SetHeight(size_t height);

    void Export(std::ofstream& os) const;
    void Read(std::ifstream& of);

    size_t GetWidth() const;
    size_t GetHeight() const;
    Image Crop(size_t width, size_t height) const;

private:
    size_t m_width_;
    size_t m_height_;
    std::vector<std::vector<Color>> m_colors_;
};