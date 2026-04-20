# Lab 4 — Process Synchronization (Sender / Receiver via shared binary file) [C++98]

Implement inter-process message passing using a shared binary file as a FIFO ring buffer. One Receiver reads messages; multiple Senders write messages. Demonstrate process synchronization using OS primitives (named events, semaphores, mutexes).

---

## Requirements (summary)

### Receiver
- Read from console: name of the binary file and number of records (capacity).
- Create the binary message file; maximum message length = 20 characters.
- Read from console the number of Sender processes to start and launch them, passing the file name as a command-line argument.
- Wait for a readiness signal from all Senders before entering the control loop.
- Control loop: accept console commands to read the next message or terminate. If empty on read, block until a new message arrives.

### Sender
- Open the shared message file (file name from command line).
- Send a readiness signal to Receiver once started.
- Control loop: accept console commands to send a message (text input, max < 20 chars) or terminate. If full on write, block until space is available.

### Additional
- The message buffer must operate as a FIFO circular queue so messages are read in the same order they were sent across senders.
- A simplified variant for testing may use a single Sender and a single Receiver with capacity = 1.

---

## Key Files (suggested)
- `Lab 4/Receiver.cpp`
- `Lab 4/Sender.cpp`
- `Lab 4/RingBuffer.h`, `Lab 4/RingBuffer.cpp` — on-disk circular queue helpers
- `Lab 4/ipc_helpers.h` — platform-specific synchronization primitives (events, named mutexes/semaphores)
- `Lab 4/tests` — gtests for every aspect of application

---

## Build (CMake, Windows)
```bash
cmake -S . -B build
cmake --build build --config Release
```

---

## Run
- Start the Receiver first; specify file name and capacity.
- Launch the configured number of Sender processes (either from Receiver or manually).
- Interact via console commands to send/read messages and to terminate.

Example (paths/args may vary):
```bash
./build/Receiver.exe
# In separate terminals:
./build/Sender.exe <message_file>
./build/Sender.exe <message_file>
```

---

## Testing
```bash
ctest --test-dir build -V
```