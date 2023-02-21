#!/bin/bash

while true; do

    sleep 1

    # Command to start the process
    /home/guiaraujo/CACIC-GUI/Client ap_init &

    process_id=$!
    
    # Log the start
    echo "Started ap process with ID $process_id at $(date)"
    
    # Wait for the process to finish
    wait $process_id
    
    # Log the restart
    echo "Restarting ap process at $(date)"
done
