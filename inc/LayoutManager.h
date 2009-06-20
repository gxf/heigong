#ifndef LAYOUT_MANAGER_H
#define LAYOUT_MANAGER_H

#include "Common.h"

class Glyph;
class Logger;
class FontsManager;
class RenderMan;

enum LAYOUT_RET{
    LO_OK,
    LO_NEW_LINE,
    LO_NEW_PAGE,
};

class LayoutManager{
    public:
        LayoutManager(int w, int h, int m_v, int m_h, 
                      Logger* log, int ls = 2, int ws = 2):
            p_width(w), p_height(h), v_m_width(m_v), h_m_width(m_h), 
            g_line_spacing(ls),g_word_spacing(ws),
            curPos(m_v, m_h), 
            curMaxHeight(0), lastMaxHeight(0),
            curBaseline(0), lastBaseline(0),
            firstLine(true), logger(log)
        {}
        virtual ~LayoutManager(){}

    public:
        // Accessor
        inline const int GetWidth(){ return p_width; }
        inline void SetWidth(const int wd){ p_width = wd; }

        inline const int GetHeight(){ return p_height; }
        inline void SetHeight(const int ht){ p_height = ht; }

        inline const int GetLineSpacing(){ return g_line_spacing; }
        inline void SetLineSpacing(const int ls){ g_line_spacing = ls; }

        inline const int GetWordSpacing(){ return g_word_spacing; }
        inline void SetWordSpacing(const int ws){ g_word_spacing = ws; }

        inline int GetCurBaseLine(){ return curBaseline; }
        inline int GetLastBaseLine(){ return lastBaseline; }

    public:
        virtual void AddGlyph(Glyph* g) = 0;
        virtual LAYOUT_RET GetCharPos(Position & pos, int width, int height, int bearingY) = 0;
        virtual LAYOUT_RET GetGraphPos(Position & pos, int width, int height) = 0;
        virtual LAYOUT_RET NewLine() = 0;
        virtual void NewPage() = 0;
        virtual void Reset() = 0;

    public:
#if 0
        LayoutManager & operator=(LayoutManger &lo){
            if (this == &lo){
                return *this;
            }
            this->p_width   = lo.p_width;
            this->p_height  = lo.p_height;
            this->v_m_width = lo.v_m_width;
            this->h_m_width = lo.h_m_width;
            this->g_line_spacing = lo.g_line_spacing;
            this->g_word_spacing = lo.g_word_spacing;
            this->curPos    = lo.curPos;
            this->curMaxHeight = lo.curMaxHeight;
            this->lastMaxHeight = lo.lastMaxHeight;
            this->curBaseline = lo.curBaseline;
            this->lastBaseline = lo.lastBaseline;
            this->firstLine = lo.firstLine; 
            this->logger = lo.logger;
        }
#endif

    protected:
        // Configuration
        int p_width;        // Page width
        int p_height;       // Page height
        int v_m_width;      // Vertical Margin width
        int h_m_width;      // Horizontal Margin width
        int g_line_spacing; // Global line spacing
        int g_word_spacing; // Global word spacing

    protected:
        // Status
        Position curPos;         // Current raster position
        int      curMaxHeight;   // Max height in one line
        int      lastMaxHeight;  // Max height in last line
        int      curBaseline;    // Current baseline position (y-axis)
        int      lastBaseline;   // Baseline of last line

        bool     firstLine;

    protected:
        Logger *    logger;
};

#endif
