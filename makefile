TEST_FILES=$(wildcard test/inc/*.h)

all:
	g++ -w -O2 -o ./bin/run main.cpp -I./inc -lrt
aix:
	xlC_r -w -O2 -o ./bin/aix_run main.cpp -I./inc
cxx:
	${CXXTEST_HOME}/bin/cxxtestgen --error-printer -o ./test/src/test.cpp ${TEST_FILES} 
	g++ -g -w -Wall -o ./test/bin/test_bin ./test/src/test.cpp -I./inc -I./ -I${CXXTEST_HOME} 
	g++ -Wall -o run main.cpp -I./inc
	./test/bin/test_bin
clean:
	rm -rf ./obj/*
	rm -rf ./bin/*
	rm -rf ./test/src/*
	rm -rf ./test/bin/*
