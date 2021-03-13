from threading import Thread
from Tkinter import *
from ctypes import *
import sys 
import os 


class Register:

	def __init__(self, window, label, data, pos):
		self.name = label
		self.value = data
		self.color = '#1111c1'
		[self.x, self.y] = pos
		self.obj = Frame(window, width=200, height=100)
		self.obj.grid(row=self.x, column=self.y, padx=5, pady=5)
		self.obj.config(background=self.color)
		self.val = Label(self.obj, 
						text=str(self.name+': '+self.value),
						background=self.color).grid(row=self.x, column=self.y, padx=10, pady=2)



def main():
	# Load The C Library for Stepping through ELF binaries
	if not os.path.isfile('firecracker.so'):
		os.system('gcc -shared -fPIC -o firecracker.so steplib.c')
	lib = cdll.LoadLibrary("./firecracker.so")
	if len(sys.argv) >= 2:
		pid = int(sys.argv[1])
		lib.show_registers(pid)

	root = Tk()

	root.wm_title("~ Firecracker ~") 
	root.config(background = "#b1b1b1b1b1b1")

	rip = Register(root, 'RIP', '0x00000000', [0,0])
	rax = Register(root, 'RAX', '0x00000000', [1,0])
	rcx = Register(root, 'RCX', '0x00000000', [2,0])
	rdx = Register(root, 'RDX', '0x00000000', [3,0])
	rbp = Register(root, 'RBP', '0x00000000', [4,0])
	cs  = Register(root,  'CS', ' 0x00000000', [5,0])

	r08 = Register(root, 'R08', '0x00000000', [0, 1])
	r09 = Register(root, 'R09', '0x00000000', [1, 1])
	r10 = Register(root, 'R10', '0x00000000', [2, 1])
	r11 = Register(root, 'R11', '0x00000000', [3, 1])
	rsp = Register(root, 'RSP', '0x00000000', [4, 1])
	ss  = Register(root,  'SS', ' 0x00000000', [5, 1])



	root.mainloop() # run it

if __name__ == '__main__':
	main()