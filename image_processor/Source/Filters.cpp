#include "../Headers/Filters.h"
#include "../Headers/MatrixFilter.h"

#include <unordered_map>
#include <cstring>
#include <string>
#include <cmath>
#include <random>
#include <ctime>

Image Image::Crop(size_t width, size_t height) const {
    if (width < 0 || height < 0) {
        throw FilterArgumentException("Crop arguments must be positive integers\n");
    }
    height = std::min(height, m_height_);
    width = std::min(width, m_width_);
    Image cropped(width, height);
    for (size_t j = 0; j < height; ++j) {
        const std::vector<Color>& source_row = m_colors_[m_height_ - height + j];
        std::vector<Color>& cropped_row = cropped.m_colors_[j];
        for (size_t i = 0; i < width; ++i) {
            cropped_row[i] = source_row[i];
        }
    }
    return cropped;
}

void Crop(Image& filtered, const Image& image, std::vector<std::string_view> par) {
    filtered = image.Crop(std::stoi(static_cast<std::string>(par[0])), std::stoi(static_cast<std::string>(par[1])));
}

Image GenerateTemplate(size_t width, size_t height, std::string_view monochrome) {
    std::srand(std::time(nullptr));
    bool bnw;  // NOLINT
    if (static_cast<std::string>(monochrome) == "true") {
        bnw = true;
    } else if (static_cast<std::string>(monochrome) == "false") {
        bnw = false;
    } else {
        throw(FilterArgumentException("Invalid monochrome value - expected true or false\n"));
    }
    const int16_t val = 256;
    const double normalize = 255.0;
    Image noise(width, height);
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            Color new_color;
            new_color.r = rand() % val / normalize;
            if (bnw) {
                new_color.g = new_color.r;
                new_color.b = new_color.g;
            } else {
                new_color.g = rand() % val / normalize;
                new_color.b = rand() % val / normalize;
            }
            noise.SetColor(new_color, x, y);
        }
    }
    return noise;
}

void Noise(Image& filtered, const Image& image, std::vector<std::string_view> par) {
    double transparency = std::stod(static_cast<std::string>(par[1]));
    if (transparency < 0 || transparency > 1) {
        throw(FilterArgumentException("Wrong transparency value, expected double between 0 and 1\n"));
    }
    Image noise = GenerateTemplate(image.GetWidth(), image.GetHeight(), par[0]);
    filtered.SetWidth(image.GetWidth());
    filtered.SetHeight(image.GetHeight());
    for (size_t y = 0; y < image.GetHeight(); ++y) {
        for (size_t x = 0; x < image.GetWidth(); ++x) {
            filtered.SetColor((image.GetColor(x, y) * (1 - transparency)) + (noise.GetColor(x, y) * transparency), x,
                              y);
        }
    }
}

void Grayscale(Image& filtered, const Image& image, std::vector<std::string_view> par) {
    filtered.SetWidth(image.GetWidth());
    filtered.SetHeight(image.GetHeight());
    const double r_coeff = 0.299;
    const double g_coeff = 0.587;
    const double b_coeff = 0.114;
    for (size_t y = 0; y < image.GetHeight(); ++y) {
        for (size_t x = 0; x < image.GetWidth(); ++x) {
            Color gray_color = image.GetColor(x, y);
            gray_color.r = r_coeff * gray_color.r + g_coeff * gray_color.g + b_coeff * gray_color.b;
            gray_color.g = gray_color.r;
            gray_color.b = gray_color.g;
            filtered.SetColor(gray_color, x, y);
        }
    }
}

void Negative(Image& filtered, const Image& image, std::vector<std::string_view> par) {
    filtered.SetWidth(image.GetWidth());
    filtered.SetHeight(image.GetHeight());
    for (size_t y = 0; y < image.GetHeight(); ++y) {
        for (size_t x = 0; x < image.GetWidth(); ++x) {
            Color inverted_color = image.GetColor(x, y);
            inverted_color.r = 1 - inverted_color.r;
            inverted_color.g = 1 - inverted_color.g;
            inverted_color.b = 1 - inverted_color.b;
            filtered.SetColor(inverted_color, x, y);
        }
    }
}

