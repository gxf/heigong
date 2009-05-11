#ifndef WIN_MANAGER_H
#define WIN_MANAGER_H

class WinManager{
    public:
        WinManager(int w, int h);
        ~WinManager();

    public:
        inline const int GetWidth(){
            return width;
        }

        inline const int GetHeight(){
            return height;
        }

    private:
        int     width;
        int     height;
        int     margin_v;   // Vertical Margin
        int     margin_h;   // Horizontal Margin
};

#endif
