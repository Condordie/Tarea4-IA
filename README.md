# Tarea 4 — Generación Procedimental con BSP
**Curso:** Inteligencia Artificial para Videojuegos  
**Entrega:** 3 de julio  
**Equipo:** 2 personas  
**Algoritmo elegido:** Binary Space Partitioning (BSP)

---

## Índice

1. [Dónde buscar papers gratis](#1-dónde-buscar-papers-gratis)
2. [Cómo elegir el juego](#2-cómo-elegir-el-juego)
3. [Juegos candidatos simples (recomendados)](#3-juegos-candidatos-simples-recomendados)
4. [Resumen de BSP aplicado a videojuegos](#4-resumen-de-bsp-aplicado-a-videojuegos)
5. [Estructura del informe LaTeX](#5-estructura-del-informe-latex)
6. [División de trabajo sugerida](#6-división-de-trabajo-sugerida)
7. [Convenciones de código C++](#7-convenciones-de-código-c)
8. [Estructura de carpetas del repositorio](#8-estructura-de-carpetas-del-repositorio)

---

## 1. Dónde buscar papers gratis

Todas las fuentes de abajo son **100% gratuitas y accesibles sin VPN**.

| Fuente | URL | Qué encontrar |
|---|---|---|
| **Google Scholar** | scholar.google.com | Punto de partida; busca el PDF gratis con el botón "Todas las versiones" |
| **Semantic Scholar** | semanticscholar.org | Igual que Scholar pero con resúmenes de IA, muy útil para filtrar rápido |
| **arXiv** | arxiv.org | Preprints de IA y computación gráfica, siempre gratis |
| **ACM Digital Library** | dl.acm.org | Muchos papers de GDC y conferencias de juegos. Busca los que digan "Open Access" |
| **IEEE Xplore** | ieeexplore.ieee.org | Filtra por "Open Access Only" para no toparte con paywalls |
| **ResearchGate** | researchgate.net | Autores suben sus propios papers; puedes pedir el PDF directamente al autor |
| **Proceedings of the AAAI AIIDE** | aaai.org/library/workshops | Conferencia específica de IA en videojuegos, muchos papers de PCG |
| **PCG Book (gratis online)** | pcgbook.com | Libro completo de Procedural Content Generation in Games, capítulo de BSP incluido |

### Términos de búsqueda útiles

Copia y pega estos en Google Scholar o Semantic Scholar:

```
"binary space partitioning" dungeon generation
"BSP tree" procedural level generation videogame
"procedural dungeon generation" roguelike
"procedural content generation" indoor map 2D
BSP room generation algorithm game
```

> **Tip:** Filtra siempre por años **2020–2025** para cumplir el requisito del informe de usar fuentes recientes. Si necesitas una fuente más antigua (el paper original de BSP es de 1969), justifica en el informe que sigue siendo el fundamento teórico y no ha sido reemplazado.

---

## 2. Cómo elegir el juego

El profesor tiene razón: **primero el juego, después confirmar que el algoritmo encaja**. Sigue este criterio:

### Checklist para validar un juego candidato

- [ ] Tiene **niveles o mapas generados** (no niveles diseñados a mano únicamente)
- [ ] Los espacios son **interiores o en cuadrícula** (BSP funciona mejor así)
- [ ] La implementación es **factible en C++** sin motor gráfico complejo (solo output en consola o ASCII ya cuenta)
- [ ] Existe **bibliografía disponible** que mencione el juego o uno similar
- [ ] El agente (NPC o jugador) puede **navegar por el mapa generado** sin una física compleja

### Señales de que el juego es demasiado complejo para implementar

- Requiere física 3D
- Los niveles tienen reglas de gameplay muy específicas que el generador debe respetar (ej. puzzles con solución única)
- No existe documentación técnica ni papers que lo mencionen

---

## 3. Juegos candidatos simples (recomendados)

Estos son los más fáciles de implementar y tienen buena bibliografía de PCG:

### Opción A — Roguelike tipo Rogue / NetHack ⭐ Recomendado
- **Qué se genera:** Mazmorras 2D con habitaciones y pasillos
- **Por qué BSP encaja perfecto:** BSP divide el espacio en particiones, cada hoja del árbol se convierte en una habitación, los nodos internos conectan habitaciones con pasillos. Es exactamente el caso de uso clásico descrito en la literatura.
- **Qué implementar en C++:** Generar el árbol BSP, crear habitaciones en cada hoja, conectar con pasillos, imprimir el mapa en consola (ASCII). El agente puede ser un pathfinding básico (BFS/A*) que navega el mapa generado.
- **Referencias clave:** Busca "BSP dungeon generation" en el PCG Book (pcgbook.com, capítulo 3).

### Opción B — The Binding of Isaac (simplificado)
- **Qué se genera:** Layout de habitaciones conectadas (grafo de cuartos)
- **Por qué BSP encaja:** Se puede usar BSP para partir el espacio de la planta y asignar cuartos a las particiones
- **Qué implementar:** Más complejo que la opción A porque hay tipos de habitaciones (jefe, tienda, etc.). Solo recomendado si quieren un tema más interesante y tienen tiempo.

### Opción C — Dungeon Crawl Stone Soup (solo el generador de mapas)
- **Qué se genera:** Mapas de mazmorra con distintas zonas
- **Nota:** Tiene código abierto en GitHub, lo que facilita entender cómo lo hacen ellos y comparar con tu implementación en el informe.

### ¿Cuál elegir?

Si la prioridad es **entregar a tiempo con buena nota**, elige la **Opción A (Roguelike básico)**. Es el caso canónico de BSP, tiene bibliografía abundante, y la implementación en C++ es directa.

---

## 4. Resumen de BSP aplicado a videojuegos

> Esta sección es una guía rápida para alinear al equipo. No reemplaza la investigación bibliográfica.

### Idea central

BSP (Binary Space Partitioning) divide recursivamente un espacio en dos subespacios usando un plano de corte (en 2D, una línea horizontal o vertical). El resultado es un árbol binario donde:

- Cada **nodo interno** representa una región particionada
- Cada **hoja** representa una región final donde se puede colocar una habitación

### Proceso típico para generación de mazmorras

```
1. Empezar con un rectángulo que representa el mapa completo
2. Dividirlo aleatoriamente en dos sub-rectángulos (horizontal o vertical)
3. Repetir recursivamente hasta que los sub-rectángulos sean demasiado pequeños
4. En cada hoja: generar una habitación de tamaño aleatorio dentro del sub-rectángulo
5. Al regresar en el árbol: conectar las habitaciones de los dos hijos con un pasillo
```

### Parámetros clave a exponer (para la sección Metodología del informe)

| Parámetro | Descripción | Valor típico |
|---|---|---|
| `MIN_LEAF_SIZE` | Tamaño mínimo de una hoja para seguir dividiendo | 10x10 tiles |
| `MAX_DEPTH` | Profundidad máxima del árbol | 5–7 niveles |
| `ROOM_MIN_RATIO` | Tamaño mínimo de habitación como % de la hoja | 0.45 |
| `ROOM_MAX_RATIO` | Tamaño máximo de habitación como % de la hoja | 0.75 |
| `SPLIT_RATIO_MIN/MAX` | Rango para el punto de corte (evita particiones muy desiguales) | 0.3 – 0.7 |

Estos son los parámetros que el informe pide justificar (sección Metodología). Deben probar distintos valores y mostrar los mapas resultantes en la sección Resultados.

---

## 5. Estructura del informe LaTeX

El informe debe seguir exactamente esta estructura (según el enunciado):

```
Abstract          ← escribir al final
Introducción      ← juego + algoritmo + por qué encajan
Estado del Arte   ← papers, otros juegos similares, técnicas alternativas
Metodología       ← implementación, parámetros, proceso de búsqueda de params
Resultados        ← mapas generados con distintos parámetros (capturas o ASCII)
Discusión         ← opiniones, hipótesis, conclusiones razonadas
Conclusiones      ← resumen de resultados y discusión
Referencias       ← todas las citas, formato consistente (BibTeX recomendado)
```

### Notas importantes

- **No usar IA para redactar.** Sí se puede usar para corrección ortográfica/gramatical.
- Usar **BibTeX** para las referencias. Semantic Scholar exporta BibTeX directo.
- Las referencias deben ser **mayoritariamente de los últimos 5 años**. Si usan fuentes más antiguas (ej. el paper original de BSP), justifiquen en el texto que siguen siendo el fundamento vigente.
- Los **mapas generados** van en Resultados. Si la salida es ASCII, pueden incluirla en un bloque `\verbatim` en LaTeX.

---

## 6. División de trabajo sugerida

> Ajústenla según sus fortalezas. Esto es solo una propuesta.

### Persona 1 — Implementación C++
- [ ] Estructura del árbol BSP (`BSPNode`, `BSPTree`)
- [ ] Función de división recursiva
- [ ] Generación de habitaciones en hojas
- [ ] Conexión de habitaciones con pasillos
- [ ] Main + Makefile
- [ ] Integración del agente (pathfinding básico si aplica)

### Persona 2 — Investigación e Informe
- [ ] Buscar y seleccionar 5–8 papers relevantes (ver sección 1)
- [ ] Redactar Introducción y Estado del Arte
- [ ] Documentar los experimentos de parámetros (con Persona 1)
- [ ] Redactar Resultados, Discusión y Conclusiones
- [ ] Armar el documento LaTeX con BibTeX

### Trabajo conjunto
- [ ] Elegir el juego final (usar checklist de sección 2)
- [ ] Definir los parámetros a explorar y correr experimentos
- [ ] Revisión cruzada del código y del informe antes de entregar

---

## 7. Convenciones de código C++

Para que el código sea consistente entre los dos:

```cpp
// Nombres de clases: PascalCase
class BSPNode { ... };

// Nombres de funciones y variables: camelCase
void splitNode(BSPNode* node, int depth);
int minLeafSize = 10;

// Constantes: UPPER_SNAKE_CASE
const int MAX_DEPTH = 6;
const float SPLIT_RATIO_MIN = 0.3f;

// Archivos: snake_case
// bsp_tree.h, bsp_tree.cpp, map_renderer.h, main.cpp
```

### Archivos mínimos requeridos por el enunciado

```
main.cpp       ← función main obligatoria
Makefile       ← compilación
README.md      ← este archivo (instrucciones de ejecución)
```

### Ejemplo de Makefile básico

```makefile
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
TARGET = bsp_dungeon
SRCS = main.cpp bsp_tree.cpp map_renderer.cpp

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)
```

---

## 8. Estructura de carpetas del repositorio

```
/
├── README.md              ← este archivo
├── Makefile
├── src/
│   ├── main.cpp
│   ├── bsp_tree.h
│   ├── bsp_tree.cpp
│   ├── map_renderer.h
│   └── map_renderer.cpp
├── informe/
│   ├── main.tex
│   ├── referencias.bib
│   └── figuras/           ← capturas de mapas generados para Resultados
└── resultados/
    └── ejemplos/          ← salidas de texto de mapas con distintos parámetros
```

---

## Checklist final antes de entregar

- [ ] El código **compila y corre** sin errores con `make`
- [ ] El mapa se puede ver (consola, ASCII, o imagen)
- [ ] El informe tiene **todas las secciones** del enunciado
- [ ] Las referencias son accesibles y tienen formato consistente
- [ ] Los parámetros del algoritmo están **justificados** en Metodología
- [ ] Los mapas en Resultados tienen su descripción de parámetros usados
- [ ] El README explica cómo compilar y ejecutar

---

*Última actualización: junio 2025*
