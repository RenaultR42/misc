#!/usr/bin/env python

from subprocess import call
from joblib import Parallel, delayed
import subprocess
import multiprocessing
import os
import sys

# A script to extract each image by ffmpeg tool from some video in different directories
# A directory should contain original video and others encoding with different parameters such as: fps, bitrate, resolution and and group of pictures size
# Then, with ImageMagick we compare each images extracted from original and compressed video
# We add each result in a total to compute the best compressed video parameters based on the quality value

DEVNULL=open(os.devnull, 'w')

format = [ 'SD', 'QV' ]
f2w = { 'SD': 640, 'QV': 320 }
fps = [ '15/3', '15/2', '15/1' ]
gop = [ 30, 120, 300 ]
bitrate = [ 100, 650, 800 ]

my_env = os.environ
num_cores = multiprocessing.cpu_count()

# To extract images from original video
for r in fps:
	rr = str(eval(r))
	for f in format:
		folder = './original_images/videos_{}format_{}fps_images'.format(f, rr)
		for name in os.listdir('./original'):
			call(['mkdir', '-p', '{}/{}'.format(folder, name)])
			sys.stdout.write('# ./{}/{}'.format(folder, name))
			sys.stdout.flush()
			ret = call(['ffmpeg', '-i', './original/{}'.format(name), '-vf', 'scale={}:-1'.format(f2w[f]), '-r', rr, '-y', '-f', 'image2', '{}/{}/./image_%4d.png'.format(folder, name)], stdout=DEVNULL, stderr=DEVNULL, env=my_env)
			print(' {}'.format(ret))

## To extract images from compressed video
for g in gop:
	for f in format:
		for r in fps:
			rr = str(eval(r))
			for b in bitrate:
				folder = 'videos_{}gop_{}format_{}fps_{}bitrate_images'.format(g, f, rr, b)
				folder_video = 'videos_{}gop_{}format_{}fps_{}bitrate'.format(g, f, rr, b)
				for name in os.listdir('./original'):
					call(['mkdir', '-p', '{}/{}'.format(folder, name)])
					sys.stdout.write('# ./{}/{}'.format(folder, name))
					sys.stdout.flush()
					ret = call(['ffmpeg', '-i', './{}/{}'.format(folder_video, name), '-r', rr, '-y', '-f', 'image2', './{}/{}/./image_%4d.png'.format(folder, name)], stdout=DEVNULL, stderr=DEVNULL, env=my_env)
					print(' {}'.format(ret))

# Launch images comparison, ideally into a specific thread
def launch_process(folder_original, image, name, folder_images):
	cmd = subprocess.Popen('compare -metric PSNR \"./original_images/{}/{}/{}\" \"{}/{}/{}\" /dev/null'.format(folder_original, name, image, folder_images, name, image), shell=True, stderr=subprocess.PIPE)
	for line in cmd.stderr:
		try:
			return float(line)
		except ValueError:
			print(line)
			return 0.0

# Compare all compressed images with original video
for g in gop:
	for f in format:
		for r in fps:
			rr = str(eval(r))
			for b in bitrate:
				folder_images = 'videos_{}gop_{}format_{}fps_{}bitrate_images'.format(g, f, rr, b)
				folder_original = 'videos_{}format_{}fps_images'.format(f, rr)
				compare_total = 0.0
				if os.path.exists('{}/results.txt'.format(folder_images)):
					break
				for name in os.listdir('./original'):
					compare_list = []
					compare_list = Parallel(n_jobs=num_cores)(delayed(launch_process)(folder_original, image, name, folder_images) for image in os.listdir('./original_images/{}/{}'.format(folder_original, name)))

					file_results = open('{}/results.txt'.format(folder_images), "a")
					file_results.write('{}: {}\n'.format(name, sum(compare_list)/(len(compare_list)-compare_list.count(0.0))))
					file_results.close()
					compare_total += sum(compare_list)/(len(compare_list)-compare_list.count(0.0))
				file_results = open('{}/results.txt'.format(folder_images), "a")
				file_results.write('\n{}: total {}\n'.format(compare_total, folder_images))
				file_results.close()

