#import from math
from math import * 
#import from rhinocommon
import Rhino as r
#import from ghpython
import ghpythonlib.components as ghc
import rhinoscriptsyntax as rs
import scriptcontext as sc
from Grasshopper.Kernel.Types import *


#Read Data from txt
path=str(input_path)
f=open(path)
filein=[]
input_polycord = []
input_polyedge = []
vertex_number = int(f.readline())
for i in range(vertex_number):
	input_polycord.append(map(float,f.readline().split(' ')))
filein = f.readline()
while filein:
	input_polyedge.append(map(int,filein.split(' ')))
	filein = f.readline()
f.close

#global variables
globalseed=generate

#math_functions needed
def distance(cords1,cords2):
	return sqrt(pow((cords1[0] - cords2[0]),2) + pow((cords1[1] - cords2[1]),2)\
	+ pow((cords1[2] - cords2[2]),2))
def angle(center,cords1,cords2):
	c = distance(center,cords1)*distance(center,cords2)
	if c == 0:
		return -1
	else:
		yourangle = acos(((cords1[0]-center[0])*(cords2[0]-center[0])\
			+(cords1[1]-center[1])*(cords2[1]-center[1]))/c)*180/pi
		return yourangle
def anglewise(vetx1,vetx2):
	anwise = angle(vetx1.vecord,vetx2.vecord,[abs(vetx1.vecord[0])+100,\
	vetx1.vecord[1],0])
	if vetx2.vecord[1]-vetx1.vecord[1]>=0:
		return anwise
	else:
		return -anwise
def random(yourlist,localseed):
	a = ghc.Random(seed=localseed)
	b=float(1)/float(len(yourlist))
	for i in range(len(yourlist)+1):
		c = i*b
		if a-c>=0:
			continue
		else:
			break
	return yourlist[i-1]
def randomrange(domain,localseed):
	a = abs(domain[1]-domain[0]) 
	b = ghc.Random(seed=localseed)
	return a*b+domain[0]
def combination2(yourlist,localseed): 
	i=localseed
	k=localseed+1
	m=random(range(len(yourlist)),i)
	n=random(range(len(yourlist)),k)  
	yourli=[0]*2
	while n==m:
		k=k+1
		n=random(range(len(yourlist)),k)
		pass
	yourli[0] = yourlist[m]
	yourli[1] = yourlist[n]
	return yourli
def combination3(yourlist,localseed):
	seed1=localseed+3
	seed2=localseed+4
	i = combination2(range(len(yourlist)),seed1)[0]
	j = combination2(range(len(yourlist)),seed1)[1]
	k = random(range(len(yourlist)),seed2)
	yourli=[0]*3
	while (k-i)*(k-j)==0:
		seed2 = seed2+1
		k = random(range(len(yourlist)),seed2)
		pass
	yourli[0] = yourlist[i]
	yourli[1] = yourlist[j]
	yourli[2] = yourlist[k]
	return yourli
#draw function needed
def drawline(cords1,cords2):
	return r.Geometry.Line(cords1[0],cords1[1],cords1[2],\
	cords2[0],cords2[1],cords2[2])
def drawcircle(cords,radius):
	a = r.Geometry.Point3d(cords[0],cords[1],cords[2])
	return r.Geometry.Circle(a,float(radius))
def drawpoint(cords):
	return r.Geometry.Point3d(cords[0],cords[1],cords[2])
def drawcirclepoint(circle,t=0):
	return r.Geometry.Circle.PointAt(circle,float(t))
def drawlinecircle(line,circle):
	return r.Geometry.Intersect.Intersection.LineCircle(line,circle)
def drawcontrolpointcurve(points,int=3):
	return r.Geometry.Curve.CreateControlPointCurve(points,degree=int)
#adjust polycord NO.
#for i in range(len(input_polyedge)):
	#input_polyedge[i][0] = input_polyedge[i][0] - 1
	#input_polyedge[i][1] = input_polyedge[i][1] - 1
#print input_polyedge
#print input_polycord
#adjust unit to human scale and meter
input_polyedgelength=[]
for i in range(len(input_polyedge)):
	input_polyedgelength.append(distance(input_polycord[input_polyedge[i][0]],\
	input_polycord[input_polyedge[i][1]]))

