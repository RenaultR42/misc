#!/bin/sh

DECODE="! image/jpeg,width=1,height=1,framerate=15/1,pixel-aspect-ratio=1/1 ! vaapijpegdec"
CONVERT="videoconvert ! videoscale ! video/x-raw,width=160,height=130"

gst-launch-1.0 -e \
videomixer name=mix \
        sink_0::xpos=0   sink_0::ypos=0  sink_0::alpha=0\
        sink_1::xpos=0   sink_1::ypos=0 \
        sink_2::xpos=160 sink_2::ypos=0 \
        sink_3::xpos=0   sink_3::ypos=130 \
        sink_4::xpos=160 sink_4::ypos=130 \
        sink_5::xpos=320 sink_5::ypos=0 \
        sink_6::xpos=320 sink_6::ypos=130 \
        sink_7::xpos=320 sink_7::ypos=260 \
        sink_8::xpos=320 sink_8::ypos=390 \
        sink_9::xpos=480 sink_9::ypos=0 \
        sink_10::xpos=480 sink_10::ypos=130 \
        sink_11::xpos=480 sink_11::ypos=260 \
        sink_12::xpos=480 sink_12::ypos=390 \
        sink_13::xpos=0   sink_13::ypos=260 \
        sink_14::xpos=160 sink_14::ypos=260 \
        sink_15::xpos=0 sink_15::ypos=390 \
        sink_16::xpos=160 sink_16::ypos=390 \
    ! jpegenc ! avimux ! filesink location=tests/results.avi \
videotestsrc pattern="black" \
    ! video/x-raw,width=640,height=480 \
    ! mix.sink_0 \
filesrc location=tests/test.avi $DECODE ! $CONVERT ! mix.sink_1 \
filesrc location=tests/test.avi $DECODE ! $CONVERT ! mix.sink_2 \
filesrc location=tests/test.avi $DECODE ! $CONVERT ! mix.sink_3 \
filesrc location=tests/test.avi $DECODE ! $CONVERT ! mix.sink_4 \
filesrc location=tests/test.avi $DECODE ! $CONVERT ! mix.sink_5 \
filesrc location=tests/test.avi $DECODE ! $CONVERT ! mix.sink_6 \
filesrc location=tests/test.avi $DECODE ! $CONVERT ! mix.sink_7 \
filesrc location=tests/test.avi $DECODE ! $CONVERT ! mix.sink_8 \
filesrc location=tests/test.avi $DECODE ! $CONVERT ! mix.sink_9 \
filesrc location=tests/test.avi $DECODE ! $CONVERT ! mix.sink_10 \
filesrc location=tests/test.avi $DECODE ! $CONVERT ! mix.sink_11 \
filesrc location=tests/test.avi $DECODE ! $CONVERT ! mix.sink_12 \
filesrc location=tests/test.avi $DECODE ! $CONVERT ! mix.sink_13 \
filesrc location=tests/test.avi $DECODE ! $CONVERT ! mix.sink_14 \
filesrc location=tests/test.avi $DECODE ! $CONVERT ! mix.sink_15 \
filesrc location=tests/test.avi $DECODE ! $CONVERT ! mix.sink_16 \
