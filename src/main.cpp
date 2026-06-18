#include <iostream>
#include "bsp_tree.h"
#include "map_renderer.h"

int main(int argc, char* argv[]) {
    BSPConfig config;

    // Permite pasar una semilla distinta como argumento: ./bsp_dungeon 1234
    if (argc > 1) {
        try {
            config.seed = static_cast<unsigned int>(std::stoul(argv[1]));
        } catch (...) {
            std::cerr << "Semilla invalida, usando valor por defecto (" << config.seed << ")\n";
        }
    }

    BSPTree tree(config);
    tree.generate();

    auto rooms    = tree.getRooms();
    auto corridors = tree.getCorridors();

    std::cout << "Mapa generado: " << rooms.size() << " habitaciones\n";
    std::cout << "Semilla: " << config.seed << "\n\n";

    MapRenderer renderer(config.mapWidth, config.mapHeight);
    renderer.render(rooms, corridors);

    return 0;
}