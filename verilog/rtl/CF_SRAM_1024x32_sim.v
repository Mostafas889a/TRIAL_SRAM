`default_nettype none

module CF_SRAM_1024x32 (
    output [31:0] DO,
    output ScanOutCC,
    input [31:0] DI,
    input [31:0] BEN,
    input [9:0] AD,
    input EN,
    input R_WB,
    input CLKin,
    input WLBI,
    input WLOFF,
    input TM,
    input SM,
    input ScanInCC,
    input ScanInDL,
    input ScanInDR,
    input vpwrac,
    input vpwrpc,
    input vgnd,
    input vpwrm,
    input vpwra,
    input vpwrp,
    input vpb,
    input vnb
);

    CF_SRAM_1024x32_stub inst (
        .DO(DO),
        .ScanOutCC(ScanOutCC),
        .DI(DI),
        .BEN(BEN),
        .AD(AD),
        .EN(EN),
        .R_WB(R_WB),
        .CLKin(CLKin),
        .WLBI(WLBI),
        .WLOFF(WLOFF),
        .TM(TM),
        .SM(SM),
        .ScanInCC(ScanInCC),
        .ScanInDL(ScanInDL),
        .ScanInDR(ScanInDR),
        .vpwrac(vpwrac),
        .vpwrpc(vpwrpc),
        .vgnd(vgnd),
        .vpwrm(vpwrm),
        .vpwra(vpwra),
        .vpwrp(vpwrp),
        .vpb(vpb),
        .vnb(vnb)
    );

endmodule

`default_nettype wire
