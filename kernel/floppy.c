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

#define NULL 0

#define N_TRIES 1000

#define TRY(expr)       \
    do {                \
        int res = expr; \
        if (res) {      \
            return res; \
        }               \
    } while (0)

static int wait_for_write_ready()
{
    const uint8_t mask  = MSR_RQM_BITMASK | MSR_DIO_BITMASK;
    const uint8_t value = MSR_RQM_BITMASK;
    for (int i = 0; i < N_TRIES; i++) {
        if ((portio_inb(MAIN_STATUS_REGISTER) & mask) == value) {
            return 0;
        }
    }
    return -1;
}

static int wait_for_read_ready()
{
    const uint8_t mask  = MSR_RQM_BITMASK | MSR_DIO_BITMASK;
    const uint8_t value = MSR_RQM_BITMASK | MSR_DIO_BITMASK;
    for (int i = 0; i < N_TRIES; i++) {
        if ((portio_inb(MAIN_STATUS_REGISTER) & mask) == value) {
            return 0;
        }
    }
    return -1;
}

static int do_command(uint8_t cmd, uint8_t *dst, int dst_size, const uint8_t *src, int src_size)
{
    // verify write possible
    TRY(wait_for_write_ready());

    portio_outb(DATA_FIFO, cmd);

    // now send parameters
    for (int i = 0; i < src_size; i++) {
        TRY(wait_for_write_ready());
        portio_outb(DATA_FIFO, src[i]);
    }

    // execution phase or result phase?
    uint8_t msr = portio_inb(MAIN_STATUS_REGISTER);
    if (msr & MSR_NDMA_BITMASK) {
        // do execution phase
        KAOS_PANIC("command execution phase not implemented");
    }

    // wait for read possible
    for (int i = 0; i < dst_size; i++) {

        TRY(wait_for_read_ready());

        // read result
        dst[i] = portio_inb(DATA_FIFO);
    }
    return 0;
}

int floppy_configure()
{
    bool implied_seek_enabled       = TRUE;
    bool fifo_disable               = FALSE;
    bool drive_polling_mode_disable = TRUE;
    uint8_t threshold_value         = 8;
    uint8_t precompensation         = 0;

    uint8_t args[3] = {
        0,
        (implied_seek_enabled << 6) | (fifo_disable << 5) | (drive_polling_mode_disable << 4) | (threshold_value - 1),
        precompensation};
    return do_command(CMD_CONFIGURE, NULL, 0, args, sizeof(args));
}

int floppy_lock(bool lock)
{
    uint8_t result;
    int ret = do_command(CMD_LOCK | (lock << 7), &result, 1, NULL, 0);
    return ret;
}

int floppy_reset()
{

    return 0;
}

int floppy_recalibrate()
{
    return 0;
}

int floppy_init()
{
    uint8_t msr = portio_inb(MAIN_STATUS_REGISTER);
    char buf[80];

    if (msr & MSR_RQM_BITMASK && !(msr & MSR_DIO_BITMASK)) {
        kaos_puts("[floppy] RQM set and DIO clear\n");
    }

    uint8_t version;
    int ret = do_command(CMD_VERSION, &version, 1, NULL, 0);
    if (ret) {
        kaos_puts("[floppy] VERSION command failed");
        return 1;
    }

    strfmt_snprintf(buf, sizeof(buf), "[floppy] Version: %b\n", version);
    kaos_puts(buf);

    if (version != 0x90) {
        KAOS_PANIC("[floppy] Unexpected version - only 0x90 is supported!");
    }

    uint8_t regs[10];
    if (do_command(CMD_DUMPREG, regs, sizeof(regs), NULL, 0) == 0) {
        /* FDC is_82072/82072A/82077/82078 */
        kaos_puts("[floppy] DUMPREGS(10):\n");
        for (int i = 0; i < sizeof(regs); i++) {
            strfmt_snprintf(buf, sizeof(buf), "[floppy]   Reg%d = %b\n", i, regs[i]);
            kaos_puts(buf);
        }
    } else if (do_command(CMD_DUMPREG, regs, 1, NULL, 0) == 0) {
        /* FDC is an 8272A */
        /*  8272a/765 don't know DUMPREGS */
        kaos_puts("[floppy] DUMPREGS(1):\n");
        strfmt_snprintf(buf, sizeof(buf), "[floppy]   Reg0 = %b\n", regs[0]);
        kaos_puts(buf);
    } else {
        kaos_puts("[floppy] DUMPREG(10) and DUMPREG(1) command failed");
        /* return 1; */
    }

    TRY(floppy_configure());

    TRY(floppy_lock(TRUE));

    TRY(floppy_reset());

    TRY(floppy_recalibrate());

    return 0;
}
