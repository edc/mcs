def parse(fh, prefix=None):
	"""given a file hander fh (actually anything iterable), parse the SDF and
	return a graph representation accepted by the MCS functions.

	@param fh file handler. It can be anything with an iterator
	@param prefix optional prefix to name the node in graph. If not given, a
	random two-letter prefix will be generated
	"""

	if prefix is None:
		alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" + "abcdefghijklmnopqrstuvwxyz" 
		import random
		prefix = alphabet[int(random.random() * 26 * 2)] + \
					alphabet[int(random.random() * 26 * 2)]
	
	assert type(prefix) is str

	graph = dict()
	atoms = []
	v_label = dict()
	e_label = dict()

	line_cnt = -1
	for line in fh:
 		line_cnt += 1
		# skip first 3 lines
		if line_cnt < 3:
			continue
		# read counts
		if line_cnt == 3:
			n_atoms = int(line[:3].strip())
			n_bonds = int(line[3:6].strip())
			#print "%d atoms" % n_atoms
			continue
		# read labels
		if line_cnt <= 3 + n_atoms:
			# label is in the 4th column
			v_id = "%s%d" % (prefix, line_cnt - 4)
			atoms.append(v_id)
			v_label[v_id] = line.split()[3].strip()
			#print "node %s labeled %s" % (v_id, v_label[v_id])
			# create entries in adjacency table
			graph[v_id] = []
			continue
		# read edges
		if line_cnt <= 3 + n_atoms + n_bonds:
			(left, right, label) = (line[:3], line[3:6], line[6:9])
			left = atoms[int(left) - 1]
			right = atoms[int(right) - 1]
			label = label.strip()
			graph[left].append(right)
			graph[right].append(left)
			if left <= right: e_label[left + ":" + right] = label
			else: e_label[right + ":" + left] = label
			#print "edge %s:%s labeled %s" % (left, right, label)
	
	return (graph, v_label, e_label)

# map is built with a script for all elements with atomic number from 
# 1 to 111
map = \
{'Ru': 'r', 'Re': 'J', 'Rf': '\x86', 'Rg': '\x8d', 'Ra': 'u', 'Rb': 'R', 'Rn': 'j', 'Rh': '.', 'Be': 'b', 'Ba': '5', 'Bh': '\x89', 'Bi': '`', 'Bk': '\x7f', 'Br': '*', 'H': 'H', 'P': 'P', 'Os': 'o', 'Es': '\x81', 'Hg': ']', 'Ge': 'g', 'Gd': ';', 'Ga': 'G', 'Pr': '7', 'Pt': '[', 'Pu': '|', 'C': 'C', 'Pb': '_', 'Pa': 'x', 'Pd': 'p', 'Cd': '0', 'Po': 'd', 'Pm': '9', 'Hs': '\x8a', 'Ho': '=', 'Hf': '@', 'K': 'K', 'He': 'h', 'Md': '\x83', 'Mg': 'M', 'Mo': '-', 'Mn': 'm', 'O': 'O', 'Mt': '\x8b', 'S': 'S', 'W': 'W', 'Zn': 'Z', 'Zr': 'z', 'Ni': '&', 'No': '\x84', 'Na': '!', 'Nb': ',', 'Nd': '8', 'Ne': 'n', 'Np': '{', 'Fr': 'q', 'Fe': 'f', 'Fm': '\x82', 'B': 'B', 'F': 'F', 'Sr': '+', 'N': 'N', 'Kr': 'k', 'Si': 's', 'Sn': '1', 'Sm': ':', 'V': 'V', 'Sc': '#', 'Sb': '2', 'Sg': '\x88', 'Se': ')', 'Co': '%', 'Cm': '~', 'Cl': 'c', 'Ca': '"', 'Cf': '\x80', 'Ce': '6', 'Xe': 'X', 'Lu': '?', 'Cs': '4', 'Cr': '$', 'Cu': "'", 'La': 'l', 'Li': 'L', 'Tl': '^', 'Tm': '>', 'Lr': '\x85', 'Th': 'w', 'Ti': 'T', 'Te': '3', 'Tb': '<', 'Tc': 't', 'Ta': 'E', 'Yb': 'y', 'Db': '\x87', 'Dy': 'D', 'Eu': '\x8e', 'Er': '\x8f', 'Ds': '\x8c', 'I': 'I', 'U': 'U', 'Y': 'Y', 'Ac': 'v', 'Ag': '/', 'Ir': 'Q', 'Am': '}', 'Al': 'A', 'As': '(', 'Ar': 'a', 'Au': '\\', 'At': 'e', 'In': 'i'}

def convert(fh):
	g, v, e = parse(fh, '')
	n_atoms = len(v)
	n_edges = len(e)
	result = ''
	result += "%d %d\n" % (n_atoms, n_edges)
	v_labels = ''
	for i in range(n_atoms):
		k = v[str(i)]
		if len(k) == 1:
			v_labels = v_labels + " " + k
		elif k in map:
			v_labels = v_labels + " " + map[k]
		elif k[0] in '0123456789' and len(k) == 2:
			v_labels = v_labels + " " + k[1]
		else:
			v_labels = v_labels + " " + k[1]
			sys.stderr.write("%s unknown\n" % k)
	result += v_labels.strip() + "\n"
	for i in e:
		(l, r) = i.strip().split(":")
		result += "%s %s %s\n" % (l, r, e[i])
	
	return result

if __name__ == '__main__':
	import sys
	print convert(file(sys.argv[1]))
