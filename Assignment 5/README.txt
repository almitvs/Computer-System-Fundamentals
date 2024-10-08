Aidan Alme-- code and debugging
Bob Alme-- code and debugging

Data Structures that Need Synchronization
1. 'Table' Objects:
   table needs synchronization to ensure only one lient can modify a table at a time.
2. stack
   stack concurrent access can lead to data corruption
3. locked_tables and altered_tables
   we can't have multiple clients attempt to lock the same table

Synochronization Method:
we used mutex lock to synchronize access. tables are locked at the start of the operation and unlocked after it completes. for transactional operations, tabls are locked and remain locked until the transaction is committed.

Why the server is free of race conditions and deadlocks:
We have methods like commit_tables and rollback_tables and trylock to prevent blocking and deadlock risks. We also have exception handling that ensure the soomth running of a single-threaded client connections.
