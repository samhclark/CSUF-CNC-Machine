#!/usr/bin/env python3
"""Runs the user interaface for a CNC machine

California State University, Fresno
Lyle's College of Engineering
Electrical and Computer Engineering Department
ECE 186(A/B) - Senior Design(I/II)
Fall 2018 - Spring 2019
Plasma CNC Machine Group - Computer Engineering Team

Instructor:    Dr. Aaron Stillmaker
Author:        Sam Clark
Group Members: Jarrod Smith (Computer)
               Zachary Stewart (Computer)
               Chris Doda (Mechanical)
               Bryant Pennebaker (Mechanical)
               Elvis Pizano (Mechanical)
"""

# TODO: Restructure this so that the root tk module and tcl variables can be
# passed around, allowing the GUI to be wrapped in a class, getting rid
# of all this top-level execution business.

#-----------------------------------------------------------------------------#
# Import Statements                                                           #
#-----------------------------------------------------------------------------#
import sys
import threading
import tkinter
import tkinter.filedialog
import tkinter.messagebox
from tkinter.ttk import *
from PIL import ImageTk, Image
from pyftdi import ftdi, usbtools
import gcodeparser
from parserutils import int_to_signed_word

if sys.version_info < (3,0):
    sys.exit("Python %s.%s or later is required.\n" % (3,0))

#-----------------------------------------------------------------------------#
# Global variables                                                            #
#-----------------------------------------------------------------------------#

# The root tk frame. All other tk widgets are contained within this
# TODO: Rewrite with PyQt. It should allow a more advanced viewing screen.
root = tkinter.Tk()

# The G-code parser object
gcp = None

# USB UART object
uart = ftdi.Ftdi()

# Tcl variables
tcl_DEBUG = tkinter.BooleanVar()
tcl_filepath = tkinter.StringVar()
tcl_syntax = tkinter.StringVar()
tcl_connected = tkinter.StringVar()
tcl_estop = tkinter.StringVar()
tcl_baudrate = tkinter.IntVar()

exit_flag = threading.Event()

curr_head_pos = (0,0,0)

#-----------------------------------------------------------------------------#
# Initializations                                                             #
#-----------------------------------------------------------------------------#

tcl_DEBUG.set(True)
tcl_baudrate.set(115200)
tcl_syntax.set("?")
tcl_estop.set("OK")

# Path to the filler image of the blank table
# TODO: Dynamically set this path based on install directory
# TODO: Generate this image with PIL
blank_img_path = "./assets/blank.png"

red_a = (255,0,0,255)
trans = (0,0,0,0)
cursor_bitmap = [ [trans, trans, trans, trans, trans, trans, red_a, red_a, trans, trans, trans, trans, trans, trans],
                  [trans, trans, trans, trans, trans, trans, red_a, red_a, trans, trans, trans, trans, trans, trans],
                  [trans, trans, trans, trans, trans, trans, red_a, red_a, trans, trans, trans, trans, trans, trans],
                  [trans, trans, trans, trans, trans, trans, red_a, red_a, trans, trans, trans, trans, trans, trans],
                  [trans, trans, trans, trans, trans, trans, red_a, red_a, trans, trans, trans, trans, trans, trans],
                  [trans, trans, trans, trans, trans, trans, red_a, red_a, trans, trans, trans, trans, trans, trans],
                  [red_a, red_a, red_a, red_a, red_a, red_a, red_a, red_a, red_a, red_a, red_a, red_a, red_a, red_a],
                  [red_a, red_a, red_a, red_a, red_a, red_a, red_a, red_a, red_a, red_a, red_a, red_a, red_a, red_a],
                  [trans, trans, trans, trans, trans, trans, red_a, red_a, trans, trans, trans, trans, trans, trans],
                  [trans, trans, trans, trans, trans, trans, red_a, red_a, trans, trans, trans, trans, trans, trans],
                  [trans, trans, trans, trans, trans, trans, red_a, red_a, trans, trans, trans, trans, trans, trans],
                  [trans, trans, trans, trans, trans, trans, red_a, red_a, trans, trans, trans, trans, trans, trans],
                  [trans, trans, trans, trans, trans, trans, red_a, red_a, trans, trans, trans, trans, trans, trans],
                  [trans, trans, trans, trans, trans, trans, red_a, red_a, trans, trans, trans, trans, trans, trans]]

