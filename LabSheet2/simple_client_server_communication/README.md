## Description: Simple Client Server Communication

The folder contains two C files: a ```client``` and a ```server```. 

## Usage

### Compiling
```sh
gcc client.c -o client
gcc server.c -o server
```

### Running
In order to test the client-server communication:
- open 2 terminal tabs
- within the first one, run the server ```./server```
- within the second one, you can run the client in two ways:
    - without sending a message ```./client <address> <port>``` (```./client 127.0.0.1 5000```). In this way, the connection will be kept alive for a timeout of 10 seconds after which the server will send a welcome message to the client
    - sending a message: ```./client <address> <port> <msg>``` (```./client 127.0.0.1 5000 hello```). You will notice that the server will act as an echo server, then it will answer with the same message. 

If you will try to run two clients in the same time, they will be handled in order and not concurrently by the server.