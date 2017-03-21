SYSTEM = x86-64_linux
LIBFORMAT = static_pic

GUROBI_DIR = /opt/gurobi651/linux64
CLIB     = -L$(GUROBI_DIR)/lib/ -lgurobi65
INC      = $(GUROBI_DIR)/include/
CPPLIB   = -L$(GUROBI_DIR)/lib/ -lgurobi_c++ $(CLIB)
GUROBI_OPTS = -I$(INC) $(CPPLIB) -lpthread -lm -m64 

CC = g++ 
CFLAGS = -O2 -Wall -ansi -pedantic -DIL_STD 
DEBUG = -pg -g -Wall -ansi -pedantic -DIL_STD 
OBJECTS = main.o time.o instance.o
 
exec : $(OBJECTS) 
	$(CC) $(CFLAGS) -o REFLECT_GUROBI_PURE $(OBJECTS) $(GUROBI_OPTS)  
 
.cpp.o : 
	$(CC) $(CFLAGS) $(GUROBI_OPTS) -c $< -o $@ 
 
clean : 
	rm -f $(OBJECTS) REFLECT_GUROBI_PURE
