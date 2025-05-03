#include <iostream>
#include <chrono>
#include <string>
#include <sys/resource.h>
#include <tuple>
#include "ImageProcessor.h"
#include "BuddySystem.h"
#include <omp.h>

void printUsage()
{
    std::cout << "Uso: ./programa_imagen entrada.jpg salida.jpg -angulo <grados> -escalar <factor> [-buddy]\n";
}

std::tuple<long, long> procesarImagen(
    const std::string &inputFile,
    const std::string &outputFile,
    float angle,
    float scale,
    bool useBuddy,
    int ompThreads)
{
    struct rusage usageStart, usageEnd;
    getrusage(RUSAGE_SELF, &usageStart);
    auto start = std::chrono::high_resolution_clock::now();

    omp_set_num_threads(ompThreads);

    ImageProcessor processor(useBuddy);
    if (!processor.loadImage(inputFile))
    {
        std::cerr << "[ERROR] No se pudo cargar la imagen\n";
        return {-1, -1};
    }

    processor.rotateImage(angle);
    processor.scaleImage(scale);
    processor.saveImage(outputFile);

    auto end = std::chrono::high_resolution_clock::now();
    getrusage(RUSAGE_SELF, &usageEnd);

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    long memoryUsed = usageEnd.ru_maxrss - usageStart.ru_maxrss;

    return {duration, memoryUsed};
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
    std::cout << "------------------------\n";

    // Comparación: sin OpenMP (1 hilo)
    auto [tiempoSec, memSec] = procesarImagen(inputFile, "temp_sin_openmp.jpg", angle, scale, useBuddy, 1);

    // Comparación: con OpenMP (máximo hilos)
    auto [tiempoPar, memPar] = procesarImagen(inputFile, outputFile, angle, scale, useBuddy, omp_get_max_threads());

    std::cout << "TIEMPO DE PROCESAMIENTO:\n";
    std::cout << " - Sin OpenMP: " << tiempoSec << " ms\n";
    std::cout << " - Con OpenMP: " << tiempoPar << " ms\n";

    std::cout << "\nMEMORIA UTILIZADA:\n";
    std::cout << " - Sin OpenMP: " << memSec / 1024.0 << " MB\n";
    std::cout << " - Con OpenMP: " << memPar / 1024.0 << " MB\n";
    std::cout << "------------------------\n";
    std::cout << "[INFO] Imagen final guardada correctamente en " << outputFile << "\n";

    return 0;
}
