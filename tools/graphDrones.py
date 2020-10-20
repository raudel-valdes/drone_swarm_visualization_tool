import pygame
import os
from pygame.locals import *

pygame.init()
pygame.mixer.init()

screen=pygame.display.set_mode((640, 480))
black=pygame.Color("black")
white=pygame.Color("white")
yellow=pygame.Color("yellow")
red=pygame.Color("red")
pygame.mouse.set_visible(False)
FPS = 10
clock = pygame.time.Clock()

def parse(line):
	coordinates = line.split(':')
	coordinates[2] = coordinates[2][:-1]
	coordinates[3] = coordinates[3][:-1]
	return float(coordinates[2]), float(coordinates[3]), float(coordinates[4])

def main():
	f1 = open('Position.txt', 'r')
	f2 = open('Position2.txt', 'r')
	x1 = 0.0
	y1 = 0.0
	z1 = 0.0
	x2 = 0.0
	y2 = 0.0
	z2 = 0.0
	pygame.draw.polygon(screen, white, ((100-5, 100), (100+5, 100), (100, 100+5)), 0)
	while 1:
		line = f1.readline()
		if(line != ''):
			pygame.draw.polygon(screen, black, ((x1-5, y1), (x1+5, y1), (x1, y1+5)), 0)
			x1,y1,z1 = parse(line)
			pygame.draw.polygon(screen, yellow, ((x1-5, y1), (x1+5, y1), (x1, y1+5)), 0)
		pygame.display.update()
		clock.tick(FPS)
		
		line = f2.readline()
		if(line != ''):
			pygame.draw.polygon(screen, black, ((x2-5, y2), (x2+5, y2), (x2, y2+5)), 0)
			x2,y2,z2 = parse(line)
			pygame.draw.polygon(screen, red, ((x2-5, y2), (x2+5, y2), (x2, y2+5)), 0)
		pygame.display.update()
		clock.tick(FPS)
	pygame.quit()

		
if __name__=='__main__':
	main()
