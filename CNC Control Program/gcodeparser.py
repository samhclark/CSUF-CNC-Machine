#!/usr/bin/env python3
"""Interacts with and parses G-code files.

Primarily used through instantiation of the GCParser object. The object has methods
that allow the user to interact with and manipulate a given G-code file.

Usage:
    parser_object = GCParser( <gcode file object> )

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
#---------------------------------------------------------------------------------------#
# Import Statements                                                                     #
#---------------------------------------------------------------------------------------#
from parserutils import *
import math
from PIL import Image, ImageDraw, ImageFilter

#---------------------------------------------------------------------------------------#
# Class definition                                                                      #
#---------------------------------------------------------------------------------------#
class GCParser:
    """G-code parser object.

    Usage: Provide GCParser with a file object or file path as a string
    """
    def __init__(self, infile):
        # Parameters of CNC machine
        self._TABLE_LENGTH_IN = 48.0        # Length of cutting area in inches
        self._TABLE_WIDTH_IN = 48.0         # Width of cutting area in inches
        self._TABLE_LENGTH_STEPS = 3200     # Num steps to cross length of cutting area
        self._TABLE_WIDTH_STEPS = 3200      # Num steps to cross width of cutting area
        self._H_STEPS_PER_INCH = 66.67      # Number of steps per horizontal inch
        self._V_STEPS_PER_INCH = 600        # Number of steps per vertical inch
        self._PKT_SEQ_NUM = 0               # Sequence number for the packets
        self._CMD_SEQ_NUM = 0               # Sequence number for the command

        self._CANON_PLANE_XY = 0            # Constant for XY plane
        self._CANON_PLANE_YZ = 1            # Constant for YZ plane
        self._CANON_PLANE_XZ = 2            # Constant for XZ plane
        self._CANON_DIST_MODE_ABS = 0       # Constant for absolute distance mode
        self._CANON_DIST_MODE_REL = 1       # Constant for relative distance mode
        self._CANON_UNITS_INCH = 0          # Constant for using inches as units
        self._CANON_UNITS_MM = 1            # Constant for using mm as units

        self._X_STEP_POS = 0                # X-coord of current position in steps
        self._Y_STEP_POS = 0                # Y-coord of current position in steps
        self._Z_STEP_POS = 0                # Z-coord of current position in steps
        self._X = 0.0                       # Initial x position is 0.0
        self._Y = 0.0                       # Initial y position is 0.0
        self._Z = 0.0                       # Initial z position is 0.0
        self._I = 0.0                       # Initial i position is 0.0
        self._J = 0.0                       # Initial j position is 0.0
        self._THICKNESS = 0                 # Thickness of material in units
        self._FEED_RATE = None              # Feed rate, units per min
        self._X_OFFSET = 0.0                # XYZ offsets are used in
        self._Y_OFFSET = 0.0                # translation of the cut, not in
        self._Z_OFFSET = 0.0                # the G2 or G3 commands

        # Default cut parameters
        self._UNITS = self._CANON_UNITS_INCH
        self._DISTANCE_MODE = self._CANON_DIST_MODE_ABS
        self._PLANE = self._CANON_PLANE_XY

        # Coordinates of bounding box
        self.MAX_X = 0
        self.MAX_Y = 0
        self.MAX_Z = 0
        self.MIN_X = 0
        self.MIN_Y = 0
        self.MIN_Z = 0

        # File object or path for G-Code file
        self.GCODE_FILE = infile

        # List of cuts to make
        self.CUTS = list()
        self.PACKETS = list()
        self.parse_file()

        if self.MIN_X < 0.0 or self.MIN_Y < 0.0 or self.MIN_Z < 0.0:
            self.translate_cuts(abs(self.MIN_X), abs(self.MIN_Y), abs(self.MIN_Z))
        else:
            self.make_packets()

        self.image = self.create_cut_image()

    def create_cut_image(self):
        """Creates and return an image file of the cut"""
        raw_canvas_size = (self._TABLE_WIDTH_STEPS,self._TABLE_LENGTH_STEPS)
        white = (255, 255, 255, 255)
        black = (0, 0, 0, 255)
        im = Image.new('RGBA', raw_canvas_size, color=black)
        draw = ImageDraw.Draw(im)
        # Fake iterating through the drawing
        curr_pos = (0,0)
        for cut in self.CUTS:
            x = cut[1][1]
            y = self._TABLE_LENGTH_STEPS-cut[2][1]
            if cut[0] == 'H0':
                curr_pos = (x, y)
            elif cut[0] == 'H1':
                line = [curr_pos, (x, y)]
                draw.line(line,fill=white) # optional width=px option
                curr_pos = (x, y)
            elif cut[0] == 'H2':
                i = cut[4][1]
                j = self._TABLE_LENGTH_STEPS-cut[5][1]
                print("DEBUG: H2 drawing")
                print("Current position: {0[0]},{0[1]}".format(curr_pos))
                print("(x,y): {},{}".format(x,y))
                print("(i,j): {},{}".format(i,j))
                # Construct bounding box
                radius = math.ceil(math.sqrt(abs(x-i)**2 + abs(y-j)**2))
                top_left = (i-radius, j-radius)
                bottom_right = (i+radius, j+radius)
                bounding_box = [top_left, bottom_right]
                print("Radius = %s" % radius)
                print("Bounding box = {0[0]},{0[1]}".format(bounding_box))

                theta1 = math.atan2(curr_pos[1]-j, curr_pos[0]-i)
                if theta1 < 0:
                    theta1 = theta1 + 2*math.pi
                theta1 = math.degrees(theta1)
                print("Theta 1: %s" % theta1)

                theta2 = math.atan2(y-j, x-i)
                if theta2 < 0:
                    theta2 = theta2 + 2*math.pi
                theta2 = math.degrees(theta2)
                print("Theta 2: %s" % theta2)

                draw.arc(bounding_box, theta1, theta2, fill=white)
                curr_pos = (x, y)

            elif cut[0] == 'H3':
                i = cut[4][1]
                j = self._TABLE_LENGTH_STEPS-cut[5][1]
                # Construct bounding box
                radius = math.ceil(math.sqrt(abs(x-i)**2 + abs(y-j)**2))
                top_left = (i-radius, j-radius)
                bottom_right = (i+radius, j+radius)
                bounding_box = [top_left, bottom_right]

                theta1 = math.atan2(curr_pos[1]-j, curr_pos[0]-i)
                if theta1 < 0:
                    theta1 = theta1 + 2*math.pi
                theta1 = math.degrees(theta1)

                theta2 = math.atan2(y-j, x-i)
                if theta2 < 0:
                    theta2 = theta2 + 2*math.pi
                theta2 = math.degrees(theta2)

                draw.arc(bounding_box, theta2, theta1, fill=white)

                curr_pos = (x, y)
        #small_im = im.resize((400,400), resample=Image.HAMMING)
        #small_im.show()
        #im.show()
        #return im.resize((640,640), resample=Image.HAMMING)
        return im

    @staticmethod
    def not_handled(line):
        """Stub function for commands that aren't yet implemented"""
        print("%s not handled" % line[0])


    def read_and_set_coords(self, command):
        """Reads and sets xyz, ijk, and p params for movement commands

        Returns: 7-tuple of the parameters that were read
        """
        _x = _y = _z = _i = _j = _k = _p = None
        for prev, curr, nxt in prev_next(command):
            if curr.startswith(('X','x')):
                if curr[1:] != '':
                    _x = float(curr[1:])
                else:
                    _x = float(nxt)
            elif curr.startswith(('Y','y')):
                if curr[1:] != '':
                    _y = float(curr[1:])
                else:
                    _y = float(nxt)
            elif curr.startswith(('Z','z')):
                if curr[1:] != '':
                    _z = float(curr[1:])
                else:
                    _z = float(nxt)
            elif curr.startswith(('I','i')):
                if curr[1:] != '':
                    _i = float(curr[1:])
                else:
                    _i = float(nxt)
            elif curr.startswith(('J','j')):
                if curr[1:] != '':
                    _j = float(curr[1:])
                else:
                    _j = float(nxt)
            elif curr.startswith(('K','k')):
                if curr[1:] != '':
                    _k = float(curr[1:])
                else:
                    _k = float(nxt)
            elif curr.startswith(('P','p')):
                if curr[1:] != '':
                    _p = int(curr[1:])
                else:
                    _p = int(nxt)
        return _x, _y, _z, _i, _j, _k, _p;


    def linear_move(self, line):
        """Linear move at the feed rate
        Corresponds to a G0 or G1 command

        Returns: H-code 4-tuple
        """
        # TODO: Allow use of millimeters
        print("Debug: Linear move")
        print(line)
        new_x, new_y, new_z, new_i, new_j, new_k, new_p = self.read_and_set_coords(line)
        if self._DISTANCE_MODE == self._CANON_DIST_MODE_ABS:
            if new_x is not None:
                self._X = new_x
                self._X_STEP_POS = int(self._X * self._H_STEPS_PER_INCH)
            if new_y is not None:
                self._Y = new_y
                self._Y_STEP_POS = int(self._Y * self._H_STEPS_PER_INCH)
            if new_z is not None:
                self._Z = new_z
                self._Z_STEP_POS = int(self._Z * self._V_STEPS_PER_INCH)
        elif self._DISTANCE_MODE == self._CANON_DIST_MODE_REL:
            if new_x is not None:
                self._X = self._X + new_x
                self._X_STEP_POS = int(self._X * self._H_STEPS_PER_INCH)
            if new_y is not None:
                self._Y = self._Y + new_y
                self._Y_STEP_POS = int(self._Y * self._H_STEPS_PER_INCH)
            if new_z is not None:
                self._Z = self._Z + new_z
                self._Z_STEP_POS = int(self._Z * self._V_STEPS_PER_INCH)
        if line[0] == 'G0' or line[0] == 'G00':
            h_cmd = 'H0'
        else:
            h_cmd = 'H1'
        h_code = (h_cmd,
                  ('X',self._X_STEP_POS),
                  ('Y',self._Y_STEP_POS),
                  ('Z',self._Z_STEP_POS))
        self.MAX_X = max(self.MAX_X, self._X)
        self.MAX_Y = max(self.MAX_Y, self._Y)
        self.MAX_Z = max(self.MAX_Z, self._Z)
        self.MIN_X = min(self.MIN_X, self._X)
        self.MIN_Y = min(self.MIN_Y, self._Y)
        self.MIN_Z = min(self.MIN_Z, self._Z)
        print(h_code)
        return h_code


    def arc_move(self, line):
        """
        Arc move, at arc feed rate
        Correspond to a G2 or G3 command
        Supports center format arcs, not radius format arcs

        Returns: H-code 7-tuple
        """
        # TODO: Allow use of millimeters
        print("Arc move")
        print(line)
        new_x, new_y, new_z, new_i, new_j, new_k, new_p = self.read_and_set_coords(line)

        # Handle the offsets first, since they are always relative coordinates
        # from the initial position
        if new_i is not None:
            center_x = self._X + new_i
        else:
            center_x = self._X
        if new_j is not None:
            center_y = self._Y + new_j
        else:
            center_y = self._Y

        if self._DISTANCE_MODE == self._CANON_DIST_MODE_ABS:
            if new_x is not None:
                self._X = new_x
            if new_y is not None:
                self._Y = new_y
            if new_z is not None:
                self._Z = new_z
        elif self._DISTANCE_MODE == self._CANON_DIST_MODE_REL:
            if new_x is not None:
                self._X = self._X + new_x
            if new_y is not None:
                self._Y = self._Y + new_y
            if new_z is not None:
                self._Z = self._Z + new_z

        self._X_STEP_POS = int(self._X * self._H_STEPS_PER_INCH)
        self._Y_STEP_POS = int(self._Y * self._H_STEPS_PER_INCH)
        self._Z_STEP_POS = int(self._Z * self._V_STEPS_PER_INCH)
        center_x = int(center_x * self._H_STEPS_PER_INCH)
        center_y = int(center_y * self._H_STEPS_PER_INCH)

        if line[0] == 'G2' or line[0] == 'G02':
            h_cmd = 'H2'
        else:
            h_cmd = 'H3'
        h_code = (h_cmd,
                ('X',self._X_STEP_POS),
                ('Y',self._Y_STEP_POS),
                ('Z',self._Z_STEP_POS),
                ('I',center_x),
                ('J',center_y),
                ('K',self._Z_STEP_POS))
        print(h_code)
        # Note this max/min calc is incorrect
        # This is only considering the start and end point, not the path
        # TODO: Fix this error
        self.MAX_X = max(self.MAX_X, self._X)
        self.MAX_Y = max(self.MAX_Y, self._Y)
        self.MAX_Z = max(self.MAX_Z, self._Z)
        self.MIN_X = min(self.MIN_X, self._X)
        self.MIN_Y = min(self.MIN_Y, self._Y)
        self.MIN_Z = min(self.MIN_Z, self._Z)
        return h_code


    def parse_line(self, code):
        """Handles the parsing of each individual line

        Only handles fully formed G-code lines, not lines that
        rely on modal behavior.

        Returns: H-code tuple (if movement command), or None
        """
        valid_codes = set({'G0', 'G00', 'G1', 'G01', 'G2', 'G02', 'G3', 'G03',
                           'G20', 'G21', 'G90', 'G91'})
        if code[0] not in valid_codes:
            self.not_handled(code)
        elif code[0] == 'G0' or code[0] == 'G00': # Rapid Move
            return self.linear_move(code)
        elif code[0] == 'G1' or code[0] == 'G01': # Linear Move
            return self.linear_move(code)
        elif code[0] == 'G2' or code[0] == 'G02': # Arc Move clockwise
            return self.arc_move(code)
        elif code[0] == 'G3' or code[0] == 'G03': # Arc Move counterclockwise
            return self.arc_move(code)
        elif code[0] == 'G20': # Units inches
            self._UNITS = self._CANON_UNITS_INCH
        elif code[0] == 'G21': # Units mm
            self._UNITS = self._CANON_UNITS_MM
        elif code[0] == 'G90': # Distance Mode Absolute
            self._DISTANCE_MODE = self._CANON_DIST_MODE_ABS
        elif code[0] == 'G91': # Distance Mode Relative
            self._DISTANCE_MODE = self._CANON_DIST_MODE_REL
        else:
            print("Line not caught:")
            print(code)
        return None


    def parse_file(self):
        """Parses the input file and writes the parsed version to the output file"""

        # TODO: Fix this to use try, except, else, finally
        try:
            file_offset = self.GCODE_FILE.tell()  # Raises AttributeError if not a file
            if file_offset != 0:
                self.GCODE_FILE.seek(0)  # Pretty sure I don't need this anymore
            for line in self.GCODE_FILE:
                if line[0] != '(' and line[0] != '\n':
                    code = strip_parens(line)
                    parsed = self.parse_line(code.split())
                    if parsed is not None:
                        self.CUTS.append(parsed)
        except AttributeError:
            with open(self.GCODE_FILE, 'r') as f:
                for line in f:
                    if line[0] != '(' and line[0] != '\n':
                        code = strip_parens(line)
                        parsed = self.parse_line(code.split())
                        if parsed is not None:
                            self.CUTS.append(parsed)


    def make_packets(self):
        """Create list of packet bytes from the CUTS list"""

        # Look up table that maps integers to single hex chars
        # Using this because the normal conversion give a whole byte
        # back and this needs to be strictly one char
        seq_num_lut = dict({0:'0', 1:'1', 2:'2', 3:'3', 4:'4', 5:'5', 6:'6', 7:'7',
            8:'8', 9:'9', 10:'A', 11:'B', 12:'C', 13:'D', 14:'E', 15:'F'})

        param_lut = dict({'X':'0', 'Y':'1', 'Z':'2', 'I':'3', 'J':'4', 'K':'5', 'P':'6'})

        cmd_lut = dict({'H0':'0',   # Rapid move
                        'H1':'1',   # Linear cut
                        'H2':'2',   # Clockwise cut
                        'H3':'3',   # Counterclockwise cut
                        'H4':'4',   # Dwell (pause)
                        'CAL':'5',  # Begin calibration
                        'COM':'6',  # Begin communication test
                        'H29':'8',  # Start packet sequence
                        'ACK':'9',  # Acknowledge packet sequence
                        'H30':'F'}) # Stop packet sequence

        # Construct start packet
        header = bytes.fromhex(seq_num_lut[self._PKT_SEQ_NUM] + cmd_lut['H29'] +
                               seq_num_lut[self._CMD_SEQ_NUM] + '0')
        data = bytes.fromhex('DEAD BEEF')
        start_packet = header + data
        self.PACKETS.append(start_packet)
        self._PKT_SEQ_NUM = four_bit_wrap(self._PKT_SEQ_NUM + 1)
        self._CMD_SEQ_NUM = four_bit_wrap(self._CMD_SEQ_NUM + 1)

        # Construct the cut packets
        for cut in self.CUTS:
            cmd = cut[0]
            for coord in cut[1:]:
                header_str = (seq_num_lut[self._PKT_SEQ_NUM] + cmd_lut[cmd] +
                              seq_num_lut[self._CMD_SEQ_NUM] + param_lut[coord[0]])
                header = bytes.fromhex(header_str)
                data = int_to_signed_word(coord[1])
                ptemp = header + data
                self.PACKETS.append(ptemp)
                self._PKT_SEQ_NUM = four_bit_wrap(self._PKT_SEQ_NUM + 1)
            self._CMD_SEQ_NUM = four_bit_wrap(self._CMD_SEQ_NUM + 1)

        # Construct the end packet
        header = bytes.fromhex(seq_num_lut[self._PKT_SEQ_NUM] + cmd_lut['H30'] +
                               seq_num_lut[self._CMD_SEQ_NUM] + '0')
        data = bytes.fromhex('DEAD BEEF')
        end_packet = header + data
        self.PACKETS.append(end_packet)
        self._PKT_SEQ_NUM = four_bit_wrap(self._PKT_SEQ_NUM + 1)
        self._CMD_SEQ_NUM = four_bit_wrap(self._CMD_SEQ_NUM + 1)


    def translate_cuts(self, x_offset=0.0, y_offset=0.0, z_offset=0.0):
        """Translate the cuts across the table by a given offset"""
        self._X_OFFSET = self._X_OFFSET + x_offset
        self._Y_OFFSET = self._Y_OFFSET + y_offset
        self._Z_OFFSET = self._Z_OFFSET + z_offset

        self._X = self._X + self._X_OFFSET
        self._Y = self._Y + self._Y_OFFSET
        self._Z = self._Z + self._Z_OFFSET
        self._I = self._X + self._X_OFFSET
        self._J = self._Y + self._Y_OFFSET
        self._K = self._Z + self._Z_OFFSET

        self._X_STEP_POS = self._X * self._H_STEPS_PER_INCH
        self._Y_STEP_POS = self._Y * self._H_STEPS_PER_INCH
        self._Z_STEP_POS = self._Z * self._V_STEPS_PER_INCH

        x_step_offset = int(self._X_OFFSET * self._H_STEPS_PER_INCH)
        y_step_offset = int(self._Y_OFFSET * self._H_STEPS_PER_INCH)
        z_step_offset = int(self._Z_OFFSET * self._V_STEPS_PER_INCH)

        newcuts = list()
        for cut in self.CUTS:
            if len(cut) == 4: # H0 or H1 cmd
                newcuts.append( (cut[0],
                                 (cut[1][0], cut[1][1] + x_step_offset),
                                 (cut[2][0], cut[2][1] + y_step_offset),
                                 (cut[3][0], cut[3][1] + z_step_offset)))
            elif len(cut) == 7: # H2 or H3 cmd
                newcuts.append( (cut[0],
                                 (cut[1][0], cut[1][1] + x_step_offset),
                                 (cut[2][0], cut[2][1] + y_step_offset),
                                 (cut[3][0], cut[3][1] + z_step_offset),
                                 (cut[4][0], cut[4][1] + x_step_offset),
                                 (cut[5][0], cut[5][1] + y_step_offset),
                                 (cut[6][0], cut[6][1] + z_step_offset)))
        self.CUTS.clear()
        for cut in newcuts:
            self.CUTS.append(cut)

        self._PKT_SEQ_NUM = 0
        self._CMD_SEQ_NUM = 0
        self.PACKETS.clear()
        self.make_packets()
