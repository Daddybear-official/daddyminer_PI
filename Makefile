ifeq ($(OS),Windows_NT)
	libs := -lpthread -I"$(OPENSSL_ROOT_DIR)\include" -L"$(OPENSSL_ROOT_DIR)\lib" -llibcrypto -lws2_32
else
	libs := -lcrypto -lpthread
endif

DaddyMinerPI: src/DaddyMinerPI.c src/mine_DUCO_S1.h src/mine_DUCO_S1.c | bin
	gcc src/DaddyMinerPI.c src/mine_DUCO_S1.c -O3 -Wall -o bin/DaddyMinerPI $(libs)

nonceMiner_minimal: src/nonceMiner_minimal.c src/mine_DUCO_S1.h src/mine_DUCO_S1.c | bin
	gcc src/nonceMiner_minimal.c src/mine_DUCO_S1.c -O3 -Wall -o bin/nonceMiner_minimal $(libs)

daddymin: src/daddymin.c src/mine_DUCO_S1.h src/mine_DUCO_S1.c | bin
	gcc src/daddymin.c src/mine_DUCO_S1.c -O3 -Wall -o bin/daddymin $(libs)

bin:
	mkdir bin
