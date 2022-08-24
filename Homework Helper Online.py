from datetime import datetime 
import sys
import traceback
from typing import AsyncIterable
import pygame
import os
import time 
import socket
import threading


WEEKDAYS = ["monday", "tuesday", "wednesday", "thursday","friday","saturday","sunday"]
MONTHS = ["january","february","march","april","may","june","july","august","september","october","november","december"]

MONTHS_DICT = {"january":31,"february":28,"march":31,"april":30,"may":31,"june":30,"july":31,"august":31,"september":30,"october":31,"november":30,"december":31}

date = datetime.now().strftime("%m-%d").split("-")

TODAY_MONTH = date[0]
TODAY_DAY = date[1]

server = None
piId = None

noEditsMade = True
loadedOffline = False
loaded = False

stopThreads = False



# this fucntion will send text to the server
def send(send):
  b = send # do this just incase audio gets overwritten in main
  # b = audio.get_raw_data()
  l = str(len(b) + 10000000) # add this so that the string is always the same size
  print("Sending message of size {}b".format(len(b)))
  server.send(l.encode("utf-8"))
  server.send(b.encode("utf-8"))


def loadId():
  filename = "Id.txt"
  # this make its ok to do files stuff once its an exe
  if getattr(sys, 'frozen', False):
    CurrentPath = sys._MEIPASS
  # If it's not use the path we're on now
  else:
    CurrentPath = os.path.dirname(__file__)

  try:
    f = open(filename, "r")

  except FileNotFoundError:
    f = open(filename, "x")
    f.close()
    f = open(filename, "r")
  except:
    print("File error")

  Id = f.readline()
  Id = line.strip("\n")
  if Id == "" or Id == None:
    Id = "1"

  return Id


def getData():
  print("Downloading data from server")
  while True:
    strlen = server.recv(8).decode("utf-8")
    length = int(strlen) - 10000000 # added this so that the bytes size is always the same 
    b = b''
    left = length
    while left != 0:
      batch = min(1024*1024, left)
      newpart = server.recv(batch)
      left -= len(newpart)
      b += newpart
      # print("{}b out of {}b: {}%".format(len(b),length,round((len(b)/length)*10000)/100))
    print("Data downloaded")
    return b.decode('utf-8')


# load the assignments list from a file
def loadFromFile(file_name="homeworkfile.txt"):	
  print("Loading data from file")
  assignments = []
  # this make its ok to do files stuff once its an exe
  if getattr(sys, 'frozen', False):
    CurrentPath = sys._MEIPASS
  # If it's not use the path we're on now
  else:
    CurrentPath = os.path.dirname(__file__)

  try:
    f = open(file_name, "r")

  except FileNotFoundError:
    f = open(file_name, "x")
    f.close()
    f = open(file_name, "r")
  except:
    print("File error")

  lines = f.read()
  
  print("Sucessfully loaded " + file_name)
  
  return lines


# load the assignments list from the server
def load():
  global loaded
  loaded = True
  if (server != None):
    text = getData()
  else:
    text = loadFromFile()

  if text == "None":
    return []
  text = text.split("\n")
  assignments = []
  for line in text:
    line = line.strip("\n").split(" ")
    if len(line) == 4:
      print(line)
      assignments.append(Assignment(line[0], line[1], line[2], line[3]))

  return assignments


# save the assignments list to a file
def saveToFile(assignments, file_name="homeworkfile.txt"): 
  try:
    f = open(file_name, "w")
  except:
    print("File error")
    sys.exit()

  for x in assignments:
    f.write(x.save_helper())
  f.close()


# save the assignments list to the server
def save(assignments): 
  text = ""
  for x in assignments:
    text += x.save_helper()

  if server != None:
    send(text)
  saveToFile(assignments)
  


class Window():
  def __init__(self, size_, screen_):
    self.size = size_
    self.screen = screen_
    self.buttons = []


