#!/bin/sh

# Script to compose mosaic video from 16 MJPEG videos with accelerated hardware for encoding or decoding
# Use ffmpeg or gstreamer as backend. Quick Sync Video (Intel) for FFMPEG, VAAPI for GStreamer

FOLDER="tests"
OUTPUT="$FOLDER/mosaic16.qsv.avi"

# gstreamer or ffmpeg
BACKEND=$1

WIDTH="640"
HEIGHT="480"
SIZE="${WIDTH}x${HEIGHT}"

REDUCED_HEIGHT=`expr $HEIGHT / 4`
REDUCED_WIDTH=`expr $WIDTH / 4`
REDUCED_SIZE="${REDUCED_WIDTH}x${REDUCED_HEIGHT}"

Y0=`expr $REDUCED_HEIGHT \* 0`
Y1=`expr $REDUCED_HEIGHT \* 1`
Y2=`expr $REDUCED_HEIGHT \* 2`
Y3=`expr $REDUCED_HEIGHT \* 3`

X0=`expr $REDUCED_WIDTH \* 0`
X1=`expr $REDUCED_WIDTH \* 1`
X2=`expr $REDUCED_WIDTH \* 2`
X3=`expr $REDUCED_WIDTH \* 3`

DECODE="! image/jpeg,width=$WIDTH,height=$HEIGHT,framerate=15/1 ! vaapijpegdec"
CONVERT="videoconvert ! videoscale ! video/x-raw,format=I420,width=$REDUCED_WIDTH,height=$REDUCED_HEIGHT,framerate=15/1"

if [ -z "$BACKEND" -o "$BACKEND" == "ffmpeg" ]; then
	ffmpeg -y \
		-i $FOLDER/1.avi -i $FOLDER/2.avi -i $FOLDER/3.avi -i $FOLDER/4.avi \
		-i $FOLDER/5.avi -i $FOLDER/6.avi -i $FOLDER/7.avi -i $FOLDER/8.avi \
		-i $FOLDER/9.avi -i $FOLDER/10.avi -i $FOLDER/11.avi -i $FOLDER/12.avi \
		-i $FOLDER/13.avi -i $FOLDER/14.avi -i $FOLDER/15.avi -i $FOLDER/16.avi \
		-filter_complex "
			nullsrc=size=$SIZE [base];
			[0:v] setpts=PTS-STARTPTS, scale=$REDUCED_SIZE [1];
			[1:v] setpts=PTS-STARTPTS, scale=$REDUCED_SIZE [2];
			[2:v] setpts=PTS-STARTPTS, scale=$REDUCED_SIZE [3];
			[3:v] setpts=PTS-STARTPTS, scale=$REDUCED_SIZE [4];
			[4:v] setpts=PTS-STARTPTS, scale=$REDUCED_SIZE [5];
			[5:v] setpts=PTS-STARTPTS, scale=$REDUCED_SIZE [6];
			[6:v] setpts=PTS-STARTPTS, scale=$REDUCED_SIZE [7];
			[7:v] setpts=PTS-STARTPTS, scale=$REDUCED_SIZE [8];
			[8:v] setpts=PTS-STARTPTS, scale=$REDUCED_SIZE [9];
			[9:v] setpts=PTS-STARTPTS, scale=$REDUCED_SIZE [10];
			[10:v] setpts=PTS-STARTPTS, scale=$REDUCED_SIZE [11];
			[11:v] setpts=PTS-STARTPTS, scale=$REDUCED_SIZE [12];
			[12:v] setpts=PTS-STARTPTS, scale=$REDUCED_SIZE [13];
			[13:v] setpts=PTS-STARTPTS, scale=$REDUCED_SIZE [14];
			[14:v] setpts=PTS-STARTPTS, scale=$REDUCED_SIZE [15];
			[15:v] setpts=PTS-STARTPTS, scale=$REDUCED_SIZE [16];
			[base][1] overlay=shortest=0 [tmp1];
			[tmp1][2] overlay=shortest=0:x=$X1 [tmp2];
			[tmp2][3] overlay=shortest=0:y=$Y1 [tmp3];
			[tmp3][4] overlay=shortest=0:x=$X1:y=$Y1 [tmp4];
			[tmp4][5] overlay=shortest=0:x=$X2 [tmp5];
			[tmp5][6] overlay=shortest=0:y=$Y2 [tmp6];
			[tmp6][7] overlay=shortest=0:x=$X2:y=$Y2 [tmp7];
			[tmp7][8] overlay=shortest=0:x=$X3 [tmp8];
			[tmp8][9] overlay=shortest=0:y=$Y3 [tmp9];
			[tmp9][10] overlay=shortest=0:x=$X3:y=$Y3 [tmp10];
			[tmp10][11] overlay=shortest=0:x=$X1:y=$Y2 [tmp11];
			[tmp11][12] overlay=shortest=0:x=$X1:y=$Y3 [tmp12];
			[tmp12][13] overlay=shortest=0:x=$X2:y=$Y1 [tmp13];
			[tmp13][14] overlay=shortest=0:x=$X2:y=$Y3 [tmp14];
			[tmp14][15] overlay=shortest=0:x=$X3:y=$Y2 [tmp15];
			[tmp15][16] overlay=shortest=0:x=$X3:y=$Y1
		" \
		-c:v h264_qsv -preset:v faster -vcodec h264 $OUTPUT
