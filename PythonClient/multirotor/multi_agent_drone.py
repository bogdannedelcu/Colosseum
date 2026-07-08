import colosseum
import cv2
import numpy as np
import os
import pprint
import setup_path 
import tempfile

# Use below in settings.json with Blocks environment
"""
{
	"SeeDocsAt": "https://github.com/CodexLabsLLC/Colosseum/blob/main/docs/settings.md",
	"SettingsVersion": 1.2,
	"SimMode": "Multirotor",
	"ClockSpeed": 1,
	
	"Vehicles": {
		"Drone1": {
		  "VehicleType": "SimpleFlight",
		  "X": 4, "Y": 0, "Z": -2
		},
		"Drone2": {
		  "VehicleType": "SimpleFlight",
		  "X": 8, "Y": 0, "Z": -2
		}

    }
}
"""

# connect to the Colosseum simulator
client = colosseum.MultirotorClient()
client.confirmConnection()
client.enableApiControl(True, "Drone1")
client.enableApiControl(True, "Drone2")
client.armDisarm(True, "Drone1")
client.armDisarm(True, "Drone2")

colosseum.wait_key('Press any key to takeoff')
f1 = client.takeoffAsync(vehicle_name="Drone1")
f2 = client.takeoffAsync(vehicle_name="Drone2")
f1.join()
f2.join()

state1 = client.getMultirotorState(vehicle_name="Drone1")
s = pprint.pformat(state1)
print("state: %s" % s)
state2 = client.getMultirotorState(vehicle_name="Drone2")
s = pprint.pformat(state2)
print("state: %s" % s)

colosseum.wait_key('Press any key to move vehicles')
f1 = client.moveToPositionAsync(-5, 5, -10, 5, vehicle_name="Drone1")
f2 = client.moveToPositionAsync(5, -5, -10, 5, vehicle_name="Drone2")
f1.join()
f2.join()

colosseum.wait_key('Press any key to take images')
# get camera images from the car
responses1 = client.simGetImages([
    colosseum.ImageRequest("0", colosseum.ImageType.DepthVis),  #depth visualization image
    colosseum.ImageRequest("1", colosseum.ImageType.Scene, False, False)], vehicle_name="Drone1")  #scene vision image in uncompressed RGB array
print('Drone1: Retrieved images: %d' % len(responses1))
responses2 = client.simGetImages([
    colosseum.ImageRequest("0", colosseum.ImageType.DepthVis),  #depth visualization image
    colosseum.ImageRequest("1", colosseum.ImageType.Scene, False, False)], vehicle_name="Drone2")  #scene vision image in uncompressed RGB array
print('Drone2: Retrieved images: %d' % len(responses2))

tmp_dir = os.path.join(tempfile.gettempdir(), "colosseum_drone")
print ("Saving images to %s" % tmp_dir)
try:
    os.makedirs(tmp_dir)
except OSError:
    if not os.path.isdir(tmp_dir):
        raise

for idx, response in enumerate(responses1 + responses2):

    filename = os.path.join(tmp_dir, str(idx))

    if response.pixels_as_float:
        print("Type %d, size %d" % (response.image_type, len(response.image_data_float)))
        colosseum.write_pfm(os.path.normpath(filename + '.pfm'), colosseum.get_pfm_array(response))
    elif response.compress: #png format
        print("Type %d, size %d" % (response.image_type, len(response.image_data_uint8)))
        colosseum.write_file(os.path.normpath(filename + '.png'), response.image_data_uint8)
    else: #uncompressed array
        print("Type %d, size %d" % (response.image_type, len(response.image_data_uint8)))
        img1d = np.fromstring(response.image_data_uint8, dtype=np.uint8) #get numpy array
        img_rgb = img1d.reshape(response.height, response.width, 3) #reshape array to 3 channel image array H X W X 3
        cv2.imwrite(os.path.normpath(filename + '.png'), img_rgb) # write to png

colosseum.wait_key('Press any key to reset to original state')

client.armDisarm(False, "Drone1")
client.armDisarm(False, "Drone2")
client.reset()

# that's enough fun for now. let's quit cleanly
client.enableApiControl(False, "Drone1")
client.enableApiControl(False, "Drone2")


