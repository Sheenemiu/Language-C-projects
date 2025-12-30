#include <graphics.h>
#include <stdio.h>

int main(void)
{ 
    initwindow(640, 480, "ASSIGNMENT");

    setcolor(BLUE);

    setlinestyle(SOLID_LINE, 0, 10);
    line(85, 350, 355, 30);

    setlinestyle(SOLID_LINE, 0, 2);
    line(325, 350, 605, 30);

    setcolor(MAGENTA);

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);

    outtextxy(230, 300, "S");
    outtextxy(250, 275, "A");
    outtextxy(270, 250, "R");
    outtextxy(290, 225, "A");
    outtextxy(310, 200, "H");
    outtextxy(330, 175, " ");
    outtextxy(350, 150, "C");
    outtextxy(370, 125, "O");
    outtextxy(390, 100, "N");
    outtextxy(410, 75, "C");
    outtextxy(430, 50, "O");
    outtextxy(450, 25, "N");

    getch();
    closegraph();

    return 0;
}