#FILE: RWGUI.py
#AUTHOR: Akshay Nagendra <akshaynag@gatech.edu>
#DESCRIPTION: Python file intended to generate a directed graph from an input file that is specified by the cmdline, or the default will be used
#See "input_graph_lecture.dmp" for an example


import turtle 
import re
import math
import random
import Tkinter as tkinter
import time
import sys

#PARAMETERS THAT CAN BE CONFIGURED BY USER
INPUT_GRAPH_FILENAME = "input_graph_lecture.dmp" #DEFAULT FILE
CURSOR_SIZE = 50
GLOBAL_SKEW_X = 1.5*CURSOR_SIZE
GLOBAL_SKEW_Y = CURSOR_SIZE
CANVAS_DISPLAY_W = 850
CANVAS_DISPLAY_H = 850
CANVAS_WIDTH = 650
CANVAS_HEIGHT = 650
WINDOW_WIDTH = 1800
WINDOW_HEIGHT = 920
WINDOW_DIM = str(WINDOW_WIDTH) + "x" + str(WINDOW_HEIGHT)
SCREEN_WIDTH = 2000
SCREEN_HEIGHT = 2000
TEXT_BUFFER = 10
FONT_SIZE = 16
SBUFFER = 20
EBUFFER = 40
Y_STEP = 2.5*CURSOR_SIZE
DEGREE_FOR_CONFLICT = 9
SCROLLBAR_WIDTH = 16
NUM_CONSOLE_BUTTONS = 3
CONSOLE_WIDTH = CANVAS_DISPLAY_W + SCROLLBAR_WIDTH
CONSOLE_BUTTON_HEIGHT = WINDOW_HEIGHT - (CANVAS_DISPLAY_H + SCROLLBAR_WIDTH)
LABEL_WIDTH = int(1.25*FONT_SIZE)
COLUMN_HEADERS = ["NODE","SIGNAL NAME","NODE DELAY","LABEL","CLUSTER"]
NUM_COLUMNS_TABLE = len(COLUMN_HEADERS)
TABLE_ELEMENTS = []
TABLE_WIDTH = WINDOW_WIDTH - (CANVAS_DISPLAY_W + SCROLLBAR_WIDTH)
LSET = []
LSET_FONT = FONT_SIZE + 4


#GUI setup
root = tkinter.Tk()
root.title("RW Clustering GUI")
root.geometry(WINDOW_DIM)
drawingFrame = tkinter.Frame(root)
drawingFrame.place(x = 0, y = 0, width = CANVAS_DISPLAY_W+SCROLLBAR_WIDTH, height = CANVAS_DISPLAY_H+SCROLLBAR_WIDTH)
v = tkinter.Scrollbar(drawingFrame, orient=tkinter.VERTICAL)
h = tkinter.Scrollbar(drawingFrame,orient=tkinter.HORIZONTAL)

v.place(x = CANVAS_DISPLAY_W, y =0, width = SCROLLBAR_WIDTH, height = CANVAS_DISPLAY_H + SCROLLBAR_WIDTH)
h.place(x=0, y = CANVAS_DISPLAY_H, width = CANVAS_DISPLAY_W + SCROLLBAR_WIDTH, height = SCROLLBAR_WIDTH)
cv = tkinter.Canvas(drawingFrame,width=SCREEN_WIDTH,height=SCREEN_HEIGHT,yscrollcommand=v.set,xscrollcommand=h.set)
cv.place(x = 0, y = 0, width = CANVAS_DISPLAY_W, height = CANVAS_DISPLAY_H)

tableFrame = tkinter.Frame(root)
tableFrame.place(x = CANVAS_DISPLAY_W + SCROLLBAR_WIDTH, y = 0, width = TABLE_WIDTH, height = WINDOW_HEIGHT)


v.config(command = cv.yview)
h.config(command = cv.xview)
cv.config(scrollregion=cv.bbox(tkinter.ALL))


p = turtle.RawTurtle(cv)
pScreen = p.getscreen()
p.speed(0)
pScreen.register_shape("Circle.gif")
pScreen.screensize(SCREEN_WIDTH,SCREEN_HEIGHT)
p.shape("Circle.gif")

LEFT_X = -(CANVAS_WIDTH/2)+CURSOR_SIZE
RIGHT_X = (CANVAS_WIDTH/2)-CURSOR_SIZE
TOP_Y = (CANVAS_HEIGHT/2)-CURSOR_SIZE
DOWN_Y = -(CANVAS_HEIGHT/2)+CURSOR_SIZE

#Center canvas
cv.yview_scroll(-5,"units")
cv.xview_scroll(-5,"units")

