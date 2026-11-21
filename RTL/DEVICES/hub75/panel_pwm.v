module panel_pwm # (
    parameter COLOR_BITS = 4
)(
    input  wire [(3*COLOR_BITS)-1:0] pix_top,
    input  wire [(3*COLOR_BITS)-1:0] pix_bottom,
    input  wire [COLOR_BITS-1:0] pwm_level,
    output wire r0, g0, b0,
    output wire r1, g1, b1
);

    wire [COLOR_BITS-1:0] top_red    = pix_top[(3*COLOR_BITS)-1:2*COLOR_BITS];
    wire [COLOR_BITS-1:0] top_green  = pix_top[(2*COLOR_BITS)-1:COLOR_BITS];
    wire [COLOR_BITS-1:0] top_blue   = pix_top[COLOR_BITS - 1 : 0];

    wire [COLOR_BITS-1:0] bottom_red    = pix_bottom[(3*COLOR_BITS)-1:2*COLOR_BITS];
    wire [COLOR_BITS-1:0] bottom_green  = pix_bottom[(2*COLOR_BITS)-1:COLOR_BITS];
    wire [COLOR_BITS-1:0] bottom_blue   = pix_bottom[COLOR_BITS - 1 : 0];

    assign r0 = (top_red > pwm_level);
    assign g0 = (top_green > pwm_level);
    assign b0 = (top_blue > pwm_level);
    assign r1 = (bottom_red > pwm_level);
    assign g1 = (bottom_green > pwm_level);
    assign b1 = (bottom_blue > pwm_level);

endmodule
