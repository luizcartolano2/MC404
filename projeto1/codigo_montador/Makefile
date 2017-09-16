SOURCE_FILES=emitirMapaDeMemoria.c main.c processarEntrada.c token.c
OUTPUT_FILE=montador.x
TEST_PATH=./testes-abertos_v2/
ROOT_PATH=./
SUFFIX_PART1=.parte1.res
SUFFIX_PART2=.res
NUMBERS=00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
LEFT_PARENTHESIS=(
RIGHT_PARENTHESIS=)

$(OUTPUT_FILE):
	gcc $(SOURCE_FILES) -lm -o $(OUTPUT_FILE)

clean:
	rm -f $(OUTPUT_FILE)

run_part1: montador.x
	for num in $(NUMBERS); \
	do diff <$(LEFT_PARENTHESIS)$(ROOT_PATH)$(OUTPUT_FILE) $(TEST_PATH)arq$$num.in 2>&1$(RIGHT_PARENTHESIS) $(TEST_PATH)arq$$num$(SUFFIX_PART1) || echo "FILE $$num DIFFERS"; \
	done
run_part2: montador.x
	for num in $(NUMBERS); \
	do diff <$(LEFT_PARENTHESIS)$(ROOT_PATH)$(OUTPUT_FILE) $(TEST_PATH)arq$$num.in 2>&1$(RIGHT_PARENTHESIS) $(TEST_PATH)arq$$num$(SUFFIX_PART2) || echo "FILE $$num DIFFERS"; \
	done