class Node:
    def __init__(self):
        self.prevn = []
        self.nextn = []
        self.nodeID = -1
        self.delay = 1
        self.circuitNets = []
        self.X = -1
        self.Y = -1
        self.cluster = []
        self.label = 0
    def printNodeInfo(self):
        print "NODE ", self.nodeID
        print "Nets Contained: ", self.circuitNets
        print "Ancestors: ", self.prevn
        print "Successors: ", self.nextn
        print "Cluster(", self.nodeID, "): ", self.cluster
        print "Label: ", self.label
        print "Delay: ", self.delay
        if self.X != -1 and self.Y != -1:
            print "X: ",self.X
            print "Y: ",self.Y
    def printSummaryInfo(self):
        print "NODE ", self.nodeID

class Edge:
    """Class to track edge objects between nodes"""
    def __init__(self):
        self.srcNode = None
        self.dstNode = None
        self.startX = -1
        self.startY = -1
        self.endX = -1
        self.endY = -1
        self.angle = -1
    def printEdgeInfo(self):
        print "EDGE FROM NODE ", self.srcNode.nodeID, " TO NODE ", self.dstNode.nodeID
        print "Starting Coordinate: (", self.startX, ", ", self.startY, ")"
        print "Ending Coordinate: (", self.endX, ", ", self.endY, ")"
        print "Heading Angle: ",self.angle

		
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
    angle = math.atan2(e.endY-e.startY,e.endX-e.startX)
    e.angle = math.degrees(angle)
    return e
			
def arrowGenerator(cursor,e,sBuffer,eBuffer):
    """Helper function to generate directional arrows between ancestors and successors"""
    cursor.penup()
    cursor.goto(e.startX,e.startY - sBuffer)
    cursor.pendown()
    distance = math.hypot(e.endX - e.startX,e.endY - e.startY) - eBuffer
    cursor.seth(e.angle)
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
            for pr in n.prevn:
                np = retrieveNodeByID(nodeMasterList,pr)
                if np not in nodesPlaced:
                    readyToPlace = False
            if readyToPlace:
                toBePlaced.append(n)
	
        #Begin placement
        CUR_Y -= Y_STEP
        if len(toBePlaced) > 0:
            X_STEP = (RIGHT_X-LEFT_X)/len(toBePlaced)
        for i in range(0,len(toBePlaced)):
            SRCX = 0
            if (i%2==0):
                SRCX = LEFT_X+X_SRC_STEP*(math.floor(i/2))
            else:
                SRCX = RIGHT_X-X_SRC_STEP*(math.floor(i/2))

            if (X_SRC_STEP > GLOBAL_SKEW_X):
                SRCX = SRCX + random.randint(-GLOBAL_SKEW_X, GLOBAL_SKEW_X)
            if (Y_STEP > GLOBAL_SKEW_Y):
                CUR_Y += random.randint(-GLOBAL_SKEW_Y,GLOBAL_SKEW_Y)
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
        cursor.goto(n.X,n.Y-TEXT_BUFFER)
        cursor.write(n.nodeID,True,align="center",font=("Times New Roman",FONT_SIZE,"normal"))
	
def verifyPlacement(nList,eList):
    """Helper function designed to verify placement of nodes onto screen for a directed acyclic graph (DAG)"""
    for e in eList:
        for n in nList:
            if n.nodeID == e.srcNode.nodeID or n.nodeID == e.dstNode.nodeID:
                continue
            elif n.Y > CURSOR_SIZE + e.srcNode.Y or n.Y < e.dstNode.Y - CURSOR_SIZE or n.X > CURSOR_SIZE + max(e.srcNode.X,e.dstNode.X) or n.X < min(e.srcNode.X,e.dstNode.X) - CURSOR_SIZE:
                continue
            headAngle = math.atan2(n.Y - e.srcNode.Y, n.X - e.srcNode.X)
            headAngle = math.degrees(headAngle)
            if headAngle - DEGREE_FOR_CONFLICT <= e.angle <= headAngle + DEGREE_FOR_CONFLICT:
                #This means that the node would be hitting the edge
                #print "Node ", n.nodeID, " is in the edge from Node ", e.srcNode.nodeID, " to Node ", e.dstNode.nodeID
                #print "Angle Violation:\nEdge Angle: ", e.angle, "\nAngle to Violating Node: ", headAngle
                return True
    return False