# Handle reading file from command line arg
if len(sys.argv) > 1:
    tcl_filepath.set(sys.argv[1])
    gcp = gcodeparser.GCParser(tcl_filepath.get())
    tcl_syntax.set("OK")
    cursor = Image.new('RGBA', (14,14))
    cursor.putdata([pixel for row in cursor_bitmap for pixel in row])
    trans_canvas = Image.new('RGBA', (3200,3200), color=(0,0,0,0))
    trans_canvas.paste(cursor, box=(-7,3192))
    local_img = Image.alpha_composite(gcp.image, trans_canvas)
    #img = ImageTk.PhotoImage(local_img)
    local_img.save('./.tmpimg','BMP')
else:
    tcl_filepath.set("No file selected")
    img = ImageTk.PhotoImage(file=blank_img_path)


#-----------------------------------------------------------------------------#
# Function definitions                                                        #
#-----------------------------------------------------------------------------#
def uart_check():
    """Run in its own thread to check for USB UART connection"""
    pass

def set_pos():
    """Read from the Ftdi UART and set the current coordinates"""
    payload = uart.read_data_bytes(6)
    print(payload)
    cmd = int.from_bytes(payload[0], byteorder='big', signed=False) & 15 # 00001111
    param = int.from_bytes(payload[1], byteorder='big', signed=False) >> 4
    arg = int.from_bytes(payload[2:], byteorder='big', signed=True)

    if cmd == 10:
        if param == 0:
            curr_head_pos[0] = arg
        elif param == 1:
            curr_head_pos[1] = arg
        elif param == 2:
            curr_head_pos[2] = arg
        else:
            print("Invalid parameter read from USB")


def stub():
    """Stub function for commands that have yet to be implemented"""
    tkinter.messagebox.showerror('Stub function', message='Not implemented', \
        detail="Lorem ipsum dolor es set")

def message_boxes():
    """
    Test function for the different types of message boxes
    """
    tkinter.messagebox.showinfo('Message title', 'showinfo')
    tkinter.messagebox.showwarning('Message title', 'showwarning')
    tkinter.messagebox.showerror('Message title', 'showerror')


def open_button_handler():
    """Handles the opening the files within the GUI"""

    # Filepath and image are both modified by this method
    global tcl_filepath
    global img
    global gcp

    fp = tkinter.filedialog.askopenfilename(initialdir = ".", \
                                            title = "Select file", \
                                            filetypes = (("G-code Files","*.ngc"),
                                                         ("all files","*")))

    print(type(fp))
    print(fp)

    if fp != tuple() and fp != '':
        tcl_filepath.set(fp)
        f = open(fp, 'r')
        gcp = gcodeparser.GCParser(f)
        tcl_syntax.set("OK")
        tmp = ImageTk.PhotoImage(gcp.image.resize((640,640), resample=Image.HAMMING))
        disp.configure(image=tmp)
        disp.image = tmp
        img = tmp
        f.close()

def menu_help_about():
    """Displays a popup with About information regarding the program"""
    about_msg = """California State University, Fresno
Lyle's College of Engineering
Electrical and Computer Engineering Department
Senior Design Project
Fall 2018 - Spring 2019
Plasma CNC Machine Group - Computer Engineering Team

Instructor:    Dr. Aaron Stillmaker
Author:        Sam Clark
Group Members: Jarrod Smith (Computer)
               Zachary Stewart (Computer)
               Chris Doda (Mechanical)
               Bryant Pennebaker (Mechanical)
               Elvis Pizano (Mechanical)

Source code at: https://github.com/sclark13/CSUF-CNC-Machine
Contact author at sam.h.clark@gmail.com
"""
    tkinter.messagebox.showinfo('About CNC Control Program', \
        message="About CNC Control Program", detail=about_msg)

def edit_handler():
    return

