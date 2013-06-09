#!/usr/bin/python
# -*- coding: utf-8 -*-

import urllib
import sys
import argparse


parser = argparse.ArgumentParser()
parser.add_argument('-c', '--command',  type=str, help = "the command")
parser.add_argument('-y', '--year',  type=str, help = "year (yy)")
parser.add_argument('-m', '--month',  type=str, help = "month (mm)")
parser.add_argument('-d', '--day',  type=str, help = "day (dd)")
parser.add_argument('-v', dest='verbose', help = "verbose", action='store_true')

try:
  args = parser.parse_args()
except argparse.ArgumentError, e:
  print "Error %d: %s" % (e.args[0],e.args[1])
  sys.exit(1)

if args.command == None:
  print "\nPlease specify the command\n"
  sys.exit(1)
 
if args.year == None:
  print "\nPlease specify the year (yy)\n"
  sys.exit(1)
  
if args.month == None:
  print "\nPlease specify the month (mm)\n"
  sys.exit(1)
  
if args.day == None:
  print "\nPlease specify the day (dd)\n"
  sys.exit(1)



command = args.command
year = args.year
month = args.month
day = args.day



cmd = "%s&%s/%s/%s?" % (command, year, month, day)






txt = urllib.urlopen("http://192.168.0.3/%s" % (cmd)).read()


print txt.replace("\n-*\n", "")



