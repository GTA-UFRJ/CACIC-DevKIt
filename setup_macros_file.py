import sys

f = open('config_macros.h','r')
filedata = f.read()
f.close()

print(str(sys.argv[1]), " <-- ", str(sys.argv[2]))
newdata = filedata.replace(str(sys.argv[1]), str(sys.argv[2]))

f = open('config_macros.h','w')
f.write(newdata)
f.close()
