ASEMBLER_S = src/asembler.cpp\
	src/instrukcije.cpp\
	src/mainAsembler.cpp\
	src/tabelaLiterala.cpp\
	src/tabelaRealokacija.cpp\
	src/tabelaSekcija.cpp\
	src/tabelaSimbola.cpp\
	misc/lexer.cpp\
	misc/parser.cpp\

LINKER_S= src/linker.cpp\
	src/mainLinker.cpp\
	src/linkerFajl.cpp\
	src/tabelaSimbola.cpp\
	src/tabelaSekcija.cpp\
	src/tabelaRealokacija.cpp\
	src/tabelaLiterala.cpp\

EMULATOR_S= src/emulator.cpp\
	src/mainEmulator.cpp\

all: asembler linker emulator

bison:
	bison -d misc/bison.y

flex: bison
	flex misc/flex.l

asembler: flex 
	g++ -Iinc -o ${@} ${ASEMBLER_S}

linker:
	g++ -Iinc -o ${@} ${LINKER_S}

emulator:
	g++ -Iinc -o ${@} ${EMULATOR_S}

cleanAsembler:
	rm -f asembler
cleanLinker:
	rm -f linker
cleanEmulator:
	rm -f emulator

clean: cleanAsembler cleanLinker cleanEmulator