minlength = min(input_polyedgelength)
maxlength = max(input_polyedgelength)
multimes = float(maxlength)/float(maxroomlength)

for i in range(len(input_polycord)):
	input_polycord[i][0] = float(float(input_polycord[i][0])/float(multimes)+500)
	input_polycord[i][1] = float(-(input_polycord[i][1])/float(multimes))
	input_polycord[i][2] = float(0)

# adjust trueD if the edge length is too small
lenthre = minroomlength * multimes
for i in range(len(input_polycord)):
	if input_polyedgelength[i] < lenthre:
		input_polyedge[i][2] = 0
	else:
		pass
# adjust trueD if the edge length is too long
#for i in range(len(input_polycord)):
	#if input 

#po=r.Geometry.Circle.PointAt(drawcircle([0,0,0],30),-0.33*pi)
#po = drawcirclepoint(drawcircle([0,0,0],30),-100)
#ccc = drawcircle([500,0,0],20)
#bbb = drawline([500,0,0],[900,300,0])
#bbb2 = drawline([500,0,0],[900,0,0])
#aaa = drawlinecircle(bbb,ccc)
#ptt = drawpoint([0,0,0])
#ptt2 = drawpoint([0,100,0])
#ptt3 = drawpoint([300,0,0])
#curve = drawcontrolpointcurve([ptt,ptt2,ptt3])
#Polygon Vertexs 
class PolyVetx():
	def __init__(self, vetxid, vecord=[0,0,0]):
		self.vetxid = vetxid
		self.vecord = vecord
		self.edges = []  
		self.reald = []   
		self.egnum = []
		self.shapfree = []
		self.shapid = []
		self.shpegid = []
		self.edgelgh = []
#Polygon Edges
class PolyEdge():
	def __init__(self, edgeid, edgepr=[-1,-1,None],edgelgh=1):
		self.edgeid = edgeid
		self.edgepr = edgepr
		self.edgevt = edgepr[0:2] 
		self.trued = edgepr[2]
		self.reald = [-1]*len(input_polycord)
		self.edgelgh = edgelgh
#Store Objects of two Classes
polyvetx = [PolyVetx(i,input_polycord[i]) for i in range(len(input_polycord))]
polyedge = [PolyEdge(i,input_polyedge[i],\
	distance(polyvetx[input_polyedge[i][0]].vecord,\
	polyvetx[input_polyedge[i][1]].vecord)) for i in range(len(input_polyedge))]


#polygraph
polygraph=[]
for edge in polyedge:
	polygraph.append(drawline(polyvetx[edge.edgevt[0]].vecord,\
	polyvetx[edge.edgevt[1]].vecord))

#Put all edges that are connected to certain Polygon Vertex
#Change 1st and 2nd number if needed,to ensure  
#the first number is the center Polygon Vertex ID
for vetx in polyvetx:
	for edge in polyedge:
		if edge.edgevt[0] == vetx.vetxid:
			vetx.edges.append(edge.edgepr[:])
			vetx.egnum.append(edge.edgeid)
			vetx.edgelgh.append(edge.edgelgh)
		elif edge.edgevt[1] == vetx.vetxid:
			vetx.edges.append(edge.edgepr[:])
			vetx.edges[-1][1] = vetx.edges[-1][0]
			vetx.edges[-1][0] = vetx.vetxid
			vetx.egnum.append(edge.edgeid)
			vetx.edgelgh.append(edge.edgelgh)
