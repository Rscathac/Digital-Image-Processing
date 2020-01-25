from collections import deque
from imutils.video import VideoStream
import numpy as np
import argparse
import cv2
import imutils
import time

class KalmanFilter:
	kf = cv2.KalmanFilter(4, 2)
	kf.measurementMatrix = np.array([[1, 0, 0, 0], [0, 1, 0, 0]], np.float32)
	kf.transitionMatrix = np.array([[1, 0, 1, 0], [0, 1, 0, 1], [0, 0, 1, 0], [0, 0, 0, 1]], np.float32)
	kf.processNoiseCov = np.array([[1,0,0,0], [0,1,0,0], [0,0,1,0],[0,0,0,1]],np.float32) * 0.03
	def Estimate(self, coordX, coordY):
		''' This function estimates the position of the object'''
		measured = np.array([[np.float32(coordX)], [np.float32(coordY)]])
		self.kf.correct(measured)
		predicted = self.kf.predict()
		return predicted

def min_y_coord(cnt):
	y_list = []
	for c in cnt:
		y_list.append(c[0][1])
	
	return (sum(y_list)/len(y_list))


video_path = 'video/demo.mp4'

# Define the boundary of ball (BGR)
ballLower = (-30, 80, 70)
ballUpper = (15, 205, 205)

rimLower = (0, 43, 46)
rimUpper = (4, 205, 205)

