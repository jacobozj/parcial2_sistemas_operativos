#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <string>

class ImageProcessor
{
public:
    ImageProcessor(bool useBuddy);
    ~ImageProcessor();

    bool loadImage(const std::string &filename);
    void rotateImage(float angle);
    void scaleImage(float scale);
    bool saveImage(const std::string &filename);
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getChannels() const { return channels; }

private:
    unsigned char *pixels;         // Matriz de salida
    unsigned char *originalPixels; // Matriz original para interpolación
    int width, height, channels;
    int originalWidth, originalHeight; // Dimensiones originales
    bool useBuddySystem;
    void *buddyMemory;

    float interpolateBilinear(float x, float y, int channel);
    void allocateMemory(size_t size);
    void freeMemory();
};

#endif