#reald for edges
localseed1=globalseed*5
for edge in polyedge:
	localseed1=localseed1+1
	if edge.trued == 1:
		if polygraph[edge.edgeid].Length < lex*0.5:
			edge.reald = [0]*len(polyvetx)
		elif polygraph[edge.edgeid].Length > maxroomlength*0.5:
			edge.reald[edge.edgevt[0]] = 1
			edge.reald[edge.edgevt[1]] = 0
		elif ghc.Random(seed = localseed1*2) < 0.20:
			edge.reald = [0]*len(polyvetx)
		elif ghc.Random(seed = localseed1*2) < 0.6:
			edge.reald[edge.edgevt[0]] = 1
			edge.reald[edge.edgevt[1]] = 0
		else:
			edge.reald = [1]*len(polyvetx)
	elif distance(polyvetx[edge.edgevt[0]].vecord,\
	polyvetx[edge.edgevt[1]].vecord)>=lex:
		if ghc.Random(seed=localseed1)>0.5:
			edge.reald[edge.edgevt[0]] = 1
			edge.reald[edge.edgevt[1]] = 0
		else:
			edge.reald[edge.edgevt[0]] = 0
			edge.reald[edge.edgevt[1]] = 1
	else:
		edge.reald = [0]*len(polyvetx)
#reald for vertexs
for vetx in polyvetx:
	for i in range(len(vetx.egnum)):
		vetx.reald.append(polyedge[vetx.egnum[i]].reald[vetx.vetxid])

#shapefree and shape edges for vertexs
localseed2=globalseed*7
for vetx in polyvetx:
	localseed2=localseed2+1
	if sum(vetx.reald) == 0:
		vetx.shapfree.append(0)
	elif sum(vetx.reald) ==1:
		if ghc.Random()<0.1:
			vetx.shapfree.append(0)
		else:
			vetx.shapfree.append(1)
			for i in range(len(vetx.reald)):
				if vetx.reald[i] == 1:
					vetx.shpegid.append(i) 
	else:
		edgenum = range(len(vetx.egnum))
		vetx.shpegid = combination2(edgenum,localseed2)
		edge1 = vetx.edges[vetx.shpegid[0]]
		edge2 = vetx.edges[vetx.shpegid[1]]
		cords1 = polyvetx[edge1[1]].vecord
		cords2 = polyvetx[edge2[1]].vecord
		reangle = angle(vetx.vecord,cords1,cords2)
		if reangle <= 40:
			del vetx.shpegid[0:2]
			vetx.shapfree.append(0)
		elif reangle <=55 or reangle >=130:
			del vetx.shpegid[0]
			vetx.shapfree.append(1)
		else:
			vetx.shapfree.append(2)

#decide shapid and shapedge
localseed3=globalseed*9
for vetx in polyvetx:
	localseed3=localseed3+1
	if vetx.shapfree[0] == 0:
		vetx.shapid.append(0)
	elif vetx.shapfree[0] == 1:
		if ghc.Random(seed=localseed3)<0.2:
			vetx.shapid.append(0)
			del vetx.shpegid[0]
		else:
			vetx.shapid.append(1)
	else:
		r0 = ghc.Random(seed=localseed3)
		if r0<0.05:
			vetx.shapid.append(0)
			del vetx.shpegid[0:2]
		elif r0<0.1:
			vetx.shapid.append(1)
			r1 = random(vetx.shpegid,localseed3+7)
			vetx.shpegid = [r1]
		else:
			vetx.shapid.append(2)
			r2 = combination2(vetx.shpegid,localseed3+3)
			vetx.shpegid = r2

class Charge():
	def __init__(self, id, domain):
		self.id = id
		self.ptdomain = [domain[0],domain[1]]
		
chargedomain=[[0.15,0.19],[0.19,0.25],[0.25,0.275],[0.275,0.31],[0.31,0.33]]	
chargeid=[5,10,15,20,25]
chargelist = [Charge(chargeid[i],chargedomain[i]) for i in range(len(chargeid))]

#pick shpegid for vetx with a shapid of 0
localseed4 = globalseed*4+4
for vetx in polyvetx:
	localseed4=localseed4+1
	if vetx.shapid[0] == 0:
		if len(vetx.egnum) <= 3:
			vetx.shpegid = range(len(vetx.egnum))
		else:
			vetx.shpegid = combination3(range(len(vetx.egnum)),localseed4)
	else:
		pass

