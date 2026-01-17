SRC = cpp/src
INC = cpp/include
BIN = cpp/bin
OBJ = cpp/obj
TEST = cpp/tests

CXX=clang++
CXXFLAGS = -I$(INC) -stdlib=libc++ -m$(ARCH)

.PHONY: test clean

test: $(TEST)/x86_test_vm

$(TEST)/x86_test_vm: $(OBJ)/x86_test_vm.o $(OBJ)/vm.o | $(TEST)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)/x86_test_vm.o $(OBJ)/vm.o

$(OBJ)/x86_test_vm.o: $(SRC)/vm_test.cpp | $(OBJ)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ)/vm.o: $(SRC)/vm.cpp | $(OBJ)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ):
	mkdir -p $@

$(TEST):
	mkdir -p $@

$(BIN):
	mkdir -p $@

clean:
	rm -rf $(OBJ)
	rm -rf $(TEST)
	rm -rf $(BIN)