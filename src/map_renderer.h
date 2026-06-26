#pragma once
#include "bsp_tree.h"
#include <vector>
#include <string>

// Caracteres usados para el mapa, estilo Rogue clásico
namespace Tile {
    constexpr char WALL    = '#';
    constexpr char FLOOR   = '.';
    constexpr char CORRIDOR = ' ';
    constexpr char PLAYER  = '@';
    constexpr char EMPTY   = ' ';
}

class MapRenderer {
public:
    MapRenderer(int width, int height);

    // Pinta habitaciones y pasillos sobre la grilla y luego imprime en consola
    void render(const std::vector<Rect>& rooms,
                const std::vector<std::pair<std::pair<int,int>,
                                            std::pair<int,int>>>& corridors);

    // Devuelve el tile en (x, y), útil para el agente
    char getTile(int x, int y) const;

private:
    int width, height;
    std::vector<std::vector<char>> grid;

    // Rellena el rectángulo con tiles de suelo
    void paintRoom(const Rect& r);

    // Dibuja un pasillo en L entre dos puntos
    void paintCorridor(int x1, int y1, int x2, int y2);

    // Imprime la grilla completa en stdout
    void print() const;
};