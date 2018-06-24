import os
import math
# os.chdir("D:\kuaipan_auto\software_develop\python_TNM2MNM")
from collections import OrderedDict

# import shapefile
import numpy as np
import copy


networkName = 'SR41'
numOfInterval = 20
curInt = 1

class Node:
	ID = None
	x = None
	y = None
	nodeType = None
	def __init__(self, record):
		words = record.split()
		self.ID = int(words[0])
		self.nodeType = words[1]
		self.x = int(words[2])
		self.y = int(words[3])


class Link:
	from1 = None
	to1	  = None
	ID = None
	linkType = None
	name = None
	length = np.float(0)
	FFS = np.float(0)
	cap = np.float(0)
	RHOJ = np.float(0)
	lane = None
	hasCounts = False
	hasSpeed = False
	volume = np.float()
	lambda_plus = np.float()
	lambda_minus = np.float()
	v = np.float()
	u = np.float()
	def __init__(self, re):
		words = re.split()
		self.ID = int(words[0])
		self.linkType = words[1]
		self.name = words[2]
		self.from1 = int(words[3])
		self.to1 = int(words[4])
		self.length = np.float32(words[5])
		self.FFS = np.float64(words[6])
		self.cap = np.float64(words[7])
		self.RHOJ = np.float64(words[8])
		self.lane = int(words[9])
	def isConnector(self):
		return int(self.RHOJ > 9999)

linkFileName = networkName + ".lin"
nodeFileName = networkName + ".nod"

linkLog = file(linkFileName).readlines()[1:]
nodeLog = file(nodeFileName).readlines()[1:]

LinkDic = OrderedDict()
for line in linkLog:
	e = Link(line)
	LinkDic[e.ID] = e

NodeDic = OrderedDict()
for line in nodeLog:
	n = Node(line)
	NodeDic[n.ID] = n


MNMFileFolder = "input_files/"
MNMFileName = "MNM_input_"

MNM_link_file = file(MNMFileFolder + MNMFileName + "link", "w")
MNM_link_file.write("#ID Type LEN(mile) FFS(mile/h) Cap(v/hour) RHOJ(v/miles) Lane\n")

MNM_node_file = file(MNMFileFolder + MNMFileName + "node", "w")
MNM_node_file.write("#ID Type\n")

MNM_od_file = file(MNMFileFolder + MNMFileName + "od", "w")
#Origin_ID <-> node_ID
#Dest_ID <-> node_ID

MNM_demand_file = file(MNMFileFolder + MNMFileName + "demand", "w")
MNM_demand_file.write("# Origin_ID Destination_ID, <demand_array>\n")

MNM_graph_file = file(MNMFileFolder + "Snap_graph", "w")
MNM_graph_file.write("# EdgeId	FromNodeId	ToNodeId\n")

OriginDic = dict()
DestDic = dict()
OriginNodeList = list()
DestNodeList = list()

num_link = 0
for linkID, link in LinkDic.iteritems():
	from_ID_ = link.from1
	to_ID_ = link.to1
	if link.linkType == "DMOLK":
		OriginDic[from_ID_] = to_ID_
		OriginNodeList.append(to_ID_)
		del NodeDic[from_ID_]
	elif link.linkType == "DMDLK":
		DestDic[to_ID_] = from_ID_
		DestNodeList.append(from_ID_)
		del NodeDic[to_ID_]
	elif link.linkType == "PQULK":
		str1 = " ".join([str(e) for e in [link.ID, "PQ", 1, 99999, 9999999, 99999, 1]]) + "\n"
		MNM_link_file.write(str1)
		str2 = " ".join([str(e) for e in [link.ID, link.from1, link.to1]]) + "\n"
		MNM_graph_file.write(str2)
		num_link = num_link + 1
	elif link.linkType == "LWRLK":
		str1 = " ".join([str(e) for e in [link.ID, "CTM", link.length, link.FFS, link.cap, link.RHOJ, link.lane]]) + "\n"
		MNM_link_file.write(str1)
		str2 = " ".join([str(e) for e in [link.ID, link.from1, link.to1]]) + "\n"
		MNM_graph_file.write(str2)
		num_link = num_link + 1
	else:
		print "ERROR! No such type :", link.linkType

MNM_link_file.close()
MNM_graph_file.close()


for nodeID, node in NodeDic.iteritems():
	if nodeID in OriginNodeList and nodeID in DestNodeList:
		print "ERROR! One node in two list"
	elif nodeID in OriginNodeList:
		node_type = "DMOND"
	elif nodeID in DestNodeList:
		node_type = "DMDND"
	else:
		node_type = "FWJ"
	str1 = " ".join([str(e) for e in [node.ID, node_type]]) + "\n"
	MNM_node_file.write(str1)

MNM_node_file.close()


MNM_od_file.write("#Origin_ID <-> node_ID\n")
for Origin_ID, node_ID in OriginDic.iteritems():
	str1 = " ".join([str(e) for e in [Origin_ID, node_ID]]) + "\n"
	MNM_od_file.write(str1)

MNM_od_file.write("#Dest_ID <-> node_ID\n")
for Dest_ID, node_ID in DestDic.iteritems():
	str1 = " ".join([str(e) for e in [Dest_ID, node_ID]]) + "\n"
	MNM_od_file.write(str1)

MNM_od_file.close()


TotInt = 20
dmdinNumofLine = 1

Origin2DestinationTotDemand = OrderedDict()
ODLog = file(networkName + '.odp', 'r').readlines()
for index, line in enumerate(ODLog):
	if line.startswith('Origin'):
		words = line.split()
		O = int(words[1])
		numOfD = int(words[2])
		Origin2DestinationTotDemand[O] = OrderedDict()
		for i in range(numOfD):
			words = ODLog[index + i + 1].split()
			D = int(words[0])
			Origin2DestinationTotDemand[O][D] = np.float(words[2])

scaleLog = file(networkName + '.dmd', 'r')
line = scaleLog.readline()
Origin2DestinationDmd = copy.deepcopy(Origin2DestinationTotDemand)
while line:
	if line.startswith('Origin'):
		from1 = int(line.split()[1])
	if line.startswith('Dest'):
		to1 = int(line.split()[1])
		scale = np.zeros((TotInt,1))
		g_count = 0
		for i in range(dmdinNumofLine):
			line = scaleLog.readline()
			temp_array = np.array(line.split(), dtype='float64') 
			scale[g_count:g_count+len(temp_array), 0] = temp_array
			g_count += len(temp_array)
		# scale = np.reshape(scale, (TotInt,1))
		Origin2DestinationDmd[from1][to1] = scale
	line = scaleLog.readline()


pair_counter = 0
for O in Origin2DestinationDmd.keys():
	for D in Origin2DestinationDmd[O].keys():
		# temp_list = [O, D] + np.asarray(Origin2DestinationDmd[O][D])
		temp_list = [O, D] + (Origin2DestinationDmd[O][D].T)[0].tolist()
		str1 = " ".join([str(e) for e in temp_list]) + "\n"
		MNM_demand_file.write(str1)
		pair_counter = pair_counter + 1

MNM_demand_file.close()

print "num_of_link", num_link
print "num_of_node", len(NodeDic)
print "num_of_O:", len(OriginDic)
print "num_of_D:", len(DestDic)
print "num_of_OD_pair:", pair_counter