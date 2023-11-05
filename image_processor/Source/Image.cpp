#include "../Headers/Image.h"

#include <limits>

const double F = 255.0;
const int16_t FIVE = 5;
const int16_t SIX = 6;
const int16_t SEVEN = 7;
const int16_t EIGHT = 8;
const int16_t NINE = 9;
const int16_t TEN = 10;
const int16_t ELEVEN = 11;
const int16_t SIXTEEN = 16;
const int16_t TWENTY_FOUR = 24;
const int32_t P1 = 256;
const int32_t P2 = 65536;
const int32_t P3 = 16777216;

void Normalize(size_t& coordinate, size_t limit) {
    if (coordinate >= limit) {
        coordinate = coordinate > std::numeric_limits<size_t>::max() / 2 ? 0 : limit - 1;
    }
}

Color::Color() : r(0), g(0), b(0) {
}

Color::Color(double r, double g, double b) : r(r), g(g), b(b) {
}

Image::Image(size_t width, size_t height) {
    m_width_ = width;
    m_height_ = height;
    m_colors_.resize(height);
    for (size_t i = 0; i < height; ++i) {
        m_colors_[i].resize(width);
    }
}

Color Image::GetColor(size_t x, size_t y) const {
    Normalize(x, m_width_);
    Normalize(y, m_height_);
    return m_colors_[y][x];
}

void Image::SetColor(const Color& color, size_t x, size_t y) {
    m_colors_[y][x].r = color.r;
    m_colors_[y][x].g = color.g;
    m_colors_[y][x].b = color.b;
}

void Image::SetHeight(size_t height) {
    m_height_ = height;
}

void Image::SetWidth(size_t width) {
    m_width_ = width;
}

void Image::Export(std::ofstream& os) const {
    unsigned char bmp_pad[3] = {0, 0, 0};
    const size_t padding_amount = ((4 - (m_width_ * 3) % 4) % 4);

    const size_t file_header_size = 14;
    const size_t information_header_size = 40;
    const size_t file_size =
        file_header_size + information_header_size + m_width_ * m_height_ * 3 + padding_amount * m_height_;

    unsigned char file_header[file_header_size] = {
        'B',
        'M',
        static_cast<unsigned char>(file_size),
        static_cast<unsigned char>(file_size >> EIGHT),
        static_cast<unsigned char>(file_size >> SIXTEEN),
        static_cast<unsigned char>(file_size >> TWENTY_FOUR),
    };

    file_header[TEN] = file_header_size + information_header_size;

    unsigned char information_header[information_header_size] = {information_header_size,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 static_cast<unsigned char>(m_width_),
                                                                 static_cast<unsigned char>(m_width_ >> EIGHT),
                                                                 static_cast<unsigned char>(m_width_ >> SIXTEEN),
                                                                 static_cast<unsigned char>(m_width_ >> TWENTY_FOUR),
                                                                 static_cast<unsigned char>(m_height_),
                                                                 static_cast<unsigned char>(m_height_ >> EIGHT),
                                                                 static_cast<unsigned char>(m_height_ >> SIXTEEN),
                                                                 static_cast<unsigned char>(m_height_ >> TWENTY_FOUR),
                                                                 1,
                                                                 0,
                                                                 TWENTY_FOUR};

    os.write(reinterpret_cast<char*>(file_header), file_header_size);
    os.write(reinterpret_cast<char*>(information_header), information_header_size);

    for (size_t y = 0; y < m_height_; ++y) {
        for (size_t x = 0; x < m_width_; ++x) {
            unsigned char r = static_cast<unsigned char>(GetColor(x, y).r * F);
            unsigned char g = static_cast<unsigned char>(GetColor(x, y).g * F);
            unsigned char b = static_cast<unsigned char>(GetColor(x, y).b * F);

            unsigned char color[] = {b, g, r};

            os.write(reinterpret_cast<char*>(color), 3);
        }

        os.write(reinterpret_cast<char*>(bmp_pad), static_cast<int64_t>(padding_amount));
    }

    os.close();

    if (!os.fail()) {
        std::cout << "File created successfully.\n";
    } else {
        std::cerr << "Error creating file. Check if there is enough space on the drive.\n";
    }
}

void Image::Read(std::ifstream& of) {
    const size_t file_header_size = 14;
    const size_t information_header_size = 40;

    unsigned char file_header[file_header_size];
    of.read(reinterpret_cast<char*>(file_header), file_header_size);

    if (file_header[0] != 'B' && file_header[1] != 'M') {
        of.close();
        std::cerr << "The specified path is not a BMP image\n";
        return;
    }

    unsigned char information_header[information_header_size];
    of.read(reinterpret_cast<char*>(information_header), information_header_size);

    m_width_ = information_header[4] + information_header[FIVE] * P1 + information_header[SIX] * P2 +
               information_header[SEVEN] * P3;
    m_height_ = information_header[EIGHT] + information_header[NINE] * P1 + information_header[TEN] * P2 +
                information_header[ELEVEN] * P3;

    m_colors_.resize(m_height_);
    for (size_t i = 0; i < m_height_; ++i) {
        m_colors_[i].resize(m_width_);
    }

    const size_t padding_amount = (4 - (m_width_ * 3) % 4) % 4;

    for (size_t y = 0; y < m_height_; ++y) {
        for (size_t x = 0; x < m_width_; ++x) {
            unsigned char color[3];
            of.read(reinterpret_cast<char*>(color), 3);

            m_colors_[y][x].r = static_cast<double>(color[2]) / F;
            m_colors_[y][x].g = static_cast<double>(color[1]) / F;
            m_colors_[y][x].b = static_cast<double>(color[0]) / F;
        }

        of.ignore(static_cast<int64_t>(padding_amount));
    }

    of.close();
    std::cout << "File read\n";
}

Color Color::operator*(double coeff) const {
    return Color{r * coeff, g * coeff, b * coeff};
}

Color& Color::operator+=(const Color& color) {
    r += color.r;
    g += color.g;
    b += color.b;
    return *this;
}

Color Color::operator+(const Color& color) const {
    return Color{r + color.r, g + color.g, b + color.b};
}

size_t Image::GetHeight() const {
    return m_height_;
}

size_t Image::GetWidth() const {
    return m_width_;
}