#include "floppy.h"

#include "stddef.h"

#include "kaos.h"
#include "panic.h"
#include "portio.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define BYTES_PER_SECTOR  512
#define SECTORS_PER_TRACK 18
#define NUMBER_OF_HEADS   2
#define BYTES_PER_TRACK   (BYTES_PER_SECTOR * SECTORS_PER_TRACK)

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

#define DOR_DRIVE_SEL           0x03
#define DOR_RESET               0x04
#define DOR_ENABLE_IRQS_AND_DMA 0x08
#define DOR_MOTOR_A             0x10
#define DOR_MOTOR_B             0x20
#define DOR_MOTOR_C             0x40
#define DOR_MOTOR_D             0x80

#define MSR_ACTA 0x01
#define MSR_ACTB 0x02
#define MSR_ACTC 0x04
#define MSR_ACTD 0x08
#define MSR_CB   0x10
#define MSR_NDMA 0x20
#define MSR_DIO  0x40
#define MSR_RQM  0x80

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
    const uint8_t mask  = MSR_RQM | MSR_DIO;
    const uint8_t value = MSR_RQM;
    for (int i = 0; i < N_TRIES; i++) {
        if ((portio_inb(MAIN_STATUS_REGISTER) & mask) == value) {
            return 0;
        }
    }
    return -1;
}

static int wait_for_read_ready()
{
    const uint8_t mask  = MSR_RQM | MSR_DIO;
    const uint8_t value = MSR_RQM | MSR_DIO;
    for (int i = 0; i < N_TRIES; i++) {
        if ((portio_inb(MAIN_STATUS_REGISTER) & mask) == value) {
            return 0;
        }
    }
    return -1;
}

static inline int command_begin(uint8_t cmd, const uint8_t *src, int src_size)
{
    // verify write possible
    TRY(wait_for_write_ready());

    portio_outb(DATA_FIFO, cmd);

    // now send parameters
    for (int i = 0; i < src_size; i++) {
        TRY(wait_for_write_ready());
        portio_outb(DATA_FIFO, src[i]);
    }

    return 0;
}

static inline int command_read_results(uint8_t *dst, int dst_size)
{
    // wait for read possible
    for (int i = 0; i < dst_size; i++) {

        TRY(wait_for_read_ready());

        // read result
        dst[i] = portio_inb(DATA_FIFO);
    }

    return 0;
}

static int do_noexec_command(uint8_t cmd, uint8_t *dst, int dst_size, const uint8_t *src, int src_size)
{
    TRY(command_begin(cmd, src, src_size));

    // assert no execution phase
    uint8_t msr = portio_inb(MAIN_STATUS_REGISTER);
    if (msr & MSR_NDMA) {
        // do execution phase
        KAOS_PANIC("command execution phase not implemented");
    }

    return command_read_results(dst, dst_size);
}

static int floppy_configure(void)
{
    bool    implied_seek_enabled       = TRUE;
    bool    fifo_disable               = FALSE;
    bool    drive_polling_mode_disable = TRUE;
    uint8_t threshold_value            = 8;
    uint8_t precompensation            = 0;

    uint8_t args[3] = {
        0,
        (implied_seek_enabled << 6) | (fifo_disable << 5) | (drive_polling_mode_disable << 4) | (threshold_value - 1),
        precompensation};
    return do_noexec_command(CMD_CONFIGURE, NULL, 0, args, sizeof(args));
}

static int floppy_lock(bool lock)
{
    uint8_t result;
    int     ret = do_noexec_command(CMD_LOCK | (lock << 7), &result, 1, NULL, 0);
    return ret;
}

static int drive_select(bool NDMA, int SRT_ms, int HLT_ms, int HUT_ms)
{

    portio_outb(CONFIGURATION_CONTROL_REGISTER, 0x0); // 500kbps - adequate for 3.5" 1.44 MB
    int data_rate = 500000;

    uint8_t SRT_value       = 16 - (SRT_ms * data_rate) / 500000;
    uint8_t HLT_value       = (HLT_ms * data_rate) / 1000000;
    uint8_t HUT_value       = (HUT_ms * data_rate) / 8000000;
    uint8_t specify_args[2] = {
        (SRT_value << 4) | HUT_value,
        (HLT_value << 1) | NDMA,
    };

    TRY(do_noexec_command(CMD_SPECIFY, NULL, 0, specify_args, sizeof(specify_args)));

    // select drive 0, and turn on its motor
    portio_outb(DIGITAL_OUTPUT_REGISTER, 0x14);

    kaos_puts("[floppy] drive select for drive 0 s\n");

    return 0;
}

static int floppy_reset(void)
{
    // method 1
    uint8_t dor = portio_inb(DIGITAL_OUTPUT_REGISTER);
    portio_outb(DIGITAL_OUTPUT_REGISTER, 0);
    // wait 4 us
    portio_wait();
    portio_wait();
    portio_wait();
    portio_wait();

    // restore value
    portio_outb(DIGITAL_OUTPUT_REGISTER, dor);

    TRY(drive_select(TRUE, 8, 30, 24));

    kaos_puts("[floppy] Reset succeeded\n");

    return 0;
}

static int floppy_recalibrate(void)
{

    for (uint8_t drive_number = 0; drive_number < 4; drive_number++) {
        TRY(do_noexec_command(CMD_RECALIBRATE, NULL, 0, &drive_number, 1));

        uint8_t sense_result[2];
        do {
            TRY(do_noexec_command(CMD_SENSE_INTERRUPT, sense_result, 2, NULL, 0));
        } while (!(sense_result[0] & 0x20) || sense_result[1] != 0);
    }
    kaos_printf("[floppy] Recalibrated all drives\n");

    return 0;
}

