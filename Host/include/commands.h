#pragma once
#include <cstdint>


/// Объявления НЧ драйвера
namespace lfc {
    /**
     * Номера каналов ЦАП для класса @ref LFSmart
     */
    enum Channel {
        CHANNEL_1,                  ///< Канал 1
        CHANNEL_2,                  ///< Канал 2
        CHANNEL_3,                  ///< Канал 3
        CHANNEL_4,                  ///< Канал 4
        CHANNEL_ALL,                ///< Каналы 1-4, все
        CHANNEL_INVALID             ///< Заглушка для неправильного канала
    };

    /**
     * Регистры НЧ драйвера
     */
    enum Registers {
        WHOIAM          = 0x00,     ///< RO 2 байта. Кто я такой, код устройства
        STATUS          = 0x01,     ///< RO 2 байта. Статус устройства. @ref status_flags "Описание полей"
        LAST_ERROR      = 0x02,     ///< RO 2 байта. Ошибка выполнения последней команды

        DAC_CH1         = 0x03,     ///< RW 2 байта. Канал 1 ЦАП, текущее значение
        DAC_CH2         = 0x04,     ///< RW 2 байта. Канал 2 ЦАП, текущее значение
        DAC_CH3         = 0x05,     ///< RW 2 байта. Канал 3 ЦАП, текущее значение
        DAC_CH4         = 0x06,     ///< RW 2 байта. Канал 4 ЦАП, текущее значение
        DAC_ALL         = 0x07,     ///< RW 2 байта. Все каналы ЦАП, текущее значение

        ADC_CH1         = 0x08,     ///< RO 2 байта. Канал 1 АЦП
        ADC_CH2         = 0x09,     ///< RO 2 байта. Канал 2 АЦП
        ADC_CH3         = 0x0A,     ///< RO 2 байта. Канал 3 АЦП
        ADC_CH4         = 0x0B,     ///< RO 2 байта. Канал 4 АЦП
        ADC_ALL         = 0x0C,     ///< RO 8 байта. Все каналы АЦП

        DAC_DEFAULT_CH1 = 0x0D,     ///< RW 2 байта. Канал 1 ЦАП, значение после включения, сброса
        DAC_DEFAULT_CH2 = 0x0E,     ///< RW 2 байта. Канал 2 ЦАП, значение после включения, сброса
        DAC_DEFAULT_CH3 = 0x0F,     ///< RW 2 байта. Канал 3 ЦАП, значение после включения, сброса
        DAC_DEFAULT_CH4 = 0x10,     ///< RW 2 байта. Канал 4 ЦАП, значение после включения, сброса

        DAC_MAX_CH1     = 0x11,     ///< RW 2 байта. Канал 1 ЦАП, максимальное значение
        DAC_MAX_CH2     = 0x12,     ///< RW 2 байта. Канал 2 ЦАП, максимальное значение
        DAC_MAX_CH3     = 0x13,     ///< RW 2 байта. Канал 3 ЦАП, максимальное значение
        DAC_MAX_CH4     = 0x14,     ///< RW 2 байта. Канал 4 ЦАП, максимальное значение
        SAVE_EEP        = 0x15,     ///< WO 2 байта. Битовая карта сохранения настроек. @ref nv_flags "Описание полей"

        THRM_PCB        = 0x16,     ///< RO 2 байта. Температура термодатчика на печатной плате, К
        THRM_MCU        = 0x17,     ///< RO 2 байта. Температура микропроцессора, К
        SVC             = 0x18,     ///< WO 2 байта. Сервисная команда. @ref svc_flags "Описание полей"
        VERSION         = 0x19,     ///< RO 2 байта. Версия программного обеспечения
        CRC_HW          = 0x1A,     ///< RO 4 байта. Контрольная сумма, посчитанная аппаратно
        CRC_SW          = 0x1B,     ///< RO 4 байта. Контрольная сумма, посчитанная программно
        CERT            = 0x1C,

        RESTRICTED      = UINT8_MAX - 1,
        INVALID         = UINT8_MAX ///< Не верный регистр
    };

    /**
     * Доступ к регистру
     */
    enum Access {
        WRITE           = 0,        ///< Запись
        READ            = 1         ///< Чтение
    };

