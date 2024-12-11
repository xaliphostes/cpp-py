from pyalgo import GreenFunction

a = GreenFunction([0,0,0], [1, 2, 0])
stress = a.run([0, 1, 2, 3, 10, 20])
print(stress)
