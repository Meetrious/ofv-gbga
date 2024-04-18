STD=-std=c++17
W_FLAGS = #-Wall -Werror -Wextra
T_INST_INCLUDE_DIR = ~/dev/tiny_instruments/include/
SET_LOCAL_INCLUDE_DIR = -I include
RT_DIR := src

# модули для bga_task
BGA_TASK_MODULES_NAMES := task parameters ios
BGA_TASK_MODULES := $(foreach NAME,$(BGA_TASK_MODULES_NAMES),${RT_DIR}${NAME}.cpp)

cpk:
	cppcheck -${STD} --enable=all \
	-I include -I ~/dev/tiny_instruments/include/ \
	--suppress=missingIncludeSystem src/*.cpp include/ofv_bga/template_lib/*.hpp

bga_task:
	g++ -c include/ofv_bga/template_lib/task.hpp ${STD} ${W_FLAGS}\
		$(SET_LOCAL_INCLUDE_DIR) \
		-I $(T_INST_INCLUDE_DIR) -o $@_module.o

evo_pipe:
	g++ -c src/evo_pipe.cpp ${STD} ${W_FLAGS}\
	 $(SET_LOCAL_INCLUDE_DIR) -I $(T_INST_INCLUDE_DIR) -o $@_module.o

bga_st_solver:
	g++ -c include/ofv_bga/template_lib/st_solver_for_BGA.hpp ${STD} ${W_FLAGS}\
	 $(SET_LOCAL_INCLUDE_DIR) -I $(T_INST_INCLUDE_DIR) -o $@_module.o

# compiles
individ:
	g++ -c src/individ.cpp ${STD} ${W_FLAGS}\
	 $(SET_LOCAL_INCLUDE_DIR) -I $(T_INST_INCLUDE_DIR) 

# compiles
norm_interface:
	g++ -c src/norm_interface.cpp ${STD} ${W_FLAGS}\
	 $(SET_LOCAL_INCLUDE_DIR) -o $@_module.o

# compiles
bga_parameters:
	g++ -c src/task_parameters.cpp ${STD} ${W_FLAGS}\
	 $(SET_LOCAL_INCLUDE_DIR) -o $@_module.o

# compiles
bga_ios:
	g++ -c src/task_ios.cpp ${STD} ${W_FLAGS}\
	$(SET_LOCAL_INCLUDE_DIR)  -I $(T_INST_INCLUDE_DIR) -o $@_module.o

by_cmake:
	cmake -Wno-dev -B ./build -S .
	make -C build
	
%.o: %.cpp
	@echo "$(BLACK_BG)$(YELLOW_TXT)\n compiling non-executable-binary-object for $<"
	g++ -c $< -o $@
	@echo "$(RESET_COLORS)"

clean:
	rm -rf ./build/*
	rm -rf *.o *.out
	rm -rf */*.o */*.out
