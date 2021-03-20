from threading import Thread
from Tkinter import *
from ctypes import *
import subprocess
import time
import sys 
import os 

lib = cdll.LoadLibrary("./dlib.so")


class Register:

	def __init__(self, window, label, data, pos):
		self.name = label
		self.value = data
		self.data = StringVar()
		self.data.set(self.name+': '+self.value)
		self.color = '#1111c1'
		[self.x, self.y] = pos
		self.obj = Frame(window, width=200, height=100)
		self.obj.grid(row=self.x, column=self.y, padx=5, pady=5)
		self.obj.config(background=self.color)
		self.val = Label(self.obj, 
						textvariable=self.data,
						background=self.color).grid(row=self.x, column=self.y, padx=10, pady=2)


def debugger(program):
	# Doesnt work with GUI though.. Tkinter won't come up here during main Thread.
	newpid = os.fork()
	while True:
		if newpid == 0:
			# Execute the debugger program
			dbg_pid = launch_program(program)
			lib.show_registers(dbg_pid)
			lib.get_rax.argtypes = [c_int]
			lib.get_rax.restypes = c_ulong
			lib.get_rip.argtypes = [c_int]
			lib.get_rip.restypes = c_ulong
			Register(root, 'RIP', '0x%08x' % c_ulong(lib.get_rip(dbg_pid) & 0xFFFFFFFF).value, [1,0])
			Register(root, 'RAX', '0x%08x' % c_ulong(lib.get_rax(dbg_pid) & 0xFFFFFFFF).value, [2,0])
			# Try Setting Breakpoint

			time.sleep(0.3)
		else:
			pids = (os.getpid, newpid)
			time.sleep(.3)


def launch_program(p):
	cmd = [p]
	p = subprocess.Popen(cmd)
	return p.pid


def quit():
	c = "ps aux | grep gui.py | cut -d ' ' -f 2 | while read n ; do kill -9 $n; done"
	os.system(c)


def main():
	# Load The C Library for Stepping through ELF binaries
	if not os.path.isfile('firecracker.so'):
		os.system('gcc -shared -fPIC -o firecracker.so bugger.c')
	
	
	# Load in program to debug
	if len(sys.argv) >=2:
		target = sys.argv[1]
	else:
		print('Usage: %s [option] program' % sys.argv[0])
		exit()
		
	# Create GUI 
	root = Tk()
	root.wm_title("~ Firecracker ~") 
	root.config(background = "#b1b1b1b1b1b1")
	# Add Quit Button
	q = Button(root, text="Quit", command=quit)
	q.grid(row=0,column=0,padx=2,pady=2)
	# Add Registers 
	Register(root, 'RIP', '0x00000000', [1,0])
	Register(root, 'RAX', '0x00000000', [2,0])
	Register(root, 'RCX', '0x00000000', [3,0])
	Register(root, 'RDX', '0x00000000', [4,0])
	Register(root, 'RBP', '0x00000000', [5,0])
	Register(root,  'CS', ' 0x00000000', [6,0])
	Register(root, 'R08', '0x00000000', [1, 1])
	Register(root, 'R09', '0x00000000', [2, 1])
	Register(root, 'R10', '0x00000000', [3, 1])
	Register(root, 'R11', '0x00000000', [4, 1])
	Register(root, 'RSP', '0x00000000', [5, 1])
	Register(root,  'SS', ' 0x00000000', [6, 1])
	# run it
	root.mainloop()


if __name__ == '__main__':
	main()