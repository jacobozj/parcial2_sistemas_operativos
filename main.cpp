#include <iostream>
#include <chrono>
#include <string>
#include <sys/resource.h>
#include "ImageProcessor.h"
#include "BuddySystem.h"

void printUsage()
{
    std::cout << "Uso: ./programa_imagen entrada.jpg salida.jpg -angulo <grados> -escalar <factor> [-buddy]\n";
}

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        printUsage();
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];
    float angle = 0.0f;
    float scale = 1.0f;
    bool useBuddy = false;

    for (int i = 3; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-angulo" && i + 1 < argc)
            angle = std::stof(argv[++i]);
        else if (arg == "-escalar" && i + 1 < argc)
            scale = std::stof(argv[++i]);
        else if (arg == "-buddy")
            useBuddy = true;
    }

    std::cout << "=== PROCESAMIENTO DE IMAGEN ===\n";
    std::cout << "Archivo de entrada: " << inputFile << "\n";
    std::cout << "Archivo de salida: " << outputFile << "\n";
    std::cout << "Modo de asignación de memoria: " << (useBuddy ? "Buddy System" : "Convencional") << "\n";

    struct rusage usageStart, usageEnd;
    getrusage(RUSAGE_SELF, &usageStart);

    auto start = std::chrono::high_resolution_clock::now();

    ImageProcessor processor(useBuddy);
    if (!processor.loadImage(inputFile))
    {
        std::cerr << "Fallo al iniciar el procesamiento de la imagen\n";
        return 1;
    }

    std::cout << "Dimensiones originales: " << processor.getWidth() << "x" << processor.getHeight() << "\n";
    std::cout << "Canales: " << processor.getChannels() << " (RGB)\n";
    std::cout << "Ángulo de rotación: " << angle << " grados\n";
    std::cout << "Factor de escalado: " << scale << "\n";

    processor.rotateImage(angle);
    std::cout << "[INFO] Imagen rotada correctamente.\n";

    processor.scaleImage(scale);
    std::cout << "[INFO] Imagen escalada correctamente.\n";

    std::cout << "Dimensiones finales: " << processor.getWidth() << "x" << processor.getHeight() << "\n";

    processor.saveImage(outputFile);
    std::cout << "[INFO] Imagen guardada correctamente en " << outputFile << "\n";

    auto end = std::chrono::high_resolution_clock::now();
    getrusage(RUSAGE_SELF, &usageEnd);

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    long memoryUsed = usageEnd.ru_maxrss - usageStart.ru_maxrss;

    std::cout << "\nTIEMPO DE PROCESAMIENTO:\n";
    std::cout << "- " << (useBuddy ? "Con Buddy System" : "Sin Buddy System") << ": " << duration << " ms\n";
    std::cout << "\nMEMORIA UTILIZADA:\n";
    std::cout << "- " << (useBuddy ? "Con Buddy System" : "Sin Buddy System") << ": " << memoryUsed / 1024.0 << " MB\n";

    return 0;
}