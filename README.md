# Folder Synchronisation
A program which synchronize two folders (source and replica) periodically. After synchronisation content of the replica folder will be modified to exactly match content of the source folder.

How to run/stop the program
-------
1. Download one of releases depends onn your system (x86 or x64)
2. Extract zip archive
3. Open Command Prompt 
4. Go to folder Release 
5. Run `Folder-Synchronisation.exe <source_folder> <replica_folder> <interval_seconds> <log_file>` where:
   - `<source_folder>` - path to the source folder
   - `<replica_folder>` - path to the replica folder
   - `<interval_seconds>` - delay between synchronisations
   - `<log_file>` - path to the log file 
6. To stop synchronisation press 'q'