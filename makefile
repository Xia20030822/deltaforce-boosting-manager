
# CC = arm-linux-gcc

# # libjpeg 的 ARM include 和 lib 路径
# JPEG_INCLUDE = /opt/arm-toolchain/arm-linux/sysroot/usr/include
# JPEG_LIB     = /opt/arm-toolchain/arm-linux/sysroot/usr/lib

# CFLAGS = -I./include  -I$(JPEG_INCLUDE) -O2
# LDFLAGS = -L$(JPEG_LIB) -ljpeg -L./lib -lfont

# # 源文件
# SRC = main.c $(wildcard src/*.c)

# # 对应的目标文件
# OBJ = $(SRC:.c=.o)

# # 最终生成程序
# TARGET = deltaforce_manager

# # 默认目标
# all: $(TARGET)

# # 链接生成可执行文件
# $(TARGET): $(OBJ)
# 	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# # 编译规则
# %.o: %.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# # 清理
# clean:
# 	rm -f $(OBJ) $(TARGET)
//todo:完善这个makefile

CC = arm-linux-gcc

# libjpeg 的 ARM include 和 lib 路径
JPEG_INCLUDE = /opt/arm-toolchain/arm-linux/sysroot/usr/include
JPEG_LIB = /opt/arm-toolchain/arm-linux/sysroot/usr/lib

# giflib 的 ARM include 和 lib 路径
GIF_INCLUDE = /home/a/arm-gif/include
GIF_LIB = /home/a/arm-gif/lib

CFLAGS = -I./include -I$(JPEG_INCLUDE) -I$(GIF_INCLUDE) -O2
LDFLAGS = -L$(JPEG_LIB) -ljpeg -L./lib -lfont -L$(GIF_LIB) -lgif -static

# 源文件
SRC = main.c $(wildcard src/*.c)

# 对应的目标文件
OBJ = $(SRC:.c=.o)

# 最终生成程序
TARGET = deltaforce_manager

# 默认目标
all: $(TARGET)

# 链接生成可执行文件
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# 编译规则
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理
clean:
	rm -f $(OBJ) $(TARGET)
