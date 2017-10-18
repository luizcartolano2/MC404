run:
	source /home/specg12-1/mc404/simulador/set_path.sh
	arm-eabi-as ra183012.s -o ra183012.o
	arm-eabi-ld ra183012.o -o ra183012 -Ttext=0x77802000 -Tdata=0x77803000
	mksd.sh --so /home/specg12-1/mc404/simulador/dummyos.elf --user ra183012
	arm-sim --rom=/home/specg12-1/mc404/simulador/dumboot.bin --sd=disk.img

clean:
	rm disk.img ra183012 ra183012.s ra183012.o
