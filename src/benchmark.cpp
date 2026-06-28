// Script de experimentación 
// Recorre combinaciones de mapWidth, mapHeight y seed, ejecuta
// BSPTree::generate() para cada una, y registra:
//   - tiempo de generación (ms)
//   - número de habitaciones generadas exitosamente
//
// Salida: benchmark_data.txt, con columnas separadas por espacio:
//   width height seed time_ms rooms
//
// Ese formato es directamente legible por:
//   - gnuplot:      splot "benchmark_data.txt" using 1:2:4 with points
//   - Python:       pandas.read_csv("benchmark_data.txt", sep=r"\s+")
//   - Excel:        importar como texto delimitado por espacios
//   - LaTeX/pgfplots: \addplot3 table {benchmark_data.txt};
//
// Compilar:
//   g++ -std=c++17 -O2 -o benchmark benchmark.cpp bsp_tree.cpp
// Ejecutar:
//   ./benchmark

#include "bsp_tree.h"
#include <fstream>
#include <iostream>
#include <vector>

int main() {
    // Rangos a explorar. Se mantienen el resto de los parámetros de
    // BSPConfig en sus valores por defecto (minLeafSize, maxDepth, etc.)
    // para aislar el efecto de width/height/seed.
    std::vector<int> widths  = { 40, 80, 120, 160, 200, 240 };
    std::vector<int> heights = { 20, 40, 60, 80, 100, 120 };
    std::vector<unsigned int> seeds = { 1, 2, 3, 4, 5 };

    const std::string outPath = "benchmark_data.txt";
    std::ofstream out(outPath);
    if (!out) {
        std::cerr << "No se pudo crear " << outPath << "\n";
        return 1;
    }

    out << "width height seed time_ms rooms\n";

    int totalRuns = static_cast<int>(widths.size() * heights.size() * seeds.size());
    int done = 0;

    for (int w : widths) {
        for (int h : heights) {
            for (unsigned int s : seeds) {
                BSPConfig cfg;
                cfg.mapWidth  = w;
                cfg.mapHeight = h;
                cfg.seed      = s;

                BSPTree tree(cfg);
                tree.generate();

                double timeMs   = tree.getLastGenerationTimeMs();
                size_t roomCount = tree.getRooms().size();

                out << w << " " << h << " " << s << " "
                    << timeMs << " " << roomCount << "\n";

                ++done;
            }
        }
    }

    out.close();

    std::cout << "Listo: " << done << "/" << totalRuns
              << " combinaciones escritas en " << outPath << "\n";
    return 0;
}