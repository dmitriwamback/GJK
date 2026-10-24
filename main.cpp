#include <iostream>

#include "renderer/Renderer.h"

int main() {
    Renderer renderer = Renderer();
    renderer.Initialize();
    renderer.GameLoop();
}
