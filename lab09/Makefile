cria_cartao:
	arm-eabi-as ra183012.s -o ra183012.o
	arm-eabi-ld ra183012.o -o ra183012 -Ttext=0x77802000
	mksd.sh --so /home/specg12-1/mc404/simulador/simulador_player/bin/knrl --user ra183012

inicia_simulador:
	player /home/specg12-1/mc404/simulador/simulador_player/worlds_mc404/simple.cfg

run:
	armsim_player --rom=/home/specg12-1/mc404/simulador/simulador_player/bin/dumboot.bin --sd=disk.img

clean:
	rm disk.img ra183012 ra183012.o
