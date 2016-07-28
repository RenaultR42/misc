#!/bin/sh

FILENAME=mosaic16.qsv.avi

./ffmpeg -y \
	-i 1.avi -i 2.avi -i 3.mp4 -i 4.mp4 -i 5.avi -i 6.avi -i 7.avi -i 8.mp4 -i 9.mp4 -i 10.mp4 -i 11.mp4 -i 12.avi -i 13.mp4 -i 14.mp4 -i 15.mp4 -i 16.mp4 \
	-filter_complex "
		nullsrc=size=640x480 [base];
		[0:v] setpts=PTS-STARTPTS, scale=160x120 [1];
		[1:v] setpts=PTS-STARTPTS, scale=160x120 [2];
		[2:v] setpts=PTS-STARTPTS, scale=160x120 [3];
		[3:v] setpts=PTS-STARTPTS, scale=160x120 [4];
		[4:v] setpts=PTS-STARTPTS, scale=160x120 [5];
		[5:v] setpts=PTS-STARTPTS, scale=160x120 [6];
		[6:v] setpts=PTS-STARTPTS, scale=160x120 [7];
		[7:v] setpts=PTS-STARTPTS, scale=160x120 [8];
		[8:v] setpts=PTS-STARTPTS, scale=160x120 [9];
		[9:v] setpts=PTS-STARTPTS, scale=160x120 [10];
		[10:v] setpts=PTS-STARTPTS, scale=160x120 [11];
		[11:v] setpts=PTS-STARTPTS, scale=160x120 [12];
		[12:v] setpts=PTS-STARTPTS, scale=160x120 [13];
		[13:v] setpts=PTS-STARTPTS, scale=160x120 [14];
		[14:v] setpts=PTS-STARTPTS, scale=160x120 [15];
		[15:v] setpts=PTS-STARTPTS, scale=160x120 [16];
		[base][1] overlay=shortest=0 [tmp1];
		[tmp1][2] overlay=shortest=0:x=160 [tmp2];
		[tmp2][3] overlay=shortest=0:y=120 [tmp3];
		[tmp3][4] overlay=shortest=0:x=160:y=120 [tmp4];
		[tmp4][5] overlay=shortest=0:x=320 [tmp5];
		[tmp5][6] overlay=shortest=0:y=240 [tmp6];
		[tmp6][7] overlay=shortest=0:x=320:y=240 [tmp7];
		[tmp7][8] overlay=shortest=0:x=480 [tmp8];
		[tmp8][9] overlay=shortest=0:y=360 [tmp9];
		[tmp9][10] overlay=shortest=0:x=480:y=360 [tmp10];
		[tmp10][11] overlay=shortest=0:x=160:y=240 [tmp11];
		[tmp11][12] overlay=shortest=0:x=160:y=360 [tmp12];
		[tmp12][13] overlay=shortest=0:x=320:y=120 [tmp13];
		[tmp13][14] overlay=shortest=0:x=320:y=360 [tmp14];
		[tmp14][15] overlay=shortest=0:x=480:y=240 [tmp15];
		[tmp15][16] overlay=shortest=0:x=480:y=120
	" \
	-c:v h264_qsv -preset:v faster -vcodec h264 $FILENAME
