# client-server-socket-programming
client server chat application

Use Case :

1. Connection Establishment : ​ The client should send the connection request to server. The server
should reply client with appropriate messages if connection can be established or not.
Successful :

If the connection can be established successfully, then generate appropriate
identifiers for the client and store them at server. Identifiers includes - Connecting time, Unique Id (5
Digit Random Number) Unique Name(10 characters long random string). After connection is
established, send client the above details with a welcome message.

Unsuccessful : ​ If the number of clients connected are already 5 then no further client is allowed to
connect and server should inform client that “Connection Limit Exceeded !!”.

2. Data Transfer Phase : ​ Client should send a query message to server asking the details of
available clients. Server should send the details of all the online clients. After receiving details, client
can transfer messages to any other client of choice by using its unique id or name. (Note that this is
a one to one communication).

Design a buffer queue at server using shared memory architecture with a separate process handling
the message delivery. Use appropriate locking mechanism to handle this critical section where
messages to be delivered have to be written and processed for delivery.

There can be a situation when a client A gets the list of online clients and before it can send any
message to client B, client B goes offline or killed. The sender in this case should be notified that
client is now disconnected and that message should be discarded.

3. Connection Termination : When the kill signal is given at the client side, capture the kill signal
and write an handler routine that should notify all other clients with the details of client which is
going to disconnect. Then safely kill the client.


4. Server Logging and Load Analysis : ​ Maintain a log using file or database with details including
message, sender and receiver details.
Write a python script that read the log and generate the load statistics. When this script runs, it
should generate the following plots :
a.
Plot a bar graph illustrating the load imposed by various clients (number of characters sent by each
client ) till now.
b.
Plot heatmap illustrating the distribution of pairwise traffic among each client. Also generate a csv in
following format.
