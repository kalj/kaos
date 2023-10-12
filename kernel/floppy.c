#include "floppy.h"

#include "kaos.h"
#include "panic.h"
#include "portio.h"
#include "strfmt.h"

#define FLOPPY_BASE                    0x3f0
#define STATUS_REGISTER_A              (FLOPPY_BASE + 0) // read-only
#define STATUS_REGISTER_B              (FLOPPY_BASE + 1) // read-only
#define DIGITAL_OUTPUT_REGISTER        (FLOPPY_BASE + 2)
#define TAPE_DRIVE_REGISTER            (FLOPPY_BASE + 3)
#define MAIN_STATUS_REGISTER           (FLOPPY_BASE + 4) // read-only
#define DATARATE_SELECT_REGISTER       (FLOPPY_BASE + 4) // write-only
#define DATA_FIFO                      (FLOPPY_BASE + 5)
#define DIGITAL_INPUT_REGISTER         (FLOPPY_BASE + 7) // read-only
#define CONFIGURATION_CONTROL_REGISTER (FLOPPY_BASE + 7) // write-only

#define MSR_ACTA_BITMASK 0x01
#define MSR_ACTB_BITMASK 0x02
#define MSR_ACTC_BITMASK 0x04
#define MSR_ACTD_BITMASK 0x08
#define MSR_CB_BITMASK   0x10
#define MSR_NDMA_BITMASK 0x20
#define MSR_DIO_BITMASK  0x40
#define MSR_RQM_BITMASK  0x80

#define CMD_READ_TRACK         2 // generates IRQ6
#define CMD_SPECIFY            3 // * set drive parameters
#define CMD_SENSE_DRIVE_STATUS 4
#define CMD_WRITE_DATA         5 // * write to the disk
#define CMD_READ_DATA          6 // * read from the disk
#define CMD_RECALIBRATE        7 // * seek to cylinder 0
#define CMD_SENSE_INTERRUPT    8 // * ack IRQ6, get status of last command
#define CMD_WRITE_DELETED_DATA 9
#define CMD_READ_ID            10 // generates IRQ6
#define CMD_READ_DELETED_DATA  12
#define CMD_FORMAT_TRACK       13 // *
#define CMD_DUMPREG            14
#define CMD_SEEK               15 // * seek both heads to cylinder X
#define CMD_VERSION            16 // * used during initialization, once
#define CMD_SCAN_EQUAL         17
#define CMD_PERPENDICULAR_MODE 18 // * used during initialization, once, maybe
#define CMD_CONFIGURE          19 // * set controller parameters
#define CMD_LOCK               20 // * protect controller params from a reset
#define CMD_VERIFY             22
#define CMD_SCAN_LOW_OR_EQUAL  25
#define CMD_SCAN_HIGH_OR_EQUAL 29

static void do_command(uint8_t cmd)
{
    uint8_t msr = portio_inb(MAIN_STATUS_REGISTER);
    if (!(msr & MSR_RQM_BITMASK) || msr & MSR_DIO_BITMASK) {
        // RQM not set and/or DIO set, reset is needed
        return;
    }

    portio_outb(DATA_FIFO, cmd);

    // now send parameters

    /* do { */
    /*     msr = portio_inb(MAIN_STATUS_REGISTER); */
    /* } */
    /* /\* while(!(msr & MSR_RQM_BITMASK)); *\/ */
    /* while(!(msr & MSR_RQM_BITMASK) || msr & MSR_DIO_BITMASK); */

    /* if(msr & MSR_DIO_BITMASK) */
    /* { */
    /*     KAOS_PANIC("unexpectedly DIO set, bailing"); */
    /* } */

    // execution phase or result phase?

    msr = portio_inb(MAIN_STATUS_REGISTER);
    if (msr & MSR_NDMA_BITMASK) {
        // do execution phase
        KAOS_PANIC("command execution phase not implemented");
    }

    do {
        msr = portio_inb(MAIN_STATUS_REGISTER);
    } while (!(msr & MSR_RQM_BITMASK) || !(msr & MSR_DIO_BITMASK));

    kaos_puts("At end of do_command\n");
}

void floppy_init()
{
    uint8_t msr = portio_inb(MAIN_STATUS_REGISTER);
    kaos_puts("MSR: ");
    char buf[80];
    strfmt_u8_hex(buf, 80, msr);
    kaos_puts(buf);
    kaos_puts("\n");

    if (msr & MSR_RQM_BITMASK && !(msr & MSR_DIO_BITMASK)) {
        kaos_puts("RQM set and DIO clear\n");
    }

    do_command(CMD_VERSION);
}
