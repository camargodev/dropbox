# Fake Dropbox 

To compile everything, run ```make```.

In one terminal, run ```./server <port>```. It starts a server at ```localhost``` (127.0.0.1).

In another terminal, run ```./client <username> localhost <port>```. The client is a simple application that receives an input from command line, send to the server and receives an answer back.

You can run on client, commands like ```upload <filepath>``` to upload a file or ```exit``` to close the connection.