int floppy_init()
{
    uint8_t dor = portio_inb(DIGITAL_OUTPUT_REGISTER);
    /* kaos_printf("[floppy] DOR: %b\n", dor); */

    kaos_puts("[floppy] clearing IRQ flag\n");
    portio_outb(DIGITAL_OUTPUT_REGISTER, dor & ~DOR_ENABLE_IRQS_AND_DMA);

    /* dor = portio_inb(DIGITAL_OUTPUT_REGISTER); */
    /* kaos_printf("[floppy] DOR: %b\n", dor); */

    uint8_t version;
    int     ret = do_noexec_command(CMD_VERSION, &version, 1, NULL, 0);
    if (ret) {
        kaos_puts("[floppy] VERSION command failed");
        return 1;
    }

    kaos_printf("[floppy] Version: %b\n", version);

    if (version != 0x90) {
        KAOS_PANIC("[floppy] Unexpected version - only 0x90 is supported!");
    }

    /* uint8_t regs[10]; */
    /* if (do_command(CMD_DUMPREG, regs, sizeof(regs), NULL, 0) == 0) { */
    /*     /\* FDC is_82072/82072A/82077/82078 *\/ */
    /*     kaos_puts("[floppy] DUMPREGS(10):\n"); */
    /*     for (int i = 0; i < sizeof(regs); i++) { */
    /*         kaos_printf("[floppy]   Reg%d = %b\n", i, regs[i]); */
    /*     } */
    /* } else if (do_command(CMD_DUMPREG, regs, 1, NULL, 0) == 0) { */
    /*     /\* FDC is an 8272A *\/ */
    /*     /\*  8272a/765 don't know DUMPREGS *\/ */
    /*     kaos_puts("[floppy] DUMPREGS(1):\n"); */
    /*     kaos_printf("[floppy]   Reg0 = %b\n", regs[0]); */
    /* } else { */
    /*     kaos_puts("[floppy] DUMPREG(10) and DUMPREG(1) command failed"); */
    /*     /\* return 1; *\/ */
    /* } */

    TRY(floppy_configure());

    TRY(floppy_lock(TRUE));

    TRY(floppy_reset());

    TRY(floppy_recalibrate());

    return 0;
}

static int floppy_read_track_chs(uint8_t *dst, int cylinder, int head, int start_offset, int n_bytes)
{
    int start_sector     = 1 + start_offset / BYTES_PER_SECTOR;
    int offset_in_sector = start_offset % BYTES_PER_SECTOR;

    kaos_printf("start_sector: %d\n", start_sector);
    kaos_printf("offset_in_sector: %d\n", offset_in_sector);

    int     drive_number = 0;
    uint8_t EOT          = SECTORS_PER_TRACK;
    uint8_t args[8]      = {
        (head & 0x3f) << 2 | (drive_number & 0x3),
        cylinder & 0xff,
        (head & 0x3f),
        start_sector & 0xff,
        2,
        EOT,
        0x1b,
        0xff,
    };

    TRY(command_begin(CMD_READ_DATA, args, sizeof(args)));

    int ibyte = 0;
    do {

        // assert no execution phase
        uint8_t msr = portio_inb(MAIN_STATUS_REGISTER);
        if ((msr & MSR_NDMA) == 0) {
            // do execution phase
            KAOS_PANIC("command execution phase expected");
        }

        while (!(portio_inb(MAIN_STATUS_REGISTER) & MSR_RQM)) {
            kaos_puts("Waiting for RQM\n");
        }

        /* kaos_puts("RQM set\n"); */

        uint8_t byte = portio_inb(DATA_FIFO);
        if (ibyte >= offset_in_sector) {
            *dst = byte;
            dst++;
            n_bytes--;
        }
        ibyte++;
    } while (n_bytes > 0);

    kaos_printf("\nRead %d bytes\n", ibyte);

    uint8_t results[7];
    TRY(command_read_results(results, sizeof(results)));
    kaos_printf("[floppy] read result:\n");
    kaos_printf("         st0:          %b\n", results[0]);
    kaos_printf("         st1:          %b\n", results[1]);
    kaos_printf("         st2:          %b\n", results[2]);
    kaos_printf("         cylinder num: %b\n", results[3]);
    kaos_printf("         end head:     %b\n", results[4]);
    kaos_printf("         end sector:   %b\n", results[5]);

    return 0;
}

int floppy_read(uint8_t *dst, int read_offset, int n_bytes_left)
{

    // for each track / cylinderto read
    // do read track (any number of sectors/bytes)
    do {
        int offset_in_track = read_offset % BYTES_PER_TRACK;
        int track           = read_offset / BYTES_PER_TRACK;
        int head            = track % NUMBER_OF_HEADS;
        int cylinder        = track / NUMBER_OF_HEADS;

        int bytes_left_in_track = BYTES_PER_TRACK - offset_in_track;
        int track_bytes         = MIN(bytes_left_in_track, n_bytes_left);

        kaos_printf("calling floppy_read_track_chs(dst, %d, %d, %d, %d)\n",
                    cylinder,
                    head,
                    offset_in_track,
                    track_bytes);
        TRY(floppy_read_track_chs(dst, cylinder, head, offset_in_track, track_bytes));

        read_offset += track_bytes;
        dst += track_bytes;
        n_bytes_left -= track_bytes;

    } while (n_bytes_left > 0);

    return 0;
}
