#include "protocol.h"


namespace Proxinse {
    namespace CSR {
        namespace CCU {
            const std::string heartbeat = "heartbeat";
            const std::string version_firmware = "version_fw";
            const std::string version_ccu = "version_ccu";
            const std::string version_ui = "version_ui";
            const std::string button = "button";
            const std::string shutdown = "shutdown";
            const std::string parameter = "parameter";
            const std::string query_parameter = "query_parameter";
            const std::string status = "status";
            const std::string button_pressed = "button_pressed";
            const std::string cameara_head_SN = "cameara_head_SN";
        }

        namespace LED {
            const std::string heartbeat = "heartbeat";
        }
    }

    namespace SZR {
        namespace OSD {
            const std::string image      = "draw_image";
            const std::string rectangle  = "draw_rectangle";
            const std::string line       = "draw_line";
            const std::string text_color = "set_text_color";
            const std::string text       = "draw_text";
        }
    }

    namespace Parameters {
        const std::string brightness         = "brightness";
        const std::string ir_gain            = "ir_gain";
        const std::string ir_saturation      = "ir_saturation";
        const std::string ir_sensitivity     = "ir_sensitivity";
        const std::string transparencyfunc   = "transparencyfunc";
        const std::string contrast           = "contrast";
        const std::string hue                = "hue";
        const std::string hue_r              = "hue_r";
        const std::string hue_g              = "hue_g";
        const std::string hue_b              = "hue_b";
        const std::string saturation         = "saturation";
        const std::string color_temperature  = "color_temperature";
        const std::string sharp              = "sharp";
        const std::string denoise            = "denoise";
        const std::string enhance            = "enhance";
        const std::string dehaze             = "dehaze";
        const std::string light              = "light";
        const std::string light_ir           = "light_ir";
        const std::string ThreeDTo2D         = "ThreeDTo2D";
        const std::string freeze             = "freeze";
        const std::string screen_shot        = "screen_shot";
        const std::string auto_white_balance = "auto_white_balance";
        const std::string record             = "record";
        const std::string record_quality     = "record_quality";
        const std::string record_encode_type = "record_encode_type";
        const std::string record_format      = "record_format";
        const std::string record_bit_rate    = "record_bit_rate";
        const std::string record_file_limit  = "record_file_limit";
        const std::string record_audio       = "record_audio";
        const std::string record_device      = "record_device";
        const std::string record_audio_volume= "record_audio_volume";
        const std::string mirror             = "mirror";
        const std::string zoom               = "zoom";
        const std::string zoom_in            = "zoom_in";
        const std::string zoom_out           = "zoom_out";
        const std::string color_bar          = "color_bar";
        const std::string gray_chart         = "gray_chart";
        const std::string flip_horizontal    = "flip_horizontal";
        const std::string flip_vertical      = "flip_vertical";
        const std::string rotate_180         = "rotate_180";
        const std::string shutter            = "shutter";
        const std::string iso                = "iso";
        const std::string shutter_range      = "shutter_range";
        const std::string iso_range          = "iso_range";
        const std::string auto_gain_target   = "auto_gain_target";
        const std::string stereopsis         = "stereopsis";
        const std::string auto_rotation      = "auto_rotation";
        const std::string stabilization      = "stabilization";
        const std::string image_format_3d    = "image_format_3d";
        const std::string record_format_3d   = "record_format_3d";
        const std::string network_format     = "network_format";
        const std::string record_channel     = "record_channel";

