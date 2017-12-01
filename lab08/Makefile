run:
	arm-eabi-gcc mapa.c -S -o mapa.s
	arm-eabi-as mapa.s -o mapa.o
	arm-eabi-as dfs.s -o dfs.o
	arm-eabi-ld mapa.o dfs.o -o dfs -Ttext=0x77802000 -Tdata=0x77803000
	mksd.sh --so /home/specg12-1/mc404/simulador/dummyos.elf --user dfs
	arm-sim --rom=/home/specg12-1/mc404/simulador/dumboot.bin --sd=disk.img

debugrun:
	arm-eabi-gcc mapa.c -S -o mapa.s
	arm-eabi-as mapa.s -o mapa.o
	arm-eabi-as dfs.s -g -o dfs.o
	arm-eabi-ld mapa.o dfs.o -g -o dfs -Ttext=0x77802000 -Tdata=0x77803000
	mksd.sh --so /home/specg12-1/mc404/simulador/dummyos.elf --user dfs
	arm-sim --rom=/home/specg12-1/mc404/simulador/dumboot.bin --sd=disk.img -g

clean:
	rm -f disk.img dfs.o dfs disk.img mapa.o program mapa.s
