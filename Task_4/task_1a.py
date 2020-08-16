
'''
*****************************************************************************************
*
*        		===============================================
*           		Rapid Rescuer (RR) Theme (eYRC 2019-20)
*        		===============================================
*
*  This script is to implement Task 1A of Rapid Rescuer (RR) Theme (eYRC 2019-20).
*  
*  This software is made available on an "AS IS WHERE IS BASIS".
*  Licensee/end user indemnifies and will keep e-Yantra indemnified from
*  any and all claim(s) that emanate from the use of the Software or 
*  breach of the terms of this agreement.
*  
*  e-Yantra - An MHRD project under National Mission on Education using ICT (NMEICT)
*
*****************************************************************************************
'''


# Team ID:			1662
# Author List:		Sujith Krishna,Thiruvikkraman,Kaushek Kumar,Arnab Das
# Filename:			task_1a.py
# Functions:		readImage, solveMaze
# 					Path,astar
# Global variables:	CELL_SIZE
# 					

# Import necessary modules
# Do not import any other modules
import cv2
import numpy as np
import os


# To enhance the maze image
import image_enhancer


# Maze images in task_1a_images folder have cell size of 20 pixels
CELL_SIZE = 20


def readImage(img_file_path):

	"""
	Purpose:
	---
	the function takes file path of original image as argument and returns it's binary form

	Input Arguments:
	---
	`img_file_path` :		[ str ]
		file path of image

	Returns:
	---
	`original_binary_img` :	[ numpy array ]
		binary form of the original image at img_file_path

	Example call:
	---
	original_binary_img = readImage(img_file_path)

	"""

	binary_img = None

	#############	Add your Code here	###############
	binary_img =  cv2.imread(img_file_path)
	_,binary_img = cv2.threshold(binary_img, 128, 255, cv2.THRESH_BINARY)
	binary_img = binary_img[:,:,0]
	###################################################
	
	return binary_img


def solveMaze(original_binary_img, initial_point, final_point, no_cells_height, no_cells_width):

	"""
	Purpose:
	---
	the function takes binary form of original image, start and end point coordinates and solves the maze
	to return the list of coordinates of shortest path from initial_point to final_point

	Input Arguments:
	---
	`original_binary_img` :	[ numpy array ]
		binary form of the original image at img_file_path
	`initial_point` :		[ tuple ]
		start point coordinates
	`final_point` :			[ tuple ]
		end point coordinates
	`no_cells_height` :		[ int ]
		number of cells in height of maze image
	`no_cells_width` :		[ int ]
		number of cells in width of maze image

	Returns:
	---
	`shortestPath` :		[ list ]
		list of coordinates of shortest path from initial_point to final_point

	Example call:
	---
	shortestPath = solveMaze(original_binary_img, initial_point, final_point, no_cells_height, no_cells_width)

	"""
	
	shortestPath = []
	start = initial_point 
	#############	Add your Code here	###############
	#cv2.imshow("Binary img",original_binary_img)
	mat = np.zeros((2*no_cells_height -1 , 2*no_cells_width -1))
	(w,h) = original_binary_img.shape
	(w,h) = int((w/(2*no_cells_width))) , int(h/(2*no_cells_height))
	i = 0
	

	for y in range(h,(h*2*no_cells_height),h):
		j=0
		for x in range(w,(w*2*no_cells_width),w):
			mat[i,j] = int((255-original_binary_img[y,x])/255)
			j+=1
		i+=1


	start = (initial_point[0]*2,initial_point[1]*2)
	end = (2*final_point[0] , 2*final_point[1] )
	path = astar(mat, start, end)
	[shortestPath.append((int(x/2),int(y/2))) for (x,y) in path]


	###################################################
	
	return shortestPath


#############	You can add other helper functions here		#############
class Node():
	def __init__(self,parent=None,pos=None):
		self.parent = parent
		self.pos = pos
		self.g = 0
		self.h = 0
		self.f = 0

def Path(current_node,maze):
	path =[]
	current = current_node
	while current is not None:
		path.append(current.pos)
		current=current.parent
	path = path[::-1]

	return path


