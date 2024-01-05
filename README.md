# Introduction
This was a practical work proposed by the professors of the Operating Systems course. We were tasked with implementing two programs: a client (Tracer) and a server (Monitor), establishing communication between them. The server should be capable of handling client requests and, in turn, storing the necessary information for this purpose.

To accomplish this task, it was necessary to utilize the C programming language. Only functions from the C library for file writing, reading, and opening were permitted for use.
# Monitor Program
To establish communication with the client, it was necessary to create an initial FIFO named "Recebe-clientes," which receives the PIDs of clients wishing to make requests and connect to the server.

Next, it checks if a request has already been received from the given PID. If the PID has been received previously, the server attempts to open the client's FIFO, where the name of the FIFO is the PID of that specific client. It waits to receive the execution time of the program executed by the client. Subsequently, the server creates a process that accesses the folder provided during server execution and generates a file with its name as the PID of the given client. It writes the client's PID, the program executed by the client, and the program's execution time into this file, separated by semicolons, and then adds this file to the folder.

Otherwise, if the server hasn’t previously received the client's PID, it still attempts to open the client's FIFO. Afterward, it reads a byte from the FIFO, which represents a flag indicating the type of request that the server wants to make.

Finally, the server closes the FIFO that receives the clients' PIDs. Upon process interruption, it releases the space used by the linked list and returns 0 if everything runs smoothly.
# Tracer Program

This program is responsible for executing the tasks and requests made by users, communicating the execution times of their tasks and the programs being executed to the server. Additionally, it notifies the server of the requests made by users, such as checking the status of programs being executed or already executed.

To achieve this, first, a FIFO is created, named after the PID of the client in use, which will later be opened by both the server and the client. Next, the "Recebe-clientes" FIFO is opened to notify the server that the client wishes to establish a connection and send information by transmitting its PID. After sending this information, the general "Recebe-clientes" FIFO is closed again
