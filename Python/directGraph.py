#FILE: directGraph.py
#AUTHOR: Akshay Nagendra <akshaynag@gatech.edu>
#DESCRIPTION: Python file intended to generate a directed graph from an input file ("input_graph.txt")
#REQUIREMENT: Need an input file called "input_graph.txt"
#See "input_graph.txt" for an example


import turtle 
import re
import math
import random

INPUT_GRAPH_FILENAME = "input_graph.txt"
GLOBAL_SKEW = 200
CANVAS_WIDTH = 800
CANVAS_HEIGHT = 800
SCREEN_WIDTH = 2000
SCREEN_HEIGHT = 2000
SBUFFER = 20
EBUFFER = 40
Y_STEP = 100
	
p = turtle.Turtle()
pScreen = p.getscreen()
pScreen.setup(width=CANVAS_WIDTH, height=CANVAS_HEIGHT, startx=None, starty=None)
pScreen.register_shape("Circle.gif")
pScreen.screensize(SCREEN_WIDTH,SCREEN_HEIGHT)
p.shape("Circle.gif")

LEFT_X = -(CANVAS_WIDTH/2)+100
RIGHT_X = (CANVAS_WIDTH/2)-100
TOP_Y = (CANVAS_HEIGHT/2)-100
DOWN_Y = -(CANVAS_HEIGHT/2)+100

class Node:
	def __init__(self):
		self.prev = []
		self.next = []
		self.nodeID = -1
		self.circuitNets = []
		self.X = -1
		self.Y = -1
	def printNodeInfo(self):
		print "NODE ", self.nodeID
		print "Nets Contained: ", self.circuitNets
		print "Ancestors: ", self.prev
		print "Successors: ", self.next
		if self.X != -1 and self.Y != -1:
			print "X: ",self.X
			print "Y: ",self.Y

class Edge:
	"""Class to track edge objects between nodes"""
	def __init__(self):
		self.srcNode = None
		self.dstNode = None
		self.startX = -1
		self.startY = -1
		self.endX = -1
		self.endY = -1
	def printEdgeInfo(self):
		print "EDGE FROM NODE ", self.srcNode.nodeID, " TO NODE ", self.dstNode.nodeID
		print "Starting Coordinate: (", self.startX, ", ", self.startY, ")"
		print "Ending Coordinate: (", self.endX, ", ", self.endY, ")"

		
def retrieveNodeByID(nList,ID):
	"""Helper function for retreiving a node by ID"""
	for i in range(0,len(nList)):
		if nList[i].nodeID == ID:
			return nList[i]
			
def edgeGenerator(srcNode,dstNode):
	"""Helper function for creating a list of edges for all nodes"""
	e = Edge()
	e.srcNode = srcNode
	e.dstNode = dstNode
	e.startX = srcNode.X 
	e.startY = srcNode.Y
	e.endX = dstNode.X
	e.endY = dstNode.Y
	return e
			
def arrowGenerator(cursor,e,sBuffer,eBuffer):
	"""Helper function to generate directional arrows between ancestors and successors"""
	cursor.penup()
	cursor.goto(e.startX,e.startY - sBuffer)
	cursor.pendown()
	distance = math.hypot(e.endX - e.startX,e.endY - e.startY) - eBuffer
	headAngle = math.atan2(e.endY-e.startY,e.endX-e.startX)
	headAngle = math.degrees(headAngle)
	cursor.seth(headAngle)
	cursor.forward(distance)
	cursor.stamp()

def nodePlacement(srcNodeList,nodeMasterList):
	"""Helper function to generate placement of all nodes"""
	X_SRC_STEP = (RIGHT_X-LEFT_X)/len(srcNodeList)
	
	nodesToBePlaced = nodeMasterList[:]
	nodesPlaced = []

	
	#Place Source Nodes
	for i in range(0,len(srcNodeList)):
		SRCX = 0;
		if (i%2 == 0):
			SRCX = LEFT_X+X_SRC_STEP*(math.floor(i/2))
		else:
			SRCX = RIGHT_X-X_SRC_STEP*(math.floor(i/2))
		nodesToBePlaced.pop(nodesToBePlaced.index(srcNodeList[i]))
		srcNodeList[i].X = SRCX
		srcNodeList[i].Y = TOP_Y
		nodesPlaced.append(srcNodeList[i])
		
	CUR_Y = TOP_Y
	#Place all next nodes
	while nodesToBePlaced:
		toBePlaced = []
		for n in nodesToBePlaced:
			readyToPlace = True
			for pr in n.prev:
				np = retrieveNodeByID(nodeMasterList,pr)
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
		
			if (X_SRC_STEP > GLOBAL_SKEW):
				SRCX = SRCX + random.randint(-GLOBAL_SKEW,GLOBAL_SKEW)

			nodesToBePlaced.pop(nodesToBePlaced.index(toBePlaced[i]))
			toBePlaced[i].X = SRCX
			toBePlaced[i].Y = CUR_Y
			nodesPlaced.append(toBePlaced[i])
	
	return nodesPlaced
	