def start_handler():
    #with open(tcl_filepath.get(), 'r') as f:
    #    gcp = gcodeparser.GCParser(f)
    if tcl_connected.get() == 'OK':
        with open('hcode.log', 'w') as out:
            for c in gcp.CUTS:
                out.write(str(c))
                out.write('\n')
        with open('packets.log', 'wb') as out:
            for p in gcp.PACKETS:
                out.write(p)
                uart.write_data(p)
    else:
        tkinter.messagebox.showerror('Error', 'No USB device detected.')
    # TODO: Catch and raise errors here

def inc_x_handler():
    if tcl_connected.get() == 'OK':
        uart.write_data(b'\x08\x00\x00\x00\x00\x00')
        uart.write_data(b'\x10\x10' + int_to_signed_word(curr_head_pos[0]+10))
        uart.write_data(b'\x20\x11' + int_to_signed_word(curr_head_pos[1]))
        uart.write_data(b'\x30\x12' + int_to_signed_word(curr_head_pos[2]))
        uart.write_data(b'\x4F\x20\x00\x00\x00\x00')

def dec_x_handler():
    if tcl_connected.get() == 'OK':
        uart.write_data(b'\x08\x00\x00\x00\x00\x00')
        uart.write_data(b'\x10\x10' + int_to_signed_word(curr_head_pos[0]-10))
        uart.write_data(b'\x20\x11' + int_to_signed_word(curr_head_pos[1]))
        uart.write_data(b'\x30\x12' + int_to_signed_word(curr_head_pos[2]))
        uart.write_data(b'\x4F\x20\x00\x00\x00\x00')

def inc_y_handler():
    if tcl_connected.get() == 'OK':
        uart.write_data(b'\x08\x00\x00\x00\x00\x00')
        uart.write_data(b'\x10\x10' + int_to_signed_word(curr_head_pos[0]))
        uart.write_data(b'\x20\x11' + int_to_signed_word(curr_head_pos[1]+10))
        uart.write_data(b'\x30\x12' + int_to_signed_word(curr_head_pos[2]))
        uart.write_data(b'\x4F\x20\x00\x00\x00\x00')

def dec_y_handler():
    if tcl_connected.get() == 'OK':
        uart.write_data(b'\x08\x00\x00\x00\x00\x00')
        uart.write_data(b'\x10\x10' + int_to_signed_word(curr_head_pos[0]))
        uart.write_data(b'\x20\x11' + int_to_signed_word(curr_head_pos[1]-10))
        uart.write_data(b'\x30\x12' + int_to_signed_word(curr_head_pos[2]))
        uart.write_data(b'\x4F\x20\x00\x00\x00\x00')

def inc_z_handler():
    if tcl_connected.get() == 'OK':
        uart.write_data(b'\x08\x00\x00\x00\x00\x00')
        uart.write_data(b'\x10\x10' + int_to_signed_word(curr_head_pos[0]))
        uart.write_data(b'\x20\x11' + int_to_signed_word(curr_head_pos[1]))
        uart.write_data(b'\x30\x12' + int_to_signed_word(curr_head_pos[2]+10))
        uart.write_data(b'\x4F\x20\x00\x00\x00\x00')

def dec_z_handler():
    if tcl_connected.get() == 'OK':
        uart.write_data(b'\x08\x00\x00\x00\x00\x00')
        uart.write_data(b'\x10\x10' + int_to_signed_word(curr_head_pos[0]))
        uart.write_data(b'\x20\x11' + int_to_signed_word(curr_head_pos[1]))
        uart.write_data(b'\x30\x12' + int_to_signed_word(curr_head_pos[2]-10))
        uart.write_data(b'\x4F\x20\x00\x00\x00\x00')

def exit_handler():
    """Handles the exit button from the File menu"""
    exit_flag.set()
    root.destroy()


#-----------------------------------------------------------------------------#
# Top level execution                                                         #
#-----------------------------------------------------------------------------#

# Initialize UART
try:
    uart.open_from_url("ftdi:///1")
    uart.set_baudrate(tcl_baudrate.get())
    uart.set_rts(True)
    tcl_connected.set("OK")
except (usbtools.UsbToolsError):
    tcl_connected.set("X")
    # TODO: Fix threading issue to resolve USB connection issue
    #usb_thread = threading.Thread(target=uart_check)
    #usb_thread.start()
    print("No USB device detected")

# Configuration of top level window
root.title("CNC Control Program")

