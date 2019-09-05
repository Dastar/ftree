CXX = clang++ 
CXXFLAGS = -std=c++11 -pedantic-errors -Wall -Wextra -g
CPPCHECK = cppcheck --enable=all --suppress=missingIncludeSystem
NDFLAGS = -std=c++11 -pedantic-errors -Wall -Wextra -O2 -DNDEBUG
VLG = valgrind --leak-check=yes --track-origins=yes -q
CXXG = g++
CXXGFLAGS = -Weffc++ -c 

SRC = $(wildcard *.cpp)
HDR = $(wildcard *.hpp)

.PHONY: test
test : $(SRC) $(HDR)
	@echo "-------------New Compiling-------------"
	#@$(CPPCHECK) $(SRC) $(HDR)
	#@echo
	#@echo "Compiling ..."
	@$(CXX) $(CXXFLAGS) -o test.out $(SRC)
	@echo
	@echo "Testing ..."
	#@$(VLG) test.out
	@./test.out
	@echo "----------------Finite-----------------"

.PHONY: release
release : $(SRC) $(HDR)
	$(CXX) $(NDFLAGS) -o ftree $(SRC)
	

.PHONY: clean
clean :
	@rm -f *.out
