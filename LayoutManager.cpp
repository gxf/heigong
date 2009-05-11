#include "Logger.h"
#include "LayoutManager.h"
#include "FontsManager.h"

LayoutManager::LayoutManager(int w, int h, int m_v, int m_h, Logger* log);
    width(w), height(h), margin_v(m_v), margin_h(m_h), 
    logger(log), fonts(log)
{
}

LayoutManager::~LayoutManager(){}
