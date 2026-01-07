# -------------------------------------------------
#   Makefile для сборки проекта под MSP430
# -------------------------------------------------

# Целевой микроконтроллер
MCU = msp430g2553          # <-- поменяйте при необходимости

# Инструменты
CXX   = msp430-elf-g++
OBJCOPY = msp430-elf-objcopy
SIZE   = msp430-elf-size

# Флаги компиляции
CXXFLAGS = -mmcu=$(MCU) -Os -Wall -Wextra -std=c++17 \
           -ffunction-sections -fdata-sections \
           -I /home/brave/ti/msp430-gcc/include 

# Флаги линковки (удаляем неиспользуемый код)
LDFLAGS  = -mmcu=$(MCU) \
           -Wl,--gc-sections -L /home/brave/ti/msp430-gcc/include

# Исходники и объектные файлы
SRCS  = main.cpp TM1638.cpp
OBJS  = $(SRCS:.cpp=.o)

# Итоговый файл прошивки (Intel HEX)
TARGET = firmware.hex

# -------------------------------------------------
all: $(TARGET)

# Компиляция .cpp → .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Линковка и генерация HEX‑файла
$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o firmware.elf
	$(OBJCOPY) -O ihex firmware.elf $(TARGET)
	$(SIZE) firmware.elf

clean:
	rm -f *.o *.elf $(TARGET)

.PHONY: all clean