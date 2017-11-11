ASM = $(wildcard asm/*.asm)
HEX = $(addprefix hex/,$(notdir $(ASM:.asm=.hex)))
LOG = $(addprefix logs/,$(notdir $(HEX:.hex=.log)))

Q1 = logs/Q1.log
Q2 = logs/Q2.log
Q3 = logs/Q3.log
Q4 = logs/Q4.log
Q4_nested = logs/Q4_nested.log
merge = logs/mergesort.log

default: all

all: $(LOG) $(HEX)

hex/%.hex: asm/%.asm
	@echo '.......................................................'
	 @echo 'Assembling ' $< '....'
	./assembler/bin/assembler $< $@
	@echo ' '

logs/%.log: hex/%.hex
	@echo 'Simulating ' $< '.....'
	./simulator/bin/sim -input $< -output $@ -mem a.mem
	@echo ''
	@echo $@ ': file succesfulling created'
	@echo '.......................................................'

Q1: $(Q1)
Q2: $(Q2)
Q3: $(Q3)
Q4: $(Q4) 
merge: $(merge)
Q4_nested: $(Q4_nested)

clean:
	rm -f $(LOG) $(HEX)