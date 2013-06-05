import serial, time, binascii, os

possibleSerialPorts = [d for d in os.listdir("/dev") if "ttyACM" in d]

spAddy = os.path.join("/dev", possibleSerialPorts[0])
print( "Using serial port: " + spAddy)
sp = serial.Serial(spAddy, baudrate=9600, timeout=1)
sp.flushInput()

def readROM():
    sp.write("r")
    return sp.read(9)

def readFlash():
    sp.write("f")
    return sp.read(512)

def pollForChip():
    sp.write("x")
    return sp.read(1) == "p"


def waitForChip():
    while True:
        print "Polling for chip. . ."
        if pollForChip(): break
        time.sleep(0.5)
    print "\tfound!"

def echo():
    sp.write("e")
    if sp.read() != "e": print "No echo :("



def getPathForRom(rom):
    return os.path.join("flash-dumps", binascii.hexlify(rom))

def dump2433():
    waitForChip()

    # Read the ROM.  echo its hex
    rom = readROM()
    print(binascii.hexlify(rom) + " (" + str(len(rom)) + ")")

    # Read it 5x and make sure they all match
    for reread in range(5): assert rom == readROM(), "Mismatched ROM read.  Aborting"

    # Read the flash
    flash = readFlash()
    print "Flash Read: %i bytes" % len(flash)

    # Read it 5x and make sure they all match
    for reread in range(5): assert flash == readFlash(), "Mismatched flash read.  Aborting"

    # Make a path to store the dumped data
    fPath = getPathForRom(rom)
    if not os.path.isdir(fPath): os.mkdir(fPath)

    # Mark them by the time, so we can reload the oldest one later
    fullNamePath = os.path.join(fPath, str(time.time()) + ".bin")
    open(fullNamePath, "wb").write(flash)

    # Echo our success
    print "Wrote", fullNamePath

def write2433(flash):
    waitForChip()

    # Retrieve the rom
    currentRom = readROM()

    # Make sure it matches
    assert currentRom == readROM(), "ROM's do not match!"

    #stupid check the flash size to 512
    assert len(flash) == 512, "ROM size is not 512  ({0}".format(len(flash))

    # Transmit the new flash
    sp.write("w")
    sp.write(flash)

    # Read the reported success flag
    print sp.read(2**15)
    print readFlash() == flash

def writeOldestToChip():
    waitForChip()
    currentRom = readROM()

    print("Detected Chip with ROM: " + binascii.hexlify(currentRom))

    romDIR = getPathForRom(currentRom)
    flashes = os.listdir(romDIR)
    flashes.sort()
    flashPath = os.path.join(romDIR, flashes[0])

    print("Attempting to flash with image from " + flashPath)
    oldestFlashData = open(flashPath).read()
    write2433(oldestFlashData)

def clearFlash():
    waitForChip()
    currentRom = readROM()

    print("Detected Chip with ROM: " + binascii.hexlify(currentRom))

    write2433("\0" * 512)


clearFlash()

# print binascii.hexlify(readFlash())
writeOldestToChip()

