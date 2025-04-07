#include "ImageProcessor.h"
#include "BuddySystem.h"
#include <iostream>
#include <cmath>
#include <cstring>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

ImageProcessor::ImageProcessor(bool useBuddy)
    : useBuddySystem(useBuddy), pixels(nullptr), originalPixels(nullptr),
      buddyMemory(nullptr), width(0), height(0), channels(0),
      originalWidth(0), originalHeight(0) {}

ImageProcessor::~ImageProcessor()
{
    freeMemory();
    if (originalPixels)
        delete[] originalPixels;
}

bool ImageProcessor::loadImage(const std::string &filename)
{
    pixels = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    if (!pixels)
    {
        std::cerr << "Error: No se pudo cargar la imagen " << filename << "\n";
        return false;
    }

    size_t size = width * height * channels;
    originalPixels = new unsigned char[size];
    memcpy(originalPixels, pixels, size);
    originalWidth = width;
    originalHeight = height;

    if (useBuddySystem)
    {
        buddyMemory = BuddySystem::allocate(size + sizeof(BuddySystem::Block));
        if (!buddyMemory)
        {
            std::cerr << "Error: Buddy System no pudo asignar memoria\n";
            delete[] originalPixels;
            originalPixels = nullptr;
            stbi_image_free(pixels); // esto sí está bien aquí, porque viene de stbi_load
            return false;
        }

        // Offset después del header
        pixels = reinterpret_cast<unsigned char *>(
            static_cast<char *>(buddyMemory) + sizeof(BuddySystem::Block));

        memcpy(pixels, originalPixels, size);

        // ✅ NO liberar pixels aquí, ya que apuntan a buddyMemory
    }
    else
    {
        stbi_image_free(pixels);
        pixels = new unsigned char[size];
        memcpy(pixels, originalPixels, size);
    }

    return true;
}

float ImageProcessor::interpolateBilinear(float x, float y, int channel)
{
    int x0 = static_cast<int>(x);
    int y0 = static_cast<int>(y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    if (x0 >= originalWidth || y0 >= originalHeight || x1 < 0 || y1 < 0)
        return 0;

    x0 = std::max(0, std::min(x0, originalWidth - 1));
    x1 = std::max(0, std::min(x1, originalWidth - 1));
    y0 = std::max(0, std::min(y0, originalHeight - 1));
    y1 = std::max(0, std::min(y1, originalHeight - 1));

    float dx = x - x0;
    float dy = y - y0;

    int idx00 = (y0 * originalWidth + x0) * channels + channel;
    int idx01 = (y0 * originalWidth + x1) * channels + channel;
    int idx10 = (y1 * originalWidth + x0) * channels + channel;
    int idx11 = (y1 * originalWidth + x1) * channels + channel;

    float v00 = originalPixels[idx00];
    float v01 = originalPixels[idx01];
    float v10 = originalPixels[idx10];
    float v11 = originalPixels[idx11];

    return (1 - dx) * (1 - dy) * v00 + dx * (1 - dy) * v01 +
           (1 - dx) * dy * v10 + dx * dy * v11;
}

void ImageProcessor::rotateImage(float angle)
{
    float rad = angle * M_PI / 180.0f;
    float cosA = cos(rad);
    float sinA = sin(rad);

    int newWidth = static_cast<int>(fabs(originalWidth * cosA) + fabs(originalHeight * sinA));
    int newHeight = static_cast<int>(fabs(originalWidth * sinA) + fabs(originalHeight * cosA));
    size_t newSize = newWidth * newHeight * channels;

    allocateMemory(newSize);

    float cx = originalWidth / 2.0f;
    float cy = originalHeight / 2.0f;
    float newCx = newWidth / 2.0f;
    float newCy = newHeight / 2.0f;

    std::cout << "[DEBUG] Nuevas dimensiones: " << newWidth << "x" << newHeight << "\n";

    for (int y = 0; y < newHeight; ++y)
    {
        for (int x = 0; x < newWidth; ++x)
        {
            float dx = x - newCx;
            float dy = y - newCy;

            float srcX = cosA * dx + sinA * dy + cx;
            float srcY = -sinA * dx + cosA * dy + cy;

            int idx = (y * newWidth + x) * channels;
            for (int c = 0; c < channels; ++c)
            {
                float value = interpolateBilinear(srcX, srcY, c);
                pixels[idx + c] = static_cast<unsigned char>(std::max(0.0f, std::min(255.0f, value)));
            }
        }
    }

    width = newWidth;
    height = newHeight;

    if (originalPixels)
        delete[] originalPixels;

    originalWidth = width;
    originalHeight = height;
    originalPixels = new unsigned char[newSize];
    memcpy(originalPixels, pixels, newSize);
}

void ImageProcessor::scaleImage(float scale)
{
    int newWidth = static_cast<int>(originalWidth * scale);
    int newHeight = static_cast<int>(originalHeight * scale);
    size_t newSize = newWidth * newHeight * channels;

    allocateMemory(newSize);

    for (int y = 0; y < newHeight; ++y)
    {
        for (int x = 0; x < newWidth; ++x)
        {
            float srcX = x / scale;
            float srcY = y / scale;
            int idx = (y * newWidth + x) * channels;
            for (int c = 0; c < channels; ++c)
            {
                float value = interpolateBilinear(srcX, srcY, c);
                pixels[idx + c] = static_cast<unsigned char>(std::max(0.0f, std::min(255.0f, value)));
            }
        }
    }

    width = newWidth;
    height = newHeight;

    if (originalPixels)
        delete[] originalPixels;

    originalWidth = width;
    originalHeight = height;
    originalPixels = new unsigned char[newSize];
    memcpy(originalPixels, pixels, newSize);
}

void ImageProcessor::allocateMemory(size_t size)
{
    freeMemory();

    if (useBuddySystem)
    {
        buddyMemory = BuddySystem::allocate(size + sizeof(BuddySystem::Block));
        if (!buddyMemory)
        {
            std::cerr << "Error: Buddy System no pudo asignar memoria\n";
            return;
        }

        pixels = reinterpret_cast<unsigned char *>(
            static_cast<char *>(buddyMemory) + sizeof(BuddySystem::Block));
    }
    else
    {
        pixels = new unsigned char[size];
    }

    if (pixels)
    {
        memset(pixels, 0, size);
    }
}

void ImageProcessor::freeMemory()
{
    if (useBuddySystem && buddyMemory)
    {
        BuddySystem::free(buddyMemory);
        buddyMemory = nullptr;
    }
    else if (pixels && !useBuddySystem)
    {
        delete[] pixels;
    }

    pixels = nullptr;
}

bool ImageProcessor::saveImage(const std::string &filename)
{
    return stbi_write_jpg(filename.c_str(), width, height, channels, pixels, 100) != 0;
}
