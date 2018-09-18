recover-from-access-violation.objdump: recover-from-access-violation.exe
	objdump -M intel -S $< > $@

recover-from-access-violation.exe: recover-from-access-violation.c
	gcc -O0 -g $< -o $@