# Menu bar
menubar = tkinter.Menu(root)

file_menu = tkinter.Menu(menubar, tearoff=0)
file_menu.add_command(label='Open', command=open_button_handler)
file_menu.add_command(label='Exit', command=exit_handler)
menubar.add_cascade(label='File', menu=file_menu)

# Add help item to menubar
help_menu = tkinter.Menu(menubar, tearoff=0)
help_menu.add_command(label='About', command=menu_help_about)
menubar.add_cascade(label='Help', menu=help_menu)

# Image frame
# TODO: Allow resizing of viewing windows
# Not sure this can be done simultaneously with cursor in Tk
display_frame = tkinter.Frame(root, width=640, height=640)
disp = Label(display_frame, image=img)
disp.pack(side="bottom", fill="both", expand="yes")
display_frame.grid(column=0, row=0)

# Right Half Frame
right_frame = tkinter.Frame(root)

# TODO: Implement progress bar
# Progress frame
#progress_frame = tkinter.Frame(root)
#pb = Progressbar(root, orient=tkinter.HORIZONTAL, length=400, mode='indeterminate')
#progress_frame.grid(column=1, row=0)

# Status frame
status_frame = tkinter.Frame(right_frame)

syntax_check_text = Label(status_frame, text="Syntax")
syntax_check_text.grid(column=0, row=0)
syntax_check_icon = Label(status_frame, textvariable=tcl_syntax)
syntax_check_icon.grid(column=0, row=1)

connected_status_text = Label(status_frame, text="Connected")
connected_status_text.grid(column=1, row=0)
connected_status_icon = Label(status_frame, textvariable=tcl_connected)
connected_status_icon.grid(column=1, row=1)

estop_status_text = Label(status_frame, text="E-stop")
estop_status_text.grid(column=2, row=0)
estop_status_icon = Label(status_frame, textvariable=tcl_estop)
estop_status_icon.grid(column=2, row=1)

status_frame.grid(column=0, row=0)

# File frame
f_frame = tkinter.Frame(right_frame)
lbl = Label(f_frame, text="Please select a G-Code file:")
lbl.pack()
txt_path = Entry(f_frame, width=25, textvariable=tcl_filepath)
txt_path.pack()
btn_browse = Button(f_frame, text="Browse", command=open_button_handler)
btn_browse.pack()
btn_start = Button(f_frame, text="Start", command=start_handler)
btn_start.pack()
f_frame.grid(column=0, row=1)

# Control frame
control_frame = tkinter.Frame(right_frame)
btn_x_inc = Button(control_frame, text="X+", command=inc_x_handler)
btn_x_inc.grid(row=0, column=0)
btn_x_dec = Button(control_frame, text="X-", command=dec_x_handler)
btn_x_dec.grid(row=1, column=0)
btn_y_inc = Button(control_frame, text="Y+", command=inc_y_handler)
btn_y_inc.grid(row=0, column=1)
btn_y_dec = Button(control_frame, text="Y-", command=dec_y_handler)
btn_y_dec.grid(row=1, column=1)
btn_z_inc = Button(control_frame, text="Z+", command=inc_z_handler)
btn_z_inc.grid(row=0, column=2)
btn_z_dec = Button(control_frame, text="Z-", command=dec_z_handler)
btn_z_dec.grid(row=1, column=2)
control_frame.grid(row=2, column=0)

right_frame.grid(column=1, row=0)

root.config(menu=menubar)
root.mainloop()

# TODO: Redo this block. Should basically detect when the USB is connected,
# indicate if it is connected, allow connect/reconnect during execution,
# prohibit running while disconnected.
while(1):
    # This try/except block catches when the GUI has been closed and closes
    # the underlying Python interpreter
    try:
        root.winfo_screen()
    except (tkinter._tkinter.TclError):
        sys.exit()

    # This try/except looks for the the USB to be connected
    if tcl_connected.get() == "OK":
        set_pos()
    else:
        try:
            uart.open_from_url("ftdi:///1")
            uart.set_baudrate(tcl_baudrate.get())
            uart.set_rts(True)
            tcl_connected.set("OK")
        except (usbtools.UsbToolsError):
            pass

