SOURCES="test/testconv1d.cpp -I src/base -I src/myarray -I src/conv1d -DNDEBUG"

VENDOR=${1}

if [ "$VENDOR" == "gcc" ]; then
	# gcc version
	g++ ${SOURCES} -g -O3 -ffast-math -march=native -o testconv1d && ./testconv1d
elif [ "$VENDOR" == "oneapi" ]; then
	# Intel oneApi version
	icpx ${SOURCES} -g -O3 -fp-model=fast -xHost -o testconv1d && ./testconv1d
else
	echo "usage: $0 [gcc/oneapi]"
fi
