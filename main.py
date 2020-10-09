import pygame
import sys

class Drone:

	img = pygame.image.load("images/drone.png")
	
	def __init__(self, id):
	
		self.time = []	
		self.id = id
		self.mode = []
		self.x = []
		self.y = []
		self.z = []
		self.life = []


def load_data(filename):
	
	drones = []

	file = open(filename, 'r')

	# go through all lines in file skipping header
	for line in file.readlines()[1:]:
		line = line.split('|')
		if line[-1] == '\n':
			line = line[:-1]

		# find right drone
		drone = None
		for d in drones:
			if int(line[1]) == d.id:
				drone = d	
		if drone == None:
			drone = Drone(int(line[1]))

		# add data to drones info lists
		drone.time.append(float(line[0]))
		drone.mode.append(line[2])
		drone.x.append(int(line[3]))
		drone.y.append(int(line[4]))
		drone.z.append(int(line[5]))
		drone.life.append(int(line[6]))
			
	return drones

pygame.init()

# read drone data from file
drones = load_data("data.txt") 
print(drones)

# used for time simulation 
time = 0.0
deltaTime = 0.0
timeFactor = 1.0
clock = pygame.time.Clock()


# loads image
background = pygame.image.load("images/background.png")


# sets up screen
screen = pygame.display.set_mode((800, 600))
pygame.display.set_caption("Drone Visualization Tool")

# event loop
while True:
	for event in pygame.event.get():
		if event.type == pygame.QUIT:
			sys.exit()


	# erase screen
	screen.blit(background, (0, 0))

	# keep track of current time in simulation
	deltaTime = clock.tick() * timeFactor
	time += deltaTime

	# display changes
	pygame.display.flip()

