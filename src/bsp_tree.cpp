#include "bsp_tree.h"
#include <stdexcept>
#include <functional>

BSPTree::BSPTree(const BSPConfig& config)
    : cfg(config), rng(config.seed)
{
    root = std::make_unique<BSPNode>(Rect(0, 0, cfg.mapWidth, cfg.mapHeight));
}

void BSPTree::generate() {
    split(root.get(), 0);
    buildRooms(root.get());
}

// Divide el nodo en dos hijos eligiendo un eje (horizontal o vertical) al azar
void BSPTree::split(BSPNode* node, int depth) {
    if (depth >= cfg.maxDepth) return;

    Rect& r = node->region;
    bool canSplitH = r.h >= cfg.minLeafSize * 2;
    bool canSplitV = r.w >= cfg.minLeafSize * 2;
    if (!canSplitH && !canSplitV) return;

    // Si solo un eje es posible, lo usamos; si ambos, elegimos al azar
    bool splitHorizontal;
    if (canSplitH && canSplitV) {
        std::uniform_int_distribution<int> coin(0, 1);
        splitHorizontal = coin(rng);
    } else {
        splitHorizontal = canSplitH;
    }

    std::uniform_real_distribution<float> ratio(cfg.splitMin, cfg.splitMax);

    if (splitHorizontal) {
        int cut = static_cast<int>(r.h * ratio(rng));
        node->left  = std::make_unique<BSPNode>(Rect(r.x, r.y,       r.w, cut));
        node->right = std::make_unique<BSPNode>(Rect(r.x, r.y + cut, r.w, r.h - cut));
    } else {
        int cut = static_cast<int>(r.w * ratio(rng));
        node->left  = std::make_unique<BSPNode>(Rect(r.x,       r.y, cut,       r.h));
        node->right = std::make_unique<BSPNode>(Rect(r.x + cut, r.y, r.w - cut, r.h));
    }

    split(node->left.get(),  depth + 1);
    split(node->right.get(), depth + 1);
}

// Coloca una habitación dentro de la región de la hoja con tamaño aleatorio
void BSPTree::createRoom(BSPNode* node) {
    Rect& r = node->region;

    std::uniform_real_distribution<float> rf(cfg.roomMinRatio, cfg.roomMaxRatio);
    int rw = std::max(3, static_cast<int>(r.w * rf(rng)));
    int rh = std::max(3, static_cast<int>(r.h * rf(rng)));

    // Posición aleatoria dentro de la región para que no siempre esté en la esquina
    std::uniform_int_distribution<int> ox(0, r.w - rw);
    std::uniform_int_distribution<int> oy(0, r.h - rh);

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

    std::pair<int,int> ca = { a->x + a->w / 2, a->y + a->h / 2 };
    std::pair<int,int> cb = { b->x + b->w / 2, b->y + b->h / 2 };

    corridors.push_back({ ca, cb });
}

// Busca cualquier habitación dentro del subárbol (preferentemente en hojas)
Rect* BSPTree::findRoom(BSPNode* node) {
    if (node->room) return node->room;
    Rect* r = nullptr;
    if (node->left)  r = findRoom(node->left.get());
    if (!r && node->right) r = findRoom(node->right.get());
    return r;
}

std::vector<Rect> BSPTree::getRooms() const {
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