import numpy as np
import os

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

def read_output(total_inverval, path):
	output = dict()
	link_id_list = list()
	f = file(path + "record/MNM_output_record_interval_volume", 'r')
	line = f.readline()
	words = line.split()
	num_link = len(words)
	for str_link_id in words:
		link_id = int(str_link_id)
		output[link_id] = np.zeros(total_inverval)
		link_id_list.append(link_id)
	line = f.readline()
	counter = 0
	while line:
		words = line.split()
		for idx, str_link_volume in enumerate(words):
			output[link_id_list[idx]][counter] = np.float(str_link_volume)
		counter = counter + 1
		line = f.readline()
	if (counter != total_inverval):
		print "Potential error"
	f.close()
	return output

def get_link_dic(path):
	linkDic = dict()
	link_log = file(path + "Philly.lin", "r").readlines()[1:]
	for line in link_log:
		e = Link(line)
		if e.linkType == "LWRLK":
			linkDic[e.ID] = e
	return linkDic

def get_matrix(link_dict, output_dict, total_inverval):
	output_matrix = np.zeros((len(link_dict), total_inverval + 1))
	for idx, link_id in enumerate(link_dict.keys()):
		output_matrix[idx][0] = link_id
		output_matrix[idx, 1:total_inverval+1] = output_dict[link_id] / (link_dict[link_id].RHOJ * np.float(link_dict[link_id].lane) * link_dict[link_id].length)
	return output_matrix


def read_results(total_inverval, path):
	link_dict = get_link_dic(path)
	output_dict = read_output(total_inverval, path)
	results = get_matrix(link_dict, output_dict, total_inverval)
	return results

def rewrite_conf(request, conf_name):
	f = file(conf_name + "new", "w")
	conf_log = file(conf_name).readlines()
	for line in conf_log:
		change_flag = False
		for trigger, value in request.iteritems():
			if line.startswith(trigger):
				print "changing:", trigger
				change_flag = True
				f.write(str(trigger) + " = " + str(value).strip("\n\t") + "\n")
		if not change_flag:
			f.write(line)

def replace_conf(conf_name):
	new_conf_name = conf_name + "new"
	if os.path.exists(new_conf_name):
		os.remove(conf_name)
	os.rename(new_conf_name, conf_name)


def modify_conf(request, path):
	conf_name = path + "config.conf"
	rewrite_conf(request, conf_name)
	replace_conf(conf_name)


def run_MNM(path):
	os.system("." + path + "dta_response")
##################################################
#######				main    			##########
##################################################

def get_DNL_results(params):
	# total_inverval = params["total_inverval"]
	total_inverval = 60
	start_interval = 20
	request = dict()
	path = "/"
	request["max_interval"] = total_inverval
	request["start_assign_interval"] = start_interval
	modify_conf(request, path)
	run_MNM(path)
	results = read_results(total_inverval, path)
	return results

params_local = dict()
# # params_local["total_inverval"] = 167
# # params_local["start_assign_interval"] = 0
a = get_DNL_results(params_local)
# np.savetxt("a.txt", a)
# # linkDic = dict()
# match_file = file("match_file", "w")
# link_log = file("Philly.lin", "r").readlines()[1:]
# for line in link_log:
# 	e = Link(line)
# 	if e.linkType == "LWRLK":
# 		linkDic[e.ID] = e
# for link in linkDic.itervalues():
# 	print link.ID
# 	match_file.write(" ".join([str(e) for e in [link.ID, link.name]]) + "\n") 



