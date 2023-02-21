#!/bin/bash
MODE=$1

while true; do
    # Command to start the process
    ./Server $MODE &

    process_id=$!
    
    # Log the start
    echo "Started server process with ID $process_id at $(date)"
    
    # Wait for the process to finish
    wait $process_id
    
    # Log the restart
    echo "Restarting server process at $(date)"
done