class Button():
  def __init__(self, name_ ,rect_="", text_=""):
    self.name = name_
    self.rect = rect_
    self.text = text_

  
# an assignment object
class Assignment():
  def __init__(self, name, day, day_name, month):
    self.name = name
    self.day = day
    self.day_name = day_name
    self.month = month

  def set_duedate(self, day, month):
    self.day = day
    self.month = month

  def save_helper(self):
    return self.name + " " + str(self.day) + " " + self.day_name + " " + self.month + "\n"

  def __str__(self):
    return self.name.replace("_", " ") + " " + self.day_name.capitalize() + ", " + self.month.capitalize() + " " + str(self.day)

def finish_frame(game_clock):
  pygame.display.flip()
  #game_clock.tick(200)


def start_screen(window, assignments):

  # different texts
  #						         font,  size, bold,  italics
  title = pygame.font.SysFont('arialblack', 50, False, False)
  small_text = pygame.font.SysFont('comicsansms', 18, False, False)
  tiny_text = pygame.font.SysFont('comicsansms', 15, True, False)
  # different colors
  text_color = [255,255,255]
  button_color = [40,40,40]
  # Render the text. "True" means anti-aliased text.
  # Black is the color. The variable BLACK was defined
  # above as a list of [0, 0, 0]
  # Note: This line creates an image of the letters,
  # but does not put it on the screen yet.
  text = title.render("Homework",True,text_color)
  text_rect = text.get_rect(center=(window.size[0]/2, 25))

  # add all the assignments
  space = 30
  num_assignments = 0
  if server == None:
      firstLine = "Offline" 
  else:
    firstLine = "Online"
  assignment_text = small_text.render(firstLine ,True,text_color)
  assignment_rect = text.get_rect(center=(150, 100 + space*num_assignments))

  window.screen.blit(assignment_text, assignment_rect)
  

  num_assignments += 1
  if not loaded:
    assignment_text = small_text.render("Loading..." ,True,text_color)
    assignment_rect = text.get_rect(center=(150, 100 + space*num_assignments))

    window.screen.blit(assignment_text, assignment_rect)
  else:
    for assignment in assignments:
      assignment_text = small_text.render(str(num_assignments) + ". " + assignment.__str__(),True,text_color)
      assignment_rect = text.get_rect(center=(150, 100 + space*num_assignments))

      window.screen.blit(assignment_text, assignment_rect)
      num_assignments += 1
  

  text2 = tiny_text.render("Name Weekday Day_number Month(blank n/a)",True,text_color)
  text2_rect = text.get_rect(center=(150, 410))

  window.screen.blit(text2, text2_rect)
  

  # draw the buttons
  x = window.size[0]
  y = window.size[1]
  x_size = 120
  y_size = 30
  spacing = x_size + 20
  startx = 0
  starty = (y - y_size)
  
  draw_button(window,"Add assignment", startx,(starty)-y_size/2,x_size,y_size, "add", text_color, button_color)
  draw_button(window,"Remove assignment", startx + spacing, ((starty)-y_size/2),x_size,y_size, "remove", text_color, button_color)
  draw_button(window,"Move assignment", startx+ spacing*2,((starty)-y_size/2) ,x_size,y_size, "move", text_color, button_color)

  window.screen.blit(text, text_rect)

def draw_button(window, name, x_start,y_start ,x_size,y_size, text, text_color, color):
  button = pygame.Rect(x_start, y_start, x_size, y_size)
  pygame.draw.rect(window.screen,color,button)

  font = pygame.font.SysFont('comicsansms', 25)
  text = font.render(text,True,text_color)
  text_rect = text.get_rect(center=(x_start+x_size/2,y_start+y_size/2))
  window.screen.blit(text, text_rect)
  # smallText = pygame.font.SysFont("Comic Sans",40)
  # textSurf, textRect = text_objects(text, smallText)
  # textRect.center = ( button.x,button.y )
  # window.screen.blit(textSurf, textRect)

  button = Button(name, button)
  not_in = True
  for old_button in window.buttons:
    if old_button.name == button.name:
      not_in = False
  if not_in:
    window.buttons.append(button)


