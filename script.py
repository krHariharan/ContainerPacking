import numpy as np

fileName = input("Enter file:")
data = np.loadtxt(fileName)[:, 1]*100
time = np.loadtxt(fileName)[:, 0]
avg = [sum(data[i:i+10])/10 for i in range(0,len(data),10)]
avg = np.append(avg, [sum(data)/len(data)])
Min = [min(data[i:i+10]) for i in range(0,len(data),10)]
Min = np.append(Min, [min(data)])
Max = [max(data[i:i+10]) for i in range(0,len(data),10)]
Max = np.append(Max, [max(data)])
std = [np.std(data[i:i+10]) for i in range(0,len(data),10)]
std = np.append(std, [np.std(data)])
time_avg = [sum(time[i:i+10])/10 for i in range(0,len(time),10)]
time_avg = np.append(time_avg, [sum(time)/len(time)])
op = np.stack((avg, Min, Max, std, time_avg)).T
print(op)
print(sum(data)/len(data))
np.savetxt("table_data.csv",op, fmt='%.4f')