#PointCloud1
def PointCloud1(vetx,localseed):
	avlength=float(sum(vetx.edgelgh))/float(len(vetx.egnum))
	if avlength <2.5:
		radius = randomrange(chargelist[0].ptdomain,localseed)
		char = [5,5,5,5,10]
	elif avlength >=2.5 and avlength <3.5:
		radius = randomrange(chargelist[1].ptdomain,localseed)
		char = [10,10,10,10,5]
	elif avlength >=3.5 and avlength <4.5:
		radius = randomrange(chargelist[2].ptdomain,localseed)
		char = [10,15,15,15,20]
	elif avlength >=4.5 and avlength <5.5:
		radius = randomrange(chargelist[3].ptdomain,localseed)
		char = [15,20,20,20,25]
	else:
		radius = randomrange(chargelist[4].ptdomain,localseed)
		char = [25,25,25,25,20]
	circle = drawcircle(vetx.vecord,radius)
	angle = []
	pts = []
	multi = 0.004
	charge = combination3(char,localseed+5)
	charge = [x*multi for x in charge]
	for i in range(len(vetx.shpegid)):
		angle.append(anglewise(vetx,polyvetx[vetx.edges[vetx.shpegid[i]][1]])*pi/180)
		pts.append(drawcirclepoint(circle,angle[i]))
	if len(vetx.shpegid)==3:
		return [pts,charge]
	else:
		pts.append(drawcirclepoint(circle,ghc.Random(localseed+1)))
		return [pts,charge]
#PointCloud2:
def PointCloud2(vetx,localseed):
	avlength=float(sum(vetx.edgelgh))/float(len(vetx.egnum))
	if avlength <2.5:
		radius = randomrange(chargelist[0].ptdomain,localseed)
		char = [7,7,7,7,15]
	elif avlength >=2.5 and avlength <3.5:
		radius = randomrange(chargelist[1].ptdomain,localseed)
		char = [15,15,15,15,7]
	elif avlength >=3.5 and avlength <4.5:
		radius = randomrange(chargelist[2].ptdomain,localseed)
		char = [15,20,20,20,30]
	elif avlength >=4.5 and avlength <5.5:
		radius = randomrange(chargelist[3].ptdomain,localseed)
		char = [20,30,30,30,35]
	else:
		radius = randomrange(chargelist[4].ptdomain,localseed)
		char = [35,35,35,35,30]
	circle = drawcircle(vetx.vecord,radius)
	t = [localseed+3,localseed+7,localseed+9]
	pts = []
	multi = 0.006
	for i in range(len(t)):
		pts.append(drawcirclepoint(circle,t[i]))
	charge = combination3(char,localseed+5)
	charge = [x*multi for x in charge]
	return [pts,charge]
