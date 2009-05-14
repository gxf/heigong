#include "Color.h"
#include "Logger.h"
#include "RenderMan.h"
#include "LayoutManager.h"
#include "FontsManager.h"
#include "MayTwelfth.h"

int main()
{
    Logger logger;
    RenderMan rend(&logger, 640, 480);

    Color col(255, 255, 0);

    rend.Init();
    rend.RenderPoint(50, 50, 10, col);
    rend.RenderLine(0, 0, 2, 30, col);
    rend.RenderLine2(0, 0, 75, 100, 3, col);
    rend.RenderRectangle(50, 50, 100, 150, col);
//    rend.RenderBitMap(50, 50, 100, 150, NULL);
    rend.RenderPixMap(50, 50, 100, 150, NULL);
    rend.Flush();
    rend.Quit();

    May12th may(&logger);
//    may.RenderString("Hello world.");
    may.RenderString("H");
    while(1){
    }
    return 0;
}
