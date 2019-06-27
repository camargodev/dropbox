# Fake Dropbox 

To compile everything, run ```make```.

##### To start a new server 

Run ```./server <port>```. 

##### To start a backup server 

Run ```./server <port> <original_svr_ip> <original_svr_port```.

##### To start a client

Run ```./client <username> <server_ip> <server_port>``` or ```./client <username> <server_ip> <server_port> <access_port>```. The access port is the port that the client provides to external services for communication. This port is used solely by new elected server to inform the client who is the new coordinator. The default (if not provided) is 4020.


#### Authors
João Vitor de Camargo 
Bernardo Trevisan
Lúcio Pereira
Cristiano Salla