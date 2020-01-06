ifeq ($(CFLAG),)
  SRC_ROOT = .
  UTIL_MODULE = UtilLib
  STDHEADER = StdHeader
endif

ifeq ($(ULDLTASK_LIB_NAME),)
  ULDLTASK_LIB_NAME = libuldltask.a
endif

UL_DL_TASK_FILE = src/task_pool.c
UL_DL_TASK_LIB_FILE = src/proc_table.c src/ipc_ctrl.c src/task_action_hook.c
UL_DL_CLIENT_LIB = src/task_client_lib.c
UL_DL_LIB_FILE = $(UL_DL_TASK_FILE) $(UL_DL_TASK_LIB_FILE) $(UL_DL_CLIENT_LIB)


INCLUDE_DIR = \
-I$(SRC_ROOT) \
-I$(SRC_ROOT)/inc \
-I$(SRC_ROOT)/$(STDHEADER) \
-I$(SRC_ROOT)/$(UTIL_MODULE)/inc \
-I$(SRC_ROOT)/$(ULDLTASK_MODULE)/inc \
-I. \
-I./inc \
-I./h



all: stdheader utillib
	$(CC) -c $(UL_DL_LIB_FILE) $(CFLAG) $(INCLUDE_DIR)
	ar -r $(ULDLTASK_LIB_NAME) *.o
	mv $(ULDLTASK_LIB_NAME) ./lib/$(ULDLTASK_LIB_NAME)
	$(CC) -c src/task_pool_main.c $(CFLAG) $(INCLUDE_DIR)
	$(CC) *.o -luldltask -L./lib/ -lpthread -o udserver
	mv udserver $(SRC_ROOT)/../
	rm -rf *.o

debug: stdheader utillib
	$(CC) -c $(UL_DL_LIB_FILE) $(CFLAG) $(INCLUDE_DIR) $(DEBUG_FLAG)
	ar -r $(ULDLTASK_LIB_NAME) *.o
	mv $(ULDLTASK_LIB_NAME) ./lib/$(ULDLTASK_LIB_NAME)
	$(CC) -c src/task_pool_main.c $(CFLAG) $(INCLUDE_DIR) $(DEBUG_FLAG)
	$(CC) *.o -luldltask -L./lib/ -lpthread -o udserver
	mv udserver $(SRC_ROOT)/../
	rm -rf *.o

clean:
	rm -rf *.o ./lib/$(ULDLTASK_LIB_NAME)
	make -C $(UTIL_MODULE) clean

distclean: clean
	rm -rf udtest
	rm -rf $(SRC_ROOT)/$(STDHEADER) $(SRC_ROOT)/$(UTIL_MODULE)

test: stdheader utillib
	$(CC) -shared -fPIC -pie src/task_pool_main.c $(UL_DL_LIB_FILE) \
-o $(ULDLTASK_LIB_NAME) $(CFLAG) $(INCLUDE_DIR) -lpthread \
-DDEBUG_TEST -DCONSOLE_DEBUG \
-Wl,-E
	rm -rf *.o
	mv $(ULDLTASK_LIB_NAME) ./lib/$(ULDLTASK_LIB_NAME)
	$(CC) -c udtask_test.c $(CFLAG) $(INCLUDE_DIR) $(DEBUG_FLAG) \
-DDEBUG_TEST -DCONSOLE_DEBUG
	$(CC) *.o -luldltask -L./lib/ -lpthread -o udtest
	rm -rf *.o

stdheader:
	@git submodule init ; \
	git submodule update

utillib:
	@git submodule init ; \
	git submodule update
	make -C $(UTIL_MODULE)
