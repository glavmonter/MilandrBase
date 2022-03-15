/**
  * \file    version.h
  * \version V2.0.0
  * \brief Версия программного обеспечения
  */

#pragma once


#define APP_VER_MAJOR 2         ///< Major
#define APP_VER_MINOR 0         ///< Minor
#define APP_VER_PATCH 0         ///< Patch

/// Числовое представление версии
#define APP_SW_VERSION (APP_VER_MAJOR * 10000 + APP_VER_MINOR * 100 + APP_VER_PATCH)
