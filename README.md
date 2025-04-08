# Procesador de Imágenes con Buddy System

Este proyecto implementa un programa en C++ para realizar operaciones básicas de procesamiento de imágenes como rotación y escalado, y comparar el rendimiento entre la gestión de memoria tradicional (`new/delete`) y el sistema de asignación de memoria **Buddy System**.

---

## Estructura del Proyecto

- `main.cpp` – Programa principal y lógica de comparación.
- `ImageProcessor.cpp/.h` – Clase encargada del procesamiento de imágenes.
- `BuddySystem.cpp/.h` – Implementación del sistema Buddy para asignación de memoria.
- `stb_image.h` y `stb_image_write.h` – Librerías para carga y guardado de imágenes.

---

## Compilación

Desde la raiz del proyecto, ejecuta:

```bash
make
```

Esto generará el ejecutable programa_imagen.

Para limpiar los archivos intermedios:

```bash
make clean
```

## Modo de uso

```bash
./programa_imagen <entrada> <salida> -angulo <grados> -escalar <factor>
```

Ejemplo

```bash
./programa_imagen entrada.jpg salida.png -angulo 45 -escalar 1.2
```
