# OS-Project-2

## Prompt
### Arthur
>1) Arthur (main thread) spawns 10 knight threads
>2) Register a SIGUSR1 handler
>3) "Drop-ship" (enqueue) shrubberies to each knights queue using a SafeQueue "forever" until a knight plants 100 shrubberies
>4) When Arthur receives a SIGUSR1 from a knight thread, set a flag to break Arthur out of the “drop-ship” loop
>5) write a single message to each Knight (and the Historian) with the payload of “It can’t be done!"
>6) wait on all knights to terminate
>7) write a final message to the Historian documenting the escape from the Knights Who Say “Ekke Ekke Ekke Ekke Ptang Zoo Boing!"

### Knight
>1) Dequeue shrubberies from its one knight shrubbery queue
>2) if the shrubbery is nice and not too expensive: “plant” (tally) the shrubbery
>3) repeat steps (1) and (2) until one of two things occurs:

  >a. the Knight plants its 100th shrubbery—signal Arthur with SIGUSR1
  
  >b. the Knight reads the “It can’t be done!” message from Arthur
