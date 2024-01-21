/**
 * FILENAME :       floppy.c
 *
 * AUTHOR :         Teemu Voipio / Ruben Lohberg
 *
 * START DATE :     8 Mar 2007
 *
 * LAST UPDATE :    21 Jan 2024
 *
 * PROJECT :        RubenOS
 *
 * DESCRIPTION :
 *  A primitive floppy driver. Writing a proper driver for a floppy disk, or
 *  maybe ATA disk using PIO is in itself a project about the complexity of
 *  this os. So instead of writing a proper driver, I made changes to a working
 *  example from the OSdev forum. This driver will most likely only work for
 *  virtual floppy drives.
 *
 *  This is the part of the operating system, I'm the least proud of.
 */

#include "floppy.h"
#include "low_level.h"
#include "screen.h"
#include "bool.h"
#include "timer.h"

/*
    The MSR byte: [read-only]
    -------------

    7   6   5    4    3    2    1    0
    MRQ DIO NDMA BUSY ACTD ACTC ACTB ACTA

    MRQ is 1 when FIFO is ready (test before read/write)
    DIO tells if controller expects write (1) or read (0)

    NDMA tells if controller is in DMA mode (1 = no-DMA, 0 = DMA)
    BUSY tells if controller is executing a command (1=busy)

    ACTA, ACTB, ACTC, ACTD tell which drives position/calibrate (1=yes)
*/

/*
    The DOR byte: [write-only]
    -------------

    7    6    5    4    3   2    1   0
    MOTD MOTC MOTB MOTA DMA NRST DR1 DR0

    DR1 and DR0 together select "current drive" = a/00, b/01, c/10, d/11
    MOTA, MOTB, MOTC, MOTD control motors for the four drives (1=on)

    DMA line enables (1 = enable) interrupts and DMA
    NRST is "not reset" so controller is enabled when it's 1
*/

/** Color attribute for printing floppy messages: Magenta */
#define FLOPPY_PRINT_ATTRIBUTE 0x05

/** Base address for the primary floppydrive controller */
#define FLOPPY_BASE 0x03F0

/** The registers of interest. There are more, but we only use these here. */
// digital output register
#define FLOPPY_DOR 2
// master status register, read only
#define FLOPPY_MSR 4
// data FIFO (first in first out buffer), in DMA (direct memory access)
// operation for commands
#define FLOPPY_FIFO 5
// configuration control register, write only
#define FLOPPY_CCR 7

/** The commands of interest. There are more, but we only use these here. */

#define CMD_SPECIFY 3
#define CMD_WRITE_DATA 5
#define CMD_READ_DATA 6
#define CMD_RECALIBRATE 7
#define CMD_SENSE_INTERRUPT 8
#define CMD_SEEK 15

/** Floppy motor states*/

#define FLOPPY_MOTOR_OFF 0
#define FLOPPY_MOTOR_ON 1

/** Counter for floppy motor ticks */
static volatile int floppy_motor_ticks = 0;
/** Flag for current motor state (off/on) */
static volatile int floppy_motor_state = 0;

/**
 * Floppy Direct Memory Access Buffer.
 * Data can be read from the floppy to the buffer, or be written from the buffer
 * to the floppy
 */
char floppy_dmabuf[FLOPPY_DMA_LENGTH];

/** Types of floppy disks */
static char *drive_types[8] = {
    "none",
    "360kB 5.25\"",
    "1.2MB 5.25\"",
    "720kB 3.5\"",
    "1.44MB 3.5\"",
    "2.88MB 3.5\"",
    "unknown type",
    "unknown type"};

// function declarations

void wait_for_interrupt();
void floppy_motor(int onoff);

/**
 * Detect drives and print type
 */
void floppy_detect_drives()
{
    // ask cmos to give us the drive type
    port_byte_out(0x70, 0x10);
    // read the drive type
    unsigned char drives = port_byte_in(0x71);

    print(" - Floppy drive 0: ", FLOPPY_PRINT_ATTRIBUTE);
    print(drive_types[drives >> 4], FLOPPY_PRINT_ATTRIBUTE);
    print("\n", FLOPPY_PRINT_ATTRIBUTE);
}

