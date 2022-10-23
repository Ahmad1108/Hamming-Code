# Introduction

The project is about encoding/decoding and code correction .it contains 3 executable programs: sender.exe, channel.exe, receiver.exe.

The sender process takes two paramenters: IP address, port number in the command line arguments.
the sender makes a TCP socket connection to the channel and sends a stream of bits using (31,26) [Hamming Code](https://en.wikipedia.org/wiki/Hamming_code).

The channel takes in the command line arguments which noisy mode to insert, there are two modes:

- **Random**: flip a bit randomly, it takes 3 parameters in the command line: flag "-r", bit error probability(in units of 2^-16), random seed. e.g, `> channel.exe -r 650 34853`
- **Deterministic**: flip every n-th bit, i.e: n,2n,3n,4n,.... it takes 2 parameters in the command line : flag "-d" and n.
e.g, `> channel.exe -d 112` flips bits 112,224,336,...

The receiver takes two parameters: IP address, port number chosen by the channel. decode the recieved stream under Hamming Code and save it on a file name chosen from the user.

![](https://user-images.githubusercontent.com/109534594/197405209-56346841-d246-4707-925c-a9f10aff8aff.png)
## Constraints

- file name is limited to 100 bytes.
- IP address was limited to localhost(127.0.0.1) only.

## How to run?

Execution environment: Visual Studio 2019.
The implementation uses [Win32 API](https://learn.microsoft.com/en-us/windows/win32/api/) and windows sockets.

## EXECUTION PROCESS

1. Run channel.exe first in order to open ports for binding and listening.
2. Run reciever.exe with IP address and reciever port taken from channel.exe stdout.
3. Run sender.exe with IP address and sender port taken from channel.exe stdout and enter file name to be encoded.

### Example of a running scenario

```CLI
> channel.exe -d 112
sender socket: IP address = 127.0.0.1 port = 12403
reciever socket: IP address = 127.0.0.1 port = 6342
```

```CLI
>reciever.exe 127.0.0.1 6342
enter file name:
rec_file.txt
recieved:3100 bytes
wrote: 2600 bytes
corrected 12 errors
enter file name:
quit
>
```

```CLI
>sender.exe 127.0.0.1 12403
enter file name:
myfile.txt
file length: 2600 bytes
sent: 3100 bytes
enter file name:
quit
>
```
