#include "irq.h"

#include "kaos_int.h"
#include "port_io.h"
#include "uart.h"

#define NUM_IDT_ARRAY_ENTRIES 256

typedef struct {
    uint16_t offset_1;       // offset bits 0..15
    uint16_t selector;       // a code segment selector in GDT or LDT
    uint8_t zero;            // unused, set to 0
    uint8_t type_attributes; // gate type, dpl, and p fields
    uint16_t offset_2;       // offset bits 16..31
} __attribute__((packed)) InterruptDescriptor;

// this should contain NUM_IDT_ARRAY_ENTRIES distinct entries, each of which
__attribute__((aligned(0x10))) static InterruptDescriptor idt[NUM_IDT_ARRAY_ENTRIES];

static void setup_interrupt(int num, void *irq, uint8_t type) {
    InterruptDescriptor *desc = &idt[num];
    desc->offset_1            = ((uint32_t)irq) & 0xFFFF;
    desc->selector            = 0x0008;
    desc->zero                = 0x0;
    desc->type_attributes     = type;
    desc->offset_2            = (((uint32_t)irq) >> 16) & 0xFFFF;
}

static struct {
    uint16_t size;
    uint32_t base;
} __attribute__((__packed__)) idtr = {.size = sizeof(idt) - 1, .base = (uint32_t)&idt};

#define STR_HELPER(x) #x
#define STR(x)        STR_HELPER(x)

#define CREATE_HANDLER(n)                                                      \
    static __attribute__((interrupt)) void stub_handler_##n(void *irq_frame) { \
        uart_puts("Hello from stub handler " STR(n) "\n");                     \
    }

