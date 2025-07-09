# Party Room Synchronization (C++)

This program simulates a multithreaded scenario where students enter and leave a room, and a "partybreaker" thread periodically enters the room when there are at least three students inside. 

## Problem Description

- Students enter and leave the room three times each.
- The partybreaker can only enter when at least three students are inside.
- While the partybreaker is inside, no students can enter.
- The partybreaker waits until all students leave before exiting the room.
- Synchronization is handled using mutexes and condition variables (`pthread_mutex_t` and `pthread_cond_t`).

## How to Compile and Run for i.e. 5 students


```bash
g++ -pthread -o party-room party-room.cpp
./party-room 5
```
# Sample output:
student 1 entered the room (1. time)  
student 2 entered the room (1. time)  
student 3 entered the room (1. time)  
partybreaker entered the room  
student 1 left the room  
student 2 left the room  
student 3 left the room  
partybreaker left the room  
...
