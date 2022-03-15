/**
  * \file    app_config.h
  * \brief   Опции тестов и проверок
  *
  * Задаются конфиги приложения
  *
  */

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#ifndef CONFIG_LOG_DEFAULT_LEVEL
    #define CONFIG_LOG_DEFAULT_LEVEL MDR_LOG_VERBOSE      ///< 0 - _NONE, 1 - _ERROR, 2 - _WARN, 3 - _INFO, 4 - _DEBUG, 5 - _VERBOSE. Уровень логгирования по-умолчанию
#endif


#ifndef CONFIG_LOG_MAXIMUM_LEVEL
    #define CONFIG_LOG_MAXIMUM_LEVEL MDR_LOG_VERBOSE        ///< Максимально возможный уровень логирования, который можно менять программно через mdr_log_level_set
#endif

#ifndef CONFIG_LOG_COLORS
    #define CONFIG_LOG_COLORS 1
#endif

#ifndef CONFIG_LOG_TIMESTAMP_SOURCE_RTOS
    #define CONFIG_LOG_TIMESTAMP_SOURCE_RTOS 1
#endif


#ifndef VERSION_HW
//#error "VERSION_HW must be defined"
#endif

#endif /* APP_CONFIG_H_ */
