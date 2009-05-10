#include "Color.h"
#include "Logger.h"
#include "RenderMan.h"

int main()
{
    Logger logger;
    RenderMan rend(&logger);

    Color col;

    rend.RenderPoint(0, 0, 1, col);
    rend.RenderLine(0, 0, 2, 30, col);
    rend.RenderLine2(0, 0, 75, 100, 3, col);
    rend.RenderRectangle(50, 50, 100, 150, col);

    return 0;
}
