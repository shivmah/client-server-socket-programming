import matplotlib.pyplot as plt; plt.rcdefaults()
import numpy as np
import matplotlib.pyplot as plt
 
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
 
objects = unique_clients
y_pos = np.arange(len(objects))
char_count=[]
k=0;
for i in unique_clients:
    t=0
    for line in log_file:
    
        if i == line[1]:
            
            t += int(line[0])
            
    char_count.append(t)    
    k=k+1

print char_count
performance = char_count
 
plt.bar(y_pos, performance, align='center', alpha=0.5)
plt.xticks(y_pos, objects)
plt.ylabel('number_of_char_sent')
plt.title('clients')
 
plt.show()
