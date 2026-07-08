import setup_path
import colosseum
import os
import tempfile

client = colosseum.VehicleClient()
client.confirmConnection()

tmp_dir = os.path.join(tempfile.gettempdir(), "colosseum_cv_mode")
print ("Saving images to %s" % tmp_dir)
try:
    os.makedirs(tmp_dir)
except OSError:
    if not os.path.isdir(tmp_dir):
        raise

CAM_NAME = "front_center"
print(f"Camera: {CAM_NAME}")

colosseum.wait_key('Press any key to get camera parameters')

cam_info = client.simGetCameraInfo(CAM_NAME)
print(cam_info)

colosseum.wait_key(f'Press any key to get images, saving to {tmp_dir}')

requests = [colosseum.ImageRequest(CAM_NAME, colosseum.ImageType.Scene),
           colosseum.ImageRequest(CAM_NAME, colosseum.ImageType.DepthVis)]

def save_images(responses, prefix = ""):
    for i, response in enumerate(responses):
        filename = os.path.join(tmp_dir, prefix + "_" + str(i))
        if response.pixels_as_float:
            print(f"Type {response.image_type}, size {len(response.image_data_float)}, pos {response.camera_position}")
            colosseum.write_pfm(os.path.normpath(filename + '.pfm'), colosseum.get_pfm_array(response))
        else:
            print(f"Type {response.image_type}, size {len(response.image_data_uint8)}, pos {response.camera_position}")
            colosseum.write_file(os.path.normpath(filename + '.png'), response.image_data_uint8)


responses = client.simGetImages(requests)
save_images(responses, "old_fov")

colosseum.wait_key('Press any key to change FoV and get images')

client.simSetCameraFov(CAM_NAME, 120)
responses = client.simGetImages(requests)
save_images(responses, "new_fov")

new_cam_info = client.simGetCameraInfo(CAM_NAME)
print(new_cam_info)

print(f"Old FOV: {cam_info.fov}, New FOV: {new_cam_info.fov}")
