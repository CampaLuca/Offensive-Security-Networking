## Description: Simple Chat

The folder contains two C files: a ```client``` and a ```server```. 
It is a concurrent server which is able to process concurrent requests from multiple clients. 
The clients can communicate with each other and they can interact at most for 3 minutes. 


## Usage

### Compiling
```sh
gcc client.c -o client
gcc server.c -o server
```

### Running
Each client can request the list of the clients connected to the server with the command ```list clients```.
Each client can send a message to an other client with the command ```send <name> <message>```

If we send something else to the server or If we try to send something to ourself, it will act as an echo server. 
 
