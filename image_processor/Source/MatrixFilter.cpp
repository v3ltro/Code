#include "../Headers/MatrixFilter.h"

Image MatrixFilter(const Image& image, const std::vector<std::vector<double>>& matrix, double thresh) {
    Image new_image = Image(image.GetWidth(), image.GetHeight());
    for (size_t y = 0; y < image.GetHeight(); ++y) {
        for (size_t x = 0; x < image.GetWidth(); ++x) {
            Color new_color;
            for (int64_t i = -1; i <= 1; ++i) {
                for (int64_t j = -1; j <= 1; ++j) {
                    new_color += image.GetColor(x + j, y + i) * matrix[i + 1][j + 1];
                }
            }
            if (thresh == 4) {
                new_color.r = std::min(1.0, std::max(new_color.r, 0.0));
                new_color.g = std::min(1.0, std::max(new_color.g, 0.0));
                new_color.b = std::min(1.0, std::max(new_color.b, 0.0));
            } else {
                if (new_color.r > thresh) {
                    new_color = Color{1, 1, 1};
                } else {
                    new_color = Color{0, 0, 0};
                }
            }

            new_image.SetColor(new_color, x, y);
        }
    }
    return new_image;
}