CREATE_HANDLER(0)
CREATE_HANDLER(1)
CREATE_HANDLER(2)
CREATE_HANDLER(3)
CREATE_HANDLER(4)
CREATE_HANDLER(5)
CREATE_HANDLER(6)
CREATE_HANDLER(7)
CREATE_HANDLER(8)
CREATE_HANDLER(9)
CREATE_HANDLER(10)
CREATE_HANDLER(11)
CREATE_HANDLER(12)
CREATE_HANDLER(13)
CREATE_HANDLER(14)
CREATE_HANDLER(15)
CREATE_HANDLER(16)
CREATE_HANDLER(17)
CREATE_HANDLER(18)
CREATE_HANDLER(19)
CREATE_HANDLER(20)
CREATE_HANDLER(21)
CREATE_HANDLER(22)
CREATE_HANDLER(23)
CREATE_HANDLER(24)
CREATE_HANDLER(25)
CREATE_HANDLER(26)
CREATE_HANDLER(27)
CREATE_HANDLER(28)
CREATE_HANDLER(29)
CREATE_HANDLER(30)
CREATE_HANDLER(31)
CREATE_HANDLER(32)
CREATE_HANDLER(33)
CREATE_HANDLER(34)
CREATE_HANDLER(35)
CREATE_HANDLER(36)
CREATE_HANDLER(37)
CREATE_HANDLER(38)
CREATE_HANDLER(39)
CREATE_HANDLER(40)
CREATE_HANDLER(41)
CREATE_HANDLER(42)
CREATE_HANDLER(43)
CREATE_HANDLER(44)
CREATE_HANDLER(45)
CREATE_HANDLER(46)
CREATE_HANDLER(47)
CREATE_HANDLER(48)
CREATE_HANDLER(49)
CREATE_HANDLER(50)
CREATE_HANDLER(51)
CREATE_HANDLER(52)
CREATE_HANDLER(53)
CREATE_HANDLER(54)
CREATE_HANDLER(55)
CREATE_HANDLER(56)
CREATE_HANDLER(57)
CREATE_HANDLER(58)
CREATE_HANDLER(59)
CREATE_HANDLER(60)
CREATE_HANDLER(61)
CREATE_HANDLER(62)
CREATE_HANDLER(63)
CREATE_HANDLER(64)
CREATE_HANDLER(65)
CREATE_HANDLER(66)
CREATE_HANDLER(67)
CREATE_HANDLER(68)
CREATE_HANDLER(69)
CREATE_HANDLER(70)
CREATE_HANDLER(71)
CREATE_HANDLER(72)
CREATE_HANDLER(73)
CREATE_HANDLER(74)
CREATE_HANDLER(75)
CREATE_HANDLER(76)
CREATE_HANDLER(77)
CREATE_HANDLER(78)
CREATE_HANDLER(79)
CREATE_HANDLER(80)
CREATE_HANDLER(81)
CREATE_HANDLER(82)
CREATE_HANDLER(83)
CREATE_HANDLER(84)
CREATE_HANDLER(85)
CREATE_HANDLER(86)
CREATE_HANDLER(87)
CREATE_HANDLER(88)
CREATE_HANDLER(89)
CREATE_HANDLER(90)
CREATE_HANDLER(91)
CREATE_HANDLER(92)
CREATE_HANDLER(93)
CREATE_HANDLER(94)
CREATE_HANDLER(95)
CREATE_HANDLER(96)
CREATE_HANDLER(97)
CREATE_HANDLER(98)
CREATE_HANDLER(99)
CREATE_HANDLER(100)
CREATE_HANDLER(101)
CREATE_HANDLER(102)
CREATE_HANDLER(103)
CREATE_HANDLER(104)
CREATE_HANDLER(105)
CREATE_HANDLER(106)
CREATE_HANDLER(107)
CREATE_HANDLER(108)
CREATE_HANDLER(109)
CREATE_HANDLER(110)
CREATE_HANDLER(111)
CREATE_HANDLER(112)
CREATE_HANDLER(113)
CREATE_HANDLER(114)
CREATE_HANDLER(115)
CREATE_HANDLER(116)
CREATE_HANDLER(117)
CREATE_HANDLER(118)
CREATE_HANDLER(119)
CREATE_HANDLER(120)
CREATE_HANDLER(121)
CREATE_HANDLER(122)
CREATE_HANDLER(123)
CREATE_HANDLER(124)
CREATE_HANDLER(125)
CREATE_HANDLER(126)
CREATE_HANDLER(127)
CREATE_HANDLER(128)
CREATE_HANDLER(129)
CREATE_HANDLER(130)
CREATE_HANDLER(131)
CREATE_HANDLER(132)
CREATE_HANDLER(133)
CREATE_HANDLER(134)
CREATE_HANDLER(135)
CREATE_HANDLER(136)
CREATE_HANDLER(137)
CREATE_HANDLER(138)
CREATE_HANDLER(139)
CREATE_HANDLER(140)
CREATE_HANDLER(141)
CREATE_HANDLER(142)
CREATE_HANDLER(143)
CREATE_HANDLER(144)
CREATE_HANDLER(145)
CREATE_HANDLER(146)
CREATE_HANDLER(147)
CREATE_HANDLER(148)
CREATE_HANDLER(149)
CREATE_HANDLER(150)
CREATE_HANDLER(151)
CREATE_HANDLER(152)
CREATE_HANDLER(153)
CREATE_HANDLER(154)
CREATE_HANDLER(155)
CREATE_HANDLER(156)
CREATE_HANDLER(157)
CREATE_HANDLER(158)
CREATE_HANDLER(159)
CREATE_HANDLER(160)
CREATE_HANDLER(161)
CREATE_HANDLER(162)
CREATE_HANDLER(163)
CREATE_HANDLER(164)
CREATE_HANDLER(165)
CREATE_HANDLER(166)
CREATE_HANDLER(167)
CREATE_HANDLER(168)
CREATE_HANDLER(169)
CREATE_HANDLER(170)
CREATE_HANDLER(171)
CREATE_HANDLER(172)
CREATE_HANDLER(173)
CREATE_HANDLER(174)
CREATE_HANDLER(175)
CREATE_HANDLER(176)
CREATE_HANDLER(177)
CREATE_HANDLER(178)
CREATE_HANDLER(179)
CREATE_HANDLER(180)
CREATE_HANDLER(181)
CREATE_HANDLER(182)
CREATE_HANDLER(183)
CREATE_HANDLER(184)
CREATE_HANDLER(185)
CREATE_HANDLER(186)
CREATE_HANDLER(187)
CREATE_HANDLER(188)
CREATE_HANDLER(189)
CREATE_HANDLER(190)
CREATE_HANDLER(191)
CREATE_HANDLER(192)
CREATE_HANDLER(193)
CREATE_HANDLER(194)
CREATE_HANDLER(195)
CREATE_HANDLER(196)
CREATE_HANDLER(197)
CREATE_HANDLER(198)
CREATE_HANDLER(199)
CREATE_HANDLER(200)
CREATE_HANDLER(201)
CREATE_HANDLER(202)
CREATE_HANDLER(203)
CREATE_HANDLER(204)
CREATE_HANDLER(205)
CREATE_HANDLER(206)
CREATE_HANDLER(207)
CREATE_HANDLER(208)
CREATE_HANDLER(209)
CREATE_HANDLER(210)
CREATE_HANDLER(211)
CREATE_HANDLER(212)
CREATE_HANDLER(213)
CREATE_HANDLER(214)
CREATE_HANDLER(215)
CREATE_HANDLER(216)
CREATE_HANDLER(217)
CREATE_HANDLER(218)
CREATE_HANDLER(219)
CREATE_HANDLER(220)
CREATE_HANDLER(221)
CREATE_HANDLER(222)
CREATE_HANDLER(223)
CREATE_HANDLER(224)
CREATE_HANDLER(225)
CREATE_HANDLER(226)
CREATE_HANDLER(227)
CREATE_HANDLER(228)
CREATE_HANDLER(229)
CREATE_HANDLER(230)
CREATE_HANDLER(231)
CREATE_HANDLER(232)
CREATE_HANDLER(233)
CREATE_HANDLER(234)
CREATE_HANDLER(235)
CREATE_HANDLER(236)
CREATE_HANDLER(237)
CREATE_HANDLER(238)
CREATE_HANDLER(239)
CREATE_HANDLER(240)
CREATE_HANDLER(241)
CREATE_HANDLER(242)
CREATE_HANDLER(243)
CREATE_HANDLER(244)
CREATE_HANDLER(245)
CREATE_HANDLER(246)
CREATE_HANDLER(247)
CREATE_HANDLER(248)
CREATE_HANDLER(249)
CREATE_HANDLER(250)
CREATE_HANDLER(251)
CREATE_HANDLER(252)
CREATE_HANDLER(253)
CREATE_HANDLER(254)
CREATE_HANDLER(255)

