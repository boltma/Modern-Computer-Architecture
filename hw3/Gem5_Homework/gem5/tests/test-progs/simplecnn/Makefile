main: main.c peripheral.o
	aarch64-linux-gnu-gcc -O2 -o main main.c peripheral.o ../../../util/m5/m5op_arm_A64.S -lm -static -Wno-unused-result

peripheral.o: peripheral.c
	aarch64-linux-gnu-gcc -O2 -c peripheral.c -Wno-unused-result

clean:
	rm main peripheral.o
	
