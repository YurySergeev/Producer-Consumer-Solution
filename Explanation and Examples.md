## Explanation

This solution is enclosed in an infinite loop of 2 threads - a producer and consumer which use shared memory limited to 2 items. The producer generates items (ints) and places them into the shared structure.
When the table if full, the consumer must wait until there is an open space. The consumer takes the items and empties the item slot, when there is nothing to consume, the consumer waits.  

##Example
Output when run:
```
Producer produced: 1       
Producer produced: 2      <-- Producer has enough time to fill the memory before Consumer begins
Consumer consumed: 2  
Consumer consumed: 1      <-- Consumer was able to take both items from filled list due to built-in 1 second Producer slowdown to be able to read the outputs
Producer produced: 3 
Consumer consumed: 3      <-- Both become synchronised
Producer produced: 4
Consumer consumed: 4
Producer produced: 5
Consumer consumed: 5
Producer produced: 6
Consumer consumed: 6
Producer produced: 7
Consumer consumed: 7
Producer produced: 8
Consumer consumed: 8
Producer produced: 9
Consumer consumed: 9
Producer produced: 10
Consumer consumed: 10     <-- Behavior will continue infinitly
```

Due to Consumer's built in sleep time, the producer has enough time to fill the memory and begin to wait, when the consumer begins operating, it has enough time to clear the memory as the producer needs time to generate the next item.
After the consumption of item 2, then 1 -- The system continues to produce 1 item and then consume it infinently many times.
