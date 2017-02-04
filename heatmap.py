import matplotlib.pyplot as plt
import numpy as np
heat_map=open("heat_map.csv","w")
log_file=[]
unique_clients=[]
sender=[]
with open ("log.txt","r") as file:
	for line in file: 
	    l=line.strip('\n').split(',')
	    print l
	    log_file.append(l)
	    if l[1]=='0':
	        continue
	    unique_clients.append(l[1])
	    unique_clients.append(l[2])
	       
	    
unique_clients=list(set(unique_clients))

print unique_clients

#unique_chars=[0 for i in range(len(unique_clients))][0 for i in range(len(unique_clients))]

mat   =  [ [ 0 for x in unique_clients ] for y in unique_clients ]

with open ("log.txt","r") as file:
	for line in file:
		log_list=line.strip("\n").split(",")
		for i in range(len(unique_clients)):
			for j in range(len(unique_clients)):
				if log_list[1] == unique_clients[i] and log_list[2] == unique_clients[j]:
					mat[i][j]=mat[i][j]+int(log_list[0])
			heat_map.write("%d,",(mat[i][j]))
			heat_map.write("\n")	
#print unique_chars


a = np.random.random((16, 16))
plt.imshow(mat, cmap='hot', interpolation='nearest')
plt.show()
close(heat_map)