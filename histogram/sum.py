import sys

b = -1

print "Enter Numbers:"
a = [int(n, 10) for n in raw_input().split()]

for i in range(len(a)):
    if sum(a[i:len(a)]) <= 4000:
        b = i
        break

print "Result: ", b

#3x3 -> 8
#4x4 -> 123
