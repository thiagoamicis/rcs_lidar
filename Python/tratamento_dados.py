import matplotlib.pyplot as plt
import numpy as np

def separador(string,sep):
    '''
    Funcao para ler os valores do Arduino e separa-los em listas
    '''
    lista = []
    k = 0
    j = k
    for i in range(len(string)):
        if string[i] in sep:
            j = i
            pal = string[k:j]
            k = j+1
            lista.append(float(pal))
    return lista

file = open('saidas_arduino/F35_corrigido.txt','r')
output = file.readlines()
file.close()

# distancia ate a mesa e forca do sinal
dist_mesa = float(output[4][0:len(output[4])-1])
str_mesa = float(output[6][0:len(output[6])-1])

# loop para identificar a linha em que comeca a leitura
line_xyz = 0
for i in range(50):
    if output[i] == 'x,y,z,str\n':
        line_xyz = i+1
        break
    
# definindo as informacoes para calculo do RCS
Y_sec = 66 # mm
X_rcs = []
theta_rcs = []
RCS_sec = []
    
# salva os dados em vetores
X = []
Y = []
Z = []
F = []
for i in range(line_xyz,len(output)-1):
    linha = separador(output[i],('\n',','))
    if linha[2] < 10: 
        # salva as coordenadas
        X.append(linha[0]/10)
        Y.append(linha[1]/10)
        Z.append(linha[2])
        if linha[3] > 1023: # deveria ser o limite
            linha[3] = 1023
        F.append(linha[3])
        # salva o RCS
        if linha[1] == Y_sec:
            X_rcs.append(linha[0]/10)
            rcs = 4*np.pi*((dist_mesa-linha[2])**2)
            rcs *= ((linha[3]/100)**2)/(1023**2)
            RCS_sec.append(rcs)
        
    
X = np.array(X)
Y = np.array(Y)
Z = np.array(Z)
F = np.array(F)

# plota um grafico de pontos
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.set_zlim(-20,20)
surf = ax.scatter(X,Y,Z,c=F,cmap='seismic',vmin=0,vmax=1023)
ax.set_xlabel('X (cm)')
ax.set_ylabel('Y (cm)')
ax.set_zlabel('Z (cm)')
ax.set_zlim(0,20)
fig.colorbar(surf, shrink=0.5, aspect=5)
plt.show()

# plota uma superficie com o resultado
fig = plt.figure()
ax = plt.subplot(projection='3d')
ax.plot_trisurf(X,Y,Z)
ax.set_xlabel('X (cm)')
ax.set_ylabel('Y (cm)')
ax.set_zlabel('Z (cm)')
plt.show()

# plota o RCS de uma secao definida
r = (max(X_rcs)-min(X_rcs))/2
for i in X_rcs:
    if i < r:
        theta = np.arccos((r+min(X_rcs)-i)/r - 0.0001)*180/np.pi
    else:
        theta = 180 - np.arccos((i-min(X_rcs)-r)/r)*180/np.pi
    theta_rcs.append(theta)
RCS_dbsm = [10*np.log10(i) for i in RCS_sec]

# em m2
img, graf = plt.subplots()
graf.set(xlabel = '$\Theta$ ($\degree$)',
         ylabel = 'RCS ($m^2$)')
plt.xlim(0,180)
plt.ylim()
graf.grid()
graf.plot(theta_rcs,RCS_sec,color='red')
plt.savefig('F35_rcs_m2.svg')
plt.show()

# em dBsm
img, graf = plt.subplots()
graf.set(xlabel = '$\Theta$ ($\degree$)',
         ylabel = 'RCS (dBsm)')
plt.xlim(0,180)
plt.ylim()
graf.grid()
graf.plot(theta_rcs,RCS_dbsm,color='red')
plt.savefig('F35_rcs_dbsm.svg')
plt.show()

# Graficos polares
theta_rcs = np.radians(theta_rcs)

# m2
fig = plt.figure()
ax = fig.add_subplot(111, polar=True)
c = ax.plot(theta_rcs, RCS_sec, color='red')
ax.set_thetamin(0)
ax.set_thetamax(180)
plt.savefig('F35_m2_polar.svg')
plt.show()

# dBsm
fig = plt.figure()
ax = fig.add_subplot(111, polar=True)
c = ax.plot(theta_rcs, RCS_dbsm, color='red')
ax.set_thetamin(0)
ax.set_thetamax(180)
plt.savefig('F35_dbsm_polar.svg')
plt.show()