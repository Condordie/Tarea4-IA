#include "map_renderer.h"
#include <iostream>
#include <algorithm>

MapRenderer::MapRenderer(int width, int height)
    : width(width), height(height),
      grid(height, std::vector<char>(width, Tile::WALL))
{}

void MapRenderer::render(
    const std::vector<Rect>& rooms,
    const std::vector<std::pair<std::pair<int,int>, std::pair<int,int>>>& corridors)
{
    for (const auto& r : rooms)
        paintRoom(r);

    for (const auto& [a, b] : corridors)
        paintCorridor(a.first, a.second, b.first, b.second);

    // Coloca al jugador en el centro de la primera habitación
    if (!rooms.empty()) {
        int px = rooms[0].x + rooms[0].w / 2;
        int py = rooms[0].y + rooms[0].h / 2;
        grid[py][px] = Tile::PLAYER;
    }

    print();
}

// Rellena el interior de la habitación con tiles de suelo
void MapRenderer::paintRoom(const Rect& r) {
    for (int y = r.y; y < r.y + r.h && y < height; ++y)
        for (int x = r.x; x < r.x + r.w && x < width; ++x)
            grid[y][x] = Tile::FLOOR;
}

// Pasillo en L: primero horizontal hasta (x2, y1), luego vertical hasta (x2, y2)
void MapRenderer::paintCorridor(int x1, int y1, int x2, int y2) {
    int startX = std::min(x1, x2);
    int endX   = std::max(x1, x2);
    for (int x = startX; x <= endX && x < width; ++x)
        if (grid[y1][x] == Tile::WALL) grid[y1][x] = Tile::CORRIDOR;

    int startY = std::min(y1, y2);
    int endY   = std::max(y1, y2);
    for (int y = startY; y <= endY && y < height; ++y)
        if (grid[y][x2] == Tile::WALL) grid[y][x2] = Tile::CORRIDOR;
}

char MapRenderer::getTile(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) return Tile::WALL;
    return grid[y][x];
}

void MapRenderer::print() const {
    for (const auto& row : grid) {
        for (char c : row) std::cout << c;
        std::cout << '\n';
    }
}