#PYGAME STUFF
def window_stuff(window, screen, game_clock, assignments, text_box_text, before):
  background = (67,67,67)
  screen.fill(background)
  start_screen(window, assignments)
  # make the textbox
  input_box = pygame.Rect(2, 400, 394, 32)
  color = [255,255,255]
  active = False
  return_text = ""
  keys = pygame.key.get_pressed()

  # if keys[pygame.K_RIGHT] and keys[pygame.K_LEFT]:
  # 	move_fullcube = left


  for event in pygame.event.get():
    if event.type == pygame.QUIT:
      return_text = "q"
    elif event.type == pygame.MOUSEBUTTONDOWN:
      mouse_pos = event.pos
      # If the user clicked on the input_box rect.
      if input_box.collidepoint(event.pos):
        # Toggle the active variable.
        active = True
      else:
        active = True
      # Change the current color of the input box.
      for button in window.buttons:
        if button.rect.collidepoint(mouse_pos):
          # prints current location of mouse
          print(button.name + " was pressed")
          return_text = button.name[0]
    if event.type == pygame.KEYDOWN:
      before = time.time()
      if event.key == pygame.K_BACKSPACE:
        if (keys[pygame.K_LCTRL]):
          # delete from end to " " or "_"
          space_index = len(text_box_text)
          space_index2 = len(text_box_text)
          if (len(text_box_text)!=0):
            if (text_box_text[-1] == "_" or text_box_text[-1] == " "):
              text_box_text = text_box_text[:len(text_box_text)-1]
              text_box_text += "t" # this is needed and I have no idea why
            try:
              space_index = text_box_text.rindex("_")
            except:
              pass
            try:
              space_index2 = text_box_text.rindex(" ")
              if space_index == len(text_box_text):
                space_index = space_index2
              elif space_index2 != len(text_box_text):
                space_index = max(space_index, space_index2)
            except:
              pass

            print(space_index)

            text_box_text = text_box_text[:space_index+1]
            if (space_index == len(text_box_text)):
              text_box_text = ""
        else:
          text_box_text = text_box_text[:-1]
      else:
        text_box_text += event.unicode

  font = pygame.font.SysFont('microsoftnewtailue', 25)
  txt_surface = font.render(text_box_text, True, color)
  screen.blit(txt_surface, (input_box.x+5, input_box.y+5))
  pygame.draw.rect(screen, color, input_box, 2)
  finish_frame(game_clock)
  return return_text, text_box_text, before

# create an assignment
def make_assignment(text):
  split_text = text.split(" ")
  print(split_text)
  if len(split_text) >= 3:
    name = split_text[0]
    day_name = split_text[1].lower()
    day = split_text[2]

    if len(split_text) == 4:
      month = split_text[3].lower()
    else:
      month = MONTHS[int(TODAY_MONTH) - 1]

    if month in MONTHS:
      if (day.isdigit()):
        day = int(day)
        if day > 0 and day <= MONTHS_DICT[month]:
          if day_name in WEEKDAYS:
            return Assignment(name, day, day_name, month)
          else:
            print(day_name + " is not a day")
        else:
          print("That is not a day")
      
      else:
        print("Day " + day + " is not in " + month)
    else:
      print(month + " is not a month")
  return "none"

# show all assignments
# def show_assignments(assignments):
# 	print()
# 	print("Current assignments:")
# 	if (len(assignments) == 0):
# 		print("No assignments!")
# 	x = 0
# 	for assignment in assignments:
# 		x += 1
# 		print(str(x) + "." , assignment)
# 	print()


