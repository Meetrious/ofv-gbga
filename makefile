
T_INST_INCLUDE_DIR = ../tiny_instruments/include
SET_LOCAL_INCLUDE_DIR = -I include
SET_EXTERNAL_INCLUDE_DIR = -I ../program/includes

# модули для bga_task
BGA_TASK_MODULES_NAMES := task evo_pipe st_solver_for_BGA individ norm_interface task_parameters task_ios
BGA_TASK_MODULES := $(foreach NAME,$(BGA_TASK_MODULES_NAMES),src/${NAME}.o)


solver_executable_test: $(BGA_TASK_MODULES)
	g++ -std=c++20 src/tests/st_solver_test.cpp ../program/libs/StraightTask.cpp ../tiny_instruments/src/easy_random.cc $^ $(SET_LOCAL_INCLUDE_DIR) $(SET_EXTERNAL_INCLUDE_DIR) -I $(T_INST_INCLUDE_DIR) -o $@

bga_st_solver_test:
	g++ -c src/tests/st_solver_test.cpp $(SET_LOCAL_INCLUDE_DIR) $(SET_EXTERNAL_INCLUDE_DIR) -I $(T_INST_INCLUDE_DIR) -o $@_module.o

bga_task:
	g++ -c src/task.cpp $(SET_LOCAL_INCLUDE_DIR) -I $(T_INST_INCLUDE_DIR) -o $@_module.o

evo_pipe:
	g++ -c src/evo_pipe.cpp $(SET_LOCAL_INCLUDE_DIR) -I $(T_INST_INCLUDE_DIR) -o $@_module.o

bga_st_solver:
	g++ -c src/st_solver_for_BGA.cpp $(SET_LOCAL_INCLUDE_DIR) $(SET_EXTERNAL_INCLUDE_DIR) -o $@_module.o

# compiles
individ:
	g++ -c src/individ.cpp $(SET_LOCAL_INCLUDE_DIR) -I $(T_INST_INCLUDE_DIR) -o $@_module.o

# compiles
norm_interface:
	g++ -c src/norm_interface.cpp $(SET_LOCAL_INCLUDE_DIR) -o $@_module.o

# compiles
bga_parameters:
	g++ -c src/task_parameters.cpp $(SET_LOCAL_INCLUDE_DIR) -o $@_module.o

# compiles
bga_ios:
	g++ -c src/task_ios.cpp $(SET_LOCAL_INCLUDE_DIR)  -I $(T_INST_INCLUDE_DIR) -o $@_module.o

by_cmake:
	cmake -Wno-dev -B ./build -S .
	make -C build
	
%.o: %.cpp
	@echo "$(BLACK_BG)$(YELLOW_TXT)\n compiling non-executable-binary-object for $<"
	g++ -c $< $(SET_LOCAL_INCLUDE_DIR) $(SET_EXTERNAL_INCLUDE_DIR) -I $(T_INST_INCLUDE_DIR) -o $@
	@echo "$(RESET_COLORS)"

clean:
	rm -rf ./build/*
	rm -rf *.o *.out
	rm -rf */*.o */*.out
