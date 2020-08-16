'''
*****************************************************************************************
*
*        		===============================================
*           		Rapid Rescuer (RR) Theme (eYRC 2019-20)
*        		===============================================
*
*  This script is to implement Task 4 of Rapid Rescuer (RR) Theme (eYRC 2019-20).
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


# Team ID:			[ For e.g., 9999 ]
# Author List:		[ Comma separated list of author names ]
# Filename:			task_4.py
# Functions:		connect_to_server, send_to_receive_from_server, find_new_path, create_combination_dict,
# 					python_client, take_input_for_reposition_restart, service_shutdown
# 					[ Comma separated list of functions in this file ]
# Global variables:	SERVER_IP, SERVER_PORT, SERVER_ADDRESS, sock
# 					[ List of global variables defined in this file ]


# Import necessary modules
# Do not import any other modules
import cv2
import socket
import sys
import os
from collections import defaultdict, Counter
import signal, threading


# IP address of robot-server (ESP32)
SERVER_IP = '192.168.4.1'
nn=3

# Port number assigned to server
SERVER_PORT = 3333
SERVER_ADDRESS = (SERVER_IP, SERVER_PORT)

# global 'sock' variable for socket connection
sock = None


def connect_to_server(SERVER_ADDRESS):

	"""
	Purpose:
	---
	the function creates socket connection with server
	Input Arguments:
	---
	`SERVER_ADDRESS` :	[ tuple ]
		port address of server
	Returns:
	---
	`sock` :	[ object of socket class ]
		object of socket class for socket communication
	Example call:
	---
	sock = connect_to_server(SERVER_ADDRESS)
	"""

	global sock
	sock = None

	#############  Add your Code here   ###############
	
	sock = socket.socket()
	sock.connect(SERVER_ADDRESS)

	###################################################

	return sock


def send_to_receive_from_server(sock, data_to_send):

	"""
	Purpose:
	---
	the function sends / receives data to / from server in proper format
	Input Arguments:
	---
	`sock` :	[ object of socket class ]
		object of socket class for socket communication
	`string`	:	[ string ]
		data to be sent from client to server
	Returns:
	---
	`sent_data` :	[ string ]
		data sent from client to server in proper format
	`recv_data` :	[ string ]
		data sent from server to client in proper format
	Example call:
	---
	sent_data, recv_data = send_to_receive_from_server(sock, shortestPath)
	"""

	sent_data = ''
	recv_data = ''

	#############  Add your Code here   ###############
	sent_data = str(data_to_send)
	sent_data = '#'+sent_data+'#\0\0'
	sock.send(str.encode(sent_data))

	
	recv_data = sock.recv(1024)
	recv_data = recv_data.decode("utf-8")
	recv_data = recv_data + '\0'
	

	###################################################

	return sent_data, recv_data


def find_new_path(recv_data, shortestPath):

	"""
	Purpose:
	---
	the function computes new shortest path from cell adjacent to obstacle to final_point
	Input Arguments:
	---
	`recv_data` :	[ string ]
		data sent from server to client in proper format
	`shortestPath`	:	[ list ]
		list of coordinates of shortest path from initial_point to final_point
	Returns:
	---
	`obstacle_coord` :	[ tuple ]
		position of dynamic obstacle in (x,y) coordinate
	`new_shortestPath` :	[ list ]
		list of coordinates of shortest path from new_initial_point to final_point
	`new_initial_point` :	[ tuple ]
		coordinate of cell adjacent to obstacle for the new shortest path
	`img` :	[ numpy array ]
	Example call:
	---
	obstacle_coord, new_shortestPath, new_initial_point, img = find_new_path(recv_data, shortestPath)
	"""

	obstacle_coord = ()
	new_shortestPath = []
	new_initial_point = ()

	global img_file_path, final_point, no_cells_height, no_cells_width

	#############  Add your Code here   ###############
	
	###################################################

	return new_shortestPath


#############	You can add other helper functions here		#############

def send_to_server(sock, data_to_send):

	sent_data = ' '
	sent_data = str(data_to_send)
	sent_data = '#'+sent_data+'#\0\0'
	sock.send(str.encode(sent_data))


def receive_from_server(sock):

	recv_data = ''
	recv_data = sock.recv(1024)
	recv_data = recv_data.decode("utf-8")
	recv_data = recv_data + '\0'
	return recv_data

def next_fz(n,combination):
	# sends the cordinates of next firezone
	global nn
	if(nn==1):
		return (9,9) , combination
	if(nn==3):
		nn=nn-1
		return (6,9) , combination
	if(nn==2):
		nn = nn-1
		return (8,6) , combination
	

#########################################################################


# NOTE:	YOU ARE NOT ALLOWED TO MAKE ANY CHANGE TO THIS FUNCTION
def create_combination_dict(combination_digits, combination_locations):

	"""
	Purpose:
	---
	the function takes digits and locations and returns appropriate dictionary for combination chosen,
	it handles the existence of two duplicate keys (digits) in a dictionary
	Input Arguments:
	---
	`combination_digits` :	[ list of integers ]
		list of digits chosen in the combination of Sum
	`combination_locations`	:	[ list of tuples ]
		list of locations in maze image of digits chosen in the combination of Sum
	Returns:
	---
	`combination` :	[ dict ]
		appropriate dictionary for the combination of digits chosen for the Sum,
		it handles the existence of two duplicate keys (digits) in a dictionary
	Example call:
	---
	combination = create_combination_dict(combination_digits, combination_locations)
	"""

	combination = defaultdict(list)
	loc = 0

	combination_digits_count = Counter(combination_digits)

	for digit in combination_digits:
		if combination_digits_count[digit] > 1:
			combination[digit].append(combination_locations[loc])
		else:
			combination[digit] = combination_locations[loc]
		loc = loc + 1
	
	combination.default_factory = None
	combination = dict(combination)

	return combination


# NOTE:	YOU ARE ALLOWED TO MAKE CHANGE TO THIS FUNCTION ONLY WHERE SPECIFIED BELOW
def python_client():

	try:

		curr_dir_path = os.getcwd()
		img_dir_path = curr_dir_path + '/../Maze_Image/'		# path to directory of 'Maze_Image'

		img_file_path = img_dir_path + '/Task4_maze.jpg'		# path to 'Task4_maze.jpg' image file


		# Importing task_1a and image_enhancer script
		try:

			task_1a_dir_path = curr_dir_path
			sys.path.append(task_1a_dir_path)

			import task_1a
			import image_enhancer

			# changing the 'CELL_SIZE' variable to 40 x 40 pixels in accordance with the size in image
			task_1a.CELL_SIZE = 40

		except Exception as e:
			print('\n[ERROR] task_1a.py or image_enhancer.pyc file is missing from Task 1A folder !\n')
			exit()

		# Read the image and find the shortest path
		try:
			original_binary_img = task_1a.readImage(img_file_path)
			height, width = original_binary_img.shape

		except AttributeError as attr_err:
			print('\n[ERROR] readImage function is not returning binary form of original image in expected format !\n')
			exit()

		no_cells_height = int(height/task_1a.CELL_SIZE)					# number of cells in height of maze image
		no_cells_width = int(width/task_1a.CELL_SIZE)					# number of cells in width of maze image
		
		try:

			print('\n============================================')

			# Create socket connection with server
			try:
				sock = connect_to_server(SERVER_ADDRESS)

				if sock == None:
					print('\n[ERROR] connect_to_server function is not returning socket object in expected format !\n')
					exit()

				else:
					print('\nConnecting to %s Port %s' %(SERVER_ADDRESS))

			except ConnectionRefusedError as connect_err:
				print('\n[ERROR] the robot-server.c file is not executing, start the server first !\n')
				exit()
			
			# Send the 'digits_list' and 'combination_of_digits' to robot
			digits_list = [8, 0, 2, 2]
			
			combination_digits = [2, 2]
			combination_locations = [(6,9), (8,6)]

			combination = create_combination_dict(combination_digits, combination_locations)

			print('\nGiven Digits in image = %s \n\nGiven Combination of Digits with Locations = %s' % (digits_list, combination))

		except Exception:
			pass

		#############  NOTE: Edit this part to complete the Task 4 implementation   ###############
			
		# below data should be of type string string
		recv_data = ''
		new_shortpath = ''
		hosp_vac = 4 # hospital vacancie

		# Sending digit_list and combination
		send_to_server(sock, digits_list)
		send_to_server(sock, combination)

		# for esp to send @started@
		recv_data = receive_from_server(sock)
			
		# starting (4,4) end ist fireszone (6,9). runs only the first time
		if(recv_data[0:9] == "@started@"):
				
			cord_start = (4,4)
			cord_stop, combination  = next_fz(hosp_vac,combination) # Tells the next fire zone or exit cordinates

			new_shortpath = task_1a.solveMaze(original_binary_img,cord_start,cord_stop,no_cells_height,no_cells_width)
			send_to_server(sock,new_shortpath)
			
		while (1):
			recv_data = receive_from_server(sock)
			if(recv_data[:-1]!= "@HA reached, Task accomplished!@"):
				n = int(recv_data[3])
				bot_vac = hosp_vac - n

				cord_start = recv_data[5:10]
				cord_start=(new_shortpath[-2][0],new_shortpath[-2][1])
				cord_stop,combination = next_fz(bot_vac,combination)
					
				new_shortpath = task_1a.solveMaze(original_binary_img,cord_start,cord_stop,no_cells_height,no_cells_width)
				send_to_server(sock, new_shortpath)
					
			else:
				sock.close()
				sys.exit()
				break

			

			##########################################################################################
	
	except KeyboardInterrupt:

		sys.exit()


# NOTE:	YOU ARE ALLOWED TO EDIT THIS FUNCTION
def take_input_for_reposition_restart():

	global sock

	try:

		while True:

			data_to_send = input('\nEnter either "%" for Restart OR "&" for Reposition: \n')

			if (data_to_send == '%') or (data_to_send == '&'):
				
				if (data_to_send == '%'):

					print('\nOne Restart for the run is taken !')

					sent_data = str(data_to_send)
					sock.sendall(sent_data.encode())

					print('\nClosing Socket')

					sock.close()

					sys.exit()

				elif (data_to_send == '&'):

					print('\nOne Reposition for the run is taken !')
				
					sent_data = str(data_to_send)
					sock.sendall(sent_data.encode())

					#############  NOTE: Edit this part to complete the Task 4 implementation   ###############
										

					##########################################################################################
			
			else:

				print('\nYou must enter either "%" OR "&" only !')

	except KeyboardInterrupt:
		
		print('\nClosing Socket')
		sock.close()

		sys.exit()


# NOTE:	YOU ARE NOT ALLOWED TO MAKE ANY CHANGE TO THIS CLASS
class ServiceExit(Exception):
	"""
	Custom exception which is used to trigger the clean exit
	of all running threads and the main program.
	"""
	pass
 

# NOTE:	YOU ARE NOT ALLOWED TO MAKE ANY CHANGE TO THIS FUNCTION
def service_shutdown(signum, frame):
	print('Caught signal %d' % signum)
	raise ServiceExit


# NOTE:	YOU ARE NOT ALLOWED TO MAKE ANY CHANGE TO THIS FUNCTION
#
# Function Name:	main
# Inputs:			None
# Outputs: 			None
# Purpose: 			the function creates two independent Threads, one for running the Python Client program,
# 					second to take the user input for Restart ( " % " ) or Reposition ( " & " ) as per the Rulebook

if __name__ == '__main__':

	try:

		# Register the signal handlers
		signal.signal(signal.SIGTERM, service_shutdown)
		signal.signal(signal.SIGINT, service_shutdown)
		
		# creating thread
		t1 = threading.Thread(target=python_client, name='python_client')
		t2 = threading.Thread(target=take_input_for_reposition_restart, name='take_input_for_reposition_restart')

		t1.shutdown_flag = threading.Event()
		t2.shutdown_flag = threading.Event()

		# starting thread 1
		t1.start()
		# starting thread 2
		t2.start()
		
	except ServiceExit:

		t1.shutdown_flag.set()
		t2.shutdown_flag.set()

		# wait until thread 1 is completely executed
		t1.join(1)
		# wait until thread 2 is completely executed
		t2.join(1)

		# both threads completely executed
		print("Done!")