TEST_FILES=$(wildcard test/inc/*.h)

all:
	g++ -w -O2 -o ./bin/run main.cpp -I./inc -lrt
aix:
	xlC_r -w -O2 -o ./bin/aix_run main.cpp -I./inc
cxx:
	${CXXTEST_HOME}/bin/cxxtestgen --error-printer -o ./test/src/test.cpp ${TEST_FILES}
	g++ -g -w -Wall -o ./test/bin/test_bin ./test/src/test.cpp -I./inc -I./ -I./test/inc -I${CXXTEST_HOME}
	./test/bin/test_bin
cxx_all:
	${CXXTEST_HOME}/bin/cxxtestgen --error-printer -o ./test/src/test.cpp ${TEST_FILES} 
	g++ -g -O0 --coverage -w -Wall -o ./test/bin/test_bin ./test/src/test.cpp -I./inc -I./ -I./test/inc -I${CXXTEST_HOME}
	cppcheck ./inc/* > /dev/null 
	g++ -Wall -o ./test/bin/warn_bin ./test/src/warn.cpp -I./inc
	./test/bin/test_bin
	lcov --quiet --directory ./ --capture --output-file ./test/app.info 2> /dev/null 
	genhtml --quiet --output-directory ./test/cov_htmp ./test/app.info 
clean:
	rm -rf ./test.gcno
	rm -rf ./test.gcda
	rm -rf ./obj/*
	rm -rf ./bin/*
	rm -rf ./test/app.info
	rm -rf ./test/cov_htmp
	rm -rf ./test/bin/*
	rm -rf ./test/src/test.cpp
