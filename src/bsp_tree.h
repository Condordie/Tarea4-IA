#pragma once
#include <vector>
#include <memory>
#include <random>

// Rectángulo genérico usado para hojas y habitaciones
struct Rect {
    int x, y, w, h;
    Rect(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}
};

// Nodo del árbol BSP. Puede tener hijos (partición) o una habitación (hoja)
struct BSPNode {
    Rect region;
    std::unique_ptr<BSPNode> left;
    std::unique_ptr<BSPNode> right;
    Rect* room = nullptr;

    explicit BSPNode(Rect r) : region(r) {}
    ~BSPNode() { delete room; }

    bool isLeaf() const { return left == nullptr && right == nullptr; }
};

// Parámetros del generador, todos modificables desde main
struct BSPConfig {
    int mapWidth      = 80;
    int mapHeight     = 40;
    int minLeafSize   = 10;  // tamaño mínimo de una hoja para seguir dividiendo
    int maxDepth      = 6;   // profundidad máxima de recursión
    float splitMin    = 0.3f; // proporción mínima del corte
    float splitMax    = 0.7f; // proporción máxima del corte
    float roomMinRatio = 0.45f; // fracción mínima de la hoja que ocupa la habitación
    float roomMaxRatio = 0.75f; // fracción máxima
    unsigned int seed = 42;
};

class BSPTree {
public:
    explicit BSPTree(const BSPConfig& config);

    // Construye el árbol y genera habitaciones y pasillos
    void generate();

    // Devuelve todos los rectángulos de habitaciones generados
    std::vector<Rect> getRooms() const;

    // Devuelve los pares de puntos centrales que deben conectarse con pasillo
    std::vector<std::pair<std::pair<int,int>, std::pair<int,int>>> getCorridors() const;

    // Activa la visualización paso a paso de las particiones BSP
    void setAnimate(bool value);

    // Tiempo que tomó la última llamada a generate(), en milisegundos
    double getLastGenerationTimeMs() const;

private:
    BSPConfig cfg;
    std::mt19937 rng;
    std::unique_ptr<BSPNode> root;
    bool animate = false;
    double lastGenerationTimeMs = 0.0;

    // Dibuja las regiones actuales para mostrar el proceso de particionado
    void renderPartitions() const;
    void collectLeafRegions(BSPNode* node, std::vector<Rect>& leaves) const;

    using Corridors = std::vector<std::pair<std::pair<int,int>, std::pair<int,int>>>;
    Corridors corridors;

    // Divide recursivamente un nodo hasta alcanzar maxDepth o minLeafSize
    void split(BSPNode* node, int depth);

    // Coloca una habitación aleatoria dentro de la región de la hoja
    void createRoom(BSPNode* node);

    // Recorre el árbol, crea habitaciones en hojas y pasillos entre hermanos
    void buildRooms(BSPNode* node);

    // Conecta los centros de las habitaciones de los subárboles izquierdo y derecho
    void connectRooms(BSPNode* left, BSPNode* right);

    // Devuelve cualquier habitación dentro del subárbol (para conectar pasillos)
    Rect* findRoom(BSPNode* node);
};