import setup_path 
import colosseum
import time

# This example shows how to use the External Physics Engine
# It allows you to control the drone through setVehiclePose and obtain collision information.
# It is especially useful for injecting your own flight dynamics model to the Colosseum drone.

# Use Blocks environment to see the drone colliding and seeing the collision information 
# in the command prompt.

# Add this line to your settings.json before running Colosseum:
# "PhysicsEngineName":"ExternalPhysicsEngine"


client = colosseum.VehicleClient()
client.confirmConnection()
pose = client.simGetVehiclePose()

pose.orientation = colosseum.to_quaternion(0.1, 0.1, 0.1)
client.simSetVehiclePose( pose, False )

for i in range(900):
    print(i)
    pose = client.simGetVehiclePose()
    pose.position = pose.position + colosseum.Vector3r(0.03, 0, 0)
    pose.orientation = pose.orientation + colosseum.to_quaternion(0.1, 0.1, 0.1)
    client.simSetVehiclePose( pose, False )
    time.sleep(0.003)
    collision = client.simGetCollisionInfo()
    if collision.has_collided:
        print(collision)

client.reset()