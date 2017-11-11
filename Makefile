ASM = $(wildcard asm/*.asm)
HEX = $(addprefix hex/,$(notdir $(ASM:.asm=.hex)))
LOG = $(addprefix logs/,$(notdir $(HEX:.hex=.log)))

default: all

all: $(LOG) $(HEX)

hex/%.hex: asm/%.asm
	@echo '.......................................................'
	 @echo 'Assembling ' $< '....'
	./assembler/bin/assembler $< $@
	@echo ' '

logs/%.log: hex/%.hex
	@echo 'Simulating ' $< '.....'
	./simulator/bin/sim -input $< -output $@
	@echo ''
	@echo $@ ': file succesfulling created'
	@echo '.......................................................'


clean:
	rm -f $(LOG) $(HEX)