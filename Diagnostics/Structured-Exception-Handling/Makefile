all: trap-int-3.objdump recover-from-access-violation.objdump

trap-int-3.objdump: trap-int-3.exe
	objdump -M intel -S $< > $@

recover-from-access-violation.objdump: recover-from-access-violation.exe
	objdump -M intel -S $< > $@

trap-int-3.exe: trap-int-3.c
	gcc -std=c99 -O0 -g $< -o $@

recover-from-access-violation.exe: recover-from-access-violation.c
	gcc -O0 -g $< -o $@
