# ============== NICLA VISION (SLAVE) CODE - PYTHON ==============
# Save as main.py on the Nicla Vision using the OpenMV IDE.

import sensor
import image
import time
from pyb import I2C
import struct

# --- I2C Configuration ---
I2C_SLAVE_ADDR = 8 # I2C Address (must match Arduino master)

# --- Camera & Vision Configuration ---
# Use a lower resolution for higher frame rates
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA) # 160x120 pixels
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)

# Define the Region of Interest (ROI) for the bottom half of the image
ROI = (0, sensor.height() // 2, sensor.width(), sensor.height() // 2)

# --- CALIBRATION REQUIRED ---
# 1. Find these LAB color thresholds using the OpenMV IDE's Threshold Editor
#    Format is (L_min, L_max, A_min, A_max, B_min, B_max)
GREEN_THRESHOLD = (30, 80, -70, -30, 0, 70)  # Placeholder for green
RED_THRESHOLD = (30, 80, 30, 80, 10, 70)      # Placeholder for red

# 2. Calibrate these values for your specific setup for distance/yaw
KNOWN_OBJECT_WIDTH = 5.0  # Real-world width of your object in cm
FOCAL_LENGTH_PIXELS = 157.0 # Calibrate this! See C++ code for explanation.
HORIZONTAL_FOV = 60.0     # Approx. horizontal field of view in degrees

# --- Global variable to hold the data to be sent ---
# This byte array will be updated continuously in the main loop.
# The format string '<ffiiii' means:
# <       - Little-endian (standard for Arduino)
# f       - float (4 bytes)
# i       - signed int (4 bytes)
# Yaw (f), Distance (f), Green Y (i), Green W (i), Red Y (i), Red W (i)
packed_data = struct.pack('<ffiiii', 0.0, 0.0, 0, 0, 0, 0)


# --- I2C Slave Callback Function ---
# This function is called by the hardware interrupt when the master requests data.
def i2c_slave_send_callback():
    # We can add logic here if needed, but for now, we just let the I2C object
    # handle sending the buffer that we update in the main loop.
    # The 'send' method is implicitly called on the 'packed_data' buffer.
    pass

# --- I2C Initialization ---
# Initialize I2C in SLAVE mode with our address
# The 'g_slave_buf' is the buffer that will be sent when the master requests data
i2c = I2C(1, I2C.SLAVE, addr=I2C_SLAVE_ADDR)
i2c.init(I2C.SLAVE, addr=I2C_SLAVE_ADDR, g_slave_buf=packed_data)


print("Nicla Vision Slave Ready.")
clock = time.clock()

# --- Main Loop ---
while True:
    clock.tick()
    img = sensor.snapshot()

    # --- Reset data for new frame ---
    yaw_angle = 0.0
    linear_distance = 0.0
    green_y, green_w, green_cx = 0, 0, 0
    red_y, red_w, red_cx = 0, 0, 0
    target_blob = None

    # --- Find the largest GREEN blob in the ROI ---
    green_blobs = img.find_blobs([GREEN_THRESHOLD], roi=ROI, pixels_threshold=100, area_threshold=100, merge=True)
    if green_blobs:
        largest_green = max(green_blobs, key=lambda b: b.pixels())
        green_y, green_w, green_cx = largest_green.y(), largest_green.w(), largest_green.cx()
        # For debugging, draw on the image in the IDE
        img.draw_rectangle(largest_green.rect(), color=(0, 255, 0))
        target_blob = largest_green

    # --- Find the largest RED blob in the ROI ---
    red_blobs = img.find_blobs([RED_THRESHOLD], roi=ROI, pixels_threshold=100, area_threshold=100, merge=True)
    if red_blobs:
        largest_red = max(red_blobs, key=lambda b: b.pixels())
        red_y, red_w, red_cx = largest_red.y(), largest_red.w(), largest_red.cx()
        img.draw_rectangle(largest_red.rect(), color=(255, 0, 0))
        # If the red blob is bigger than the green one, make it the target
        if target_blob is None or largest_red.w() > target_blob.w():
            target_blob = largest_red

    # --- Calculate Distance and Yaw if a target object was found ---
    if target_blob:
        # Estimate distance
        linear_distance = (KNOWN_OBJECT_WIDTH * FOCAL_LENGTH_PIXELS) / target_blob.w()

        # Estimate yaw angle
        angle_per_pixel = HORIZONTAL_FOV / sensor.width()
       # yaw_angle = (target_blob.cx() - (sensor.width() / 2.0)) * angle_per_pixel

    # --- Pack the final data into the global buffer ---
    # The I2C hardware will send this buffer when requested by the master.
    packed_data = struct.pack('<ffiiii', yaw_angle, linear_distance, green_y, green_w, red_y, red_w)

    # Update the buffer for the I2C slave object
    i2c.slave_deinit()
    i2c.init(I2C.SLAVE, addr=I2C_SLAVE_ADDR, g_slave_buf=packed_data)


    # Optional: Print to OpenMV IDE serial console for debugging
    # print(f"Yaw: {yaw_angle:.1f}, Dist: {linear_distance:.1f}")
