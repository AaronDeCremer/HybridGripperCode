#  Copyright (c) 2023. [Bart Engelen, Aaron De Cremer, Joren Crabbé]

import socket
import serial
import time
import numpy as np
from datetime import date

class Gripper:
    """
    This class is here for future work, to integrate the gripper into an existing system with vision detection for example
    """
    def __init__(self, gripperIP ="192.168.10.150", port = 1010):
        """
        intialises the connection with the robot
        :param gripperIP: This parameter initializes the gripperIP
        :param printInfo: TRUE = print extra information ; FALSE = run code without printing
        :param taskPort : The ethernetport used for communicating the robot tasks
        :param daqPort : The ethernetport used for daq
        """
        self.gripper_send = socket.socket(socket.AF_INET, socket.SOCKs_STREAM)
        self.gripper_send.connect((gripperIP, port))

        """serial connection with arduino"""
        ser = serial.Serial('COM3', 9600)

    def close(self):
        """
        closes the existing socket connection
        """
        self.gripper_send.close()

    def timestamp(self):
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

    def conf(self, data, endOfLine = '&'):
        """
        sends confirmation of connection/readiness to robot
        :param: data = int value 1 or 0
        :param: endOfLine = end of line command 
        """
        self.gripper_send.send((data + endOfLine).encode('utf-8'))
  


    def receive(self):
        """
        listens to robot to receive command for gripper control
        :return: The datastring received from the robot
        """
        outputString = self.gripper_send.recv(3).decode('utf-8')
        timelable = self.timestamp()
        return outputString, timelable
    
    """ write function to define gripper behaviour """

    def gripperOpen(self, opening, suctionheight):
        """
        Uses data of object to pick to calculate motor distances and to send it over to the Arduino
        Opening is the distance the gripper should be opened, or with other words the width of the object
        suctionheight is the height the suction cup should be at to pick an object.

        Note: If this code is to be used with an Arduino, multithreading should be used so the motor doesn't stutter.
        Use gripper control instead if to run without using this class.
        """
        ser = serial.Serial('COM3', 9600)
        while True:
            data = ser.readline().decode().strip()  # Read one line of serial data
            if data:  # Check if there is any data
                print(f"Received data from Arduino: {data}")

            opening = int(opening)
            distancemotor1 = opening
            distancemotor2 = suctionheight

            if (data == "Enter motor distances seperated by a comma: motor1,motor2") or (data == "Enter Next Move Values (0,0 for reset):"):
                "Check if Arduino is ready to receive instructions"
                if 0 < int(distancemotor1) < 135 and 0 < int(distancemotor2) < 135:
                    # Send the distances to the Arduino via serial. This also limits the distance to 135 for safety
                    ser.write(f"{opening},{suctionheight}\n".encode())
                    print("Distances sent to Arduino.")
                    print(opening)
                    print(suctionheight)

                else:
                    "If distances are invalid, both motors will return to home position"
                    print("distances invalid, exceeding limit")
                    ser.write(f"{0},{0}\n".encode())