    /**
     * Ошибки после выполнения команд на запись/чтение регистров НЧ.
     * Ошибки вычитываются из регистра LAST_ERROR, при этом регистр обнуляется (LF_NOERROR)
     */
    enum LastError {
        LE_NOERROR = 0,             ///< Нет ошибок в последней выполненной команде
        LE_UNKNOWN_COMMAND,         ///< Получена неизвестная команда
        LE_ACCESS_ERROR,            ///< Доступ на запись в регистр Только-чтение
        LE_INVALID_STATE,           ///< Запись регистра DAC_CH* при включенном DDS
        LE_DAC_TRUNCATE,            ///< Записанное значение регистра DAC_CH* ограниченно значением DAC_MAX_CH*
        LE_DAC_OVERFLOW,            ///< Записанное значение регистра DAC_MAX_CH* меньше текущего значения DAC_CH*
        LE_EEPROM_ERROR,            ///< Ошибка сохранения регистров DAC_MAX_CH* или DAC_DEFAULT_CH*
        LE_CRC_ERROR,               ///< Принятый пакет данных с ошибкой CRC
        LE_INACTIVE,                ///< Команда управления, но устройство не активно
        LE_RESTRICTED,
    };
}


/**
 * @defgroup nv_flags Поля регистра SAVE_EEP
 * Использовать любую комбинацию флагов без строчных букв для записи в энергонезависимую память
 * @{
 */
#define NV_DAC_DEFAULT_CH1_Pos      (0U)
#define NV_DAC_DEFAULT_CH1_Msk      (0x1UL << NV_DAC_DEFAULT_CH1_Pos)
#define NV_DAC_DEFAULT_CH1          NV_DAC_DEFAULT_CH1_Msk     ///< Сохранить начальное значение канал 1 ЦАП (DAC_DEFAULT_CH1)

#define NV_DAC_DEFAULT_CH2_Pos      (1U)
#define NV_DAC_DEFAULT_CH2_Msk      (0x1UL << NV_DAC_DEFAULT_CH2_Pos)
#define NV_DAC_DEFAULT_CH2          NV_DAC_DEFAULT_CH2_Msk     ///< Сохранить начальное значение канал 2 ЦАП (DAC_DEFAULT_CH2)

#define NV_DAC_DEFAULT_CH3_Pos      (2U)
#define NV_DAC_DEFAULT_CH3_Msk      (0x1UL << NV_DAC_DEFAULT_CH3_Pos)
#define NV_DAC_DEFAULT_CH3          NV_DAC_DEFAULT_CH3_Msk     ///< Сохранить начальное значение канал 3 ЦАП (DAC_DEFAULT_CH3)

#define NV_DAC_DEFAULT_CH4_Pos      (3U)
#define NV_DAC_DEFAULT_CH4_Msk      (0x1UL << NV_DAC_DEFAULT_CH4_Pos)
#define NV_DAC_DEFAULT_CH4          NV_DAC_DEFAULT_CH4_Msk     ///< Сохранить начальное значение канал 4 ЦАП (DAC_DEFAULT_CH4)

#define NV_DAC_MAX_CH1_Pos          (4U)
#define NV_DAC_MAX_CH1_Msk          (0x1UL << NV_DAC_MAX_CH1_Pos)
#define NV_DAC_MAX_CH1              NV_DAC_MAX_CH1_Msk         ///< Сохранить максимальное значение канал 1 ЦАП (DAC_MAX_CH1)

#define NV_DAC_MAX_CH2_Pos          (5U)
#define NV_DAC_MAX_CH2_Msk          (0x1UL << NV_DAC_MAX_CH2_Pos)
#define NV_DAC_MAX_CH2              NV_DAC_MAX_CH2_Msk        ///< Сохранить максимальное значение канал 2 ЦАП (DAC_MAX_CH2)

#define NV_DAC_MAX_CH3_Pos          (6U)
#define NV_DAC_MAX_CH3_Msk          (0x1UL << NV_DAC_MAX_CH3_Pos)
#define NV_DAC_MAX_CH3              NV_DAC_MAX_CH3_Msk        ///< Сохранить максимальное значение канал 3 ЦАП (DAC_MAX_CH3)

