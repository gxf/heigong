#ifndef LAYOUT_MANAGER_H
#define LAYOUT_MANAGER_H

#include "Common.h"

class Logger;
class FontsManager;

class LayoutManager{
    public:
        LayoutManager(int w, int h, int m_v, int m_h, Logger* log,
                      int ls = 2, int ws = 2);
        ~LayoutManager();

    public:
    public:
        enum GLYTH_TYPE{
            GT_CHAR,
            GT_BITMAP,
        };

        // Interfaces
        const Position GetProperPos(GLYTH_TYPE tp, int width, int height);

    public:
        inline const int GetWidth(){
            return p_width;
        }

        inline const int GetHeight(){
            return p_height;
        }

        inline void SetWidth(const int wd){
            p_width = wd;
        }

        inline void SetHeight(const int ht){
            p_height = ht;
        }
    private:
        void GetCharPos(Position & pos, int width, int height);
            
        void GetImagePos(Position & pos, int width, int height);

    private:
        //
        int p_width;        // Page width
        int p_height;       // Page height
        int v_m_width;      // Vertical Margin width
        int h_m_width;      // Horizontal Margin width
        int g_line_spacing; // Global line spacing
        int g_word_spacing; // Global word spacing

        Position curPos; // Current raster position

    private:
        Logger*     logger;
};

#endif
