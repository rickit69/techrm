#!/usr/bin/python
# -*- coding: utf-8 -*-


import bluetooth
import argparse
import sys


"""   Examples

python test.py -dn BT1 -cl "mth&?"				reads temperature and humidity

python test.py -dn BT1 -cl "rdt&?"				reads date and hour

python test.py -dn BT1 -cl "sf&14/01/20?"			reads the size of the specified file  (format:  yy/mm/dd)

python test.py -dn BT1 -cl "rf&14/01/20?"			reads the specified file  (format:  yy/mm/dd)

python test.py -dn BT1 -cl "tsd&?"				reads the type of SD card

python test.py -dn BT1 -cl "ssd&?"				reads the size of SD card

python test.py -dn BT1 -cl "fm&?"				reads the size of free memory in microcontroller

python test.py -dn BT1 -cl "sdt&2014&01&21&14&30&00?"		sets date and hour (format:  yyyy, mm, dd, hh, mm, ss)

python test.py -dn BT1 -cl "rbt&?"				reads battery voltage

"""


target_diz = {"BT1" : "00:13:04:10:06:20"}    # put here the names and addresses of your Bluetooth devices

parser = argparse.ArgumentParser()

parser.add_argument('-dn', '--device_name', help = "the name of the Bluetooth device")
parser.add_argument('-cl', '--command_line', help = "the command line")

 
  
try:
  args = parser.parse_args()
except argparse.ArgumentError, e:
  print "Error %d: %s" % (e.args[0],e.args[1])
  sys.exit(1)

if args.device_name == None:
  print "\nYou must put the name of the Bluetooth device\n"
  sys.exit(1)

if args.command_line == None:
  print "\nYou must put the command line\n"
  sys.exit(1)
  
  
device_name =  args.device_name
command_line = args.command_line
  
 
def read_data(sock):
  data = ""
  while(1):
    char = sock.recv(1)
    data = data + char
    if char == "#": break
  return data
 

 
bd_addr = target_diz[device_name]
port = 1 


 
sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )

sock.connect((bd_addr, port))

select_command = command_line.split("&")[0]
#print device_name, bd_addr, command_line, select_command


if select_command == "sf":
  sock.send(command_line)
  print read_data(sock)
elif select_command == "rf":
  cmd_size = "sf&" + command_line.split("&")[1]
  sock.send(cmd_size)
  size = read_data(sock).replace("*","").replace("#", "").strip()
  cmd = command_line.replace("?", "&0&") + size + "?"
  sock.send(cmd)
  print read_data(sock)
elif select_command == "mth":
  sock.send(command_line)
  print read_data(sock)
elif select_command == "rdt":
  sock.send(command_line)
  print read_data(sock)
elif select_command == "tsd":
  sock.send(command_line)
  print read_data(sock)
elif select_command == "ssd":
  sock.send(command_line)
  print read_data(sock)
elif select_command == "fm":
  sock.send(command_line)
  print read_data(sock)
elif select_command == "sdt":
  sock.send(command_line)
elif select_command == "rbt":
  sock.send(command_line)
  print read_data(sock)
  
  
sock.close()



