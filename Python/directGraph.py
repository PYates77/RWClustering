#FILE: directGraph.py
#AUTHOR: Akshay Nagendra <akshaynag@gatech.edu>
#DESCRIPTION: Python file intended to generate a directed graph from an input file ("input_graph.txt")
#REQUIREMENT: Need an input file called "input_graph.txt"
#See "input_graph.txt" for an example


import turtle 
import re
import math

INPUT_GRAPH_FILENAME = "input_graph.txt"

class Node:
	def __init__(self):
		self.prev = [];
		self.next = [];
		self.nodeID = -1;
		self.circuitNets = [];
		self.X = -1;
		self.Y = -1;
	def printNodeInfo(self):
		print "NODE ", self.nodeID
		print "Nets Contained: ", self.circuitNets
		print "Ancestors: ", self.prev
		print "Successors: ", self.next
		if self.X != -1 and self.Y != -1:
			print "X: ",self.X
			print "Y: ",self.Y
		
def retrieveNodeByID(nList,ID):
	"""Helper function for retreiving a node by ID"""
	for i in range(0,len(nList)):
		if nList[i].nodeID == ID:
			return nList[i]
			
def arrowGenerator(cursor,srcNode,dstNode,sBuffer,eBuffer):
	"""Helper function to generate directional arrows between ancestors and successors"""
	cursor.penup()
	cursor.goto(srcNode.X,srcNode.Y - sBuffer)
	cursor.pendown()
	distance = math.hypot(dstNode.X - srcNode.X,dstNode.Y - srcNode.Y) - eBuffer
	if (dstNode.X == srcNode.X):
		cursor.seth(-90)
	else:
		headAngle = math.atan((dstNode.Y-srcNode.Y)/(dstNode.X-srcNode.X))
		headAngle = math.degrees(headAngle)
		if (dstNode.X < srcNode.X):
			cursor.seth(headAngle+180)
		else:
			cursor.seth(-headAngle)
	cursor.forward(distance)
	cursor.stamp()
	
	
	
print "Parsing graph.txt"
graphFile = open(INPUT_GRAPH_FILENAME,"r")
nodeList = []
FLAG = ""
delay = 1


for line in graphFile:
	#parse each line 
	line = line.strip()
	sO = re.search( r'//(.*)', line)
	if sO:
		FLAG = sO.group(1)
	if (FLAG == "NODES"):
		sO2 = re.search(r'(.*):(.*);(.*);(.*)',line)
		if sO2:
			newNode = Node()
			newNode.nodeID = sO2.group(1)
			newNode.circuitNets = sO2.group(2).split()
			newNode.prev = sO2.group(3).split()
			newNode.next = sO2.group(4).split()
			nodeList.append(newNode)
	if (FLAG == "DELAY"):
		delay = line

sourceNodeList = []	
for n in nodeList:
	if not n.prev:
		sourceNodeList.append(n) 	
SCREEN_WIDTH = 800
SCREEN_HEIGHT = 800
	
p = turtle.Turtle()
pScreen = p.getscreen()
pScreen.setup(width=SCREEN_WIDTH, height=SCREEN_HEIGHT, startx=None, starty=None)
pScreen.register_shape("Circle.gif")
pScreen.screensize(2000,2000)
p.shape("Circle.gif")

LEFT_X = -(SCREEN_WIDTH/2)+100
RIGHT_X = (SCREEN_WIDTH/2)-100
TOP_Y = (SCREEN_HEIGHT/2)-100
DOWN_Y = -(SCREEN_HEIGHT/2)+100

#sourceNodeList = [1,2,3,4,5,6,7]
X_SRC_STEP = (RIGHT_X-LEFT_X)/len(sourceNodeList)
Y_STEP = 150



p.ht()
p.penup()

nodesToBePlaced = nodeList[:]
nodesPlaced = []

#Place Source Nodes
for i in range(0,len(sourceNodeList)):
	SRCX = 0;
	if (i%2 == 0):
		SRCX = LEFT_X+X_SRC_STEP*(math.floor(i/2))
	else:
		SRCX = RIGHT_X-X_SRC_STEP*(math.floor(i/2))
	p.goto(SRCX,TOP_Y)
	p.stamp()
	p.penup()
	p.goto(SRCX,TOP_Y-10)
	p.write(sourceNodeList[i].nodeID,True,align="center",font=("Times New Roman",20,"normal"))
	p.penup()
	nodesToBePlaced.pop(nodesToBePlaced.index(sourceNodeList[i]))
	sourceNodeList[i].X = SRCX
	sourceNodeList[i].Y = TOP_Y
	nodesPlaced.append(sourceNodeList[i])

CUR_Y = TOP_Y
#Place all next nodes
while nodesToBePlaced:
	toBePlaced = []
	for n in nodesToBePlaced:
		readyToPlace = True
		for pr in n.prev:
			np = retrieveNodeByID(nodeList,pr)
			if np not in nodesPlaced:
				readyToPlace = False
		if readyToPlace:
			toBePlaced.append(n)
	#Begin placement

	CUR_Y -= Y_STEP
	X_STEP = (RIGHT_X-LEFT_X)/len(toBePlaced)
	for i in range(0,len(toBePlaced)):
		SRCX = 0
		if (i%2==0):
			SRCX = LEFT_X+X_SRC_STEP*(math.floor(i/2))
		else:
			SRCX = RIGHT_X-X_SRC_STEP*(math.floor(i/2))
		p.goto(SRCX,CUR_Y)
		p.stamp()
		p.penup()
		p.goto(SRCX,CUR_Y-10)
		p.write(toBePlaced[i].nodeID,True,align="center",font=("Times New Roman",20,"normal"))
		p.penup()
		nodesToBePlaced.pop(nodesToBePlaced.index(toBePlaced[i]))
		toBePlaced[i].X = SRCX
		toBePlaced[i].Y = CUR_Y
		nodesPlaced.append(toBePlaced[i])

nodeList = nodesPlaced
for nPlaced in nodeList:
	nPlaced.printNodeInfo()

#Switch turtle back to arrow
p.shape("classic")
p.st()

for srcNode in nodeList:
	for n in srcNode.next:
		nextNode = retrieveNodeByID(nodeList,n)
		arrowGenerator(p,srcNode,nextNode,20,40)
		
		
turtle.done()
