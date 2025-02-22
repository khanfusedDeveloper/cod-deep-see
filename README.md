Basic command for opening stream via gst-launch-1.0:
gst-launch-1.0 v4l2src device=/dev/video0 ! 'video/x-raw, height=720, width=1280, framerate=60/1' ! videoconvert ! ximagesink -e