#define NV_DAC_MAX_CH4_Pos          (7U)
#define NV_DAC_MAX_CH4_Msk          (0x1UL << NV_DAC_MAX_CH4_Pos)
#define NV_DAC_MAX_CH4              NV_DAC_MAX_CH4_Msk        ///< Сохранить максимальное значение канал 4 ЦАП (DAC_MAX_CH4)
/** @} */


/**
 * @defgroup svc_flags Поля регистра SVC
 * Биты управления НЧ драйвером
 * @{
 */
#define LF_SVC_RESET_Pos            (0U)
#define LF_SVC_RESET_Msk            (0x01UL << LF_SVC_RESET_Pos)
#define LF_SVC_RESET                LF_SVC_RESET_Msk          ///< 1 делает программный сброс НЧ драйвера и загрузку значения ЦАП из регистров DAC_DEFAULT_CH*

#define LF_SVC_ADC_EN_Pos           (3U)
#define LF_SVC_ADC_EN_Msk           (0x01UL << LF_SVC_ADC_EN_Pos)
#define LF_SVC_ADC_EN               LF_SVC_ADC_EN_Msk         ///< 1 включает измерительный АЦП, работает по-умолчанию

#define LF_SVC_ADC_DIS_Pos          (4U)
#define LF_SVC_ADC_DIS_Msk          (0x01UL << LF_SVC_ADC_DIS_Pos)
#define LF_SVC_ADC_DIS              LF_SVC_ADC_DIS_Msk        ///< 1 выключает измерительный АЦП

#define LF_SVC_START_Pos            (5U)
#define LS_SVC_START_Msk            (0x01UL << LF_SVC_START_Pos)
#define LS_SVC_START                LS_SVC_START_Msk          ///< 1 Включает нормальную работу НЧ драйвера

#define LF_SVC_STOP_Pos             (6U)
#define LF_SVC_STOP_Msk             (0x01UL << LF_SVC_STOP_Pos)
#define LF_SVC_STOP                 LF_SVC_STOP_Msk           ///< 1 Выключает НЧ драйвер
/** @} */


/**
 * @defgroup cert_flags Поля регистра CERT
 * Биты управления функциями самоконтроля
 * @{
 */
#define LF_CERT_START_Pos           (0U)
#define LF_CERT_START_Msk           (0x1UL << LF_CERT_START_Pos)
#define LF_CERT_START               LF_CERT_START_Msk

#define LF_CERT_NSD_Pos             (1U)
#define LF_CERT_NSD_Msk             (0x1UL << LF_CERT_NSD_Pos)
#define LF_CERT_NSD                 LF_CERT_NSD_Msk

#define LF_CERT_RECRC_Pos           (2U)
#define LF_CERT_RECRC_Msk           (0x1UL << LF_CERT_RECRC_Pos)
#define LF_CERT_RECRC               LF_CERT_RECRC_Msk

#define LF_CERT_POWERCK_Pos         (3U)
#define LF_CERT_POWERCK_Msk         (0x1UL << LF_CERT_POWERCK_Pos)
#define LF_CERT_POWERCK             LF_CERT_POWERCK_Msk

#define LF_CERT_RESET_Pos           (4U)
#define LF_CERT_RESET_Msk           (0x1UL << LF_CERT_RESET_Pos)
#define LF_CERT_RESET               LF_CERT_RESET_Msk
/** @} */


/**
 * @defgroup status_flags Поля регистра STATUS
 * Регистр только чтение. Устанавливаются любые комбинации бит
 * @{
 */
#define LF_STATUS_DDS_Pos           (0U)
#define LF_STATUS_DDS_Msk           (0x1UL << LF_STATUS_DDS_Pos)
#define LF_STATUS_DDS               LF_STATUS_DDS_Msk        ///< 1 - генерация синуса включена

#define LF_STATUS_PG_Pos            (1U)
#define LF_STATUS_PG_Msk            (0x1UL << LF_STATUS_PG_Pos)
#define LF_STATUS_PG                LF_STATUS_PG_Msk         ///< 1 - Внутренний источник питания +16 В в норме

#define LF_STATUS_INVALID_CH1_Pos   (2U)
#define LF_STATUS_INVALID_CH1_Msk   (0x1UL << LF_STATUS_INVALID_CH1_Pos)
#define LF_STATUS_INVALID_CH1       LF_STATUS_INVALID_CH1_Msk ///< 1 - Выходное напряжение канала 1 ЦАП отличается более чем на 2% от заданного