/**
 * Poll until the floppy ready bit is set
 */
void wait_floppy_ready()
{
    while (!(0x80 & port_byte_in(FLOPPY_BASE + FLOPPY_MSR)))
        ;
}

/**
 * Write a command to a floppy controller.
 *
 * @param cmd command to write
 */
void floppy_write_cmd(char cmd)
{
    wait_floppy_ready();
    // write command
    port_byte_out(FLOPPY_BASE + FLOPPY_FIFO, cmd);
}

/**
 * Read data from the floppy's FIFO register
 */
unsigned char floppy_read_data()
{
    wait_floppy_ready();
    return port_byte_in(FLOPPY_BASE + FLOPPY_FIFO);
}

/**
 * Read the interrupt state of a floppy controller.
 *
 * @param st0 where to write the status
 * @param cyl where to write the cylinder number
 */
void floppy_check_interrupt(int *st0, int *cyl)
{
    // reqeust interrupt data
    floppy_write_cmd(CMD_SENSE_INTERRUPT);

    // get interrupt data from FIFO register
    *st0 = floppy_read_data();
    *cyl = floppy_read_data();
}

/**
 * Move to cylinder 0, which calibrates the drive
 */
int floppy_calibrate()
{
    // set to bogus cylinder
    int i, st0, cyl = -1;
    // turn on motor
    floppy_motor(FLOPPY_MOTOR_ON);

    for (i = 0; i < 10; i++)
    {
        // Attempt to positions head to cylinder 0
        floppy_write_cmd(CMD_RECALIBRATE);
        // argument is drive, we only support drive 0
        floppy_write_cmd(0);

        wait_for_interrupt();
        floppy_check_interrupt(&st0, &cyl);

        if (!cyl)
        { // found cylinder 0 ?
            floppy_motor(FLOPPY_MOTOR_OFF);
            return 0;
        }
    }
    print("floppy_calibrate: 10 retries exhausted\n", FLOPPY_PRINT_ATTRIBUTE);
    floppy_motor(FLOPPY_MOTOR_OFF);
    return -1;
}

/**
 * Reset the floppy controller
 */
int floppy_reset()
{
    // disable controller
    port_byte_out(FLOPPY_BASE + FLOPPY_DOR, 0x00);
    // enable controller
    port_byte_out(FLOPPY_BASE + FLOPPY_DOR, 0x0C);

    wait_for_interrupt();
    // read clear interrupt data
    {
        // ignore these here..
        int st0, cyl;
        floppy_check_interrupt(&st0, &cyl);
    }

    // set transfer speed 500kb/s
    port_byte_out(FLOPPY_BASE + FLOPPY_CCR, 0x00);

    //  - 1st byte is: bits[7:4] = steprate, bits[3:0] = head unload time
    //  - 2nd byte is: bits[7:1] = head load time, bit[0] = no-DMA
    //
    //  steprate    = (8.0ms - entry*0.5ms)*(1MB/s / xfer_rate)
    //  head_unload = 8ms * entry * (1MB/s / xfer_rate), where entry 0 -> 16
    //  head_load   = 1ms * entry * (1MB/s / xfer_rate), where entry 0 -> 128
    //
    floppy_write_cmd(CMD_SPECIFY);
    floppy_write_cmd(0xdf); /* steprate = 3ms, unload time = 240ms */
    floppy_write_cmd(0x02); /* load time = 16ms, no-DMA = 0 */

    // it could fail...
    if (floppy_calibrate())
        return -1;
    return 0;
}

/**
 * Turn the floppy motor on or off.
 *
 * @param onoff on or off
 */
void floppy_motor(int onoff)
{
    if (onoff)
    {
        if (!floppy_motor_state)
        {
            // need to turn on
            port_byte_out(FLOPPY_BASE + FLOPPY_DOR, 0x1c);
            timer_sleep(50); // wait 500 ms = hopefully enough for modern drives
        }
        floppy_motor_state = FLOPPY_MOTOR_ON;
    }
    else
    {
        floppy_motor_state = FLOPPY_MOTOR_OFF;
    }
}

