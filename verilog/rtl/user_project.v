`default_nettype none

module user_project (
`ifdef USE_POWER_PINS
    inout vccd1,
    inout vssd1,
`endif
    input wire wb_clk_i,
    input wire wb_rst_i,
    input wire wbs_stb_i,
    input wire wbs_cyc_i,
    input wire wbs_we_i,
    input wire [3:0] wbs_sel_i,
    input wire [31:0] wbs_dat_i,
    input wire [31:0] wbs_adr_i,
    output wire wbs_ack_o,
    output wire [31:0] wbs_dat_o,
    input wire [127:0] la_data_in,
    output wire [127:0] la_data_out,
    input wire [127:0] la_oenb,
    input wire [37:0] io_in,
    output wire [37:0] io_out,
    output wire [37:0] io_oeb,
    input wire [2:0] user_irq
);

  wire [3:0] peripheral_select;
  wire sram0_stb;
  wire sram1_stb;
  wire sram0_ack;
  wire sram1_ack;
  wire [31:0] sram0_dat_o;
  wire [31:0] sram1_dat_o;
  reg [31:0] mux_dat_o;

  assign peripheral_select = wbs_adr_i[19:16];
  assign sram0_stb = (peripheral_select == 4'h0) & wbs_stb_i;
  assign sram1_stb = (peripheral_select == 4'h1) & wbs_stb_i;

  always @(*) begin
    case (peripheral_select)
      4'h0: mux_dat_o = sram0_dat_o;
      4'h1: mux_dat_o = sram1_dat_o;
      default: mux_dat_o = 32'hDEADBEEF;
    endcase
  end

  assign wbs_dat_o = mux_dat_o;
  assign wbs_ack_o = sram0_ack | sram1_ack;

  assign la_data_out = 128'h0;
  assign io_out = 38'h0;
  assign io_oeb = {38{1'b1}};

  CF_SRAM_1024x32_wb_wrapper #(
    .WIDTH(12)
  ) sram0_inst (
`ifdef USE_POWER_PINS
    .VPWR(vccd1),
    .VGND(vssd1),
`endif
    .wb_clk_i(wb_clk_i),
    .wb_rst_i(wb_rst_i),
    .wbs_stb_i(sram0_stb),
    .wbs_cyc_i(wbs_cyc_i),
    .wbs_we_i(wbs_we_i),
    .wbs_sel_i(wbs_sel_i),
    .wbs_dat_i(wbs_dat_i),
    .wbs_adr_i(wbs_adr_i),
    .wbs_ack_o(sram0_ack),
    .wbs_dat_o(sram0_dat_o)
  );

  CF_SRAM_1024x32_wb_wrapper #(
    .WIDTH(12)
  ) sram1_inst (
`ifdef USE_POWER_PINS
    .VPWR(vccd1),
    .VGND(vssd1),
`endif
    .wb_clk_i(wb_clk_i),
    .wb_rst_i(wb_rst_i),
    .wbs_stb_i(sram1_stb),
    .wbs_cyc_i(wbs_cyc_i),
    .wbs_we_i(wbs_we_i),
    .wbs_sel_i(wbs_sel_i),
    .wbs_dat_i(wbs_dat_i),
    .wbs_adr_i(wbs_adr_i),
    .wbs_ack_o(sram1_ack),
    .wbs_dat_o(sram1_dat_o)
  );

endmodule

`default_nettype wire