static void (*stub_handlers[])(void *) = {
    stub_handler_0,   stub_handler_1,   stub_handler_2,   stub_handler_3,   stub_handler_4,   stub_handler_5,
    stub_handler_6,   stub_handler_7,   stub_handler_8,   stub_handler_9,   stub_handler_10,  stub_handler_11,
    stub_handler_12,  stub_handler_13,  stub_handler_14,  stub_handler_15,  stub_handler_16,  stub_handler_17,
    stub_handler_18,  stub_handler_19,  stub_handler_20,  stub_handler_21,  stub_handler_22,  stub_handler_23,
    stub_handler_24,  stub_handler_25,  stub_handler_26,  stub_handler_27,  stub_handler_28,  stub_handler_29,
    stub_handler_30,  stub_handler_31,  stub_handler_32,  stub_handler_33,  stub_handler_34,  stub_handler_35,
    stub_handler_36,  stub_handler_37,  stub_handler_38,  stub_handler_39,  stub_handler_40,  stub_handler_41,
    stub_handler_42,  stub_handler_43,  stub_handler_44,  stub_handler_45,  stub_handler_46,  stub_handler_47,
    stub_handler_48,  stub_handler_49,  stub_handler_50,  stub_handler_51,  stub_handler_52,  stub_handler_53,
    stub_handler_54,  stub_handler_55,  stub_handler_56,  stub_handler_57,  stub_handler_58,  stub_handler_59,
    stub_handler_60,  stub_handler_61,  stub_handler_62,  stub_handler_63,  stub_handler_64,  stub_handler_65,
    stub_handler_66,  stub_handler_67,  stub_handler_68,  stub_handler_69,  stub_handler_70,  stub_handler_71,
    stub_handler_72,  stub_handler_73,  stub_handler_74,  stub_handler_75,  stub_handler_76,  stub_handler_77,
    stub_handler_78,  stub_handler_79,  stub_handler_80,  stub_handler_81,  stub_handler_82,  stub_handler_83,
    stub_handler_84,  stub_handler_85,  stub_handler_86,  stub_handler_87,  stub_handler_88,  stub_handler_89,
    stub_handler_90,  stub_handler_91,  stub_handler_92,  stub_handler_93,  stub_handler_94,  stub_handler_95,
    stub_handler_96,  stub_handler_97,  stub_handler_98,  stub_handler_99,  stub_handler_100, stub_handler_101,
    stub_handler_102, stub_handler_103, stub_handler_104, stub_handler_105, stub_handler_106, stub_handler_107,
    stub_handler_108, stub_handler_109, stub_handler_110, stub_handler_111, stub_handler_112, stub_handler_113,
    stub_handler_114, stub_handler_115, stub_handler_116, stub_handler_117, stub_handler_118, stub_handler_119,
    stub_handler_120, stub_handler_121, stub_handler_122, stub_handler_123, stub_handler_124, stub_handler_125,
    stub_handler_126, stub_handler_127, stub_handler_128, stub_handler_129, stub_handler_130, stub_handler_131,
    stub_handler_132, stub_handler_133, stub_handler_134, stub_handler_135, stub_handler_136, stub_handler_137,
    stub_handler_138, stub_handler_139, stub_handler_140, stub_handler_141, stub_handler_142, stub_handler_143,
    stub_handler_144, stub_handler_145, stub_handler_146, stub_handler_147, stub_handler_148, stub_handler_149,
    stub_handler_150, stub_handler_151, stub_handler_152, stub_handler_153, stub_handler_154, stub_handler_155,
    stub_handler_156, stub_handler_157, stub_handler_158, stub_handler_159, stub_handler_160, stub_handler_161,
    stub_handler_162, stub_handler_163, stub_handler_164, stub_handler_165, stub_handler_166, stub_handler_167,
    stub_handler_168, stub_handler_169, stub_handler_170, stub_handler_171, stub_handler_172, stub_handler_173,
    stub_handler_174, stub_handler_175, stub_handler_176, stub_handler_177, stub_handler_178, stub_handler_179,
    stub_handler_180, stub_handler_181, stub_handler_182, stub_handler_183, stub_handler_184, stub_handler_185,
    stub_handler_186, stub_handler_187, stub_handler_188, stub_handler_189, stub_handler_190, stub_handler_191,
    stub_handler_192, stub_handler_193, stub_handler_194, stub_handler_195, stub_handler_196, stub_handler_197,
    stub_handler_198, stub_handler_199, stub_handler_200, stub_handler_201, stub_handler_202, stub_handler_203,
    stub_handler_204, stub_handler_205, stub_handler_206, stub_handler_207, stub_handler_208, stub_handler_209,
    stub_handler_210, stub_handler_211, stub_handler_212, stub_handler_213, stub_handler_214, stub_handler_215,
    stub_handler_216, stub_handler_217, stub_handler_218, stub_handler_219, stub_handler_220, stub_handler_221,
    stub_handler_222, stub_handler_223, stub_handler_224, stub_handler_225, stub_handler_226, stub_handler_227,
    stub_handler_228, stub_handler_229, stub_handler_230, stub_handler_231, stub_handler_232, stub_handler_233,
    stub_handler_234, stub_handler_235, stub_handler_236, stub_handler_237, stub_handler_238, stub_handler_239,
    stub_handler_240, stub_handler_241, stub_handler_242, stub_handler_243, stub_handler_244, stub_handler_245,
    stub_handler_246, stub_handler_247, stub_handler_248, stub_handler_249, stub_handler_250, stub_handler_251,
    stub_handler_252, stub_handler_253, stub_handler_254, stub_handler_255,
};

void irq_init() {

    for (int i = 0; i < NUM_IDT_ARRAY_ENTRIES; i++) {
        setup_interrupt(i, stub_handlers[i], 0x8E);
    }

    asm volatile("lidt %0" : : "m"(idtr));
    asm("sti");
}
