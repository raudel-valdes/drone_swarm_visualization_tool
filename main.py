import pygame
import sys
import math
import numpy as np

class Drone:


	img = pygame.image.load("images/drone.png")
	rect = img.get_rect()

	def __init__(self, id):
	
		self.time = []	
		self.id = id
		self.mode = []
		self.x = []
		self.y = []
		self.z = []
		self.life = []
		self.img = None
		self.rect = None
		self.screen_pos = [0, 0] 

	def mouseCollide(self):
		x, y = pygame.mouse.get_pos()
		dx = x - self.screen_pos[0]
		dy = y - self.screen_pos[1]
		dist = math.sqrt(dx*dx + dy*dy)
		if dist < 20:
			return True
		else:
			return False
		

	def update(self, time, trail=False, alpha=0):

		global x, y, scale, metersPerUnit, pixelsPerUnit, screen

		if not trail:
			trailTime = time - time % 1 
			for i in range(1, 1000):
				trailTime -= 1 
				if trailTime < 0:
					break
				#self.update(trailTime, True, 100 * (200 - i) / 100)
				self.update(trailTime, True, 40)
				
				
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

		# interpolates position
		posX = self.x[i - 1] + interFactor * (self.x[i] - self.x[i-1])
		posY = self.y[i - 1] + interFactor * (self.y[i] - self.y[i-1])
		pos = pos_to_grid([posX, posY])
		pos[0] = int(pos[0])
		pos[1] = int(pos[1])

						
		

		# draws trail
		if trail:
			#pygame.draw.circle(screen, (0, 0, 0), (pos[0], pos[1]), 10)	
			s = pygame.Surface((4, 4), pygame.SRCALPHA)
			s.fill((255, 255, 40, alpha))
			#pos[0] -= pos[0] % 10
			#pos[1] -= pos[1] % 10
			screen.blit(s, pos)

		# draws drone
		else:

			# finds angle
			v1 = [self.x[i], self.y[i]]
			v1[0] -= self.x[i-1]
			v1[1] -= self.y[i-1]
			v2 = [0, -1]	
			self.img = pygame.transform.rotate(Drone.img, angleBetween(v1, v2))
			self.rect = self.img.get_rect()
			self.rect.center = pos
			screen.blit(self.img, self.rect)
			self.screen_pos = pos

			


def angleBetween(a, b):
	return 360 * math.atan2( a[0]*b[1] - a[1]*b[0], a[0]*b[0] + a[1]*b[1]) / (2 * math.pi)


def load_data(filename):

	global maxTime
	
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

		if drone.time[-1] > maxTime:
			maxTime = drone.time[-1] 
			
	return drones


def draw_grid(screen):

	global x, y, scale, metersPerUnit, pixelsPerUnit

	# draws rows
	rows = int(screen.get_height() / pixelsPerUnit)
	rows += 1 # accounts for shifting
	for i in range(-1, rows):
		rowOffset = y / metersPerUnit
		rowOffset *= pixelsPerUnit
		boldOffset = rowOffset
		rowOffset = -(rowOffset % pixelsPerUnit)
		rowY = i * pixelsPerUnit
		rowY += rowOffset
		bold = False
		if bold:
			pygame.draw.line(screen, (0, 0, 0), (0, rowY), (screen.get_width(), rowY), 2) 
		else:	
			pygame.draw.line(screen, (40, 40, 40), (0, rowY), (screen.get_width(), rowY), 1) 

	# draws columns
	cols = int(screen.get_width() / pixelsPerUnit)
	cols += 1 # accounts for shifting
	for i in range(-1, cols):
		colOffset = x / metersPerUnit
		colOffset *= pixelsPerUnit
		colOffset = -(colOffset % pixelsPerUnit)
		colX = i * pixelsPerUnit
		colX += colOffset
		pygame.draw.line(screen, (40, 40, 40), (colX, 0), (colX, screen.get_height()), 1) 


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

# sets up screen
screen = pygame.display.set_mode((1200, 800))
pygame.display.set_caption("Drone Visualization Tool")
pygame.display.set_icon(Drone.img)

# slider bar for scolling through time
bar = pygame.image.load("images/bar.png")
barRect = bar.get_rect()
barRect.center = [int(screen.get_width() / 2), int(barRect.height)]
slider = pygame.image.load("images/slider.png")
sliderRect = slider.get_rect()
sliderPercent = 0.0

# play/pause buttons
play = pygame.image.load("images/play.png")
playRect = play.get_rect()
pause = pygame.image.load("images/play.png")
pauseRect = pause.get_rect()
playing = False


# used for time simulation 
time = 0.1
minTime = 0.0
maxTime = 0.0
deltaTime = 0.0
timeScale = 1.0
x = -20.0
y = -20.0
scale = 1.0
metersPerUnit = 10 
pixelsPerUnitStart = 200 
pixelsPerUnit = pixelsPerUnitStart 
clock = pygame.time.Clock()

# read drone data from file
if len(sys.argv) != 2:
	print("Program needs exactly 1 arg for data file.")
	sys.exit()
drones = load_data(sys.argv[1]) 

mouseDrag = False
mouseZoom = False
mousePressed = False
mousePressedTime = 0
mouseClicked = False
selectedDrone = None 


