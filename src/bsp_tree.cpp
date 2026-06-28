#include "bsp_tree.h"
#include <algorithm>
#include <stdexcept>
#include <functional>
#include <thread>
#include <chrono>
#include <iostream>

BSPTree::BSPTree(const BSPConfig& config)
    : cfg(config), rng(config.seed)
{
    root = std::make_unique<BSPNode>(Rect(0, 0, cfg.mapWidth, cfg.mapHeight));
}

void BSPTree::setAnimate(bool value) {
    animate = value;
}

void BSPTree::renderPartitions() const {
    std::vector<std::string> grid(cfg.mapHeight, std::string(cfg.mapWidth, ' '));
    std::vector<Rect> leaves;
    collectLeafRegions(root.get(), leaves);

    for (const Rect& leaf : leaves) {
        int x1 = std::max(0, leaf.x);
        int x2 = std::min(cfg.mapWidth - 1, leaf.x + leaf.w - 1);
        int y1 = std::max(0, leaf.y);
        int y2 = std::min(cfg.mapHeight - 1, leaf.y + leaf.h - 1);

        for (int x = x1; x <= x2; ++x) {
            grid[y1][x] = '-';
            grid[y2][x] = '-';
        }
        for (int y = y1; y <= y2; ++y) {
            grid[y][x1] = '|';
            grid[y][x2] = '|';
        }
        if (x1 < cfg.mapWidth && y1 < cfg.mapHeight)
            grid[y1][x1] = '+';
        if (x1 < cfg.mapWidth && y2 < cfg.mapHeight)
            grid[y2][x1] = '+';
        if (x2 < cfg.mapWidth && y1 < cfg.mapHeight)
            grid[y1][x2] = '+';
        if (x2 < cfg.mapWidth && y2 < cfg.mapHeight)
            grid[y2][x2] = '+';
    }

    std::cout << "\x1B[2J\x1B[H"; // limpiar pantalla y mover cursor arriba
    std::cout << "BSP partitions (leaf regions): " << leaves.size() << "\n";
    for (const auto& row : grid) {
        std::cout << row << '\n';
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
}

void BSPTree::collectLeafRegions(BSPNode* node, std::vector<Rect>& leaves) const {
    if (!node) return;
    if (node->isLeaf()) {
        leaves.push_back(node->region);
        return;
    }
    collectLeafRegions(node->left.get(), leaves);
    collectLeafRegions(node->right.get(), leaves);
}

void BSPTree::generate() {
    auto t0 = std::chrono::high_resolution_clock::now();

    split(root.get(), 0);
    buildRooms(root.get());

    auto t1 = std::chrono::high_resolution_clock::now();
    lastGenerationTimeMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
}

double BSPTree::getLastGenerationTimeMs() const {
    return lastGenerationTimeMs;
}

// Divide el nodo en dos hijos eligiendo un eje (horizontal o vertical) al azar
void BSPTree::split(BSPNode* node, int depth) {

    // Si se alcanza la profundidad máxima, no se divide más
    if (depth >= cfg.maxDepth) return;
    
    // Si la región es demasiado pequeña, no se puede dividir más
    Rect& r = node->region;
    
    // Comprobamos si es posible dividir horizontal o verticalmente
    bool canSplitH = r.h >= cfg.minLeafSize * 2;
    bool canSplitV = r.w >= cfg.minLeafSize * 2;

    // Si no se puede dividir en ningún eje, termina
    if (!canSplitH && !canSplitV) return;

    // Si solo un eje es posible, lo usamos; si ambos, elegimos al azar
    bool splitHorizontal;
    if (canSplitH && canSplitV) {
        std::uniform_int_distribution<int> coin(0, 1);
        splitHorizontal = coin(rng);
    } else {
        splitHorizontal = canSplitH;
    }
    // Elegimos un punto de corte aleatorio dentro del rango permitido
    std::uniform_real_distribution<float> ratio(cfg.splitMin, cfg.splitMax);
    // Creamos los nodos hijos con las regiones correspondientes
    if (splitHorizontal) {
        int cut = static_cast<int>(r.h * ratio(rng));
        node->left  = std::make_unique<BSPNode>(Rect(r.x, r.y,       r.w, cut));
        node->right = std::make_unique<BSPNode>(Rect(r.x, r.y + cut, r.w, r.h - cut));
    } else {
        int cut = static_cast<int>(r.w * ratio(rng));
        node->left  = std::make_unique<BSPNode>(Rect(r.x,       r.y, cut,       r.h));
        node->right = std::make_unique<BSPNode>(Rect(r.x + cut, r.y, r.w - cut, r.h));
    }
    // Llamamos recursivamente a split en los hijos, aumentando la profundidad.
    if (animate) renderPartitions();
    split(node->left.get(),  depth + 1);
    split(node->right.get(), depth + 1);
}

// Coloca una habitación dentro de la región de la hoja con tamaño aleatorio
void BSPTree::createRoom(BSPNode* node) {
    // Aseguramos que la habitación tenga un tamaño mínimo y máximo relativo a la región
    Rect& r = node->region;
    // El tamaño de la habitación se elige aleatoriamente entre un porcentaje del tamaño de la región
    std::uniform_real_distribution<float> rf(cfg.roomMinRatio, cfg.roomMaxRatio);
    int rw = std::max(3, static_cast<int>(r.w * rf(rng)));
    int rh = std::max(3, static_cast<int>(r.h * rf(rng)));

    // Posición aleatoria dentro de la región para que no siempre esté en la esquina
    std::uniform_int_distribution<int> ox(0, r.w - rw);
    std::uniform_int_distribution<int> oy(0, r.h - rh);
    // Creamos la habitación y la asignamos al nodo
    node->room = new Rect(r.x + ox(rng), r.y + oy(rng), rw, rh);
}

// Recorre el árbol en postorden: primero los hijos, después conecta sus habitaciones
void BSPTree::buildRooms(BSPNode* node) {
    if (node->isLeaf()) {
        createRoom(node);
        return;
    }
    buildRooms(node->left.get());
    buildRooms(node->right.get());
    connectRooms(node->left.get(), node->right.get());
}

// Conecta los centros de dos habitaciones con un pasillo en L
void BSPTree::connectRooms(BSPNode* left, BSPNode* right) {
    Rect* a = findRoom(left);
    Rect* b = findRoom(right);
    if (!a || !b) return;
    // Calculamos los centros de las habitaciones
    std::pair<int,int> ca = { a->x + a->w / 2, a->y + a->h / 2 };
    std::pair<int,int> cb = { b->x + b->w / 2, b->y + b->h / 2 };
    // Creamos un pasillo en forma de L: primero horizontal, luego vertical (o viceversa)
    if (std::uniform_int_distribution<int>(0, 1)(rng) == 0) {
        // Horizontal primero
        corridors.push_back({ ca, { cb.first, ca.second } });
        corridors.push_back({ { cb.first, ca.second }, cb });
    } else {
        // Vertical primero
        corridors.push_back({ ca, { ca.first, cb.second } });
        corridors.push_back({ { ca.first, cb.second }, cb });
    }
}

// Busca cualquier habitación dentro del subárbol (preferentemente en hojas)
Rect* BSPTree::findRoom(BSPNode* node) {
    if (node->room) return node->room;                      // Si el nodo tiene habitación, la devuelve
    Rect* r = nullptr;                                      // Si no, busca recursivamente en los hijos
    if (node->left)  r = findRoom(node->left.get());        // Si no encuentra en el izquierdo, busca en el derecho
    if (!r && node->right) r = findRoom(node->right.get()); // Devuelve la primera habitación encontrada
    return r;   // Si no encuentra ninguna, devuelve nullptr
}

std::vector<Rect> BSPTree::getRooms() const {
    // Recorre el árbol y recoge todas las habitaciones en un vector
    std::vector<Rect> rooms;
    std::function<void(BSPNode*)> collect = [&](BSPNode* node) {
        if (!node) return;
        if (node->room) rooms.push_back(*node->room);
        collect(node->left.get());
        collect(node->right.get());
    };
    collect(root.get());
    return rooms;
}

BSPTree::Corridors BSPTree::getCorridors() const {
    return corridors;
}