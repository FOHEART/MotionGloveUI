#pragma once

#include <QtCore/qglobal.h>

#ifndef LIBH2UI_API
  #ifdef LIBH2UI_EXPORTS

    #ifdef _WIN32
    #define LIBH2UI_API __declspec(dllexport)
    #else
    #define LIBH2UI_API 
    #endif

  #else	

    #ifdef LIBH2UI_STATIC_LIB
      #define LIBH2UI_API
    #elif defined(_WIN32)
      #define LIBH2UI_API __declspec(dllimport)
    #else
      #define LIBH2UI_API
    #endif

  #endif	

#endif