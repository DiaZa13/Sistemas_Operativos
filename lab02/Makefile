default: ej5a ipc
	./ej5a && sleep 2

single: ipc
	./ipc 10 a A

ej5a: ej5a.c
	gcc -o ej5a ej5a.c

ipc: ipc.c
	gcc -o ipc ipc.c -lrt

clean:
	rm ipc
	rm ej5a