def displayTable(frameTable,nList):
    #Populate table of information
    for i in range(len(nList)+1): #Rows
        for j in range(NUM_COLUMNS_TABLE): #Columns
            b = tkinter.Label(frameTable,relief=tkinter.RAISED)
            msg = ""
            fontStyle = ""
            if not i:
                fontStyle = "bold"
                msg = COLUMN_HEADERS.pop(0)
            else:
                if j == 0:
                    msg = nList[i-1].nodeID
                if j == 1:
                    msg = nList[i - 1].circuitNet
                if j == 2:
                    msg = nList[i-1].delay
                if j == 3:
                    msg = nList[i-1].label
                if j == 4:
                    msg = "{"
                    for c in nList[i-1].cluster:
                        msg += c + ","
                    msg = msg[0:len(msg)-1] + "}"
                fontStyle = "normal"
            b.config(font=("Times New Roman", FONT_SIZE, fontStyle))
            b.config(text=msg)
            b.grid(row=i, column=j,sticky=tkinter.N+tkinter.E+tkinter.S+tkinter.W)
            TABLE_ELEMENTS.append(b)
    lsetTitle = tkinter.Label(frameTable,relief=tkinter.RAISED,text="CURRENT L SET",font=("Times New Roman", LSET_FONT, "bold"))
    lsetTitle.grid(row=len(nList)+1,column=0,columnspan=5,sticky=tkinter.N+tkinter.E+tkinter.S+tkinter.W)
    lsetLabel = tkinter.Label(frameTable, relief=tkinter.RAISED,text=LSET,font=("Times New Roman", LSET_FONT, "normal"))
    lsetLabel.grid(row=len(nList)+2,column=0,columnspan=5,sticky=tkinter.N+tkinter.E+tkinter.S+tkinter.W)
    return lsetLabel

    
def DAGPLACER(srcNList,nList):
    count = 0
    needToPlace = True
    while (needToPlace):
        #do placement
        #print "Attempting Placement Iteration ", count
        nList = nodePlacement(srcNList,nList)
        edgeList = []
        for srcNode in nList:
            for n in srcNode.nextn:
                nextNode = retrieveNodeByID(nList,n)
                e = edgeGenerator(srcNode,nextNode)
                edgeList.append(e)
        #verify placement
        needToPlace = verifyPlacement(nList,edgeList)
        #if (needToPlace):
            #print "Placement Iteration ", count, " Failed"
        count += 1
        #cont = raw_input("Enter any key to continue")
    return (nList,edgeList,count-1)

def drawDAG():
    #WARNING: GLOBAL FUNCTION!!!!!
    #Drawing time
    p.ht()
    p.penup()
    p.shape("Circle.gif")
    drawNodes(p, nodeList)
    p.shape("classic")
    p.st()
    for e in edgeList:
        arrowGenerator(p, e, SBUFFER, EBUFFER)

def prevCallback():
    cv.delete("all")
    
#END OF FUNCTIONS
    
    
if len(sys.argv) > 1:
    INPUT_GRAPH_FILENAME = sys.argv[1]
print "Parsing graph.txt"
graphFile = open(INPUT_GRAPH_FILENAME,"r")
nodeList = []
edgeList = []
FLAG = ""

#Parse logic
for line in graphFile:
    #parse each line 
    line = line.strip()
    sO = re.search( r'//(.*)', line)
    if sO:
        FLAG = sO.group(1)
        continue
    if (FLAG == "NODES"):
        sO2 = re.search(r'(.*):(.*);(\d+);(.*);(.*);(.*);(.*)',line)
        if sO2:
            newNode = Node()
            newNode.nodeID = sO2.group(1).strip()
            newNode.circuitNet = sO2.group(2).strip()
            newNode.delay = sO2.group(3).strip()
            newNode.prevn = sO2.group(4).split()
            newNode.nextn = sO2.group(5).split()
            newNode.label = sO2.group(6).split()
            newNode.cluster = sO2.group(7).split()
            newNode.cluster.sort()
            nodeList.append(newNode)
    if (FLAG == "CLUSTERS"):
    	sO3 = re.search(r'^(.*):(.*);LSET:(.*)$',line)

sourceNodeList = []	
for n in nodeList:
    if not n.prevn:
        sourceNodeList.append(n)

LSET_LABEL = displayTable(tableFrame,nodeList)

start = time.time()

(nodeList,edgeList,count) = DAGPLACER(sourceNodeList,nodeList)

end = time.time()
print "SUMMARY:\nIterations Required For Placement: ", count, "\nElapsed Time (ms): ", str((end - start )*1000.0)

#console buttons
dagButton = tkinter.Button(root, text="DAG", command=drawDAG)
dagButton.place(x=0,y=CANVAS_DISPLAY_H+SCROLLBAR_WIDTH,width = CONSOLE_WIDTH/NUM_CONSOLE_BUTTONS,height = CONSOLE_BUTTON_HEIGHT)
prevButton = tkinter.Button(root,text="PREV",command=prevCallback)
prevButton.place(x=CONSOLE_WIDTH/NUM_CONSOLE_BUTTONS,y=CANVAS_DISPLAY_H+SCROLLBAR_WIDTH,width = CONSOLE_WIDTH/NUM_CONSOLE_BUTTONS,height = CONSOLE_BUTTON_HEIGHT)


#cv.create_oval(0,0,100,100)


#drawDAG(p,nodeList,edgeList)

#b = Button(master, text="DAG", command=drawDAG)

turtle.done()

root.mainloop()