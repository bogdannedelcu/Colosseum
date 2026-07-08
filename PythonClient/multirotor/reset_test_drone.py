import setup_path 
import colosseum

import time

# connect to the Colosseum simulator
client = colosseum.MultirotorClient()
client.confirmConnection()
client.enableApiControl(True)
client.armDisarm(True)

print("fly")
client.moveToPositionAsync(0, 0, -10, 5).join()

print("reset")
client.reset()
client.enableApiControl(True)
client.armDisarm(True)
time.sleep(5)
print("done")

print("fly")
client.moveToPositionAsync(0, 0, -10, 5).join()