def OneLine1(vetx,localseed):
	multi = 0.0045
	#charge list
	charhead = [[20,15],[20,20],[20,20],[20,20],[25,25],[25,25],[25,25],\
	[30,20],[30,15],[30,25],[35,20],[35,15],[35,20]]
	charmiddle_1 = [[7.5,7.5,7.5],[10,10,10],[10,10,10],[7.5,7.5,10]]
	charmiddle_2 = [[7.5,10],[10,10],[12.5,12.5]]
	charmiddle_3 = [[5],[7.5]]
	charlast = [[35],[35],[30],[25],[25],[20],[20]]
	#linked edge and vetx
	edge = polyedge[vetx.egnum[vetx.shpegid[0]]]
	vetx2 = polyvetx[vetx.edges[vetx.shpegid[0]][1]]
	length = polygraph[edge.edgeid].Length
	#two main lists
	pts = []
	charge = []
	#pick effect domain according to edge openess(trued) 
	if edge.trued == 1:
		domain1 = [0.2*length,0.40*length]
		domain2 = [0.9,0.40*maxroomlength]
	else:
		domain1 = [0.2*length,0.30*length]
		domain2 = [0.9,0.40*maxroomlength]
	domain = [max([domain1[0],domain2[0]]),min([domain1[1],domain2[1]])]
	farpt = randomrange(domain,localseed)
	t = float(farpt)/float(length)
	center3 = [(1-t)*vetx.vecord[0]+t*vetx2.vecord[0],\
	(1-t)*vetx.vecord[1]+t*vetx2.vecord[1],0]
	center2 = [0.5*vetx.vecord[0]+0.5*center3[0],\
	0.5*vetx.vecord[1]+0.5*center3[1],0]
	# control circles
	r1 = randomrange([0.145,0.300],localseed+1)
	r2 = randomrange([0.02,0.1],localseed*2+1)
	r3 = randomrange([0.06,0.145],localseed*3+3)
	r4 = random([0.005,0.01,0.02,0.03,0.04,0.05],localseed*4+4)
	controlcircle1 = drawcircle(vetx.vecord,r1)
	controlcircle2 = drawcircle(center2,r2)
	controlcircle3 = drawcircle(center3,r3)
	center4 = drawcirclepoint(controlcircle1,t=localseed*4+4)
	controlcircle4 = drawcircle(center4,r4)
	#points and curve controlpoint
	controlpoint = drawcirclepoint(controlcircle2,t=localseed*5+5)
	pt1 = drawcirclepoint(controlcircle4,t=localseed*6+7)
	pt2 = drawcirclepoint(controlcircle1,t=localseed*7+1)
	pt6 = drawcirclepoint(controlcircle3,t=localseed*8+3)
	pts = pts + [pt1,pt2]
	# control curve
	controlcurve = drawcontrolpointcurve([pt2,controlpoint,pt6])
	# LineType: 6 points or 5 points or 4 points?
	if farpt > 1.8:
		count = 6
	elif farpt > 1.4:
		count = random([5,6],localseed*2)
	else:
		count = random([4,5,6],localseed*3+1)
	# divide curve for segments
	se = r.Geometry.Curve.DivideByCount(controlcurve,count-2,False)
	for i in range(count-3):
		pts.append(r.Geometry.Curve.PointAt(controlcurve,se[i]))
	pts.append(pt6)
	# chargevalue
	if len(pts) == 6:
		charge = charge + random(charhead,localseed*4+2)+\
		random(charmiddle_1,localseed*9+10)+random(charlast,localseed*10+3)
	elif len(pts) == 5:
		charge = charge + random(charhead,localseed*4+2)+\
		random(charmiddle_2,localseed*9+10)+random(charlast,localseed*10+3)
	elif len(pts) == 4:
		charge = charge + random(charhead,localseed*4+2)+\
		random(charmiddle_3,localseed*9+10)+random(charlast,localseed*10+3)
	charge = [x*multi for x in charge]
	return [pts,charge,[controlcurve],\
	[controlcircle1,controlcircle2,controlcircle3,controlcircle4]]
