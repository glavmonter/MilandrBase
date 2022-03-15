# Набор программ для тестирования SSP и I2C микроконтроллера 1986ВЕ92QI

# Сборка

1. Скопировать всё из [fmt](https://github.com/fmtlib/fmt/tree/8.0.1) в fmt
2. Установка либ FTDI:
   1. Распаковать куда-нибудь [LibFT4222-v1.4.4.zip](https://www.ftdichip.com/Support/SoftwareExamples/LibFT4222-v1.4.4.zip)
   2. Скопировать всё из imports в ftdi в проект
3. Сохранить [cxxopts.hpp](https://github.com/jarro2783/cxxopts/blob/v2.2.1/include/cxxopts.hpp) в include
4. cmake .. 
5. make