pts = deque(maxlen = 64)
center = None
# Read the video stream
vs = cv2.VideoCapture(video_path)
time.sleep(2.0)
x, y, flag, curx, cury, cnt = 0, 0, 0, 0, 0, 0
backSub = cv2.createBackgroundSubtractorMOG2()
kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3,3))
lists = []
times = []
last_predict = []
last = []
last_rad = 0
last_rim_area = 0
last_rim_moment = (0, 0)
rim_fail = False
kfObj = KalmanFilter()
predictedCoords = np.zeros((2, 1), np.float32)
frame_cnt = 0
make_frame = []
made_cnt = 0
while True:
	frame_cnt += 1
	#print(frame_cnt)
	ret, frame = vs.read()
	if frame is None:
			break

	# Convert frame to HSV space & gray space
	frame = imutils.resize(frame, width=1000)
	cv2.imshow('Original', frame)
	hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
	gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

	output = frame.copy()
	circle_img = frame.copy()
	rim_img = frame.copy()

		
	# Apply color filter on basketball and rim
	mask = cv2.inRange(hsv, ballLower, ballUpper)
	rim = cv2.inRange(hsv, rimLower, rimUpper)
	   
	# Applay background substraction
	fgMask = backSub.apply(mask)
	fgMask = cv2.bilateralFilter(fgMask, 9, 75, 75)
	fgMask = cv2.morphologyEx(fgMask, cv2.MORPH_OPEN, kernel)
	fgMask = cv2.GaussianBlur(fgMask,(11,11), 0)
	cv2.imshow('background substraction', fgMask)
	fgMask = cv2.Canny(fgMask, 100, 300)
	#cv2.imshow('background substraction', fgMask)

	# Rim detection
	rims = cv2.findContours(rim.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
	rims = imutils.grab_contours(rims)
	rim_x, rim_y = 0, 0

	if len(rims) > 0:
		r = max(rims, key=cv2.contourArea)

		x, y, w, h = cv2.boundingRect(r)
		rim_x = int(x+w/2)
		rim_y = int(y+h/2)
		#print('rim coordinate',x+w, y+h)
		cv2.rectangle(rim_img, (x,y) , (x+w,y+h), (0,255,0),2)
		#cv2.rectangle(frame, (rim_x-10, rim_y), (rim_x+10, rim_y+1), (255, 0, 0), -1)
		#cv2.imshow('rim detection', rim_img)
		area = cv2.contourArea(r)
		M = cv2.moments(r)
		cx = int(M['m10']/M['m00'])
		cy = int(M['m01']/M['m00'])
		if last_rim_area != 0:
			if (cx - last_rim_moment[0]) <= -5 and abs(area - last_rim_area) >= 25 and abs(area - last_rim_area) < 30:
			
				rim_fail = True
		last_rim_moment = (cx, cy)
		last_rim_area = area
		#print(cx, cy, area)
		#print(rim_fail)
	
	# Circle detection
	contours, hierarchy = cv2.findContours(
		fgMask,
		cv2.RETR_LIST,
		cv2.CHAIN_APPROX_SIMPLE
	)

	centers = []
	radii = []
		
	for contour in contours:
		area = cv2.contourArea(contour)
		((x, y), rad) = cv2.minEnclosingCircle(contour)
		circle_area = 3.1415*rad*rad

		if circle_area < 55 or circle_area > 800 or area < 55 or area > 1000:
			continue
		#if circle_area < 100 or circle_area > 800 or area < 100 or area > 1000:
		#	continue
		if ((area - circle_area) / (circle_area))**2 > 0.1:
			continue
		br = cv2.boundingRect(contour)
		radii.append(br[2])

		cv2.circle(circle_img, (int(x), int(y)), int(rad), 255, 2)
		center = [int(x), int(y), int(rad)]
		centers.append(center)

	edge = cv2.Canny(gray, 30, 150)

	# Apply open operation on the basketball mask
	mask = cv2.erode(mask, None, iterations=2)
	mask = cv2.dilate(mask, None, iterations=3)

	# Basketball detection (use color)
	res = cv2.bitwise_and(frame,frame, mask = mask)
	#cv2.imshow('res' ,res)
	cnts = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
	cnts = imutils.grab_contours(cnts)
	cnt = min(cnts, key=min_y_coord)
	for c in range(len(times)):
		times[c] += 1
		
	if len(cnt > 0):

		((x, y), radius) = cv2.minEnclosingCircle(cnt)

		
		if radius > 0:
			
			center_flag = False
			
			#print(int(x), int(y), rim_x, rim_y, last)
			# No ball detected
			if centers == [] and len(last) != 0:
				#print('case 1')
				center_flag = True
				if ((last[-1][0] - int(x))**2 + (last[-1][1] - int(y))**2) < 300:
					if x > (rim_x - 15) and x < (rim_x + 15):
						if len(last) == 3:
							if last[2][0] > rim_x - 15 and last[2][0] < rim_x + 15:
								#print('y', y, 'rim', rim_y, last)
								if y > rim_y and last[0][1] < rim_y and last[1][1] < rim_y and last[2][1] > rim_y:

									if frame_cnt > 70 and not rim_fail:
										make_cnt+=1
										print('Field goal made:', made_cnt)
										frame_cnt = 0
									elif rim_fail:
										rim_fail = False
									frame_cnt = 0
					if len(last) < 3:				
						last.append((int(x), int(y)))
						last_rad = int(radius)
					else:
						last = last[1:]
						last.append((int(x), int(y)))
						last_rad = int(radius)
					
					predict = kfObj.Estimate(int(x), int(y))
					cv2.circle(frame, (int(x), int(y)), int(radius),(0, 255, 255), 2)
					cv2.circle(frame, (int(x), int(y)), 3, (0, 0, 255), -1)
					#cv2.circle(frame, (predict[0], predict[1]), int(last_rad), (255, 255, 0), 2)

				else:
					predict = kfObj.Estimate((last[len(last)-1][0]), (last[len(last)-1][1]))
					if len(last) == 3:
						last = last[1:]
					last.append((int(predict[0]), int(predict[1])))
					#cv2.circle(frame, (predict[0], predict[1]), int(last_rad), (255, 255, 0), 2)

			for point in centers:
				if ((point[0] - int(x))**2 + (point[1] - int(y))**2) < 100:
					#print('case 2')
					center_flag = True
					flag_index = 0
					# decide which cluster
					for index in range(len(lists)):
						length = len(lists[index])
						if (x - lists[index][length-1][0])**2 + (y - lists[index][length-1][1])**2 < 1000:
							lists[index].append(center)
							times[index] = 0
							flag_index = 1
							if length >5:
								del lists[index][0]
							break
					if flag_index == 0:
						list2 = []
						list2.append(center)
						lists.append(list2)
						times.append(int(0))
						
					
					# Find a new ball, reset the Kalman filter
					if len(last) > 0 and ((int(x)-last[-1][0])**2 + (int(y)-last[-1][1])**2) > 3000:
						last = []
						kfObj = KalmanFilter()
					
					if x > (rim_x - 20) and x < (rim_x + 20):
						if len(last) == 3:
							if last[2][0] > rim_x - 20 and last[2][0] < rim_x + 20:
								if y > rim_y and last[0][1] < rim_y and last[1][1] < rim_y and last[2][1] > rim_y:

									if frame_cnt > 80 and not rim_fail:
										made_cnt+=1
										print('Field goal made:', made_cnt)
										frame_cnt = 0
									elif rim_fail:
										rim_fail = False
									frame_cnt = 0


					if len(last) < 3:				
						last.append((int(x), int(y)))
						last_rad = int(radius)
					else:
						last = last[1:]
						last.append((int(x), int(y)))
						last_rad = int(radius)

					predict = kfObj.Estimate(int(x), int(y))
					cv2.circle(frame, (int(x), int(y)), int(radius),(0, 255, 255), 2)
					cv2.circle(frame, (int(x), int(y)), 3, (0, 0, 255), -1)
					break
					#cv2.circle(frame, (predict[0], predict[1]), int(last_rad), (255, 255, 0), 2)
			
			# No suitable circle detected
			if center_flag is False and len(last) > 0:
				#print('case 3')
				predict = kfObj.Estimate((last[-1][0]), (last[-1][1]))
				#predict[0] = last[-1][0]
				#predict[1] = last[-1][1]
				if predict[0] > (rim_x - 10) and predict[0] < (rim_x + 10):
					if len(last) == 3:
						if last[2][0] > rim_x - 10 and last[2][0] < rim_x + 10:
							if predict[1] > rim_y and last[0][1] < rim_y and last[1][1] < rim_y and last[2][1] > rim_y:

								if frame_cnt > 70 and not rim_fail:
									made_cnt += 1
									print('Field goal made:', made_cnt)
									frame_cnt = 0
								elif rim_fail:
									rim_fail = False
								frame_cnt = 0					
				
				if len(last) == 3:
					last = last[1:]
				last.append((int(predict[0]), int(predict[1])))		
				#cv2.circle(frame, (predict[0], predict[1]), int(last_rad), (255, 255, 0), 2)



	cnt_time = 0
	while cnt_time < len(times):
		#print('times', times[c])
		if times[cnt_time] >= 5:
			del times[cnt_time]
			del lists[cnt_time]
			cnt_time -= 1
		cnt_time += 1

		
	cnt_time = 0
	# loop over the set of tracked points
	for j in lists:
		if times[cnt_time] > 0:
			cnt_time += 1
			#continue
		for i in range(1, len(j)):
		 # if either of the tracked points are None, ignore them
			if j[i - 1] is None or j[i] is None:
				continue		
			if j[i - 1] == j[i]:
				continue
						
			#print(j)
			thickness = int(np.sqrt(64 / float(i + 1)) * 1.5)
			#cv2.line(frame, j[i - 1], j[i], (0, 0, 255), thickness)
						
	# show the frame to our screen
	#cv2.imshow("Frame", frame)
	key = cv2.waitKey(1) & 0xFF

	# if the 'q' key is pressed, stop the loop
	if key == ord("q"):
		break
	if key == ord("t"):
		time.sleep(30)
																																																										 
																																																														 
vs.release()
																																																																		 
# close all windows
cv2.destroyAllWindows()

