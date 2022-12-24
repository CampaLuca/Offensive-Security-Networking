## Description: Concurrent Client - Server Communication

The folder contains two C files: a ```client``` and a ```server```. 
It is a concurrent server which is able to process concurrent requests from multiple clients. 


## Usage

### Compiling
```sh
gcc client.c -o client
gcc server.c -o server
```

### Running
In order to test the concurrent server:
- open 3 terminal tabs
- within the first one, run the server ```./server```
- within the second one, run one client without sending a message ```./client <address> <port>``` (```./client 127.0.0.1 5000```). In this way, the connection will be kept alive for a timeout of 10 seconds after which the server will send a welcome message to the client.
- within the third one, run a client who will send a message: ```./client <address> <port> <msg>``` (```./client 127.0.0.1 5000 hello```). You will notice that the server will act as an echo server, then it will answer with the same message. That will happen even if there are other clients trying to interact with the same server. 