else
	gst-launch-1.0 -e \
		videomixer name=mix \
			sink_0::xpos=0 sink_0::ypos=0  sink_0::alpha=0\
			sink_1::xpos=$X0 sink_1::ypos=$Y0 \
			sink_2::xpos=$X1 sink_2::ypos=$Y0 \
			sink_3::xpos=$X0 sink_3::ypos=$Y1 \
			sink_4::xpos=$X1 sink_4::ypos=$Y1 \
			sink_5::xpos=$X2 sink_5::ypos=$Y0 \
			sink_6::xpos=$X2 sink_6::ypos=$Y1 \
			sink_7::xpos=$X2 sink_7::ypos=$Y2 \
			sink_8::xpos=$X2 sink_8::ypos=$Y3 \
			sink_9::xpos=$X3 sink_9::ypos=$Y0 \
			sink_10::xpos=$X3 sink_10::ypos=$Y1 \
			sink_11::xpos=$X3 sink_11::ypos=$Y2 \
			sink_12::xpos=$X3 sink_12::ypos=$Y3 \
			sink_13::xpos=$X0 sink_13::ypos=$Y2 \
			sink_14::xpos=$X1 sink_14::ypos=$Y2 \
			sink_15::xpos=$X0 sink_15::ypos=$Y3 \
			sink_16::xpos=$X1 sink_16::ypos=$Y3 \
		    ! jpegenc ! avimux ! filesink location=$OUTPUT \
		videotestsrc pattern="black" \
		    ! video/x-raw,width=$WIDTH,height=$HEIGHT \
		    ! mix.sink_0 \
			filesrc location=$FOLDER/1.avi $DECODE ! $CONVERT ! mix.sink_1 \
			filesrc location=$FOLDER/2.avi $DECODE ! $CONVERT ! mix.sink_2 \
			filesrc location=$FOLDER/3.avi $DECODE ! $CONVERT ! mix.sink_3 \
			filesrc location=$FOLDER/4.avi $DECODE ! $CONVERT ! mix.sink_4 \
			filesrc location=$FOLDER/5.avi $DECODE ! $CONVERT ! mix.sink_5 \
			filesrc location=$FOLDER/6.avi $DECODE ! $CONVERT ! mix.sink_6 \
			filesrc location=$FOLDER/7.avi $DECODE ! $CONVERT ! mix.sink_7 \
			filesrc location=$FOLDER/8.avi $DECODE ! $CONVERT ! mix.sink_8 \
			filesrc location=$FOLDER/9.avi $DECODE ! $CONVERT ! mix.sink_9 \
			filesrc location=$FOLDER/10.avi $DECODE ! $CONVERT ! mix.sink_10 \
			filesrc location=$FOLDER/11.avi $DECODE ! $CONVERT ! mix.sink_11 \
			filesrc location=$FOLDER/12.avi $DECODE ! $CONVERT ! mix.sink_12 \
			filesrc location=$FOLDER/13.avi $DECODE ! $CONVERT ! mix.sink_13 \
			filesrc location=$FOLDER/14.avi $DECODE ! $CONVERT ! mix.sink_14 \
			filesrc location=$FOLDER/15.avi $DECODE ! $CONVERT ! mix.sink_15 \
			filesrc location=$FOLDER/16.avi $DECODE ! $CONVERT ! mix.sink_16
fi
