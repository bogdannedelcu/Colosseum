#!/bin/bash
COLOSSEUM_EXECUTABLE=/home/colosseum_user/BlocksV2/BlocksV2.sh

echo Starting Colosseum binary...
$COLOSSEUM_EXECUTABLE &

echo Waiting 10 seconds before starting app...
sleep 10

echo Starting Python app
python3.6 /home/colosseum_user/app/multirotor.py