        const std::string capture_black_image  = "capture_black_image";
        const std::string capture_stereo_image = "capture_stereo_image";
        const std::string vidcap               = "vidcap";
        const std::string do_auto_gain         = "do_auto_gain";
        const std::string auto_gain_base_ev    = "auto_gain_base_ev";
        const std::string stereo_depth         = "stereo_depth";
        const std::string stereo_calc_depth    = "stereo_calc_depth";
        const std::string shift1               = "shift1";
        const std::string shift2               = "shift2";
        const std::string mipi_gain_offset     = "mipi_gain_offset";
        const std::string ccm                  = "ccm";
        const std::string show_pip             = "show_pip";
        const std::string pip_position         = "pip_position";
        const std::string pip_rectangle        = "pip_rectangle";
        const std::string add_osd              = "add_osd";
        const std::string clear_osd            = "clear_osd";
        const std::string show_osd             = "show_osd";
        const std::string textOnFrozenScreen   = "textOnFrozenScreen";
        const std::string textNoCameraData     = "textNoCameraData";
        const std::string primary_screen       = "primary_screen";
        const std::string secondary_screen     = "secondary_screen";
        const std::string screen_output_format = "screen_output_format";
        const std::string query_screen         = "query_screen";
        const std::string show_hint_on_screen  = "show_hint_on_screen";
        const std::string display_fps          = "display_fps";
        const std::string demo                 = "demo";
        const std::string write_register       = "write_register";
        const std::string read_register        = "read_register";
        const std::string query_version        = "query_version";
        const std::string reboot               = "reboot";
        const std::string rebootFirmware       = "rebootFirmware";
        const std::string rebootFpga           = "rebootFpga";
        const std::string debug_message        = "debug_message";
        const std::string diagnostics          = "diagnostics";
        const std::string exitSystem           = "exitSystem";
        const std::string led_remind           = "led_remind";
        const std::string led_warning          = "led_warning";
        const std::string textWhiteBalance     = "textWhiteBalance";
        const std::string multi_lang_fluotext  = "multi_lang_fluotext";
        const std::string wb_state             = "wb_state";
        const std::string fluo_mode            = "fluo_mode";
        const std::string fluo_display_mode    = "fluo_display_mode";
        const std::string fluo_single_window   = "fluo_single_window";
        const std::string fluo_univariatecolor = "fluo_univariatecolor";
        const std::string fluo_colormap        = "fluo_colormap";
        const std::string fluo_colorful        = "fluo_colorful";
        const std::string operation_info       = "operation_info";
        const std::string osd_custom_info      = "osd_custom_info";
        const std::string load_camerahead_data = "load_camerahead_data";
        const std::string color_gamut          = "color_gamut";
        const std::string record_resolution    = "record_resolution";
        const std::string lock_time            = "lock_time";
        const std::string trig_delay           = "trig_delay";
        const std::string trig_pwm_duty        = "trig_pwm_duty";
        const std::string light_wf_interleaved = "light_wf_interleaved";
        const std::string set_exposure         = "set_exposure";
        const std::string temperatue_state     = "temperatue_state";
        const std::string mcu_firmware_file    = "mcu_firmware_file";
        const std::string mems_mode            = "mems_mode";
        const std::string mems_rotate180_angle = "mems_rotate180_angle";
        const std::string mems_still_angle     = "mems_still_angle";
        const std::string license_expired      = "license_expired";
        const std::string license_expired_text = "license_expired_text";
        const std::string enable_fpga_tp       = "enable_fpga_tp";
        const std::string set_fpga_tp_data     = "set_fpga_tp_data";
        const std::string hdmi_self_test       = "hdmi_self_test";
        const std::string live_video           = "live_video";
        const std::string set_sensor_offset    = "set_sensor_offset";
        const std::string pip_enable           = "pip_enable";
        const std::string exposure_mode        = "exposure_mode";
        const std::string write_cameraDb       = "write_cameraDb";
        const std::string agc_lock_coef        = "agc_lock_coef";
        const std::string agc_toolseg_thresh   = "agc_toolseg_thresh";
        const std::string agc_y_target         = "agc_y_target";
        const std::string agc_upper_bound      = "agc_upper_bound";
        const std::string upgrade_fpga         = "upgrade_fpga";
        const std::string upgrade_hdmi_cap     = "upgrade_hdmi_cap";
        const std::string use_screen_cap       = "use_screen_cap";
        const std::string close_serial_port    = "close_serial_port";
        const std::string display_proxinse_logo = "display_proxinse_logo";
        const std::string colorbar_changed     = "colorbar_changed";
        const std::string get_sensor_offset    = "get_sensor_offset";
        const std::string toolSeg_state        = "toolSeg_state";
        const std::string fov                  = "fov";
        const std::string demoire              = "demoire";
    }
}