/**
 * Seek for a given cylinder, with a given head
 *
 * @param cyli cylinder number
 * @param head head number
 */
int floppy_seek(unsigned cyli, int head)
{
    unsigned i, st0, cyl = -1; // set to bogus cylinder

    floppy_motor(FLOPPY_MOTOR_ON);
    // try cylinders until the specified one is found
    for (i = 0; i < 10; i++)
    {
        // Attempt to position to given cylinder
        // 1st byte bit[1:0] = drive, bit[2] = head
        // 2nd byte is cylinder number
        floppy_write_cmd(CMD_SEEK);
        floppy_write_cmd(head << 2);
        floppy_write_cmd(cyli);

        wait_for_interrupt();
        floppy_check_interrupt((int *)&st0, (int *)&cyl);

        if (cyl == cyli)
        {
            floppy_motor(FLOPPY_MOTOR_OFF);
            return 0;
        }
    }

    print("floppy_seek: 10 retries exhausted\n", FLOPPY_PRINT_ATTRIBUTE);
    floppy_motor(FLOPPY_MOTOR_OFF);
    return -1;
}

/**
 * Used by floppy_dma_init and floppy_do_track to specify direction
 */
typedef enum
{
    floppy_dir_read = 1,
    floppy_dir_write = 2
} floppy_dir;

/**
 * Initialize floppy for Direct Memory Access
 *
 * @dir set to read or write mode
 */
static void floppy_dma_init(floppy_dir dir)
{
    union
    {
        unsigned char b[4]; // 4 bytes
        unsigned long l;    // 1 long = 32-bit
    } a, c;                 // address and count

    a.l = (unsigned)&floppy_dmabuf;
    c.l = (unsigned)FLOPPY_DMA_LENGTH - 1; // -1 because of DMA counting

    unsigned char mode;
    switch (dir)
    {
    // 01:0:0:01:10 = single/inc/no-auto/to-mem/chan2
    case floppy_dir_read:
        mode = 0x46;
        break;
    // 01:0:0:10:10 = single/inc/no-auto/from-mem/chan2
    case floppy_dir_write:
        mode = 0x4a;
        break;
    default:
        print("floppy_dma_init: invalid direction", FLOPPY_PRINT_ATTRIBUTE);
        return; // not reached, please "mode user uninitialized"
    }

    port_byte_out(0x0a, 0x06); // mask chan 2

    port_byte_out(0x0c, 0xff);   // reset flip-flop
    port_byte_out(0x04, a.b[0]); //  - address low byte
    port_byte_out(0x04, a.b[1]); //  - address high byte

    port_byte_out(0x81, a.b[2]); // external page register

    port_byte_out(0x0c, 0xff);   // reset flip-flop
    port_byte_out(0x05, c.b[0]); //  - count low byte
    port_byte_out(0x05, c.b[1]); //  - count high byte

    port_byte_out(0x0b, mode); // set mode (see above)

    port_byte_out(0x0a, 0x02); // unmask chan 2
}

/**
 * This monster does full cylinder (both tracks) transfer to
 * the specified direction (since the difference is small).
 *
 * @param cyl cylinder number
 * @dir read or write mode
 */
