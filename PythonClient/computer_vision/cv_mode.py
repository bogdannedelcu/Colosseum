# In settings.json first activate computer vision mode:
# https://github.com/CodexLabsLLC/Colosseum/blob/main/docs/image_apis.md#computer-vision-mode

import setup_path
import colosseum

import pprint
import os
import time
import math
import tempfile

pp = pprint.PrettyPrinter(indent=4)

client = colosseum.VehicleClient()
client.confirmConnection()

colosseum.wait_key('Press any key to set camera-0 gimbal to 15-degree pitch')
camera_pose = colosseum.Pose(colosseum.Vector3r(0, 0, 0), colosseum.to_quaternion(math.radians(15), 0, 0)) #radians
client.simSetCameraPose("0", camera_pose)

colosseum.wait_key('Press any key to get camera parameters')
for camera_name in range(5):
    camera_info = client.simGetCameraInfo(str(camera_name))
    print("CameraInfo %d:" % camera_name)
    pp.pprint(camera_info)

tmp_dir = os.path.join(tempfile.gettempdir(), "colosseum_cv_mode")
print ("Saving images to %s" % tmp_dir)
try:
    os.makedirs(tmp_dir)
except OSError:
    if not os.path.isdir(tmp_dir):
        raise

colosseum.wait_key('Press any key to get images')
for x in range(3): # do few times
    z = x * -20 - 5 # some random number
    client.simSetVehiclePose(colosseum.Pose(colosseum.Vector3r(z, z, z), colosseum.to_quaternion(x / 3.0, 0, x / 3.0)), True)

    responses = client.simGetImages([
        colosseum.ImageRequest("0", colosseum.ImageType.DepthVis),
        colosseum.ImageRequest("1", colosseum.ImageType.DepthPerspective, True),
        colosseum.ImageRequest("2", colosseum.ImageType.Segmentation),
        colosseum.ImageRequest("3", colosseum.ImageType.Scene),
        colosseum.ImageRequest("4", colosseum.ImageType.DisparityNormalized),
        colosseum.ImageRequest("4", colosseum.ImageType.SurfaceNormals)])

    for i, response in enumerate(responses):
        filename = os.path.join(tmp_dir, str(x) + "_" + str(i))
        if response.pixels_as_float:
            print("Type %d, size %d, pos %s" % (response.image_type, len(response.image_data_float), pprint.pformat(response.camera_position)))
            colosseum.write_pfm(os.path.normpath(filename + '.pfm'), colosseum.get_pfm_array(response))
        else:
            print("Type %d, size %d, pos %s" % (response.image_type, len(response.image_data_uint8), pprint.pformat(response.camera_position)))
            colosseum.write_file(os.path.normpath(filename + '.png'), response.image_data_uint8)

    pose = client.simGetVehiclePose()
    pp.pprint(pose)

    time.sleep(3)

# currently reset() doesn't work in CV mode. Below is the workaround
client.simSetVehiclePose(colosseum.Pose(colosseum.Vector3r(0, 0, 0), colosseum.to_quaternion(0, 0, 0)), True)