def astar(maze,start,stop):

	node_start = Node(None,start)
	node_goal  = Node(None,stop)
	open=[]
	close=[]
	open.append(node_start)
	
	while (len(open)>0):
		
		node_current = open[0]
		current_index = 0
		for index,item in enumerate(open):
			if (item.f < node_current.f) or ((item.f == node_current.f) and (item.h < node_current.h)):
				node_current=item
				current_index = index


		open.pop(current_index)
		close.append(node_current)


		if (node_current.pos == node_goal.pos):
			
			return Path(node_current,maze)


		children =[]
		for new_position in [(0,2), (2, 0), (0, -2),(-2, 0)]: 
			node_position = (node_current.pos[0] + new_position[0], node_current.pos[1] + new_position[1])
			if (node_position[0] > (len(maze) - 1) or node_position[0] < 0 or node_position[1] > (len(maze[len(maze)-1]) -1) or node_position[1] < 0):
				continue
			if (maze[node_position[0]][node_position[1]] != 0.) or (maze[node_position[0]-int(new_position[0]/2)][node_position[1]-int(new_position[1]/2)] != 0.):
				continue
			x= False
			for i in range(len(close)):
				if (close[i].pos == node_position):
					x= True
			if (x ==  True):
				continue
			new_node = Node(node_current,node_position)
			
			children.append(new_node)

		for child in children:

			if (len([close_child for close_child in close if close_child == child ])>0):
				continue
			child.g = node_current.g + 1
			child.h = ((child.pos[0] - node_goal.pos[0])**2) + ( ( child.pos[1] - node_goal.pos[1])**2)
			child.f = child.g + child.h

			if( len([i for i in open if child == i and child.g>i.g]) >0):
				continue
			open.append(child)

			



#########################################################################


# NOTE:	YOU ARE NOT ALLOWED TO MAKE ANY CHANGE TO THIS FUNCTION
# 
# Function Name:	main
# Inputs:			None
# Outputs: 			None
# Purpose: 			the function first takes 'maze00.jpg' as input and solves the maze by calling readImage
# 					and solveMaze functions, it then asks the user whether to repeat the same on all maze images
# 					present in 'task_1a_images' folder or not

if __name__ == '__main__':

	curr_dir_path = os.getcwd()
	img_dir_path = curr_dir_path + '/../task_1a_images/'				# path to directory of 'task_1a_images'
	
	file_num = 0
	img_file_path = img_dir_path + 'maze0' + str(file_num) + '.jpg'		# path to 'maze00.jpg' image file

	print('\n============================================')

	print('\nFor maze0' + str(file_num) + '.jpg')

	try:
		
		original_binary_img = readImage(img_file_path)
		height, width= original_binary_img.shape

	except AttributeError as attr_error:
		
		print('\n[ERROR] readImage function is not returning binary form of original image in expected format !\n')
		exit()
	
	no_cells_height = int(height/CELL_SIZE)							# number of cells in height of maze image
	no_cells_width = int(width/CELL_SIZE)							# number of cells in width of maze image
	initial_point = (0, 0)											# start point coordinates of maze
	final_point = ((no_cells_height-1),(no_cells_width-1))			# end point coordinates of maze

	try:

		shortestPath = solveMaze(original_binary_img, initial_point, final_point, no_cells_height, no_cells_width)
		if len(shortestPath) > 2:
			img = image_enhancer.highlightPath(original_binary_img, initial_point, final_point, shortestPath)
			
		else:

			print('\n[ERROR] shortestPath returned by solveMaze function is not complete !\n')
			exit()
	
	except TypeError as type_err:
		
		print('\n[ERROR] solveMaze function is not returning shortest path in maze image in expected format !\n')
		exit()

	print('\nShortest Path = %s \n\nLength of Path = %d' % (shortestPath, len(shortestPath)))
	
	print('\n============================================')
	
	cv2.imshow('canvas0' + str(file_num), img)
	cv2.waitKey(0)
	cv2.destroyAllWindows()

	choice = input('\nWant to run your script on all maze images ? ==>> "y" or "n": ')

	if choice == 'y':

		file_count = len(os.listdir(img_dir_path))

		for file_num in range(file_count):

			img_file_path = img_dir_path + 'maze0' + str(file_num) + '.jpg'

			print('\n============================================')

			print('\nFor maze0' + str(file_num) + '.jpg')

			try:
				
				original_binary_img = readImage(img_file_path)
				height, width = original_binary_img.shape

			except AttributeError as attr_error:
				
				print('\n[ERROR] readImage function is not returning binary form of original image in expected format !\n')
				exit()
			
			no_cells_height = int(height/CELL_SIZE)							# number of cells in height of maze image
			no_cells_width = int(width/CELL_SIZE)							# number of cells in width of maze image
			initial_point = (0, 0)											# start point coordinates of maze
			final_point = ((no_cells_height-1),(no_cells_width-1))			# end point coordinates of maze

			try:

				shortestPath = solveMaze(original_binary_img, initial_point, final_point, no_cells_height, no_cells_width)

				if len(shortestPath) > 2:

					img = image_enhancer.highlightPath(original_binary_img, initial_point, final_point, shortestPath)
					
				else:

					print('\n[ERROR] shortestPath returned by solveMaze function is not complete !\n')
					exit()
			
			except TypeError as type_err:
				
				print('\n[ERROR] solveMaze function is not returning shortest path in maze image in expected format !\n')
				exit()

			print('\nShortest Path = %s \n\nLength of Path = %d' % (shortestPath, len(shortestPath)))
			
			print('\n============================================')

			cv2.imshow('canvas0' + str(file_num), img)
			cv2.waitKey(0)
			cv2.destroyAllWindows()
	
	else:

		print('')