int floppy_do_track(unsigned cyl, floppy_dir dir)
{
    // transfer command, set below
    unsigned char cmd;

    // Read is MT:MF:SK:0:0:1:1:0, write MT:MF:0:0:1:0:1
    // where MT = multitrack, MF = MFM mode, SK = skip deleted
    //
    // Specify multitrack and MFM mode
    static const int flags = 0xC0;
    switch (dir)
    {
    case floppy_dir_read:
        cmd = CMD_READ_DATA | flags;
        break;
    case floppy_dir_write:
        cmd = CMD_WRITE_DATA | flags;
        break;
    default:
        print("floppy_do_track: invalid direction", FLOPPY_PRINT_ATTRIBUTE);
        return 0; // not reached, but pleases "cmd used uninitialized"
    }

    // seek both heads
    if (floppy_seek(cyl, 0))
        return -1;
    if (floppy_seek(cyl, 1))
        return -1;

    int i;
    for (i = 0; i < 20; i++)
    {
        floppy_motor(FLOPPY_MOTOR_ON);

        // init dma..
        floppy_dma_init(dir);

        timer_sleep(10); // give some time (100ms) to settle after the seeks

        floppy_write_cmd(cmd);  // set above for current direction
        floppy_write_cmd(0);    // 0:0:0:0:0:HD:US1:US0 = head and drive
        floppy_write_cmd(cyl);  // cylinder
        floppy_write_cmd(0);    // first head (should match with above)
        floppy_write_cmd(1);    // first sector, strangely counts from 1
        floppy_write_cmd(2);    // bytes/sector, 128*2^x (x=2 -> 512)
        floppy_write_cmd(18);   // number of tracks to operate on
        floppy_write_cmd(0x1b); // GAP3 length, 27 is default for 3.5"
        floppy_write_cmd(0xff); // data length (0xff if B/S != 0)

        wait_for_interrupt(); // don't SENSE_INTERRUPT here!

        // first read status information
        unsigned char st0, st1, st2, rcy, rhe, rse, bps;
        st0 = floppy_read_data();
        st1 = floppy_read_data();
        st2 = floppy_read_data();
        /*
         * These are cylinder/head/sector values, updated with some
         * rather bizarre logic, that I would like to understand.
         *
         */
        rcy = floppy_read_data();
        rhe = floppy_read_data();
        rse = floppy_read_data();
        // surpress unused parameter warnings
        (void)(rcy);
        (void)(rhe);
        (void)(rse);
        // bytes per sector, should be what we programmed in
        bps = floppy_read_data();

        floppy_motor(FLOPPY_MOTOR_OFF);
        return 0;
    }

    print("floppy_do_sector: 20 retries exhausted\n", FLOPPY_PRINT_ATTRIBUTE);
    floppy_motor(FLOPPY_MOTOR_OFF);
    return -1;
}

/**
 * Read a floppy track to the dma buffer
 *
 * @param base base address of the floppy controller (primary/secondary)
 * @param cyl cylinder number
 */
int floppy_read_track(unsigned cyl)
{
    return floppy_do_track(cyl, floppy_dir_read);
}

/**
 * Write a floppy track from the dma buffer
 *
 * @param base base address of the floppy controller (primary/secondary)
 * @param cyl cylinder number
 */
int floppy_write_track(unsigned cyl)
{
    return floppy_do_track(cyl, floppy_dir_write);
}

unsigned int floppy_controller_interrupts = 0;

/**
 * This function should be waiting for an interrupt to occurr, but since that
 * is weirdly broken, it instead waits for 500ms
 */
void wait_for_interrupt()
{
    timer_sleep(50);
}

/**
 * Write the buffer to the specified track
 *
 * @param index track index
 */
void floppy_write_buffer(unsigned int index)
{
    floppy_write_track(index);
}

/**
 * Read the specified track to the buffer
 *
 * @param index track index
 */
void floppy_read_buffer(unsigned int index)
{
    floppy_read_track(index);
}

/**
 * Clear the dma buffer
 */
void floppy_clear_buffer()
{
    memset((unsigned char *)floppy_dmabuf, 0, FLOPPY_DMA_LENGTH);
}

/**
 * Installs the floppy driver
 */
void floppy_install()
{
    memset((unsigned char *)floppy_dmabuf, 0, FLOPPY_DMA_LENGTH);
    floppy_detect_drives();
    floppy_reset(FLOPPY_BASE);
    print("Floppy reset\n", FLOPPY_PRINT_ATTRIBUTE);
    floppy_calibrate(FLOPPY_BASE);
    print("Floppy calibrated\n", FLOPPY_PRINT_ATTRIBUTE);
}