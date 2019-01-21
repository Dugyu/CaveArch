#以上几行是程序运行所需的各种包，可能有多的，不过把这几行都写上肯定没有问题

import Rhino
import scriptcontext
import rhinoscriptsyntax as rs
import System.Collections.Generic as SCG
import clr

clr.AddReference("Grasshopper")

from Grasshopper import DataTree
from Grasshopper.Kernel.Data import GH_Path


#Graft之后的DataTree的特点:若有路径x,y，则不可能有路径x

#用于合并两个子路径
def MergePath(paths,result,i,j):
	result.insert(j+1,result[i:j+1])
	del result[i:j+1]
	paths.insert(j+1,paths[i][0:-1])
	del paths[i:j+1]

#将DataTree转为List
def DataTreeToList(dTree=None):
	result = list(dTree.AllData())
	paths = []
	for i in dTree.Paths:
		paths.append(list(i))

	while len(result)!= 1:
		curLength = 0
		s = -1
		for i in range(len(paths)):
			if len(paths[i]) > curLength:
				curLength = len(paths[i])
				s = i
		t = s
		
		while t < len(paths) - 1 \
		and len(paths[s])==len(paths[t]) \
		and paths[s][0:-1] == paths[t+1][0:-1] \
		and paths[s][-1] != paths[t+1][-1]:
			t += 1
		MergePath(paths,result,s,t)	
	return result[0]


#调用方法
resultList = DataTreeToList(myDataTree)
