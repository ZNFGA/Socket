Tata Cara:

nano server.c
nano clientUDP.c
nano serverTCP.c
nano clientTCP.c

masukkin semua kodingannya

terus buat running buka 2 terminal buat server dan client

gcc server.c -o server
gcc clientUDP.c -o clientUDP

clang server.c -o server
clang clientUDP.c -o clientUDP

**Cara buat TCP sama**

./server.c
./clientUDP.c

terus nanti diclient disuruh masukin ip : 127.0.0.1

nah habis itu udah bisa ngirim pesan dan pada TCP bisa saling kirim tetapi UDP ga bisa
