import pygame
import sys
import math

class Drone:

	
	def __init__(self, id):
	
		self.time = []	
		self.id = id
		self.mode = []
		self.x = []
		self.y = []
		self.z = []
		self.life = []

		self.img = pygame.image.load("images/drone.png")
		self.rect = self.img.get_rect()

	def update(self):

		global x, y, scale, metersPerUnit, pixelsPerUnit, time, screen
		
		before = 0.0
		after = 0.0
		i = 0
		while after <= time and i < len(self.time) - 1:
			i += 1
			after = self.time[i]
		

		# skip displaying if outside of provided timestamps
		if (len(self.time) <= 1):
			return
		if (time > self.time[-1]):
			return
		if (time < self.time[0]):
			return

		before = self.time[i - 1]
		if (after == before):
			interFactor = 0.0
		else:
			interFactor = (time - before) / (after - before)

		posX = self.x[i - 1] + interFactor * (self.x[i] - self.x[i-1])
		posY = self.y[i - 1] + interFactor * (self.y[i] - self.y[i-1])
		pos = pos_to_grid([posX, posY])
		self.rect.center = pos
		screen.blit(self.img, self.rect)

			
		
			


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
		drones.append(drone)
			
	return drones


def draw_grid(screen):

	global x, y, scale, metersPerUnit, pixelsPerUnit

	# draws rows
	rows = int(screen.get_height() / pixelsPerUnit)
	rows += 1 # accounts for shifting
	for i in range(-1, rows):
		rowOffset = y / metersPerUnit
		rowOffset *= pixelsPerUnit
		rowOffset = -(rowOffset % pixelsPerUnit)
		rowY = i * pixelsPerUnit
		rowY += rowOffset
		pygame.draw.line(screen, (70, 70, 70), (0, rowY), (screen.get_width(), rowY), 1) 

	# draws columns
	cols = int(screen.get_width() / pixelsPerUnit)
	cols += 1 # accounts for shifting
	for i in range(-1, cols):
		colOffset = x / metersPerUnit
		colOffset *= pixelsPerUnit
		colOffset = -(colOffset % pixelsPerUnit)
		colX = i * pixelsPerUnit
		colX += colOffset
		pygame.draw.line(screen, (70, 70, 70), (colX, 0), (colX, screen.get_height()), 1) 


def pos_to_grid(pos):

	global x, y, scale, metersPerUnit, pixelsPerUnit
	
	pos[0] -= x
	pos[0] /= metersPerUnit
	pos[0] *= pixelsPerUnit 
	pos[1] -= y
	pos[1] /= metersPerUnit
	pos[1] *= pixelsPerUnit 

	return pos

	
def mag(v):
	return math.sqrt(v[0] * v[0] + v[1] * v[1])

def unit(v):
	if v[0] == 0 and v[1] == 0:
		return [0, 0]
	v2 = [v[0], v[1]]
	v2[0] /= mag(v)
	v2[1] /= mag(v)
	return v2
		

pygame.init()
font = pygame.font.Font("Azonix.otf", 12)

# read drone data from file
drones = load_data("data.txt") 

# used for time simulation 
time = 0.1
deltaTime = 0.0
timeScale = 1.0
x = -20.0
y = -20.0
scale = 1.0
metersPerUnit = 10 
pixelsPerUnitStart = 100 
pixelsPerUnit = pixelsPerUnitStart 
clock = pygame.time.Clock()

mouseDrag = False
mouseZoom = False
mousePressed = False


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
		elif event.type == pygame.MOUSEBUTTONDOWN:
			if event.button == 1:
				mousePressedPos = event.pos
				mousePressed = True
			elif event.button == 2:
				mouseZoomPos = event.pos
				mouseZoom = True
				mouseZoomOrigin = scale
				mouseZoomCenter = [event.pos[0], event.pos[1]] 
				mouseZoomCenter[0] *= metersPerUnit 
				mouseZoomCenter[0] /= pixelsPerUnit
				mouseZoomCenter[0] += x
				mouseZoomCenter[1] *= metersPerUnit 
				mouseZoomCenter[1] /= pixelsPerUnit
				mouseZoomCenter[1] += y 
				print(mouseZoomCenter)
			elif event.button == 3:
				mouseDragPos = event.pos
				mouseDragOrigin = [x, y]
				mouseDrag = True
		elif event.type == pygame.MOUSEBUTTONUP:
			if event.button == 1:
				mousePressed = False 
			elif event.button == 2:
				mouseZoom = False
			elif event.button == 3:
				mouseDrag = False 

	pixelsPerUnit = pixelsPerUnitStart * scale

	# erase screen
	screen.blit(background, (0, 0))

	draw_grid(screen)

			
	
	# right click mouse drag for repositioning	
	if mouseDrag:
		scrollFactor = 2.0
		posX, posY = pygame.mouse.get_pos()
		dx = posX - mouseDragPos[0]
		dx /= pixelsPerUnit
		dx *= metersPerUnit
		dx *= scrollFactor
		dy = posY - mouseDragPos[1]	
		dy /= pixelsPerUnit
		dy *= metersPerUnit
		dy *= scrollFactor
		x = mouseDragOrigin[0] - dx
		y = mouseDragOrigin[1] - dy

	if mouseZoom:
		zoomFactor = 0.06 
		posX, posY = pygame.mouse.get_pos()
		dy = posY - mouseZoomPos[1]	
		dy *= zoomFactor
		scale = mouseZoomOrigin + zoomFactor * dy
		if scale < 0.05:
			scale = 0.05

		# aligns center
		offsetX = mouseZoomPos[0] 
		offsetX /= pixelsPerUnit
		offsetX *= metersPerUnit
		offsetY = mouseZoomPos[1] 
		offsetY /= pixelsPerUnit
		offsetY *= metersPerUnit
		x = mouseZoomCenter[0] - offsetX
		y = mouseZoomCenter[1] - offsetY
		
		


	# draw latest stuff
	for drone in drones:
		drone.update()

	# left click for measurements
	if mousePressed:
		
		posX, posY = pygame.mouse.get_pos()
		dx = posX - mousePressedPos[0]
		dx /= pixelsPerUnit
		dx *= metersPerUnit
		dy = posY - mousePressedPos[1]	
		dy /= pixelsPerUnit
		dy *= metersPerUnit
		dist = math.sqrt(dx*dx + dy*dy)
		txt = font.render(str(round(dist, 1)) + " m", True, (0, 0, 0))	
		rect = txt.get_rect()
		perp = unit([dx, dy])
		perp = [perp[1], -perp[0]]
		if perp[1] >= 0:
			perp = [-perp[0], -perp[1]]
		cenX = ((mousePressedPos[0] + posX) / 2) + perp[0] * 40
		cenY = ((mousePressedPos[1] + posY) / 2) + perp[1] * 40
		rect.center = [cenX, cenY]
		pygame.draw.line(screen, (0, 0, 255), mousePressedPos, (posX, posY), 2)	
		#beg = (mousePressedPos[0] + perp[0] * 5, mousePressedPos[1] + perp[1] * 4)
		#end = (mousePressedPos[0] - perp[0] * 5, mousePressedPos[1] - perp[1] * 4)
		#pygame.draw.line(screen, (0, 0, 255), beg, end, 3)	
		screen.blit(txt, rect)

	# keep track of current time in simulation
	deltaTime = clock.tick() * timeScale
	#time += deltaTime

	# display changes
	pygame.display.flip()

