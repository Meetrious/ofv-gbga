
T_INST_INCLUDE_DIR = ~/dev/tiny_instruments/include/
LOCAL_INCLUDE_DIR = include
RT_DIR := src

# модули для bga_task
BGA_TASK_MODULES_NAMES := task parameters ios
BGA_TASK_MODULES := $(foreach NAME,$(BGA_TASK_MODULES_NAMES),${RT_DIR}${NAME}.cpp)

individ:
	g++ -c $(RT_DIR)/individ.cpp -I $(T_INST_INCLUDE_DIR) -I $(LOCAL_INCLUDE_DIR)

bga_task:
	g++ -c $(RT_DIR)/parameters.cpp -I $(LOCAL_INCLUDE_DIR) -o $@_module.o

bga_st_solver:
	g++ -c $(RT_DIR)/st_solver_for_BGA.cpp -I $(LOCAL_INCLUDE_DIR) -o $@_module.o

norm_interface:
	g++ -c $(RT_DIR)/norm_interface.cpp -I $(LOCAL_INCLUDE_DIR) -o $@_module.o


clean:
	rm -rf *.o *.out
	rm -rf */*.o */*.out