# remove an assignment from the list
def remove_assignment(assignments, text):
  # show_assignments(assignments)
  if (len(assignments) == 0):
    return assignments
  if (text == "q"):
    return assignments
  if (text.isdigit()):
    text = int(text) - 1
    if (text >= 0 and text < len(assignments)):
      assignments.pop(text)
  return assignments


# move an assignment to a new place in the list
def move_assignment(assignments, text):
  # show_assignments(assignments);
  text = text.split(" ")
  if len(text) >= 2:
    if (text[0] == "q"):
      return assignments
    if (text[0].isdigit() and text[1].isdigit()):
      text[0] = int(text[0]) - 1
      text[1] = int(text[1]) - 1
      if (text[0] >= 0 and text[0] < len(assignments) and text[1] >= 0 and text[1] < len(assignments)):
        move = assignments[text[0]]
        assignments.pop(text[0])
        assignments.insert(text[1], move)
        print("Moved successfully")
      else:
        print("That is outside of the bounds")

  return assignments


# will return the username of this user if it exists
def getID():
  try:
    with open("id.txt", "r") as f:
      return f.read().replace("\n","")
  except: # this means that the id file doesnt exist
    return "1"


def loadIp():
  try:
    with open("ServerIP.txt", "r") as f:
      return f.read().replace("\n","")
  except: # this means that the ServerIP file doesnt exist
    return "1"

# this will connect to the server and ensure that the server knows we are still connected
def serverConnection():
  global loadedOffline
  global assignments
  global server
  Id = getID()
  while True: # try to connect
    try:
      print("Trying to connect to server")
      serverIp = loadIp()
      server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
      server.settimeout(5) # 5 seconds need this so that ctrl c works
      server.connect((serverIp, 51153))
      server.send(Id.encode('utf-8'))
      print("Sent Id")
      if noEditsMade:
        assignments = load()
      return server, Id
    except:
      print(traceback.format_exc())
      if not loadedOffline:
        assignments = load()
        loadedOffline = True
      if stopThreads:
        break


try:
  while True:

    t = threading.Thread(target=serverConnection, args=()) 
    t.start() 
    

    # PYGAME STUFF

    game = pygame.init() # start pygame

    text_box_text = ""

    window_width = 400
    window_height = 500
    window_place = str(pygame.display.Info().current_w - window_width - 10) + ",40"
    os.environ['SDL_VIDEO_WINDOW_POS'] = window_place # make the windows start in the top right corner
    size = [window_width, window_height]

    # screen = pygame.display.set_mode((0, 0), pygame.FULLSCREEN) # This is fullscreen
    pygame.display.set_caption("Homework Helper")
    screen = pygame.display.set_mode(size)
    window = Window(size, screen)

    game_clock = pygame.time.Clock()
    # END PYGAME STUFF
    waiting_for_text = False
    before = time.time()


    assignments = []

    while True:
      # print(time.time() - before)
      if (time.time() - before > 15):
        time.sleep(1) # 1 fps
      else:
        time.sleep(.033333) # 30 fps 

      # make the window
      text, text_box_text, before = window_stuff(window, screen, game_clock, assignments, text_box_text, before)

      if len(text) > 0:
        before = time.time()
        text = text.lower()
        if (text == "a"):
          noEditsMade = False
          assignment = make_assignment(text_box_text)
          print(text_box_text)
          text_box_text = ''
          print(assignment)
          if (assignment != "none"):
            assignments.append(assignment)
          
        # elif (text == "s"):
        # 	show_assignments(assignments)
        elif (text == "r"):
          noEditsMade = False
          assignments = remove_assignment(assignments, text_box_text)
          print(text_box_text)
          text_box_text = ''

        elif (text == "m"):
          noEditsMade = False
          assignments = move_assignment(assignments, text_box_text)
          print(text_box_text)
          text_box_text = ''
        elif (text == "q"):
          break
        save(assignments)
          
    save(assignments)
    break
except:
  stopThreads = True