def TwoLine1(vetx,localseed):
	#variables
	multi = 0.003
	#two main lists to fill
	pts = []
	charge = []
	#charge list
	charhead = [[20],[25]]
	charlast = [[25],[20]]
	charmiddle_1 = [[35],[30]]
	charmiddle_2 = [[35,35],[30,30],[35,30],[30,35]] 
	charmiddle_3 = [[30,30,30],[35,35,35],[30,35,30]]
	charmiddle_4 = [[30,35,35,30],[30,35,30,30],[30,30,35,30],[35,35,35,35]]
	#linked 2 edges 2 vetxs
	edge_1 = polyedge[vetx.egnum[vetx.shpegid[0]]]
	vetx_1 = polyvetx[vetx.edges[vetx.shpegid[0]][1]]
	edge_2 = polyedge[vetx.egnum[vetx.shpegid[1]]]
	vetx_2 = polyvetx[vetx.edges[vetx.shpegid[1]][1]]
	edge_2 = polyedge[vetx.egnum[vetx.shpegid[1]]]
	edge_2 = polyedge[vetx.edges[vetx.shpegid[1]][1]]
	length_1 = polygraph[edge_1.edgeid].Length
	length_2 = polygraph[edge_1.edgeid].Length
	#pick effect domain according to edge openess(trued) 
	if edge_1.trued == 1:
		domain_1a = [0.20*length_1,0.35*length_1]
		domain_1b = [0.5,0.35*maxroomlength]
	else:
		domain_1a = [0.20*length_1,0.30*length_1]
		domain_1b = [0.5,0.30*maxroomlength]
	domain_1 = [max([domain_1a[0],domain_1b[0]]),min([domain_1a[1],domain_1b[1]])]
	if edge_2.trued == 1:
		domain_2a = [0.22*length_2,0.40*length_2]
		domain_2b = [1,0.40*maxroomlength]
	else:
		domain_2a = [0.22*length_2,0.35*length_2]
		domain_2b = [1,0.40*maxroomlength]
	domain_2 = [max([domain_2a[0],domain_2b[0]]),min([domain_2a[1],domain_2b[1]])]
	farpt_1 = randomrange(domain_1,localseed+1)
	farpt_2 = randomrange(domain_2,localseed*2+1)
	t_1 = float(farpt_1)/float(length_1)
	t_2 = float(farpt_2)/float(length_2)
	# start and end points
	pt1 = drawpoint([(1-t_1)*vetx.vecord[0]+t_1*vetx_1.vecord[0],\
	(1-t_1)*vetx.vecord[1]+t_1*vetx_1.vecord[1],0])
	pt6 = drawpoint([(1-t_2)*vetx.vecord[0]+t_2*vetx_2.vecord[0],\
	(1-t_2)*vetx.vecord[1]+t_2*vetx_2.vecord[1],0])
	# controlcurve
	controlcurve = drawcontrolpointcurve([pt1,drawpoint(vetx.vecord),pt6])
	controlcurve_guid = sc.doc.Objects.AddCurve(controlcurve)
	controlcurve_length = rs.CurveLength(controlcurve_guid)
	# count and segment 
	if controlcurve_length > 2.8:
		count = 6
	elif controlcurve_length > 2.1:
		count = random([5,6],localseed*3+1)
	elif controlcurve_length > 1.4:
		count = random([4,5,6],localseed*3+1)
	else:
		count = random([3,4,5,6],localseed*3+1)
	se = r.Geometry.Curve.DivideByCount(controlcurve,count-1,False)
	pts.append(pt1)
	for i in range(count-2):
		pts.append(r.Geometry.Curve.PointAt(controlcurve,se[i]))
	pts.append(pt6)
	# charge values
	if len(pts) == 6:
		charge = charge + random(charhead,localseed*5+1)+\
		random(charmiddle_4,localseed*6+1)+random(charlast,localseed*7+3)
	elif len(pts) == 5:
		charge = charge + random(charhead,localseed*5+2)+\
		random(charmiddle_3,localseed*6+2)+random(charlast,localseed*7+2)
	elif len(pts) == 4:
		charge = charge + random(charhead,localseed*5+3)+\
		random(charmiddle_2,localseed*6+3)+random(charlast,localseed*7+1)
	elif len(pts) == 3:
		charge = charge + random(charhead,localseed*5+4)+\
		random(charmiddle_1,localseed*6+4)+random(charlast,localseed*7)
	charge = [x*multi for x in charge]
	return [pts,charge,[controlcurve]]

#Main Drawing
chargepoint = []
chargevalue = []
pts = []
curve = []
circle = []
localseed4 = globalseed*4+1
for vetx in polyvetx:
	localseed4 = localseed4+1
	if vetx.shapid[0] == 0:
		if ghc.Random(seed=localseed4)<0.7:
			chargepoint = chargepoint + PointCloud1(vetx,localseed4)[0]
			chargevalue = chargevalue + PointCloud1(vetx,localseed4)[1]
		else:
			chargepoint = chargepoint + PointCloud2(vetx,localseed4)[0]
			chargevalue = chargevalue + PointCloud2(vetx,localseed4)[1]
	elif vetx.shapid[0] ==1:
		chargepoint = chargepoint + OneLine1(vetx,localseed4)[0]
		chargevalue = chargevalue + OneLine1(vetx,localseed4)[1]
		curve = curve + OneLine1(vetx,localseed4)[2]
		circle = circle +OneLine1(vetx,localseed4)[3]
	else:
		pass
		chargepoint =  chargepoint + TwoLine1(vetx,localseed4)[0]
		chargevalue =  chargevalue + TwoLine1(vetx,localseed4)[1]
		curve = curve + TwoLine1(vetx,localseed4)[2]
#print for test
for vetx in polyvetx:
	print vetx.vetxid,
	print vetx.shapid,
	print vetx.shpegid


