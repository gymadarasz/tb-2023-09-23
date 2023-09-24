#include "Graphics.h"

int main()
{
    Graphics graph;
    graph.createWindow();
    while (!graph.eventLoop());
    graph.closeWindow();

    return 0;
}