# loads background 
background = pygame.image.load("images/background.png")



# event loop
while True:
	for event in pygame.event.get():
		if event.type == pygame.QUIT:
			sys.exit()
		elif event.type == pygame.MOUSEBUTTONDOWN:
			if event.button == 1:
				if event.pos[0] >= barRect.left - 20 and event.pos[0] <= barRect.right + 20 and event.pos[1] >= barRect.top and event.pos[1] <= barRect.bottom: 
					pressedElement = "slider"
					playing = False
				else:
					pressedElement = "none"
				mousePressedPos = event.pos
				mousePressed = True
				mousePressedTime = 0.0
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
				if mousePressedTime < 200: # ms
					mouseClicked = True
			elif event.button == 2:
				mouseZoom = False
			elif event.button == 3:
				mouseDrag = False 

		elif event.type == pygame.KEYDOWN:
			if event.key == pygame.K_RIGHT:
				timeScale *= 2 
			elif event.key == pygame.K_LEFT:
				timeScale /= 2 
			elif event.key == pygame.K_SPACE:
				if playing:
					playing = False
				else:
					playing = True
				

	pixelsPerUnit = pixelsPerUnitStart * scale

	# erase screen
	screen.blit(background, (0, 0))
	
	# right click mouse drag for repositioning	
	if mouseDrag:
		scrollFactor = 1.22
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
	draw_grid(screen)
	for drone in drones:
		drone.update(time)

	if mouseClicked:
		for i in range(0, len(drones)):
			if drones[i].mouseCollide():
				selectedDrone = i

	if mousePressed:

		posX, posY = pygame.mouse.get_pos()

		# slider controls time		
		if pressedElement == "slider":
			sliderPercent = float(posX - barRect.left) / float(barRect.right - barRect.left)
			if sliderPercent > 1.0:
				sliderPercent = 1.0
			elif sliderPercent < 0:
				sliderPercent = 0

		# left click for measurements
		else:
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

	# top tool bar for control of simulation
	img = pygame.Surface((screen.get_width(), 74), pygame.SRCALPHA)
	img.fill((150, 150, 150, 150))
	rect = img.get_rect()
	rect.left = 0
	rect.top = 0
	screen.blit(img, rect)
	pygame.draw.line(screen, (150, 150, 150), (0, rect.bottom), (screen.get_width(), rect.bottom), 4)

	# side tool bar for drone info
	img = pygame.Surface((200, screen.get_height() / 2), pygame.SRCALPHA)
	img.fill((150, 150, 150, 150))
	rect = img.get_rect()
	rect.right = screen.get_width() 
	rect.top = screen.get_height() / 2 
	screen.blit(img, rect)
	pygame.draw.line(screen, (150, 150, 150), (rect.left, screen.get_height()), (rect.left, screen.get_height() / 2), 4)
	pygame.draw.line(screen, (150, 150, 150), (rect.right, screen.get_height() / 2), (rect.left, screen.get_height() / 2), 4)
	if selectedDrone:
		sel_img = pygame.transform.rotozoom(drones[selectedDrone].img, 0, 2.0)
		sel_rect = sel_img.get_rect()
		sel_rect.centerx = rect.centerx
		sel_rect.top = rect.top + 20
		screen.blit(sel_img, sel_rect)	

	# slider graphics
	screen.blit(bar, barRect)
	sliderRect.centerx = int(barRect.left + sliderPercent * barRect.width)
	sliderRect.centery = barRect.centery
	screen.blit(slider, sliderRect)

	# min time
	txt = font.render("0.0 s", True, (0, 0, 0))
	rect = txt.get_rect()
	rect.right = barRect.left - 10
	rect.centery = barRect.centery
	screen.blit(txt, rect)

	# max time
	txt = font.render(str(round(maxTime, 1)) + " s", True, (0, 0, 0))
	rect = txt.get_rect()
	rect.left = barRect.right + 10
	rect.centery = barRect.centery
	screen.blit(txt, rect)

	# current time
	txt = font.render(str(round(time, 1)) + " s", True, (0, 0, 0))
	rect = txt.get_rect()
	rect.centerx = sliderRect.centerx
	rect.top = barRect.bottom + 4
	screen.blit(txt, rect)

	# time scale
	txt = font.render(str(round(timeScale, 1)) + "x", True, (0, 0, 0))
	rect = txt.get_rect()
	rect.centery = sliderRect.centery
	rect.left = 10 
	screen.blit(txt, rect)

	# play pause button
	'''if playing:
		img = pause
		rect = pauseRect
	else:
		img = play
		rect = playRect
	rect.left = 10
	rect.centery = sliderRect.centery
	screen.blit(img, rect)'''

	# keep track of current time in simulation
	trueDelta = clock.tick()
	if mousePressed and mousePressedTime < 1000:
		mousePressedTime += trueDelta	
	deltaTime = trueDelta * timeScale
	deltaTime /= 1000
	if playing:
		time += deltaTime
		if time > maxTime:
			time = maxTime 
			playing = False
		sliderPercent = time / maxTime
	else:
		time = sliderPercent * maxTime

	# display changes
	pygame.display.flip()


