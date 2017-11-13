import serial # import Serial Library
import time
import numpy   # Import numpy

#######################################################  CORE  ############################################################

port='COM4'
baudrate = '115200'

print ("# connecting on port " + port + " at " + str(baudrate))
arduinoData = serial.Serial(port, baudrate) #Creating our serial object named arduinoData

#reset 
arduinoData.setDTR(1)
time.sleep(0.25)
arduinoData.setDTR(0)

Fs = 539000
Ts=1/Fs


def ReadVoltage():

	data=[] 
	dataArrayVoltage=[]
	time=numpy.arange(3,801)*Ts
	
	while arduinoData.inWaiting()==0: #Wait here until there is data
		pass #do nothing	

	while arduinoData.readline() != b'start\r\n': #Wait for sync byte
		pass #do nothing
	
	for i in range(1,801):
		arduinoRead = arduinoData.readline() #read the line of text from the serial port
		data.append(arduinoRead)

	for i in range(1,801):
		if data[i] != "":
			try:
				value=int(data[i])
			except:
				value=500
			voltage=(0.0048875855)*value
			dataArrayVoltage.append(voltage) #read the line of text from the serial port
					
	return dataArrayVoltage
	
	
#########################################   graphic interface    ########################################################

import matplotlib.pyplot as plt
import random
import Tkinter
import sys
 
def quitApp():
    plt.close()
    root.quit()
    sys.exit()
	
# def STOP():
# def interrupt():
    # counter.set(counter.get() + 1)
    # root.after(30, interrupt)
 
def START():
	start=1
	while start==1:
		plt.close()
		plt.ylim(-0.001,5.001)                                 #Set y min and max values
		plt.title('ARDUINO SCOPE')      #Plot the title
		plt.grid(True)  
		plt.xlabel('Time [Sec]') 	#Turn the grid on
		plt.ylabel('Voltage')     
		ax = plt.subplot(111)
		xdata = numpy.arange(2,800)
		y = ReadVoltage()
		ydata = y
		ax.plot(xdata, ydata)
		plt.show()
 
root = Tkinter.Tk()
root.geometry('%dx%d+%d+%d' % (300, 30, 500, 10))

Tkinter.Button(root, text='  START  ', command=START).grid()
#counter = Tkinter.IntVar()
#Tkinter.Label(root, textvariable=counter).grid()
root.protocol('WM_DELETE_WINDOW', quitApp)
#root.after(0, interrupt)
root.mainloop()