#define LF_STATUS_INVALID_CH2_Pos   (3U)
#define LF_STATUS_INVALID_CH2_Msk   (0x1UL << LF_STATUS_INVALID_CH2_Pos)
#define LF_STATUS_INVALID_CH2       LF_STATUS_INVALID_CH2_Msk ///< 1 - Выходное напряжение канала 2 ЦАП отличается более чем на 2% от заданного

#define LF_STATUS_INVALID_CH3_Pos   (4U)
#define LF_STATUS_INVALID_CH3_Msk   (0x1UL << LF_STATUS_INVALID_CH3_Pos)
#define LF_STATUS_INVALID_CH3       LF_STATUS_INVALID_CH3_Msk ///< 1 - Выходное напряжение канала 3 ЦАП отличается более чем на 2% от заданного

#define LF_STATUS_INVALID_CH4_Pos   (5U)
#define LF_STATUS_INVALID_CH4_Msk   (0x1UL << LF_STATUS_INVALID_CH4_Pos)
#define LF_STATUS_INVALID_CH4       LF_STATUS_INVALID_CH4_Msk ///< 1 - Выходное напряжение канала 4 ЦАП отличается более чем на 2% от заданного

#define LF_STATUS_ADC_ENABLED_Pos   (6U)
#define LF_STATUS_ADC_ENABLED_Msk   (0x1UL << LF_STATUS_ADC_ENABLED_Pos)
#define LF_STATUS_ADC_ENABLED       LF_STATUS_ADC_ENABLED_Msk ///< 1 - Работает измерительный АЦП

#define LF_STATUS_ASMTEST_Pos       (7U)
#define LF_STATUS_ASMTEST_Msk       (0x1UL << LF_STATUS_ASMTEST_Pos)
#define LF_STATUS_ASMTEST           LF_STATUS_ASMTEST_Msk     ///< 1 - Пройден тест на ассемблер

#define LF_STATUS_CRCTEST_Pos       (8U)
#define LF_STATUS_CRCTEST_Msk       (0x1UL << LF_STATUS_CRCTEST_Pos)
#define LF_STATUS_CRCTEST           LF_STATUS_CRCTEST_Msk     ///< 1 - Пройден тест на CRC32

#define LF_STATUS_ENABLED_Pos       (9U)
#define LF_STATUS_ENABLED_Msk       (0x1UL << LF_STATUS_ENABLED_Pos)
#define LF_STATUS_ENABLED           LF_STATUS_ENABLED_Msk     ///< 1 - Нормальная работа НЧ Драйвера

#define LF_STATUS_NSD_Pos           (10U)
#define LF_STATUS_NSD_Msk           (0x1UL << LF_STATUS_NSD_Pos)
#define LF_STATUS_NSD               LF_STATUS_NSD_Msk

#define LF_STATUS_CERT_CRC_RDY_Pos   (11U)
#define LF_STATUS_CERT_CRC_RDY_Msk   (0x1UL << LF_STATUS_CERT_CRC_RDY_Pos)
#define LF_STATUS_CERT_CRC_RDY       LF_STATUS_CERT_CRC_RDY_Msk

#define LF_STATUS_CERT_PWRCK_RDY_Pos (12U)
#define LF_STATUS_CERT_PWRCK_RDY_Msk (0x1UL << LF_STATUS_CERT_PWRCK_RDY_Pos)
#define LF_STATUS_CERT_PWRCK_RDY     LF_STATUS_CERT_PWRCK_RDY_Msk

#define LF_STATUS_CERT_PWRCK_RESULT_Pos (13U)
#define LF_STATUS_CERT_PWRCK_RESULT_Msk (0x1UL << LF_STATUS_CERT_PWRCK_RESULT_Pos)
#define LF_STATUS_CERT_PWRCK_RESULT  LF_STATUS_CERT_PWRCK_RESULT_Msk

#define LF_STATUS_INVALID_CH_ALL    (LF_STATUS_INVALID_CH1 | LF_STATUS_INVALID_CH2 | LF_STATUS_INVALID_CH3 | LF_STATUS_INVALID_CH4)
/** @} */
