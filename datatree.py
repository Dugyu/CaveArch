#以上几行是程序运行所需的各种包，可能有多的，不过把这几行都写上肯定没有问题

import Rhino
import scriptcontext
import rhinoscriptsyntax as rs
import System.Collections.Generic as SCG
import clr

clr.AddReference("Grasshopper")

from Grasshopper import DataTree
from Grasshopper.Kernel.Data import GH_Path


#定义一个DataTree，此处的dataTree换成其他名字也可以
dataTree = DataTree[object]()

#以下为函数的定义部分
def AddToPath(l,dTree,pathRoot=''):
	if type(l)==list:
		for i in range(len(l)):
			if pathRoot !='':
				AddToPath(l[i],dTree,pathRoot+','+str(i))
			else:
				AddToPath(l[i],dTree,str(i))
	else:
		exec('path = GH_Path('+pathRoot+')')
		dTree.Add(l,path)
		


#以下为该函数的调用方法
#第一个参数为希望加到DataTree的列表名称
#第二个参数为希望加进去的DataTree的名称
#第三个参数为希望把这个列表加到DataTree的什么路径下，可以不写，即默认为根路径下，下面的前两行是等效的
#如果希望把整个列表加到0;1路径下，则可按照第三行那样写
AddToPath(yourlist1,datatree_a)
AddToPath(yourlist1,datatree_a,'')
AddToPath(yourlist2,datatree_b,'0,1')
