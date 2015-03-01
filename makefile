TARGET = HTTPserver
DEBUG=yes
#==================����Ŀ¼=================
ALL_DIR = $(shell find . -type d)
INC_DIRS =./include #ͷ�ļ�·��
LIB_DIRS = #���ӿ�����Ŀ¼
#==================�ļ�====================
C_SRCS = $(foreach file,$(ALL_DIR),$(wildcard $(file)/*.c))#��Ҫ�����c�ļ�
CPP_SRCS = $(foreach file,$(ALL_DIR),$(wildcard $(file)/*.cpp))#��Ҫ�����cpp�ļ�
OBJS = $(C_SRCS:.c=.o)
OBJS += $(CPP_SRCS:.cpp=.o)
DEPS = $(C_SRCS:.c=.d)
DEPS += $(CPP_SRCS:.cpp=.d)
LIBS=#���ӿ�����
#==================Ԥ����=================
CC = gcc
CXX = g++
DEFINES = #��Ҫ����ĺ�
#==================����ѡ��=================
CFLAGS = -pipe -Wall -Wextra -MMD -DNDEBUG -D_GNU_SOURCE
ifeq ($(DEBUG),yes)
CFLAGS += -g
else
CFLAGS += -O3 -s
endif

CXXFLAGS = $(addprefix -I ,$(INC_DIRS)) $(addprefix -D ,$(DEFINES))
EXTRA_CXXFLAGS = $(CFLAGS)#��������ѡ��

LDFLAGS = $(addprefix -L ,$(LIB_DIRS)) $(addprefix -l ,$(LIBS))
EXTRA_LDFLAGS = #��������ѡ��
#====================αĿ��================
.PHONY: all clean
all:$(TARGET)

clean:
	@-rm -f $(OBJS) $(DEPS) $(TARGET);
	@echo "clear *.d,*.o,target completly!"
test:
	@echo $(OBJS)
#=====================Ŀ��==================
$(TARGET): $(OBJS)
	@echo -e "Linking $^==>$@..."
	@$(CXX) $(LDFLAGS) $(EXTRA_LDFLAGS) $^ -o $@ 
	@echo -e "Link completly"
#===================Ŀ���ļ����ɹ���================
%.o:%.c
	@echo -e "Compiling $< ==> $@..."
	@$(CC) -c $(CXXFLAGS) $(EXTRA_CXXFLAGS) $< -o $@
%.o:%.cpp
	@echo -e "Compiling $< ==> $@..."
	@$(CXX) -c $(CXXFLAGS) $(EXTRA_CXXFLAGS) $< -o $@
#===================�����ļ�����================
-include $(DEPS)#�����ļ�����������޸��ļ���ֻ�����������ļ����б���