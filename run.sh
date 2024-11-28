SOURCES="test/testconv1d.cpp -I src/base -I src/myarray -I src/conv1d -DNDEBUG"

# gcc version
g++ ${SOURCES} -O3 -funsafe-math-optimizations -march=native -g -o testconv1d && ./testconv1d

# Intel oneApi version
#icpx ${SOURCES} -g -O3 -fp-model=fast -xHost -o testconv1d && ./testconv1d