def drawNodes(cursor,nList):
	"""Helper function to draw the nodes onto the screen"""
	for n in nList:
		cursor.penup()
		cursor.goto(n.X,n.Y)
		cursor.stamp()
		cursor.penup()
		cursor.goto(n.X,n.Y-10)
		cursor.write(n.nodeID,True,align="center",font=("Times New Roman",20,"normal"))
	
def verifyPlacement(nList):
	"""Helper function designed to verify placement of nodes onto screen for a directed acyclic graph (DAG)"""
	CONFLICT = True
	while(CONFLICT):
		CONFLICT = False
		
	
	
	
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

p.ht()
p.penup()



# X_SRC_STEP = (RIGHT_X-LEFT_X)/len(sourceNodeList)

# nodesToBePlaced = nodeList[:]
# nodesPlaced = []

# #Place Source Nodes
# for i in range(0,len(sourceNodeList)):
	# SRCX = 0;
	# if (i%2 == 0):
		# SRCX = LEFT_X+X_SRC_STEP*(math.floor(i/2))
	# else:
		# SRCX = RIGHT_X-X_SRC_STEP*(math.floor(i/2))
	# p.goto(SRCX,TOP_Y)
	# p.stamp()
	# p.penup()
	# p.goto(SRCX,TOP_Y-10)
	# p.write(sourceNodeList[i].nodeID,True,align="center",font=("Times New Roman",20,"normal"))
	# p.penup()
	# nodesToBePlaced.pop(nodesToBePlaced.index(sourceNodeList[i]))
	# sourceNodeList[i].X = SRCX
	# sourceNodeList[i].Y = TOP_Y
	# nodesPlaced.append(sourceNodeList[i])

# CUR_Y = TOP_Y
# #Place all next nodes
# while nodesToBePlaced:
	# toBePlaced = []
	# for n in nodesToBePlaced:
		# readyToPlace = True
		# for pr in n.prev:
			# np = retrieveNodeByID(nodeList,pr)
			# if np not in nodesPlaced:
				# readyToPlace = False
		# if readyToPlace:
			# toBePlaced.append(n)
	# #Begin placement

	# CUR_Y -= Y_STEP
	# X_STEP = (RIGHT_X-LEFT_X)/len(toBePlaced)
	# for i in range(0,len(toBePlaced)):
		# SRCX = 0
		# if (i%2==0):
			# SRCX = LEFT_X+X_SRC_STEP*(math.floor(i/2))
		# else:
			# SRCX = RIGHT_X-X_SRC_STEP*(math.floor(i/2))
		
		# if (X_SRC_STEP > GLOBAL_SKEW):
			# SRCX = SRCX + random.randint(-GLOBAL_SKEW,GLOBAL_SKEW)
		# p.goto(SRCX,CUR_Y)
		# p.stamp()
		# p.penup()
		# p.goto(SRCX,CUR_Y-10)
		# p.write(toBePlaced[i].nodeID,True,align="center",font=("Times New Roman",20,"normal"))
		# p.penup()
		# nodesToBePlaced.pop(nodesToBePlaced.index(toBePlaced[i]))
		# toBePlaced[i].X = SRCX
		# toBePlaced[i].Y = CUR_Y
		# nodesPlaced.append(toBePlaced[i])

# nodeList = nodesPlaced
# for nPlaced in nodeList:
	# nPlaced.printNodeInfo()

#do placement
nodeList = nodePlacement(sourceNodeList,nodeList)	
#verify placement
#draw nodes
drawNodes(p,nodeList)	

#Switch turtle back to arrow
p.shape("classic")
p.st()

edgeList = []
for srcNode in nodeList:
	for n in srcNode.next:
		nextNode = retrieveNodeByID(nodeList,n)
		e = edgeGenerator(srcNode,nextNode)
		arrowGenerator(p,e,SBUFFER,EBUFFER)
		edgeList.append(e)
		
		
turtle.done()