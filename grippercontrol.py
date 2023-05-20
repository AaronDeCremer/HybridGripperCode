#  Copyright (c) 2023. [Bart Engelen, Aaron De Cremer, Joren Crabbé]
from Gripper import *
import socket
import time
from datetime import date

gripperIP = "192.168.10.150"
port = 1005
"""
intialises the connection with the robot
:param gripperIP: This parameter initializes the gripperIP
:param printInfo: TRUE = print extra information ; FALSE = run code without printing
:param taskPort : The ethernetport used for communicating the robot tasks
:param daqPort : The ethernetport used for daq
"""
gripper_send = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
gripper_send.connect((gripperIP, port))

"""add connection to arduino"""


def close():
    """
    closes the existing socket connection
    """
    gripper_send.close()


def timestamp():
    """
    function to create timestamp to be send to robot for logging
    :return: formatted string with date and time
    """
    t = time.localtime()
    current_time = time.strftime("%H:%M:%S", t)
    dateNow = str(date.today())
    convTime = current_time[0] + current_time[1] + current_time[3] + current_time[4] + current_time[6] + \
               current_time[7]
    convDate = dateNow[2] + dateNow[3] + dateNow[5] + dateNow[6] + dateNow[8] + dateNow[9]

    return convDate + convTime


def conf(data, endOfLine='&'):
    """
    sends confirmation of connection/readyness to robot
    :param: data = int value 1 or 0
    :param: endOfLine = end of line command
    """
    gripper_send.send((data + endOfLine).encode('utf-8'))


def receive():
    """
    listens to robot to receive command for gripper control
    :return: The datastring received from the robot
    """
    outputString = gripper_send.recv(4).decode('utf-8')
    timelable = timestamp()
    return outputString, timelable


conf("1")

ser = serial.Serial('COM3', 9600)
suctionheight = 0
i = 0
"""
In the current state of the code, the robot only sends one value for the drive to move, so only module at the time was 
and can be tested using this code. If multiple modules or motors should be moved, change the code
 or use the Arduino code
"""
while True:
    print(receive())
    gripOpening = receive()
    print("received")
    first_string = gripOpening[0]
    RealGripOpening_string = first_string[:3]  # takes the first 3 characters of the string
    print("String =" + RealGripOpening_string)
    RealGripOpening_string = int(RealGripOpening_string)
    data = ser.readline().decode().strip()  # Read one line of serial data
    if data:  # Check if there is any data
        if 0 < int(RealGripOpening_string) < 135 and 0 < int(suctionheight) < 135:
            # Send the distances to the Arduino via serial
            ser.write(f"{RealGripOpening_string},{suctionheight}\n".encode())
            print("Distances sent to Arduino." + str(suctionheight) + "," + str(RealGripOpening_string))
            print("next loop")

        else:
            "If distances are invalid, both motors will return to home position"
            print("distances invalid, exceeding limit")
            ser.write(f"{0},{0}\n".encode())
            print("looped")
    i += 1
    if (i > 20):
        # automatically break the control loop after i * time.sleep(x) seconds
        break
    time.sleep(0.7)  # Delay so that each system has the time to proces their request
    # This is especially important for the slow serial connection of the Arduino
