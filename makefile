TARGET = HTTPserver
DEBUG=yes
#==================工作目录=================
ALL_DIR = $(shell find . -type d)
INC_DIRS =./include #头文件路径
LIB_DIRS = #链接库所在目录
#==================文件====================
C_SRCS = $(foreach file,$(ALL_DIR),$(wildcard $(file)/*.c))#需要编译的c文件
CPP_SRCS = $(foreach file,$(ALL_DIR),$(wildcard $(file)/*.cpp))#需要编译的cpp文件
OBJS = $(C_SRCS:.c=.o)
OBJS += $(CPP_SRCS:.cpp=.o)
DEPS = $(C_SRCS:.c=.d)
DEPS += $(CPP_SRCS:.cpp=.d)
LIBS=#链接库名称
#==================预定义=================
CC = gcc
CXX = g++
DEFINES = #需要定义的宏
#==================编译选项=================
CFLAGS = -pipe -Wall -Wextra -MMD -DNDEBUG -D_GNU_SOURCE
ifeq ($(DEBUG),yes)
CFLAGS += -g
else
CFLAGS += -O3 -s
endif

CXXFLAGS = $(addprefix -I ,$(INC_DIRS)) $(addprefix -D ,$(DEFINES))
EXTRA_CXXFLAGS = $(CFLAGS)#其他编译选项

LDFLAGS = $(addprefix -L ,$(LIB_DIRS)) $(addprefix -l ,$(LIBS))
EXTRA_LDFLAGS = #其他链接选项
#====================伪目标================
.PHONY: all clean
all:$(TARGET)

clean:
	@-rm -f $(OBJS) $(DEPS) $(TARGET);
	@echo "clear *.d,*.o,target completly!"
test:
	@echo $(OBJS)
#=====================目标==================
$(TARGET): $(OBJS)
	@echo -e "Linking $^==>$@..."
	@$(CXX) $(LDFLAGS) $(EXTRA_LDFLAGS) $^ -o $@ 
	@echo -e "Link completly"
#===================目标文件生成规则================
%.o:%.c
	@echo -e "Compiling $< ==> $@..."
	@$(CC) -c $(CXXFLAGS) $(EXTRA_CXXFLAGS) $< -o $@
%.o:%.cpp
	@echo -e "Compiling $< ==> $@..."
	@$(CXX) -c $(CXXFLAGS) $(EXTRA_CXXFLAGS) $< -o $@
#===================依赖文件包含================
-include $(DEPS)#依赖文件规则包含，修改文件后只对有依赖的文件进行编译