void Sharpening(Image& filtered, const Image& image, std::vector<std::string_view> par) {
    filtered = MatrixFilter(image, {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}});  // NOLINT: thanks
}

void EdgeDetection(Image& filtered, const Image& image, std::vector<std::string_view> par) {
    double threshold = std::stod(static_cast<std::string>(par[0]));
    Grayscale(filtered, image, par);
    filtered = MatrixFilter(filtered, {{0, -1, 0}, {-1, 4, -1}, {0, -1, 0}}, threshold);
}

void GaussianBlur(Image& filtered, const Image& image, std::vector<std::string_view> par) {
    double sigma = std::stod(static_cast<std::string>(par[0]));
    filtered.SetWidth(image.GetWidth());
    filtered.SetHeight(image.GetHeight());
    size_t size = static_cast<size_t>(std::ceil(6 * std::abs(sigma)));  // NOLINT
    size += !(size % 2);
    std::vector<double> coefficients(size);
    int64_t half = static_cast<int64_t>(coefficients.size() / 2);
    const double coeff1 = 2 * sigma * sigma;
    const double coeff2 = std::sqrt(2 * M_PI * sigma * sigma);
    Image transition(image.GetWidth(), image.GetHeight());  // First iteration, blurring along vertical axis
    for (int64_t x = 0; x <= half; ++x) {
        double val = std::exp(-static_cast<double>(x * x) / coeff1) / coeff2;
        coefficients[half - x] = val;
        coefficients[half + x] = val;
    }
    for (int64_t y = 0; y < static_cast<int64_t>(image.GetHeight()); ++y) {
        for (int64_t x = 0; x < static_cast<int64_t>(image.GetWidth()); ++x) {
            Color color;
            for (int64_t i = -half; i <= half; ++i) {
                color += image.GetColor(x, y + i) * coefficients[half + i];
            }
            transition.SetColor(color, x, y);
        }
    }
    for (int64_t y = 0; y < static_cast<int64_t>(image.GetHeight()); ++y) {
        for (int64_t x = 0; x < static_cast<int64_t>(image.GetHeight()); ++x) {
            Color color;
            for (int64_t i = -half; i <= half; ++i) {
                color += transition.GetColor(x + i, y) * coefficients[half + i];
            }
            filtered.SetColor(color, x, y);
        }
    }
}

void ApplyFilter(Image& filtered, const Image& image, std::pair<std::string, std::vector<std::string_view>> filter) {
    std::unordered_map<std::string, size_t> arg_count = {{"-crop", 2}, {"-gs", 0},   {"-neg", 0},  {"-sharp", 0},
                                                         {"-edge", 1}, {"-blur", 1}, {"-noise", 2}};
    std::unordered_map<std::string, void (*)(Image&, const Image&, std::vector<std::string_view>)> map = {
        {"-gs", Grayscale},      {"-neg", Negative}, {"-sharp", Sharpening}, {"-edge", EdgeDetection},
        {"-blur", GaussianBlur}, {"-crop", Crop},    {"-noise", Noise}};
    if (arg_count[filter.first] != filter.second.size()) {
        throw FilterArgumentException("Invalid \"" + filter.first + "\" arguments count. See help for reference\n");
    }
    try {
        map[filter.first](filtered, image, filter.second);
    } catch (const std::invalid_argument& error) {
        throw FilterArgumentException("Invalid \"" + filter.first + "\" argument type. See help for reference\n");
    }
}

void FilterChain(Image& image, std::vector<std::pair<std::string, std::vector<std::string_view>>> filters) {
    const Image& start = image;
    for (size_t i = 0; i < filters.size(); ++i) {
        ApplyFilter(image, start, filters[i]);
    }
}