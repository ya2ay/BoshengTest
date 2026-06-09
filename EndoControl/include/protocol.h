#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>

namespace Proxinse {
    namespace CSR {
        namespace CCU {
            extern const std::string heartbeat;
            extern const std::string version_firmware;
            extern const std::string version_ccu;
            extern const std::string version_ui;
            extern const std::string button;
            extern const std::string shutdown;
            extern const std::string parameter;
            extern const std::string query_parameter;
            extern const std::string status;
            extern const std::string button_pressed;
            extern const std::string cameara_head_SN;
        }
    }

    namespace SZR {
        namespace OSD {
            extern const std::string image;
            extern const std::string rectangle;
            extern const std::string line;
            extern const std::string text_color;
            extern const std::string text;
        }
    }

    namespace Parameters {
        extern const std::string brightness;
        extern const std::string ir_gain;
        extern const std::string ir_saturation;
        extern const std::string ir_sensitivity;
        extern const std::string transparencyfunc;
        extern const std::string contrast;
        extern const std::string hue;
        extern const std::string hue_r;
        extern const std::string hue_g;
        extern const std::string hue_b;
        extern const std::string saturation;
        extern const std::string color_temperature;
        extern const std::string sharp;
        extern const std::string denoise;
        extern const std::string enhance;
        extern const std::string dehaze;
        extern const std::string light;
        extern const std::string light_ir;
        extern const std::string ThreeDTo2D;
        extern const std::string freeze;
        extern const std::string screen_shot;
        extern const std::string auto_white_balance;
        extern const std::string record;
        extern const std::string record_quality;
        extern const std::string record_encode_type;
        extern const std::string record_format;
        extern const std::string record_bit_rate;
        extern const std::string record_file_limit;
        extern const std::string record_audio;
        extern const std::string record_device;
        extern const std::string record_audio_volume;
        extern const std::string mirror;
        extern const std::string zoom;
        extern const std::string zoom_in;
        extern const std::string zoom_out;
        extern const std::string color_bar;
        extern const std::string gray_chart;
        extern const std::string flip_horizontal;
        extern const std::string flip_vertical;
        extern const std::string rotate_180;
        extern const std::string shutter;
        extern const std::string iso;
        extern const std::string shutter_range;
        extern const std::string iso_range;
        extern const std::string auto_gain_target;
        extern const std::string stereopsis;
        extern const std::string auto_rotation;
        extern const std::string stabilization;
        extern const std::string image_format_3d;
        extern const std::string record_format_3d;
        extern const std::string network_format;
        extern const std::string record_channel;

        extern const std::string capture_black_image;
        extern const std::string capture_stereo_image;
        extern const std::string vidcap;
        extern const std::string do_auto_gain;
        extern const std::string auto_gain_base_ev;
        extern const std::string stereo_depth;
        extern const std::string stereo_calc_depth;
        extern const std::string shift1;
        extern const std::string shift2;
        extern const std::string mipi_gain_offset;
        extern const std::string ccm;
        extern const std::string show_pip;
        extern const std::string pip_position;
        extern const std::string pip_rectangle;
        extern const std::string add_osd;
        extern const std::string clear_osd;
        extern const std::string show_osd;
        extern const std::string textOnFrozenScreen;
        extern const std::string textNoCameraData;
        extern const std::string primary_screen;
        extern const std::string secondary_screen;
        extern const std::string screen_output_format;
        extern const std::string query_screen;
        extern const std::string show_hint_on_screen;
        extern const std::string display_fps;
        extern const std::string demo;
        extern const std::string write_register;
        extern const std::string read_register;
        extern const std::string query_version;
        extern const std::string reboot;
        extern const std::string rebootFirmware;
        extern const std::string rebootFpga;
        extern const std::string debug_message;
        extern const std::string diagnostics;
        extern const std::string exitSystem;
        extern const std::string led_remind;
        extern const std::string led_warning;
        extern const std::string textWhiteBalance;
        extern const std::string multi_lang_fluotext;
        extern const std::string wb_state;
        extern const std::string fluo_mode;
        extern const std::string fluo_display_mode;
        extern const std::string fluo_single_window;
        extern const std::string fluo_univariatecolor;
        extern const std::string fluo_colormap;
        extern const std::string fluo_colorful;
        extern const std::string operation_info;
        extern const std::string osd_custom_info;
        extern const std::string load_camerahead_data;
        extern const std::string color_gamut;
        extern const std::string record_resolution;
        extern const std::string lock_time;
        extern const std::string trig_delay;
        extern const std::string trig_pwm_duty;
        extern const std::string light_wf_interleaved;
        extern const std::string set_exposure;
        extern const std::string temperatue_state;
        extern const std::string mcu_firmware_file;
        extern const std::string mems_mode;
        extern const std::string mems_rotate180_angle;
        extern const std::string mems_still_angle;
        extern const std::string license_expired;
        extern const std::string license_expired_text;
        extern const std::string enable_fpga_tp;
        extern const std::string set_fpga_tp_data;
        extern const std::string hdmi_self_test;
        extern const std::string live_video;
        extern const std::string set_sensor_offset;
        extern const std::string pip_enable;
        extern const std::string exposure_mode;
        extern const std::string write_cameraDb;
        extern const std::string agc_lock_coef;
        extern const std::string agc_toolseg_thresh;
        extern const std::string agc_y_target;
        extern const std::string agc_upper_bound;
        extern const std::string upgrade_fpga;
        extern const std::string upgrade_hdmi_cap;
        extern const std::string use_screen_cap;
        extern const std::string close_serial_port;
        extern const std::string display_proxinse_logo;
        extern const std::string colorbar_changed;
        extern const std::string get_sensor_offset;
        extern const std::string toolSeg_state;
        extern const std::string fov;
        extern const std::string demoire;
    }
}

#endif // PROTOCOL_H
