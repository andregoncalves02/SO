# Introduction
This was a practical work proposed by the professors of the Operating Systems course. We were tasked with implementing two programs: a client (Tracer) and a server (Monitor), establishing communication between them. To accomplish this task, we could only use functions form the C library for writing, reading and opening files.

The Tracer program is responsible for executing the tasks and requests made by users, and communicate their execution times to the server. In turn, the server program should be capable of responding to many clients at the same time and storing all their data and execution times. This two programs communicate by FIFO, where a client has his own PID, that identifies him and also makes possible for the server to respond back.
